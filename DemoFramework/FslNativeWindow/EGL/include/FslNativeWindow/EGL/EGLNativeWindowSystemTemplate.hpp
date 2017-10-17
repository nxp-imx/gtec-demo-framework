#ifndef FSLNATIVEWINDOW_EGL_EGLNATIVEWINDOWSYSTEMTEMPLATE_HPP
#define FSLNATIVEWINDOW_EGL_EGLNATIVEWINDOWSYSTEMTEMPLATE_HPP
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
#include <FslNativeWindow/EGL/IEGLNativeWindowSystem.hpp>
#include <FslNativeWindow/EGL/IEGLNativeWindow.hpp>
#include <FslNativeWindow/EGL/NativeEGLSetup.hpp>

namespace Fsl
{
  class NativeEGLSetup;

  enum class EGLHandleForceConvert
  {
    Enabled
  };

  template<typename TNativeWindowSystem>
  class EGLNativeWindowSystemTemplate
    : public IEGLNativeWindowSystem
  {
    TNativeWindowSystem m_system;
    EGLNativeDisplayType m_displayType;
  public:

    EGLNativeWindowSystemTemplate(const NativeWindowSystemSetup& setup, const PlatformNativeWindowAllocationFunction& allocateWindowFunction, const PlatformNativeWindowSystemParams& systemParams = PlatformNativeWindowSystemParams())
      : m_system(setup, allocateWindowFunction, systemParams)
      , m_displayType(static_cast<EGLNativeDisplayType>(m_system.GetPlatformDisplay()))
    {
    }

    //! @brief This is just a copy of the above code that does a reinterpret_cast instead, this allows code to 'force' a conversion
    EGLNativeWindowSystemTemplate(const NativeWindowSystemSetup& setup, const PlatformNativeWindowAllocationFunction& allocateWindowFunction, const EGLHandleForceConvert, const PlatformNativeWindowSystemParams& systemParams = PlatformNativeWindowSystemParams())
      : m_system(setup, allocateWindowFunction, systemParams)
      , m_displayType(reinterpret_cast<EGLNativeDisplayType>(m_system.GetPlatformDisplay()))
    {
    }

    EGLNativeWindowSystemTemplate(const NativeWindowSystemSetup& setup, const PlatformNativeWindowAllocationFunction& allocateWindowFunction, EGLNativeDisplayType displayType, const PlatformNativeWindowSystemParams& systemParams = PlatformNativeWindowSystemParams())
      : m_system(setup, allocateWindowFunction, systemParams)
      , m_displayType(displayType)
    {
    }

    // From INativeWindowSystem
    virtual std::shared_ptr<INativeWindow> CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams = nullptr) override
    {
      return m_system.CreateNativeWindow(nativeWindowSetup, pPlatformCustomWindowAllocationParams);
    }


    virtual bool ProcessMessages(const NativeWindowProcessMessagesArgs& args) override
    {
      return m_system.ProcessMessages(args);
    }

    // From IEGLNativeWindowSystem
    virtual EGLNativeDisplayType GetDisplayType() const override
    {
      return m_displayType;
    }

    virtual std::shared_ptr<IEGLNativeWindow> CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup, const NativeEGLSetup& nativeEglSetup) override
    {
      // NOTE: Beware that above CreateNativeWindow might be called directly, so this method really should just 'call it' and then do a pointer cast
      auto window = CreateNativeWindow(nativeWindowSetup, &nativeEglSetup);
      auto ptr = std::dynamic_pointer_cast<IEGLNativeWindow>(window);
      if (!ptr)
        throw NotSupportedException("Invalid window type returned by the native window system");
      return ptr;
    }
  };
}

#endif
