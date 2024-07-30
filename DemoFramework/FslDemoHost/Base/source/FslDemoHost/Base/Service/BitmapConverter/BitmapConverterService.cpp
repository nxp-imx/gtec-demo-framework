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

#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslDemoHost/Base/Service/BitmapConverter/BitmapConverterService.hpp>
#include <FslDemoService/BitmapConverter/BitmapConverterConfig.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Log/Bitmap/FmtBitmapOrigin.hpp>
#include <FslGraphics/Log/FmtColorSpace.hpp>
#include <FslGraphics/Log/FmtPixelFormat.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/TextureUtil.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <fmt/format.h>
#include <deque>

namespace Fsl
{
  namespace
  {
    ImageConvertResult TryConvertLibraries(Bitmap& rBitmap, const PixelFormat desiredPixelFormat,
                                           const ReadOnlySpan<std::shared_ptr<IImageConverterService>> converterLibraries)
    {
      ImageConvertResult result = ImageConvertResult::NotSupported;
      for (const auto& converter : converterLibraries)
      {
        const auto libResult = converter->TryConvert(rBitmap, desiredPixelFormat);
        if (libResult == ImageConvertResult::Completed)
        {
          return ImageConvertResult::Completed;
        }
        if (result != ImageConvertResult::Completed && libResult == ImageConvertResult::OriginConversionNotSupported)
        {
          result = libResult;
        }
        else if (libResult == ImageConvertResult::PixelFormatConversionNotSupported && result == ImageConvertResult::NotSupported)
        {
          result = libResult;
        }
      }
      return result;
    }

    ImageConvertResult TryConvertLibraries(Bitmap& rToBitmap, const Bitmap& fromBitmap,
                                           const ReadOnlySpan<std::shared_ptr<IImageConverterService>> converterLibraries)
    {
      if (rToBitmap.GetOrigin() != fromBitmap.GetOrigin())
      {
        return ImageConvertResult::OriginConversionNotSupported;
      }

      ImageConvertResult result = ImageConvertResult::NotSupported;
      for (const auto& converter : converterLibraries)
      {
        const auto libResult = converter->TryConvert(rToBitmap, fromBitmap);
        if (libResult == ImageConvertResult::Completed)
        {
          return ImageConvertResult::Completed;
        }
        if (result != ImageConvertResult::Completed && libResult == ImageConvertResult::OriginConversionNotSupported)
        {
          result = libResult;
        }
        else if (libResult == ImageConvertResult::PixelFormatConversionNotSupported && result == ImageConvertResult::NotSupported)
        {
          result = libResult;
        }
      }
      return result;
    }


    ImageConvertResult TryConvertLibraries(Texture& rTexture, const PixelFormat desiredPixelFormat,
                                           const ReadOnlySpan<std::shared_ptr<IImageConverterService>> converterLibraries)
    {
      ImageConvertResult result = ImageConvertResult::NotSupported;
      for (const auto& converter : converterLibraries)
      {
        const auto libResult = converter->TryConvert(rTexture, desiredPixelFormat);
        if (libResult == ImageConvertResult::Completed)
        {
          return ImageConvertResult::Completed;
        }
        if (result != ImageConvertResult::Completed && libResult == ImageConvertResult::OriginConversionNotSupported)
        {
          result = libResult;
        }
        else if (libResult == ImageConvertResult::PixelFormatConversionNotSupported && result == ImageConvertResult::NotSupported)
        {
          result = libResult;
        }
      }
      return result;
    }


    bool TryConvertOrigin(Bitmap& rBitmap, const BitmapOrigin desiredOrigin)
    {
      if (desiredOrigin == BitmapOrigin::Undefined || rBitmap.GetOrigin() == desiredOrigin)
      {
        return true;
      }
      if ((rBitmap.GetOrigin() == BitmapOrigin::LowerLeft && desiredOrigin == BitmapOrigin::UpperLeft) ||
          (rBitmap.GetOrigin() == BitmapOrigin::UpperLeft && desiredOrigin == BitmapOrigin::LowerLeft))
      {
        BitmapUtil::FlipHorizontal(rBitmap);
        return true;
      }
      return false;
    }


