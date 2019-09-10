/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Bits/BitsUtil.hpp>
#include <FslDemoApp/Base/Overlay/DemoAppProfilerOverlay.hpp>
#include <FslDemoApp/Base/Service/Host/IHostInfo.hpp>
#include <FslDemoHost/Vulkan/Config/DemoAppHostConfigVulkan.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsSwapchainInfo.hpp>
#include <FslUtil/Vulkan1_0/Log/All.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <RapidVulkan/Debug/Strings/VkResult.hpp>
#include <RapidVulkan/Debug/Strings/VkImageUsageFlagBits.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

namespace Fsl
{
  using namespace Vulkan;

  namespace VulkanBasic
  {
    namespace
    {
      // The desired minimum image count for the swap buffers
      const uint32_t DESIRED_MIN_SWAP_BUFFER_COUNT = 2;

      const auto DEFAULT_TIMEOUT = std::numeric_limits<uint64_t>::max();


      DemoAppVulkanSetup ProcessDemoAppSetup(DemoAppVulkanSetup demoAppVulkanSetup, const uint32_t defaultMaxFramesInFlight)
      {
        if (demoAppVulkanSetup.MaxFramesInFlight == 0)
        {
          demoAppVulkanSetup.MaxFramesInFlight = defaultMaxFramesInFlight;
        }
        else if (demoAppVulkanSetup.MaxFramesInFlight > defaultMaxFramesInFlight)
        {
          demoAppVulkanSetup.MaxFramesInFlight = defaultMaxFramesInFlight;
          FSLLOG_WARNING("MaxFramesInFlight capped to: " << defaultMaxFramesInFlight);
        }
        return demoAppVulkanSetup;
      }

      VkImageUsageFlags FilterUnsupportedImageUsageFlags(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface,
                                                         const VkImageUsageFlags flags)
      {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));

        VkImageUsageFlags srcFlags = flags;
        VkImageUsageFlags supportedFlags = 0;
        int bitIndex = BitsUtil::IndexOf(srcFlags);
        while (bitIndex >= 0)
        {
          VkImageUsageFlags flag = static_cast<uint32_t>(1u) << bitIndex;
          if ((surfaceCapabilities.supportedUsageFlags & flag) != 0u)
          {
            supportedFlags |= flag;
          }
          else
          {
            FSLLOG_WARNING("Desired swapchain image usage flag 0x" << std::hex << flag << " ("
                                                                   << RapidVulkan::Debug::ToString(static_cast<VkImageUsageFlagBits>(flag))
                                                                   << ") is unsupported so it was ignored");
          }
          srcFlags &= ~flag;
          bitIndex = BitsUtil::IndexOf(srcFlags);
        }
        return supportedFlags;
      }


