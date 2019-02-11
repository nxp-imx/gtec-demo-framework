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

#include "VulkanWindowDemoApp.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Log/Math/LogExtent2D.hpp>
#include <FslDemoApp/Base/Host/DemoAppHostConfig.hpp>
#include <FslDemoApp/Base/Host/DemoAppHostConfigWindow.hpp>
#include <FslDemoApp/Base/Service/Host/IHostInfo.hpp>
#include <FslDemoHost/Base/Service/WindowHost/IWindowHostInfo.hpp>
#include <FslDemoHost/Vulkan/Config/DemoAppHostConfigVulkan.hpp>
#include <FslUtil/Vulkan1_0/Log/All.hpp>
#include <FslDemoHost/Vulkan/Config/PhysicalDeviceFeatureUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/DeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/QueueUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindow.hpp>
#include "VulkanWindowSystem.hpp"
#include "VulkanWindowSystemHelper.hpp"
#include <array>
#include <string>
#include <sstream>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    std::string ToString(const Extent2D& extent)
    {
      std::stringstream stream;
      stream << extent;
      return stream.str();
    }

    std::string ToString(const VkExtent2D& extent)
    {
      std::stringstream stream;
      stream << extent;
      return stream.str();
    }


    void FilterFeatures(std::deque<Vulkan::PhysicalDeviceFeatureRequest>& rFilteredFeatures,
                        const std::deque<Vulkan::PhysicalDeviceFeatureRequest>& requiredFeatures,
                        const VkPhysicalDeviceFeatures& physicalDeviceFeatures)
    {
      rFilteredFeatures.clear();
      for (auto entry : requiredFeatures)
      {
        // Look at the current value
        auto value = PhysicalDeviceFeatureUtil::Get(physicalDeviceFeatures, entry.Feature);
        if (value != VK_FALSE)
        {
          rFilteredFeatures.push_back(entry);
        }
        else
        {
          switch (entry.Requirement)
          {
          case FeatureRequirement::Mandatory:
            throw NotSupportedException("The requested physical device feature was not supported: " +
                                        std::string(PhysicalDeviceFeatureUtil::ToString(entry.Feature)));
          case FeatureRequirement::Optional:
            FSLLOG_DEBUG("VulkanPhysicalDevice optional feature '" << PhysicalDeviceFeatureUtil::ToString(entry.Feature) << "' is unavailable.");
            break;
          default:
            FSLLOG_WARNING("Invalid requirement for feature: " << PhysicalDeviceFeatureUtil::ToString(entry.Feature));
            break;
          }
        }
      }
    }

    void ApplyFeatures(VkPhysicalDeviceFeatures& rPhysicalDeviceFeatures, const std::deque<Vulkan::PhysicalDeviceFeatureRequest>& requiredFeatures)
    {
      for (auto entry : requiredFeatures)
      {
        PhysicalDeviceFeatureUtil::Set(rPhysicalDeviceFeatures, entry.Feature, VK_TRUE);
      }
    }


    void ApplyFeatureRequirements(VkPhysicalDeviceFeatures& rPhysicalDeviceFeatures,
                                  const std::deque<Vulkan::PhysicalDeviceFeatureRequest>& requiredFeatures, const VkPhysicalDevice physicalDevice)
    {
      // Query the device
      VkPhysicalDeviceFeatures physicalDeviceFeatures{};
      vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

      // Then filter out all unavailable optional features
      std::deque<Vulkan::PhysicalDeviceFeatureRequest> filteredFeatures;
      FilterFeatures(filteredFeatures, requiredFeatures, physicalDeviceFeatures);

      ApplyFeatures(rPhysicalDeviceFeatures, filteredFeatures);
    }
  }

  VulkanWindowDemoApp::VulkanWindowDemoApp(const DemoAppConfig& demoAppConfig)
    : DemoAppWindow(demoAppConfig)
    , m_instance()
    , m_surface(VK_NULL_HANDLE)
    , m_deviceActiveFeatures{}
    , m_deviceCreateInfo(std::make_shared<Vulkan::DeviceCreateInfoCopy>())
  {
    auto hostInfo = demoAppConfig.DemoServiceProvider.Get<IHostInfo>();
    auto appHostConfigBase = hostInfo->TryGetAppHostConfig();
    if (!appHostConfigBase)
    {
      throw std::runtime_error("The AppHostConfig was not set");
    }
    auto appHostConfig = std::dynamic_pointer_cast<DemoAppHostConfigWindow>(appHostConfigBase);
    if (!appHostConfig)
    {
      throw std::runtime_error("The AppHostConfig was not of the expected type");
    }

    auto windowHostInfo = demoAppConfig.DemoServiceProvider.Get<IWindowHostInfo>();
    auto windowSystem = VulkanWindowSystemHelper::GetWindowSystem(windowHostInfo);
    m_nativeWindow = VulkanWindowSystemHelper::GetActiveWindow(windowHostInfo);


    // TODO: get a shared ptr to the instance instead
    m_instance = windowSystem->GetInstance();
    m_instanceCreateInfo = windowSystem->GetInstanceCreateInfo();
    m_surface = m_nativeWindow->GetVulkanSurface();

    {
      m_physicalDevice = windowSystem->GetPhysicalDevice();

      const auto deviceQueueFamilyProperties = PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(m_physicalDevice.Device);
      auto supportFilter =
        PhysicalDeviceKHRUtil::GetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice.Device, m_surface, deviceQueueFamilyProperties.size());

      const uint32_t queueFamilyIndex = QueueUtil::GetQueueFamilyIndex(deviceQueueFamilyProperties, VK_QUEUE_GRAPHICS_BIT, 0, &supportFilter);

      const float queuePriorities[1] = {0.0f};
      VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
      deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      deviceQueueCreateInfo.flags = 0;
      deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
      deviceQueueCreateInfo.queueCount = 1;
      deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

      std::array<const char*, 1> extensions = {"VK_KHR_swapchain"};

      VkDeviceCreateInfo deviceCreateInfo{};
      deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      deviceCreateInfo.queueCreateInfoCount = 1;
      deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
      deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
      deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

      // Lookup the user defines feature requirements and set them
      auto demoAppHostConfigVulkan = appHostConfig->TryGetCustomDemoAppHostConfig<DemoAppHostConfigVulkan>();
      if (demoAppHostConfigVulkan && demoAppHostConfigVulkan->HasDeviceRequiredFeatures())
      {
        std::deque<Vulkan::PhysicalDeviceFeatureRequest> requiredFeatures;
        demoAppHostConfigVulkan->ExtractDeviceRequiredFeatures(requiredFeatures);

        ApplyFeatureRequirements(m_deviceActiveFeatures, requiredFeatures, m_physicalDevice.Device);
        deviceCreateInfo.pEnabledFeatures = &m_deviceActiveFeatures;
      }

      m_device.Reset(m_physicalDevice.Device, deviceCreateInfo);
      *m_deviceCreateInfo = std::move(Vulkan::DeviceCreateInfoCopy(deviceCreateInfo));

      m_deviceQueue = DeviceUtil::GetDeviceQueue(m_device.Get(), queueFamilyIndex, 0);
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice.Device, m_surface, &surfaceCapabilities));
    if (ConvertUtil::Convert(surfaceCapabilities.currentExtent) != GetScreenExtent())
    {
      throw InitFailedException("The Vulkan surface extent did not match the window extent. Screen: " + ToString(GetScreenExtent()) +
                                " vs surface: " + ToString(surfaceCapabilities.currentExtent));
    }
  }


  VulkanWindowDemoApp::~VulkanWindowDemoApp()
  {
    SafeWaitForDeviceIdle();
  }


  void VulkanWindowDemoApp::OnDestroy()
  {
    SafeWaitForDeviceIdle();
  }

  void VulkanWindowDemoApp::SafeWaitForDeviceIdle() noexcept
  {
    try
    {
      if (m_device.IsValid())
      {
        m_device.DeviceWaitIdle();
      }
    }
    catch (const std::exception& ex)
    {
      // We log and swallow it since destructors are not allowed to throw
      FSLLOG_ERROR("DeviceWaitIdle, threw exception: " << ex.what());
    }
  }
}
