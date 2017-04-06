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

#include <FslGraphicsEGL/EGLStringUtil.hpp>
#include <sstream>
#include <iomanip>

namespace Fsl
{
  std::string EGLStringUtil::GetConfigEnumToString(const EGLenum value)
  {
    switch (value)
    {
    case EGL_ALPHA_MASK_SIZE:
      return std::string("EGL_ALPHA_MASK_SIZE");
    case EGL_ALPHA_SIZE:
      return std::string("EGL_ALPHA_SIZE");
    case EGL_BIND_TO_TEXTURE_RGB:
      return std::string("EGL_BIND_TO_TEXTURE_RGB");
      break;
    case EGL_BIND_TO_TEXTURE_RGBA:
      return std::string("EGL_BIND_TO_TEXTURE_RGBA");
      break;
    case EGL_BLUE_SIZE:
      return std::string("EGL_BLUE_SIZE");
      break;
    case EGL_BUFFER_SIZE:
      return std::string("EGL_BUFFER_SIZE");
      break;
    case EGL_COLOR_BUFFER_TYPE:
      return std::string("EGL_COLOR_BUFFER_TYPE");
      break;
    case EGL_CONFIG_CAVEAT:
      return std::string("EGL_CONFIG_CAVEAT");
      break;
    case EGL_CONFIG_ID:
      return std::string("EGL_CONFIG_ID");
      break;
    case EGL_CONFORMANT:
      return std::string("EGL_CONFORMANT");
      break;
    case EGL_DEPTH_SIZE:
      return std::string("EGL_DEPTH_SIZE");
      break;
    case EGL_GREEN_SIZE:
      return std::string("EGL_GREEN_SIZE");
      break;
    case EGL_LEVEL:
      return std::string("EGL_LEVEL");
      break;
    case EGL_LUMINANCE_SIZE:
      return std::string("EGL_LUMINANCE_SIZE");
      break;
    case EGL_MATCH_NATIVE_PIXMAP:
      return std::string("EGL_MATCH_NATIVE_PIXMAP");
      break;
    case EGL_NATIVE_RENDERABLE:
      return std::string("EGL_NATIVE_RENDERABLE");
      break;
    case EGL_MAX_SWAP_INTERVAL:
      return std::string("EGL_MAX_SWAP_INTERVAL");
      break;
    case EGL_MIN_SWAP_INTERVAL:
      return std::string("EGL_MIN_SWAP_INTERVAL");
      break;
    case EGL_RED_SIZE:
      return std::string("EGL_RED_SIZE");
      break;
    case EGL_SAMPLE_BUFFERS:
      return std::string("EGL_SAMPLE_BUFFERS");
      break;
    case EGL_SAMPLES:
      return std::string("EGL_SAMPLES");
      break;
    case EGL_STENCIL_SIZE:
      return std::string("EGL_STENCIL_SIZE");
      break;
    case EGL_RENDERABLE_TYPE:
      return std::string("EGL_RENDERABLE_TYPE");
      break;
    case EGL_SURFACE_TYPE:
      return std::string("EGL_SURFACE_TYPE");
      break;
    case EGL_TRANSPARENT_TYPE:
      return std::string("EGL_TRANSPARENT_TYPE");
      break;
    case EGL_TRANSPARENT_RED_VALUE:
      return std::string("EGL_TRANSPARENT_RED_VALUE");
      break;
    case EGL_TRANSPARENT_GREEN_VALUE:
      return std::string("EGL_TRANSPARENT_GREEN_VALUE");
      break;
    case EGL_TRANSPARENT_BLUE_VALUE:
      return std::string("EGL_TRANSPARENT_BLUE_VALUE");
      break;
    default:
      std::stringstream sstream;
      sstream << std::hex << std::setfill('0') << std::setw(4) << value;
      return sstream.str();
    }
  }
}
