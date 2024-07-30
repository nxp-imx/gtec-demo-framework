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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslUtil/OpenGLES2/GLCompressedFormat.hpp>
#include <FslUtil/OpenGLES2/GLRawBitmapUtil.hpp>
#include <GLES2/gl2ext.h>
#include <array>
#include <cassert>

namespace Fsl
{
  namespace GLES2
  {
    namespace
    {
      enum class ByteAlignment
      {
        Undefined = 0,
        B1 = 1,
        B2 = 2,
        B4 = 4,
        B8 = 8,
        Find = 0x8000
      };

      enum class FormatState
      {
        Invalid = 0,
        Normal = 1,
        Compressed = 2
      };

      struct FormatGL
      {
        FormatState State{FormatState::Invalid};
        GLint InternalFormat{};
        GLint Format{};
        GLenum Type{};
        ByteAlignment Alignment{ByteAlignment::Undefined};

        constexpr FormatGL() = default;

        constexpr FormatGL(const GLint internalFormat, const GLint format, const GLenum type)
          : State(FormatState::Normal)
          , InternalFormat(internalFormat)
          , Format(format)
          , Type(type)
          , Alignment(ByteAlignment::Find)
        {
        }

        constexpr FormatGL(const GLint internalFormat, const GLint format, const GLenum type, const ByteAlignment alignment)
          : State(FormatState::Normal)
          , InternalFormat(internalFormat)
          , Format(format)
          , Type(type)
          , Alignment(alignment)
        {
        }

        constexpr FormatGL(const GLint internalFormat, const GLint format, const ByteAlignment alignment, const FormatState formatState)
          : State(formatState)
          , InternalFormat(internalFormat)
          , Format(format)
          , Alignment(alignment)
        {
        }

        constexpr FormatGL(const GLint internalFormat, const GLint format, const FormatState formatState)
          : State(formatState)
          , InternalFormat(internalFormat)
          , Format(format)
          , Alignment(ByteAlignment::B4)
        {
        }

        constexpr FormatGL(const GLCompressedFormat internalFormat, const GLint format, const FormatState formatState)
          : FormatGL(static_cast<GLint>(internalFormat), format, formatState)
        {
        }
      };


