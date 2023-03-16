#if !defined(__ANDROID__) && defined(__linux__) && !defined(FSL_WINDOWSYSTEM_X11) && defined(FSL_WINDOWSYSTEM_FRAMEBUFFER) && \
  !defined(FSL_WINDOWSYSTEM_WAYLAND)
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

#include "EGLNativeWindowSystemFB.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/EGL/EGLNativeWindowTemplate.hpp>
#include <FslNativeWindow/Platform/FB/PlatformNativeWindowFB.hpp>
#include <FslUtil/EGL/EGLCheck.hpp>
#include <FslUtil/EGL/Exceptions.hpp>
#include <EGL/egl.h>
#include <algorithm>


namespace Fsl
{
  namespace
  {
    void CustomDestroyDisplay(void* pDisplay)
    {
      if (pDisplay != nullptr)
      {
        fbDestroyDisplay(static_cast<EGLNativeDisplayType>(pDisplay));
      }
    }


    void CustomDestroyWindow(void* pWindow)
    {
      if (pWindow != nullptr)
      {
        fbDestroyWindow(static_cast<EGLNativeWindowType>(pWindow));
      }
    }


    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                  const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      if (windowParams.PlatformWindow != nullptr)
        throw NotSupportedException("FB window can not already be set");

      const auto pNativeEglSetup = dynamic_cast<const NativeEGLSetup*>(pPlatformCustomWindowAllocationParams);
      if (!pNativeEglSetup)
        throw NotSupportedException("NativeEGLSetup pointer expected");

      const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();
      const EGLDisplay hDisplay = pNativeEglSetup->Display;
      const EGLConfig eglConfig = pNativeEglSetup->Config;
      int windowWidth, windowHeight, windowX, windowY;

      // Get the native visual id that matches the egl config
      EGLint nativeVisualId;
      EGL_CHECK(eglGetConfigAttrib(hDisplay, eglConfig, EGL_NATIVE_VISUAL_ID, &nativeVisualId));

      auto display = static_cast<EGLNativeDisplayType>(windowParams.PlatformDisplay);
      if (nativeWindowConfig.GetWindowMode() != WindowMode::Window)
      {
        FSLLOG3_INFO("Window Size/Position not defined, setting them to MAX Display Resolution");
        fbGetDisplayGeometry(display, &windowWidth, &windowHeight);
        windowX = 0;
        windowY = 0;
      }
      else
      {
        const Rectangle windowRectangle = nativeWindowConfig.GetWindowRectangle();
        windowWidth = windowRectangle.Width();
        windowHeight = windowRectangle.Height();
        windowX = windowRectangle.X();
        windowY = windowRectangle.Y();
      }

      PlatformNativeWindowParams newWindowParams(windowParams);
      try
      {
        newWindowParams.PlatformWindow = fbCreateWindow(display, windowX, windowY, windowWidth, windowHeight);
        if (newWindowParams.PlatformWindow == nullptr)
          throw GraphicsException("Could not create FB Window");
        newWindowParams.DestroyWindowFunc = CustomDestroyWindow;

        return std::make_shared<EGLNativeWindowTemplate<PlatformNativeWindowFB>>(nativeWindowSetup, newWindowParams,
                                                                                 pPlatformCustomWindowAllocationParams);
      }
      catch (const std::exception&)
      {
        CustomDestroyWindow(newWindowParams.PlatformWindow);
        throw;
      }
    }


    PlatformNativeDisplayType GetDisplay(const NativeWindowSystemSetup& setup)
    {
      FSLLOG3_WARNING_IF(setup.GetConfig().GetDisplayId() > 3, "FB backend cannot supports more than 4 display. Using DisplayId 0-3 instead of {}",
                         setup.GetConfig().GetDisplayId());

      return fbGetDisplayByIndex(std::max(std::min(setup.GetConfig().GetDisplayId(), 3), 0));
    }
  }


  EGLNativeWindowSystemFB::EGLNativeWindowSystemFB(const NativeWindowSystemSetup& setup)
    : EGLNativeWindowSystemTemplate<PlatformNativeWindowSystemFB>(setup, AllocateWindow,
                                                                  PlatformNativeWindowSystemParams(GetDisplay(setup), CustomDestroyDisplay))
  {
  }
}
#endif
