#ifndef FSLUTIL_VULKAN1_0_UNITTEST_UTIL_FSLUTIL_VULKAN1_0_UNITTEST_TESTFIXTUREFSLUTIL_VULKAN1_0_COMPUTE_HPP
#define FSLUTIL_VULKAN1_0_UNITTEST_UTIL_FSLUTIL_VULKAN1_0_UNITTEST_TESTFIXTUREFSLUTIL_VULKAN1_0_COMPUTE_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/UnitTest/Helper/Common.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/QueueUtil.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <RapidVulkan/Instance.hpp>
#include <fmt/format.h>
#include <iostream>
#include <string>

class TestFixtureFslUtil_Vulkan1_0_Compute : public ::testing::TestWithParam<VkBufferUsageFlags>
{
  std::string m_reason;

protected:
  RapidVulkan::Instance m_instance;
  Fsl::Vulkan::VUDevice m_device;

public:
  TestFixtureFslUtil_Vulkan1_0_Compute()
  {
    try
    {
      m_instance = TryCreateInstance();
      if (m_instance.IsValid())
      {
        VkPhysicalDevice physicalDevice = FindDevice(m_instance.Get());

        m_device = CreateDevice(physicalDevice);
      }
    }
    catch (const std::exception& ex)
    {
      SkipTest(fmt::format("Failed to create vulkan instance: {}", ex.what()));
      m_instance.Reset();
    }
  }

  ~TestFixtureFslUtil_Vulkan1_0_Compute() override = default;

protected:
  void SkipTest(const std::string& testName)
  {
    std::cout << "\nSkipped '" << testName << "'";
    if (!m_instance.IsValid())
    {
      std::cout << " (cant create instance : '" << m_reason << "')\n";
    }
    else
    {
      std::cout << "\n";
    }
  }

private:
  RapidVulkan::Instance TryCreateInstance()
  {
    try
    {
      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "TestFixtureFslUtil_Vulkan1_0_TestInstance";
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "No Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_0;

      VkInstanceCreateInfo instanceCreateInfo{};
      instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      instanceCreateInfo.pApplicationInfo = &appInfo;

      return RapidVulkan::Instance(instanceCreateInfo);
    }
    catch (const std::exception& ex)
    {
      m_reason = ex.what();
      return {};
    }
  }

  static Fsl::Vulkan::VUDevice CreateDevice(const VkPhysicalDevice physicalDevice)
  {
    const auto deviceQueueFamilyProperties = Fsl::Vulkan::PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(physicalDevice);
    const uint32_t queueFamilyIndex = Fsl::Vulkan::QueueUtil::GetQueueFamilyIndex(deviceQueueFamilyProperties, VK_QUEUE_COMPUTE_BIT, 0, nullptr);

    const float queuePriorities = 0.0f;
    VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = &queuePriorities;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

    return {physicalDevice, deviceCreateInfo};
  }

  static VkPhysicalDevice FindDevice(const VkInstance instance)
  {
    const auto physicalDevices = Fsl::Vulkan::InstanceUtil::EnumeratePhysicalDevices(instance);
    if (physicalDevices.empty())
    {
      throw Fsl::NotSupportedException("No physical devices found");
    }

    // Locate a device that support compute
    for (const auto& physicalDevice : physicalDevices)
    {
      VkPhysicalDeviceProperties deviceProperties{};
      vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

      auto queueProperties = Fsl::Vulkan::PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(physicalDevice);
      vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

      for (auto& queue : queueProperties)
      {
        if ((queue.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0u)
        {
          return physicalDevice;
        }
      }
    }
    throw Fsl::NotSupportedException("No physical devices that supports compute found");
  }
};

#endif
