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

#include <FslUtil/EGL/EGLStringUtil.hpp>
#include <FslUtil/EGL/EGLCheck.hpp>
#include <FslUtil/EGL/Exceptions.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/String/ToString.hpp>
//#include <EGL/eglext.h>
#include <sstream>
#include <iomanip>

#define DEFINE_BITFLAG_NAMERECORD(fLAG) BitFlagAndNameRecord((fLAG), #fLAG)

#ifndef EGL_OPENGL_ES3_BIT
#define EGL_OPENGL_ES3_BIT  0x0040
#endif
// https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_stream_producer_eglsurface.txt
#ifndef EGL_STREAM_BIT_KHR
#define EGL_STREAM_BIT_KHR 0x0800
#endif
// https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_mutable_render_buffer.txt
#ifndef EGL_MUTABLE_RENDER_BUFFER_BIT_KHR
#define EGL_MUTABLE_RENDER_BUFFER_BIT_KHR 0x00001000
#endif

namespace Fsl
{
  namespace
  {
    struct BitFlagAndNameRecord
    {
      EGLint Value;
      const char* Name;

      BitFlagAndNameRecord(const EGLint value, const char* pszName)
        : Value(value)
        , Name(pszName)
      {
      }
    };

    BitFlagAndNameRecord g_bitflags_EGL_CONFORMANT[] =
    {
#ifdef EGL_OPENGL_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_OPENGL_BIT),
#endif
#ifdef EGL_OPENGL_ES_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_OPENGL_ES_BIT),
#endif
#ifdef EGL_OPENGL_ES2_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_OPENGL_ES2_BIT),
#endif
#ifdef EGL_OPENGL_ES3_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_OPENGL_ES3_BIT),
#endif
#ifdef EGL_OPENVG_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_OPENVG_BIT),
#endif
    };


    BitFlagAndNameRecord g_bitflags_EGL_SURFACE_TYPE[] =
    {
#ifdef EGL_MULTISAMPLE_RESOLVE_BOX_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_MULTISAMPLE_RESOLVE_BOX_BIT),
#endif
#ifdef EGL_PBUFFER_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_PBUFFER_BIT),
#endif
#ifdef EGL_PIXMAP_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_PIXMAP_BIT),
#endif
#ifdef EGL_SWAP_BEHAVIOR_PRESERVED_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_SWAP_BEHAVIOR_PRESERVED_BIT),
#endif
#ifdef EGL_VG_ALPHA_FORMAT_PRE_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_VG_ALPHA_FORMAT_PRE_BIT),
#endif
#ifdef EGL_VG_COLORSPACE_LINEAR_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_VG_COLORSPACE_LINEAR_BIT),
#endif
#ifdef EGL_WINDOW_BIT
      DEFINE_BITFLAG_NAMERECORD(EGL_WINDOW_BIT),
#endif
#ifdef EGL_STREAM_BIT_KHR
      DEFINE_BITFLAG_NAMERECORD(EGL_STREAM_BIT_KHR),
#endif
#ifdef EGL_MUTABLE_RENDER_BUFFER_BIT_KHR
      DEFINE_BITFLAG_NAMERECORD(EGL_MUTABLE_RENDER_BUFFER_BIT_KHR),
#endif
    };




    std::string GetBitflagsString(EGLint bitmask, const BitFlagAndNameRecord* pRecords, const std::size_t numRecords)
    {
      std::stringstream stream;
      std::size_t foundCount = 0;
      if (numRecords > 0 && pRecords != nullptr)
      {
        for (std::size_t index = 0; index < numRecords; ++index)
        {
          if ((bitmask & pRecords[index].Value) == pRecords[index].Value)
          {
            if( foundCount > 0 )
              stream << "|" << pRecords[index].Name;
            else
              stream << pRecords[index].Name;
            bitmask &= ~pRecords[index].Value;
            ++foundCount;
          }
        }
      }
      if(foundCount <= 0 && bitmask != 0)
        stream << "0x" << std::hex << bitmask;
      else if (bitmask != 0)
        stream << "|0x" << std::hex << bitmask;
      return stream.str();
    }


    std::string ToUnknownString(const EGLint value)
    {
      return std::string("Unknown (") + ToString(value) + std::string(")");
    }

    std::string ToBoolString(const EGLint value)
    {
      return value == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE";
    }

