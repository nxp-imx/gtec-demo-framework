/****************************************************************************************************************************************************
 * Copyright 2018, 2022, 2024 NXP
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

#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslDemoApp/Base/FrameInfo.hpp>
#include <FslDemoApp/Base/Overlay/DemoAppProfilerOverlay.hpp>
#include <FslDemoApp/Base/Service/Host/IHostInfo.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslDemoHost/Vulkan/Config/DemoAppHostConfigVulkan.hpp>
#include <FslDemoService/Graphics/Control/GraphicsBeginFrameInfo.hpp>
#include <FslDemoService/Graphics/Control/GraphicsDependentCreateInfo.hpp>
#include <FslDemoService/Graphics/Control/IGraphicsServiceHost.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/BasicNativeBeginCustomVulkanFrameInfo.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/BasicNativeDependentCustomVulkanCreateInfo.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsSwapchainInfo.hpp>
#include <FslUtil/Vulkan1_0/Log/All.hpp>
#include <FslUtil/Vulkan1_0/Log/FmtAll.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/SurfaceFormatUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/Debug/Strings/VkImageUsageFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkResult.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <memory>

namespace Fsl::VulkanBasic
{
  namespace
  {
    namespace LocalConfig
    {
      // The desired minimum image count for the swap buffers
      constexpr const uint32_t DesiredMinSwapBufferCount = 2;

      constexpr const auto DefaultTimeout = std::numeric_limits<uint64_t>::max();
    }


    DemoAppVulkanSetup ProcessDemoAppSetup(DemoAppVulkanSetup demoAppVulkanSetup)
    {
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
          FSLLOG3_WARNING("Desired swapchain image usage flag 0x{:x} ({}) is unsupported so it was ignored", flag,
                          RapidVulkan::Debug::ToString(static_cast<VkImageUsageFlagBits>(flag)));
        }
        srcFlags &= ~flag;
        bitIndex = BitsUtil::IndexOf(srcFlags);
      }
      return supportedFlags;
    }


    Vulkan::SurfaceFormatInfo FindPreferredSurfaceInfo(VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface,
                                                       const std::deque<Vulkan::SurfaceFormatInfo>& preferredFormats)
    {
      std::vector<Vulkan::SurfaceFormatInfo> finalPreferredFormats(preferredFormats.begin(), preferredFormats.end());

      auto supportedFormats = Vulkan::PhysicalDeviceKHRUtil::GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface);
      if (!preferredFormats.empty())
      {
        auto res = Vulkan::SurfaceFormatUtil::TryFindPreferredFormat(SpanUtil::AsReadOnlySpan(supportedFormats),
                                                                     SpanUtil::AsReadOnlySpan(finalPreferredFormats));
        if (res.Format != VK_FORMAT_UNDEFINED)
        {
          return res;
        }
        FSLLOG3_VERBOSE("None of the preferred surface formats found, using default");
      }
      if (supportedFormats.empty())
      {
        throw NotSupportedException("No surface formats found");
      }

      // Try to locate a format
      {    // Add the suggested defaults
        const ReadOnlySpan<Vulkan::SurfaceFormatInfo> suggestedEntries = Vulkan::SurfaceFormatUtil::GetSuggestedDefaultPreferredSurfaceFormats();
        finalPreferredFormats.reserve(finalPreferredFormats.size() + suggestedEntries.size());
        for (const auto& suggested : suggestedEntries)
        {
          finalPreferredFormats.push_back(suggested);
        }
      }
      return Vulkan::SurfaceFormatUtil::TryFindSurfaceFormat(SpanUtil::AsReadOnlySpan(supportedFormats),
                                                             SpanUtil::AsReadOnlySpan(finalPreferredFormats), VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
    }

    AppDrawResult WaitForFenceAndResetIt(const VkDevice device, const VkFence fence)
    {
      {    // Wait for the current frames fence and reset it
        // time to synchronize by waiting for the fence before we modify command queues etc.
        auto waitResult = vkWaitForFences(device, 1, &fence, VK_TRUE, LocalConfig::DefaultTimeout);
        if (waitResult != VK_SUCCESS)
        {
          FSLLOG3_WARNING("vkWaitForFences failed with: {}", RapidVulkan::Debug::ToString(waitResult));
          return AppDrawResult::Failed;
        }

        auto resetResult = vkResetFences(device, 1, &fence);
        if (resetResult != VK_SUCCESS)
        {
          FSLLOG3_WARNING("vkResetFences failed with: {}", RapidVulkan::Debug::ToString(resetResult));
          return AppDrawResult::Failed;
        }
      }
      return AppDrawResult::Completed;
    }
  }

  DemoAppVulkanBasic::DemoAppVulkanBasic(const DemoAppConfig& demoAppConfig, const DemoAppVulkanSetup& demoAppVulkanSetup)
    : DemoAppVulkan(demoAppConfig)
    , AppSetup(ProcessDemoAppSetup(demoAppVulkanSetup))
    , m_cachedExtentPx(demoAppConfig.WindowMetrics.ExtentPx)
  {
    auto hostInfo = demoAppConfig.DemoServiceProvider.Get<IHostInfo>();
    auto hostConfig = hostInfo->GetConfig();
    if (hostConfig.StatOverlay)
    {
      m_demoAppProfilerOverlay = std::make_unique<DemoAppProfilerOverlay>(demoAppConfig.DemoServiceProvider, hostConfig.LogStatsFlags);
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

    m_resources.MainCommandPool.Reset(m_device.Get(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_deviceQueue.QueueFamilyIndex);
    m_resources.Frames = CreateFrameSyncObjects(m_device.Get(), GetRenderConfig().MaxFramesInFlight);
  }


  DemoAppVulkanBasic::~DemoAppVulkanBasic()
  {
    if (m_dependentResources.Valid)
    {
      auto currentLifeCycleState = GetObjectLifeCycleState();
      try
      {
        FSLLOG3_WARNING_IF(currentLifeCycleState != ObjectLifeCycle::Constructing,
                           "Resources still allocated during destruction, trying to free them");
        FreeResources();

        // Release the swapchain (we dont do this in FreeResources because BuildResources might reuse it).
        m_swapchain.Reset();
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Exception during FreeResources: {}", ex.what());
        FSLLOG3_ERROR("FreeResources failed to complete. Setting exit code to failure");
        try
        {
          // IMPROVEMENT: Be able to RequestExit and change exit code in one operation
          // We request a exit (this might silently return if a exit is already scheduled so the exit code will not be set)
          GetDemoAppControl()->RequestExit(EXIT_FAILURE);
          // Which is why we also call ChangeExitCode (beware this only changes the exit code if a non-default exit code has not been set)
          GetDemoAppControl()->ChangeExitCode(EXIT_FAILURE);
        }
        catch (const std::exception& ex2)
        {
          FSLLOG3_ERROR("internal error, failed to request exit: {0}", ex2.what());
        }
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

  AppDrawResult DemoAppVulkanBasic::TryPrepareDraw(const FrameInfo& frameInfo)
  {
    auto result = TryDoPrepareDraw(frameInfo);
    SetAppState(result);

    const FrameDrawRecord& frameResources = m_resources.Frames[frameInfo.FrameIndex];
    if (result == AppDrawResult::Completed && m_dependentResources.NGScreenshotLink && m_swapchain.IsValid() && frameResources.HasSwapBufferImage)
    {
      *m_dependentResources.NGScreenshotLink = Vulkan::NativeGraphicsSwapchainInfo(m_swapchain[frameResources.AssignedSwapImageIndex],
                                                                                   m_swapchain.GetImageFormat(), m_swapchain.GetImageUsageFlags());
    }
    else
    {
      *m_dependentResources.NGScreenshotLink = {};
    }
    return result;
  }

  void DemoAppVulkanBasic::_BeginDraw(const FrameInfo& frameInfo)
  {
    DemoAppVulkan::_BeginDraw(frameInfo);

    // _Endframe was not called properly
    assert(!m_frameRecord.IsValid);
    if (m_graphicsServiceHost)
    {
      Vulkan::BasicNativeBeginCustomVulkanFrameInfo vulkanBeginInfo(m_dependentResources.CmdBuffers[frameInfo.FrameIndex]);
      GraphicsBeginFrameInfo beginInfo(frameInfo.FrameIndex, &vulkanBeginInfo);
      m_graphicsServiceHost->BeginFrame(beginInfo);
    }
    m_frameRecord = FrameRecord(frameInfo.FrameIndex);
  }

  void DemoAppVulkanBasic::_EndDraw(const FrameInfo& frameInfo)
  {
    if (m_graphicsServiceHost)
    {
      m_graphicsServiceHost->EndFrame();
    }
    m_frameRecord = {};
    DemoAppVulkan::_EndDraw(frameInfo);
  }


  void DemoAppVulkanBasic::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    DemoAppVulkan::ConfigurationChanged(windowMetrics);

    if (windowMetrics.ExtentPx != m_cachedExtentPx)
    {
      m_cachedExtentPx = windowMetrics.ExtentPx;

      // Quick, dirty and slow resize support by destroying any screen size dependent Vulkan resources.
      if (AppSetup.ActiveResizeStrategy == ResizeStrategy::RebuildResources)
      {
        if (IsResourcesAllocated())
        {
          TryRebuildResources();
        }
      }
    }
  }


  void DemoAppVulkanBasic::Draw(const FrameInfo& frameInfo)
  {
    const auto currentFrameIndex = frameInfo.FrameIndex;
    const FrameDrawRecord& frameRecord = m_resources.Frames[currentFrameIndex];
    // Allow the app to draw
    const VkFramebuffer framebuffer = m_dependentResources.SwapchainRecords[frameRecord.AssignedSwapImageIndex].Framebuffer.Get();

    // Possible improvement:
    // We could replace the VulkanDraw with a normal Draw call but that would just mean that all implementations would have
    // to get the commandBuffers, currentSwapBufferIndex and active framebuffer as the first thing they do.
    // We would need a helper method like this
    //    VulkanDrawContext& GetVulkanDrawContext() const
    // The context would be non copyable and contain the above mentioned things

    DrawContext drawContext(m_swapchain.GetImageExtent(), framebuffer, currentFrameIndex);
    VulkanDraw(frameInfo.Time, m_dependentResources.CmdBuffers, drawContext);

    assert(frameRecord.ImageAcquiredSemaphore.IsValid());
    const VkSemaphore waitSemaphore = frameRecord.ImageAcquiredSemaphore.Get();
    const VkSemaphore signalSemaphore = frameRecord.ImageReleasedSemaphore.Get();
    const VkFence queueSubmitFence = frameRecord.QueueSubmitFence.Get();

    // Submit the draw operations
    const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = &waitDstStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = m_dependentResources.CmdBuffers.GetPointer(currentFrameIndex);
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    m_deviceQueue.Submit(1, &submitInfo, queueSubmitFence);
  }


  AppDrawResult DemoAppVulkanBasic::TrySwapBuffers(const FrameInfo& frameInfo)
  {
    auto result = TryDoSwapBuffers(frameInfo);
    SetAppState(result);
    return result;
  }


  void DemoAppVulkanBasic::AddSystemUI([[maybe_unused]] VkCommandBuffer hCmdBuffer, [[maybe_unused]] const uint32_t frameIndex)
  {
    // We assume that we get called with the 'active' command buffer
    assert(m_dependentResources.Valid);
    // assert(cmdBufferIndex == m_resources.CurrentFrame);
    assert(hCmdBuffer == m_dependentResources.CmdBuffers[frameIndex]);

    if (m_demoAppProfilerOverlay)
    {
      m_demoAppProfilerOverlay->Draw(GetWindowMetrics());
    }
  }


  void DemoAppVulkanBasic::BuildResources()
  {
    if (m_dependentResources.Valid)
    {
      throw UsageErrorException("Resources are still allocated, call FreeResources first");
    }
    FSLLOG3_VERBOSE("DemoAppVulkanBasic::BuildResources()");

    try
    {
      // We do this right away to ensure we can call 'FreeResources' if something goes wrong
      m_dependentResources.Valid = true;

      FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): Creating swapchain");
      // m_launchOptions.ScreenshotsEnabled

      auto desiredSwapchainImageUsageFlags = AppSetup.DesiredSwapchainImageUsageFlags;
      if (m_launchOptions.ScreenshotsEnabled != OptionUserChoice::Off)
      {
        // Add this to allow for screenshot support and we rely on the filtering below to remove it if its unsupported
        desiredSwapchainImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      }

      auto fallbackExtent = TypeConverter::UncheckedTo<VkExtent2D>(GetScreenExtent());
      const VkPresentModeKHR presentMode = !m_launchOptions.OverridePresentMode ? AppSetup.DesiredSwapchainPresentMode : m_launchOptions.PresentMode;
      const auto supportedImageUsageFlags = FilterUnsupportedImageUsageFlags(m_physicalDevice.Device, m_surface, desiredSwapchainImageUsageFlags);
      const VkImageUsageFlags desiredImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | supportedImageUsageFlags;

      m_swapchain =
        Vulkan::SwapchainKHRUtil::CreateSwapchain(m_physicalDevice.Device, m_device.Get(), 0, m_surface, LocalConfig::DesiredMinSwapBufferCount, 1,
                                                  desiredImageUsageFlags, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                                  presentMode, VK_TRUE, m_swapchain.Get(), fallbackExtent, m_surfaceFormatInfo);

      uint32_t swapchainImageCount = m_swapchain.GetImageCount();
      if (swapchainImageCount == 0)
      {
        throw std::runtime_error("We need at least one image in the swapchain");
      }

      m_dependentResources.FramesInFlightCount = 1;
      // std::min(GetRenderConfig().MaxFramesInFlight, swapchainImageCount);
      FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): Swapchain image count: {}", swapchainImageCount);
      // Ensure that the render loop frame counter never goes above this value
      GetDemoAppControl()->SetRenderLoopFrameCounter(m_dependentResources.FramesInFlightCount);

      if (AppSetup.DepthBuffer == DepthBufferMode::Enabled)
      {
        FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): Creating depth image view");
        auto extent = m_swapchain.GetImageExtent();
        extent = VkExtent2D{std::max(extent.width, AppSetup.DepthBufferMinimumExtent.Width.Value),
                            std::max(extent.height, AppSetup.DepthBufferMinimumExtent.Height.Value)};
        m_dependentResources.DepthImage = CreateBasicDepthImageView(m_device, extent, m_resources.MainCommandPool.Get());

        FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): DepthBuffer PixelFormat: {}", m_dependentResources.DepthImage.Image().GetFormat());
      }

      FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): Creating command buffers");
      m_dependentResources.CmdBuffers.Reset(m_device.Get(), m_resources.MainCommandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                            m_dependentResources.FramesInFlightCount);

      FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): OnBuildResources");
      // Allow the app to create its resources
      const VkImageView depthImageView = m_dependentResources.DepthImage.GetImageView();
      const VkFormat depthImageFormat = m_dependentResources.DepthImage.GetFormat();
      const VkExtent2D depthImageExtent = m_dependentResources.DepthImage.GetExtent2D();

      BuildResourcesContext buildResourcesContext(m_swapchain.GetImageExtent(), m_swapchain.GetImageFormat(), swapchainImageCount,
                                                  m_dependentResources.FramesInFlightCount, depthImageView, depthImageFormat, depthImageExtent,
                                                  m_resources.MainCommandPool.Get());
      const VkRenderPass mainRenderPass = OnBuildResources(buildResourcesContext);

      FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): Populate swapchain records");

      m_dependentResources.SwapchainRecords.clear();
      m_dependentResources.SwapchainRecords.resize(swapchainImageCount);

      for (uint32_t i = 0; i < swapchainImageCount; ++i)
      {
        BuildSwapchainImageView(m_dependentResources.SwapchainRecords[i], i);
        const VkImageView swapchainImageView = m_dependentResources.SwapchainRecords[i].SwapchainImageView.Get();

        FrameBufferCreateContext frameBufferCreateContext(swapchainImageView, m_swapchain.GetImageExtent(), mainRenderPass, depthImageView);
        m_dependentResources.SwapchainRecords[i].Framebuffer = CreateFramebuffer(frameBufferCreateContext);
      }

      // Create a struct containing all relevant information to be able to capture a screenshot on demand
      m_dependentResources.NGScreenshotLink = std::make_shared<Vulkan::NativeGraphicsSwapchainInfo>();

      if (m_graphicsServiceHost)
      {
        FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): IGraphicsServiceHost::CreateDependentResources");

        // This also links the swapchain info with the native graphics service this information is used for enabling the screenshot capabilities.
        const VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        Vulkan::BasicNativeDependentCustomVulkanCreateInfo vulkanCreateInfo(pipelineCache, mainRenderPass, AppSetup.SubpassSystemUI,
                                                                            m_dependentResources.NGScreenshotLink);

        GraphicsDependentCreateInfo createInfo(GetScreenExtent(), &vulkanCreateInfo);
        m_graphicsServiceHost->CreateDependentResources(createInfo);
      }
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("BuildResources failed with: {}", ex.what());
      FreeResources();
      throw;
    }
    FSLLOG3_VERBOSE2("DemoAppVulkanBasic::BuildResources(): Completed");
  }

  void DemoAppVulkanBasic::FreeResources()
  {
    if (!m_dependentResources.Valid)
    {
      return;
    }
    FSLLOG3_VERBOSE("DemoAppVulkanBasic::FreeResources()");

    SafeWaitForDeviceIdle();

    if (m_graphicsServiceHost)
    {
      m_graphicsServiceHost->DestroyDependentResources();
    }

    for (std::size_t i = 0; i < m_dependentResources.SwapchainRecords.size(); ++i)
    {
      m_dependentResources.SwapchainRecords[i].Framebuffer.Reset();
      m_dependentResources.SwapchainRecords[i].SwapchainImageView.Reset();
    }
    m_dependentResources.SwapchainRecords.clear();

    // Allow the app to free its custom resources
    OnFreeResources();

    m_dependentResources.CmdBuffers.Reset();
    m_dependentResources.DepthImage.Reset();

    SafeWaitForDeviceIdle();
    m_dependentResources.FramesInFlightCount = 0;
    m_dependentResources.Valid = false;
    FSLLOG3_VERBOSE2("DemoAppVulkanBasic::FreeResources(): Completed");
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
    if (!m_swapchain.IsValid() || m_currentAppState != AppState::Ready || !m_frameRecord.IsValid)
    {
      FSLLOG3_WARNING_IF(!m_frameRecord.IsValid, "Not called while rendering a frame, so this will always fail");
      rSwapchainInfo = {};
      return false;
    }

    const auto frameAssignedSwapImageIndex = m_resources.Frames[m_frameRecord.FrameIndex].AssignedSwapImageIndex;

    rSwapchainInfo = SwapchainInfo(m_swapchain.GetImageExtent(), m_swapchain[frameAssignedSwapImageIndex], m_swapchain.GetImageFormat(),
                                   m_swapchain.GetImageUsageFlags());
    return true;
  }


  RapidVulkan::RenderPass DemoAppVulkanBasic::CreateBasicRenderPass()
  {
    if (!m_swapchain.IsValid())
    {
      throw UsageErrorException("Swapchain is not valid");
    }
    FSLLOG3_VERBOSE2("DemoAppVulkanBasic::CreateBasicRenderPass()");

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
      return {m_device.Get(), 0, 1, &colorAttachmentDescription, 1, &subpassDescription, 1, &subpassDependency};
    }

    // Ensure that the correct dst masks are set
    subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

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
    return {m_swapchain.GetDevice(), 0, static_cast<uint32_t>(attachments.size()), attachments.data(), 1, &subpassDescription, 1, &subpassDependency};
  }


  std::vector<DemoAppVulkanBasic::FrameDrawRecord> DemoAppVulkanBasic::CreateFrameSyncObjects(const VkDevice device, const uint32_t maxFramesInFlight)
  {
    FSLLOG3_VERBOSE2("DemoAppVulkanBasic::CreateFrameSyncObjects()");

    std::vector<DemoAppVulkanBasic::FrameDrawRecord> framesDrawRecords(maxFramesInFlight);

    for (auto& rFrame : framesDrawRecords)
    {
      assert(!rFrame.HasSwapBufferImage);
      assert(rFrame.AssignedSwapImageIndex == 0);
      assert(!rFrame.ImageAcquiredSemaphore.IsValid());
      // rFrame.ImageAcquiredSemaphore.Reset(device, 0);  // We set this on demand, so we start with a empty one
      rFrame.ImageReleasedSemaphore.Reset(device, 0);
      rFrame.QueueSubmitFence.Reset(device, VK_FENCE_CREATE_SIGNALED_BIT);
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

    return {m_device.Get(),
            0,
            frameBufferCreateContext.RenderPass,
            numViews,
            imageViews.data(),
            frameBufferCreateContext.SwapChainImageExtent.width,
            frameBufferCreateContext.SwapChainImageExtent.height,
            1};
  }


  Vulkan::VUImageMemoryView DemoAppVulkanBasic::CreateBasicDepthImageView(const Vulkan::VUDevice& device, const VkExtent2D& depthImageExtent,
                                                                          const VkCommandPool commandPool, const VkSampleCountFlagBits sampleCount)
  {
    FSLLOG3_VERBOSE2("DemoAppVulkanBasic::CreateBasicDepthImageView()");

    const auto depthFormat = device.GetPhysicalDevice().FindDepthFormat(false);

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = depthFormat;
    imageCreateInfo.extent = {depthImageExtent.width, depthImageExtent.height, 1};
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = sampleCount;
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

    // Transition the depth buffer to a optimal layout
    {
      RapidVulkan::CommandBuffer commandBuffer(device.Get(), commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
      commandBuffer.Begin(beginInfo);
      {
        Vulkan::CommandBufferUtil::SetImageLayout(commandBuffer.Get(), depthImageMemoryView.Image().Get(), VK_IMAGE_ASPECT_COLOR_BIT,
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
      FSLLOG3_WARNING("Wait for device idle failed");
      return RecreateSwapchainResult::Failed;
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice.Device, m_surface, &surfaceCapabilities) != VK_SUCCESS)
    {
      FSLLOG3_WARNING("Failed to get surface capabilities");
      return RecreateSwapchainResult::Failed;
    }

    if (surfaceCapabilities.currentExtent.width == 0 || surfaceCapabilities.currentExtent.height == 0)
    {
      // Don't even try to recreate things at the moment
      return RecreateSwapchainResult::NotReady;
    }

    FSLLOG3_VERBOSE("DemoAppVulkanBasic::TryRecreateSwapchain() Recreating vulkan swapchain");

    try
    {
      FreeResources();
      BuildResources();

      if (m_graphicsServiceHost)
      {
        m_graphicsServiceHost->OnRenderSystemEvent(BasicRenderSystemEvent::SwapchainRecreated);
      }
      return RecreateSwapchainResult::Completed;
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_WARNING("Failed to recreate swapchain due to: {}", ex.what());
      return RecreateSwapchainResult::Failed;
    }
  }

  // Per frame:
  // - ImageAcquiredSemaphore - Given to vkAcquireNextImageKHR, used by vkQueueSubmit (set when the presentation engine is finished using the
  // image).
  // - ImageReleasedSemaphore - Given to vkQueuePresentKHR, used by vkQueueSubmit. (set when the command buffer has finished executing)
  // - QueueSubmitFence       - Given to vkQueueSubmit, and its signaled once all submitted command buffers have completed execution


  AppDrawResult DemoAppVulkanBasic::TryDoPrepareDraw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    if (m_currentAppState == AppState::WaitForSwapchainRecreation)
    {
      switch (TryRecreateSwapchain())
      {
      case RecreateSwapchainResult::Completed:
        break;
      case RecreateSwapchainResult::NotReady:
        return AppDrawResult::NotReady;
      case RecreateSwapchainResult::Failed:
        FSLLOG3_VERBOSE("DemoAppVulkanBasic::TryDoPrepareDraw() Failed to recreate swapchain");
        return AppDrawResult::Failed;
      default:
        throw NotSupportedException("Unsupported result");
      }
    }
    else if (m_currentAppState != AppState::Ready)
    {
      throw NotSupportedException("Unsupported app state");
    }

    const auto currentFrameIndex = frameInfo.FrameIndex;
    uint32_t acquiredSwapImageIndex{0};
    VkResult result = VK_SUCCESS;
    RapidVulkan::Semaphore imageAcquiredSemaphore = m_resources.AcquireSemaphore(m_device.Get());
    {
      result = vkAcquireNextImageKHR(m_device.Get(), m_swapchain.Get(), LocalConfig::DefaultTimeout, imageAcquiredSemaphore.Get(), VK_NULL_HANDLE,
                                     &acquiredSwapImageIndex);
    }

    switch (result)
    {
    case VK_SUBOPTIMAL_KHR:
    case VK_SUCCESS:
      {
        {    // Wait for the frame to be ready, so we know the frame resources can be reused
          AppDrawResult waitResult = WaitForFenceAndResetIt(m_device.Get(), m_resources.Frames[currentFrameIndex].QueueSubmitFence.Get());
          if (waitResult != AppDrawResult::Completed)
          {
            return waitResult;
          }
        }

        {    // Patch the current frame record with the selected image
          // Check if the swapchain image is currently assigned to a frame, and if it is we wait for it
          SwapchainRecord& rSwapchainRecord = m_dependentResources.SwapchainRecords[acquiredSwapImageIndex];

          // Check if the assigned frame index was assigned to another frame and if it was we wait for that frame to finish before reclaiming the
          // swapchain record.
          if (rSwapchainRecord.HasAssignedFrame && rSwapchainRecord.AssignedFrameIndex != currentFrameIndex)
          {
            // FSLLOG3_INFO("Remapping frameIndex {} to image previously used in frameIndex{}", currentFrameIndex,
            // rSwapchainRecord.AssignedFrameIndex);
            const FrameDrawRecord& rOldFrame = m_resources.Frames[rSwapchainRecord.AssignedFrameIndex];

            // We only wait for the other frames fence (and it will be up to the frame to reset it once we get to it)
            auto waitResult = vkWaitForFences(m_device.Get(), 1, rOldFrame.QueueSubmitFence.GetPointer(), VK_TRUE, LocalConfig::DefaultTimeout);
            if (waitResult != VK_SUCCESS)
            {
              FSLLOG3_WARNING("vkWaitForFences failed with: {}", RapidVulkan::Debug::ToString(waitResult));
              return AppDrawResult::Failed;
            }
            rSwapchainRecord.HasAssignedFrame = false;
          }

          // Since we made sure that no frame is associated with the acquiredSwapImageIndex we can simply just assign it to this frame.
          // We also mark the swapchain record with the current frameIndex so we can do a quick lookup of the previously assigned frameIndex the
          // next time we need the same swapImageIndex

          FrameDrawRecord& rFrame = m_resources.Frames[currentFrameIndex];
          rFrame.HasSwapBufferImage = true;
          rFrame.AssignedSwapImageIndex = acquiredSwapImageIndex;
          rSwapchainRecord.HasAssignedFrame = true;
          rSwapchainRecord.AssignedFrameIndex = currentFrameIndex;

          // Release the old image acquired semaphore if available
          if (rFrame.ImageAcquiredSemaphore.IsValid())
          {
            m_resources.ReleaseSemaphore(std::move(rFrame.ImageAcquiredSemaphore));
          }
          // Store the new one
          rFrame.ImageAcquiredSemaphore = std::move(imageAcquiredSemaphore);
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


  AppDrawResult DemoAppVulkanBasic::TryDoSwapBuffers(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const FrameDrawRecord& frameResources = m_resources.Frames[frameInfo.FrameIndex];
    const VkSemaphore signalSemaphore = frameResources.ImageReleasedSemaphore.Get();

    auto result = m_swapchain.TryQueuePresent(m_deviceQueue.Queue, 1, &signalSemaphore, &frameResources.AssignedSwapImageIndex, nullptr);

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
      FSLLOG3_VERBOSE_IF(m_currentAppState != AppState::Ready, "DemoAppVulkanBasic::SetAppState(): Ready");
      m_currentAppState = AppState::Ready;
      break;
    case AppDrawResult::Failed:
    case AppDrawResult::NotReady:
    case AppDrawResult::Retry:
    default:
      if (m_currentAppState != AppState::WaitForSwapchainRecreation)
      {
        FSLLOG3_VERBOSE("DemoAppVulkanBasic::SetAppState(): WaitForSwapchainRecreation");
        if (m_graphicsServiceHost)
        {
          if (m_device.IsValid())
          {
            FSLLOG3_VERBOSE4("DemoAppVulkanBasic::SetAppState(): Swapchain lost, waiting for device to be idle");
            vkDeviceWaitIdle(m_device.Get());
          }
          // Give the native graphics service a chance to do some processing when this occurs
          m_graphicsServiceHost->OnRenderSystemEvent(BasicRenderSystemEvent::SwapchainLost);
        }
      }
      m_currentAppState = AppState::WaitForSwapchainRecreation;
      break;
    }
  }
}
