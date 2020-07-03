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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/OpenGLES3/Setup/RegisterDemoApp.hpp>
#include <FslUtil/EGL/EGLUtil.hpp>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "SRGBFramebuffer.hpp"
#include "SharedData.hpp"
#include <array>

// If our EGL header don't have these magic values defined we define em here since we dont use them unless the extension is detected.
// The values are defined here:
// https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_gl_colorspace.txt

#ifndef EGL_GL_COLORSPACE
#ifdef EGL_GL_COLORSPACE_KHR
#define EGL_GL_COLORSPACE EGL_GL_COLORSPACE_KHR
#else
#define EGL_GL_COLORSPACE 0x309D
#endif
#endif
#ifndef EGL_GL_COLORSPACE_SRGB_KHR
#define EGL_GL_COLORSPACE_SRGB_KHR 0x3089
#endif


namespace Fsl
{
  namespace
  {
    // Custom EGL config (these will per default overwrite the custom settings. However a exact EGL config can be used)
    const std::array<EGLint, 1> g_eglConfigAttribs = {EGL_NONE};

    const std::array<EGLint, 3> g_eglCreateWindowAttribs = {EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE};

    const EGLint* GetCreateWindowSurfaceAttribs(const EGLDisplay display, const DemoAppHostCreateWindowSurfaceInfoEGL& /*createInfo*/,
                                                const std::shared_ptr<ITag>& userTag)
    {
      const auto userTagEx = std::dynamic_pointer_cast<SharedData>(userTag);

      // Since the extension 'EGL_KHR_gl_colorspace' is optional we check for it
      const bool hasExtension = EGLUtil::HasExtension(display, "EGL_KHR_gl_colorspace");
      if (!userTagEx || !hasExtension)
      {
        FSLLOG3_INFO_IF(!hasExtension, "EGL_KHR_gl_colorspace not detected");
        FSLLOG3_INFO_IF(!userTagEx, "Expected user tag missing");
        return nullptr;
      }

      userTagEx->SRGBFramebufferEnabled = true;
      FSLLOG3_INFO("EGL_KHR_gl_colorspace detected, requesting EGL_GL_COLORSPACE=EGL_GL_COLORSPACE_SRGB_KHR");
      return g_eglCreateWindowAttribs.data();
    }
  }

  // Configure the demo environment to run this demo app in a OpenGLES3 host environment
  void ConfigureDemoAppEnvironment(HostDemoAppSetup& rSetup)
  {
    const auto sharedData = std::make_shared<SharedData>();

    DemoAppHostConfigEGL config(g_eglConfigAttribs.data());
    config.SetUserTag(sharedData);
    // Hook into the system and allow us to customize the attribs passed to eglCreateWindowSurface at runtime
    // depending on the availability of extensions
    config.SetCallbackGetCreateWindowSurfaceAttribs(GetCreateWindowSurfaceAttribs);

    // https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_gl_colorspace.txt
    config.AddExtensionRequest(ExtensionType::EGL, "EGL_KHR_gl_colorspace", ExtensionPrecense::Optional);

    CustomDemoAppConfig customDemoAppConfig(sharedData);
    DemoAppRegister::GLES3::Register<SRGBFramebuffer>(rSetup, "GLES3.SRGBFramebuffer", config, customDemoAppConfig);
  }
}