      // clang-format off
      constexpr std::array<FormatGL, 188> Convert = {
                                                                                                  // Internal Format       | Format             | Type                                | Bits | Bits  | Bits | Bits  | Bits     | Format (-) means this would be a conversion operation
        FormatGL(),                                                                               //                                                                                                                           | Undefined
        FormatGL(),                                                                               //                                                                                                                           | R4G4_UNORM_PACK8
        FormatGL(),                                                                               //                                                                                                                           | R4G4B4A4_UNORM_PACK16
        FormatGL(),                                                                               //                                                                                                                           | B4G4R4A4_UNORM_PACK16
        FormatGL(),                                                                               //                                                                                                                           | R5G6B5_UNORM_PACK16
        FormatGL(),                                                                               //                                                                                                                           | B5G6R5_UNORM_PACK16
        FormatGL(),                                                                               //                                                                                                                           | R5G5B5A1_UNORM_PACK16
        FormatGL(),                                                                               //                                                                                                                           | B5G5R5A1_UNORM_PACK16
        FormatGL(),                                                                               //                                                                                                                           | A1R5G5B5_UNORM_PACK16
        FormatGL(),                                                                               //                                                                                                                           | R8_UNORM
        FormatGL(),                                                                               //                                                                                                                           | R8_SNORM
        FormatGL(),                                                                               //                                                                                                                           | R8_USCALED
        FormatGL(),                                                                               //                                                                                                                           | R8_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | R8_UINT
        FormatGL(),                                                                               //                                                                                                                           | R8_SINT
        FormatGL(),                                                                               //                                                                                                                           | R8_SRGB
        FormatGL(),                                                                               //                                                                                                                           | R8G8_UNORM
        FormatGL(),                                                                               //                                                                                                                           | R8G8_SNORM
        FormatGL(),                                                                               //                                                                                                                           | R8G8_USCALED
        FormatGL(),                                                                               //                                                                                                                           | R8G8_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | R8G8_UINT
        FormatGL(),                                                                               //                                                                                                                           | R8G8_SINT
        FormatGL(),                                                                               //                                                                                                                           | R8G8_SRGB
        FormatGL(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE),                                               // GL_RGB                | GL_RGB             | GL_UNSIGNED_BYTE                    | 8    | 8     | 8    |       |          | R8G8B8_UNORM
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8_SNORM
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8_USCALED
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8_UINT
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8_SINT
        FormatGL(GL_SRGB_EXT, GL_SRGB_EXT, GL_UNSIGNED_BYTE),                                     // GL_SRGB               | GL_RGB             | GL_UNSIGNED_BYTE                    | 8    | 8     | 8    |       |          | R8G8B8_SRGB
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8_UNORM
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8_SNORM
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8_USCALED
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8_UINT
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8_SINT
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8_SRGB
        FormatGL(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),                                             // GL_RGBA               | GL_RGBA            | GL_UNSIGNED_BYTE                    | 8    | 8     | 8    | 8     |          | R8G8B8A8_UNORM
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8A8_SNORM
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8A8_USCALED
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8A8_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8A8_UINT
        FormatGL(),                                                                               //                                                                                                                           | R8G8B8A8_SINT
        FormatGL(GL_SRGB_ALPHA_EXT, GL_SRGB_ALPHA_EXT, GL_UNSIGNED_BYTE),                         // GL_SRGB8_ALPHA8       | GL_RGBA            | GL_UNSIGNED_BYTE                    | 8    | 8     | 8    | 8     |          | R8G8B8A8_SRGB
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8A8_UNORM
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8A8_SNORM
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8A8_USCALED
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8A8_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8A8_UINT
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8A8_SINT
        FormatGL(),                                                                               //                                                                                                                           | B8G8R8A8_SRGB
        FormatGL(),                                                                               //                                                                                                                           | A8B8G8R8_UNORM_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A8B8G8R8_SNORM_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A8B8G8R8_USCALED_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A8B8G8R8_SSCALED_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A8B8G8R8_UINT_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A8B8G8R8_SINT_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A8B8G8R8_SRGB_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2R10G10B10_UNORM_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2R10G10B10_SNORM_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2R10G10B10_USCALED_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2R10G10B10_SSCALED_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2R10G10B10_UINT_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2R10G10B10_SINT_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2B10G10R10_UNORM_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2B10G10R10_SNORM_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2B10G10R10_USCALED_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2B10G10R10_SSCALED_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2B10G10R10_UINT_PACK32
        FormatGL(),                                                                               //                                                                                                                           | A2B10G10R10_SINT_PACK32
        FormatGL(),                                                                               //                                                                                                                           | R16_UNORM
        FormatGL(),                                                                               //                                                                                                                           | R16_SNORM
        FormatGL(),                                                                               //                                                                                                                           | R16_USCALED
        FormatGL(),                                                                               //                                                                                                                           | R16_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | R16_UINT
        FormatGL(),                                                                               //                                                                                                                           | R16_SINT
        FormatGL(),                                                                               //                                                                                                                           | R16_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R16G16_UNORM
        FormatGL(),                                                                               //                                                                                                                           | R16G16_SNORM
        FormatGL(),                                                                               //                                                                                                                           | R16G16_USCALED
        FormatGL(),                                                                               //                                                                                                                           | R16G16_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | R16G16_UINT
        FormatGL(),                                                                               //                                                                                                                           | R16G16_SINT
        FormatGL(),                                                                               //                                                                                                                           | R16G16_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16_UNORM
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16_SNORM
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16_USCALED
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16_UINT
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16_SINT
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16A16_UNORM
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16A16_SNORM
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16A16_USCALED
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16A16_SSCALED
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16A16_UINT
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16A16_SINT
        FormatGL(),                                                                               //                                                                                                                           | R16G16B16A16_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R32_UINT
        FormatGL(),                                                                               //                                                                                                                           | R32_SINT
        FormatGL(),                                                                               //                                                                                                                           | R32_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R32G32_UINT
        FormatGL(),                                                                               //                                                                                                                           | R32G32_SINT
        FormatGL(),                                                                               //                                                                                                                           | R32G32_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R32G32B32_UINT
        FormatGL(),                                                                               //                                                                                                                           | R32G32B32_SINT
        FormatGL(),                                                                               //                                                                                                                           | R32G32B32_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R32G32B32A32_UINT
        FormatGL(),                                                                               //                                                                                                                           | R32G32B32A32_SINT
        FormatGL(),                                                                               //                                                                                                                           | R32G32B32A32_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R64_UINT
        FormatGL(),                                                                               //                                                                                                                           | R64_SINT
        FormatGL(),                                                                               //                                                                                                                           | R64_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R64G64_UINT
        FormatGL(),                                                                               //                                                                                                                           | R64G64_SINT
        FormatGL(),                                                                               //                                                                                                                           | R64G64_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R64G64B64_UINT
        FormatGL(),                                                                               //                                                                                                                           | R64G64B64_SINT
        FormatGL(),                                                                               //                                                                                                                           | R64G64B64_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | R64G64B64A64_UINT
        FormatGL(),                                                                               //                                                                                                                           | R64G64B64A64_SINT
        FormatGL(),                                                                               //                                                                                                                           | R64G64B64A64_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | B10G11R11_UFLOAT_PACK32
        FormatGL(),                                                                               //                                                                                                                           | E5B9G9R9_UFLOAT_PACK32
        FormatGL(),                                                                               //                                                                                                                           | D16_UNORM
        FormatGL(),                                                                               //                                                                                                                           | X8_D24_UNORM_PACK32
        FormatGL(),                                                                               //                                                                                                                           | D32_SFLOAT
        FormatGL(),                                                                               //                                                                                                                           | S8_UINT
        FormatGL(),                                                                               //                                                                                                                           | D16_UNORM_S8_UINT
        FormatGL(),                                                                               //                                                                                                                           | D24_UNORM_S8_UINT
        FormatGL(),                                                                               //                                                                                                                           | D32_SFLOAT_S8_UINT
        FormatGL(),                                                                               //                                                                                                                           | BC1_RGB_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC1_RGB_SRGB_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC1_RGBA_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC1_RGBA_SRGB_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC2_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC2_SRGB_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC3_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC3_SRGB_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC4_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC4_SNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC5_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC5_SNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC6H_UFLOAT_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC6H_SFLOAT_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC7_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | BC7_SRGB_BLOCK
        FormatGL(GLCompressedFormat::RGB8_ETC2, GL_RGB, FormatState::Compressed),                       // GL_COMPRESSED_RGB8_ETC2                      | GL_RGB                                                                     | ETC2_R8G8B8_UNORM_BLOCK
        FormatGL(GLCompressedFormat::SRGB8_ETC2, GL_RGB, FormatState::Compressed),                      // GL_COMPRESSED_SRGB8_ETC2                     | GL_RGB                                                                     | ETC2_R8G8B8_SRGB_BLOCK
        FormatGL(GLCompressedFormat::RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA, FormatState::Compressed),  // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2  | GL_RGBA                                                                    | ETC2_R8G8B8A1_UNORM_BLOCK
        FormatGL(GLCompressedFormat::SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA, FormatState::Compressed), // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 | GL_RGBA                                                                    | ETC2_R8G8B8A1_SRGB_BLOCK
        FormatGL(GLCompressedFormat::RGBA8_ETC2_EAC, GL_RGBA, FormatState::Compressed),                 // GL_COMPRESSED_RGBA8_ETC2_EAC                 | GL_RGBA                                                                    | ETC2_R8G8B8A8_UNORM_BLOCK
        FormatGL(GLCompressedFormat::SRGB8_ALPHA8_ETC2_EAC, GL_RGBA, FormatState::Compressed),          // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          | GL_RGBA                                                                    | ETC2_R8G8B8A8_SRGB_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | EAC_R11_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | EAC_R11_SNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | EAC_R11G11_UNORM_BLOCK
        FormatGL(),                                                                               //                                                                                                                           | EAC_R11G11_SNORM_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_RGBA, FormatState::Compressed),              //                                                                                                                           | ASTC_4x4_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_RGBA, FormatState::Compressed),      //                                                                                                                           | ASTC_4x4_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_5x4_KHR, GL_RGBA, FormatState::Compressed),              //                                                                                                                           | ASTC_5x4_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, GL_RGBA, FormatState::Compressed),      //                                                                                                                           | ASTC_5x4_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_5x5_KHR, GL_RGBA, FormatState::Compressed),              //                                                                                                                           | ASTC_5x5_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, GL_RGBA, FormatState::Compressed),      //                                                                                                                           | ASTC_5x5_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_6x5_KHR, GL_RGBA, FormatState::Compressed),              //                                                                                                                           | ASTC_6x5_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, GL_RGBA, FormatState::Compressed),      //                                                                                                                           | ASTC_6x5_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_6x6_KHR, GL_RGBA, FormatState::Compressed),              //                                                                                                                           | ASTC_6x6_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, GL_RGBA, FormatState::Compressed),      //                                                                                                                           | ASTC_6x6_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_8x5_KHR, GL_RGBA, FormatState::Compressed),              //                                                                                                                           | ASTC_8x5_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, GL_RGBA, FormatState::Compressed),      //                                                                                                                           | ASTC_8x5_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_8x6_KHR, GL_RGBA, FormatState::Compressed),              //                                                                                                                           | ASTC_8x6_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, GL_RGBA, FormatState::Compressed),      //                                                                                                                           | ASTC_8x6_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_8x8_KHR, GL_RGBA, FormatState::Compressed),              //                                                                                                                           | ASTC_8x8_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, GL_RGBA, FormatState::Compressed),      //                                                                                                                           | ASTC_8x8_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_10x5_KHR, GL_RGBA, FormatState::Compressed),             //                                                                                                                           | ASTC_10x5_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, GL_RGBA, FormatState::Compressed),     //                                                                                                                           | ASTC_10x5_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_10x6_KHR, GL_RGBA, FormatState::Compressed),             //                                                                                                                           | ASTC_10x6_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, GL_RGBA, FormatState::Compressed),     //                                                                                                                           | ASTC_10x6_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_10x8_KHR, GL_RGBA, FormatState::Compressed),             //                                                                                                                           | ASTC_10x8_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, GL_RGBA, FormatState::Compressed),     //                                                                                                                           | ASTC_10x8_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_10x10_KHR, GL_RGBA, FormatState::Compressed),            //                                                                                                                           | ASTC_10x10_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, GL_RGBA, FormatState::Compressed),    //                                                                                                                           | ASTC_10x10_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_12x10_KHR, GL_RGBA, FormatState::Compressed),            //                                                                                                                           | ASTC_12x10_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, GL_RGBA, FormatState::Compressed),    //                                                                                                                           | ASTC_12x10_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA_ASTC_12x12_KHR, GL_RGBA, FormatState::Compressed),            //                                                                                                                           | ASTC_12x12_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, GL_RGBA, FormatState::Compressed),    //                                                                                                                           | ASTC_12x12_SRGB_BLOCK
        // Extended formats
        FormatGL(GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE),                                           // GL_ALPHA              | GL_ALPHA           | GL_UNSIGNED_BYTE              | Alpha                      | A               | EX_ALPHA8_UINT
        FormatGL(GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE),                                   // GL_LUMINANCE          | GL_LUMINANCE       | GL_UNSIGNED_BYTE              | Luminance                  | L               | EX_LUMINANCE8_UINT
        FormatGL(GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE),                       // GL_LUMINANCE_ALPHA    | GL_LUMINANCE_ALPHA | GL_UNSIGNED_BYTE              | Luminance, Alpha           | L, A            | EX_LUMINANCE8_ALPHA8_UINT
      };
      // clang-format on

