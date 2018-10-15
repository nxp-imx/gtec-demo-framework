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

#include <FslUtil/OpenGLES2/GLRawBitmapUtil.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <cassert>

namespace Fsl
{
  namespace GLES2
  {
    GLRawBitmapUtil::Result GLRawBitmapUtil::Convert(const PixelFormat pixelFormat, const uint32_t width, const uint32_t stride,
                                                     const bool exactMatch)
    {
      if (stride < width)
      {
        throw UnsupportedStrideException(stride);
      }

      FSLLOG_DEBUG_WARNING_IF(exactMatch, "GLES2 does not support exact matching of pixelFormats with the textureFormat");

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
        return Result(
          GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
          RawBitmapUtil::CalcAlignment(pixelFormat, width, stride,
                                       2));    // GL_LUMINANCE_ALPHA    | GL_LUMINANCE_ALPHA | GL_UNSIGNED_BYTE              | Luminance, Alpha | L, A
      default:
        break;
      }

      switch (PixelFormatUtil::GetPixelFormatLayout(pixelFormat))
      {
      case PixelFormatLayout::R8G8B8:
        return Result(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
                      RawBitmapUtil::CalcAlignment(
                        pixelFormat, width, stride,
                        3));    // GL_RGB                | GL_RGB             | GL_UNSIGNED_BYTE,             | Red, Green, Blue           | R, G, B
      case PixelFormatLayout::R8G8B8A8:
        return Result(
          GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
          RawBitmapUtil::CalcAlignment(
            pixelFormat, width, stride,
            4));    // GL_RGBA               | GL_RGBA            | GL_UNSIGNED_BYTE,             | Red, Green, Blue, Alpha    | R, G, B, A
      default:
        throw UnsupportedPixelFormatException(pixelFormat);
      }
    }
  }
}
