#ifndef FSLDEMOAPP_VULKAN_CONFIG_DEMOAPPHOSTCONFIGVULKANEX_HPP
#define FSLDEMOAPP_VULKAN_CONFIG_DEMOAPPHOSTCONFIGVULKANEX_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2017 Freescale Semiconductor, Inc.
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
#include <FslDemoApp/Base/Host/DemoAppHostConfigVulkan.hpp>
#include <FslUtil/Vulkan1_0/Misc/InstanceFeatureRequest.hpp>
#include <FslUtil/Vulkan1_0/Misc/PhysicalDeviceFeatureRequest.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <deque>

namespace Fsl
{
  class DemoAppHostConfigVulkanEx
    : public DemoAppHostConfigVulkan
  {
    std::deque<Vulkan::PhysicalDeviceFeatureRequest> m_physicalDeviceFeatureRequest;
    std::deque<Vulkan::InstanceFeatureRequest> m_instanceLayerRequest;
    std::deque<Vulkan::InstanceFeatureRequest> m_instanceExtensionRequest;
    ConfigControl m_layerConfigControl;
    ConfigControl m_extensionConfigControl;
  public:
    DemoAppHostConfigVulkanEx()
      : DemoAppHostConfigVulkan()
      , m_layerConfigControl(ConfigControl::Default)
      , m_extensionConfigControl(ConfigControl::Default)
    {
    }


    DemoAppHostConfigVulkanEx(const VulkanDemoAppMode demoAppMode)
      : DemoAppHostConfigVulkan(demoAppMode)
      , m_layerConfigControl(ConfigControl::Default)
      , m_extensionConfigControl(ConfigControl::Default)
    {
    }

    ConfigControl GetInstanceLayerConfigControl() const
    {
      return m_layerConfigControl;
    }

    ConfigControl GetInstanceExtensionConfigControl() const
    {
      return m_extensionConfigControl;
    }


    void SetInstanceLayerConfigControl(const ConfigControl value)
    {
      m_layerConfigControl = value;
    }


    void SetInstanceExtensionConfigControl(const ConfigControl value)
    {
      m_extensionConfigControl = value;
    }

    //! @brief Add a physical device feature
    void AddPhysicalDeviceFeatureRequest(const Vulkan::PhysicalDeviceFeature feature, const Vulkan::FeatureRequirement requirement)
    {
      if (feature == Vulkan::PhysicalDeviceFeature::Invalid || requirement == Vulkan::FeatureRequirement::Invalid)
        throw std::invalid_argument("Not a valid feature requirement");

      if (m_physicalDeviceFeatureRequest.end() !=
          std::find_if(m_physicalDeviceFeatureRequest.begin(), m_physicalDeviceFeatureRequest.end(),
          [feature](const Vulkan::PhysicalDeviceFeatureRequest& entry) { return (entry.Feature == feature); }))
      {
        throw UsageErrorException("feature already requested");
      }

      m_physicalDeviceFeatureRequest.push_back(Vulkan::PhysicalDeviceFeatureRequest(feature, requirement));
    }

    //! @brief Add a instance layer request
    void AddInstanceLayerRequest(const std::string& name, const Vulkan::FeatureRequirement requirement)
    {
      if (name.size() == 0 || requirement  == Vulkan::FeatureRequirement::Invalid)
        throw std::invalid_argument("Not a valid instance layer request");

      if (m_instanceLayerRequest.end() != std::find_if(m_instanceLayerRequest.begin(), m_instanceLayerRequest.end(),
                                                      [name](const Vulkan::InstanceFeatureRequest& entry) { return (entry.Name == name); }))
      {
        throw UsageErrorException("instance layer already requested: " + name);
      }

      m_instanceLayerRequest.push_back(Vulkan::InstanceFeatureRequest(name, requirement));
    }


    //! @brief Add a instance extension request
    void AddInstanceExtensionRequest(const std::string& name, const Vulkan::FeatureRequirement requirement)
    {
      if (name.size() == 0 || requirement == Vulkan::FeatureRequirement::Invalid)
        throw std::invalid_argument("Not a valid instance layer request");

      if (m_instanceExtensionRequest.end() != std::find_if(m_instanceExtensionRequest.begin(), m_instanceExtensionRequest.end(),
                                                          [name](const Vulkan::InstanceFeatureRequest& entry) { return (entry.Name == name); }))
      {
        throw UsageErrorException("instance extension already requested: " + name);
      }

      m_instanceExtensionRequest.push_back(Vulkan::InstanceFeatureRequest(name, requirement));
    }


    void ExtractDeviceRequiredFeatures(std::deque<Vulkan::PhysicalDeviceFeatureRequest>& rTarget) const
    {
      rTarget.clear();
      for (auto entry : m_physicalDeviceFeatureRequest)
        rTarget.push_back(entry);
    }


    const std::deque<Vulkan::InstanceFeatureRequest>& GetInstanceLayerRequests() const
    {
      return m_instanceLayerRequest;
    }


    const std::deque<Vulkan::InstanceFeatureRequest>& GetInstanceExtensionRequests() const
    {
      return m_instanceExtensionRequest;
    }


    bool HasDeviceRequiredFeatures() const
    {
      return m_physicalDeviceFeatureRequest.size() > 0;
    }

    //! @brief Called by the system before the VkDevice is created
    //virtual void BeforeInitDevice(const VkPhysicalDevice physicalDevice)
    //{
    //}

  };
}

#endif
