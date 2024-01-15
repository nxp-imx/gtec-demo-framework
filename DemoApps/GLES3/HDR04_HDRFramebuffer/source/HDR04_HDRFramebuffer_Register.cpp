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
#include <FslDemoApp/Base/Service/Options/IOptions.hpp>
#include <FslDemoApp/Base/Service/Options/Options.hpp>
#include <FslDemoApp/OpenGLES3/Setup/RegisterDemoApp.hpp>
#include <FslUtil/EGL/EGLUtil.hpp>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <array>
#include "HDR04_HDRFramebuffer.hpp"
#include "OptionParserEx.hpp"
#include "SharedData.hpp"

// If our EGL header don't have these magic values defined we define em here since we dont use them unless the extension is detected.
// The values are defined here:
// https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_gl_colorspace.txt
// https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_bt2020_linear.txt

#ifndef EGL_GL_COLORSPACE
#ifdef EGL_GL_COLORSPACE_KHR
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_GL_COLORSPACE EGL_GL_COLORSPACE_KHR
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_GL_COLORSPACE 0x309D
#endif
#endif
#ifndef EGL_GL_COLORSPACE_BT2020_LINEAR_EXT
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_GL_COLORSPACE_BT2020_LINEAR_EXT 0x333F
#endif
#ifndef EGL_GL_COLORSPACE_SCRGB_LINEAR_EXT
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_GL_COLORSPACE_SCRGB_LINEAR_EXT 0x3350
#endif


namespace Fsl
{
  namespace
  {
    // Custom EGL config (these will per default overwrite the custom settings. However a exact EGL config can be used)
    const std::array<EGLint, (4 * 2) + 1> g_eglConfigAttribs = {
      // Prefer a EGL config with more than eight bits per channel (HDR)
      EGL_RED_SIZE, 10, EGL_GREEN_SIZE, 10, EGL_BLUE_SIZE, 10, EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER, EGL_NONE};

    const std::array<EGLint, 3> g_eglCreateWindowAttribs_bt2020 = {EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_BT2020_LINEAR_EXT, EGL_NONE};

    const std::array<EGLint, 3> g_eglCreateWindowAttribs_scrgb = {EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_SCRGB_LINEAR_EXT, EGL_NONE};


    const EGLint* GetCreateWindowSurfaceAttribs(const EGLDisplay display, const DemoAppHostCreateWindowSurfaceInfoEGL& createInfo,
                                                const std::shared_ptr<ITag>& userTag)
    {
      const auto userTagEx = std::dynamic_pointer_cast<SharedData>(userTag);
      if (!userTagEx)
      {
        FSLLOG3_INFO("Expected user tag missing");
        return nullptr;
      }

      const bool hasExtensionSCRGB = EGLUtil::HasExtension(display, "EGL_EXT_gl_colorspace_scrgb_linear");
      const bool hasExtensionBT2020 = EGLUtil::HasExtension(display, "EGL_EXT_gl_colorspace_bt2020_linear");

      FSLLOG3_INFO("Config HDR compatible:               {}", createInfo.IsConfigAttribsHDRCompatible);
      if (createInfo.IsConfigAttribsHDRCompatible)
      {
        FSLLOG3_INFO("Display HDR compatible:              {}", createInfo.IsDisplayHDRCompatible);
      }
      else
      {
        FSLLOG3_INFO("Display HDR compatible:              not checked");
      }
      FSLLOG3_INFO("EGL_EXT_gl_colorspace_scrgb_linear: {}", hasExtensionSCRGB);
      FSLLOG3_INFO("EGL_EXT_gl_colorspace_bt2020_linear: {}", hasExtensionBT2020);

      Options optionsService(createInfo.TheServiceProvider.Get<IOptions>());
      const auto options = optionsService.GetOptionParser<OptionParserEx>();
      const auto disableDisplayHDRCheck = options->IsDisplayHDRCheckDisabled();
      bool IsDisplayHDRCompatible = disableDisplayHDRCheck ? true : createInfo.IsDisplayHDRCompatible;
      FSLLOG3_WARNING_IF(disableDisplayHDRCheck, "Display HDR check disabled from command line");
      if (!IsDisplayHDRCompatible || !createInfo.IsConfigAttribsHDRCompatible)
      {
        FSLLOG3_INFO("HDRFramebuffer not supported");
        return nullptr;
      }

      // Since the extension 'EGL_GL_COLORSPACE_SCRGB_LINEAR_EXT' is optional we check for it
      if (hasExtensionSCRGB)
      {
        userTagEx->HDRFramebufferEnabled = true;
        FSLLOG3_INFO("EGL_EXT_gl_colorspace_scrgb_linear detected, requesting EGL_GL_COLORSPACE=EGL_GL_COLORSPACE_SCRGB_LINEAR_EXT");
        return g_eglCreateWindowAttribs_scrgb.data();
      }

      // Since the extension 'EGL_EXT_gl_colorspace_bt2020_linear' is optional we check for it
      if (hasExtensionBT2020)
      {
        userTagEx->HDRFramebufferEnabled = true;
        FSLLOG3_INFO("EGL_EXT_gl_colorspace_bt2020_linear detected, requesting EGL_GL_COLORSPACE=EGL_GL_COLORSPACE_BT2020_LINEAR_EXT");
        return g_eglCreateWindowAttribs_bt2020.data();
      }
      return nullptr;
    }
  }


  // Configure the demo environment to run this demo app in a OpenGLES3 host environment
  void ConfigureDemoAppEnvironment(HostDemoAppSetup& rSetup)
  {
    const auto sharedData = std::make_shared<SharedData>();

    DemoAppHostConfigEGL config(g_eglConfigAttribs.data(), ConfigControl::Default);
    config.SetUserTag(sharedData);
    // Hook into the system and allow us to customize the attribs passed to eglCreateWindowSurface at runtime
    // depending on the availability of extensions
    config.SetCallbackGetCreateWindowSurfaceAttribs(GetCreateWindowSurfaceAttribs);

    // // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_scrgb_linear.txt
    config.AddExtensionRequest(ExtensionType::EGL, "EGL_EXT_gl_colorspace_scrgb_linear", ExtensionPrecense::Optional);
    // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_bt2020_linear.txt
    config.AddExtensionRequest(ExtensionType::EGL, "EGL_EXT_gl_colorspace_bt2020_linear", ExtensionPrecense::Optional);
    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_color_buffer_float.txt
    config.AddExtensionRequest(ExtensionType::OpenGLES, "GL_EXT_color_buffer_float", ExtensionPrecense::Mandatory);

    CustomDemoAppConfig customDemoAppConfig(sharedData);
    DemoAppRegister::GLES3::Register<HDR04_HDRFramebuffer, OptionParserEx>(rSetup, "GLES3.HDR04_HDRFramebuffer", config, customDemoAppConfig);
  }
}
