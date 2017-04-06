#ifndef FSLDEMOAPPVULKAN_CONFIG_DEMOAPPHOSTCONFIGVULKANEX_HPP
#define FSLDEMOAPPVULKAN_CONFIG_DEMOAPPHOSTCONFIGVULKANEX_HPP
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
#include <FslDemoApp/Host/DemoAppHostConfigVulkan.hpp>
#include <FslGraphicsVulkan1_0/Extend/PhysicalDeviceFeatureRequest.hpp>
#include <vulkan/vulkan.h>
#include <deque>

namespace Fsl
{
  class DemoAppHostConfigVulkanEx
    : public DemoAppHostConfigVulkan
  {
    std::deque<Vulkan::PhysicalDeviceFeatureRequest> m_physicalDeviceFeatureRequest;
  public:
    DemoAppHostConfigVulkanEx()
      : DemoAppHostConfigVulkan()
    {
    }


    DemoAppHostConfigVulkanEx(const VulkanDemoAppMode demoAppMode)
      : DemoAppHostConfigVulkan(demoAppMode)
    {
    }


    void AddPhysicalDeviceFeatureRequest(const Vulkan::PhysicalDeviceFeature feature, const Vulkan::FeatureRequirement requirement)
    {
      if (feature == Vulkan::PhysicalDeviceFeature::Invalid || requirement == Vulkan::FeatureRequirement::Invalid)
        throw std::invalid_argument("Not a valid feature requirement");

      m_physicalDeviceFeatureRequest.push_back(Vulkan::PhysicalDeviceFeatureRequest(feature, requirement));
    }


    void ExtractDeviceRequiredFeatures(std::deque<Vulkan::PhysicalDeviceFeatureRequest>& rTarget) const
    {
      rTarget.clear();
      for (auto entry : m_physicalDeviceFeatureRequest)
        rTarget.push_back(entry);
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
