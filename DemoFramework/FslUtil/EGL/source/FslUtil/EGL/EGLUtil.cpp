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

#include <FslBase/String/StringUtil.hpp>
#include <FslUtil/EGL/EGLCheck.hpp>
#include <FslUtil/EGL/EGLUtil.hpp>
#include <EGL/egl.h>
#include <algorithm>
#include <vector>

namespace Fsl::EGLUtil
{
  // NOLINTNEXTLINE(misc-misplaced-const)
  std::vector<StringViewLite> GetExtensions(const EGLDisplay display)
  {
    const auto* const pszExtensions = eglQueryString(display, EGL_EXTENSIONS);
    EGL_CHECK_FOR_ERROR();
    if (pszExtensions == nullptr)
    {
      throw std::runtime_error("Failed to retrieve EGL extensions");
    }

    return StringUtil::Split(pszExtensions, ' ', true);
  }


  // NOLINTNEXTLINE(misc-misplaced-const)
  bool HasExtension(const EGLDisplay display, const char* const pszExtensionName)
  {
    const auto extensions = GetExtensions(display);
    return std::find(extensions.begin(), extensions.end(), StringViewLite(pszExtensionName)) != extensions.end();
  }


  // NOLINTNEXTLINE(misc-misplaced-const)
  std::vector<EGLConfig> GetConfigs(const EGLDisplay dpy)
  {
    EGLint numConfigs = 0;
    if (eglGetConfigs(dpy, nullptr, 0, &numConfigs) != EGL_TRUE)
    {
      throw EGLGraphicsException("eglGetConfigs", eglGetError(), __FILE__, __LINE__);
    }

    std::vector<EGLConfig> result(numConfigs);
    if (eglGetConfigs(dpy, result.data(), static_cast<EGLint>(result.size()), &numConfigs) != EGL_TRUE || numConfigs < 0)
    {
      throw EGLGraphicsException("eglGetConfigs", eglGetError(), __FILE__, __LINE__);
    }
    if (static_cast<std::size_t>(numConfigs) < result.size())
    {
      result.resize(numConfigs);
    }
    return result;
  }

  std::vector<EGLenum> GetConfigAttribs()
  {
    std::vector<EGLenum> result = {EGL_ALPHA_MASK_SIZE,
                                   EGL_ALPHA_SIZE,
                                   EGL_BIND_TO_TEXTURE_RGB,
                                   EGL_BIND_TO_TEXTURE_RGBA,
                                   EGL_BLUE_SIZE,
                                   EGL_BUFFER_SIZE,
                                   EGL_COLOR_BUFFER_TYPE,
                                   EGL_CONFIG_CAVEAT,
                                   EGL_CONFIG_ID,
                                   EGL_CONFORMANT,
                                   EGL_DEPTH_SIZE,
                                   EGL_GREEN_SIZE,
                                   EGL_LEVEL,
                                   EGL_LUMINANCE_SIZE,
#ifdef EGL_MAX_PBUFFER_WIDTH
                                   EGL_MAX_PBUFFER_WIDTH,
#endif
#ifdef EGL_MAX_PBUFFER_HEIGHT
                                   EGL_MAX_PBUFFER_HEIGHT,
#endif
    // EGL_MATCH_NATIVE_PIXMAP,  // can not be queried
#ifdef EGL_MAX_PBUFFER_PIXELS
                                   EGL_MAX_PBUFFER_PIXELS,
#endif
                                   EGL_MAX_SWAP_INTERVAL,
                                   EGL_MIN_SWAP_INTERVAL,
                                   EGL_NATIVE_RENDERABLE,
#ifdef EGL_NATIVE_VISUAL_ID
                                   EGL_NATIVE_VISUAL_ID,
#endif
#ifdef EGL_NATIVE_VISUAL_TYPE
                                   EGL_NATIVE_VISUAL_TYPE,
#endif
                                   EGL_RED_SIZE,
                                   EGL_RENDERABLE_TYPE,
                                   EGL_SAMPLE_BUFFERS,
                                   EGL_SAMPLES,
                                   EGL_STENCIL_SIZE,
                                   EGL_SURFACE_TYPE,
                                   EGL_TRANSPARENT_TYPE,
                                   EGL_TRANSPARENT_RED_VALUE,
                                   EGL_TRANSPARENT_GREEN_VALUE,
                                   EGL_TRANSPARENT_BLUE_VALUE};
    return result;
  }
}
