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

#include <FslUtil/OpenGLES3/GLRawBitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <cassert>

namespace Fsl
{
  namespace GLES3
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
        GLint InternalFormat;
        GLint Format;
        GLenum Type;
        ByteAlignment Alignment{ByteAlignment::Undefined};

        FormatGL()
          : InternalFormat{}
          , Format{}
          , Type{}
        {
        }

        FormatGL(const GLint internalFormat, const GLint format, const GLenum type)
          : State(FormatState::Normal)
          , InternalFormat(internalFormat)
          , Format(format)
          , Type(type)
          , Alignment(ByteAlignment::Find)
        {
        }

        FormatGL(const GLint internalFormat, const GLint format, const GLenum type, const ByteAlignment alignment)
          : State(FormatState::Normal)
          , InternalFormat(internalFormat)
          , Format(format)
          , Type(type)
          , Alignment(alignment)
        {
        }

        FormatGL(const GLint internalFormat, const GLint format, const ByteAlignment alignment, const FormatState formatState)
          : State(formatState)
          , InternalFormat(internalFormat)
          , Format(format)
          , Type{}
          , Alignment(alignment)
        {
        }

        FormatGL(const GLint internalFormat, const GLint format, const FormatState formatState)
          : State(formatState)
          , InternalFormat(internalFormat)
          , Format(format)
          , Type{}
          , Alignment(ByteAlignment::B4)
        {
        }
      };

      // https://www.khronos.org/opengles/sdk/docs/man3/html/glTexImage2D.xhtml


      // Unsized               |                    |                               |                            |
      // Internal Format       | Format             | Type                          | RGBA and Luminance Values  |Internal Components
      // ----------------------------------------------------------------------------------------------------------------------------
      // GL_RGB                | GL_RGB             | GL_UNSIGNED_BYTE,             | Red, Green, Blue           | R, G, B
      //                       |                    | GL_UNSIGNED_SHORT_5_6_5 (P)   |                            |
      // GL_RGBA               | GL_RGBA            | GL_UNSIGNED_BYTE,             | Red, Green, Blue, Alpha    | R, G, B, A
      //                       |                    | GL_UNSIGNED_SHORT_4_4_4_4,    |                            |
      //                       |                    | GL_UNSIGNED_SHORT_5_5_5_1 (P) |                            |
      // GL_LUMINANCE_ALPHA    | GL_LUMINANCE_ALPHA | GL_UNSIGNED_BYTE              | Luminance, Alpha           | L, A
      // GL_LUMINANCE          | GL_LUMINANCE       | GL_UNSIGNED_BYTE              | Luminance                  | L
      // GL_ALPHA              | GL_ALPHA           | GL_UNSIGNED_BYTE              | Alpha                      | A
      //
      // Type: (P) = packed
      //
      // 8 entries
      //
      //
      // The component resolution prefix indicates the internal data type:
      // - f is floating point
      // - i is signed integer
      // - ui is unsigned integer
      // - s is signed normalized fixed-point,
      // - no prefix is unsigned normalized fixed-point
      //
      // Sized                 |                    |                                     | Red  | Green | Blue | Alpha | Shared   | Pixel
      // Internal Format       | Format             | Type                                | Bits | Bits  | Bits | Bits  | Bits     | Format (-) means
      // this would be a conversion operation
      //-------------------------------------------------------------------------------------------------------------------------  |--------
      // GL_R8                 | GL_RED             | GL_UNSIGNED_BYTE                    | 8    |       |      |       |          | R8_UNORM
      // GL_R8_SNORM           | GL_RED             | GL_BYTE                             | s8   |       |      |       |          | R8_SNORM
      // GL_R16F               | GL_RED             | GL_HALF_FLOAT                       | f16  |       |      |       |          | R16_SFLOAT
      //                       |                    | GL_FLOAT                            |      |       |      |       |          | -
      // GL_R32F               | GL_RED             | GL_FLOAT                            | f32  |       |      |       |          | R32_SFLOAT
      // GL_R8UI               | GL_RED_INTEGER     | GL_UNSIGNED_BYTE                    | ui8  |       |      |       |          | R8_UINT
      // GL_R8I                | GL_RED_INTEGER     | GL_BYTE                             | i8   |       |      |       |          | R8_SINT
      // GL_R16UI              | GL_RED_INTEGER     | GL_UNSIGNED_SHORT                   | ui16 |       |      |       |          | R16_UINT
      // GL_R16I               | GL_RED_INTEGER     | GL_SHORT                            | i16  |       |      |       |          | R16_SINT
      // GL_R32UI              | GL_RED_INTEGER     | GL_UNSIGNED_INT                     | ui32 |       |      |       |          | R32_UINT
      // GL_R32I               | GL_RED_INTEGER     | GL_INT                              | i32  |       |      |       |          | R32_SINT
      // GL_RG8                | GL_RG              | GL_UNSIGNED_BYTE                    | 8    | 8     |      |       |          | R8G8_UNORM
      // GL_RG8_SNORM          | GL_RG              | GL_BYTE                             | s8   | s8    |      |       |          | R8G8_SNORM
      // GL_RG16F              | GL_RG              | GL_HALF_FLOAT                       | f16  | f16   |      |       |          | R16G16_SFLOAT
      //                       |                    | GL_FLOAT                            |      |       |      |       |          | -
      // GL_RG32F              | GL_RG              | GL_FLOAT                            | f32  | f32   |      |       |          | R32G32_SFLOAT
      // GL_RG8UI              | GL_RG_INTEGER      | GL_UNSIGNED_BYTE                    | ui8  | ui8   |      |       |          | R8G8_UINT
      // GL_RG8I               | GL_RG_INTEGER      | GL_BYTE                             | i8   | i8    |      |       |          | R8G8_SINT
      // GL_RG16UI             | GL_RG_INTEGER      | GL_UNSIGNED_SHORT                   | ui16 | ui16  |      |       |          | R16G16_UINT
      // GL_RG16I              | GL_RG_INTEGER      | GL_SHORT                            | i16  | i16   |      |       |          | R16G16_SINT
      // GL_RG32UI             | GL_RG_INTEGER      | GL_UNSIGNED_INT                     | ui32 | ui32  |      |       |          | R32G32_UINT
      // GL_RG32I              | GL_RG_INTEGER      | GL_INT                              | i32  | i32   |      |       |          | R32G32_SINT
      // GL_RGB8               | GL_RGB             | GL_UNSIGNED_BYTE                    | 8    | 8     | 8    |       |          | R8G8B8_UNORM
      // GL_SRGB8              | GL_RGB             | GL_UNSIGNED_BYTE                    | 8    | 8     | 8    |       |          | R8G8B8_SRGB
      // GL_RGB565             | GL_RGB             | GL_UNSIGNED_BYTE                    | 5    | 6     | 5    |       |          | - R5G6B5_UNORM
      //                       |                    | GL_UNSIGNED_SHORT_5_6_5 (P)         |      |       |      |       |          |
      //                       R5G6B5_UNORM_PACK16
      // GL_RGB8_SNORM         | GL_RGB             | GL_BYTE                             | s8   | s8    | s8   |       |          | R8G8B8_SNORM
      // GL_R11F_G11F_B10F     | GL_RGB             | GL_UNSIGNED_INT_10F_11F_11F_REV (P) | f11  | f11   | f10  |       |          |
      // B10G11R11_UFLOAT_PACK32??
      //                       |                    | GL_HALF_FLOAT                       |      |       |      |       |          | -
      //                       R11G11B10_SFLOAT |                    | GL_FLOAT                            |      |       |      |       |          |
      //                       - R11G11B10_SFLOAT
      // GL_RGB9_E5            | GL_RGB             | GL_UNSIGNED_INT_5_9_9_9_REV (P)     | 9    | 9     |9     | 5     |          |
      // E5B9G9R9_UNORM_PACK32??  (no vulkan format or incorrect name?)
      //                       |                    | GL_HALF_FLOAT                       |      |       |      |       |          | -
      //                       |                    | GL_FLOAT                            |      |       |      |       |          | -
      // GL_RGB16F             | GL_RGB             | GL_HALF_FLOAT                       | f16  | f16   | f16  |       |          | R16G16B16_SFLOAT
      //                       |                    | GL_FLOAT                            |      |       |      |       |          | -
      // GL_RGB32F             | GL_RGB             | GL_FLOAT                            | f32  | f32   | f32  |       |          | R32G32B32_SFLOAT
      // GL_RGB8UI             | GL_RGB_INTEGER     | GL_UNSIGNED_BYTE                    | ui8  | ui8   | ui8  |       |          | R8G8B8_UINT
      // GL_RGB8I              | GL_RGB_INTEGER     | GL_BYTE                             | i8   | i8    | i8   |       |          | R8G8B8_SINT
      // GL_RGB16UI            | GL_RGB_INTEGER     | GL_UNSIGNED_SHORT                   | ui16 | ui16  | ui16 |       |          | R16G16B16_UINT
      // GL_RGB16I             | GL_RGB_INTEGER     | GL_SHORT                            | i16  | i16   | i16  |       |          | R16G16B16_SINT
      // GL_RGB32UI            | GL_RGB_INTEGER     | GL_UNSIGNED_INT                     | ui32 | ui32  | ui32 |       |          | R32G32B32_UINT
      // GL_RGB32I             | GL_RGB_INTEGER     | GL_INT                              | i32  | i32   | i32  |       |          | R32G32B32_SINT
      // GL_RGBA8              | GL_RGBA            | GL_UNSIGNED_BYTE                    | 8    | 8     | 8    | 8     |          | R8G8B8A8_UNORM
      // GL_SRGB8_ALPHA8       | GL_RGBA            | GL_UNSIGNED_BYTE                    | 8    | 8     | 8    | 8     |          | R8G8B8A8_SRGB
      // GL_RGBA8_SNORM        | GL_RGBA            | GL_BYTE                             | s8   | s8    | s8   | s8    |          | R8G8B8A8_SNORM
      // GL_RGB5_A1            | GL_RGBA            | GL_UNSIGNED_SHORT_5_5_5_1 (P)       | 5    | 5     | 5    | 1     |          |
      // R5G5B5A1_UNORM_PACK16
      //                       |                    | GL_UNSIGNED_BYTE                    |      |       |      |       |          | - R5G5B5A1_UNORM?
      //                       |                    | GL_UNSIGNED_INT_2_10_10_10_REV (P)  |      |       |      |       |          | -
      //                       A2B10G10R10_UNORM_PACK32
      // GL_RGBA4              | GL_RGBA            | GL_UNSIGNED_SHORT_4_4_4_4 (P)       | 4    | 4     | 4    | 4     |          |
      // R4G4B4A4_UNORM_PACK16
      //                       |                    | GL_UNSIGNED_BYTE                    |      |       |      |       |          | - R4G4B4A4_UNORM
      // GL_RGB10_A2           | GL_RGBA            | GL_UNSIGNED_INT_2_10_10_10_REV (P)  | 10   | 10    | 10   | 2     |          |
      // A2B10G10R10_UNORM_PACK32 GL_RGBA16F            | GL_RGBA            | GL_HALF_FLOAT                       | f16  | f16   | f16  | f16   |
      // | R16G16B16A16_SFLOAT
      //                       |                    | GL_FLOAT                            |      |       |      |       |          | -
      // GL_RGBA32F            | GL_RGBA            | GL_FLOAT                            | f32  | f32   | f32  | f32   |          |
      // R32G32B32A32_SFLOAT GL_RGBA8UI            | GL_RGBA_INTEGER    | GL_UNSIGNED_BYTE                    | ui8  | ui8   | ui8  | ui8   |
      // | R8G8B8A8_UINT GL_RGBA8I             | GL_RGBA_INTEGER    | GL_BYTE                             | i8   | i8    | i8   | i8    |          |
      // R8G8B8A8_SINT GL_RGB10_A2UI         | GL_RGBA_INTEGER    | GL_UNSIGNED_INT_2_10_10_10_REV (P)  | ui10 | ui10  | ui10 | ui2   |          |
      // A2B10G10R10_UINT_PACK32 GL_RGBA16UI           | GL_RGBA_INTEGER    | GL_UNSIGNED_SHORT                   | ui16 | ui16  | ui16 | ui16  |
      // | R16G16B16A16_UINT GL_RGBA16I            | GL_RGBA_INTEGER    | GL_SHORT                            | i16  | i16   | i16  | i16   |
      // | R16G16B16A16_SINT GL_RGBA32I            | GL_RGBA_INTEGER    | GL_INT                              | i32  | i32   | i32  | i32   |
      // | R32G32B32A32_SINT GL_RGBA32UI           | GL_RGBA_INTEGER    | GL_UNSIGNED_INT                     | ui32 | ui32  | ui32 | ui32  |
      // | R32G32B32A32_UINT
      //
      // 61 entries (12 conversions, so 49 raw)
      //
      // Depth Formats
      // -------------
      //                       |                    |                                       | Depth | Stencil  | Pixel
      // Sized Internal Format | Format             | Type                                  | Bits  | Bits     | Format
      //------------------------------------------------------------------------------------|-------|----------|------------
      // GL_DEPTH_COMPONENT16  | GL_DEPTH_COMPONENT | GL_UNSIGNED_SHORT,                    | 16    |          | D16_UNORM
      //                       |                    | GL_UNSIGNED_INT                       |       |          | D16_UNORM
      // GL_DEPTH_COMPONENT24  | GL_DEPTH_COMPONENT | GL_UNSIGNED_INT                       | 24    |          | D24_UNORM
      // GL_DEPTH_COMPONENT32F | GL_DEPTH_COMPONENT | GL_FLOAT                              | f32   |          | D32_SFLOAT
      // GL_DEPTH24_STENCIL8   | GL_DEPTH_STENCIL   | GL_UNSIGNED_INT_24_8 (P)              | 24    | 8        | D24S8_UNORM_PACK32
      // GL_DEPTH32F_STENCIL8  | GL_DEPTH_STENCIL   | GL_FLOAT_32_UNSIGNED_INT_24_8_REV (P) | f32   | 8        | D32_SFLOAT_S8_UNORM
      //
      // 6 entries
      // 67 sized internal formats

      // https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glCompressedTexImage2D.xhtml

      // Table 1. Compressed Internal Formats
      // Compressed Internal Format                   | Base Internal Format | Image Size
      // GL_COMPRESSED_R11_EAC                        | GL_RED               | ceil(width / 4) * ceil(height / 4) * 8
      // GL_COMPRESSED_SIGNED_R11_EAC                 | GL_RED               | ceil(width / 4) * ceil(height / 4) * 8
      // GL_COMPRESSED_RG11_EAC                       | GL_RG                | ceil(width / 4) * ceil(height / 4) * 16
      // GL_COMPRESSED_SIGNED_RG11_EAC                | GL_RG                | ceil(width / 4) * ceil(height / 4) * 16
      // GL_COMPRESSED_RGB8_ETC2                      | GL_RGB               | ceil(width / 4) * ceil(height / 4) * 8
      // GL_COMPRESSED_SRGB8_ETC2                     | GL_RGB               | ceil(width / 4) * ceil(height / 4) * 8
      // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2  | GL_RGBA              | ceil(width / 4) * ceil(height / 4) * 8
      // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 | GL_RGBA              | ceil(width / 4) * ceil(height / 4) * 8
      // GL_COMPRESSED_RGBA8_ETC2_EAC                 | GL_RGBA              | ceil(width / 4) * ceil(height / 4) * 16
      // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          | GL_RGBA              | ceil(width / 4) * ceil(height / 4) * 16

      FormatGL g_convert[] = {
        // Internal Format       | Format             | Type                                | Bits | Bits  | Bits | Bits  | Bits     | Format (-)
        // means this would be a conversion operation
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           Undefined
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R4G4_UNORM_PACK8
        FormatGL(GL_RGBA4, GL_RGBA, GL_UNSIGNED_BYTE),    // GL_RGBA4              | GL_RGBA            | GL_UNSIGNED_SHORT_4_4_4_4 (P)       | 4    |
                                                          // 4     | 4    | 4     |          | R4G4B4A4_UNORM_PACK16
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B4G4R4A4_UNORM_PACK16
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R5G6B5_UNORM_PACK16
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B5G6R5_UNORM_PACK16
        FormatGL(GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1),    // GL_RGB5_A1            | GL_RGBA            | GL_UNSIGNED_SHORT_5_5_5_1 (P)
                                                                     // | 5    | 5     | 5    | 1     |          | R5G5B5A1_UNORM_PACK16
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B5G5R5A1_UNORM_PACK16
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A1R5G5B5_UNORM_PACK16
        FormatGL(GL_R8, GL_RED, GL_UNSIGNED_BYTE),    // GL_R8                 | GL_RED             | GL_UNSIGNED_BYTE                    | 8    |
                                                      // |      |       |          | R8_UNORM
        FormatGL(GL_R8_SNORM, GL_RED, GL_BYTE),       // GL_R8_SNORM           | GL_RED             | GL_BYTE                             | s8   |
                                                      // |      |       |          | R8_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8_SSCALED
        FormatGL(GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE),    // GL_R8UI               | GL_RED_INTEGER     | GL_UNSIGNED_BYTE                    |
                                                                // ui8  |       |      |       |          | R8_UINT
        FormatGL(GL_R8I, GL_RED_INTEGER, GL_BYTE),    // GL_R8I                | GL_RED_INTEGER     | GL_BYTE                             | i8   |
                                                      // |      |       |          | R8_SINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8_SRGB
        FormatGL(GL_RG8, GL_RG, GL_UNSIGNED_BYTE),    // GL_RG8                | GL_RG              | GL_UNSIGNED_BYTE                    | 8    | 8
                                                      // |      |       |          | R8G8_UNORM
        FormatGL(GL_RG8_SNORM, GL_RG, GL_BYTE),       // GL_RG8_SNORM          | GL_RG              | GL_BYTE                             | s8   | s8
                                                      // |      |       |          | R8G8_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8G8_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8G8_SSCALED
        FormatGL(GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE),    // GL_RG8UI              | GL_RG_INTEGER      | GL_UNSIGNED_BYTE                    |
                                                                // ui8  | ui8   |      |       |          | R8G8_UINT
        FormatGL(GL_RG8I, GL_RG_INTEGER, GL_BYTE),    // GL_RG8I               | GL_RG_INTEGER      | GL_BYTE                             | i8   | i8
                                                      // |      |       |          | R8G8_SINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8G8_SRGB
        FormatGL(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE),    // GL_RGB8               | GL_RGB             | GL_UNSIGNED_BYTE                    | 8    | 8
                                                        // | 8    |       |          | R8G8B8_UNORM
        FormatGL(GL_RGB8_SNORM, GL_RGB, GL_BYTE),    // GL_RGB8_SNORM         | GL_RGB             | GL_BYTE                             | s8   | s8
                                                     // | s8   |       |          | R8G8B8_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8G8B8_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8G8B8_SSCALED
        FormatGL(GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_BYTE),    // GL_RGB8UI             | GL_RGB_INTEGER     | GL_UNSIGNED_BYTE
                                                                  // | ui8  | ui8   | ui8  |       |          | R8G8B8_UINT
        FormatGL(GL_RGB8I, GL_RGB_INTEGER, GL_BYTE),     // GL_RGB8I              | GL_RGB_INTEGER     | GL_BYTE                             | i8   |
                                                         // i8    | i8   |       |          | R8G8B8_SINT
        FormatGL(GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE),    // GL_SRGB8              | GL_RGB             | GL_UNSIGNED_BYTE                    | 8    |
                                                         // 8     | 8    |       |          | R8G8B8_SRGB
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8_UNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8_SSCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8_SINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8_SRGB
        FormatGL(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE),    // GL_RGBA8              | GL_RGBA            | GL_UNSIGNED_BYTE                    | 8    |
                                                          // 8     | 8    | 8     |          | R8G8B8A8_UNORM
        FormatGL(GL_RGBA8_SNORM, GL_RGBA, GL_BYTE),    // GL_RGBA8_SNORM        | GL_RGBA            | GL_BYTE                             | s8   | s8
                                                       // | s8   | s8    |          | R8G8B8A8_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8G8B8A8_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R8G8B8A8_SSCALED
        FormatGL(GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE),    // GL_RGBA8UI            | GL_RGBA_INTEGER    | GL_UNSIGNED_BYTE
                                                                    // | ui8  | ui8   | ui8  | ui8   |          | R8G8B8A8_UINT
        FormatGL(GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE),    // GL_RGBA8I             | GL_RGBA_INTEGER    | GL_BYTE                             | i8   |
                                                          // i8    | i8   | i8    |          | R8G8B8A8_SINT
        FormatGL(GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE),    // GL_SRGB8_ALPHA8       | GL_RGBA            | GL_UNSIGNED_BYTE                    |
                                                                 // 8    | 8     | 8    | 8     |          | R8G8B8A8_SRGB
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8A8_UNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8A8_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8A8_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8A8_SSCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8A8_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8A8_SINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           B8G8R8A8_SRGB
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A8B8G8R8_UNORM_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A8B8G8R8_SNORM_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A8B8G8R8_USCALED_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A8B8G8R8_SSCALED_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A8B8G8R8_UINT_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A8B8G8R8_SINT_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A8B8G8R8_SRGB_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2R10G10B10_UNORM_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2R10G10B10_SNORM_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2R10G10B10_USCALED_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2R10G10B10_SSCALED_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2R10G10B10_UINT_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2R10G10B10_SINT_PACK32
        FormatGL(GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV),    // GL_RGB10_A2           | GL_RGBA            |
                                                                           // GL_UNSIGNED_INT_2_10_10_10_REV (P)  | 10   | 10    | 10   | 2     |
                                                                           // | A2B10G10R10_UNORM_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2B10G10R10_SNORM_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2B10G10R10_USCALED_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2B10G10R10_SSCALED_PACK32
        FormatGL(GL_RGB10_A2UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV),    // GL_RGB10_A2UI         | GL_RGBA_INTEGER    |
                                                                                     // GL_UNSIGNED_INT_2_10_10_10_REV (P)  | ui10 | ui10  | ui10 |
                                                                                     // ui2   |          | A2B10G10R10_UINT_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           A2B10G10R10_SINT_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16_UNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16_SSCALED
        FormatGL(GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT),    // GL_R16UI              | GL_RED_INTEGER     | GL_UNSIGNED_SHORT
                                                                  // | ui16 |       |      |       |          | R16_UINT
        FormatGL(GL_R16I, GL_RED_INTEGER, GL_SHORT),    // GL_R16I               | GL_RED_INTEGER     | GL_SHORT                            | i16  |
                                                        // |      |       |          | R16_SINT
        FormatGL(GL_R16F, GL_RED, GL_HALF_FLOAT),       // GL_R16F               | GL_RED             | GL_HALF_FLOAT                       | f16  |
                                                        // |      |       |          | R16_SFLOAT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16_UNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16_SSCALED
        FormatGL(GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT),    // GL_RG16UI             | GL_RG_INTEGER      | GL_UNSIGNED_SHORT
                                                                  // | ui16 | ui16  |      |       |          | R16G16_UINT
        FormatGL(GL_RG16I, GL_RG_INTEGER, GL_SHORT),    // GL_RG16I              | GL_RG_INTEGER      | GL_SHORT                            | i16  |
                                                        // i16   |      |       |          | R16G16_SINT
        FormatGL(GL_RG16F, GL_RG, GL_HALF_FLOAT),    // GL_RG16F              | GL_RG              | GL_HALF_FLOAT                       | f16  | f16
                                                     // |      |       |          | R16G16_SFLOAT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16B16_UNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16B16_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16B16_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16B16_SSCALED
        FormatGL(GL_RGB16UI, GL_RGB_INTEGER, GL_UNSIGNED_SHORT),    // GL_RGB16UI            | GL_RGB_INTEGER     | GL_UNSIGNED_SHORT
                                                                    // | ui16 | ui16  | ui16 |       |          | R16G16B16_UINT
        FormatGL(GL_RGB16I, GL_RGB_INTEGER, GL_SHORT),    // GL_RGB16I             | GL_RGB_INTEGER     | GL_SHORT                            | i16  |
                                                          // i16   | i16  |       |          | R16G16B16_SINT
        FormatGL(GL_RGB16F, GL_RGB, GL_HALF_FLOAT),       // GL_RGB16F             | GL_RGB             | GL_HALF_FLOAT                       | f16  |
                                                          // f16   | f16  |       |          | R16G16B16_SFLOAT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16B16A16_UNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16B16A16_SNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16B16A16_USCALED
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R16G16B16A16_SSCALED
        FormatGL(GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT),    // GL_RGBA16UI           | GL_RGBA_INTEGER    | GL_UNSIGNED_SHORT
                                                                      // | ui16 | ui16  | ui16 | ui16  |          | R16G16B16A16_UINT
        FormatGL(GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT),    // GL_RGBA16I            | GL_RGBA_INTEGER    | GL_SHORT                            | i16
                                                            // | i16   | i16  | i16   |          | R16G16B16A16_SINT
        FormatGL(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT),    // GL_RGBA16F            | GL_RGBA            | GL_HALF_FLOAT                       | f16  |
                                                         // f16   | f16  | f16   |          | R16G16B16A16_SFLOAT
        FormatGL(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT),    // GL_R32UI              | GL_RED_INTEGER     | GL_UNSIGNED_INT                     |
                                                                // ui32 |       |      |       |          | R32_UINT
        FormatGL(GL_R32I, GL_RED_INTEGER, GL_INT),    // GL_R32I               | GL_RED_INTEGER     | GL_INT                              | i32  |
                                                      // |      |       |          | R32_SINT
        FormatGL(GL_R32F, GL_RED, GL_FLOAT),    // GL_R32F               | GL_RED             | GL_FLOAT                            | f32  |       |
                                                // |       |          | R32_SFLOAT
        FormatGL(GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT),    // GL_RG32UI             | GL_RG_INTEGER      | GL_UNSIGNED_INT                     |
                                                                // ui32 | ui32  |      |       |          | R32G32_UINT
        FormatGL(GL_RG32I, GL_RG_INTEGER, GL_INT),    // GL_RG32I              | GL_RG_INTEGER      | GL_INT                              | i32  | i32
                                                      // |      |       |          | R32G32_SINT
        FormatGL(GL_RG32F, GL_RG, GL_FLOAT),    // GL_RG32F              | GL_RG              | GL_FLOAT                            | f32  | f32   |
                                                // |       |          | R32G32_SFLOAT
        FormatGL(GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT),    // GL_RGB32UI            | GL_RGB_INTEGER     | GL_UNSIGNED_INT
                                                                  // | ui32 | ui32  | ui32 |       |          | R32G32B32_UINT
        FormatGL(GL_RGB32I, GL_RGB_INTEGER, GL_INT),    // GL_RGB32I             | GL_RGB_INTEGER     | GL_INT                              | i32  |
                                                        // i32   | i32  |       |          | R32G32B32_SINT
        FormatGL(GL_RGB32F, GL_RGB, GL_FLOAT),    // GL_RGB32F             | GL_RGB             | GL_FLOAT                            | f32  | f32   |
                                                  // f32  |       |          | R32G32B32_SFLOAT
        FormatGL(GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT),    // GL_RGBA32UI           | GL_RGBA_INTEGER    | GL_UNSIGNED_INT
                                                                    // | ui32 | ui32  | ui32 | ui32  |          | R32G32B32A32_UINT
        FormatGL(GL_RGBA32I, GL_RGBA_INTEGER, GL_INT),    // GL_RGBA32I            | GL_RGBA_INTEGER    | GL_INT                              | i32  |
                                                          // i32   | i32  | i32   |          | R32G32B32A32_SINT
        FormatGL(GL_RGBA32F, GL_RGBA, GL_FLOAT),    // GL_RGBA32F            | GL_RGBA            | GL_FLOAT                            | f32  | f32
                                                    // | f32  | f32   |          | R32G32B32A32_SFLOAT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64_SINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64_SFLOAT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64_SINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64_SFLOAT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64B64_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64B64_SINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64B64_SFLOAT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64B64A64_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64B64A64_SINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           R64G64B64A64_SFLOAT
        FormatGL(GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV),    // GL_R11F_G11F_B10F     | GL_RGB             |
                                                                                 // GL_UNSIGNED_INT_10F_11F_11F_REV (P) | f11  | f11   | f10  |
                                                                                 // |          | B10G11R11_UFLOAT_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           E5B9G9R9_UFLOAT_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           D16_UNORM
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           X8_D24_UNORM_PACK32
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           D32_SFLOAT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           S8_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           D16_UNORM_S8_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           D24_UNORM_S8_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           D32_SFLOAT_S8_UINT
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC1_RGB_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC1_RGB_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC1_RGBA_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC1_RGBA_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC2_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC2_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC3_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC3_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC4_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC4_SNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC5_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC5_SNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC6H_UFLOAT_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC6H_SFLOAT_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC7_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           BC7_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGB8_ETC2, GL_RGB,
                 FormatState::Compressed),    // GL_COMPRESSED_RGB8_ETC2                      | GL_RGB | ETC2_R8G8B8_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ETC2, GL_RGB,
                 FormatState::Compressed),    // GL_COMPRESSED_SRGB8_ETC2                     | GL_RGB | ETC2_R8G8B8_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA,
                 FormatState::Compressed),    // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2  | GL_RGBA | ETC2_R8G8B8A1_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA,
                 FormatState::Compressed),    // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 | GL_RGBA | ETC2_R8G8B8A1_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_RGBA8_ETC2_EAC, GL_RGBA,
                 FormatState::Compressed),    // GL_COMPRESSED_RGBA8_ETC2_EAC                 | GL_RGBA | ETC2_R8G8B8A8_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_RGBA,
                 FormatState::Compressed),    // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          | GL_RGBA | ETC2_R8G8B8A8_SRGB_BLOCK
        FormatGL(GL_COMPRESSED_R11_EAC, GL_RED,
                 FormatState::Compressed),    // GL_COMPRESSED_R11_EAC                        | GL_RED | EAC_R11_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SIGNED_R11_EAC, GL_RED,
                 FormatState::Compressed),    // GL_COMPRESSED_SIGNED_R11_EAC                 | GL_RED | EAC_R11_SNORM_BLOCK
        FormatGL(GL_COMPRESSED_RG11_EAC, GL_RG,
                 FormatState::Compressed),    // GL_COMPRESSED_RG11_EAC                       | GL_RG | EAC_R11G11_UNORM_BLOCK
        FormatGL(GL_COMPRESSED_SIGNED_RG11_EAC, GL_RG,
                 FormatState::Compressed),    // GL_COMPRESSED_SIGNED_RG11_EAC                | GL_RG | EAC_R11G11_SNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_4x4_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_4x4_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_5x4_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_5x4_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_5x5_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_5x5_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_6x5_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_6x5_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_6x6_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_6x6_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_8x5_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_8x5_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_8x6_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_8x6_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_8x8_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_8x8_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_10x5_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_10x5_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_10x6_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_10x6_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_10x8_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_10x8_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_10x10_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_10x10_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_12x10_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_12x10_SRGB_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_12x12_UNORM_BLOCK
        FormatGL(),    //                                                                                                                           |
        //                                                                                                                           ASTC_12x12_SRGB_BLOCK
        // Extended formats
        FormatGL(GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE),    // GL_ALPHA              | GL_ALPHA           | GL_UNSIGNED_BYTE              | Alpha
                                                           // | A               | EX_ALPHA8_UINT
        FormatGL(GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE),    // GL_LUMINANCE          | GL_LUMINANCE       | GL_UNSIGNED_BYTE              |
                                                                   // Luminance                  | L               | EX_LUMINANCE8_UINT
        FormatGL(GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE),    // GL_LUMINANCE_ALPHA    | GL_LUMINANCE_ALPHA | GL_UNSIGNED_BYTE
                                                                               // | Luminance, Alpha           | L, A            |
                                                                               // EX_LUMINANCE8_ALPHA8_UINT
      };

      // Do some sanity checking
      static_assert(static_cast<uint32_t>(sizeof(g_convert) / sizeof(FormatGL)) == static_cast<uint32_t>(PixelFormat::ENUM_ID_RANGE_SIZE),
                    "g_convert needs to match the size of the enum range");


      GLRawBitmapUtil::Result ConvertPixelFormat(const PixelFormat pixelFormat, const uint32_t width, const uint32_t stride)
      {
        assert(stride >= width);

        // Simplified format conversion
        const auto bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(pixelFormat);
        const auto index = PixelFormatUtil::GetFormatRangeIndex(pixelFormat);
        const auto format = g_convert[index];
        if (format.State == FormatState::Normal)
        {
          if (format.Alignment == ByteAlignment::Find)
          {
            return GLRawBitmapUtil::Result(format.InternalFormat, format.Format, format.Type,
                                           RawBitmapUtil::CalcAlignment(pixelFormat, width, stride, bytesPerPixel));
          }
          return GLRawBitmapUtil::Result(format.InternalFormat, format.Format, format.Type, static_cast<GLint>(format.Alignment));
        }


        throw UnsupportedPixelFormatException(pixelFormat);
      }


      GLRawBitmapUtil::CompressedResult ConvertCompressedPixelFormat(const PixelFormat pixelFormat, const uint32_t width)
      {
        // Simplified format conversion
        const auto index = PixelFormatUtil::GetFormatRangeIndex(pixelFormat);
        const auto format = g_convert[index];
        if (format.State == FormatState::Compressed && format.Alignment != ByteAlignment::Find)
        {
          return GLRawBitmapUtil::CompressedResult(format.InternalFormat, format.Format, static_cast<GLint>(format.Alignment));
        }
        throw UnsupportedPixelFormatException(pixelFormat);
      }
    }


    GLRawBitmapUtil::Result GLRawBitmapUtil::Convert(const PixelFormat pixelFormat, const uint32_t width, const uint32_t stride,
                                                     const bool exactMatch)
    {
      if (stride < width)
      {
        throw UnsupportedStrideException(stride);
      }

      if (!exactMatch)
      {
        // Since no exact match is required we let OpenGL chose a internal format
        switch (pixelFormat)
        {
        case PixelFormat::EX_ALPHA8_UNORM:
          return Result(GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE,
                        RawBitmapUtil::CalcAlignment(pixelFormat, width, stride,
                                                     1));    // GL_ALPHA              | GL_ALPHA           | GL_UNSIGNED_BYTE              | Alpha | A
        case PixelFormat::EX_LUMINANCE8_UNORM:
          return Result(
            GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,
            RawBitmapUtil::CalcAlignment(pixelFormat, width, stride,
                                         1));    // GL_LUMINANCE          | GL_LUMINANCE       | GL_UNSIGNED_BYTE              | Luminance | L
        case PixelFormat::EX_LUMINANCE8_ALPHA8_UNORM:
          return Result(GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
                        RawBitmapUtil::CalcAlignment(
                          pixelFormat, width, stride,
                          2));    // GL_LUMINANCE_ALPHA    | GL_LUMINANCE_ALPHA | GL_UNSIGNED_BYTE              | Luminance, Alpha           | L, A
        default:
          break;
        }

        // switch (PixelFormatUtil::GetPixelFormatLayout(pixelFormat))
        //{
        // case PixelFormatLayout::R8G8B8:
        //  return Result(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, RawBitmapUtil::CalcAlignment(pixelFormat, width, stride, 3));
        //  // GL_RGB                | GL_RGB             | GL_UNSIGNED_BYTE,             | Red, Green, Blue           | R, G, B
        // case PixelFormatLayout::R8G8B8A8:
        //  return Result(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, RawBitmapUtil::CalcAlignment(pixelFormat, width, stride, 4));
        //  // GL_RGBA               | GL_RGBA            | GL_UNSIGNED_BYTE,             | Red, Green, Blue, Alpha    | R, G, B, A
        // default:
        //  break;
        //}
      }
      return ConvertPixelFormat(pixelFormat, width, stride);
    }


    GLRawBitmapUtil::CompressedResult GLRawBitmapUtil::ConvertCompressed(const PixelFormat pixelFormat, const uint32_t width)
    {
      return ConvertCompressedPixelFormat(pixelFormat, width);
    }
  }


  namespace
  {
    // https://www.khronos.org/registry/gles/specs/3.2/es_spec_3.2.pdf
    // 8.4.2.1 Unpacking
    // Data are taken from the currently bound pixel unpack buffer or client memory as a sequence of
    // - signed or unsigned bytes (GL data types byte and ubyte)
    // - signed or unsigned short integers (GL data types short and ushort)
    // - signed or unsigned integers (GL data types int and uint)
    // - floating-point values (GL data types half and float).
    //
    // These elements are grouped into sets of one, two, three, or four values, depending on the format,
    //
    // Table 8.5 summarizes the format of groups obtained from memory; it also indicates those formats that yield
    // indices and those that yield floating - point or integer components.
    //
    // The values of each GL data type are interpreted as they would be specified in the language of the clients GL binding
    //
    //  8.4.2.3 Conversion to floating - point
    //  This step applies only to groups of floating - point components.It is not performed
    //  on indices or integer components.For groups containing both components and
    //  indices, such as DEPTH_STENCIL, the indices are not converted.
    //  Each element in a group is converted to a floating - point value.For unsigned
    //  or signed normalized fixed - point elements, equations 2.1 or 2.2, respectively, are
    //  used.
    //
    //  8.4.2.4 Conversion to RGB
    //  This step is applied only if the format is LUMINANCE or LUMINANCE_ALPHA.If
    //  the format is LUMINANCE, then each group of one element is converted to a group
    //  of R, G, and B(three) elements by copying the original single element into each of
    //  the three new elements.If the format is LUMINANCE_ALPHA, then each group of
    //  two elements is converted to a group of R, G, B, and A(four) elements by copying
    //  the first original element into each of the first three new elements and copying the
    //  second original element to the A(fourth) new element.
    //
    //  8.4.2.5 Final Expansion to RGBA
    //  This step is performed only for non - depth component groups.Each group is converted
    //  to a group of 4 elements as follows : if a group does not contain an A element,
    //  then A is added and set to one for integer components or 1.0 for floating - point components.
    //  If any of R, G, or B is missing from the group, each missing element is
    //  added and assigned a value of 0 for integer components or 0.0 for floating - point
    //  components.


    // https://www.khronos.org/registry/gles/specs/3.0/es_spec_3.0.4.pdf

    // Table 3.2: Valid combinations of format, type, and sized internal format
    //                   |                                    | External  |
    //                   |                                    | bytes     | Internal
    // Format            | Type                               | Per Pixel | Format
    // --------------------------------------------------------------------------------
    // RGBA              | UNSIGNED_BYTE                      | 4         | RGBA8,
    //                   |                                    |           | RGB5_A1,
    //                   |                                    |           | RGBA4,
    //                   |                                    |           | SRGB8_ALPHA8
    // RGBA              | BYTE                               | 4         | RGBA8_SNORM
    // RGBA              | UNSIGNED_SHORT_4_4_4_4 (P)         | 2         | RGBA4
    // RGBA              | UNSIGNED_SHORT_5_5_5_1 (P)         | 2         | RGB5_A1
    // RGBA              | UNSIGNED_INT_2_10_10_10_REV (P)    | 4         | RGB10_A2,
    //                   |                                    |           | RGB5_A1
    // RGBA              | HALF_FLOAT                         | 8         | RGBA16F
    // RGBA              | FLOAT                              | 16        | RGBA32F,
    //                   |                                    |           | RGBA16F
    // RGBA_INTEGER      | UNSIGNED_BYTE                      | 4         | RGBA8UI
    // RGBA_INTEGER      | BYTE                               | 4         | RGBA8I
    // RGBA_INTEGER      | UNSIGNED_SHORT                     | 8         | RGBA16UI
    // RGBA_INTEGER      | SHORT                              | 8         | RGBA16I
    // RGBA_INTEGER      | UNSIGNED_INT                       | 16        | RGBA32UI
    // RGBA_INTEGER      | INT                                | 16        | RGBA32I
    // RGBA_INTEGER      | UNSIGNED_INT_2_10_10_10_REV (P)    | 4         | RGB10_A2UI
    // RGB               | UNSIGNED_BYTE                      | 3         | RGB8,
    //                   |                                    |           | RGB565,
    //                   |                                    |           | SRGB8
    // RGB               | BYTE                               | 3         | RGB8_SNORM
    // RGB               | UNSIGNED_SHORT_5_6_5 (P)           | 2         | RGB565
    // RGB               | UNSIGNED_INT_10F_11F_11F_REV (P)   | 4         | R11F_G11F_B10F
    // RGB               | UNSIGNED_INT_5_9_9_9_REV (P)       | 4         | RGB9_E5
    // RGB               | HALF_FLOAT                         | 6         | RGB16F,
    //                   |                                    |           | R11F_G11F_B10F,
    //                   |                                    |           | RGB9_E5
    // RGB               | FLOAT                              | 12        | RGB32F,
    //                   |                                    |           | RGB16F,
    //                   |                                    |           | R11F_G11F_B10F,
    //                   |                                    |           | RGB9_E5
    // RGB_INTEGER       | UNSIGNED_BYTE                      | 3         | RGB8UI
    // RGB_INTEGER       | BYTE                               | 3         | RGB8I
    // RGB_INTEGER       | UNSIGNED_SHORT                     | 6         | RGB16UI
    // RGB_INTEGER       | SHORT                              | 6         | RGB16I
    // RGB_INTEGER       | UNSIGNED_INT                       | 12        | RGB32UI
    // RGB_INTEGER       | INT                                | 12        | RGB32I
    // RG                | UNSIGNED_BYTE                      | 2         | RG8
    // RG                | BYTE                               | 2         | RG8_SNORM
    // RG                | HALF_FLOAT                         | 4         | RG16F
    // RG                | FLOAT                              | 8         | RG32F,
    //                   |                                    |           | RG16F
    // RG_INTEGER        | UNSIGNED_BYTE                      | 2         | RG8UI
    // RG_INTEGER        | BYTE                               | 2         | RG8I
    // RG_INTEGER        | UNSIGNED_SHORT                     | 4         | RG16UI
    // RG_INTEGER        | SHORT                              | 4         | RG16I
    // RG_INTEGER        | UNSIGNED_INT                       | 8         | RG32UI
    // RG_INTEGER        | INT                                | 8         | RG32I
    // RED               | UNSIGNED_BYTE                      | 1         | R8
    // RED               | BYTE                               | 1         | R8_SNORM
    // RED               | HALF_FLOAT                         | 2         | R16F
    // RED               | FLOAT                              | 4         | R32F,
    //                   |                                    |           | R16F
    // RED_INTEGER       | UNSIGNED_BYTE                      | 1         | R8UI
    // RED_INTEGER       | BYTE                               | 1         | R8I
    // RED_INTEGER       | UNSIGNED_SHORT                     | 2         | R16UI
    // RED_INTEGER       | SHORT                              | 2         | R16I
    // RED_INTEGER       | UNSIGNED_INT                       | 4         | R32UI
    // RED_INTEGER       | INT                                | 4         | R32I
    // DEPTH_COMPONENT   | UNSIGNED_SHORT                     | 2         | DEPTH_COMPONENT16
    // DEPTH_COMPONENT   | UNSIGNED_INT                       | 4         | DEPTH_COMPONENT24,
    //                   |                                    |           | DEPTH_COMPONENT16
    // DEPTH_COMPONENT   | FLOAT                              | 4         | DEPTH_COMPONENT32F
    // DEPTH_STENCIL     | UNSIGNED_INT_24_8 (P)              | 4         | DEPTH24_STENCIL8
    // DEPTH_STENCIL     | FLOAT_32_UNSIGNED_INT_24_8_REV (P) | 8         | DEPTH32F_STENCIL8
    //
    // 67 entries

    // Table 3.3: Valid combinations of format, type, and unsized internal format
    //                   |                                | External  |
    //                   |                                | bytes     | Internal
    // Format            | Type                           | Per Pixel | Format
    // ---------------------------------------------------------------------------
    // RGBA              | UNSIGNED_BYTE                  | 4         | RGBA
    // RGBA              | UNSIGNED_SHORT_4_4_4_4 (P)     | 2         | RGBA
    // RGBA              | UNSIGNED_SHORT_5_5_5_1 (P)     | 2         | RGBA
    // RGB               | UNSIGNED_BYTE                  | 3         | RGB
    // RGB               | UNSIGNED_SHORT_5_6_5  (P)      | 2         | RGB
    // LUMINANCE_ALPHA   | UNSIGNED_BYTE                  | 2         | LUMINANCE_ALPHA
    // LUMINANCE         | UNSIGNED_BYTE                  | 1         | LUMINANCE
    // ALPHA             | UNSIGNED_BYTE                  | 1         | ALPHA
    //
    // 8 entries

    // Table 8.13:
    // Correspondence of sized internal color formats to base internal formats,
    // internal data type, minimum component resolutions, and use cases for each sized
    // internal format.
    //
    // The component resolution prefix indicates the internal data type:
    // - f is floating point
    // - i is signed integer
    // - ui is unsigned integer
    // - s is signed normalized fixed-point,
    // - no prefix is unsigned normalized fixed-point

    // The "CR"(color - renderable), "TF"(texture - filterable), "Req.rend."
    // and "Req.tex." columns are described in sections 9.4, 8.16, 9.2.5,
    // and 8.5.1, respectively.

    // Sized          | Base    | Bits/component                   | CR | TF | Req.  | Req.
    // Internal       | Internal| S are shared bits                |    |    | rend. | tex.  | Pixel
    // Format         | Format  | R    | G    | B    | A    | S    |    |    |       |       | Format
    // ------------------------------------------------------------------------------------- |--------
    // R8             | RED     | 8    |      |      |      |      | X  | X  | X     | X     | R8_UNORM
    // R8_SNORM       | RED     | s8   |      |      |      |      |    | X  |       | X     | R8_SNORM
    // RG8            | RG      | 8    | 8    |      |      |      | X  | X  | X     | X     | R8G8_UNORM
    // RG8_SNORM      | RG      | s8   | s8   |      |      |      |    | X  |       | X     | R8G8_SNORM
    // RGB8           | RGB     | 8    | 8    | 8    |      |      | X  | X  | X     | X     | R8G8B8_UNORM
    // RGB8_SNORM     | RGB     | s8   | s8   | s8   |      |      |    | X  |       | X     | R8G8B8_SNORM
    // RGB565         | RGB     | 5    | 6    | 5    |      |      | X  | X  | X     | X     | R5G6B5_UNORM
    // RGBA4          | RGBA    | 4    | 4    | 4    | 4    |      | X  | X  | X     | X     | R4G4B4A4_UNORM
    // RGB5_A1        | RGBA    | 5    | 5    | 5    | 1    |      | X  | X  | X     | X     | R5G5B5A1_UNORM
    // RGBA8          | RGBA    | 8    | 8    | 8    | 8    |      | X  | X  | X     | X     | R8G8B8A8_UNORM
    // RGBA8_SNORM    | RGBA    | s8   | s8   | s8   | s8   |      |    | X  |       | X     | R8G8B8A8_SNORM
    // RGB10_A2       | RGBA    | 10   | 10   | 10   | 2    |      | X  | X  | X     | X     | R10G10B10A2_UNORM
    // RGB10_A2UI     | RGBA    | ui10 | ui10 | ui10 | ui2  |      | X  |    | X     | X     | R10G10B10A2
    // SRGB8          | RGB     | 8    | 8    | 8    |      |      |    | X  |       | X     | R8G8B8_SRGB
    // SRGB8_ALPHA8   | RGBA    | 8    | 8    | 8    | 8    |      | X  | X  | X     | X     | R8G8B8A8_SRGB
    // R16F           | RED     | f16  |      |      |      |      |    | X  |       | X     | R16_SFLOAT
    // RG16F          | RG      | f16  | f16  |      |      |      |    | X  |       | X     | R16G16_SFLOAT
    // RGB16F         | RGB     | f16  | f16  | f16  |      |      |    | X  |       | X     | R16G16B16_SFLOAT
    // RGBA16F        | RGBA    | f16  | f16  | f16  | f16  |      |    | X  |       | X     | R16G16B16A16_SFLOAT
    // R32F           | RED     | f32  |      |      |      |      |    |    |       | X     | R32_SFLOAT
    // RG32F          | RG      | f32  | f32  |      |      |      |    |    |       | X     | R32G32_SFLOAT
    // RGB32F         | RGB     | f32  | f32  | f32  |      |      |    |    |       | X     | R32G32B32_SFLOAT
    // RGBA32F        | RGBA    | f32  | f32  | f32  | f32  |      |    |    |       | X     | R32G32B32A32_SFLOAT
    // R11F_G11F_B10F | RGB     | f11  | f11  | f10  |      |      |    | X  |       | X     | R11G11B10_SFLOAT
    // RGB9_E5        | RGB     | 9    | 9    | 9    |      | 5    |    | X  |       | X     | R9G9B9 ?_UNORM
    // R8I            | RED     | i8   |      |      |      |      | X  |    | X     | X     | R8_SINT
    // R8UI           | RED     | ui8  |      |      |      |      | X  |    | X     | X     | R8_UINT
    // R16I           | RED     | i16  |      |      |      |      | X  |    | X     | X     | R16_SINT
    // R16UI          | RED     | ui16 |      |      |      |      | X  |    | X     | X     | R16_UINT
    // R32I           | RED     | i32  |      |      |      |      | X  |    | X     | X     | R32_SINT
    // R32UI          | RED     | ui32 |      |      |      |      | X  |    | X     | X     | R32_UINT
    // RG8I           | RG      | i8   | i8   |      |      |      | X  |    | X     | X     | R8G8_SINT
    // RG8UI          | RG      | ui8  | ui8  |      |      |      | X  |    | X     | X     | R8G8_UINT
    // RG16I          | RG      | i16  | i16  |      |      |      | X  |    | X     | X     | R16G16_SINT
    // RG16UI         | RG      | ui16 | ui16 |      |      |      | X  |    | X     | X     | R16G16_UINT
    // RG32I          | RG      | i32  | i32  |      |      |      | X  |    | X     | X     | R32G32_SINT
    // RG32UI         | RG      | ui32 | ui32 |      |      |      | X  |    | X     | X     | R32G32_UINT
    // RGB8I          | RGB     | i8   | i8   | i8   |      |      |    |    |       | X     | R8G8B8_SINT
    // RGB8UI         | RGB     | ui8  | ui8  | ui8  |      |      |    |    |       | X     | R8G8B8_UINT
    // RGB16I         | RGB     | i16  | i16  | i16  |      |      |    |    |       | X     | R16G16B16_UINT
    // RGB16UI        | RGB     | ui16 | ui16 | ui16 |      |      |    |    |       | X     | R16G16B16_UINT
    // RGB32I         | RGB     | i32  | i32  | i32  |      |      |    |    |       | X     | R32G32B32_SINT
    // RGB32UI        | RGB     | ui32 | ui32 | ui32 |      |      |    |    |       | X     | R32G32B32_UINT
    // RGBA8I         | RGBA    | i8   | i8   | i8   | i8   |      | X  |    | X     | X     | R8G8B8A8_SINT
    // RGBA8UI        | RGBA    | ui8  | ui8  | ui8  | ui8  |      | X  |    | X     | X     | R8G8B8A8_UINT
    // RGBA16I        | RGBA    | i16  | i16  | i16  | i16  |      | X  |    | X     | X     | R16G16B16A16_SINT
    // RGBA16UI       | RGBA    | ui16 | ui16 | ui16 | ui16 |      | X  |    | X     | X     | R16G16B16A16_UINT
    // RGBA32I        | RGBA    | i32  | i32  | i32  | i32  |      | X  |    | X     | X     | R32G32B32A32_SINT
    //
    // 48 entries

    // Some cut and pasted verification checks just to be 100% on the safe side and its a compile time check so its fine


#define LOCAL_ERROR_MESSAGE "The index did not match our assumption"

    // Due to lack of constexpr we use this nasty macro
    // So because of the macros be sure to keep all real code above this so you dont get any interference from them
    //#define LOCAL_GET_PIXELFORMAT_ID(X) ((static_cast<uint32_t>((X)) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
    // static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE))
    constexpr int LOCAL_GET_PIXELFORMAT_ID(const PixelFormat pf)
    {
      return ((static_cast<uint32_t>((pf)) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
              static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE));
    }

    // Some cut and pasted verification checks just to be 100% on the safe side and its a compile time check so its fine
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::Undefined) == 0, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R4G4_UNORM_PACK8) == 1, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R4G4B4A4_UNORM_PACK16) == 2, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B4G4R4A4_UNORM_PACK16) == 3, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R5G6B5_UNORM_PACK16) == 4, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B5G6R5_UNORM_PACK16) == 5, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R5G5B5A1_UNORM_PACK16) == 6, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B5G5R5A1_UNORM_PACK16) == 7, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A1R5G5B5_UNORM_PACK16) == 8, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8_UNORM) == 9, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8_SNORM) == 10, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8_USCALED) == 11, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8_SSCALED) == 12, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8_UINT) == 13, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8_SINT) == 14, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8_SRGB) == 15, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8_UNORM) == 16, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8_SNORM) == 17, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8_USCALED) == 18, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8_SSCALED) == 19, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8_UINT) == 20, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8_SINT) == 21, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8_SRGB) == 22, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8_UNORM) == 23, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8_SNORM) == 24, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8_USCALED) == 25, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8_SSCALED) == 26, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8_UINT) == 27, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8_SINT) == 28, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8_SRGB) == 29, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8_UNORM) == 30, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8_SNORM) == 31, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8_USCALED) == 32, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8_SSCALED) == 33, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8_UINT) == 34, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8_SINT) == 35, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8_SRGB) == 36, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8A8_UNORM) == 37, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8A8_SNORM) == 38, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8A8_USCALED) == 39, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8A8_SSCALED) == 40, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8A8_UINT) == 41, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8A8_SINT) == 42, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R8G8B8A8_SRGB) == 43, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8A8_UNORM) == 44, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8A8_SNORM) == 45, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8A8_USCALED) == 46, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8A8_SSCALED) == 47, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8A8_UINT) == 48, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8A8_SINT) == 49, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B8G8R8A8_SRGB) == 50, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A8B8G8R8_UNORM_PACK32) == 51, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A8B8G8R8_SNORM_PACK32) == 52, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A8B8G8R8_USCALED_PACK32) == 53, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A8B8G8R8_SSCALED_PACK32) == 54, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A8B8G8R8_UINT_PACK32) == 55, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A8B8G8R8_SINT_PACK32) == 56, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A8B8G8R8_SRGB_PACK32) == 57, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2R10G10B10_UNORM_PACK32) == 58, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2R10G10B10_SNORM_PACK32) == 59, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2R10G10B10_USCALED_PACK32) == 60, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2R10G10B10_SSCALED_PACK32) == 61, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2R10G10B10_UINT_PACK32) == 62, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2R10G10B10_SINT_PACK32) == 63, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2B10G10R10_UNORM_PACK32) == 64, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2B10G10R10_SNORM_PACK32) == 65, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2B10G10R10_USCALED_PACK32) == 66, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2B10G10R10_SSCALED_PACK32) == 67, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2B10G10R10_UINT_PACK32) == 68, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::A2B10G10R10_SINT_PACK32) == 69, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16_UNORM) == 70, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16_SNORM) == 71, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16_USCALED) == 72, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16_SSCALED) == 73, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16_UINT) == 74, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16_SINT) == 75, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16_SFLOAT) == 76, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16_UNORM) == 77, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16_SNORM) == 78, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16_USCALED) == 79, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16_SSCALED) == 80, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16_UINT) == 81, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16_SINT) == 82, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16_SFLOAT) == 83, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16_UNORM) == 84, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16_SNORM) == 85, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16_USCALED) == 86, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16_SSCALED) == 87, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16_UINT) == 88, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16_SINT) == 89, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16_SFLOAT) == 90, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16A16_UNORM) == 91, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16A16_SNORM) == 92, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16A16_USCALED) == 93, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16A16_SSCALED) == 94, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16A16_UINT) == 95, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16A16_SINT) == 96, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R16G16B16A16_SFLOAT) == 97, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32_UINT) == 98, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32_SINT) == 99, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32_SFLOAT) == 100, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32_UINT) == 101, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32_SINT) == 102, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32_SFLOAT) == 103, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32B32_UINT) == 104, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32B32_SINT) == 105, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32B32_SFLOAT) == 106, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32B32A32_UINT) == 107, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32B32A32_SINT) == 108, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R32G32B32A32_SFLOAT) == 109, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64_UINT) == 110, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64_SINT) == 111, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64_SFLOAT) == 112, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64_UINT) == 113, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64_SINT) == 114, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64_SFLOAT) == 115, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64B64_UINT) == 116, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64B64_SINT) == 117, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64B64_SFLOAT) == 118, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64B64A64_UINT) == 119, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64B64A64_SINT) == 120, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::R64G64B64A64_SFLOAT) == 121, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::B10G11R11_UFLOAT_PACK32) == 122, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::E5B9G9R9_UFLOAT_PACK32) == 123, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::D16_UNORM) == 124, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::X8_D24_UNORM_PACK32) == 125, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::D32_SFLOAT) == 126, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::S8_UINT) == 127, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::D16_UNORM_S8_UINT) == 128, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::D24_UNORM_S8_UINT) == 129, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::D32_SFLOAT_S8_UINT) == 130, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC1_RGB_UNORM_BLOCK) == 131, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC1_RGB_SRGB_BLOCK) == 132, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC1_RGBA_UNORM_BLOCK) == 133, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC1_RGBA_SRGB_BLOCK) == 134, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC2_UNORM_BLOCK) == 135, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC2_SRGB_BLOCK) == 136, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC3_UNORM_BLOCK) == 137, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC3_SRGB_BLOCK) == 138, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC4_UNORM_BLOCK) == 139, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC4_SNORM_BLOCK) == 140, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC5_UNORM_BLOCK) == 141, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC5_SNORM_BLOCK) == 142, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC6H_UFLOAT_BLOCK) == 143, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC6H_SFLOAT_BLOCK) == 144, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC7_UNORM_BLOCK) == 145, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::BC7_SRGB_BLOCK) == 146, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ETC2_R8G8B8_UNORM_BLOCK) == 147, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ETC2_R8G8B8_SRGB_BLOCK) == 148, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK) == 149, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK) == 150, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK) == 151, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK) == 152, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::EAC_R11_UNORM_BLOCK) == 153, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::EAC_R11_SNORM_BLOCK) == 154, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::EAC_R11G11_UNORM_BLOCK) == 155, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::EAC_R11G11_SNORM_BLOCK) == 156, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_4x4_UNORM_BLOCK) == 157, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_4x4_SRGB_BLOCK) == 158, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_5x4_UNORM_BLOCK) == 159, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_5x4_SRGB_BLOCK) == 160, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_5x5_UNORM_BLOCK) == 161, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_5x5_SRGB_BLOCK) == 162, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_6x5_UNORM_BLOCK) == 163, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_6x5_SRGB_BLOCK) == 164, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_6x6_UNORM_BLOCK) == 165, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_6x6_SRGB_BLOCK) == 166, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_8x5_UNORM_BLOCK) == 167, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_8x5_SRGB_BLOCK) == 168, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_8x6_UNORM_BLOCK) == 169, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_8x6_SRGB_BLOCK) == 170, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_8x8_UNORM_BLOCK) == 171, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_8x8_SRGB_BLOCK) == 172, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_10x5_UNORM_BLOCK) == 173, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_10x5_SRGB_BLOCK) == 174, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_10x6_UNORM_BLOCK) == 175, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_10x6_SRGB_BLOCK) == 176, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_10x8_UNORM_BLOCK) == 177, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_10x8_SRGB_BLOCK) == 178, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_10x10_UNORM_BLOCK) == 179, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_10x10_SRGB_BLOCK) == 180, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_12x10_UNORM_BLOCK) == 181, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_12x10_SRGB_BLOCK) == 182, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_12x12_UNORM_BLOCK) == 183, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::ASTC_12x12_SRGB_BLOCK) == 184, LOCAL_ERROR_MESSAGE);

    // extended formats
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::EX_ALPHA8_UNORM) == 185, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::EX_LUMINANCE8_UNORM) == 186, LOCAL_ERROR_MESSAGE);
    static_assert(LOCAL_GET_PIXELFORMAT_ID(PixelFormat::EX_LUMINANCE8_ALPHA8_UNORM) == 187, LOCAL_ERROR_MESSAGE);
  }
}
