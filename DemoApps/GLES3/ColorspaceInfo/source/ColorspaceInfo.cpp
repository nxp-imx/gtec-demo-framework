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

#include "ColorspaceInfo.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoHost/EGL/Config/Service/IEGLHostInfo.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/EGL/EGLUtil.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  namespace
  {
    const char* const g_pszEglColorspaceExtensions[] = {
      // https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_gl_colorspace.txt
      "EGL_KHR_gl_colorspace",
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_bt2020_linear.txt
      "EGL_EXT_gl_colorspace_bt2020_linear",
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_bt2020_linear.txt
      "EGL_EXT_gl_colorspace_bt2020_pq",
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_scrgb_linear.txt
      "EGL_EXT_gl_colorspace_scrgb_linear",
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_display_p3.txt
      "EGL_EXT_gl_colorspace_display_p3",
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_display_p3.txt
      "EGL_EXT_gl_colorspace_display_p3_linear",
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_gl_colorspace_scrgb.txt
      "EGL_EXT_gl_colorspace_scrgb"    //  (non-linear)

    };

    const char* const g_pszEglColorspaceMetaDataExtensions[] = {
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_surface_SMPTE2086_metadata.txt
      "EGL_EXT_surface_SMPTE2086_metadata",
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_surface_CTA861_3_metadata.txt
      "EGL_EXT_surface_CTA861_3_metadata"};

    const char* const g_pszOtherExtensions[] = {
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_pixel_format_float.txt
      "EGL_EXT_pixel_format_float",
    };
  }

  ColorspaceInfo::ColorspaceInfo(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const auto eglHostInfo = config.DemoServiceProvider.Get<IEGLHostInfo>();
    const EGLDisplay eglDisplay = eglHostInfo->GetDisplay();

    FSLLOG3_INFO("Checking for EGL colorspace extensions");
    std::size_t extCount = sizeof(g_pszEglColorspaceExtensions) / sizeof(char*);
    for (std::size_t i = 0; i < extCount; ++i)
    {
      FSLLOG3_INFO("- {}: {}", g_pszEglColorspaceExtensions[i],
                   EGLUtil::HasExtension(eglDisplay, g_pszEglColorspaceExtensions[i]) ? "Supported" : "Not supported");
    }

    FSLLOG3_INFO("Checking for colorspace meta data extensions");
    extCount = sizeof(g_pszEglColorspaceMetaDataExtensions) / sizeof(char*);
    for (std::size_t i = 0; i < extCount; ++i)
    {
      FSLLOG3_INFO("- {}: {}", g_pszEglColorspaceMetaDataExtensions[i],
                   EGLUtil::HasExtension(eglDisplay, g_pszEglColorspaceMetaDataExtensions[i]) ? "Supported" : "Not supported");
    }

    FSLLOG3_INFO("Checking for other relevant extensions");
    extCount = sizeof(g_pszOtherExtensions) / sizeof(char*);
    for (std::size_t i = 0; i < extCount; ++i)
    {
      FSLLOG3_INFO("- {}: {}", g_pszOtherExtensions[i], EGLUtil::HasExtension(eglDisplay, g_pszOtherExtensions[i]) ? "Supported" : "Not supported");
    }

    // Close the app
    GetDemoAppControl()->RequestExit();
  }
}