    std::map<BasicToneMapper, std::vector<ToneMappingRecord>> Create(const std::deque<std::shared_ptr<IImageToneMappingService>>& services,
                                                                     const ConversionType conversionType)
    {
      std::map<BasicToneMapper, std::vector<ToneMappingRecord>> map;
      for (const auto& service : services)
      {
        const auto span = service->GetSupportedToneMappings(conversionType);
        for (const auto& mapping : span)
        {
          auto itrFind = map.find(mapping.ToneMapper);
          if (itrFind == map.end())
          {
            map.emplace(mapping.ToneMapper, std::vector<ToneMappingRecord>{ToneMappingRecord(mapping.Format, service)});
          }
          else
          {
            itrFind->second.emplace_back(mapping.Format, service);
          }
        }
      }
      return map;
    }

    bool IsHDRFormat(const PixelFormat pixelFormat) noexcept
    {
      PixelFormatLayout layout = PixelFormatUtil::GetPixelFormatLayout(pixelFormat);
      switch (layout)
      {
      case PixelFormatLayout::R16G16B16:
      case PixelFormatLayout::R16G16B16A16:
      case PixelFormatLayout::R32G32B32:
      case PixelFormatLayout::R32G32B32A32:
      case PixelFormatLayout::A2R10G10B10_PACK32:
      case PixelFormatLayout::A2B10G10R10_PACK32:
      case PixelFormatLayout::R64G64B64:
      case PixelFormatLayout::R64G64B64A64:
      case PixelFormatLayout::B10G11R11_PACK32:
        return true;
      default:
        return false;
      }
    }

    bool ApplyToneMapping(const ReadOnlySpan<std::shared_ptr<IImageConverterService>> converterLibraries,
                          const std::map<BasicToneMapper, std::vector<ToneMappingRecord>>& toneMappingMap, Bitmap& rBitmap,
                          const BitmapConverterConfig& converterConfig)
    {
      assert(converterConfig.ToneMapper != BasicToneMapper::Clamp);
      const auto itrFind = toneMappingMap.find(converterConfig.ToneMapper);
      if (itrFind == toneMappingMap.end())
      {
        return false;
      }

      const PixelFormat toneMapPixelFormat =
        PixelFormatUtil::HasAlphaChannel(rBitmap.GetPixelFormat()) ? PixelFormat::R32G32B32A32_SFLOAT : PixelFormat::R32G32B32_SFLOAT;

      Bitmap tmpBitmap(rBitmap.GetSize(), toneMapPixelFormat, rBitmap.GetOrigin(), rBitmap.GetStrideRequirement());
      if (TryConvertLibraries(tmpBitmap, rBitmap, converterLibraries) != ImageConvertResult::Completed)
      {
        return false;
      }

      for (const auto& record : itrFind->second)
      {
        for (const auto& mapping : record.Service->GetSupportedToneMappings(ConversionType::Bitmap))
        {
          if (mapping.ToneMapper == converterConfig.ToneMapper && mapping.Format == tmpBitmap.GetPixelFormat())
          {
            if (record.Service->TryToneMap(tmpBitmap, converterConfig.ToneMapper, converterConfig.Exposure) == ToneMappingResult::Completed)
            {
              rBitmap = std::move(tmpBitmap);
              return true;
            }
          }
        }
      }
      return false;
    }
  }


  BitmapConverterService::BitmapConverterService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
    // Extract the converter libraries so we can use the available ones
    std::deque<std::shared_ptr<IImageConverterService>> converterLibraries;
    serviceProvider.Get<IImageConverterService>(converterLibraries);
    // Extract the converter libraries so we can use the available ones
    std::deque<std::shared_ptr<IImageToneMappingService>> toneMappingServices;
    serviceProvider.Get<IImageToneMappingService>(toneMappingServices);

