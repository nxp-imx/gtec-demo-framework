#ifndef FSLDEMOHOST_VULKAN_CONFIG_CONFIGUTIL_HPP
#define FSLDEMOHOST_VULKAN_CONFIG_CONFIGUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoHost/Vulkan/Config/FeatureRequest.hpp>
#include <FslDemoHost/Vulkan/Config/FeatureRequirement.hpp>
#include <FslDemoHost/Vulkan/Config/OptionUserChoice.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <deque>

namespace Fsl::ConfigUtil
{
  inline Vulkan::FeatureRequirement ToFeatureRequirement(const OptionUserChoice userChoice, const Vulkan::FeatureRequirement defaultSetting)
  {
    switch (userChoice)
    {
    case OptionUserChoice::Default:
      return defaultSetting;
    case OptionUserChoice::On:
      return Vulkan::FeatureRequirement::Mandatory;
    case OptionUserChoice::Off:
      return Vulkan::FeatureRequirement::Invalid;
    default:
      throw NotSupportedException("Unsupported user choice option");
    }
  }

  inline bool TryReplace(std::deque<Vulkan::FeatureRequest>& rRequests, const Vulkan::FeatureRequest& featureRequest)
  {
    auto itrFind = std::find_if(rRequests.begin(), rRequests.end(),
                                [featureRequest](const Vulkan::FeatureRequest& entry) { return (entry.Name == featureRequest.Name); });
    if (itrFind == rRequests.end())
    {
      return false;
    }

    *itrFind = featureRequest;
    return true;
  }


  inline void MergeFeatureRequests(std::deque<Vulkan::FeatureRequest>& rRequests, const std::deque<Vulkan::FeatureRequest>& newRequests)
  {
    for (const auto& entry : newRequests)
    {
      if (!TryReplace(rRequests, entry))
      {
        rRequests.push_back(entry);
      }
    }
  }


  inline const char* GetEntryName(const VkLayerProperties& entry)
  {
    return entry.layerName;
  }


  inline const char* GetEntryName(const VkExtensionProperties& entry)
  {
    return entry.extensionName;
  }


  template <typename T>
  void PrepareConfig(std::deque<std::string>& rDst, const std::deque<Vulkan::FeatureRequest>& requests, const std::vector<T>& properties,
                     const char* const pszDesc)
  {
    FSLLOG3_VERBOSE("Examining {} requests", pszDesc);
    for (const auto& request : requests)
    {
      auto itrFind = std::find_if(properties.begin(), properties.end(), [request](const T& entry) { return (GetEntryName(entry) == request.Name); });

      // If we didnt find the extension then check for the alternative name
      if (itrFind == properties.end() && request.AlternativeNames)
      {
        auto itrAlternativeName = request.AlternativeNames->begin();
        while (itrAlternativeName != request.AlternativeNames->end())
        {
          const std::string alternativeName = *itrAlternativeName;
          itrFind = std::find_if(properties.begin(), properties.end(),
                                 [alternativeName](const T& entry) { return (GetEntryName(entry) == alternativeName); });
          if (itrFind != properties.end())
          {
            // Found, so just push it
            rDst.push_back(alternativeName);
            FSLLOG3_VERBOSE("- {} found", alternativeName, request.Name);
            return;
          }
          ++itrAlternativeName;
        }
      }

      if (itrFind != properties.end())
      {
        // Found, so just push it
        rDst.push_back(request.Name);
        FSLLOG3_VERBOSE("- {} found", request.Name);
      }
      else
      {
        // Not found, so lets see if its mandatory or optional
        switch (request.Requirement)
        {
        case Vulkan::FeatureRequirement::Mandatory:
          FSLLOG3_VERBOSE("- {} was mandatory and not found.", request.Name);
          if (request.AlternativeNames)
          {
            for (const auto& alternativeName : *request.AlternativeNames)
            {
              FSLLOG3_VERBOSE("  - Neither was {}.", alternativeName);
            }
          }
          throw NotSupportedException(std::string("The ") + std::string(pszDesc) + std::string(" is not supported: ") + request.Name);
        case Vulkan::FeatureRequirement::Optional:
          FSLLOG3_VERBOSE("- {} was optional and not found.", request.Name);
          if (request.AlternativeNames)
          {
            for (const auto& alternativeName : *request.AlternativeNames)
            {
              FSLLOG3_VERBOSE("  - Neither was {}.", alternativeName);
            }
          }
          break;
        default:
          throw NotSupportedException("The FeatureRequirement type is unsupported");
        }
      }
    }
  }


  inline void FilterFeatureByName(std::deque<Vulkan::FeatureRequest>& rLayerRequests, const std::string& name)
  {
    auto itrFind =
      std::find_if(rLayerRequests.begin(), rLayerRequests.end(), [name](const Vulkan::FeatureRequest& entry) { return (entry.Name == name); });
    if (itrFind != rLayerRequests.end())
    {
      FSLLOG3_VERBOSE("Removing '{}' due to command line option.", name);
      rLayerRequests.erase(itrFind);
    }
  }
}

#endif