      // Do some sanity checking
      static_assert(static_cast<uint32_t>(Convert.size()) == static_cast<uint32_t>(PixelFormat::ENUM_ID_RANGE_SIZE),
                    "g_convert needs to match the size of the enum range");


      GLRawBitmapUtil::Result ConvertPixelFormat(const PixelFormat pixelFormat, const uint32_t width, const uint32_t stride)
      {
        assert(stride >= width);

        // Simplified format conversion
        const auto bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(pixelFormat);
        const auto index = PixelFormatUtil::GetFormatRangeIndex(pixelFormat);
        const auto format = Convert[index];
        if (format.State == FormatState::Normal)
        {
          if (format.Alignment == ByteAlignment::Find)
          {
            return {format.InternalFormat, format.Format, format.Type, RawBitmapUtil::CalcAlignment(pixelFormat, width, stride, bytesPerPixel)};
          }
          return {format.InternalFormat, format.Format, format.Type, static_cast<GLint>(format.Alignment)};
        }

        throw UnsupportedPixelFormatException(pixelFormat);
      }


      GLRawBitmapUtil::CompressedResult ConvertCompressedPixelFormat(const PixelFormat pixelFormat, const uint32_t width)
      {
        FSL_PARAM_NOT_USED(width);
        // Simplified format conversion
        const auto index = PixelFormatUtil::GetFormatRangeIndex(pixelFormat);
        const auto format = Convert[index];
        if (format.State == FormatState::Compressed && format.Alignment != ByteAlignment::Find)
        {
          return {format.InternalFormat, format.Format, static_cast<GLint>(format.Alignment)};
        }
        throw UnsupportedPixelFormatException(pixelFormat);
      }
    }

