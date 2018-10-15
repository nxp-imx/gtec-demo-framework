#ifndef FSLDEMOAPP_BASE_SERVICE_IMAGECONVERTERLIBRARY_IIMAGECONVERTERLIBRARYSERVICE_HPP
#define FSLDEMOAPP_BASE_SERVICE_IMAGECONVERTERLIBRARY_IIMAGECONVERTERLIBRARYSERVICE_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/PixelFormat.hpp>

namespace Fsl
{
  class Bitmap;
  class Texture;

  class IImageConverterLibraryService
  {
  public:
    virtual ~IImageConverterLibraryService() = default;

    //! @brief Convert the bitmap to the desired pixel format and origin else return false.
    //! @param rDstBitmap = the bitmap to write the result to (the bitmap will be reset as necessary)
    //!                     (beware this can be equal to srcBitmap, so return false if in-place conversion is unsupported)
    //! @param srcBitmap = the bitmap to convert from  (beware this can be equal to rDstBitmap, so return false if in-place conversion is unsupported)
    //! @note if desiredPixelFormat is PixelFormat::Undefined no pixel format conversion is performed
    //! @note if desiredOriginHint is BitmapOrigin::Undefined no bitmap origin conversion is performed
    //! @note desiredOriginHint is a hint flag, so its ok to return true even if the origin wasn't converted
    //! @return true if the rDstBitmap was converted to the desiredPixelFormat (beware the origin might still not be the desired one)
    virtual bool TryConvert(Bitmap& rDstBitmap, const Bitmap& srcBitmap, const PixelFormat desiredPixelFormat,
                            const BitmapOrigin desiredOriginHint) = 0;

    //! @brief Convert the texture to the desired pixel format and origin else return false.
    //! @param rDstTexture = the bitmap to write the result to (the bitmap will be reset as necessary)
    //!                      (beware this can be equal to srcTexture, so return false if in-place conversion is unsupported)
    //! @param srcTexture = the bitmap to convert from  (beware this can be equal to rDstTexture, so return false if in-place conversion is
    //! unsupported)
    //! @note if desiredPixelFormat is PixelFormat::Undefined no pixel format conversion is performed
    //! @note if desiredOriginHint is BitmapOrigin::Undefined no bitmap origin conversion is performed
    //! @note desiredOriginHint is a hint flag, so its ok to return true even if the origin wasn't converted
    //! @return true if the rDstTexture was converted to the desiredPixelFormat (beware the origin might still not be the desired one)
    virtual bool TryConvert(Texture& rDstTexture, const Texture& srcTexture, const PixelFormat desiredPixelFormat,
                            const BitmapOrigin desiredOriginHint) = 0;
  };
}

#endif
