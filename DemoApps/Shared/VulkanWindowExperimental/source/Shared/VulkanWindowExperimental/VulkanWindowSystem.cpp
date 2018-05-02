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

#include <Shared/VulkanWindowExperimental/VulkanWindowSystem.hpp>
#include <Shared/VulkanWindowExperimental/OptionParser.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Base/Host/DemoAppHostConfigWindow.hpp>
#include <FslDemoApp/Base/Host/DemoHostCustomWindowSystemSetup.hpp>
#include <FslDemoApp/Base/Service/Options/IOptions.hpp>
#include <FslDemoApp/Base/Service/Options/Options.hpp>
#include <FslDemoHost/Vulkan/Config/DemoAppHostConfigVulkanEx.hpp>
#include <FslNativeWindow/Vulkan/VulkanNativeWindowSystemFactory.hpp>
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindowSystem.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/SafeType/InstanceCreateInfoCopy.hpp>
#include <array>
#include <algorithm>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
#ifdef NDEBUG
    const static bool LOCAL_VALIDATION_LAYER_ENABLED = false;
#else
    const static bool LOCAL_VALIDATION_LAYER_ENABLED = true;
#endif

    const auto CONFIG_VALIDATION_LAYER_NAME = "VK_LAYER_LUNARG_standard_validation";

    struct InstanceConfigRequest
    {
      std::deque<Vulkan::InstanceFeatureRequest> LayerRequests;
      std::deque<Vulkan::InstanceFeatureRequest> ExtensionRequests;
    };

    struct InstanceConfig
    {
      std::deque<std::string> Layers;
      std::deque<std::string> Extensions;
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

    InstanceConfigRequest BuildHostInstanceConfigRequest(const std::string& khrSurfaceExtensionName, const OptionUserChoice userChoiceValidationLayer,
                                                         const ConfigControl instanceLayerConfigControl,
                                                         const ConfigControl instanceExtensionConfigControl)
    {
      InstanceConfigRequest instanceConfig;

      const auto buildDebugLayerRequirement = LOCAL_VALIDATION_LAYER_ENABLED ? Vulkan::FeatureRequirement::Optional : Vulkan::FeatureRequirement::Invalid;
      const auto debugLayerRequirement = ToFeatureRequirement(userChoiceValidationLayer, buildDebugLayerRequirement);
      if (debugLayerRequirement != Vulkan::FeatureRequirement::Invalid)
        instanceConfig.LayerRequests.push_back(InstanceFeatureRequest(CONFIG_VALIDATION_LAYER_NAME, debugLayerRequirement));

      // Always add the SURFACE extensions the extension ConfigControl does not modify this
      instanceConfig.ExtensionRequests.push_back(InstanceFeatureRequest(VK_KHR_SURFACE_EXTENSION_NAME, Vulkan::FeatureRequirement::Mandatory));
      instanceConfig.ExtensionRequests.push_back(InstanceFeatureRequest(khrSurfaceExtensionName, Vulkan::FeatureRequirement::Mandatory));

      //if (instanceLayerConfigControl == ConfigControl::Overwrite)
      //{

      //}

      //if (instanceExtensionConfigControl == ConfigControl::Overwrite)
      //{

      //}
      return instanceConfig;
    }


    bool TryReplace(std::deque<Vulkan::InstanceFeatureRequest>& rRequests, const Vulkan::InstanceFeatureRequest& featureRequest)
    {
      auto itrFind = std::find_if(rRequests.begin(), rRequests.end(), [featureRequest](const Vulkan::InstanceFeatureRequest& entry) { return (entry.Name == featureRequest.Name); });
      if (itrFind == rRequests.end())
        return false;

      *itrFind = featureRequest;
      return true;
    }


    void MergeInstanceFeatureRequests(std::deque<Vulkan::InstanceFeatureRequest>& rRequests, const std::deque<Vulkan::InstanceFeatureRequest>& newRequests)
    {
      for (const auto& entry : newRequests)
      {
        if (! TryReplace(rRequests, entry))
          rRequests.push_back(entry);
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
    void PrepareConfig(std::deque<std::string>& rDst, std::deque<Vulkan::InstanceFeatureRequest> requests, const std::vector<T>& properties, const char*const pszDesc)
    {
      FSLLOG2(LogType::Verbose, "Examining " << pszDesc << " requests");
      for (const auto& request : requests)
      {
        const auto itrFind = std::find_if(properties.begin(), properties.end(), [request](const T& entry) { return (GetEntryName(entry) == request.Name); });
        if (itrFind != properties.end())
        {
          // Found, so just push it
          rDst.push_back(request.Name);
          FSLLOG2(LogType::Verbose, "- " << request.Name << " found");
        }
        else
        {
          // Not found, so lets see if its mandatory or optional
          switch (request.Requirement)
          {
          case Vulkan::FeatureRequirement::Mandatory:
            FSLLOG2(LogType::Verbose, "- " << request.Name << " was mandatory and not found.");
            throw NotSupportedException(std::string("The ") + std::string(pszDesc) + std::string(" is not supported: ") + request.Name);
          case Vulkan::FeatureRequirement::Optional:
            FSLLOG2(LogType::Verbose, "- " << request.Name << " was optional and not found.");
            break;
          default:
            throw NotSupportedException("The FeatureRequirement type is unsupported");
          }
        }
      }
    }


    void FilterFeatureByName(std::deque<Vulkan::InstanceFeatureRequest>& rLayerRequests, const std::string& name)
    {
      auto itrFind = std::find_if(rLayerRequests.begin(), rLayerRequests.end(), [name](const Vulkan::InstanceFeatureRequest& entry) { return (entry.Name == name); });
      if (itrFind != rLayerRequests.end())
      {
        FSLLOG2(LogType::Verbose, "Removing '" << name << "' due to command line option.");
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


    //! @brief
    //! @param khrSurfaceExtensionName
    //! @param userEnableValidationLayerRequest true if the user has requested it from the command line
    //! @param customdemoAppHostConfig a optional demo app host config
    InstanceConfig BuildInstanceConfig(const std::string& khrSurfaceExtensionName, const OptionUserChoice userChoiceValidationLayer, const std::shared_ptr<DemoAppHostConfigVulkanEx>& customDemoAppHostConfig)
    {
      const auto instanceLayerConfigControl = customDemoAppHostConfig ? customDemoAppHostConfig->GetInstanceLayerConfigControl() : ConfigControl::Default;
      const auto instanceExtensionConfigControl = customDemoAppHostConfig ? customDemoAppHostConfig->GetInstanceExtensionConfigControl() : ConfigControl::Default;

      InstanceConfigRequest instanceConfig = BuildHostInstanceConfigRequest(khrSurfaceExtensionName, userChoiceValidationLayer, instanceLayerConfigControl, instanceExtensionConfigControl);
      InstanceConfigRequest appInstanceConfig;
      if (customDemoAppHostConfig)
      {
        appInstanceConfig.LayerRequests = customDemoAppHostConfig->GetInstanceLayerRequests();
        appInstanceConfig.ExtensionRequests = customDemoAppHostConfig->GetInstanceExtensionRequests();
      }

      MergeInstanceFeatureRequests(instanceConfig.LayerRequests, appInstanceConfig.LayerRequests);
      MergeInstanceFeatureRequests(instanceConfig.ExtensionRequests, appInstanceConfig.ExtensionRequests);

      if (userChoiceValidationLayer == OptionUserChoice::Off)
        FilterFeatureByName(instanceConfig.LayerRequests, CONFIG_VALIDATION_LAYER_NAME);
      return PrepareConfig(instanceConfig);
    }
  }

  VulkanWindowSystem::VulkanWindowSystem(const DemoHostCustomWindowSystemSetup& setup)
    : DemoHostCustomWindowSystem(VulkanNativeWindowSystemFactory::AllocateNative(setup.WindowSystemSetup))
    , m_instance()
    , m_instanceCreateInfo(std::make_shared<Vulkan::InstanceCreateInfoCopy>())
    , m_physicalDevice()
  {
    FSLLOG("VulkanWindowSystem creating");
    Options optionsService(setup.Provider.Get<IOptions>());
    auto appOptionParser = optionsService.GetOptionParser<OptionParser>();
    const auto physicialDeviceIndex = appOptionParser->GetPhysicalDeviceIndex();
    const auto userChoiceValidationLayer = appOptionParser->GetValidationLayer();

    auto vulkanWindowSystem = std::dynamic_pointer_cast<IVulkanNativeWindowSystem>(m_windowSystem);
    if (! vulkanWindowSystem)
      throw NotSupportedException("The window system was not of the expected type");

    const std::string khrSurfaceExtensionName = vulkanWindowSystem->GetKHRSurfaceExtensionName();
    const auto instanceConfig = BuildInstanceConfig(khrSurfaceExtensionName, userChoiceValidationLayer, std::dynamic_pointer_cast<DemoAppHostConfigVulkanEx>(setup.CustomDemoAppHostConfig));

    { // Transform the config to nasty C style arrays, then create the instance
      std::vector<const char*> layers(instanceConfig.Layers.size());
      std::vector<const char*> extensions(instanceConfig.Extensions.size());
      for (std::size_t i = 0; i < instanceConfig.Layers.size(); ++i)
        layers[i] = instanceConfig.Layers[i].c_str();
      for (std::size_t i = 0; i < instanceConfig.Extensions.size(); ++i)
        extensions[i] = instanceConfig.Extensions[i].c_str();

      m_instance = InstanceUtil::CreateInstance("VulkanWindowSystem", VK_MAKE_VERSION(1, 0, 0), VK_MAKE_VERSION(1, 0, 0), 0, layers, extensions, m_instanceCreateInfo.get());
    }
    m_physicalDevice = InstanceUtil::GetPhysicalDevice(m_instance.Get(), physicialDeviceIndex);

    FSLLOG("VulkanWindowSystem created");
  }


  VulkanWindowSystem::~VulkanWindowSystem()
  {
    FSLLOG("VulkanWindowSystem destroying");
  }


  std::shared_ptr<INativeWindow> VulkanWindowSystem::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams)
  {
    if (pPlatformCustomWindowAllocationParams)
      throw NotSupportedException("pPlatformCustomWindowAllocationParams must be null");

    NativeVulkanSetup vulkanSetup(m_instance.Get(), m_physicalDevice.Device);
    return m_windowSystem->CreateNativeWindow(nativeWindowSetup, &vulkanSetup);
  }

}
