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

#include <FslUtil/OpenGLES3/DebugStrings.hpp>
#include <FslUtil/OpenGLES3/GLCompressedFormat.hpp>

namespace Fsl::GLES3::Debug
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
    case GL_FRAMEBUFFER_UNDEFINED:
      return "GL_FRAMEBUFFER_UNDEFINED";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
      return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    case GL_FRAMEBUFFER_UNSUPPORTED:
      return "GL_FRAMEBUFFER_UNSUPPORTED";
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
      return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    default:
      return "Unknown";
    }
  }


  extern const char* TryTextureFormatToString(const GLint format)
  {
    switch (format)
    {
    case GL_COMPRESSED_R11_EAC:
      return "GL_COMPRESSED_R11_EAC";
    case GL_COMPRESSED_SIGNED_R11_EAC:
      return "GL_COMPRESSED_SIGNED_R11_EAC";
    case GL_COMPRESSED_RG11_EAC:
      return "GL_COMPRESSED_RG11_EAC";
    case GL_COMPRESSED_SIGNED_RG11_EAC:
      return "GL_COMPRESSED_SIGNED_RG11_EAC";

    case GL_COMPRESSED_RGB8_ETC2:
      return "GL_COMPRESSED_RGB8_ETC2";
    case GL_COMPRESSED_SRGB8_ETC2:
      return "GL_COMPRESSED_SRGB8_ETC2";
    case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return "GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2";
    case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return "GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";
    case GL_COMPRESSED_RGBA8_ETC2_EAC:
      return "GL_COMPRESSED_RGBA8_ETC2_EAC";
    case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
      return "GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC";

    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_4x4):
      return "GL_COMPRESSED_RGBA_ASTC_4x4";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_5x4):
      return "GL_COMPRESSED_RGBA_ASTC_5x4";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_5x5):
      return "GL_COMPRESSED_RGBA_ASTC_5x5";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_6x5):
      return "GL_COMPRESSED_RGBA_ASTC_6x5";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_6x6):
      return "GL_COMPRESSED_RGBA_ASTC_6x6";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_8x5):
      return "GL_COMPRESSED_RGBA_ASTC_8x5";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_8x6):
      return "GL_COMPRESSED_RGBA_ASTC_8x6";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_8x8):
      return "GL_COMPRESSED_RGBA_ASTC_8x8";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_10x5):
      return "GL_COMPRESSED_RGBA_ASTC_10x5";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_10x6):
      return "GL_COMPRESSED_RGBA_ASTC_10x6";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_10x8):
      return "GL_COMPRESSED_RGBA_ASTC_10x8";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_10x10):
      return "GL_COMPRESSED_RGBA_ASTC_10x10";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_12x10):
      return "GL_COMPRESSED_RGBA_ASTC_12x10";
    case static_cast<GLint>(GLCompressedFormat::RGBA_ASTC_12x12):
      return "GL_COMPRESSED_RGBA_ASTC_12x12";

    case static_cast<GLint>(GLCompressedFormat::ETC1_RGB8_OES):
      return "GL_ETC1_RGB8_OES";

    case static_cast<GLint>(GLCompressedFormat::PALETTE4_RGB8_OES):
      return "GL_PALETTE4_RGB8_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE4_RGBA8_OES):
      return "GL_PALETTE4_RGBA8_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE4_R5_G6_B5_OES):
      return "GL_PALETTE4_R5_G6_B5_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE4_RGBA4_OES):
      return "GL_PALETTE4_RGBA4_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE4_RGB5_A1_OES):
      return "GL_PALETTE4_RGB5_A1_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE8_RGB8_OES):
      return "GL_PALETTE8_RGB8_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE8_RGBA8_OES):
      return "GL_PALETTE8_RGBA8_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE8_R5_G6_B5_OES):
      return "GL_PALETTE8_R5_G6_B5_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE8_RGBA4_OES):
      return "GL_PALETTE8_RGBA4_OES";
    case static_cast<GLint>(GLCompressedFormat::PALETTE8_RGB5_A1_OES):
      return "GL_PALETTE8_RGB5_A1_OES";

    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_4x4):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_5x4):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_5x5):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_6x5):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_6x6):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_8x5):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_8x6):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_8x8):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_10x5):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_10x6):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_10x8):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_10x10):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_12x10):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10";
    case static_cast<GLint>(GLCompressedFormat::SRGB8_ALPHA8_ASTC_12x12):
      return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12";

    default:
      return nullptr;
    }
  }
}
