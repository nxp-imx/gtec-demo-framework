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

#include <Shared/VulkanCustom/VulkanWindowSystem.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Base/Host/DemoAppHostConfigWindow.hpp>
#include <FslDemoApp/Base/Host/DemoHostCustomWindowSystemSetup.hpp>
#include <FslDemoApp/Base/Service/Options/IOptions.hpp>
#include <FslDemoApp/Base/Service/Options/Options.hpp>
#include <FslDemoHost/Vulkan/Config/DemoAppHostConfigVulkan.hpp>
#include <FslDemoHost/Vulkan/Config/InstanceConfigUtil.hpp>
#include <FslNativeWindow/Vulkan/VulkanNativeWindowSystemFactory.hpp>
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindowSystem.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/SafeType/InstanceCreateInfoCopy.hpp>
#include <Shared/VulkanCustom/OptionParser.hpp>
#include <array>
#include <algorithm>

namespace Fsl
{
  using namespace Vulkan;


  VulkanWindowSystem::VulkanWindowSystem(const DemoHostCustomWindowSystemSetup& setup)
    : DemoHostCustomWindowSystem(VulkanNativeWindowSystemFactory::AllocateNative(setup.WindowSystemSetup))
    , m_instanceCreateInfo(std::make_shared<Vulkan::InstanceCreateInfoCopy>())
  {
    FSLLOG3_INFO("VulkanWindowSystem creating");
    Options optionsService(setup.Provider.Get<IOptions>());
    auto appOptionParser = optionsService.GetOptionParser<OptionParser>();
    const auto physicialDeviceIndex = appOptionParser->GetPhysicalDeviceIndex();
    const auto userChoiceValidationLayer = appOptionParser->GetValidationLayer();

    auto vulkanWindowSystem = std::dynamic_pointer_cast<IVulkanNativeWindowSystem>(m_windowSystem);
    if (!vulkanWindowSystem)
    {
      throw NotSupportedException("The window system was not of the expected type");
    }

    const std::string khrSurfaceExtensionName = vulkanWindowSystem->GetKHRSurfaceExtensionName();

    {
      const auto instanceConfig = InstanceConfigUtil::InstanceConfigAsCharArrays(
        InstanceConfigUtil::BuildInstanceConfig(khrSurfaceExtensionName, InstanceConfigUtil::InstanceUserChoice(userChoiceValidationLayer),
                                                std::dynamic_pointer_cast<DemoAppHostConfigVulkan>(setup.CustomDemoAppHostConfig)));

      m_instance = InstanceUtil::CreateInstance("VulkanWindowSystem", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0, 0, instanceConfig.Layers,
                                                instanceConfig.Extensions, m_instanceCreateInfo.get());
    }
    m_physicalDevice = VUPhysicalDeviceRecord(InstanceUtil::GetPhysicalDevice(m_instance.Get(), physicialDeviceIndex));

    FSLLOG3_INFO("VulkanWindowSystem created");
  }


  VulkanWindowSystem::~VulkanWindowSystem()
  {
    FSLLOG3_INFO("VulkanWindowSystem destroying");
  }


  std::shared_ptr<INativeWindow>
    VulkanWindowSystem::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                           const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    if (pPlatformCustomWindowAllocationParams != nullptr)
    {
      throw NotSupportedException("pPlatformCustomWindowAllocationParams must be null");
    }

    NativeVulkanSetup vulkanSetup(m_instance.Get(), m_physicalDevice.Device);
    return m_windowSystem->CreateNativeWindow(nativeWindowSetup, &vulkanSetup);
  }
}
