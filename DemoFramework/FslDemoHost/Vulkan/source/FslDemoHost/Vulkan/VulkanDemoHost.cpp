/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Base/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Vulkan/VulkanDemoHost.hpp>
#include <FslDemoHost/Vulkan/VulkanDemoHostOptionParser.hpp>
#include <FslNativeWindow/Vulkan/VulkanNativeWindowSystemFactory.hpp>
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindowSystem.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindow.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowProcessMessagesArgs.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Semaphore.hpp>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <vector>

#if 0
#define LOCAL_LOG(X) FSLLOG("VulkanDemoHost: " << X)
#else
#define LOCAL_LOG(X) {}
#endif

#if 1
#define LOCAL_LOG_WARNING(X) FSLLOG_WARNING("VulkanDemoHost: " << X)
#else
#define LOCAL_LOG_WARNING(X) {}
#endif

using namespace RapidVulkan;

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    uint32_t GetCommandQueueFamily(VkPhysicalDevice physicalDevice, VkSurfaceKHR vulkanSurface)
    {
      std::vector<VkQueueFamilyProperties> queueFamilyProperties = PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(physicalDevice);

      uint32_t queueIndex = 0;

      for (uint32_t currentQueueFamilyIndex = 0; currentQueueFamilyIndex < queueFamilyProperties.size(); currentQueueFamilyIndex++)
      {
        VkBool32 supported;
        RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, currentQueueFamilyIndex, vulkanSurface, &supported));

        // FIX: queueIndex is always zero, what was the intention?
        if (supported && (queueFamilyProperties[currentQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) && queueFamilyProperties[currentQueueFamilyIndex].queueCount > queueIndex)
          return currentQueueFamilyIndex;
      }

      throw GraphicsException("Vulkan could not locate a queue with the VK_QUEUE_GRAPHICS_BIT enabled");
    }


    void InitImageView(RapidVulkan::ImageView& rImageView, const VkDevice device, const VkImage image, const VkFormat imageFormat)
    {
      VkImageViewCreateInfo imageViewCreateInfo{};
      imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      imageViewCreateInfo.flags = 0;
      imageViewCreateInfo.image = image;
      imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      imageViewCreateInfo.format = imageFormat;
      imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
      imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

      rImageView.Reset(device, imageViewCreateInfo);
    }


    void InitFramebuffer(RapidVulkan::Framebuffer& rFramebuffers, const VkDevice device, const VkRenderPass renderPass, const VkImageView imageView, const uint32_t width, const uint32_t height)
    {
      VkFramebufferCreateInfo framebufferCreateInfo{};
      framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferCreateInfo.flags = 0;
      framebufferCreateInfo.renderPass = renderPass;
      framebufferCreateInfo.attachmentCount = 1;
      framebufferCreateInfo.pAttachments = &imageView;
      framebufferCreateInfo.width = width;
      framebufferCreateInfo.height = height;
      framebufferCreateInfo.layers = 1;

      rFramebuffers.Reset(device, framebufferCreateInfo);
    }


    void InitCommandBuffer(RapidVulkan::CommandBuffer& rCommandBuffer, const VkDevice device, const VkCommandPool commandPool, const VkRenderPass renderPass, const VkImage image, const VkExtent2D& imageExtent, const VkFramebuffer framebuffer)
    {
      VkCommandBufferAllocateInfo cmdBufferCreateInfo{};
      cmdBufferCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufferCreateInfo.commandPool = commandPool;
      cmdBufferCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      cmdBufferCreateInfo.commandBufferCount = 1;

      rCommandBuffer.Reset(device, cmdBufferCreateInfo);

      VkCommandBufferInheritanceInfo commandBufferInheritanceInfo{};
      commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
      commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
      commandBufferInheritanceInfo.subpass = 0;
      commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
      commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
      commandBufferInheritanceInfo.queryFlags = 0;
      commandBufferInheritanceInfo.pipelineStatistics = 0;

      VkCommandBufferBeginInfo commandBufferBeginInfo{};
      commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      commandBufferBeginInfo.flags = 0;
      commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

      rCommandBuffer.Begin(commandBufferBeginInfo);
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkCmdPipelineBarrier(rCommandBuffer.Get(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

        VkClearColorValue clearColorValue{};
        clearColorValue.float32[0] = 0.0f;
        clearColorValue.float32[1] = 0.0f;
        clearColorValue.float32[2] = 1.0f;
        clearColorValue.float32[3] = 1.0f;

        VkClearValue clearValues[1] = { clearColorValue };

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffer;
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent = imageExtent;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(rCommandBuffer.Get(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
          // The window is cleaned with above defined blue color due to the VK_ATTACHMENT_LOAD_OP_CLEAR (see render pass creation).
        }
        vkCmdEndRenderPass(rCommandBuffer.Get());

        memset(&imageMemoryBarrier, 0, sizeof(VkImageMemoryBarrier));
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkCmdPipelineBarrier(rCommandBuffer.Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
      }
      rCommandBuffer.End();
    }

  }


  VulkanDemoHost::VulkanDemoHost(const DemoHostConfig& demoHostConfig)
    : ADemoHost(demoHostConfig)
    , m_options(demoHostConfig.GetOptions<VulkanDemoHostOptionParser>())
    , m_nativeWindowSetup()
    , m_windowSystem()
    , m_window()
    , m_demoHostConfig(demoHostConfig)
    , m_isActivated(true)
    , m_activeApi(DemoHostFeatureName::OpenGLES, 0)
    , m_instance()
    , m_physicalDevice(VK_NULL_HANDLE)
    , m_device()
    , m_queue(VK_NULL_HANDLE)
    , m_commandPool()
    , m_swapchain()
    , m_imageExtend()
    , m_imageFormat()
    , m_renderPass()
    , m_swapchainImages()
    , m_imageViews()
    , m_framebuffers()
    , m_commandBuffers()
  {
    const NativeWindowSystemSetup nativeWindowSystemSetup(demoHostConfig.GetEventQueue(), m_options->GetNativeWindowConfig(), m_options->GetNativeWindowTag());
    m_windowSystem = VulkanNativeWindowSystemFactory::Allocate(nativeWindowSystemSetup);

    const DemoHostAppSetup hostAppSetup = demoHostConfig.GetDemoHostAppSetup();

    if (hostAppSetup.DemoHostFeatures->size() <= 0)
      throw NotSupportedException("Must use at least one feature");

    m_activeApi = hostAppSetup.DemoHostFeatures->front();

    m_nativeWindowSetup.reset(new NativeWindowSetup(demoHostConfig.GetDemoHostAppSetup().AppSetup.ApplicationName, demoHostConfig.GetEventQueue(), m_options->GetNativeWindowConfig()));

    Init();
  }


  VulkanDemoHost::~VulkanDemoHost()
  {
    Shutdown();
  }


  void VulkanDemoHost::OnActivate()
  {
    m_isActivated = true;
  }


  void VulkanDemoHost::OnDeactivate()
  {
    m_isActivated = false;
  }


  void VulkanDemoHost::OnSuspend()
  {
    Shutdown();
  }


  void VulkanDemoHost::OnResume()
  {
    Init();
  }


  DemoHostFeature VulkanDemoHost::GetActiveAPI() const
  {
    return m_activeApi;
  }


  Point2 VulkanDemoHost::GetScreenResolution() const
  {
    // FIX: this is the only real invalid data that we return
    return Point2(0, 0);
  }


  bool VulkanDemoHost::SwapBuffers()
  {
    if (!m_isActivated)
      return true;

    const Semaphore imageAcquiredSemaphore(m_device.Get(), 0);
    const Semaphore renderingCompleteSemaphore(m_device.Get(), 0);

    //

    // TODO: Check, if surface/window did resize and react on it.

    //

    uint32_t currentBuffer;

    auto result = vkAcquireNextImageKHR(m_device.Get(), m_swapchain.Get(), UINT64_MAX, imageAcquiredSemaphore.Get(), VK_NULL_HANDLE, &currentBuffer);
    if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
    {
      const VkSemaphore waitSemaphores = imageAcquiredSemaphore.Get();
      const VkSemaphore signalSemaphores = renderingCompleteSemaphore.Get();

      VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = &waitSemaphores;
      submitInfo.pWaitDstStageMask = &waitDstStageMask;

      // TODO Add commands.
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = m_commandBuffers[currentBuffer].GetPointer();

      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = &signalSemaphores;

      RAPIDVULKAN_CHECK(vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE));

      const VkSwapchainKHR swapchains = m_swapchain.Get();

      VkPresentInfoKHR presentInfo{};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = &signalSemaphores;
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = &swapchains;
      presentInfo.pImageIndices = &currentBuffer;
      presentInfo.pResults = nullptr;

      result = vkQueuePresentKHR(m_queue, &presentInfo);

      if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
      {
        RAPIDVULKAN_CHECK(vkQueueWaitIdle(m_queue));
      }
    }

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      // TODO: Rebuild swapchain etc.
    }
    else if (result != VK_SUCCESS)
    {
      return false;
    }

    //

    // Validate that we are not suspended
    return true;
  }


  bool VulkanDemoHost::ProcessNativeMessages(const bool allowBlock)
  {
    // This will be called while suspended
    // Allow the native window to process messages
    NativeWindowProcessMessagesArgs args(allowBlock);
    return m_windowSystem->ProcessMessages(args);
  }


  void VulkanDemoHost::Init()
  {
    LOCAL_LOG("Init");
    try
    {
      // Get the basic Vulkan configuration ready
      InitVulkan();

      const NativeVulkanSetup nativeVulkanSetup(m_instance.Get(), m_physicalDevice);
      m_window = m_windowSystem->CreateNativeWindow(*m_nativeWindowSetup, nativeVulkanSetup);

      InitDeviceQueueCommandPool();
      InitRenderPass(m_imageFormat);

      // Create command ImageView, Framebuffer and CommandBuffer per swapchain image
      {
        const auto device = m_device.Get();
        const auto renderPass = m_renderPass.Get();
        const auto commandPool = m_commandPool.Get();

        m_swapchainImages = SwapchainKHRUtil::GetSwapchainImagesKHR(device, m_swapchain.Get());

        m_imageViews.resize(m_swapchainImages.size());
        m_framebuffers.resize(m_swapchainImages.size());
        m_commandBuffers.resize(m_swapchainImages.size());

        for (std::size_t i = 0; i < m_swapchainImages.size(); ++i)
        {
          InitImageView(m_imageViews[i], device, m_swapchainImages[i], m_imageFormat);
          InitFramebuffer(m_framebuffers[i], device, renderPass, m_imageViews[i].Get(), static_cast<uint32_t>(m_imageExtend.width), static_cast<uint32_t>(m_imageExtend.height));
          InitCommandBuffer(m_commandBuffers[i], device, commandPool, renderPass, m_swapchainImages[i], m_imageExtend, m_framebuffers[i].Get());
        }
      }
    }
    catch (const std::exception&)
    {
      LOCAL_LOG("Init failed with exception: " << ex.what());
      Shutdown();
      throw;
    }
  }


  void VulkanDemoHost::Shutdown()
  {
    LOCAL_LOG("Shutdown");
    m_commandBuffers.clear();
    m_framebuffers.clear();
    m_imageViews.clear();
    ShutdownRenderPass();
    ShutdownDeviceQueueCommandPool();
    m_window.reset();
    ShutdownVulkan();
  }


  void VulkanDemoHost::InitVulkan()
  {
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "DemoApp";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "DemoFramework";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    // FIXME: Gather dynamically, also depending on platform.
    assert(m_windowSystem);
    const auto surfaceExtensionName = m_windowSystem->GetKHRSurfaceExtensionName();
    const char* enabledExtensionNames[2] = { VK_KHR_SURFACE_EXTENSION_NAME, surfaceExtensionName.c_str() };

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
    instanceCreateInfo.enabledExtensionCount = 2;
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames;

    // Acquire a Vulkan instance
    m_instance.Reset(instanceCreateInfo);

    std::vector<VkPhysicalDevice> allPhysicalDevices = InstanceUtil::EnumeratePhysicalDevices(m_instance.Get());
    if (allPhysicalDevices.size() <= 0)
      throw GraphicsException("Did not find any Vulkan enabled devices");

    // TODO: all app device selection and or command line selection

    // For now we just use the first device
    m_physicalDevice = allPhysicalDevices[0];
  }


  void VulkanDemoHost::ShutdownVulkan()
  {
    m_instance.Reset();
  }


  void VulkanDemoHost::InitDeviceQueueCommandPool()
  {
    const uint32_t queueFamilyIndex = GetCommandQueueFamily(m_physicalDevice, m_window->GetVulkanSurface());

    float queuePriorities[1] = { 0.0f };
    VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = 1;
    // FIXME: Gather dynamically, also depending on platform.
    const char* enabledExtensionNames[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames;
    deviceCreateInfo.pEnabledFeatures = nullptr;

    m_device.Reset(m_physicalDevice, deviceCreateInfo);

    // FIX: queueIndex came from the code that was re-factored into GetCommandQueueFamily, however in that code it was initialized
    //      to zero and never modified.
    uint32_t queueIndex = 0;
    vkGetDeviceQueue(m_device.Get(), queueFamilyIndex, queueIndex, &m_queue);

    //

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

    m_commandPool.Reset(m_device.Get(), commandPoolCreateInfo);

    //
    // Gather surface capabilities.
    //

    VkSurfaceCapabilitiesKHR surfaceCapabilities;

    RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_window->GetVulkanSurface(), &surfaceCapabilities));

    VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    if (!(surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
    {
      preTransform = surfaceCapabilities.currentTransform;
    }

    const std::vector<VkPresentModeKHR> surfacePresentModes = PhysicalDeviceKHRUtil::GetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_window->GetVulkanSurface());
    const std::vector<VkSurfaceFormatKHR> surfaceFormats = PhysicalDeviceKHRUtil::GetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_window->GetVulkanSurface());

    // Regarding specification, this present mode has to be supported.
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
    VkColorSpaceKHR imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    VkBool32 surfaceFormatFound = VK_FALSE;

    for (size_t i = 0; i < surfaceFormats.size(); i++)
    {
      if (surfaceFormats[i].format == format && surfaceFormats[i].colorSpace == imageColorSpace)
      {
        surfaceFormatFound = VK_TRUE;
        break;
      }
    }

    // The requested format was not found, default to the first format
    if (!surfaceFormatFound)
    {
      format = surfaceFormats[0].format;
      imageColorSpace = surfaceFormats[0].colorSpace;
    }

    // Create the swap chain
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = m_window->GetVulkanSurface();
    // FIMXE
    swapchainCreateInfo.minImageCount = 2;
    swapchainCreateInfo.imageFormat = format;
    swapchainCreateInfo.imageColorSpace = imageColorSpace;
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = preTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    // FIXME
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    m_swapchain.Reset(m_device.Get(), swapchainCreateInfo);

    m_imageExtend = surfaceCapabilities.currentExtent;
    m_imageFormat = format;
  }


  void VulkanDemoHost::ShutdownDeviceQueueCommandPool()
  {
    m_swapchain.Reset();
    m_commandPool.Reset();
    m_device.Reset();
  }


  void VulkanDemoHost::InitRenderPass(const VkFormat imageFormat)
  {
    VkAttachmentDescription attachmentDescription{};
    attachmentDescription.flags = 0;
    attachmentDescription.format = imageFormat;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &attachmentDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = nullptr;

    m_renderPass.Reset(m_device.Get(), renderPassCreateInfo);
  }


  void VulkanDemoHost::ShutdownRenderPass()
  {
    m_renderPass.Reset();
  }

}
