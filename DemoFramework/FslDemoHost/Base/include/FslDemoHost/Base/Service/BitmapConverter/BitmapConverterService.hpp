#ifndef FSLDEMOHOST_BASE_SERVICE_BITMAPCONVERTER_BITMAPCONVERTERSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_BITMAPCONVERTER_BITMAPCONVERTERSERVICE_HPP
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

#include <FslDemoService/BitmapConverter/IBitmapConverter.hpp>
#include <FslDemoService/ImageConverter/IImageConverterService.hpp>
#include <FslDemoService/ImageConverter/IImageToneMappingService.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  struct ToneMappingRecord
  {
    PixelFormat Format;
    std::shared_ptr<IImageToneMappingService> Service;

    explicit ToneMappingRecord(const PixelFormat format, std::shared_ptr<IImageToneMappingService> service)
      : Format(format)
      , Service(std::move(service))
    {
    }
  };


  class BitmapConverterService final
    : public ThreadLocalService
    , public IBitmapConverter
  {
    std::vector<std::shared_ptr<IImageConverterService>> m_converterLibraries;
    std::map<BasicToneMapper, std::vector<ToneMappingRecord>> m_toneMappingMap;

  public:
    explicit BitmapConverterService(const ServiceProvider& serviceProvider);
    ~BitmapConverterService() final;

    // From IBitmapConverter
    bool TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                    const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) final;
    void Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                 const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) final;
    bool TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                    const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) final;
    void Convert(Texture& rTexture, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                 const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) final;

    bool TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                    const BitmapConverterConfig& converterConfig) final;
    bool TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                    const BitmapConverterConfig& converterConfig) final;

    void Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                 const BitmapConverterConfig& converterConfig) final;
    void Convert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                 const BitmapConverterConfig& converterConfig) final;
  };
}

#endif
