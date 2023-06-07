/*
 * Vulkan Example base class
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in VulkanExampleBase to make it easier
// to port samples.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <memory>
#include <sstream>
#include <utility>
// #include <chrono>

using namespace std;
using namespace RapidVulkan;

namespace Fsl
{
  using namespace Vulkan;

  namespace Willems
  {
    namespace
    {
      VkFormat GetSupportedDepthFormat(VkPhysicalDevice physicalDevice)
      {
        // Since all depth formats may be optional, we need to find a suitable depth format to use
        // Start with the highest precision packed format
        std::vector<VkFormat> depthFormats = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT,
                                              VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM};

        for (auto& format : depthFormats)
        {
          VkFormatProperties formatProps;
          vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
          // Format must support depth stencil attachment for optimal tiling
          if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0u)
          {
            return format;
          }
        }
        throw NotSupportedException("No compatible depth format fount");
      }
    }


    void VulkanWillemsDemoApp::_PostConstruct()
    {
      DemoAppVulkan::_PostConstruct();

      // Done this way to ensure a app flow that follows the original example
      // with the added benefit of running this on a fully constructed object
      try
      {
        Prepare();
      }
      catch (const std::exception&)
      {
        // Wait for everything to be idle before we try to free it
        SafeWaitForDeviceIdle();
        throw;
      }
    }


    void VulkanWillemsDemoApp::_Update(const DemoTime& demoTime)
    {
      m_frameTimer = demoTime.DeltaTime;

      m_camera.Update(m_frameTimer);
      if (m_camera.Moving())
      {
        m_viewChanged = true;
      }

      ++m_frameCounter;

      // Convert to clamped timer value
      if (!m_paused)
      {
        m_timer += m_timerSpeed * m_frameTimer;
        if (m_timer > 1.0)
        {
          m_timer -= 1.0f;
        }
      }

      m_fpsTimer += TimeSpanUtil::ToMicrosecondsF(demoTime.ElapsedTime);
      if (m_fpsTimer > 1000.0f)
      {
        // FIX:
        // if (!m_enableTextOverlay)
        //{
        //  std::string windowTitle = GetWindowTitle();
        //  SetWindowText(window, windowTitle.c_str());
        //}
        m_lastFPS = static_cast<uint32_t>(round(1.0f / m_frameTimer));
        UpdateTextOverlay();
        m_fpsTimer = 0.0f;
        m_frameCounter = 0;
      }

      if (m_viewChanged)
      {
        m_viewChanged = false;
        OnViewChanged();
      }

      DemoAppVulkan::_Update(demoTime);
    }


    void VulkanWillemsDemoApp::_Draw(const FrameInfo& frameInfo)
    {
      DemoAppVulkan::_Draw(frameInfo);


      if (m_device.IsValid())
      {
        // Flush device to make sure all resources can be freed
        vkDeviceWaitIdle(m_device.Get());
      }
    }


    void VulkanWillemsDemoApp::OnKeyEvent(const KeyEvent& event)
    {
      DemoAppVulkan::OnKeyEvent(event);
      if (event.IsHandled())
      {
        return;
      }

      const bool isPressed = event.IsPressed();
      if (isPressed)
      {
        switch (event.GetKey())
        {
        case VirtualKey::F1:
          if (isPressed && m_enableTextOverlay && m_textOverlay)
          {
            m_textOverlay->Visible = !m_textOverlay->Visible;
            m_commandBuffersDirty = true;
          }
          break;
        default:
          break;
        }
      }


      if (m_camera.Type == Camera::CameraType::FirstPerson)
      {
        switch (event.GetKey())
        {
        case VirtualKey::W:
        case VirtualKey::GamePadDpadUp:
          m_camera.Keys.Up = isPressed;
          break;
        case VirtualKey::S:
        case VirtualKey::GamePadDpadDown:
          m_camera.Keys.Down = isPressed;
          break;
        case VirtualKey::A:
        case VirtualKey::GamePadDpadLeft:
          m_camera.Keys.Left = isPressed;
          break;
        case VirtualKey::D:
        case VirtualKey::GamePadDpadRight:
          m_camera.Keys.Right = isPressed;
          break;
        default:
          break;
        }
      }
    }


    void VulkanWillemsDemoApp::OnMouseButtonEvent(const MouseButtonEvent& event)
    {
      DemoAppVulkan::OnMouseButtonEvent(event);
      if (event.IsHandled())
      {
        return;
      }

      const auto pos = event.GetPosition();
      m_mousePos.x = static_cast<float>(pos.X);
      m_mousePos.y = static_cast<float>(pos.Y);

      switch (event.GetButton())
      {
      case VirtualMouseButton::Left:
        m_mouseButtons.Left = event.IsPressed();
        break;
      case VirtualMouseButton::Middle:
        m_mouseButtons.Middle = event.IsPressed();
        break;
      case VirtualMouseButton::Right:
        m_mouseButtons.Right = event.IsPressed();
        break;
      default:
        break;
      }

      m_viewChanged = true;
    }


    void VulkanWillemsDemoApp::OnMouseMoveEvent(const MouseMoveEvent& event)
    {
      DemoAppVulkan::OnMouseMoveEvent(event);
      if (event.IsHandled())
      {
        return;
      }

      const auto pos = event.GetPosition();
      const auto mouseButtonFlags = event.GetMouseButtonFlags();
      const Vector2 newPos(pos.X, pos.Y);

      m_mouseButtons.Left = mouseButtonFlags.IsLeftButtonPressed();
      m_mouseButtons.Middle = mouseButtonFlags.IsMiddleButtonPressed();
      m_mouseButtons.Right = mouseButtonFlags.IsRightButtonPressed();

      const float dx = m_mousePos.x - newPos.X;
      const float dy = m_mousePos.y - newPos.Y;

      if (m_mouseButtons.Left)
      {
        m_rotation.x += dy * 1.25f * m_rotationSpeed;
        m_rotation.y -= dx * 1.25f * m_rotationSpeed;
        m_camera.Rotate(glm::vec3(dy * m_camera.RotationSpeed, -dx * m_camera.RotationSpeed, 0.0f));
        m_viewChanged = true;
      }
      if (m_mouseButtons.Right)
      {
        const float mod = dy * .005f * m_zoomSpeed;
        m_zoom += mod;
        m_camera.Translate(glm::vec3(-0.0f, 0.0f, mod));
        m_viewChanged = true;
      }
      if (m_mouseButtons.Middle)
      {
        const float mod = 0.01f;
        m_cameraPos.x -= dx * mod;
        m_cameraPos.y -= dy * mod;
        m_camera.Translate(glm::vec3(-dx * mod, -dy * mod, 0.0f));
        m_viewChanged = true;
      }
      m_mousePos = glm::vec2(static_cast<float>(pos.X), static_cast<float>(pos.Y));
    }


    void VulkanWillemsDemoApp::OnMouseWheelEvent(const MouseWheelEvent& event)
    {
      DemoAppVulkan::OnMouseWheelEvent(event);
      if (event.IsHandled())
      {
        return;
      }

      const auto wheelDelta = event.GetDelta();
      const auto adjust = static_cast<float>(wheelDelta) * 0.005f * m_zoomSpeed;
      m_zoom += adjust;
      m_camera.Translate(glm::vec3(0.0f, 0.0f, adjust));
      m_viewChanged = true;
    }


    VulkanWillemsDemoApp::VulkanWillemsDemoApp(const DemoAppConfig& demoAppConfig, MeshLoaderAllocFunc meshLoaderAllocFunc)
      : DemoAppVulkan(demoAppConfig)
      , m_vulkanDevice(m_physicalDevice.Device, m_device.Get())
      , m_meshLoaderAllocFunc(std::move(meshLoaderAllocFunc))
      , m_viewChanged(false)
      , m_enableVSync(false)
      , m_deviceProperties(m_vulkanDevice.GetProperties())
      , m_deviceFeatures(m_vulkanDevice.GetFeatures())
      , m_enableTextOverlay(false)
    {
      m_defaultClearColor.float32[0] = 0.025f;
      m_defaultClearColor.float32[1] = 0.025f;
      m_defaultClearColor.float32[2] = 0.025f;
      m_defaultClearColor.float32[3] = 1.0f;

      // Create synchronization objects
      VkSemaphoreCreateInfo semaphoreCreateInfo{};
      semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
      semaphoreCreateInfo.pNext = nullptr;
      semaphoreCreateInfo.flags = 0;

      // Create a semaphore used to synchronize image presentation
      // Ensures that the image is displayed before we start submitting new commands to the queue
      m_semaphores.PresentComplete.Reset(m_device.Get(), semaphoreCreateInfo);
      // Create a semaphore used to synchronize command submission
      // Ensures that the image is not presented until all commands have been submitted and executed
      m_semaphores.RenderComplete.Reset(m_device.Get(), semaphoreCreateInfo);
      // Create a semaphore used to synchronize command submission
      // Ensures that the image is not presented until all commands for the text overlay have been submitted and executed
      // Will be inserted after the render complete semaphore if the text overlay is enabled
      m_semaphores.TextOverlayComplete.Reset(m_device.Get(), semaphoreCreateInfo);

      // Set up submit info structure
      // Semaphores will stay the same during application lifetime
      // Command buffer submission info is set by each example submitInfo{};
      m_submitInfo = VkSubmitInfo{};
      m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      m_submitInfo.pNext = nullptr;
      m_submitInfo.pWaitDstStageMask = &m_submitPipelineStages;
      m_submitInfo.waitSemaphoreCount = 1;
      m_submitInfo.pWaitSemaphores = m_semaphores.PresentComplete.GetPointer();
      m_submitInfo.signalSemaphoreCount = 1;
      m_submitInfo.pSignalSemaphores = m_semaphores.RenderComplete.GetPointer();

#ifdef __ANDROID__
      FSLLOG3_INFO("Forcing vsync on");
      m_enableVSync = true;
#endif

      FSLLOG3_INFO("VulkanWillemsDemoApp constructed");
    }


    VulkanWillemsDemoApp::~VulkanWillemsDemoApp()
    {
      FSLLOG3_INFO("VulkanWillemsDemoApp destroying");
      SafeWaitForDeviceIdle();
    }


    void VulkanWillemsDemoApp::Prepare()
    {
      const auto depthFormat = GetSupportedDepthFormat(m_physicalDevice.Device);
      CreateCommandPool();
      CreateSetupCommandBuffer();
      SetupSwapchain();
      CreateCommandBuffers();
      SetupDepthStencil(depthFormat);
      SetupRenderPass(m_swapchain.GetImageFormat(), depthFormat);
      CreatePipelineCache();
      SetupFrameBuffer();
      FlushSetupCommandBuffer();
      // Recreate setup command buffer for derived class
      CreateSetupCommandBuffer();

      m_textureLoader = std::make_unique<Willems::VulkanTextureLoader>(GetContentManager(), m_physicalDevice.Device, m_device.Get(),
                                                                       m_deviceQueue.Queue, m_commandPool.Get(), m_deviceActiveFeatures);

      if (m_enableTextOverlay)
      {
        // Load the text rendering shaders
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        shaderStages.push_back(LoadShader("shaders/textoverlay.vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
        shaderStages.push_back(LoadShader("shaders/textoverlay.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
        m_textOverlay = std::make_unique<VulkanTextOverlay>(&m_vulkanDevice, m_deviceQueue.Queue, &m_frameBuffers, GetScreenExtent(),
                                                            m_renderPass.Get(), shaderStages);
        UpdateTextOverlay();
      }
    }

    bool VulkanWillemsDemoApp::CheckCommandBuffers()
    {
      for (std::size_t i = 0; i < m_drawCmdBuffers.Size(); ++i)
      {
        if (m_drawCmdBuffers[i] == VK_NULL_HANDLE)
        {
          return false;
        }
      }
      return true;
    }


    void VulkanWillemsDemoApp::DestroyCommandBuffers()
    {
      m_drawCmdBuffers.Reset();
    }


    void VulkanWillemsDemoApp::CreateCommandPool()
    {
      m_commandPool.Reset(m_device.Get(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_deviceQueue.QueueFamilyIndex);
    }


    void VulkanWillemsDemoApp::CreateSetupCommandBuffer()
    {
      VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
      cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufAllocateInfo.commandPool = m_commandPool.Get();
      cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      cmdBufAllocateInfo.commandBufferCount = 1;

      m_setupCmdBuffer.Reset(m_device.Get(), cmdBufAllocateInfo);

      VkCommandBufferBeginInfo cmdBufInfo{};
      cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

      m_setupCmdBuffer.Begin(cmdBufInfo);
    }


    void VulkanWillemsDemoApp::SetupSwapchain()
    {
      const auto screenExtent = GetScreenExtent();
      m_swapchain.Reset(m_physicalDevice.Device, m_device.Get(), m_surface, screenExtent, m_enableVSync);
    }


    void VulkanWillemsDemoApp::CreateCommandBuffers()
    {
      // Create one command buffer for each swap chain image and reuse for rendering
      VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
      cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufAllocateInfo.commandPool = m_commandPool.Get();
      cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      cmdBufAllocateInfo.commandBufferCount = m_swapchain.GetImageCount();

      m_drawCmdBuffers.Reset(m_device.Get(), cmdBufAllocateInfo);
    }


    void VulkanWillemsDemoApp::SetupDepthStencil(const VkFormat depthFormat)
    {
      const auto screenExtent = TypeConverter::UncheckedTo<VkExtent3D>(PxExtent3D(GetScreenExtent(), 1));

      VkImageCreateInfo image{};
      image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      image.pNext = nullptr;
      image.imageType = VK_IMAGE_TYPE_2D;
      image.format = depthFormat;
      image.extent = screenExtent;
      image.mipLevels = 1;
      image.arrayLayers = 1;
      image.samples = VK_SAMPLE_COUNT_1_BIT;
      image.tiling = VK_IMAGE_TILING_OPTIMAL;
      image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      image.flags = 0;

      m_depthStencil.Image.Reset(m_device.Get(), image);

      const VkMemoryRequirements memReqs = m_depthStencil.Image.GetImageMemoryRequirements();
      VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = m_physicalDevice.MemoryProperties;
      const auto memoryTypeIndex =
        MemoryTypeUtil::GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

      VkMemoryAllocateInfo memAlloc{};
      memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memAlloc.pNext = nullptr;
      memAlloc.allocationSize = 0;
      memAlloc.memoryTypeIndex = 0;
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex = memoryTypeIndex;

      m_depthStencil.Mem.Reset(m_device.Get(), memAlloc);
      RAPIDVULKAN_CHECK(vkBindImageMemory(m_device.Get(), m_depthStencil.Image.Get(), m_depthStencil.Mem.Get(), 0));

      VkImageViewCreateInfo depthStencilView{};
      depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      depthStencilView.pNext = nullptr;
      depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
      depthStencilView.format = depthFormat;
      depthStencilView.flags = 0;
      depthStencilView.subresourceRange = {};
      depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
      depthStencilView.subresourceRange.baseMipLevel = 0;
      depthStencilView.subresourceRange.levelCount = 1;
      depthStencilView.subresourceRange.baseArrayLayer = 0;
      depthStencilView.subresourceRange.layerCount = 1;
      depthStencilView.image = m_depthStencil.Image.Get();

      m_depthStencil.View.Reset(m_device.Get(), depthStencilView);
    }


    void VulkanWillemsDemoApp::SetupRenderPass(const VkFormat colorFormat, const VkFormat depthFormat)
    {
      std::array<VkAttachmentDescription, 2> attachments{};
      // Color attachment
      attachments[0].format = colorFormat;
      attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      // Depth attachment
      attachments[1].format = depthFormat;
      attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      VkAttachmentReference colorReference{};
      colorReference.attachment = 0;
      colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      VkAttachmentReference depthReference{};
      depthReference.attachment = 1;
      depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      VkSubpassDescription subpassDescription{};
      subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription.colorAttachmentCount = 1;
      subpassDescription.pColorAttachments = &colorReference;
      subpassDescription.pDepthStencilAttachment = &depthReference;
      subpassDescription.inputAttachmentCount = 0;
      subpassDescription.pInputAttachments = nullptr;
      subpassDescription.preserveAttachmentCount = 0;
      subpassDescription.pPreserveAttachments = nullptr;
      subpassDescription.pResolveAttachments = nullptr;

      // Subpass dependencies for layout transitions
      std::array<VkSubpassDependency, 2> dependencies{};

      dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      dependencies[0].dstSubpass = 0;
      dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      dependencies[1].srcSubpass = 0;
      dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
      dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      VkRenderPassCreateInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
      renderPassInfo.attachmentCount = UncheckedNumericCast<uint32_t>(attachments.size());
      renderPassInfo.pAttachments = attachments.data();
      renderPassInfo.subpassCount = 1;
      renderPassInfo.pSubpasses = &subpassDescription;
      renderPassInfo.dependencyCount = UncheckedNumericCast<uint32_t>(dependencies.size());
      renderPassInfo.pDependencies = dependencies.data();

      m_renderPass.Reset(m_device.Get(), renderPassInfo);
    }


    void VulkanWillemsDemoApp::CreatePipelineCache()
    {
      VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
      pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
      m_pipelineCache.Reset(m_device.Get(), pipelineCacheCreateInfo);
    }


    void VulkanWillemsDemoApp::SetupFrameBuffer()
    {
      const auto screenExtent = GetScreenExtent();

      std::array<VkImageView, 2> attachments{};

      // Depth/Stencil attachment is the same for all frame buffers
      attachments[1] = m_depthStencil.View.Get();

      VkFramebufferCreateInfo frameBufferCreateInfo{};
      frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      frameBufferCreateInfo.pNext = nullptr;
      frameBufferCreateInfo.renderPass = m_renderPass.Get();
      frameBufferCreateInfo.attachmentCount = UncheckedNumericCast<uint32_t>(attachments.size());
      frameBufferCreateInfo.pAttachments = attachments.data();
      frameBufferCreateInfo.width = screenExtent.Width;
      frameBufferCreateInfo.height = screenExtent.Height;
      frameBufferCreateInfo.layers = 1;

      // Create frame buffers for every swap chain image
      m_frameBuffers.clear();
      m_frameBuffers.resize(m_swapchain.GetImageCount());
      for (uint32_t i = 0; i < m_frameBuffers.size(); ++i)
      {
        attachments[0] = m_swapchain[i].View.Get();
        m_frameBuffers[i].Reset(m_device.Get(), frameBufferCreateInfo);
      }
      m_currentBufferIndex = 0;
    }


    void VulkanWillemsDemoApp::FlushSetupCommandBuffer()
    {
      if (!m_setupCmdBuffer.IsValid())
      {
        return;
      }

      m_setupCmdBuffer.End();

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = m_setupCmdBuffer.GetPointer();

      m_deviceQueue.Submit(1, &submitInfo, VK_NULL_HANDLE);
      m_deviceQueue.WaitIdle();

      m_setupCmdBuffer.Reset();
    }


    CommandBuffer VulkanWillemsDemoApp::CreateCommandBuffer(const VkCommandBufferLevel level, const bool begin)
    {
      VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
      cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufAllocateInfo.commandPool = m_commandPool.Get();
      cmdBufAllocateInfo.level = level;
      cmdBufAllocateInfo.commandBufferCount = 1;

      CommandBuffer cmdBuffer(m_device.Get(), cmdBufAllocateInfo);

      // If requested, also start the new command buffer
      if (begin)
      {
        VkCommandBufferBeginInfo cmdBufInfo{};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufInfo.pNext = nullptr;
        cmdBuffer.Begin(cmdBufInfo);
      }
      return cmdBuffer;
    }


    void VulkanWillemsDemoApp::FlushCommandBuffer(RapidVulkan::CommandBuffer& rCommandBuffer, const VkQueue queue, const bool free)
    {
      if (!rCommandBuffer.IsValid())
      {
        FSLLOG3_DEBUG_WARNING("Can not flush a invalid command buffer");
        return;
      }
      rCommandBuffer.End();

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = rCommandBuffer.GetPointer();

      RAPIDVULKAN_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
      RAPIDVULKAN_CHECK(vkQueueWaitIdle(queue));

      if (free)
      {
        rCommandBuffer.Reset();
      }
    }


    void VulkanWillemsDemoApp::CreateBuffer(Buffer& rBuffer, Memory& rMemory, const VkBufferUsageFlags usageFlags,
                                            const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size, const void* pData)
    {
      try
      {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usageFlags;

        rBuffer.Reset(m_device.Get(), bufferCreateInfo);

        VkMemoryRequirements memReqs = rBuffer.GetBufferMemoryRequirements();
        VkMemoryAllocateInfo memAlloc{};
        memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAlloc.pNext = nullptr;
        memAlloc.allocationSize = 0;
        memAlloc.memoryTypeIndex = 0;
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = m_vulkanDevice.GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);

        rMemory.Reset(m_device.Get(), memAlloc);

        if (pData != nullptr)
        {
          void* pMapped = nullptr;
          rMemory.MapMemory(0, size, 0, &pMapped);
          {
            std::memcpy(pMapped, pData, size);
          }
          rMemory.UnmapMemory();
        }
        RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), rBuffer.Get(), rMemory.Get(), 0));
      }
      catch (const std::exception&)
      {
        rBuffer.Reset();
        rMemory.Reset();
        throw;
      }
    }


    void VulkanWillemsDemoApp::CreateBuffer(RapidVulkan::Buffer& rBuffer, RapidVulkan::Memory& rMemory, const VkBufferUsageFlags usage,
                                            const VkDeviceSize size, const void* const pData)
    {
      return CreateBuffer(rBuffer, rMemory, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, size, pData);
    }


    void VulkanWillemsDemoApp::CreateBuffer(RapidVulkan::Buffer& rBuffer, RapidVulkan::Memory& rMemory, VkDescriptorBufferInfo& rDescriptor,
                                            const VkBufferUsageFlags usage, const VkDeviceSize size, const void* const pData)
    {
      CreateBuffer(rBuffer, rMemory, usage, size, pData);
      rDescriptor.offset = 0;
      rDescriptor.buffer = rBuffer.Get();
      rDescriptor.range = size;
    }


    void VulkanWillemsDemoApp::CreateBuffer(RapidVulkan::Buffer& rBuffer, RapidVulkan::Memory& rMemory, VkDescriptorBufferInfo& rDescriptor,
                                            const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size,
                                            const void* const pData)
    {
      CreateBuffer(rBuffer, rMemory, usage, memoryPropertyFlags, size, pData);
      rDescriptor.offset = 0;
      rDescriptor.buffer = rBuffer.Get();
      rDescriptor.range = size;
    }


    void VulkanWillemsDemoApp::UpdateTextOverlay()
    {
      if (!m_enableTextOverlay)
      {
        return;
      }

      m_textOverlay->BeginTextUpdate();

      m_textOverlay->AddText(m_title, 5.0f, 5.0f, VulkanTextOverlay::TextAlign::Left);

      std::stringstream ss;
      ss << std::fixed << std::setprecision(3) << (m_frameTimer * 1000.0f) << "ms (" << m_lastFPS << " fps)";
      m_textOverlay->AddText(ss.str(), 5.0f, 25.0f, VulkanTextOverlay::TextAlign::Left);

      m_textOverlay->AddText(m_deviceProperties.deviceName, 5.0f, 45.0f, VulkanTextOverlay::TextAlign::Left);

      GetOverlayText(*m_textOverlay);

      m_textOverlay->EndTextUpdate();
    }


    void VulkanWillemsDemoApp::GetOverlayText(VulkanTextOverlay& /*rTextOverlay*/)
    {
      // Can be overriden in derived class
    }


    bool VulkanWillemsDemoApp::TryPrepareFrame()
    {
      if (m_commandBuffersDirty || m_textOverlay->Dirty)
      {
        m_commandBuffersDirty = false;
        BuildCommandBuffers();
      }

      // Get next image in the swap chain (back/front buffer)
      const auto result = m_swapchain.TryAcquireNextImage(m_semaphores.PresentComplete.Get(), m_currentBufferIndex);
      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
        // TODO: support 'soft restart'
        FSLLOG3_INFO("Restaring app due to VK_ERROR_OUT_OF_DATE_KHR");
        GetDemoAppControl()->RequestAppRestart();
        return false;
      }
      if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
      {
        throw std::runtime_error("Could not acquire next image.");
      }
      return true;
    }


    void VulkanWillemsDemoApp::SubmitFrame()
    {
      // Present the current buffer to the swap chain
      // Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
      // This ensures that the image is not presented to the windowing system until all commands have been submitted

      const VkSemaphore semaphore = m_semaphores.RenderComplete.Get();
      const auto result = m_swapchain.TryQueuePresent(m_deviceQueue.Queue, m_currentBufferIndex, semaphore);
      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
        // TODO: support 'soft restart'
        FSLLOG3_INFO("Restaring app due to VK_ERROR_OUT_OF_DATE_KHR");
        GetDemoAppControl()->RequestAppRestart();
        return;
      }
      if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
      {
        throw std::runtime_error("Could not present queue");
      }

      RAPIDVULKAN_CHECK(vkQueueWaitIdle(m_deviceQueue.Queue));
    }


    VkPipelineShaderStageCreateInfo VulkanWillemsDemoApp::LoadShader(const std::string& fileName, const VkShaderStageFlagBits stage)
    {
      ShaderModule shaderModule = LoadShader(fileName, m_device.Get(), stage);

      VkPipelineShaderStageCreateInfo shaderStage{};
      shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shaderStage.stage = stage;
      shaderStage.module = shaderModule.Get();
      shaderStage.pName = "main";    // todo : make param

      m_shaderModules.push_back(std::move(shaderModule));
      return shaderStage;
    }


    ShaderModule VulkanWillemsDemoApp::LoadShader(const std::string& fileName, const VkDevice device, const VkShaderStageFlagBits /*stage*/)
    {
      const auto shaderBinary = GetContentManager()->ReadBytes(IO::Path(fileName));

      VkShaderModuleCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      createInfo.codeSize = shaderBinary.size();
      createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBinary.data());

      return {device, createInfo};
    }


    MeshLoader::MeshBuffer VulkanWillemsDemoApp::LoadMesh(const std::string& filename, const std::vector<MeshLoader::VertexLayout>& vertexLayout,
                                                          const float scale)
    {
      MeshLoader::MeshCreateInfo meshCreateInfo{};
      meshCreateInfo.Scale = glm::vec3(scale);
      meshCreateInfo.Center = glm::vec3(0.0f);
      meshCreateInfo.UVScale = glm::vec2(1.0f);
      return LoadMesh(filename, vertexLayout, &meshCreateInfo);
    }


    MeshLoader::MeshBuffer VulkanWillemsDemoApp::LoadMesh(const std::string& filename, const std::vector<MeshLoader::VertexLayout>& vertexLayout,
                                                          const MeshLoader::MeshCreateInfo* const pMeshCreateInfo)
    {
      if (!m_meshLoaderAllocFunc)
      {
        throw NotSupportedException("Mesh loading is not supported, switch to a VulkanWillemsMeshDemoApp type if you need it");
      }

      auto meshLoader = m_meshLoaderAllocFunc(GetContentManager());

      meshLoader->LoadMesh(filename);
      assert(meshLoader->GetMeshCount() > 0);

      CommandBuffer copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, false);
      return meshLoader->CreateBuffers(m_vulkanDevice, vertexLayout, pMeshCreateInfo, true, copyCmd.Get(), m_deviceQueue.Queue);
    }


    void VulkanWillemsDemoApp::DrawUI(const VkCommandBuffer commandBuffer)
    {
      if (m_textOverlay && m_textOverlay->Visible)
      {
        m_textOverlay->AddToCommandBuffer(commandBuffer);
      }
    }
  }
}
