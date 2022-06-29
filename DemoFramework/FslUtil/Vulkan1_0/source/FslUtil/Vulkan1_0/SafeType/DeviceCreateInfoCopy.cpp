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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/SafeType/DeviceCreateInfoCopy.hpp>

namespace Fsl::Vulkan
{
  //! @brief Move assignment operator
  DeviceCreateInfoCopy& DeviceCreateInfoCopy::operator=(DeviceCreateInfoCopy&& other) noexcept
  {
    if (this != &other)
    {
      // Claim ownership here
      m_value = other.m_value;
      m_queueCreateInfos = std::move(other.m_queueCreateInfos);
      m_enabledLayerNames = std::move(other.m_enabledLayerNames);
      m_enabledExtensionNames = std::move(other.m_enabledExtensionNames);
      m_enabledFeatures = other.m_enabledFeatures;

      // Remove the data from other
      other.m_value = VkDeviceCreateInfo{};
      PatchPointers();
    }
    return *this;
  }

  //! @brief Move constructor
  //! Transfer ownership from other to this
  DeviceCreateInfoCopy::DeviceCreateInfoCopy(DeviceCreateInfoCopy&& other) noexcept
    : m_value(other.m_value)
    , m_queueCreateInfos(std::move(other.m_queueCreateInfos))
    , m_enabledLayerNames(std::move(other.m_enabledLayerNames))
    , m_enabledExtensionNames(std::move(other.m_enabledExtensionNames))
    , m_enabledFeatures(other.m_enabledFeatures)
  {
    // Remove the data from other
    other.m_value = VkDeviceCreateInfo{};
    PatchPointers();
  }


  DeviceCreateInfoCopy::DeviceCreateInfoCopy()
    : m_value()
    , m_enabledFeatures()
  {
  }


  DeviceCreateInfoCopy::DeviceCreateInfoCopy(const VkDeviceCreateInfo& value)
    : m_value(value)
    , m_queueCreateInfos(value.pQueueCreateInfos, value.queueCreateInfoCount)
    , m_enabledLayerNames(value.ppEnabledLayerNames, value.enabledLayerCount)
    , m_enabledExtensionNames(value.ppEnabledExtensionNames, value.enabledExtensionCount)
    , m_enabledFeatures(value.pEnabledFeatures != nullptr ? *value.pEnabledFeatures : VkPhysicalDeviceFeatures{})
  {
    // Now use the safe copied values instead
    PatchPointers();
    m_value.pNext = nullptr;
    if (Fsl::LogConfig::GetLogLevel() >= LogType::Verbose)
    {
      FSLLOG3_DEBUG_WARNING_IF(value.pNext != nullptr, "DeviceCreateInfoCopy always stores a nullptr for pNext");
    }
  }


  void DeviceCreateInfoCopy::PatchPointers() noexcept
  {
    m_value.ppEnabledLayerNames = m_value.ppEnabledLayerNames != nullptr ? m_enabledLayerNames.data() : nullptr;
    m_value.ppEnabledExtensionNames = m_value.ppEnabledExtensionNames != nullptr ? m_enabledExtensionNames.data() : nullptr;
    m_value.pEnabledFeatures = (m_value.pEnabledFeatures != nullptr ? &m_enabledFeatures : nullptr);
  }
}
