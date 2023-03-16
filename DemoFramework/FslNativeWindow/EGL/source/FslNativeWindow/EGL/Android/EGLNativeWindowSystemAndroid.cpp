#ifdef __ANDROID__
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

#include "EGLNativeWindowSystemAndroid.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslNativeWindow/EGL/EGLNativeWindowTemplate.hpp>
#include <FslNativeWindow/Platform/Android/PlatformNativeWindowAndroid.hpp>
#include <FslNativeWindow/Platform/Android/PlatformNativeWindowAndroidCallbackCombiner.hpp>
#include <FslNativeWindow/Platform/Android/PlatformNativeWindowSystemAndroidTag.hpp>


#if 0
#define LOCAL_LOG(X) FSLLOG3_INFO("EGLNativeWindowSystemAndroid: {}", (X))
#else
#define LOCAL_LOG(X) \
  {                  \
  }
#endif

namespace Fsl
{
  std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    const auto nativeEglSetup = EGLNativeWindowHelper::ToNativeEGLSetup(pPlatformCustomWindowAllocationParams);

    // setup a little lambda method for tweaking the window after its been created.
    PlatformCallbackNativeWindowAndroidOnWindowCreate onWindowCreated = [nativeEglSetup](ANativeWindow* hDisplay, android_app* pAppState)
    {
      // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
      // As soon as we picked a EGLConfig, we can safely reconfigure the ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
      EGLint format;
      LOCAL_LOG("eglGetConfigAttrib");
      if (eglGetConfigAttrib(nativeEglSetup.Display, nativeEglSetup.Config, EGL_NATIVE_VISUAL_ID, &format))
      {
        LOCAL_LOG("ANativeWindow_setBuffersGeometry");
        ANativeWindow_setBuffersGeometry(pAppState->window, 0, 0, format);
      }
      else
      {
        FSLLOG3_WARNING("Failed to acquire information for ANativeWindow_setBuffersGeometry");
      }
    };

    LOCAL_LOG("AllocateWindow1");

    // Patch the params and forward
    PlatformNativeWindowParams customWindowParams(windowParams);
    customWindowParams.OnWindowCreated = PlatformNativeWindowAndroidCallbackCombiner::Combine(onWindowCreated, windowParams.OnWindowCreated);

    LOCAL_LOG("AllocateWindow2");
    return std::make_shared<EGLNativeWindowTemplate<PlatformNativeWindowAndroid>>(nativeWindowSetup, customWindowParams,
                                                                                  pPlatformCustomWindowAllocationParams);
  }


  EGLNativeWindowSystemAndroid::EGLNativeWindowSystemAndroid(const NativeWindowSystemSetup& setup)
    : EGLNativeWindowSystemTemplate<PlatformNativeWindowSystemAndroid>(setup, AllocateWindow, EGL_DEFAULT_DISPLAY)
  {
  }
}
#endif