      Vulkan::SurfaceFormatInfo FindPreferredSurfaceInfo(VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface,
                                                         const std::deque<Vulkan::SurfaceFormatInfo>& preferredFormats)
      {
        auto supportedFormats = Vulkan::PhysicalDeviceKHRUtil::GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface);
        if (!preferredFormats.empty())
        {
          for (auto entry : preferredFormats)
          {
            for (auto candidate : supportedFormats)
            {
              if (entry.Format == candidate.format && entry.ColorSpace == candidate.colorSpace)
              {
                return entry;
              }
            }
          }
          FSLLOG2(LogType::Verbose, "None of the preferred surface formats found, using default");
        }
        if (supportedFormats.empty())
        {
          throw NotSupportedException("No surface formats found");
        }
        // Default to the first supported format (not optimal but at least its something)
        return Vulkan::SurfaceFormatInfo(supportedFormats[0].format, supportedFormats[0].colorSpace);
      }
    }

    DemoAppVulkanBasic::DemoAppVulkanBasic(const DemoAppConfig& demoAppConfig, const DemoAppVulkanSetup& demoAppVulkanSetup)
      : DemoAppVulkan(demoAppConfig)
      , m_appSetup(ProcessDemoAppSetup(demoAppVulkanSetup, GetDefaultMaxFramesInFlight()))
    {
      auto hostInfo = demoAppConfig.DemoServiceProvider.Get<IHostInfo>();
      auto hostConfig = hostInfo->GetConfig();
      if (hostConfig.StatOverlay)
      {
        m_demoAppProfilerOverlay.reset(new DemoAppProfilerOverlay(demoAppConfig.DemoServiceProvider));
      }
      auto demoHostConfig = hostInfo->TryGetAppHostConfig();
      if (!demoHostConfig)
      {
        throw NotSupportedException("Could not access the demo host config");
      }
      m_demoHostConfig = std::dynamic_pointer_cast<DemoAppHostConfigVulkan>(demoHostConfig);
      if (!m_demoHostConfig)
      {
        throw NotSupportedException("DemoHostConfig not of the expected type");
      }

      m_surfaceFormatInfo = FindPreferredSurfaceInfo(m_physicalDevice.Device, m_surface, m_demoHostConfig->GetPreferredSurfaceFormats());

      auto renderConfig = GetRenderConfig();

      m_resources.MainCommandPool.Reset(m_device.Get(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_deviceQueue.QueueFamilyIndex);
      m_resources.Frames = CreateFrameSyncObjects(m_device.Get(), renderConfig.MaxFramesInFlight);
    }


    DemoAppVulkanBasic::~DemoAppVulkanBasic()
    {
      if (m_dependentResources.Valid)
      {
        auto currentLifeCycleState = GetObjectLifeCycleState();
        try
        {
          FSLLOG_WARNING_IF(currentLifeCycleState != ObjectLifeCycle::Constructing,
                            "Resources still allocated during destruction, trying to free them");
          FreeResources();

          // Release the swapchain (we dont do this in FreeResources because BuildResources might reuse it).
          m_swapchain.Reset();
        }
        catch (const std::exception& ex)
        {
          FSLLOG_ERROR("Exception during FreeResources: " << ex.what());

          FSLLOG_ERROR("FreeResources failed to complete. Setting exit code to failure");

          // IMPROVEMENT: Be able to RequestExit and change exit code in one operation
          // We request a exit (this might silently return if a exit is already scheduled so the exit code will not be set)
          GetDemoAppControl()->RequestExit(EXIT_FAILURE);
          // Which is why we also call ChangeExitCode (beware this only changes the exit code if a non-default exit code has not been set)
          GetDemoAppControl()->ChangeExitCode(EXIT_FAILURE);
        }
      }
    }


    void DemoAppVulkanBasic::OnConstructed()
    {
      DemoAppVulkan::OnConstructed();
      if (!m_dependentResources.Valid)
      {
        BuildResources();
      }
    }


    void DemoAppVulkanBasic::OnDestroy()
    {
      // Calling FreeResources here instead of in the destructor ensures that 'virtual' methods are called as expected
      FreeResources();

      // Release the swapchain (we dont do this in FreeResources because BuildResources might reuse it).
      m_swapchain.Reset();

      // Finally call the OnDestroy of our inherited object
      DemoAppVulkan::OnDestroy();
    }


    RenderConfig DemoAppVulkanBasic::GetRenderConfig() const
    {
      assert(m_appSetup.MaxFramesInFlight != 0);
      assert(m_appSetup.MaxFramesInFlight <= GetDefaultMaxFramesInFlight());

      RenderConfig renderConfig;
      renderConfig.MaxFramesInFlight = m_appSetup.MaxFramesInFlight;
      return renderConfig;
    }


    AppDrawResult DemoAppVulkanBasic::TryPrepareDraw(const DemoTime& demoTime)
    {
      auto result = TryDoPrepareDraw(demoTime);
      SetAppState(result);

      if (result == AppDrawResult::Completed && m_dependentResources.NGScreenshotLink && m_swapchain.IsValid())
      {
        *m_dependentResources.NGScreenshotLink = Vulkan::NativeGraphicsSwapchainInfo(m_swapchain[m_dependentResources.CurrentSwapBufferIndex],
                                                                                     m_swapchain.GetImageFormat(), m_swapchain.GetImageUsageFlags());
      }
      else
      {
        *m_dependentResources.NGScreenshotLink = {};
      }
      return result;
    }

    void DemoAppVulkanBasic::_Draw(const DemoTime& demoTime)
    {
      if (m_nativeGraphicsService)
      {
        m_nativeGraphicsService->VulkanBeginFrame(m_dependentResources.CmdBuffers[m_dependentResources.CurrentSwapBufferIndex],
                                                  m_dependentResources.CurrentSwapBufferIndex);
        try
        {
          DemoAppVulkan::_Draw(demoTime);
        }
        catch (const std::exception& ex)
        {
          FSLLOG_ERROR("Draw threw exception: " << ex.what());
          m_nativeGraphicsService->VulkanEndFrame();
          throw;
        }
        m_nativeGraphicsService->VulkanEndFrame();
      }
      else
      {
        DemoAppVulkan::_Draw(demoTime);
      }
    }


    void DemoAppVulkanBasic::Resized(const Point2& size)
    {
      DemoAppVulkan::Resized(size);

      // Quick, dirty and slow resize support by destroying any screen size dependent Vulkan resources.
      if (m_appSetup.ActiveResizeStrategy == ResizeStrategy::RebuildResources)
      {
        if (IsResourcesAllocated())
        {
          TryRebuildResources();
        }
      }
    }


    void DemoAppVulkanBasic::Draw(const DemoTime& demoTime)
    {
      const auto currentSwapBufferIndex = m_dependentResources.CurrentSwapBufferIndex;
      // Allow the app to draw
      auto framebuffer = m_dependentResources.SwapchainRecords[currentSwapBufferIndex].Framebuffer.Get();

      // Possible improvement:
      // We could replace the VulkanDraw with a normal Draw call but that would just mean that all implementations would have
      // to get the commandBuffers, currentSwapBufferIndex and active framebuffer as the first thing they do.
      // We would need a helper method like this
      //    VulkanDrawContext& GetVulkanDrawContext() const
      // The context would be non copyable and contain the above mentioned things

      DrawContext drawContext(m_swapchain.GetImageExtent(), framebuffer, m_dependentResources.CurrentSwapBufferIndex, m_resources.CurrentFrame);
      VulkanDraw(demoTime, m_dependentResources.CmdBuffers, drawContext);

      const auto waitSemaphore = m_resources.Frames[m_resources.CurrentFrame].ImageAcquiredSemaphore.Get();
      const auto signalSemaphore = m_resources.Frames[m_resources.CurrentFrame].RenderingCompleteSemaphore.Get();
      const auto inFlightFence = m_resources.Frames[m_resources.CurrentFrame].InFlightFence.Get();

      // Submit the draw operations
      const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = &waitSemaphore;
      submitInfo.pWaitDstStageMask = &waitDstStageMask;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = m_dependentResources.CmdBuffers.GetPointer(currentSwapBufferIndex);
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = &signalSemaphore;

      m_deviceQueue.Submit(1, &submitInfo, inFlightFence);
    }


    AppDrawResult DemoAppVulkanBasic::TrySwapBuffers(const DemoTime& demoTime)
    {
      auto result = TryDoSwapBuffers(demoTime);
      SetAppState(result);
      return result;
    }


    void DemoAppVulkanBasic::AddSystemUI(VkCommandBuffer hCmdBuffer, const uint32_t cmdBufferIndex)
    {
      // We assume that we get called with the 'active' command buffer
      assert(m_dependentResources.Valid);
      assert(cmdBufferIndex == m_dependentResources.CurrentSwapBufferIndex);
      assert(hCmdBuffer == m_dependentResources.CmdBuffers[cmdBufferIndex]);

      if (m_demoAppProfilerOverlay)
      {
        m_demoAppProfilerOverlay->Draw(GetScreenResolution());
      }
    }


    void DemoAppVulkanBasic::BuildResources()
    {
      if (m_dependentResources.Valid)
      {
        throw UsageErrorException("Resources are still allocated, call FreeResources first");
      }
      FSLLOG2(LogType::Verbose, "DemoAppVulkanBasic::BuildResources()");

      try
      {
        // We do this right away to ensure we can call 'FreeResources' if something goes wrong
        m_dependentResources.Valid = true;

        FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::BuildResources(): Creating swapchain");
        // m_launchOptions.ScreenshotsEnabled

        auto desiredSwapchainImageUsageFlags = m_appSetup.DesiredSwapchainImageUsageFlags;
        if (m_launchOptions.ScreenshotsEnabled != OptionUserChoice::Off)
        {
          // Add this to allow for screenshot support and we rely on the filtering below to remove it if its unsupported
          desiredSwapchainImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }

        auto fallbackExtent = ConvertUtil::Convert(GetScreenExtent());
        const VkPresentModeKHR presentMode =
          !m_launchOptions.OverridePresentMode ? m_appSetup.DesiredSwapchainPresentMode : m_launchOptions.PresentMode;
        const auto supportedImageUsageFlags = FilterUnsupportedImageUsageFlags(m_physicalDevice.Device, m_surface, desiredSwapchainImageUsageFlags);
        const VkImageUsageFlags desiredImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | supportedImageUsageFlags;

        m_swapchain = SwapchainKHRUtil::CreateSwapchain(
          m_physicalDevice.Device, m_device.Get(), 0, m_surface, DESIRED_MIN_SWAP_BUFFER_COUNT, 1, desiredImageUsageFlags, VK_SHARING_MODE_EXCLUSIVE,
          0, nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, presentMode, VK_TRUE, m_swapchain.Get(), fallbackExtent, m_surfaceFormatInfo);

        uint32_t swapchainImageCount = m_swapchain.GetImageCount();
        if (swapchainImageCount == 0)
        {
          throw std::runtime_error("We need at least one image in the swapchain");
        }

        m_dependentResources.MaxFramesInFlight = std::min(GetRenderConfig().MaxFramesInFlight, swapchainImageCount);
        FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::BuildResources(): Swapchain image count: " << swapchainImageCount);

        if (m_appSetup.DepthBuffer == DepthBufferMode::Enabled)
        {
          FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::BuildResources(): Creating depth image view");
          m_dependentResources.DepthImage = CreateBasicDepthImageView(m_device, m_swapchain.GetImageExtent(), m_resources.MainCommandPool.Get());

          FSLLOG2(LogType::Verbose2,
                  "DemoAppVulkanBasic::BuildResources(): DepthBuffer PixelFormat: " << m_dependentResources.DepthImage.Image().GetFormat());
        }

        FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::BuildResources(): Creating command buffers");
        m_dependentResources.CmdBuffers.Reset(m_device.Get(), m_resources.MainCommandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                              swapchainImageCount);

        FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::BuildResources(): OnBuildResources");
        // Allow the app to create its resources
        VkFormat depthImageFormat =
          m_dependentResources.DepthImage.IsValid() ? m_dependentResources.DepthImage.Image().GetFormat() : VK_FORMAT_UNDEFINED;


        BuildResourcesContext buildResourcesContext(m_swapchain.GetImageExtent(), m_swapchain.GetImageFormat(), swapchainImageCount,
                                                    m_dependentResources.MaxFramesInFlight, depthImageFormat);
        auto mainRenderPass = OnBuildResources(buildResourcesContext);

        FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::BuildResources(): Populate swapchain records");

        m_dependentResources.SwapchainRecords.clear();
        m_dependentResources.SwapchainRecords.resize(swapchainImageCount);

        VkImageView depthImageView = m_dependentResources.DepthImage.IsValid() ? m_dependentResources.DepthImage.ImageView().Get() : VK_NULL_HANDLE;
        for (uint32_t i = 0; i < swapchainImageCount; ++i)
        {
          BuildSwapchainImageView(m_dependentResources.SwapchainRecords[i], i);
          const auto swapchainImageView = m_dependentResources.SwapchainRecords[i].SwapchainImageView.Get();

          FrameBufferCreateContext frameBufferCreateContext(swapchainImageView, m_swapchain.GetImageExtent(), mainRenderPass, depthImageView);
          m_dependentResources.SwapchainRecords[i].Framebuffer = CreateFramebuffer(frameBufferCreateContext);
        }

        // Create a struct containing all relevant information to be able to capture a screenshot on demand
        m_dependentResources.NGScreenshotLink = std::make_shared<Vulkan::NativeGraphicsSwapchainInfo>();

        if (m_nativeGraphicsService)
        {
          FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::BuildResources(): NativeGraphicsService::VulkanCreateDependentResources");
          m_nativeGraphicsService->VulkanCreateDependentResources(swapchainImageCount, mainRenderPass, m_appSetup.SubpassSystemUI, GetScreenExtent());

          // link the swapchain info with the native graphics service this information is used for enabling the screenshot capabilities.
          m_nativeGraphicsService->SetSwapchainInfoLink(m_dependentResources.NGScreenshotLink);
        }
      }
      catch (const std::exception& ex)
      {
        FSLLOG_ERROR("BuildResources failed with: " << ex.what());
        FreeResources();
        throw;
      }
      FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::BuildResources(): Completed");
    }

    void DemoAppVulkanBasic::FreeResources()
    {
      if (!m_dependentResources.Valid)
      {
        return;
      }
      FSLLOG2(LogType::Verbose, "DemoAppVulkanBasic::FreeResources()");

      SafeWaitForDeviceIdle();

      if (m_nativeGraphicsService)
      {
        m_nativeGraphicsService->VulkanDestroyDependentResources();
      }

      for (std::size_t i = 0; i < m_dependentResources.SwapchainRecords.size(); ++i)
      {
        m_dependentResources.SwapchainRecords[i].Framebuffer.Reset();
        m_dependentResources.SwapchainRecords[i].SwapchainImageView.Reset();
      }
      m_dependentResources.SwapchainRecords.clear();
      m_dependentResources.CurrentSwapBufferIndex = 0;

      // Allow the app to free its custom resources
      OnFreeResources();

      m_dependentResources.CmdBuffers.Reset();
      m_dependentResources.DepthImage.Reset();

      SafeWaitForDeviceIdle();
      m_dependentResources.MaxFramesInFlight = 0;
      m_dependentResources.Valid = false;
      FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::FreeResources(): Completed");
    }


    bool DemoAppVulkanBasic::TryRebuildResources()
    {
      if (m_currentAppState != AppState::Ready)
      {
        return false;
      }
      FreeResources();
      BuildResources();
      return true;
    }


    bool DemoAppVulkanBasic::TryGetSwapchainInfo(SwapchainInfo& rSwapchainInfo) const
    {
      if (!m_swapchain.IsValid() || m_currentAppState != AppState::Ready)
      {
        rSwapchainInfo = {};
        return false;
      }

      rSwapchainInfo = SwapchainInfo(m_swapchain.GetImageExtent(), m_swapchain[m_dependentResources.CurrentSwapBufferIndex],
                                     m_swapchain.GetImageFormat(), m_swapchain.GetImageUsageFlags());
      return true;
    }


    RapidVulkan::RenderPass DemoAppVulkanBasic::CreateBasicRenderPass()
    {
      if (!m_swapchain.IsValid())
      {
        throw UsageErrorException("Swapchain is not valid");
      }
      FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::CreateBasicRenderPass()");

      VkAttachmentDescription colorAttachmentDescription{};
      colorAttachmentDescription.flags = 0;
      colorAttachmentDescription.format = m_swapchain.GetImageFormat();
      colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
      colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      VkAttachmentReference colorAttachmentReference{};
      colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      VkSubpassDependency subpassDependency{};
      subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency.dstSubpass = 0;
      subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency.srcAccessMask = 0;
      subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

      VkSubpassDescription subpassDescription{};
      subpassDescription.flags = 0;
      subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription.inputAttachmentCount = 0;
      subpassDescription.pInputAttachments = nullptr;
      subpassDescription.colorAttachmentCount = 1;
      subpassDescription.pColorAttachments = &colorAttachmentReference;
      subpassDescription.pResolveAttachments = nullptr;
      subpassDescription.pDepthStencilAttachment = nullptr;
      subpassDescription.preserveAttachmentCount = 0;
      subpassDescription.pPreserveAttachments = nullptr;

      if (!m_dependentResources.DepthImage.IsValid())
      {
        // Basic renderPass no depth buffer
        return RapidVulkan::RenderPass(m_device.Get(), 0, 1, &colorAttachmentDescription, 1, &subpassDescription, 1, &subpassDependency);
      }

      VkAttachmentDescription depthAttachmentDescription{};
      depthAttachmentDescription.format = m_dependentResources.DepthImage.Image().GetFormat();
      depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
      depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      VkAttachmentReference depthAttachmentReference{};
      depthAttachmentReference.attachment = 1;
      depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      // patch -> VkSubpassDescription
      subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

      std::array<VkAttachmentDescription, 2> attachments = {colorAttachmentDescription, depthAttachmentDescription};
      return RapidVulkan::RenderPass(m_swapchain.GetDevice(), 0, static_cast<uint32_t>(attachments.size()), attachments.data(), 1,
                                     &subpassDescription, 1, &subpassDependency);
    }


    std::vector<DemoAppVulkanBasic::FrameDrawRecord> DemoAppVulkanBasic::CreateFrameSyncObjects(const VkDevice device,
                                                                                                const uint32_t maxFramesInFlight)
    {
      FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::CreateFrameSyncObjects()");

      std::vector<DemoAppVulkanBasic::FrameDrawRecord> framesDrawRecords(maxFramesInFlight);

      for (auto& rFrame : framesDrawRecords)
      {
        rFrame.ImageAcquiredSemaphore.Reset(device, 0);
        rFrame.RenderingCompleteSemaphore.Reset(device, 0);
        rFrame.InFlightFence.Reset(device, VK_FENCE_CREATE_SIGNALED_BIT);
      }
      return framesDrawRecords;
    }


    // m_swapchain.GetImageExtent()
    void DemoAppVulkanBasic::BuildSwapchainImageView(SwapchainRecord& rSwapchainRecord, const uint32_t swapBufferIndex)
    {
      VkComponentMapping componentMapping{};    // = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
      VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

      rSwapchainRecord.SwapchainImageView.Reset(m_device.Get(), 0, m_swapchain[swapBufferIndex], VK_IMAGE_VIEW_TYPE_2D, m_swapchain.GetImageFormat(),
                                                componentMapping, imageSubresourceRange);
    }


    RapidVulkan::Framebuffer DemoAppVulkanBasic::CreateFramebuffer(const FrameBufferCreateContext& frameBufferCreateContext)
    {
      std::array<VkImageView, 2> imageViews = {frameBufferCreateContext.SwapchainImageView, frameBufferCreateContext.DepthBufferImageView};
      const uint32_t numViews = frameBufferCreateContext.DepthBufferImageView == VK_NULL_HANDLE ? 1 : 2;

      return RapidVulkan::Framebuffer(m_device.Get(), 0, frameBufferCreateContext.RenderPass, numViews, imageViews.data(),
                                      frameBufferCreateContext.SwapChainImageExtent.width, frameBufferCreateContext.SwapChainImageExtent.height, 1);
    }


    Vulkan::VUImageMemoryView DemoAppVulkanBasic::CreateBasicDepthImageView(const Vulkan::VUDevice& device, const VkExtent2D& swapchainImageExtent,
                                                                            const VkCommandPool commandPool)
    {
      FSLLOG2(LogType::Verbose2, "DemoAppVulkanBasic::CreateBasicDepthImageView()");

      const auto depthFormat = device.GetPhysicalDevice().FindDepthFormat();

      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      imageCreateInfo.format = depthFormat;
      imageCreateInfo.extent = {swapchainImageExtent.width, swapchainImageExtent.height, 1};
      imageCreateInfo.mipLevels = 1;
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
      imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

      VkImageSubresourceRange subresourceRange{};
      subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = 1;
      subresourceRange.baseArrayLayer = 0;
      subresourceRange.layerCount = 1;

      Vulkan::VUImageMemoryView depthImageMemoryView(device, imageCreateInfo, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, "DepthBuffer");

      RapidVulkan::CommandBuffer commandBuffer(device.Get(), commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

      // Transition the depth buffer to a optimal layout
      {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        commandBuffer.Begin(beginInfo);
        {
          CommandBufferUtil::SetImageLayout(commandBuffer.Get(), depthImageMemoryView.Image().Get(), VK_IMAGE_ASPECT_COLOR_BIT,
                                            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange);
        }
        commandBuffer.End();
      }

      return depthImageMemoryView;
    }

    DemoAppVulkanBasic::RecreateSwapchainResult DemoAppVulkanBasic::TryRecreateSwapchain()
    {
      // Before we try to recreate things we need to ensure nothing is in use.
      if (vkDeviceWaitIdle(m_device.Get()) != VK_SUCCESS)
      {
        FSLLOG_WARNING("Wait for device idle failed");
        return RecreateSwapchainResult::Failed;
      }

      VkSurfaceCapabilitiesKHR surfaceCapabilities;
      if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice.Device, m_surface, &surfaceCapabilities) != VK_SUCCESS)
      {
        FSLLOG_WARNING("Failed to get surface capabilities");
        return RecreateSwapchainResult::Failed;
      }

      if (surfaceCapabilities.currentExtent.width == 0 || surfaceCapabilities.currentExtent.height == 0)
      {
        // Don't even try to recreate things at the moment
        return RecreateSwapchainResult::NotReady;
      }

      FSLLOG2(LogType::Verbose, "DemoAppVulkanBasic::TryRecreateSwapchain() Recreating vulkan swapchain");

      try
      {
        FreeResources();
        BuildResources();
        return RecreateSwapchainResult::Completed;
      }
      catch (const std::exception& ex)
      {
        FSLLOG_WARNING("Failed to recreate swapchain due to: " << ex.what());
        return RecreateSwapchainResult::Failed;
      }
    }

    AppDrawResult DemoAppVulkanBasic::TryDoPrepareDraw(const DemoTime& demoTime)
    {
      FSL_PARAM_NOT_USED(demoTime);

      if (m_currentAppState == AppState::WaitForSwapchainRecreation)
      {
        switch (TryRecreateSwapchain())
        {
        case RecreateSwapchainResult::Completed:
          break;
        case RecreateSwapchainResult::NotReady:
          return AppDrawResult::NotReady;
        case RecreateSwapchainResult::Failed:
          FSLLOG2(LogType::Verbose, "DemoAppVulkanBasic::TryDoPrepareDraw() Failed to recreate swapchain");
          return AppDrawResult::Failed;
        default:
          throw NotSupportedException("Unsupported result");
        }
      }
      else if (m_currentAppState != AppState::Ready)
      {
        throw NotSupportedException("Unsupported app state");
      }


      const auto waitSemaphore = m_resources.Frames[m_resources.CurrentFrame].ImageAcquiredSemaphore.Get();
      const auto inFlightFence = m_resources.Frames[m_resources.CurrentFrame].InFlightFence.Get();

      auto result = vkAcquireNextImageKHR(m_device.Get(), m_swapchain.Get(), DEFAULT_TIMEOUT, waitSemaphore, VK_NULL_HANDLE,
                                          &m_dependentResources.CurrentSwapBufferIndex);
      switch (result)
      {
      case VK_SUBOPTIMAL_KHR:
      case VK_SUCCESS:
      {
        // time to synchronize by waiting for the fence before we modify command queues etc.
        auto waitResult = vkWaitForFences(m_device.Get(), 1, &inFlightFence, VK_TRUE, DEFAULT_TIMEOUT);
        if (waitResult != VK_SUCCESS)
        {
          FSLLOG_WARNING("vkWaitForFences failed with: " << RapidVulkan::Debug::ToString(waitResult));
          return AppDrawResult::Failed;
        }

        auto resetResult = vkResetFences(m_device.Get(), 1, &inFlightFence);
        if (resetResult != VK_SUCCESS)
        {
          FSLLOG_WARNING("vkResetFences failed with: " << RapidVulkan::Debug::ToString(resetResult));
          return AppDrawResult::Failed;
        }
        return AppDrawResult::Completed;
      }
      case VK_ERROR_OUT_OF_DATE_KHR:
      case VK_ERROR_SURFACE_LOST_KHR:
        return AppDrawResult::Retry;
      default:
        return AppDrawResult::Failed;
      }
    }


    AppDrawResult DemoAppVulkanBasic::TryDoSwapBuffers(const DemoTime& demoTime)
    {
      FSL_PARAM_NOT_USED(demoTime);

      auto renderConfig = GetRenderConfig();

      const auto signalSemaphore = m_resources.Frames[m_resources.CurrentFrame].RenderingCompleteSemaphore.Get();
      const auto currentSwapBufferIndex = m_dependentResources.CurrentSwapBufferIndex;

      auto result = m_swapchain.TryQueuePresent(m_deviceQueue.Queue, 1, &signalSemaphore, &currentSwapBufferIndex, nullptr);

      // Move to next frame (we cap the 'max frames in flight' so its never larger than the amount of swapchainRecords)
      const auto cappedMaxFramesInFlight =
        std::min(renderConfig.MaxFramesInFlight, static_cast<uint32_t>(m_dependentResources.SwapchainRecords.size()));
      m_resources.CurrentFrame = (m_resources.CurrentFrame + 1) % cappedMaxFramesInFlight;

      switch (result)
      {
      case VK_SUBOPTIMAL_KHR:
      case VK_SUCCESS:
        return AppDrawResult::Completed;
      case VK_ERROR_OUT_OF_DATE_KHR:
      case VK_ERROR_SURFACE_LOST_KHR:
        // Simply try to recreate the swapchain
        return AppDrawResult::Retry;
      case VK_ERROR_OUT_OF_HOST_MEMORY:
      case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      case VK_ERROR_DEVICE_LOST:
      default:
        // This case should restart the app and demo host
        return AppDrawResult::Failed;
      }
    }


    void DemoAppVulkanBasic::SetAppState(AppDrawResult result)
    {
      switch (result)
      {
      case AppDrawResult::Completed:
        FSLLOG2_IF(m_currentAppState != AppState::Ready, LogType::Verbose, "DemoAppVulkanBasic::SetAppState(): Ready");
        m_currentAppState = AppState::Ready;
        break;
      case AppDrawResult::Failed:
      case AppDrawResult::NotReady:
      case AppDrawResult::Retry:
      default:
        FSLLOG2_IF(m_currentAppState != AppState::WaitForSwapchainRecreation, LogType::Verbose,
                   "DemoAppVulkanBasic::SetAppState(): WaitForSwapchainRecreation");
        m_currentAppState = AppState::WaitForSwapchainRecreation;
        break;
      }
    }
  }
}
