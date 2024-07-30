#ifndef FSLDEMOSERVICE_IMAGECONVERTER_IIMAGECONVERTERSERVICE_HPP
#define FSLDEMOSERVICE_IMAGECONVERTER_IIMAGECONVERTERSERVICE_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2024 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/Bitmap/SupportedConversion.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslDemoService/ImageConverter/ConversionType.hpp>
#include <FslDemoService/ImageConverter/ImageConvertResult.hpp>

namespace Fsl
{
  class Bitmap;
  class Texture;

  class IImageConverterService
  {
  public:
    virtual ~IImageConverterService() = default;

    //! @brief Get the supported conversions
    virtual ReadOnlySpan<SupportedConversion> GetSupportedConversions(const ConversionType conversionType) const noexcept = 0;

    //! @brief Convert the bitmap to the desired pixel format and origin else return false.
    //! @param rBitmap = the bitmap to read and write the result to (the bitmap will be reset as necessary)
    //! @note if desiredPixelFormat is PixelFormat::Undefined no pixel format conversion is performed
    //! @return ImageConvertResult::Completed if rTexture was converted to the desiredPixelFormat and origin.
    virtual ImageConvertResult TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat) = 0;

    //! @brief Convert the texture to the desired pixel format and origin else return false.
    //! @param rTexture = the bitmap to write the result to (the bitmap will be reset as necessary)
    //! @note if desiredPixelFormat is PixelFormat::Undefined no pixel format conversion is performed
    //! @note if desiredOrigin is BitmapOrigin::Undefined no bitmap origin conversion is performed
    //! @return ImageConvertResult::Completed if rTexture was converted to the desiredPixelFormat and origin.
    virtual ImageConvertResult TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat) = 0;


    //! @brief Convert the bitmap to the desired pixel format and origin else return false.
    //! @param rDstBitmap = the bitmap to write the result to (the bitmap will be reset as necessary)
    //!                     This can NOT be equal to srcBitmap. If inplace conversion is desired use the other method.
    //! @param srcBitmap = the bitmap to convert from.
    //! @return ImageConvertResult::Completed if rTexture was converted to the desiredPixelFormat and origin.
    virtual ImageConvertResult TryConvert(Bitmap& rDstBitmap, const Bitmap& srcBitmap) = 0;

    //! @brief Convert the texture to the desired pixel format and origin else return false.
    //! @param rDstBitmap = the bitmap to write the result to (the bitmap will be reset as necessary)
    //!                     This can NOT be equal to srcBitmap. If inplace conversion is desired use the other method.
    //! @param srcBitmap = the bitmap to convert from.
    //! unsupported)
    //! @return ImageConvertResult::Completed if rTexture was converted to the desiredPixelFormat and origin.
    virtual ImageConvertResult TryConvert(Texture& rDstTexture, const Texture& srcTexture) = 0;
  };
}

#endif