    m_converterLibraries = {converterLibraries.begin(), converterLibraries.end()};
    m_toneMappingMap = Create(toneMappingServices, ConversionType::Bitmap);
  }


  BitmapConverterService::~BitmapConverterService() = default;


  bool BitmapConverterService::TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvertOrigin(rBitmap, desiredOrigin))
    {
      // Failed to convert origin to the desired one
      return false;
    }

    // Try the conversion again now that it is just a pixel format conversion
    auto result = TryConvertLibraries(rBitmap, desiredPixelFormat, SpanUtil::AsReadOnlySpan(m_converterLibraries));
    if (result == ImageConvertResult::Completed)
    {
      return true;
    }

    // Try using one of the 'non overlapping memory' converters
    Bitmap tmpBitmap(rBitmap.GetSize(), desiredPixelFormat, rBitmap.GetOrigin(), rBitmap.GetStrideRequirement());
    result = TryConvertLibraries(tmpBitmap, rBitmap, SpanUtil::AsReadOnlySpan(m_converterLibraries));
    if (result == ImageConvertResult::Completed)
    {
      rBitmap = std::move(tmpBitmap);
      return true;
    }
    return false;
  }


  void BitmapConverterService::Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvert(rBitmap, desiredPixelFormat, desiredOrigin))
    {
      throw GraphicsException(fmt::format("Unsupported conversion from '{}, {}' to '{}, {}'", rBitmap.GetPixelFormat(), rBitmap.GetOrigin(),
                                          desiredPixelFormat, desiredOrigin));
    }
  }


  bool BitmapConverterService::TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TextureUtil::TryChangeOrigin(rTexture, desiredOrigin))
    {
      // Failed to convert origin to the desired one
      return false;
    }
    // Try the conversion again now that it is just a pixel format conversion
    const auto result = TryConvertLibraries(rTexture, desiredPixelFormat, SpanUtil::AsReadOnlySpan(m_converterLibraries));
    return result == ImageConvertResult::Completed;
  }


  void BitmapConverterService::Convert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvert(rTexture, desiredPixelFormat, desiredOrigin))
    {
      throw GraphicsException(fmt::format("Unsupported conversion from '{}, {}' to '{}, {}'", rTexture.GetPixelFormat(), rTexture.GetBitmapOrigin(),
                                          desiredPixelFormat, desiredOrigin));
    }
  }


  bool BitmapConverterService::TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                                          const BitmapConverterConfig& converterConfig)
  {
    const PixelFormat srcPixelFormat = rBitmap.GetPixelFormat();
    if (converterConfig.ToneMapper != BasicToneMapper::Clamp && IsHDRFormat(srcPixelFormat) && !IsHDRFormat(desiredPixelFormat))
    {
      if (!ApplyToneMapping(SpanUtil::AsReadOnlySpan(m_converterLibraries), m_toneMappingMap, rBitmap, converterConfig))
      {
        return false;
      }
    }
    return TryConvert(rBitmap, desiredPixelFormat, desiredOrigin);
  }


  bool BitmapConverterService::TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                                          const BitmapConverterConfig& converterConfig)
  {
    return false;
  }


  void BitmapConverterService::Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                                       const BitmapConverterConfig& converterConfig)
  {
    if (!TryConvert(rBitmap, desiredPixelFormat, desiredOrigin, converterConfig))
    {
      throw GraphicsException(fmt::format("Unsupported conversion from '{}, {}' to '{}, {}'", rBitmap.GetPixelFormat(), rBitmap.GetOrigin(),
                                          desiredPixelFormat, desiredOrigin));
    }
  }


  void BitmapConverterService::Convert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                                       const BitmapConverterConfig& converterConfig)
  {
    if (!TryConvert(rTexture, desiredPixelFormat, desiredOrigin, converterConfig))
    {
      throw GraphicsException(fmt::format("Unsupported conversion from '{}, {}' to '{}, {}'", rTexture.GetPixelFormat(), rTexture.GetBitmapOrigin(),
                                          desiredPixelFormat, desiredOrigin));
    }
  }

}
