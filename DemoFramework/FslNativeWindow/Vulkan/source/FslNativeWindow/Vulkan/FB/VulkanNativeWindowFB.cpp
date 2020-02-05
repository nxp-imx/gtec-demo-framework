#if !defined(__ANDROID__) && defined(__linux__) && defined(FSL_WINDOWSYSTEM_FRAMEBUFFER)
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

#include "VulkanNativeWindowSystemFB.hpp"
#include "VulkanNativeWindowFB.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>
#include <FslUtil/Vulkan1_0/Log/FmtAll.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <algorithm>
#include <vector>

#if 0
#define LOCAL_LOG(X) FSLLOG3_INFO("VulkanNativeWindowFB: {}", (X))
#else
#define LOCAL_LOG(X) \
  {                  \
  }
#endif

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    const auto PLATFORM_KHR_SURFACE_EXTENSION_NAME = VK_KHR_DISPLAY_EXTENSION_NAME;

    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                  const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<VulkanNativeWindowFB>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }


    NativeVulkanSetup ToNativeVulkanSetup(const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      const auto pNativeSetup = dynamic_cast<const NativeVulkanSetup*>(pPlatformCustomWindowAllocationParams);
      if (!pNativeSetup)
        throw NotSupportedException("NativeVulkanSetup pointer expected");
      return *pNativeSetup;
    }


    //! @brief Try to locate a display mode that matches the native display resolution and prefer the fastest refresh rate.
    VkDisplayModeKHR TryFindTheBestDisplayModeMatch(const VkDisplayPropertiesKHR& displayProperties,
                                                    const std::vector<VkDisplayModePropertiesKHR>& displayModeProperties)
    {
      uint32_t bestMatchIndex = 0;
      uint32_t refreshRate = 0;

      for (std::size_t i = 0; i < displayModeProperties.size(); ++i)
      {
        if (displayModeProperties[i].parameters.visibleRegion.width == displayProperties.physicalResolution.width &&
            displayModeProperties[i].parameters.visibleRegion.height == displayProperties.physicalResolution.height &&
            displayModeProperties[i].parameters.refreshRate > refreshRate)
        {
          bestMatchIndex = i;
          refreshRate = displayModeProperties[i].parameters.refreshRate;
        }
      }
      return (refreshRate != 0 ? displayModeProperties[bestMatchIndex].displayMode : VK_NULL_HANDLE);
    }


    bool CanPlaneSupportDisplay(const VkPhysicalDevice physicalDevice, const VkDisplayKHR currentDisplay, const VkDisplayModeKHR displayMode,
                                const VkExtent2D& requiredExtent, const uint32_t planeIndex)
    {
      const auto planeSupportedDisplays = PhysicalDeviceKHRUtil::GetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex);
      for (uint32_t displayIndex = 0; displayIndex < planeSupportedDisplays.size(); ++displayIndex)
      {
        // Check, if plane can be used on requested display.
        if (planeSupportedDisplays[displayIndex] == currentDisplay)
        {
          VkDisplayPlaneCapabilitiesKHR displayPlaneCapabilities;
          const auto result = vkGetDisplayPlaneCapabilitiesKHR(physicalDevice, displayMode, planeIndex, &displayPlaneCapabilities);
          if (result != VK_SUCCESS)
          {
            LOCAL_LOG("Could not get display plane capabilities.");
            return false;
          }

          // Check, if plane fulfills extent requirements.
          if (displayPlaneCapabilities.minDstExtent.width > requiredExtent.width ||
              displayPlaneCapabilities.minDstExtent.height > requiredExtent.height)
          {
            LOCAL_LOG("Extent to small.");
            continue;
          }

          if (displayPlaneCapabilities.maxDstExtent.width < requiredExtent.width ||
              displayPlaneCapabilities.maxDstExtent.height < requiredExtent.height)
          {
            LOCAL_LOG("Extent to large.");
            continue;
          }

          return true;
        }
      }
      return false;
    }


    bool TryFindSupportedPlaneIndex(const VkPhysicalDevice physicalDevice, const VkDisplayKHR currentDisplay, const VkDisplayModeKHR displayMode,
                                    const VkExtent2D& requiredExtent, const std::vector<VkDisplayPlanePropertiesKHR>& planeProperties,
                                    uint32_t& rPlaneIndex)
    {
      // Go through all planes.
      for (uint32_t planeIndex = 0; planeIndex < planeProperties.size(); ++planeIndex)
      {
        // Check, if plane is attached to a display.
        if (planeProperties[planeIndex].currentDisplay != VK_NULL_HANDLE)
        {
          if (CanPlaneSupportDisplay(physicalDevice, currentDisplay, displayMode, requiredExtent, planeIndex))
          {
            rPlaneIndex = planeIndex;
            return true;
          }
        }
      }
      rPlaneIndex = 0;
      return false;
    }


    inline void Log(const VkDisplayPropertiesKHR& displayProperties)
    {
      FSLLOG3_INFO("- Name: {}", (displayProperties.displayName != nullptr ? displayProperties.displayName : ""));
      FSLLOG3_INFO("- PhysicalDimensions: {}", displayProperties.physicalDimensions);
      FSLLOG3_INFO("- PhysicalResolution: {}", displayProperties.physicalResolution);
      FSLLOG3_INFO("- PhysicalResolution: {}", static_cast<uint32_t>(displayProperties.supportedTransforms));
      FSLLOG3_INFO("- PlaneReorderPossible: {}", (displayProperties.planeReorderPossible == VK_TRUE ? "true" : "false"));
      FSLLOG3_INFO("- persistentContent: {}", (displayProperties.persistentContent == VK_TRUE ? "true" : "false"));
    }


    inline void Log(const VkDisplayModeParametersKHR& parameters)
    {
      FSLLOG3_INFO("- VisibleRegion: {}", parameters.visibleRegion);
      FSLLOG3_INFO("- RefreshRate: {}", parameters.refreshRate);
    }


    void Log(const std::vector<VkDisplayPropertiesKHR>& properties)
    {
      FSLLOG3_INFO("Found {} display properties", properties.size());
      std::size_t index = 0;
      for (const auto properties : properties)
      {
        FSLLOG3_INFO("- Display index: {}", index);
        Log(properties);
        ++index;
      }
    }

    void Log(const VkDisplayModePropertiesKHR& displayModeProperties)
    {
      Log(displayModeProperties.parameters);
    }


    void Log(const std::vector<VkDisplayModePropertiesKHR>& displayModeProperties)
    {
      FSLLOG3_INFO("Found {} display mode properties", displayModeProperties.size());
      std::size_t index = 0;
      for (const auto properties : displayModeProperties)
      {
        FSLLOG3_INFO("- Index: {}", index);
        Log(properties);
        ++index;
      }
    }


    void Log(const std::vector<VkDisplayModePropertiesKHR>& displayModeProperties, const VkDisplayModeKHR displayMode)
    {
      auto itrFind = std::find_if(displayModeProperties.begin(), displayModeProperties.end(),
                                  [displayMode](const VkDisplayModePropertiesKHR& val) { return (val.displayMode == displayMode); });
      if (itrFind != displayModeProperties.end())
      {
        FSLLOG3_INFO("Selected");
        Log(*itrFind);
      }
      else
      {
        FSLLOG3_WARNING("Could not find the displayModeProperties");
      }
    }


    void Log(const std::vector<VkDisplayPlanePropertiesKHR>& deviceDisplayPlaneProperties)
    {
      FSLLOG3_INFO("Found {} display plane properties", deviceDisplayPlaneProperties.size());
      std::size_t index = 0;
      for (const auto properties : deviceDisplayPlaneProperties)
      {
        FSLLOG3_INFO("- Index: {}", index);
        FSLLOG3_INFO("- CurrentStackIndex: {}", properties.currentStackIndex);
        ++index;
      }
    }

    VkDisplaySurfaceCreateInfoKHR Prepare(const NativeWindowSetup& nativeWindowSetup, const VkPhysicalDevice physicalDevice,
                                          const uint32_t verbosityLevel)
    {
      // Get information about the physical displays
      const auto displayProperties = PhysicalDeviceKHRUtil::GetPhysicalDeviceDisplayPropertiesKHR(physicalDevice);
      if (displayProperties.size() <= 0)
        throw NotSupportedException("No physical displays found");

      if (verbosityLevel >= 2)
        Log(displayProperties);

      // Force the user requested display id to be in range
      const auto config = nativeWindowSetup.GetConfig();
      auto displayId = config.GetDisplayId();
      if (displayId < 0 || static_cast<std::size_t>(displayId) >= displayProperties.size())
      {
        FSLLOG3_WARNING("DisplayId {} is invalid, expected a value between 0 and {}, using displayId: 0", displayId, displayProperties.size());
        displayId = 0;
      }

      if (verbosityLevel >= 1)
      {
        FSLLOG3_INFO("User requested display index (id): {}", displayId);
        if (verbosityLevel >= 2)
          Log(displayProperties[displayId]);
      }

      // Get display mode properties for the chosen display
      const auto displayModeProperties = PhysicalDeviceKHRUtil::GetDisplayModePropertiesKHR(physicalDevice, displayProperties[displayId].display);
      if (displayProperties.size() <= 0)
        throw NotSupportedException(std::string("Display: ") + ToString(displayId) + " did not support any modes");

      if (verbosityLevel >= 2)
        Log(displayModeProperties);

      // Try to find a display mode that matches the native resolution
      const VkDisplayModeKHR displayMode = TryFindTheBestDisplayModeMatch(displayProperties[displayId], displayModeProperties);
      if (displayMode == VK_NULL_HANDLE)
        throw NotSupportedException("Could not find a matching display mode");

      if (verbosityLevel >= 1)
        Log(displayModeProperties, displayMode);

      // Try to find a plane index that supports our display
      auto imageExtent = displayProperties[displayId].physicalResolution;
      const auto deviceDisplayPlaneProperties = PhysicalDeviceKHRUtil::GetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice);

      if (verbosityLevel >= 2)
        Log(deviceDisplayPlaneProperties);

      uint32_t planeIndex;
      if (!TryFindSupportedPlaneIndex(physicalDevice, displayProperties[displayId].display, displayMode, imageExtent, deviceDisplayPlaneProperties,
                                      planeIndex))
      {
        FSLLOG3_INFO("No plane found. Falling back to plane with current resolution.");
        planeIndex = 0;
        // currentDisplay = deviceDisplayPlaneProperties[planeIndex].currentDisplay;
        VkDisplayPlaneCapabilitiesKHR displayPlaneCapabilities;
        RAPIDVULKAN_CHECK(vkGetDisplayPlaneCapabilitiesKHR(physicalDevice, displayMode, planeIndex, &displayPlaneCapabilities));
        imageExtent = displayPlaneCapabilities.maxDstExtent;
      }

      if (verbosityLevel >= 1)
      {
        FSLLOG3_INFO("Selected planeIndex: {}", planeIndex);
      }

      VkDisplaySurfaceCreateInfoKHR surfaceCreateInfo{};
      surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
      surfaceCreateInfo.displayMode = displayMode;
      surfaceCreateInfo.planeIndex = planeIndex;
      surfaceCreateInfo.planeStackIndex = deviceDisplayPlaneProperties[planeIndex].currentStackIndex;
      surfaceCreateInfo.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
      surfaceCreateInfo.globalAlpha = 1.0f;
      surfaceCreateInfo.alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR;
      surfaceCreateInfo.imageExtent = imageExtent;
      return surfaceCreateInfo;
    }
  }    // namespace


  VulkanNativeWindowSystemFB::VulkanNativeWindowSystemFB(const NativeWindowSystemSetup& setup)
    : VulkanNativeWindowSystemTemplate<PlatformNativeWindowSystemFB>(setup, PLATFORM_KHR_SURFACE_EXTENSION_NAME, AllocateWindow)
  {
  }


  VulkanNativeWindowFB::VulkanNativeWindowFB(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                             const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : AVulkanNativeWindow(ToNativeVulkanSetup(pPlatformCustomWindowAllocationParams))
    , PlatformNativeWindowFB(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams)
  {
    auto surfaceCreateInfo = Prepare(nativeWindowSetup, m_physicalDevice, nativeWindowSetup.GetVerbosityLevel());
    RAPIDVULKAN_CHECK(vkCreateDisplayPlaneSurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface));
  }


  VulkanNativeWindowFB::~VulkanNativeWindowFB()
  {
  }


  PlatformNativeWindowType VulkanNativeWindowFB::GetWindowType() const
  {
    return GetPlatformWindow();
  }
}    // namespace Fsl
#endif
