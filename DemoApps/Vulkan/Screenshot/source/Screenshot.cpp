/****************************************************************************************************************************************************
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the NXP. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include "Screenshot.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>
#include <FslUtil/Vulkan1_0/VUScopedMapMemory.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/Debug/Strings/VkFormat.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <limits>
#include <utility>

namespace Fsl
{
  namespace
  {
    // timeout in nanoseconds
    constexpr uint64_t FENCE_TIMEOUT = 1000000000ull * 10;

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      VulkanBasic::DemoAppVulkanSetup setup;
      // Needed for our screenshot method
      setup.DesiredSwapchainImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      return setup;
    }

    struct ImageRecord
    {
      RapidVulkan::Image TheImage;
      RapidVulkan::Memory TheMemory;
      VkDeviceSize AllocationSize{0};
      VkExtent2D Extent{};
    };


    ImageRecord PrepareDstImage(const Vulkan::VUDevice& device, const VkExtent2D& imageExtent, const VkFormat imageFormat)
    {
      // Create the linear dst image
      VkImageCreateInfo dstImageCreateInfo{};
      dstImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      dstImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      dstImageCreateInfo.format = imageFormat;
      dstImageCreateInfo.extent = {imageExtent.width, imageExtent.height, 1};
      dstImageCreateInfo.mipLevels = 1;
      dstImageCreateInfo.arrayLayers = 1;
      dstImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      dstImageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
      dstImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
      dstImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      RapidVulkan::Image dstImage(device.Get(), dstImageCreateInfo);

      // Get memory requirements
      VkMemoryRequirements memRequirements{};
      vkGetImageMemoryRequirements(device.Get(), dstImage.Get(), &memRequirements);

      // Allocate memory
      VkMemoryAllocateInfo dstImageMemoryAllocateInfo{};
      dstImageMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      dstImageMemoryAllocateInfo.allocationSize = memRequirements.size;
      dstImageMemoryAllocateInfo.memoryTypeIndex = device.GetPhysicalDevice().GetMemoryTypeIndex(
        memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      RapidVulkan::Memory dstImageMemory(device.Get(), dstImageMemoryAllocateInfo);

      // Bind the memory and image
      RapidVulkan::CheckError(vkBindImageMemory(device.Get(), dstImage.Get(), dstImageMemory.Get(), 0), "vkBindImageMemory", __FILE__, __LINE__);

      ImageRecord result;
      result.TheImage = std::move(dstImage);
      result.TheMemory = std::move(dstImageMemory);
      result.AllocationSize = dstImageMemoryAllocateInfo.allocationSize;
      result.Extent = imageExtent;
      return result;
    }

    Bitmap ExtractToBitmap(const ImageRecord& image, const VkFormat imageFormat)
    {
      const auto pixelFormat = Vulkan::VulkanConvert::ToPixelFormat(imageFormat);
      const VkDevice device = image.TheImage.GetDevice();

      // Get information about the image layout
      VkImageSubresource subResource{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0};
      VkSubresourceLayout subResourceLayout{};
      vkGetImageSubresourceLayout(device, image.TheImage.Get(), &subResource, &subResourceLayout);

      if (subResourceLayout.offset > image.AllocationSize)
      {
        throw NotSupportedException("invalid sub resource layout detected");
      }
      if (subResourceLayout.rowPitch > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("rowPitch size is unsupported");
      }

      // Do the extraction
      void* pImage = nullptr;
      // We use the scoped map class here since it will since its exception safe
      Vulkan::VUScopedMapMemory scopedMap(device, image.TheMemory.Get(), 0, VK_WHOLE_SIZE, 0, &pImage);

      assert(pImage != nullptr);
      assert(subResourceLayout.offset <= image.AllocationSize);

      const uint8_t* pImageMemory = static_cast<const uint8_t*>(pImage) + subResourceLayout.offset;
      const PxExtent2D extent(image.Extent.width, image.Extent.height);
      // Calculate the content size adjusting for the offset
      assert(subResourceLayout.rowPitch <= std::numeric_limits<uint32_t>::max());
      const auto stride = UncheckedNumericCast<uint32_t>(subResourceLayout.rowPitch);

      // Extract the bitmap data
      return Bitmap(pImageMemory, subResourceLayout.size, extent, pixelFormat, stride, BitmapOrigin::UpperLeft);
    }

    void TransitionImagesToTransferLayout(const VkCommandBuffer commandBuffer, const VkImage dstImage, const VkImage srcImage)
    {
      constexpr VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.image = dstImage;
        imageMemoryBarrier.subresourceRange = imageSubresourceRange;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }

      // Transition source image to transfer source layout
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        imageMemoryBarrier.image = srcImage;
        imageMemoryBarrier.subresourceRange = imageSubresourceRange;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }
    }

    void TransitionFromTransferLayout(const VkCommandBuffer commandBuffer, const VkImage dstImage, const VkImage srcImage)
    {
      constexpr VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
      // Transition source image back to source format
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.image = srcImage;
        imageMemoryBarrier.subresourceRange = imageSubresourceRange;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }

      // Transition destination image to a CPU readable layout
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.image = dstImage;
        imageMemoryBarrier.subresourceRange = imageSubresourceRange;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }
    }
  }

  Screenshot::Screenshot(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_graphicsService(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(std::dynamic_pointer_cast<Vulkan::NativeBatch2D>(m_graphicsService->GetNativeBatch2D()))
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    auto contentManager = GetContentManager();
    {
      Fsl::Bitmap bitmap;
      contentManager->Read(bitmap, "Test.png", PixelFormat::R8G8B8A8_UNORM);
      m_texture.Reset(m_graphicsService->GetNativeGraphics(), bitmap, Texture2DFilterHint::Nearest);
    }


    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    auto& uiFactory = *uiControlFactory;


    // Allocate the screenshot button
    m_btnScreenshot = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Screenshot");
    m_btnScreenshot->SetAlignmentX(UI::ItemAlignment::Center);

    // Create a label to write stuff into when a button is pressed
    m_label = uiFactory.CreateLabel("");
    m_label->SetAlignmentX(UI::ItemAlignment::Center);
    m_label->SetAlignmentY(UI::ItemAlignment::Center);

    // Create a horizontal stack layout and add the UI elements
    auto uiStack = std::make_shared<UI::StackLayout>(context);
    uiStack->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    uiStack->SetAlignmentX(UI::ItemAlignment::Center);
    uiStack->SetAlignmentY(UI::ItemAlignment::Far);
    uiStack->AddChild(m_label);
    uiStack->AddChild(m_btnScreenshot);

    auto bottomBar = uiFactory.CreateBottomBar();
    bottomBar->SetContent(uiStack);

    // Finally add everything to the window manager (to ensure its seen)
    auto windowManager = m_uiExtension->GetWindowManager();
    windowManager->Add(bottomBar);
  }


  void Screenshot::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_btnScreenshot)
    {
      m_screenshotRequested = true;
    }
  }


  void Screenshot::Update(const DemoTime& /*demoTime*/)
  {
  }


  void Screenshot::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        m_nativeBatch->Begin();
        m_nativeBatch->Draw(m_texture, Vector2(), Color::White());
        m_nativeBatch->End();

        // Calling this last allows the UI to draw on top of everything.
        m_uiExtension->Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  AppDrawResult Screenshot::TrySwapBuffers(const FrameInfo& frameInfo)
  {
    auto result = VulkanBasic::DemoAppVulkanBasic::TrySwapBuffers(frameInfo);
    if (result != AppDrawResult::Completed)
    {
      return result;
    }

    if (m_screenshotRequested)
    {
      m_screenshotRequested = false;
      // Bitmap bitmap;
      // m_graphicsService->Capture(bitmap, PixelFormat::R8G8B8A8_UINT);
      auto bitmap = TryCaptureScreenshot();
      if (bitmap.IsValid())
      {
        auto manager = GetPersistentDataManager();
        manager->Write("screenshot.png", bitmap);

        m_label->SetContent("Screenshot saved");
      }
      else
      {
        m_label->SetContent("Screenshot failed");
      }
    }
    return AppDrawResult::Completed;
  }


  VkRenderPass Screenshot::OnBuildResources(const VulkanBasic::BuildResourcesContext& /*context*/)
  {
    // Since we only draw using the NativeBatch we just create the most basic render pass that is compatible
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void Screenshot::OnFreeResources()
  {
    m_dependentResources = {};
  }


  Bitmap Screenshot::TryCaptureScreenshot()
  {
    // Check blit support for source and destination
    // Check if the device supports blitting from optimal images (the swapchain images are in optimal format)

    VulkanBasic::SwapchainInfo swapchainInfo;
    if (!TryGetSwapchainInfo(swapchainInfo))
    {
      FSLLOG3_WARNING("Failed to get swapchain info, capture cancelled");
      return Bitmap();
    }

    if (swapchainInfo.CurrentImage == VK_NULL_HANDLE)
    {
      FSLLOG3_WARNING("Invalid swapchain image, capture cancelled");
      return Bitmap();
    }
    if ((swapchainInfo.ImageUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) == 0u)
    {
      FSLLOG3_INFO("Swapchain did not support VK_IMAGE_USAGE_TRANSFER_SRC_BIT, capture cancelled");
      return Bitmap();
    }
    VkFormat srcImageFormat = swapchainInfo.ImageFormat;
    if (srcImageFormat == VK_FORMAT_UNDEFINED)
    {
      FSLLOG3_WARNING("Invalid swapchain image format, capture cancelled");
      return Bitmap();
    }

    auto srcPixelFormat = Vulkan::VulkanConvert::ToPixelFormat(srcImageFormat);
    if (PixelFormatUtil::IsCompressed(srcPixelFormat))
    {
      FSLLOG3_WARNING("srcPixelFormat is compressed, capture cancelled");
      return Bitmap();
    }

    VkFormat dstImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormatProperties formatProperties{};
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice.Device, srcImageFormat, &formatProperties);

    // Check if the device supports 'blit' from the source format
    const bool allowSrcBlit = (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) != 0u;
    // Check if the device supports 'blit' to the dst linear format
    const bool allowDstBlit = (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) != 0u;

    FSLLOG3_VERBOSE_IF(!allowSrcBlit, "Device does not support blitting from src format: {}", RapidVulkan::Debug::ToString(srcImageFormat));
    FSLLOG3_VERBOSE_IF(!allowDstBlit, "Device does not support blitting to dst format: {}", RapidVulkan::Debug::ToString(dstImageFormat));

    // if (allowSrcBlit && allowDstBlit)
    //{
    //  return TryCaptureScreenshotViaBlit(swapchainInfo, dstImageFormat);
    //}
    return TryCaptureScreenshotViaCopy(swapchainInfo);
  }


  Bitmap Screenshot::TryCaptureScreenshotViaBlit(const VulkanBasic::SwapchainInfo& /*swapchainInfo*/, const VkFormat /*dstImageFormat*/)
  {
    FSLLOG3_VERBOSE("Capturing via blit");

    // We wait for the device to be idle before we start capturing
    SafeWaitForDeviceIdle();

    FSLLOG3_ERROR("CaptureScreenshotViaBlit not implemented");
    return Bitmap();
  }


  // The swapchain image must have VK_IMAGE_USAGE_TRANSFER_SRC_BIT set for this to work
  Bitmap Screenshot::TryCaptureScreenshotViaCopy(const VulkanBasic::SwapchainInfo& swapchainInfo)
  {
    FSLLOG3_VERBOSE("Capturing via copy");

    assert(swapchainInfo.CurrentImage != VK_NULL_HANDLE);
    assert((swapchainInfo.ImageUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0u);
    assert(swapchainInfo.ImageFormat != VK_FORMAT_UNDEFINED);

    VkImage srcImage = swapchainInfo.CurrentImage;
    // We use the same format as the source as the copy command dont convert it for us
    VkFormat dstImageFormat = swapchainInfo.ImageFormat;

    // We wait for the device to be idle before we start capturing
    SafeWaitForDeviceIdle();

    // Prepare the image that we will 'transfer' the screenshot to
    auto dstImage = PrepareDstImage(m_device, swapchainInfo.ImageExtent, dstImageFormat);


    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = m_deviceQueue.QueueFamilyIndex;

    RapidVulkan::CommandPool commandPool(m_device.Get(), commandPoolCreateInfo);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool.Get();
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    RapidVulkan::CommandBuffer commandBuffer(m_device.Get(), commandBufferAllocateInfo);
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBuffer.Begin(commandBufferBeginInfo);
    {
      TransitionImagesToTransferLayout(commandBuffer.Get(), dstImage.TheImage.Get(), srcImage);

      // Copy the current content of the swapchain image to the dst buffer
      {
        VkImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.layerCount = 1;
        // imageCopyRegion.srcOffset;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        // imageCopyRegion.dstOffset;
        imageCopyRegion.extent = {swapchainInfo.ImageExtent.width, swapchainInfo.ImageExtent.height, 1};

        // Schedule copy
        vkCmdCopyImage(commandBuffer.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage.TheImage.Get(),
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
      }

      TransitionFromTransferLayout(commandBuffer.Get(), dstImage.TheImage.Get(), srcImage);
    }
    commandBuffer.End();

    RapidVulkan::Fence fence;
    fence.Reset(m_device.Get(), 0);

    // Submit to the queue
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer.GetPointer();

    m_deviceQueue.Submit(1, &submitInfo, fence.Get());

    // Wait for the fence to signal that command buffer has finished executing
    fence.WaitForFence(FENCE_TIMEOUT);

    // The dst image now contains the content
    return ExtractToBitmap(dstImage, dstImageFormat);
  }
}
