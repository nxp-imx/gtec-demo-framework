#ifdef FSL_FEATURE_STB
/****************************************************************************************************************************************************
 * Copyright 2017, 2022, 2024 NXP
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

#include <FslBase/IO/File.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Create.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Util/Graphics/Service/ImageLibrary/ImageLibrarySTBService.hpp>
#include <FslDemoService/BitmapConverter/IBitmapConverter.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <stb_image_write.h>
#include <limits>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr int DefaultJpgQuality = 85;
      constexpr int MinComp = 1;
      constexpr int MaxComp = 4;
    }


    // STB pixel formats: 1=Y, 2=YA, 3=RGB, 4=RGBA. (Y is monochrome color)

    // STB format Notes:
    // - BMP format expands Y to RGB in the file format and does not
    // - PNG creates output files with the same number of components as the input.
    //   output alpha.
    // - HDR expects linear float data. Since the format is always 32-bit rgb(e)
    // - JPEG does ignore alpha channels in input data; quality is between 1 and 100. Higher quality looks better but results in a bigger image.
    //   JPEG baseline(no JPEG progressive).

    constexpr bool IsSupported(const PixelFormat pixelFormat, const ImageFormat imageFormat) noexcept
    {
      if (imageFormat != ImageFormat::Hdr)
      {
        switch (pixelFormat)
        {
        case PixelFormat::R8_UNORM:
        case PixelFormat::R8_UINT:
        case PixelFormat::R8_SRGB:
        case PixelFormat::R8G8_UNORM:
        case PixelFormat::R8G8_UINT:
        case PixelFormat::R8G8_SRGB:
        case PixelFormat::R8G8B8_UNORM:
        case PixelFormat::R8G8B8_UINT:
        case PixelFormat::R8G8B8_SRGB:
        case PixelFormat::R8G8B8A8_UNORM:
        case PixelFormat::R8G8B8A8_UINT:
        case PixelFormat::R8G8B8A8_SRGB:
          return true;
        default:
          return false;
        }
      }

      // HDR
      switch (pixelFormat)
      {
      case PixelFormat::R32_SFLOAT:
      case PixelFormat::R32G32_SFLOAT:
      case PixelFormat::R32G32B32_SFLOAT:
      case PixelFormat::R32G32B32A32_SFLOAT:
        return true;
      default:
        return false;
      }
    }


    constexpr PixelFormat TryFindCompatiblePixelFormat(const PixelFormat pixelFormat) noexcept
    {
      switch (pixelFormat)
      {
      case PixelFormat::B8G8R8_UNORM:
      case PixelFormat::R8G8B8_UNORM:
        return PixelFormat::R8G8B8_UNORM;
      case PixelFormat::B8G8R8_SNORM:
      case PixelFormat::R8G8B8_SNORM:
        return PixelFormat::R8G8B8_SNORM;
      case PixelFormat::B8G8R8_USCALED:
      case PixelFormat::R8G8B8_USCALED:
        return PixelFormat::R8G8B8_USCALED;
      case PixelFormat::B8G8R8_SSCALED:
      case PixelFormat::R8G8B8_SSCALED:
        return PixelFormat::R8G8B8_SSCALED;
      case PixelFormat::B8G8R8_UINT:
      case PixelFormat::R8G8B8_UINT:
        return PixelFormat::R8G8B8_UINT;
      case PixelFormat::B8G8R8_SINT:
      case PixelFormat::R8G8B8_SINT:
        return PixelFormat::R8G8B8_SINT;
      case PixelFormat::B8G8R8_SRGB:
      case PixelFormat::R8G8B8_SRGB:
        return PixelFormat::R8G8B8_SRGB;
      case PixelFormat::B8G8R8A8_UNORM:
      case PixelFormat::R8G8B8A8_UNORM:
        return PixelFormat::R8G8B8A8_UNORM;
      case PixelFormat::B8G8R8A8_SNORM:
      case PixelFormat::R8G8B8A8_SNORM:
        return PixelFormat::R8G8B8A8_SNORM;
      case PixelFormat::B8G8R8A8_USCALED:
      case PixelFormat::R8G8B8A8_USCALED:
        return PixelFormat::R8G8B8A8_USCALED;
      case PixelFormat::B8G8R8A8_SSCALED:
      case PixelFormat::R8G8B8A8_SSCALED:
        return PixelFormat::R8G8B8A8_SSCALED;
      case PixelFormat::B8G8R8A8_UINT:
      case PixelFormat::R8G8B8A8_UINT:
        return PixelFormat::R8G8B8A8_UINT;
      case PixelFormat::B8G8R8A8_SINT:
      case PixelFormat::R8G8B8A8_SINT:
        return PixelFormat::R8G8B8A8_SINT;
      case PixelFormat::B8G8R8A8_SRGB:
      case PixelFormat::R8G8B8A8_SRGB:
        return PixelFormat::R8G8B8A8_SRGB;
      case PixelFormat::R16G16B16_SFLOAT:
        return PixelFormat::R32G32B32_SFLOAT;
      case PixelFormat::R16G16B16A16_SFLOAT:
        return PixelFormat::R32G32B32A32_SFLOAT;
      default:
        return PixelFormat::Undefined;
      }
    }

    template <typename T>
    class ScopedSTBImage
    {
    public:
      ScopedSTBImage(const ScopedSTBImage&) = delete;
      ScopedSTBImage& operator=(const ScopedSTBImage&) = delete;

      // NOLINTNEXTLINE(readability-identifier-naming)
      T* pContent;

      explicit ScopedSTBImage(T* pImageData)
        : pContent(pImageData)
      {
      }

      ~ScopedSTBImage()
      {
        if (pContent != nullptr)
        {
          stbi_image_free(pContent);
          pContent = nullptr;
        }
      }
    };

    bool TryReadHDR(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat pixelFormatHint, const BitmapOrigin originHint,
                    const PixelChannelOrder preferredChannelOrderHint)
    {
      FSL_PARAM_NOT_USED(pixelFormatHint);
      FSL_PARAM_NOT_USED(originHint);
      FSL_PARAM_NOT_USED(preferredChannelOrderHint);

      int width = 0;
      int height = 0;
      int channels = 0;

      ScopedSTBImage<float> imageData(stbi_loadf(absolutePath.ToUTF8String().c_str(), &width, &height, &channels, 0));
      if (imageData.pContent == nullptr || width < 0 || height < 0 || (channels != 3 && channels != 4))
      {
        return false;
      }

      try
      {
        const PixelFormat pixelFormat = (channels == 3 ? PixelFormat::R32G32B32_SFLOAT : PixelFormat::R32G32B32A32_SFLOAT);
        auto sizePx = PxSize2D::Create(width, height);
        const std::size_t cbContent = (sizeof(float) * channels) * sizePx.RawUnsignedWidth() * sizePx.RawUnsignedHeight();

        rBitmap.Reset(SpanUtil::CreateReadOnly(reinterpret_cast<const uint8_t*>(imageData.pContent), cbContent), sizePx, pixelFormat);
        return true;
      }
      catch (const std::exception&)
      {
        return false;
      }
    }


    bool TryReadImage(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat pixelFormatHint, const BitmapOrigin originHint,
                      const PixelChannelOrder preferredChannelOrderHint)
    {
      FSL_PARAM_NOT_USED(pixelFormatHint);
      FSL_PARAM_NOT_USED(originHint);
      FSL_PARAM_NOT_USED(preferredChannelOrderHint);

      int width = 0;
      int height = 0;
      int channels = 0;

      ScopedSTBImage<uint8_t> imageData(stbi_load(absolutePath.ToUTF8String().c_str(), &width, &height, &channels, 0));
      if (imageData.pContent == nullptr || width < 0 || height < 0 || (channels != 3 && channels != 4))
      {
        return false;
      }

      try
      {
        const PixelFormat pixelFormat = (channels == 3 ? PixelFormat::R8G8B8_UINT : PixelFormat::R8G8B8A8_UINT);
        auto sizePx = PxSize2D::Create(width, height);
        const std::size_t cbContent = channels * sizePx.RawUnsignedWidth() * sizePx.RawUnsignedHeight();

        rBitmap.Reset(SpanUtil::CreateReadOnly(imageData.pContent, cbContent), sizePx, pixelFormat);
        return true;
      }
      catch (const std::exception&)
      {
        return false;
      }
    }
  }


  ImageLibrarySTBService::ImageLibrarySTBService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_bitmapConverter(serviceProvider.Get<IBitmapConverter>())
  {
  }


  ImageLibrarySTBService::~ImageLibrarySTBService() = default;


  std::string ImageLibrarySTBService::GetName() const
  {
    return {"-ImageLibrarySTBService-"};
  }


  void ImageLibrarySTBService::ExtractSupportedImageFormats(std::deque<ImageFormat>& rFormats)
  {
    rFormats.push_back(ImageFormat::Bmp);
    rFormats.push_back(ImageFormat::Jpeg);
    rFormats.push_back(ImageFormat::Png);
    rFormats.push_back(ImageFormat::Tga);
    rFormats.push_back(ImageFormat::Hdr);
  }


  bool ImageLibrarySTBService::TryRead(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat pixelFormatHint,
                                       const BitmapOrigin originHint, const PixelChannelOrder preferredChannelOrderHint)
  {
    const auto imageFormat = ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath);
    switch (imageFormat)
    {
    case ImageFormat::Hdr:
      return TryReadHDR(rBitmap, absolutePath, pixelFormatHint, originHint, preferredChannelOrderHint);
    case ImageFormat::Bmp:
    case ImageFormat::Jpeg:
    case ImageFormat::Png:
    case ImageFormat::Tga:
      return TryReadImage(rBitmap, absolutePath, pixelFormatHint, originHint, preferredChannelOrderHint);
    default:
      return false;
    }
  }


  bool ImageLibrarySTBService::TryRead(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat pixelFormatHint,
                                       const BitmapOrigin originHint, const PixelChannelOrder preferredChannelOrderHint)
  {
    FSL_PARAM_NOT_USED(rTexture);
    FSL_PARAM_NOT_USED(absolutePath);
    FSL_PARAM_NOT_USED(pixelFormatHint);
    FSL_PARAM_NOT_USED(originHint);
    FSL_PARAM_NOT_USED(preferredChannelOrderHint);
    return false;
  }


  bool ImageLibrarySTBService::TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const bool allowOverwrite)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      return false;
    }
    if (!allowOverwrite && IO::File::Exists(absolutePath))
    {
      return false;
    }

    if (bitmap.GetOrigin() == BitmapOrigin::UpperLeft && IsSupported(bitmap.GetPixelFormat(), imageFormat))
    {
      return TryWriteNow(absolutePath, bitmap, imageFormat);
    }

    // Try to see if there is a directly compatible format we can try instead
    // The fallback mode here is necessary to improve compatibility until we add better 'write' support where
    // a lib can list the image formats and which pixel formats+origins it supports.
    const auto compatiblePixelFormat = TryFindCompatiblePixelFormat(bitmap.GetPixelFormat());
    if (compatiblePixelFormat == PixelFormat::Undefined)
    {
      return false;
    }

    // We found a fallback format, so lets try to convert the image
    Bitmap bitmapClone(bitmap);
    if (!m_bitmapConverter->TryConvert(bitmapClone, compatiblePixelFormat, BitmapOrigin::UpperLeft))
    {
      return false;
    }

    if (!IsSupported(bitmapClone.GetPixelFormat(), imageFormat))
    {
      return false;
    }
    return TryWriteNow(absolutePath, bitmapClone, imageFormat);
  }


  bool ImageLibrarySTBService::TryWriteNow(const IO::Path& dstName, const Bitmap& bitmap, const ImageFormat imageFormat)
  {
    assert(IsSupported(bitmap.GetPixelFormat(), imageFormat));
    assert(IO::Path::IsPathRooted(dstName));
    int comp = NumericCast<int32_t>(PixelFormatUtil::GetChannelCount(bitmap.GetPixelFormat()));
    if (comp < LocalConfig::MinComp || comp > LocalConfig::MaxComp)
    {
      return false;
    }
    const auto& dstPathNameString = dstName.ToUTF8String();

    const Bitmap::ScopedDirectReadAccess directAccess(bitmap);

    const auto minimumStride = PixelFormatUtil::CalcMinimumStride(directAccess.AsRawBitmap().Width(), directAccess.AsRawBitmap().GetPixelFormat());
    if (imageFormat != ImageFormat::Png && directAccess.AsRawBitmap().Stride() != minimumStride)
    {
      return false;
    }

    const auto rawBitmapWidth = directAccess.AsRawBitmap().RawWidth();
    const auto rawBitmapHeight = directAccess.AsRawBitmap().RawHeight();

    switch (imageFormat)
    {
    case ImageFormat::Bmp:
      return stbi_write_bmp(dstPathNameString.c_str(), rawBitmapWidth, rawBitmapHeight, comp, directAccess.AsRawBitmap().Content()) != 0;
    case ImageFormat::Hdr:
      return stbi_write_hdr(dstPathNameString.c_str(), rawBitmapWidth, rawBitmapHeight, comp,
                            static_cast<const float*>(directAccess.AsRawBitmap().Content())) != 0;
    case ImageFormat::Jpeg:
      return stbi_write_jpg(dstPathNameString.c_str(), rawBitmapWidth, rawBitmapHeight, comp, directAccess.AsRawBitmap().Content(),
                            LocalConfig::DefaultJpgQuality) != 0;
    case ImageFormat::Png:
      // NOLINTNEXTLINE(misc-redundant-expression)
      static_assert(std::numeric_limits<int>::max() >= std::numeric_limits<int32_t>::max(), "We expect int to be at least 32bit");
      if (directAccess.AsRawBitmap().Stride() >= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
      {
        return false;
      }
      return stbi_write_png(dstPathNameString.c_str(), rawBitmapWidth, rawBitmapHeight, comp, directAccess.AsRawBitmap().Content(),
                            UncheckedNumericCast<int>(directAccess.AsRawBitmap().Stride())) != 0;
    case ImageFormat::Tga:
      return stbi_write_tga(dstPathNameString.c_str(), rawBitmapWidth, rawBitmapHeight, comp, directAccess.AsRawBitmap().Content()) != 0;
    default:
      return false;
    }
  }
}
#endif
