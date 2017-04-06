#ifndef FSLDEMOPLATFORM_CONFIGURATION_ANDROID_IMAGELIBRARYSERVICEANDROID_HPP
#define FSLDEMOPLATFORM_CONFIGURATION_ANDROID_IMAGELIBRARYSERVICEANDROID_HPP
#if defined(__ANDROID__)
/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslDemoHost/Service/ThreadLocal/ThreadLocalService.hpp>
#include <FslDemoApp/Service/ImageLibrary/IImageLibraryService.hpp>
#include <FslDemoApp/Service/ServiceProvider.hpp>

namespace Fsl
{
  class ImageLibraryServiceAndroid
    : public ThreadLocalService
    , public IImageLibraryService
  {
  public:
    ImageLibraryServiceAndroid(const ServiceProvider& serviceProvider);
    ~ImageLibraryServiceAndroid();

    // From IImageLibraryService
    virtual std::string GetName() const override;
    virtual void ExtractSupportedImageFormats(std::deque<ImageFormat>& rFormats) override;
    virtual bool TryRead(Bitmap& rBitmap, const IO::Path& path, const PixelFormat pixelFormatHint, const BitmapOrigin originHint) override;
    virtual bool TryRead(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat pixelFormatHint, const BitmapOrigin originHint) override;
    virtual bool TryWrite(const IO::Path& path, const Bitmap& bitmap, const ImageFormat imageFormat, const bool allowOverwrite) override;
  private:
  };
}

#endif
#endif
