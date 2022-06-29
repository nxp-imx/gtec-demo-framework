/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Host/DemoAppHostConfigWindow.hpp>
#include <FslDemoApp/Base/Host/DemoHostCustomWindowSystemSetup.hpp>
#include <FslDemoApp/Base/Service/Options/IOptions.hpp>
#include <FslDemoApp/Base/Service/Options/Options.hpp>
#include <FslDemoHost/Vulkan/Config/PhysicalDeviceConfigUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include "ConfigUtil.hpp"

namespace Fsl::PhysicalDeviceConfigUtil
{
  namespace
  {
    struct DeviceConfigRequest
    {
      std::deque<Vulkan::FeatureRequest> ExtensionRequests;
    };


    DeviceConfigRequest BuildHostDeviceConfigRequest(const ReadOnlySpan<Vulkan::FeatureRequest> hostExtensionRequests, ConfigControl configControl)
    {
      // FIX: add support for config control
      FSL_PARAM_NOT_USED(configControl);
      DeviceConfigRequest deviceConfig;
      for (std::size_t i = 0; i < hostExtensionRequests.size(); ++i)
      {
        deviceConfig.ExtensionRequests.emplace_back(hostExtensionRequests[i]);
      }
      return deviceConfig;
    }

    DeviceConfig PrepareConfig(const VkPhysicalDevice device, const DeviceConfigRequest& deviceConfigRequest)
    {
      DeviceConfig config;
      ConfigUtil::PrepareConfig(config.Extensions, deviceConfigRequest.ExtensionRequests,
                                Vulkan::PhysicalDeviceUtil::EnumerateDeviceExtensionProperties(device), "extension");
      return config;
    }
  }

  DeviceConfig BuildConfig(const VkPhysicalDevice device, const std::shared_ptr<DemoAppHostConfigVulkan>& customDemoAppHostConfig,
                           const ReadOnlySpan<Vulkan::FeatureRequest> hostExtensionRequests)
  {
    const auto deviceExtensionConfigControl =
      customDemoAppHostConfig ? customDemoAppHostConfig->GetDeviceExtensionConfigControl() : ConfigControl::Default;

    DeviceConfigRequest deviceConfig = BuildHostDeviceConfigRequest(hostExtensionRequests, deviceExtensionConfigControl);
    DeviceConfigRequest appDeviceConfig;
    if (customDemoAppHostConfig)
    {
      appDeviceConfig.ExtensionRequests = customDemoAppHostConfig->GetDeviceExtensionRequests();
    }

    ConfigUtil::MergeFeatureRequests(deviceConfig.ExtensionRequests, appDeviceConfig.ExtensionRequests);

    return PrepareConfig(device, deviceConfig);
  }
}
