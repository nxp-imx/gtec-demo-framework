/****************************************************************************************************************************************************
* Copyright 2017 NXP
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

#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/SafeType/InstanceCreateInfoCopy.hpp>
#include <FslBase/Log/Log.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>
#include <cstring>


namespace Fsl
{
  namespace Vulkan
  {
    namespace InstanceUtil
    {
      namespace
      {
        const int ENGINE_MAJOR = 1;
        const int ENGINE_MINOR = 0;
        const int ENGINE_PATCH = 0;
      }

      bool IsInstanceLayerAvailable(const std::vector<VkLayerProperties> layerProperties, const char*const pszLayerName)
      {
        if (pszLayerName == nullptr)
        {
          FSLLOG_DEBUG_WARNING("IsInstanceLayerAvailable called with nullptr, this always returns false.");
          return false;
        }

        for (uint32_t i = 0; i < layerProperties.size(); ++i)
        {
          if (std::strcmp(pszLayerName, layerProperties[i].layerName) == 0)
            return true;
        }
        return false;
      }


      bool IsInstanceLayersAvailable(const uint32_t layerCount, const char* const* enabledLayerNames)
      {
        if (layerCount == 0 || enabledLayerNames == nullptr)
          return false;

        const std::vector<VkLayerProperties> layerProperties = InstanceUtil::EnumerateInstanceLayerProperties();
        for (uint32_t extensionIndex = 0; extensionIndex < layerCount; ++extensionIndex)
        {
          if (!IsInstanceLayerAvailable(layerProperties, enabledLayerNames[extensionIndex]))
            return false;
        }
        return true;
      }


      bool IsInstanceExtensionAvailable(const std::vector<VkExtensionProperties> extensionProperties, const char*const pszExtensionName)
      {
        if (pszExtensionName == nullptr)
        {
          FSLLOG_DEBUG_WARNING("IsInstanceExtensionAvailable called with nullptr, this always returns false.");
          return false;
        }

        for (uint32_t i = 0; i < extensionProperties.size(); ++i)
        {
          if (std::strcmp(pszExtensionName, extensionProperties[i].extensionName) == 0)
            return true;
        }
        return false;
      }


      bool IsInstanceExtensionsAvailable(const uint32_t extensionCount, const char* const* enabledExtensionNames, const char*const pszLayerName)
      {
        if (extensionCount == 0 || enabledExtensionNames == nullptr )
          return false;

        const std::vector<VkExtensionProperties> extensionProperties = InstanceUtil::EnumerateInstanceExtensionProperties(pszLayerName);
        for (uint32_t extensionIndex = 0; extensionIndex < extensionCount; ++extensionIndex)
        {
          if (!IsInstanceExtensionAvailable(extensionProperties, enabledExtensionNames[extensionIndex]))
            return false;
        }
        return true;
      }


      RapidVulkan::Instance CreateInstance(const std::string& applicationName, const uint32_t applicationVersion,
                                           const uint32_t apiVersion, const VkInstanceCreateFlags flags, const uint32_t enabledLayerCount,
                                           const char* const* ppszEnabledLayerNames, const uint32_t enabledExtensionCount,
                                           const char* const* ppszEnabledExtensionNames, InstanceCreateInfoCopy* pInstanceCreateInfoCopy)
      {
        if (ppszEnabledLayerNames == nullptr && enabledLayerCount > 0)
          throw std::invalid_argument("enabledLayerCount can not be non-zero when no enabled extensions are supplied");

        if (ppszEnabledExtensionNames == nullptr && enabledExtensionCount > 0)
          throw std::invalid_argument("enabledExtensionCount can not be non-zero when no enabled extensions are supplied");

        if (enabledLayerCount > 0)
        {
          if (!IsInstanceLayersAvailable(enabledLayerCount, ppszEnabledLayerNames))
            throw NotSupportedException("Extension not available");
        }

        if (enabledExtensionCount > 0)
        {
          if (!IsInstanceExtensionsAvailable(enabledExtensionCount, ppszEnabledExtensionNames))
            throw NotSupportedException("Extension not available");
        }

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = applicationName.c_str();
        applicationInfo.applicationVersion = applicationVersion;
        applicationInfo.pEngineName = "DemoFramework";
        applicationInfo.engineVersion = VK_MAKE_VERSION(ENGINE_MAJOR, ENGINE_MINOR, ENGINE_PATCH);
        applicationInfo.apiVersion = apiVersion;

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.flags = flags;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = enabledLayerCount;
        instanceCreateInfo.ppEnabledLayerNames = ppszEnabledLayerNames;
        instanceCreateInfo.enabledExtensionCount = enabledExtensionCount;
        instanceCreateInfo.ppEnabledExtensionNames = ppszEnabledExtensionNames;

        if (pInstanceCreateInfoCopy != nullptr)
        {
          *pInstanceCreateInfoCopy = std::move(InstanceCreateInfoCopy(instanceCreateInfo));
        }
        return RapidVulkan::Instance(instanceCreateInfo);
      }


      PhysicalDeviceRecord GetPhysicalDevice(const VkInstance instance, const uint32_t index)
      {
        if (instance == VK_NULL_HANDLE)
          throw std::invalid_argument("Instance has to be valid");

        const std::vector<VkPhysicalDevice> physicalDevices = InstanceUtil::EnumeratePhysicalDevices(instance);
        if (index >= physicalDevices.size())
          throw std::invalid_argument("physical device index out of bounds");

        return PhysicalDeviceRecord(physicalDevices[index]);
      }


      std::vector<VkLayerProperties> EnumerateInstanceLayerProperties()
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkEnumerateInstanceLayerProperties(&count, nullptr), "failed to acquire the count");

        std::vector<VkLayerProperties> result(count);
        RAPIDVULKAN_CHECK2(vkEnumerateInstanceLayerProperties(&count, result.data()), "failed to enumerate layer properties");
        return result;
      }


      std::vector<VkExtensionProperties> EnumerateInstanceExtensionProperties(const char*const pszLayerName)
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkEnumerateInstanceExtensionProperties(pszLayerName, &count, nullptr), "failed to acquire the count");

        std::vector<VkExtensionProperties> result(count);
        RAPIDVULKAN_CHECK2(vkEnumerateInstanceExtensionProperties(pszLayerName, &count, result.data()), "failed to enumerate layer properties");
        return result;
      }


      std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(const VkInstance instance)
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkEnumeratePhysicalDevices(instance, &count, nullptr), "failed to acquire the count");

        std::vector<VkPhysicalDevice> result(count);
        RAPIDVULKAN_CHECK2(vkEnumeratePhysicalDevices(instance, &count, result.data()), "failed to enumerate devices");
        return result;
      }
    }
  }
}
