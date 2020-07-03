#ifndef FSLDEMOAPP_UTIL_GRAPHICS_SERVICE_IMAGECONVERTERLIBRARY_IMAGECONVERTERLIBRARYFP16SERVICE_HPP
#define FSLDEMOAPP_UTIL_GRAPHICS_SERVICE_IMAGECONVERTERLIBRARY_IMAGECONVERTERLIBRARYFP16SERVICE_HPP
#ifdef FSL_FEATURE_HALF
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

#include <FslDemoApp/Base/Service/ImageConverterLibrary/IImageConverterLibraryService.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>

namespace Fsl
{
  class ImageConverterLibraryFP16Service final
    : public ThreadLocalService
    , public IImageConverterLibraryService
  {
  public:
    ImageConverterLibraryFP16Service(const ServiceProvider& serviceProvider);
    ~ImageConverterLibraryFP16Service();

    // From IImageConverterLibraryService
    bool TryConvert(Bitmap& rDstBitmap, const Bitmap& srcBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOriginHint) final;
    bool TryConvert(Texture& rDstTexture, const Texture& srcTexture, const PixelFormat desiredPixelFormat,
                    const BitmapOrigin desiredOriginHint) final;
  };
}

#endif
#endif
