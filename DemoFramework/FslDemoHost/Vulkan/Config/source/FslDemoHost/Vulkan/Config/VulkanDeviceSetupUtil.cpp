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

#include <FslDemoHost/Vulkan/Config/VulkanDeviceSetupUtil.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoHost/Vulkan/Config/PhysicalDeviceFeatureRequestUtil.hpp>
#include <FslDemoHost/Vulkan/Config/IVulkanDeviceCreationCustomizer.hpp>
#include <FslUtil/Vulkan1_0/Util/DeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/QueueUtil.hpp>
#include <array>
#include <deque>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    VulkanDeviceSetup VulkanDeviceSetupUtil::CreateSetup(const VUPhysicalDeviceRecord& physicalDevice, const VkSurfaceKHR surface,
                                                         const std::deque<PhysicalDeviceFeatureRequest>& featureRequestDeque,
                                                         const ReadOnlySpan<const char*>& extensions,
                                                         IVulkanDeviceCreationCustomizer* const pDeviceCreationCustomizer)
    {
      {
        const auto deviceQueueFamilyProperties = PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(physicalDevice.Device);
        auto supportFilter =
          PhysicalDeviceKHRUtil::GetPhysicalDeviceSurfaceSupportKHR(physicalDevice.Device, surface, deviceQueueFamilyProperties.size());

        const uint32_t queueFamilyIndex = QueueUtil::GetQueueFamilyIndex(deviceQueueFamilyProperties, VK_QUEUE_GRAPHICS_BIT, 0, &supportFilter);

        std::array<float, 1> queuePriorities = {0.0f};
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.flags = 0;
        deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        deviceQueueCreateInfo.queueCount = static_cast<uint32_t>(queuePriorities.size());
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = UncheckedNumericCast<uint32_t>(extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

        // Allow the app to tweak the next pointer
        if (pDeviceCreationCustomizer != nullptr)
        {
          pDeviceCreationCustomizer->Configure(physicalDevice.Device);
          deviceCreateInfo.pNext = pDeviceCreationCustomizer->GetVkDeviceCreateInfoNextPointer();
        }

        // Lookup the user defines feature requirements and set them
        VkPhysicalDeviceFeatures deviceFeatures{};
        if (!featureRequestDeque.empty())
        {
          PhysicalDeviceFeatureRequestUtil::ApplyFeatureRequirements(deviceFeatures, featureRequestDeque, physicalDevice.Device);
          deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        }

        VulkanDeviceSetup setup;
        setup.Device.Reset(physicalDevice.Device, deviceCreateInfo);
        setup.DeviceFeatures = deviceFeatures;
        setup.DeviceCreateInfo = std::make_shared<Vulkan::DeviceCreateInfoCopy>(deviceCreateInfo);
        setup.DeviceQueueRecord = DeviceUtil::GetDeviceQueue(setup.Device.Get(), queueFamilyIndex, 0);
        return setup;
      }
    }
  }
}
