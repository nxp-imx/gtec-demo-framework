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

#include <FslUtil/OpenGLES2/DebugStrings.hpp>
#include <FslUtil/OpenGLES2/GLCompressedFormat.hpp>
#include <GLES2/gl2ext.h>

namespace Fsl
{
  namespace GLES2
  {
    namespace Debug
    {
      extern const char* ErrorCodeToString(const GLenum errorCode)
      {
        switch (errorCode)
        {
        case GL_NO_ERROR:
          return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
          return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
          return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
          return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
          return "GL_OUT_OF_MEMORY";

        // Other useful enum values
        case GL_FRAMEBUFFER_COMPLETE:
          return "GL_FRAMEBUFFER_COMPLETE";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
          return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
          return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
          return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        case GL_FRAMEBUFFER_UNSUPPORTED:
          return "GL_FRAMEBUFFER_UNSUPPORTED";
        default:
          return "Unknown";
        }
      }


      extern const char* TryTextureFormatToString(const GLint format)
      {
        switch (format)
        {
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_4x4_KHR";
        case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_5x4_KHR";
        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_5x5_KHR";
        case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_6x5_KHR";
        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_6x6_KHR";
        case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_8x5_KHR";
        case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_8x6_KHR";
        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_8x8_KHR";
        case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_10x5_KHR";
        case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_10x6_KHR";
        case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_10x8_KHR";
        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_10x10_KHR";
        case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_12x10_KHR";
        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
          return "GL_COMPRESSED_RGBA_ASTC_12x12_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
          return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR";

        case GL_ETC1_RGB8_OES:
          return "GL_ETC1_RGB8_OES";

        case GL_PALETTE4_RGB8_OES:
          return "GL_PALETTE4_RGB8_OES";
        case GL_PALETTE4_RGBA8_OES:
          return "GL_PALETTE4_RGBA8_OES";
        case GL_PALETTE4_R5_G6_B5_OES:
          return "GL_PALETTE4_R5_G6_B5_OES";
        case GL_PALETTE4_RGBA4_OES:
          return "GL_PALETTE4_RGBA4_OES";
        case GL_PALETTE4_RGB5_A1_OES:
          return "GL_PALETTE4_RGB5_A1_OES";
        case GL_PALETTE8_RGB8_OES:
          return "GL_PALETTE8_RGB8_OES";
        case GL_PALETTE8_RGBA8_OES:
          return "GL_PALETTE8_RGBA8_OES";
        case GL_PALETTE8_R5_G6_B5_OES:
          return "GL_PALETTE8_R5_G6_B5_OES";
        case GL_PALETTE8_RGBA4_OES:
          return "GL_PALETTE8_RGBA4_OES";
        case GL_PALETTE8_RGB5_A1_OES:
          return "GL_PALETTE8_RGB5_A1_OES";

        case GL_3DC_X_AMD:
          return "GL_3DC_X_AMD";
        case GL_3DC_XY_AMD:
          return "GL_3DC_XY_AMD";

        case GL_ATC_RGB_AMD:
          return "GL_ATC_RGB_AMD";
        case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
          return "GL_ATC_RGBA_EXPLICIT_ALPHA_AMD";
        case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
          return "GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD";

        case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
          return "GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE";
        case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
          return "GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE";

        case GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT:
          return "GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT";
        case GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT:
          return "GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT:
          return "GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT:
          return "GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG:
          return "GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG";
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG:
          return "GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG";

        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
          return "GL_COMPRESSED_RGB_S3TC_DXT1_EXT";
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
          return "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT";

        case static_cast<GLint>(GLCompressedFormat::R11_EAC):
          return "GL_COMPRESSED_R11_EAC";
        case static_cast<GLint>(GLCompressedFormat::SIGNED_R11_EAC):
          return "GL_COMPRESSED_SIGNED_R11_EAC";
        case static_cast<GLint>(GLCompressedFormat::RG11_EAC):
          return "GL_COMPRESSED_RG11_EAC";
        case static_cast<GLint>(GLCompressedFormat::SIGNED_RG11_EAC):
          return "GL_COMPRESSED_SIGNED_RG11_EAC";

        case static_cast<GLint>(GLCompressedFormat::RGB8_ETC2):
          return "GL_COMPRESSED_RGB8_ETC2";
        case static_cast<GLint>(GLCompressedFormat::SRGB8_ETC2):
          return "GL_COMPRESSED_SRGB8_ETC2";
        case static_cast<GLint>(GLCompressedFormat::RGB8_PUNCHTHROUGH_ALPHA1_ETC2):
          return "GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2";
        case static_cast<GLint>(GLCompressedFormat::SRGB8_PUNCHTHROUGH_ALPHA1_ETC2):
          return "GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";
        case static_cast<GLint>(GLCompressedFormat::RGBA8_ETC2_EAC):
          return "GL_COMPRESSED_RGBA8_ETC2_EAC";
        case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ETC2_EAC):
          return "GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC";

        default:
          return nullptr;
        }
      }
    }
  }
}
