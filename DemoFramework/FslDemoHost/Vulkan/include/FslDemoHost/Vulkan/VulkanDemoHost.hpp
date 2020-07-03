#ifndef FSLDEMOHOST_VULKAN_VULKANDEMOHOST_HPP
#define FSLDEMOHOST_VULKAN_VULKANDEMOHOST_HPP
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

#include <FslDemoApp/Shared/Host/ConfigControl.hpp>
#include <FslDemoHost/Base/ADemoHost.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <FslUtil/Vulkan1_0/SafeType/InstanceCreateInfoCopy.hpp>
#include <RapidVulkan/Instance.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Vulkan
  {
    class NativeGraphicsService;
  }

  class IVulkanNativeWindowSystem;
  class IVulkanNativeWindow;
  class IWindowHostInfoControl;
  class VulkanDemoHostOptionParser;
  class VulkanHostService;

  class VulkanDemoHost : public ADemoHost
  {
    std::shared_ptr<VulkanDemoHostOptionParser> m_options;
    std::shared_ptr<Vulkan::NativeGraphicsService> m_nativeGraphicsService;
    std::unique_ptr<NativeWindowSetup> m_nativeWindowSetup;
    std::shared_ptr<IVulkanNativeWindowSystem> m_windowSystem;
    std::shared_ptr<IVulkanNativeWindow> m_window;
    std::shared_ptr<IWindowHostInfoControl> m_windowHostInfoControl;
    std::shared_ptr<VulkanHostService> m_vulkanHostService;
    DemoHostConfig m_demoHostConfig;
    bool m_isActivated = true;
    DemoHostFeature m_activeApi;
    std::shared_ptr<Vulkan::InstanceCreateInfoCopy> m_instanceCreateInfo;
    RapidVulkan::Instance m_instance;
    Vulkan::VUPhysicalDeviceRecord m_physicalDevice;

  public:
    explicit VulkanDemoHost(const DemoHostConfig& demoHostConfig);
    ~VulkanDemoHost() override;

    void OnActivate() override;
    void OnDeactivate() override;
    void OnSuspend() override;
    void OnResume() override;
    DemoHostFeature GetActiveAPI() const override;
    DemoWindowMetrics GetWindowMetrics() const override;
    SwapBuffersResult TrySwapBuffers() override;
    bool ProcessNativeMessages(const bool allowBlock) override;

  private:
    void Init();
    void Shutdown();
    void InitVulkan();
    void ShutdownVulkan();
  };
}

#endif
