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

#include <FslDemoHost/Vulkan/Config/InstanceConfigUtil.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Base/Host/DemoAppHostConfigWindow.hpp>
#include <FslDemoApp/Base/Host/DemoHostCustomWindowSystemSetup.hpp>
#include <FslDemoApp/Base/Service/Options/IOptions.hpp>
#include <FslDemoApp/Base/Service/Options/Options.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/SafeType/InstanceCreateInfoCopy.hpp>
#include <array>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  using namespace Vulkan;

  namespace InstanceConfigUtil
  {
    namespace
    {
#ifdef NDEBUG
      constexpr bool LOCAL_VALIDATION_LAYER_ENABLED = false;
#else
      constexpr static bool LOCAL_VALIDATION_LAYER_ENABLED = true;
#endif


      const auto CONFIG_VALIDATION_LAYER_OLD_NAME = "VK_LAYER_LUNARG_standard_validation";
      const auto CONFIG_VALIDATION_LAYER_NAME = "VK_LAYER_KHRONOS_validation";
      const auto CONFIG_API_DUMP_LAYER_NAME = "VK_LAYER_LUNARG_api_dump";

      struct InstanceConfigRequest
      {
        std::deque<Vulkan::InstanceFeatureRequest> LayerRequests;
        std::deque<Vulkan::InstanceFeatureRequest> ExtensionRequests;
      };

      Vulkan::FeatureRequirement ToFeatureRequirement(const OptionUserChoice userChoice, const Vulkan::FeatureRequirement defaultSetting)
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

      void AppendUserChoice(std::deque<Vulkan::InstanceFeatureRequest>& rLayerRequests, const OptionUserChoice userChoice, const bool defaultChoice,
                            const char* const pszLayerName)
      {
        assert(pszLayerName != nullptr);

        const auto buildDebugLayerRequirement = defaultChoice ? Vulkan::FeatureRequirement::Optional : Vulkan::FeatureRequirement::Invalid;
        const auto debugLayerRequirement = ToFeatureRequirement(userChoice, buildDebugLayerRequirement);
        if (debugLayerRequirement != Vulkan::FeatureRequirement::Invalid)
        {
          rLayerRequests.emplace_back(pszLayerName, debugLayerRequirement);
        }
      }

      void AppendUserChoice(std::deque<Vulkan::InstanceFeatureRequest>& rLayerRequests, const OptionUserChoice userChoice, const bool defaultChoice,
                            const char* const pszLayerName, const char* const pszAlternativeLayerName)
      {
        assert(pszLayerName != nullptr);
        assert(pszAlternativeLayerName != nullptr);

        const auto buildDebugLayerRequirement = defaultChoice ? Vulkan::FeatureRequirement::Optional : Vulkan::FeatureRequirement::Invalid;
        const auto debugLayerRequirement = ToFeatureRequirement(userChoice, buildDebugLayerRequirement);
        if (debugLayerRequirement != Vulkan::FeatureRequirement::Invalid)
        {
          auto alternativeNames = std::make_shared<std::vector<std::string>>();
          alternativeNames->push_back(pszAlternativeLayerName);
          rLayerRequests.emplace_back(pszLayerName, debugLayerRequirement, alternativeNames);
        }
      }


      InstanceConfigRequest BuildHostInstanceConfigRequest(const std::string& khrSurfaceExtensionName, const InstanceUserChoice& instanceUserChoice,
                                                           const ConfigControl /*instanceLayerConfigControl*/,
                                                           const ConfigControl /*instanceExtensionConfigControl*/)
      {
        InstanceConfigRequest instanceConfig;

        AppendUserChoice(instanceConfig.LayerRequests, instanceUserChoice.UserChoiceApiDump, false, CONFIG_API_DUMP_LAYER_NAME);
        AppendUserChoice(instanceConfig.LayerRequests, instanceUserChoice.ValidationLayer, LOCAL_VALIDATION_LAYER_ENABLED,
                         CONFIG_VALIDATION_LAYER_NAME, CONFIG_VALIDATION_LAYER_OLD_NAME);

        // Always add the SURFACE extensions the extension ConfigControl does not modify this
        instanceConfig.ExtensionRequests.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME, Vulkan::FeatureRequirement::Mandatory);
        instanceConfig.ExtensionRequests.emplace_back(khrSurfaceExtensionName, Vulkan::FeatureRequirement::Mandatory);

        // if (instanceLayerConfigControl == ConfigControl::Overwrite)
        //{

        //}

        // if (instanceExtensionConfigControl == ConfigControl::Overwrite)
        //{

        //}
        return instanceConfig;
      }


      bool TryReplace(std::deque<Vulkan::InstanceFeatureRequest>& rRequests, const Vulkan::InstanceFeatureRequest& featureRequest)
      {
        auto itrFind = std::find_if(rRequests.begin(), rRequests.end(),
                                    [featureRequest](const Vulkan::InstanceFeatureRequest& entry) { return (entry.Name == featureRequest.Name); });
        if (itrFind == rRequests.end())
        {
          return false;
        }

        *itrFind = featureRequest;
        return true;
      }


      void MergeInstanceFeatureRequests(std::deque<Vulkan::InstanceFeatureRequest>& rRequests,
                                        const std::deque<Vulkan::InstanceFeatureRequest>& newRequests)
      {
        for (const auto& entry : newRequests)
        {
          if (!TryReplace(rRequests, entry))
          {
            rRequests.push_back(entry);
          }
        }
      }


      const char* GetEntryName(const VkLayerProperties& entry)
      {
        return entry.layerName;
      }


      const char* GetEntryName(const VkExtensionProperties& entry)
      {
        return entry.extensionName;
      }


      template <typename T>
      void PrepareConfig(std::deque<std::string>& rDst, const std::deque<Vulkan::InstanceFeatureRequest>& requests, const std::vector<T>& properties,
                         const char* const pszDesc)
      {
        FSLLOG3_VERBOSE("Examining {} requests", pszDesc);
        for (const auto& request : requests)
        {
          auto itrFind =
            std::find_if(properties.begin(), properties.end(), [request](const T& entry) { return (GetEntryName(entry) == request.Name); });

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


      void FilterFeatureByName(std::deque<Vulkan::InstanceFeatureRequest>& rLayerRequests, const std::string& name)
      {
        auto itrFind = std::find_if(rLayerRequests.begin(), rLayerRequests.end(),
                                    [name](const Vulkan::InstanceFeatureRequest& entry) { return (entry.Name == name); });
        if (itrFind != rLayerRequests.end())
        {
          FSLLOG3_VERBOSE("Removing '{}' due to command line option.", name);
          rLayerRequests.erase(itrFind);
        }
      }


      InstanceConfig PrepareConfig(const InstanceConfigRequest& instanceConfigRequest)
      {
        InstanceConfig config;
        PrepareConfig(config.Layers, instanceConfigRequest.LayerRequests, InstanceUtil::EnumerateInstanceLayerProperties(), "layer");
        PrepareConfig(config.Extensions, instanceConfigRequest.ExtensionRequests, InstanceUtil::EnumerateInstanceExtensionProperties(), "extension");
        return config;
      }
    }

    InstanceConfig BuildInstanceConfig(const std::string& khrSurfaceExtensionName, const InstanceUserChoice& instanceUserChoice,
                                       const std::shared_ptr<DemoAppHostConfigVulkan>& customDemoAppHostConfig)
    {
      const auto instanceLayerConfigControl =
        customDemoAppHostConfig ? customDemoAppHostConfig->GetInstanceLayerConfigControl() : ConfigControl::Default;
      const auto instanceExtensionConfigControl =
        customDemoAppHostConfig ? customDemoAppHostConfig->GetInstanceExtensionConfigControl() : ConfigControl::Default;

      InstanceConfigRequest instanceConfig =
        BuildHostInstanceConfigRequest(khrSurfaceExtensionName, instanceUserChoice, instanceLayerConfigControl, instanceExtensionConfigControl);
      InstanceConfigRequest appInstanceConfig;
      if (customDemoAppHostConfig)
      {
        appInstanceConfig.LayerRequests = customDemoAppHostConfig->GetInstanceLayerRequests();
        appInstanceConfig.ExtensionRequests = customDemoAppHostConfig->GetInstanceExtensionRequests();
      }

      MergeInstanceFeatureRequests(instanceConfig.LayerRequests, appInstanceConfig.LayerRequests);
      MergeInstanceFeatureRequests(instanceConfig.ExtensionRequests, appInstanceConfig.ExtensionRequests);

      if (instanceUserChoice.ValidationLayer == OptionUserChoice::Off)
      {
        FilterFeatureByName(instanceConfig.LayerRequests, CONFIG_VALIDATION_LAYER_NAME);
      }
      if (instanceUserChoice.UserChoiceApiDump == OptionUserChoice::Off)
      {
        FilterFeatureByName(instanceConfig.LayerRequests, CONFIG_API_DUMP_LAYER_NAME);
      }
      return PrepareConfig(instanceConfig);
    }
  }
}