    GLRawBitmapUtil::Result GLRawBitmapUtil::Convert(const PixelFormat pixelFormat, const uint32_t width, const uint32_t stride,
                                                     [[maybe_unused]] const bool exactMatch)
    {
      if (stride < width)
      {
        throw UnsupportedStrideException(UncheckedNumericCast<int32_t>(stride));
      }

      FSLLOG3_DEBUG_WARNING_IF(exactMatch, "GLES2 does not support exact matching of pixelFormats with the textureFormat");

      switch (pixelFormat)
      {
      case PixelFormat::EX_ALPHA8_UNORM:
        return {GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE,
                RawBitmapUtil::CalcAlignment(pixelFormat, width, stride,
                                             1)};    // GL_ALPHA              | GL_ALPHA           | GL_UNSIGNED_BYTE              | Alpha | A
      case PixelFormat::EX_LUMINANCE8_UNORM:
        return {GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                RawBitmapUtil::CalcAlignment(pixelFormat, width, stride,
                                             1)};    // GL_LUMINANCE          | GL_LUMINANCE       | GL_UNSIGNED_BYTE              | Luminance | L
      case PixelFormat::EX_LUMINANCE8_ALPHA8_UNORM:
        return {
          GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
          RawBitmapUtil::CalcAlignment(pixelFormat, width, stride,
                                       2)};    // GL_LUMINANCE_ALPHA    | GL_LUMINANCE_ALPHA | GL_UNSIGNED_BYTE              | Luminance, Alpha | L, A
      default:
        break;
      }

      // switch (PixelFormatUtil::GetPixelFormatLayout(pixelFormat))
      //{
      // case PixelFormatLayout::R8G8B8:
      //  return Result(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
      //                RawBitmapUtil::CalcAlignment(
      //                  pixelFormat, width, stride,
      //                  3));    // GL_RGB                | GL_RGB             | GL_UNSIGNED_BYTE,             | Red, Green, Blue           | R, G, B
      // case PixelFormatLayout::R8G8B8A8:
      //  return Result(
      //    GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
      //    RawBitmapUtil::CalcAlignment(
      //      pixelFormat, width, stride,
      //      4));    // GL_RGBA               | GL_RGBA            | GL_UNSIGNED_BYTE,             | Red, Green, Blue, Alpha    | R, G, B, A
      // default:
      //  throw UnsupportedPixelFormatException(pixelFormat);
      //}
      return ConvertPixelFormat(pixelFormat, width, stride);
    }