    std::string GetAttributeValueString_EGL_COLOR_BUFFER_TYPE(const EGLint value)
    {
      switch (value)
      {
      case EGL_RGB_BUFFER:
        return "EGL_RGB_BUFFER";
      case EGL_LUMINANCE_BUFFER:
        return "EGL_LUMINANCE_BUFFER";
      default:
        return ToUnknownString(value);
      }
    }


    std::string GetAttributeValueString_EGL_CONFORMANT(const EGLint value)
    {
      return GetBitflagsString(value, g_bitflags_EGL_CONFORMANT, sizeof(g_bitflags_EGL_CONFORMANT) / sizeof(BitFlagAndNameRecord));
    }


    std::string GetAttributeValueString_EGL_CONFIG_CAVEAT(const EGLint value)
    {
      switch (value)
      {
      case EGL_NONE:
        return "EGL_NONE";
      case EGL_SLOW_CONFIG:
        return "EGL_SLOW_CONFIG";
#ifdef EGL_NON_CONFORMANT
      case EGL_NON_CONFORMANT:
        return "EGL_NON_CONFORMANT";
#endif
      default:
        return ToUnknownString(value);
      }
    }

    std::string GetAttributeValueString_EGL_TRANSPARENT_TYPE(const EGLint value)
    {
      switch (value)
      {
      case EGL_NONE:
        return "EGL_NONE";
      case EGL_TRANSPARENT_RGB:
        return "EGL_TRANSPARENT_RGB";
      default:
        return ToUnknownString(value);
      }
    }


    std::string GetAttributeValueString_EGL_SURFACE_TYPE(const EGLint value)
    {
      return GetBitflagsString(value, g_bitflags_EGL_SURFACE_TYPE, sizeof(g_bitflags_EGL_SURFACE_TYPE) / sizeof(BitFlagAndNameRecord));
    }



  }

  std::string EGLStringUtil::GetConfigEnumToString(const EGLenum attribute)
  {
    switch (attribute)
    {
    case EGL_ALPHA_MASK_SIZE:
      return std::string("EGL_ALPHA_MASK_SIZE");
    case EGL_ALPHA_SIZE:
      return std::string("EGL_ALPHA_SIZE");
    case EGL_BIND_TO_TEXTURE_RGB:
      return std::string("EGL_BIND_TO_TEXTURE_RGB");
    case EGL_BIND_TO_TEXTURE_RGBA:
      return std::string("EGL_BIND_TO_TEXTURE_RGBA");
    case EGL_BLUE_SIZE:
      return std::string("EGL_BLUE_SIZE");
    case EGL_BUFFER_SIZE:
      return std::string("EGL_BUFFER_SIZE");
    case EGL_COLOR_BUFFER_TYPE:
      return std::string("EGL_COLOR_BUFFER_TYPE");
    case EGL_CONFIG_CAVEAT:
      return std::string("EGL_CONFIG_CAVEAT");
    case EGL_CONFIG_ID:
      return std::string("EGL_CONFIG_ID");
    case EGL_CONFORMANT:
      return std::string("EGL_CONFORMANT");
    case EGL_DEPTH_SIZE:
      return std::string("EGL_DEPTH_SIZE");
    case EGL_GREEN_SIZE:
      return std::string("EGL_GREEN_SIZE");
    case EGL_LEVEL:
      return std::string("EGL_LEVEL");
    case EGL_LUMINANCE_SIZE:
      return std::string("EGL_LUMINANCE_SIZE");
#ifdef EGL_MAX_PBUFFER_WIDTH
    case EGL_MAX_PBUFFER_WIDTH:
      return std::string("EGL_MAX_PBUFFER_WIDTH");
#endif
#ifdef EGL_MAX_PBUFFER_HEIGHT
    case EGL_MAX_PBUFFER_HEIGHT:
      return std::string("EGL_MAX_PBUFFER_HEIGHT");
#endif
    case EGL_MATCH_NATIVE_PIXMAP:
      return std::string("EGL_MATCH_NATIVE_PIXMAP");
#ifdef EGL_MAX_PBUFFER_PIXELS
    case EGL_MAX_PBUFFER_PIXELS:
      return std::string("EGL_MAX_PBUFFER_PIXELS");
#endif
    case EGL_MAX_SWAP_INTERVAL:
      return std::string("EGL_MAX_SWAP_INTERVAL");
    case EGL_MIN_SWAP_INTERVAL:
      return std::string("EGL_MIN_SWAP_INTERVAL");
    case EGL_NATIVE_RENDERABLE:
      return std::string("EGL_NATIVE_RENDERABLE");
#ifdef EGL_NATIVE_VISUAL_ID
    case EGL_NATIVE_VISUAL_ID:
      return std::string("EGL_NATIVE_VISUAL_ID");
#endif
#ifdef EGL_NATIVE_VISUAL_TYPE
    case EGL_NATIVE_VISUAL_TYPE:
      return std::string("EGL_NATIVE_VISUAL_TYPE");
#endif
    case EGL_RED_SIZE:
      return std::string("EGL_RED_SIZE");
    case EGL_RENDERABLE_TYPE:
      return std::string("EGL_RENDERABLE_TYPE");
    case EGL_SAMPLE_BUFFERS:
      return std::string("EGL_SAMPLE_BUFFERS");
    case EGL_SAMPLES:
      return std::string("EGL_SAMPLES");
    case EGL_STENCIL_SIZE:
      return std::string("EGL_STENCIL_SIZE");
    case EGL_SURFACE_TYPE:
      return std::string("EGL_SURFACE_TYPE");
    case EGL_TRANSPARENT_TYPE:
      return std::string("EGL_TRANSPARENT_TYPE");
    case EGL_TRANSPARENT_RED_VALUE:
      return std::string("EGL_TRANSPARENT_RED_VALUE");
    case EGL_TRANSPARENT_GREEN_VALUE:
      return std::string("EGL_TRANSPARENT_GREEN_VALUE");
    case EGL_TRANSPARENT_BLUE_VALUE:
      return std::string("EGL_TRANSPARENT_BLUE_VALUE");
    default:
      std::stringstream sstream;
      sstream << std::hex << std::setfill('0') << std::setw(4) << attribute;
      return sstream.str();
    }
  }


