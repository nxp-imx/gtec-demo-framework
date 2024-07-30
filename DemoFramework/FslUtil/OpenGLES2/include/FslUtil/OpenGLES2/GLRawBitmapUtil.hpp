#ifndef FSLUTIL_OPENGLES2_GLRAWBITMAPUTIL_HPP
#define FSLUTIL_OPENGLES2_GLRAWBITMAPUTIL_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslGraphics/Bitmap/ReadOnlyRawBitmap.hpp>
#include <FslUtil/OpenGLES2/Common.hpp>
#include <GLES2/gl2.h>

namespace Fsl::GLES2
{
  class GLRawBitmapUtil
  {
  public:
    struct Result
    {
      GLint InternalFormat{0};
      GLint Format{0};
      GLenum Type{0};
      GLint Alignment{0};

      Result() = default;

      Result(const GLint internalFormat, const GLint format, const GLenum type, const GLint alignment)
        : InternalFormat(internalFormat)
        , Format(format)
        , Type(type)
        , Alignment(alignment)
      {
      }
    };

    struct CompressedResult
    {
      GLint InternalFormat;    // compressed internal format
      GLint BaseInternalFormat;
      GLint Alignment;

      CompressedResult(const GLint internalFormat, const GLint baseInternalFormat, const GLint alignment)
        : InternalFormat(internalFormat)
        , BaseInternalFormat(baseInternalFormat)
        , Alignment(alignment)
      {
      }
    };

    //! @brief Convert the rawBitmap to a texture description suitable for glTexImage2D
    //! @param exactMatch if this is true we require a exact texture format to pixel format match.
    //                    if false then we allow OpenGL to find something good enough.
    static Result Convert(const ReadOnlyRawBitmap& rawBitmap, const bool exactMatch)
    {
      return Convert(rawBitmap.GetPixelFormat(), rawBitmap.RawUnsignedWidth(), rawBitmap.Stride(), exactMatch);
    }

    //! @brief Convert the rawBitmap to a texture description suitable for glTexImage2D
    //! @param exactMatch if this is true we require a exact texture format to pixel format match.
    //                    if false then we allow OpenGL to find something good enough.
    static Result Convert(const PixelFormat pixelFormat, const uint32_t width, const uint32_t stride, const bool exactMatch);

    static CompressedResult ConvertCompressed(const PixelFormat pixelFormat, const uint32_t width);
  };
}

#endif