    GLRawBitmapUtil::CompressedResult GLRawBitmapUtil::ConvertCompressed(const PixelFormat pixelFormat, const uint32_t width)
    {
      return ConvertCompressedPixelFormat(pixelFormat, width);
    }
  }


  namespace
  {
    // Some cut and pasted verification checks just to be 100% on the safe side and its a compile time check so its fine
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_ERROR_MESSAGE "The index did not match our assumption"

    // Due to lack of constexpr we use this nasty macro
    // So because of the macros be sure to keep all real code above this so you dont get any interference from them
    // #define LOCAL_GET_PIXELFORMAT_ID(X) ((static_cast<uint32_t>((X)) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
    // static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE))
    constexpr uint32_t LocalGetPixelformatId(const PixelFormat pf)
    {
      return ((static_cast<uint32_t>((pf)) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
              static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE));
    }

    // Some cut and pasted verification checks just to be 100% on the safe side and its a compile time check so its fine
    static_assert(LocalGetPixelformatId(PixelFormat::Undefined) == 0, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R4G4_UNORM_PACK8) == 1, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R4G4B4A4_UNORM_PACK16) == 2, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B4G4R4A4_UNORM_PACK16) == 3, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R5G6B5_UNORM_PACK16) == 4, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B5G6R5_UNORM_PACK16) == 5, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R5G5B5A1_UNORM_PACK16) == 6, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B5G5R5A1_UNORM_PACK16) == 7, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A1R5G5B5_UNORM_PACK16) == 8, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8_UNORM) == 9, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8_SNORM) == 10, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8_USCALED) == 11, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8_SSCALED) == 12, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8_UINT) == 13, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8_SINT) == 14, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8_SRGB) == 15, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8_UNORM) == 16, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8_SNORM) == 17, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8_USCALED) == 18, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8_SSCALED) == 19, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8_UINT) == 20, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8_SINT) == 21, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8_SRGB) == 22, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8_UNORM) == 23, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8_SNORM) == 24, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8_USCALED) == 25, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8_SSCALED) == 26, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8_UINT) == 27, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8_SINT) == 28, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8_SRGB) == 29, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8_UNORM) == 30, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8_SNORM) == 31, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8_USCALED) == 32, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8_SSCALED) == 33, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8_UINT) == 34, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8_SINT) == 35, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8_SRGB) == 36, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8A8_UNORM) == 37, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8A8_SNORM) == 38, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8A8_USCALED) == 39, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8A8_SSCALED) == 40, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8A8_UINT) == 41, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8A8_SINT) == 42, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R8G8B8A8_SRGB) == 43, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8A8_UNORM) == 44, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8A8_SNORM) == 45, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8A8_USCALED) == 46, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8A8_SSCALED) == 47, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8A8_UINT) == 48, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8A8_SINT) == 49, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B8G8R8A8_SRGB) == 50, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A8B8G8R8_UNORM_PACK32) == 51, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A8B8G8R8_SNORM_PACK32) == 52, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A8B8G8R8_USCALED_PACK32) == 53, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A8B8G8R8_SSCALED_PACK32) == 54, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A8B8G8R8_UINT_PACK32) == 55, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A8B8G8R8_SINT_PACK32) == 56, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A8B8G8R8_SRGB_PACK32) == 57, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2R10G10B10_UNORM_PACK32) == 58, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2R10G10B10_SNORM_PACK32) == 59, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2R10G10B10_USCALED_PACK32) == 60, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2R10G10B10_SSCALED_PACK32) == 61, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2R10G10B10_UINT_PACK32) == 62, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2R10G10B10_SINT_PACK32) == 63, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2B10G10R10_UNORM_PACK32) == 64, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2B10G10R10_SNORM_PACK32) == 65, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2B10G10R10_USCALED_PACK32) == 66, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2B10G10R10_SSCALED_PACK32) == 67, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2B10G10R10_UINT_PACK32) == 68, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::A2B10G10R10_SINT_PACK32) == 69, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16_UNORM) == 70, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16_SNORM) == 71, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16_USCALED) == 72, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16_SSCALED) == 73, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16_UINT) == 74, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16_SINT) == 75, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16_SFLOAT) == 76, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16_UNORM) == 77, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16_SNORM) == 78, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16_USCALED) == 79, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16_SSCALED) == 80, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16_UINT) == 81, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16_SINT) == 82, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16_SFLOAT) == 83, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16_UNORM) == 84, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16_SNORM) == 85, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16_USCALED) == 86, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16_SSCALED) == 87, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16_UINT) == 88, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16_SINT) == 89, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16_SFLOAT) == 90, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16A16_UNORM) == 91, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16A16_SNORM) == 92, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16A16_USCALED) == 93, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16A16_SSCALED) == 94, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16A16_UINT) == 95, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16A16_SINT) == 96, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R16G16B16A16_SFLOAT) == 97, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32_UINT) == 98, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32_SINT) == 99, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32_SFLOAT) == 100, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32_UINT) == 101, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32_SINT) == 102, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32_SFLOAT) == 103, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32B32_UINT) == 104, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32B32_SINT) == 105, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32B32_SFLOAT) == 106, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32B32A32_UINT) == 107, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32B32A32_SINT) == 108, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R32G32B32A32_SFLOAT) == 109, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64_UINT) == 110, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64_SINT) == 111, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64_SFLOAT) == 112, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64_UINT) == 113, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64_SINT) == 114, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64_SFLOAT) == 115, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64B64_UINT) == 116, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64B64_SINT) == 117, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64B64_SFLOAT) == 118, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64B64A64_UINT) == 119, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64B64A64_SINT) == 120, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::R64G64B64A64_SFLOAT) == 121, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::B10G11R11_UFLOAT_PACK32) == 122, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::E5B9G9R9_UFLOAT_PACK32) == 123, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::D16_UNORM) == 124, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::X8_D24_UNORM_PACK32) == 125, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::D32_SFLOAT) == 126, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::S8_UINT) == 127, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::D16_UNORM_S8_UINT) == 128, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::D24_UNORM_S8_UINT) == 129, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::D32_SFLOAT_S8_UINT) == 130, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC1_RGB_UNORM_BLOCK) == 131, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC1_RGB_SRGB_BLOCK) == 132, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC1_RGBA_UNORM_BLOCK) == 133, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC1_RGBA_SRGB_BLOCK) == 134, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC2_UNORM_BLOCK) == 135, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC2_SRGB_BLOCK) == 136, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC3_UNORM_BLOCK) == 137, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC3_SRGB_BLOCK) == 138, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC4_UNORM_BLOCK) == 139, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC4_SNORM_BLOCK) == 140, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC5_UNORM_BLOCK) == 141, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC5_SNORM_BLOCK) == 142, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC6H_UFLOAT_BLOCK) == 143, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC6H_SFLOAT_BLOCK) == 144, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC7_UNORM_BLOCK) == 145, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::BC7_SRGB_BLOCK) == 146, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ETC2_R8G8B8_UNORM_BLOCK) == 147, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ETC2_R8G8B8_SRGB_BLOCK) == 148, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK) == 149, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK) == 150, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK) == 151, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK) == 152, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::EAC_R11_UNORM_BLOCK) == 153, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::EAC_R11_SNORM_BLOCK) == 154, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::EAC_R11G11_UNORM_BLOCK) == 155, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::EAC_R11G11_SNORM_BLOCK) == 156, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_4x4_UNORM_BLOCK) == 157, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_4x4_SRGB_BLOCK) == 158, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_5x4_UNORM_BLOCK) == 159, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_5x4_SRGB_BLOCK) == 160, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_5x5_UNORM_BLOCK) == 161, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_5x5_SRGB_BLOCK) == 162, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_6x5_UNORM_BLOCK) == 163, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_6x5_SRGB_BLOCK) == 164, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_6x6_UNORM_BLOCK) == 165, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_6x6_SRGB_BLOCK) == 166, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_8x5_UNORM_BLOCK) == 167, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_8x5_SRGB_BLOCK) == 168, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_8x6_UNORM_BLOCK) == 169, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_8x6_SRGB_BLOCK) == 170, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_8x8_UNORM_BLOCK) == 171, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_8x8_SRGB_BLOCK) == 172, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_10x5_UNORM_BLOCK) == 173, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_10x5_SRGB_BLOCK) == 174, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_10x6_UNORM_BLOCK) == 175, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_10x6_SRGB_BLOCK) == 176, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_10x8_UNORM_BLOCK) == 177, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_10x8_SRGB_BLOCK) == 178, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_10x10_UNORM_BLOCK) == 179, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_10x10_SRGB_BLOCK) == 180, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_12x10_UNORM_BLOCK) == 181, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_12x10_SRGB_BLOCK) == 182, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_12x12_UNORM_BLOCK) == 183, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::ASTC_12x12_SRGB_BLOCK) == 184, LOCAL_ERROR_MESSAGE);

    // extended formats
    static_assert(LocalGetPixelformatId(PixelFormat::EX_ALPHA8_UNORM) == 185, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::EX_LUMINANCE8_UNORM) == 186, LOCAL_ERROR_MESSAGE);
    static_assert(LocalGetPixelformatId(PixelFormat::EX_LUMINANCE8_ALPHA8_UNORM) == 187, LOCAL_ERROR_MESSAGE);
  }
}
