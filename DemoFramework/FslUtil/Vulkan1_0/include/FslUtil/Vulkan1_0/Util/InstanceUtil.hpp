#ifndef FSLUTIL_VULKAN1_0_UTIL_INSTANCEUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_INSTANCEUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/Instance.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl::Vulkan
{
  class InstanceCreateInfoCopy;

  namespace InstanceUtil
  {
    bool IsInstanceLayersAvailable(const uint32_t layerCount, const char* const* enabledLayerNames);
    bool IsInstanceExtensionsAvailable(const uint32_t extensionCount, const char* const* enabledExtensionNames,
                                       const char* const pszLayerName = nullptr);

    //! @param pInstanceCreateInfoCopy if not null then this will be filled with a copy of the instance create info.
    RapidVulkan::Instance CreateInstance(const std::string& applicationName, const uint32_t applicationVersion, const uint32_t apiVersion,
                                         const VkInstanceCreateFlags flags, const uint32_t enabledLayerCount,
                                         const char* const* ppszEnabledLayerNames, const uint32_t enabledExtensionCount,
                                         const char* const* ppszEnabledExtensionNames, InstanceCreateInfoCopy* pInstanceCreateInfoCopy = nullptr);


    //! @param pInstanceCreateInfoCopy if not null then this will be filled with a copy of the instance create info.
    inline RapidVulkan::Instance CreateInstance(const std::string& applicationName, const uint32_t applicationVersion, const uint32_t apiVersion,
                                                const VkInstanceCreateFlags flags, const std::vector<const char*>& enabledLayerNames,
                                                const std::vector<const char*>& enabledExtensionNames,
                                                InstanceCreateInfoCopy* pInstanceCreateInfoCopy = nullptr)
    {
      return CreateInstance(applicationName, applicationVersion, apiVersion, flags, static_cast<uint32_t>(enabledLayerNames.size()),
                            enabledLayerNames.data(), static_cast<uint32_t>(enabledExtensionNames.size()), enabledExtensionNames.data(),
                            pInstanceCreateInfoCopy);
    }


    VkPhysicalDevice GetPhysicalDevice(const VkInstance instance, const uint32_t index);

    //! @brief Enumerate the instance layer properties
    std::vector<VkLayerProperties> EnumerateInstanceLayerProperties();

    //! @brief Enumerate
    std::vector<VkExtensionProperties> EnumerateInstanceExtensionProperties(const char* const pszLayerName = nullptr);

    //! @brief Enumerate the physical devices
    std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(const VkInstance instance);
  }
}

#endif
