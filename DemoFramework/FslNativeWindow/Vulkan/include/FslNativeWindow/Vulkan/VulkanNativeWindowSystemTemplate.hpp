#ifndef FSLNATIVEWINDOW_VULKAN_VULKANNATIVEWINDOWSYSTEMTEMPLATE_HPP
#define FSLNATIVEWINDOW_VULKAN_VULKANNATIVEWINDOWSYSTEMTEMPLATE_HPP
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

#include <FslNativeWindow/Platform/PlatformNativeWindowAllocationFunction.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindowSystem.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindow.hpp>
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>

namespace Fsl
{
  struct NativeVulkanSetup;

  template<typename T>
  class VulkanNativeWindowSystemTemplate
    : public virtual IVulkanNativeWindowSystem
  {
    T m_system;
    std::string m_khrRSurfaceExtensionName;
  public:
    VulkanNativeWindowSystemTemplate(const NativeWindowSystemSetup& setup, const std::string& khrRSurfaceExtensionName, const PlatformNativeWindowAllocationFunction& allocateWindowFunction)
      : m_system(setup, allocateWindowFunction)
      , m_khrRSurfaceExtensionName(khrRSurfaceExtensionName)
    {
    }

    virtual std::string GetKHRSurfaceExtensionName() const override
    {
      return m_khrRSurfaceExtensionName;
    }


    virtual PlatformNativeDisplayType GetDisplayType() const override
    {
      return m_system.GetPlatformDisplay();
    }


    virtual std::shared_ptr<INativeWindow> CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams = nullptr) override
    {
      return m_system.CreateNativeWindow(nativeWindowSetup, pPlatformCustomWindowAllocationParams);
    }


    virtual std::shared_ptr<IVulkanNativeWindow> CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup, const NativeVulkanSetup& nativeVulkanSetup) override
    {
      // NOTE: Beware that above CreateNativeWindow might be called directly, so this method really should just 'call it' and then do a pointer cast
      auto window = CreateNativeWindow(nativeWindowSetup, &nativeVulkanSetup);
      auto ptr = std::dynamic_pointer_cast<IVulkanNativeWindow>(window);
      if (!ptr)
        throw NotSupportedException("Invalid window type returned by the native window system");
      return ptr;
    }

    virtual bool ProcessMessages(const NativeWindowProcessMessagesArgs& args) override
    {
      return m_system.ProcessMessages(args);
    }

    //! @brief Check if the display is considered HDR compatible
    virtual bool IsDisplayHDRCompatible(const int32_t displayId) const override
    {
      return m_system.IsDisplayHDRCompatible(displayId);
    }
  };
}

#endif
