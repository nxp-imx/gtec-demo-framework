#ifndef FSLDEMOSERVICE_BITMAPCONVERTER_IBITMAPCONVERTER_HPP
#define FSLDEMOSERVICE_BITMAPCONVERTER_IBITMAPCONVERTER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/PixelFormat.hpp>

namespace Fsl
{
  class Bitmap;
  class Texture;
  struct BitmapConverterConfig;

  class IBitmapConverter
  {
  public:
    virtual ~IBitmapConverter() = default;

    //! @brief Convert the bitmap to the desired pixel format and origin else return false.
    //! @note if desiredPixelFormat is PixelFormat::Undefined no pixel format conversion is performed
    //! @note if desiredOrigin is BitmapOrigin::Undefined no bitmap origin conversion is performed
    virtual bool TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                            const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) = 0;

    //! @brief Convert the bitmap to the desired pixel format and origin (if possible, else throw a exception)
    //! @note if desiredPixelFormat is PixelFormat::Undefined no pixel format conversion is performed
    //! @note if desiredOrigin is BitmapOrigin::Undefined no bitmap origin conversion is performed
    virtual void Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                         const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) = 0;

    //! @brief Convert the texture to the desired pixel format and origin else return false.
    //! @note if desiredPixelFormat is PixelFormat::Undefined no pixel format conversion is performed
    //! @note if desiredOrigin is BitmapOrigin::Undefined no bitmap origin conversion is performed
    virtual bool TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                            const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) = 0;

    //! @brief Convert the texture to the desired pixel format and origin (if possible, else throw a exception)
    //! @note if desiredPixelFormat is PixelFormat::Undefined no pixel format conversion is performed
    //! @note if desiredOrigin is BitmapOrigin::Undefined no bitmap origin conversion is performed
    virtual void Convert(Texture& rTexture, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                         const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) = 0;

    //! @brief Convert the bitmap to the desired pixel format else return false.
    // virtual bool TryConvert(Bitmap& rDstBitmap, const Bitmap& srcBitmap, const PixelFormat desiredPixelFormat) = 0;

    //! @brief Convert the srcBitmap to rDstBitmap and convert it to the desired pixel format in the process (if possible, else throw a exception)
    // virtual void Convert(Bitmap& rDstBitmap, const Bitmap& srcBitmap, const PixelFormat desiredPixelFormat) = 0;

    virtual bool TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin, const BitmapConverterConfig& converterConfig) = 0;
    virtual bool TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin, const BitmapConverterConfig& converterConfig) = 0;

    virtual void Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin, const BitmapConverterConfig& converterConfig) = 0;
    virtual void Convert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin, const BitmapConverterConfig& converterConfig) = 0;
  };
}

#endif