  std::string EGLStringUtil::GetConfigAttribToString(const EGLenum attribute, const EGLint value)
  {
    switch (attribute)
    {
    case EGL_BIND_TO_TEXTURE_RGB:
    case EGL_BIND_TO_TEXTURE_RGBA:
    case EGL_NATIVE_RENDERABLE:
      return ToBoolString(value);
    case EGL_COLOR_BUFFER_TYPE:
      return GetAttributeValueString_EGL_COLOR_BUFFER_TYPE(value);
    case EGL_CONFIG_CAVEAT:
      return GetAttributeValueString_EGL_CONFIG_CAVEAT(value);
    case EGL_CONFORMANT:
    case EGL_RENDERABLE_TYPE:
      return GetAttributeValueString_EGL_CONFORMANT(value);
    case EGL_TRANSPARENT_TYPE:
      return GetAttributeValueString_EGL_TRANSPARENT_TYPE(value);
    case EGL_SURFACE_TYPE:
      return GetAttributeValueString_EGL_SURFACE_TYPE(value);

      //case EGL_ALPHA_MASK_SIZE:
    //case EGL_ALPHA_SIZE:
    //case EGL_BLUE_SIZE:
    //case EGL_BUFFER_SIZE:
    //case EGL_CONFIG_ID:
    //case EGL_DEPTH_SIZE:
    //case EGL_GREEN_SIZE:
    //case EGL_LEVEL:
    //case EGL_LUMINANCE_SIZE:
    //case EGL_MAX_PBUFFER_WIDTH:
    //case EGL_MAX_PBUFFER_HEIGHT:
    //case EGL_MAX_PBUFFER_PIXELS:
    //case EGL_MAX_SWAP_INTERVAL:
    //case EGL_MIN_SWAP_INTERVAL:
    //case EGL_MATCH_NATIVE_PIXMAP:  // can not be queried
    //case EGL_NATIVE_VISUAL_ID:
    //case EGL_NATIVE_VISUAL_TYPE:
    //case EGL_RED_SIZE:
    //case EGL_SAMPLE_BUFFERS:
    //case EGL_SAMPLES:
    //case EGL_STENCIL_SIZE:
    //case EGL_TRANSPARENT_RED_VALUE:
    //case EGL_TRANSPARENT_GREEN_VALUE:
    //case EGL_TRANSPARENT_BLUE_VALUE:
    default:
      return ToString(value);
    }
  }
}
