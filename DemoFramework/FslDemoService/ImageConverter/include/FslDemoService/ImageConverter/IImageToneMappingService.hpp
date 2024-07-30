#ifndef FSLDEMOSERVICE_IMAGECONVERTER_IIMAGETONEMAPPINGSERVICE_HPP
#define FSLDEMOSERVICE_IMAGECONVERTER_IIMAGETONEMAPPINGSERVICE_HPP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDemoService/ImageConverter/ConversionType.hpp>
#include <FslDemoService/ImageConverter/ToneMappingResult.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/ToneMapping/BasicToneMapper.hpp>
#include <FslGraphics/ToneMapping/SupportedToneMapping.hpp>

namespace Fsl
{
  class Bitmap;
  class Texture;

  class IImageToneMappingService
  {
  public:
    virtual ~IImageToneMappingService() = default;

    virtual ReadOnlySpan<SupportedToneMapping> GetSupportedToneMappings(const ConversionType conversionType) const noexcept = 0;

    //! @brief Convert the bitmap to the desired pixel format and origin else return false.
    //! @param rBitmap = the bitmap to read and write the result to (the bitmap will be reset as necessary)
    //! @return ToneMappingResult::Completed if the tone-mapping was applied.
    virtual ToneMappingResult TryToneMap(Bitmap& rBitmap, const BasicToneMapper toneMapping, const float exposure) = 0;

    //! @brief Convert the bitmap to the desired pixel format and origin else return false.
    //! @param rBitmap = the bitmap to read and write the result to (the bitmap will be reset as necessary)
    //! @return ToneMappingResult::Completed if the tone-mapping was applied.
    virtual ToneMappingResult TryToneMap(Texture& rTexture, const BasicToneMapper toneMapping, const float exposure) = 0;
  };
}

#endif
