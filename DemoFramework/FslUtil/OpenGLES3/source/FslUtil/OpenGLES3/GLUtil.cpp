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

#include <FslBase/String/StringUtil.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <cassert>
#include <cstring>

namespace Fsl::GLES3::GLUtil
{
  namespace
  {
    struct PixelEncodingRecord
    {
      GLint Format{0};
      GLint Type{0};

      constexpr PixelEncodingRecord(const GLint format, const GLint type) noexcept
        : Format(format)
        , Type(type)
      {
      }
    };


    struct CaptureMethodRecord
    {
      PixelEncodingRecord Encoding;
      PixelFormat BitmapPixelFormat;

      constexpr CaptureMethodRecord(const PixelEncodingRecord encoding, const PixelFormat bitmapPixelFormat) noexcept
        : Encoding(encoding)
        , BitmapPixelFormat(bitmapPixelFormat)
      {
      }
    };

    PixelEncodingRecord GetImplementationColorEncodingRecord() noexcept
    {
      GLint implColorReadFormat = 0;
      glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &implColorReadFormat);
      GLint implColorReadType = 0;
      glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &implColorReadType);
      return {implColorReadFormat, implColorReadType};
    }


    CaptureMethodRecord DecideCaptureMethod(const PixelEncodingRecord colorEncoding) noexcept
    {
      switch (colorEncoding.Type)
      {
      case GL_UNSIGNED_BYTE:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_5_5_5_1:
        return {PixelEncodingRecord(colorEncoding.Format, GL_UNSIGNED_BYTE), PixelFormat::R8G8B8A8_UINT};
      case GL_BYTE:
        return {PixelEncodingRecord(colorEncoding.Format, GL_BYTE), PixelFormat::R8G8B8A8_SINT};
      case GL_HALF_FLOAT:
        return {PixelEncodingRecord(colorEncoding.Format, GL_HALF_FLOAT), PixelFormat::R16G16B16A16_SNORM};
      case GL_FLOAT:
        return {PixelEncodingRecord(colorEncoding.Format, GL_FLOAT), PixelFormat::R32G32B32A32_SFLOAT};
      // case GL_UNSIGNED_INT_2_10_10_10_REV:
      // case GL_UNSIGNED_INT_10F_11F_11F_REV:
      // case GL_UNSIGNED_INT_5_9_9_9_REV:
      default:
        return {PixelEncodingRecord(colorEncoding.Format, GL_UNSIGNED_BYTE), PixelFormat::R8G8B8A8_UINT};
      }
    }


    CaptureMethodRecord MakeCaptureDecision() noexcept
    {
      return DecideCaptureMethod(GetImplementationColorEncodingRecord());
    }
  }

  std::vector<StringViewLite> GetExtensions()
  {
    const auto* pszExtensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    if (pszExtensions == nullptr)
    {
      GL_CHECK_FOR_ERROR();
      throw std::runtime_error("Failed to retrieve extension list");
    }
    return StringUtil::Split(pszExtensions, ' ', true);
  }


  // bool HasExtension(const char* const pszExtensionName)
  //{
  //   if (pszExtensionName == nullptr)
  //   {
  //     throw std::invalid_argument("pszExtensionName can not be null");
  //   }

  //  const auto* pszExtensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
  //  if (pszExtensions == nullptr)
  //  {
  //    return false;
  //  }

  //  const auto lenExtensionName = strlen(pszExtensionName);

  //  const char* pszCurrentLocation = pszExtensions;
  //  while (pszCurrentLocation != nullptr)
  //  {
  //    const char* pszCharLocation = strstr(pszCurrentLocation, pszExtensionName);
  //    if (pszCharLocation == nullptr)
  //    {
  //      return false;
  //    }

  //    // If this isn't the first entry, then the previous character must be a space
  //    if (pszCharLocation == pszExtensions || *(pszCharLocation - 1) == ' ')
  //    {
  //      const char endChar = *(pszCharLocation + lenExtensionName);
  //      if (endChar == ' ' || endChar == 0)
  //      {
  //        return true;
  //      }
  //    }
  //    // Not a exact match -> so exit
  //    pszCurrentLocation = pszCharLocation + 1;
  //  }
  //  return false;
  //}

  bool HasExtension(const char* const pszExtensionName)
  {
    if (pszExtensionName == nullptr)
    {
      throw std::invalid_argument("pszExtensionName can not be null");
    }

    int max = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &max);

    for (int i = 0; i < max; ++i)
    {
      if (strcmp(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)), pszExtensionName) == 0)
      {
        return true;
      }
    }
    return false;
  }

  void Capture(Bitmap& rBitmap, const PixelFormat pixelFormat)
  {
    // Read the viewport to get the current size
    std::array<GLint, 4> viewport{};
    GL_CHECK(glGetIntegerv(GL_VIEWPORT, viewport.data()))

    const auto srcRectanglePx = PxRectangle::Create(viewport[0], viewport[1], viewport[2], viewport[3]);
    return Capture(rBitmap, pixelFormat, srcRectanglePx);
  }


  void Capture(Bitmap& rBitmap, const PixelFormat pixelFormat, const PxRectangle& srcRectanglePx)
  {
    const CaptureMethodRecord captureDecision = MakeCaptureDecision();

    // We don't need to clear as we are going to overwrite everything anyway
    // We utilize PixelFormatLayout::R8G8B8A8 here since that is what glReadPixels is filling it with
    // that also allows the convert method to detect if the the supplied pixelFormat is different and then
    // convert as necessary
    rBitmap.Reset(srcRectanglePx.GetSize(), captureDecision.BitmapPixelFormat, BitmapOrigin::LowerLeft, BitmapClearMethod::DontClear);

    // glFinish();
    {
      Bitmap::ScopedDirectReadWriteAccess scopedAccess(rBitmap);
      GL_CHECK(glReadPixels(srcRectanglePx.RawX(), srcRectanglePx.RawY(), srcRectanglePx.RawWidth(), srcRectanglePx.RawHeight(),
                            captureDecision.Encoding.Format, captureDecision.Encoding.Type, scopedAccess.AsRawBitmap().Content()));
      RawBitmapUtil::FlipHorizontal(scopedAccess.AsRawBitmap());
    }

    // Convert if necessary (convert will do nothing if the format is already correct)
    BitmapUtil::Convert(rBitmap, pixelFormat);
  }


  GLenum Convert(const PrimitiveType primitiveType)
  {
    switch (primitiveType)
    {
    case PrimitiveType::LineList:
      return GL_LINES;
    case PrimitiveType::LineStrip:
      return GL_LINE_STRIP;
    case PrimitiveType::TriangleList:
      return GL_TRIANGLES;
    case PrimitiveType::TriangleStrip:
      return GL_TRIANGLE_STRIP;
    default:
      throw UnsupportedPrimitiveTypeException("Unknown primitive type");
    }
  }


  std::vector<GLint> GetCompressedTextureFormats()
  {
    GLint count = 0;
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &count);

    std::vector<GLint> res(count);
    if (count > 0)
    {
      glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, res.data());
    }
    return res;
  }

}
