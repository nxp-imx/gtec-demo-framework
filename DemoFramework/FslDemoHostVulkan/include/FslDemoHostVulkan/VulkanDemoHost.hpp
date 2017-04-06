#ifndef FSLDEMOHOSTVULKAN_VULKANDEMOHOST_HPP
#define FSLDEMOHOSTVULKAN_VULKANDEMOHOST_HPP
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

#include <vulkan/vulkan.h>
#include <deque>
#include <vector>
#include <FslDemoApp/Host/ConfigControl.hpp>
#include <FslDemoHost/ADemoHost.hpp>
#include <FslGraphicsVulkan1_0/CommandBuffer.hpp>
#include <FslGraphicsVulkan1_0/CommandPool.hpp>
#include <FslGraphicsVulkan1_0/Device.hpp>
#include <FslGraphicsVulkan1_0/Framebuffer.hpp>
#include <FslGraphicsVulkan1_0/Instance.hpp>
#include <FslGraphicsVulkan1_0/ImageView.hpp>
#include <FslGraphicsVulkan1_0/RenderPass.hpp>
#include <FslGraphicsVulkan1_0/SwapchainKHR.hpp>
#include <FslNativeWindow/NativeWindowSetup.hpp>
#include <array>

namespace Fsl
{
  class VulkanDemoHostOptionParser;
  class IVulkanNativeWindowSystem;
  class IVulkanNativeWindow;
  class IEGLNativeWindow;
  class IEGLNativeWindowSystem;

  class VulkanDemoHost
    : public ADemoHost
  {

    std::shared_ptr<VulkanDemoHostOptionParser> m_options;
    std::unique_ptr<NativeWindowSetup> m_nativeWindowSetup;
    std::shared_ptr<IVulkanNativeWindowSystem> m_windowSystem;
    std::shared_ptr<IVulkanNativeWindow> m_window;
    DemoHostConfig m_demoHostConfig;
    bool m_isActivated;
    DemoHostFeature m_activeApi;
    Vulkan::Instance m_instance;
    VkPhysicalDevice m_physicalDevice;
    Vulkan::Device m_device;
    VkQueue m_queue;
    Vulkan::CommandPool m_commandPool;
    Vulkan::SwapchainKHR m_swapchain;
    VkExtent2D m_imageExtend;
    VkFormat m_imageFormat;
    Vulkan::RenderPass m_renderPass;
    std::vector<VkImage> m_swapchainImages;

    std::vector<Vulkan::ImageView> m_imageViews;
    std::vector<Vulkan::Framebuffer> m_framebuffers;
    std::vector<Vulkan::CommandBuffer> m_commandBuffers;
  public:
    VulkanDemoHost(const DemoHostConfig& demoHostConfig);
    ~VulkanDemoHost();

    virtual void OnActivate();
    virtual void OnDeactivate();
    virtual void OnSuspend();
    virtual void OnResume();
    virtual DemoHostFeature GetActiveAPI() const;
    virtual Point2 GetScreenResolution() const;
    virtual bool SwapBuffers();
    virtual bool ProcessNativeMessages(const bool allowBlock);
  private:
    void Init();
    void Shutdown();
    void InitVulkan();
    void ShutdownVulkan();
    void InitDeviceQueueCommandPool();
    void ShutdownDeviceQueueCommandPool();
    void InitRenderPass(const VkFormat imageFormat);
    void ShutdownRenderPass();
  };
}

#endif
