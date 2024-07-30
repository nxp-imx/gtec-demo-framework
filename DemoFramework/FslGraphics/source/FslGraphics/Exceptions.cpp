/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Log/FmtPixelFormat.hpp>
#include <fmt/format.h>

namespace Fsl
{
  UnsupportedPixelFormatException::UnsupportedPixelFormatException(const char* const psz, const PixelFormat pixelFormat)
    : GraphicsException(fmt::format("{}. PixelFormat: {}", psz != nullptr ? psz : "nullptr", pixelFormat))
    , m_pixelFormat(pixelFormat)
  {
  }

  UnsupportedPixelFormatException::UnsupportedPixelFormatException(const std::string& str, const PixelFormat pixelFormat)
    : GraphicsException(fmt::format("{}. PixelFormat: {}", str, pixelFormat))
    , m_pixelFormat(pixelFormat)
  {
  }

  UnsupportedPixelFormatException::UnsupportedPixelFormatException(const PixelFormat pixelFormat)
    : GraphicsException(fmt::format("Unsupported pixel format. PixelFormat: {}", pixelFormat))
    , m_pixelFormat(pixelFormat)
  {
  }

  UnsupportedPixelFormatConversionException::UnsupportedPixelFormatConversionException(const char* const psz, const PixelFormat fromPixelFormat,
                                                                                       const PixelFormat toPixelFormat)
    : ConversionException(fmt::format("{}. From: {} To: {}", psz != nullptr ? psz : "nullptr", fromPixelFormat, toPixelFormat))
    , m_fromPixelFormat(fromPixelFormat)
    , m_toPixelFormat(toPixelFormat)
  {
  }

  UnsupportedPixelFormatConversionException::UnsupportedPixelFormatConversionException(const std::string& str, const PixelFormat fromPixelFormat,
                                                                                       const PixelFormat toPixelFormat)
    : ConversionException(fmt::format("{}. From: {} To: {}", str, fromPixelFormat, toPixelFormat))
    , m_fromPixelFormat(fromPixelFormat)
    , m_toPixelFormat(toPixelFormat)
  {
  }

  UnsupportedPixelFormatConversionException::UnsupportedPixelFormatConversionException(const PixelFormat fromPixelFormat,
                                                                                       const PixelFormat toPixelFormat)
    : ConversionException(fmt::format("Unsupported pixel format conversion from: {} to: {}", fromPixelFormat, toPixelFormat))
    , m_fromPixelFormat(fromPixelFormat)
    , m_toPixelFormat(toPixelFormat)
  {
  }

}
