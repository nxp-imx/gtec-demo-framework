#ifndef FSLDEMOHOST_VULKAN_CONFIG_DEMOAPPHOSTCONFIGVULKAN_HPP
#define FSLDEMOHOST_VULKAN_CONFIG_DEMOAPPHOSTCONFIGVULKAN_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
#include <FslBase/ITag.hpp>
#include <FslDemoApp/Base/Host/DemoAppHostConfig.hpp>
#include <FslDemoApp/Shared/Host/ConfigControl.hpp>
#include <FslDemoHost/Vulkan/Config/FeatureRequest.hpp>
#include <FslDemoHost/Vulkan/Config/PhysicalDeviceFeatureRequest.hpp>
#include <FslUtil/Vulkan1_0/SurfaceFormatInfo.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <deque>

namespace Fsl
{
  // enum class VulkanDemoAppMode
  //{
  //  Managed,
  //  Freestyle,
  //};


  class DemoAppHostConfigVulkan : public DemoAppHostConfig
  {
    // VulkanDemoAppMode m_demoAppMode;
    std::deque<Vulkan::SurfaceFormatInfo> m_preferedSurfaceFormats;
    std::deque<Vulkan::PhysicalDeviceFeatureRequest> m_physicalDeviceFeatureRequest;
    std::deque<Vulkan::FeatureRequest> m_instanceLayerRequest;
    std::deque<Vulkan::FeatureRequest> m_instanceExtensionRequest;
    std::deque<Vulkan::FeatureRequest> m_deviceExtensionRequest;
    ConfigControl m_layerConfigControl{ConfigControl::Default};
    ConfigControl m_extensionConfigControl{ConfigControl::Default};
    ConfigControl m_deviceExtensionConfigControl{ConfigControl::Default};

  public:
    DemoAppHostConfigVulkan();
    // DemoAppHostConfigVulkan(const VulkanDemoAppMode demoAppMode);


    // VulkanDemoAppMode GetDemoAppMode() const
    //{
    //  return m_demoAppMode;
    //}

    ConfigControl GetInstanceLayerConfigControl() const
    {
      return m_layerConfigControl;
    }

    ConfigControl GetInstanceExtensionConfigControl() const
    {
      return m_extensionConfigControl;
    }

    ConfigControl GetDeviceExtensionConfigControl() const
    {
      return m_deviceExtensionConfigControl;
    }

    void AddPreferedSurfaceFormat(const Vulkan::SurfaceFormatInfo& surfaceFormatInfo)
    {
      m_preferedSurfaceFormats.push_back(surfaceFormatInfo);
    }

    void SetInstanceLayerConfigControl(const ConfigControl value)
    {
      m_layerConfigControl = value;
    }


    void SetInstanceExtensionConfigControl(const ConfigControl value)
    {
      m_extensionConfigControl = value;
    }

    void SetDeviceExtensionConfigControl(const ConfigControl value)
    {
      m_deviceExtensionConfigControl = value;
    }


    //! @brief Add a physical device feature
    void AddPhysicalDeviceFeatureRequest(const Vulkan::PhysicalDeviceFeature feature, const Vulkan::FeatureRequirement requirement)
    {
      if (feature == Vulkan::PhysicalDeviceFeature::Invalid || requirement == Vulkan::FeatureRequirement::Invalid)
      {
        throw std::invalid_argument("Not a valid feature requirement");
      }

      if (m_physicalDeviceFeatureRequest.end() !=
          std::find_if(m_physicalDeviceFeatureRequest.begin(), m_physicalDeviceFeatureRequest.end(),
                       [feature](const Vulkan::PhysicalDeviceFeatureRequest& entry) { return (entry.Feature == feature); }))
      {
        throw UsageErrorException("feature already requested");
      }

      m_physicalDeviceFeatureRequest.emplace_back(feature, requirement);
    }

    //! @brief Add a instance layer request
    void AddInstanceLayerRequest(const std::string& name, const Vulkan::FeatureRequirement requirement)
    {
      if (name.empty() || requirement == Vulkan::FeatureRequirement::Invalid)
      {
        throw std::invalid_argument("Not a valid instance layer request");
      }

      if (m_instanceLayerRequest.end() != std::find_if(m_instanceLayerRequest.begin(), m_instanceLayerRequest.end(),
                                                       [name](const Vulkan::FeatureRequest& entry) { return (entry.Name == name); }))
      {
        throw UsageErrorException("instance layer already requested: " + name);
      }

      m_instanceLayerRequest.emplace_back(name, requirement);
    }


    //! @brief Add a instance extension request
    void AddInstanceExtensionRequest(const std::string& name, const Vulkan::FeatureRequirement requirement)
    {
      if (name.empty() || requirement == Vulkan::FeatureRequirement::Invalid)
      {
        throw std::invalid_argument("Not a valid instance layer request");
      }

      if (m_instanceExtensionRequest.end() != std::find_if(m_instanceExtensionRequest.begin(), m_instanceExtensionRequest.end(),
                                                           [name](const Vulkan::FeatureRequest& entry) { return (entry.Name == name); }))
      {
        throw UsageErrorException("instance extension already requested: " + name);
      }

      m_instanceExtensionRequest.emplace_back(name, requirement);
    }

    void AddDeviceExtensionRequest(const std::string& name, const Vulkan::FeatureRequirement requirement)
    {
      if (name.empty() || requirement == Vulkan::FeatureRequirement::Invalid)
      {
        throw std::invalid_argument("Not a valid instance layer request");
      }

      if (m_deviceExtensionRequest.end() != std::find_if(m_deviceExtensionRequest.begin(), m_deviceExtensionRequest.end(),
                                                         [name](const Vulkan::FeatureRequest& entry) { return (entry.Name == name); }))
      {
        throw UsageErrorException("device extension already requested: " + name);
      }

      m_deviceExtensionRequest.emplace_back(name, requirement);
    }

    void ExtractDeviceRequiredFeatures(std::deque<Vulkan::PhysicalDeviceFeatureRequest>& rTarget) const
    {
      rTarget.clear();
      for (auto entry : m_physicalDeviceFeatureRequest)
      {
        rTarget.push_back(entry);
      }
    }

    const std::deque<Vulkan::SurfaceFormatInfo>& GetPreferredSurfaceFormats() const
    {
      return m_preferedSurfaceFormats;
    }


    const std::deque<Vulkan::FeatureRequest>& GetInstanceLayerRequests() const
    {
      return m_instanceLayerRequest;
    }


    const std::deque<Vulkan::FeatureRequest>& GetInstanceExtensionRequests() const
    {
      return m_instanceExtensionRequest;
    }


    const std::deque<Vulkan::FeatureRequest>& GetDeviceExtensionRequests() const
    {
      return m_deviceExtensionRequest;
    }


    bool HasDeviceRequiredFeatures() const
    {
      return !m_physicalDeviceFeatureRequest.empty();
    }
  };
}

#endif
