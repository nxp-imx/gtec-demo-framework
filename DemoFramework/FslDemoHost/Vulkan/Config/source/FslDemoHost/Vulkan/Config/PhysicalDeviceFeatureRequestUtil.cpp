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

#include <FslDemoHost/Vulkan/Config/PhysicalDeviceFeatureRequestUtil.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoHost/Vulkan/Config/PhysicalDeviceFeatureUtil.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    namespace
    {
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
    }

    void PhysicalDeviceFeatureRequestUtil::ApplyFeatures(VkPhysicalDeviceFeatures& rPhysicalDeviceFeatures,
                                                         const std::deque<Vulkan::PhysicalDeviceFeatureRequest>& requiredFeatures)
    {
      for (auto entry : requiredFeatures)
      {
        PhysicalDeviceFeatureUtil::Set(rPhysicalDeviceFeatures, entry.Feature, VK_TRUE);
      }
    }


    void PhysicalDeviceFeatureRequestUtil::ApplyFeatureRequirements(VkPhysicalDeviceFeatures& rPhysicalDeviceFeatures,
                                                                    const std::deque<Vulkan::PhysicalDeviceFeatureRequest>& requiredFeatures,
                                                                    const VkPhysicalDevice physicalDevice)
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
}
