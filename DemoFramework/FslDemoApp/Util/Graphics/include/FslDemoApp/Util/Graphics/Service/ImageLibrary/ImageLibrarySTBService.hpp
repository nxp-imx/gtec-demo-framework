#ifndef FSLDEMOAPP_UTIL_GRAPHICS_SERVICE_IMAGELIBRARY_IMAGELIBRARYSTBSERVICE_HPP
#define FSLDEMOAPP_UTIL_GRAPHICS_SERVICE_IMAGELIBRARY_IMAGELIBRARYSTBSERVICE_HPP
#ifdef FSL_FEATURE_STB
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslDemoApp/Base/Service/ImageLibrary/IImageLibraryService.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>

namespace Fsl
{
  class IBitmapConverter;

  class ImageLibrarySTBService final
    : public ThreadLocalService
    , public IImageLibraryService
  {
    std::shared_ptr<IBitmapConverter> m_bitmapConverter;

  public:
    explicit ImageLibrarySTBService(const ServiceProvider& serviceProvider);
    ~ImageLibrarySTBService() final;

    // From IImageLibraryService
    std::string GetName() const final;
    void ExtractSupportedImageFormats(std::deque<ImageFormat>& rFormats) final;
    bool TryRead(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat pixelFormatHint, const BitmapOrigin originHint,
                 const PixelChannelOrder preferredChannelOrderHint) final;
    bool TryRead(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat pixelFormatHint, const BitmapOrigin originHint,
                 const PixelChannelOrder preferredChannelOrderHint) final;
    bool TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const bool allowOverwrite) final;

  private:
    bool TryWrite(const IO::Path& dstName, const Bitmap& bitmap, const int comp, const ImageFormat imageFormat);
  };
}

#endif
#endif
