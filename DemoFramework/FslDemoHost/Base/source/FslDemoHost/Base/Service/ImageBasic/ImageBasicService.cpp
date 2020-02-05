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

#include <FslDemoHost/Base/Service/ImageBasic/ImageBasicService.hpp>
#include <FslDemoApp/Base/Service/ImageLibrary/IImageLibraryService.hpp>
#include <FslDemoApp/Base/Service/BitmapConverter/IBitmapConverter.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/IO/File.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/ContainerTypeConvert.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <FslGraphics/IO/BMPUtil.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/TextureBlobBuilder.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  namespace
  {
    bool TryLoadViaImageService(const std::shared_ptr<IImageLibraryService>& imageLibraryService, Bitmap& rBitmap, const IO::Path& absolutePath,
                                const PixelFormat desiredPixelFormat, const BitmapOrigin usedOriginHint,
                                const PixelChannelOrder preferredChannelOrderHint)
    {
      if (!imageLibraryService)
      {
        return false;
      }

      // If pixel format is set to undefined we try to load into the 'native' format of the image
      return imageLibraryService->TryRead(rBitmap, absolutePath, desiredPixelFormat, usedOriginHint, preferredChannelOrderHint);
    }


    bool TryLoadViaImageService(const std::shared_ptr<IImageLibraryService>& imageLibraryService, Texture& rTexture, const IO::Path& absolutePath,
                                const PixelFormat desiredPixelFormat, const BitmapOrigin usedOriginHint,
                                const PixelChannelOrder preferredChannelOrderHint)
    {
      if (!imageLibraryService)
      {
        return false;
      }
      // If pixel format is set to undefined we try to load into the 'native' format of the image
      return imageLibraryService->TryRead(rTexture, absolutePath, desiredPixelFormat, usedOriginHint, preferredChannelOrderHint);
    }
  }


  ImageBasicService::ImageBasicService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_bitmapConverter(serviceProvider.Get<IBitmapConverter>())

  {
    serviceProvider.Get<IImageLibraryService>(m_imageLibraryServices);

    // Sort the library services by name to ensure that we have a consistent order
    auto funcByNameComp = [](const std::shared_ptr<IImageLibraryService>& lhs, const std::shared_ptr<IImageLibraryService>& rhs) {
      return lhs->GetName() > rhs->GetName();
    };
    std::sort(m_imageLibraryServices.begin(), m_imageLibraryServices.end(), funcByNameComp);

    // Query all libs for their supported extensions (note its optional for them to provide this list)
    std::deque<ImageFormat> formats;
    for (auto itr = m_imageLibraryServices.begin(); itr != m_imageLibraryServices.end(); ++itr)
    {
      formats.clear();
      (*itr)->ExtractSupportedImageFormats(formats);
      for (auto itrFormat = formats.begin(); itrFormat != formats.end(); ++itrFormat)
      {
        auto itrFind = m_formatToImageLibrary.find(*itrFormat);
        if (itrFind == m_formatToImageLibrary.end())
        {
          m_formatToImageLibrary[*itrFormat] = std::make_shared<ImageLibraryDeque>();
          m_formatToImageLibrary[*itrFormat]->push_back(*itr);
        }
        else
        {
          itrFind->second->push_back(*itr);
        }
      }
    }
  }


  ImageBasicService::~ImageBasicService() = default;


  void ImageBasicService::Read(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                               const PixelChannelOrder preferredChannelOrder) const
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      throw std::invalid_argument(fmt::format("Read only takes absolute paths: '{}'", absolutePath));
    }

    if (!IO::File::Exists(absolutePath))
    {
      throw std::invalid_argument(fmt::format("File not found: '{}'", absolutePath));
    }

    // Try all image services one by one (basic strategy for now)
    bool isLoaded = false;

    // Try to guess which service we need by looking at the file extension, and then check of there is a image service available for the format
    const ImageFormat imageFormatBasedOnExt = ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath);
    if (imageFormatBasedOnExt != ImageFormat::Undefined)
    {
      const auto itrFind = m_formatToImageLibrary.find(imageFormatBasedOnExt);
      if (itrFind != m_formatToImageLibrary.end())
      {
        auto itrCurrent = itrFind->second->begin();
        const auto itrCurrentEnd = itrFind->second->end();
        while (itrCurrent != itrCurrentEnd && !isLoaded)
        {
          isLoaded = TryLoadViaImageService(*itrCurrent, rBitmap, absolutePath, desiredPixelFormat, desiredOrigin, preferredChannelOrder);
          ++itrCurrent;
        }
      }
    }

    // No such luck, so lets just try all the registered image services
    for (auto itr = m_imageLibraryServices.begin(); !isLoaded && itr != m_imageLibraryServices.end(); ++itr)
    {
      isLoaded = TryLoadViaImageService(*itr, rBitmap, absolutePath, desiredPixelFormat, desiredOrigin, preferredChannelOrder);
    }

    // The fall back loader only supports BMP
    if (!isLoaded && imageFormatBasedOnExt == ImageFormat::Bmp)
    {
      BMPUtil::Load(rBitmap, absolutePath, desiredOrigin);
    }

    if (!isLoaded)
    {
      throw NotSupportedException(fmt::format("None of the available image libraries could load: '{}'", absolutePath));
    }

    const auto usedDesiredPixelFormat = (desiredPixelFormat != PixelFormat::Undefined ? desiredPixelFormat : rBitmap.GetPixelFormat());

    if (rBitmap.GetPixelFormat() != usedDesiredPixelFormat || rBitmap.GetOrigin() != desiredOrigin)
    {
      m_bitmapConverter->Convert(rBitmap, usedDesiredPixelFormat, desiredOrigin);
    }

    // When loading a undefined pixel format we prefer the unorm variant
    if (desiredPixelFormat == PixelFormat::Undefined)
    {
      rBitmap.TrySetCompatiblePixelFormatFlag(PixelFormatFlags::NF_UNorm);
    }
  }


  void ImageBasicService::Read(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat,
                               const BitmapOrigin desiredOrigin, const PixelChannelOrder preferredChannelOrder) const
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      throw std::invalid_argument(fmt::format("Read only takes absolute paths: '{}'", absolutePath));
    }

    if (!IO::File::Exists(absolutePath))
    {
      throw std::invalid_argument(fmt::format("File not found: '{}'", absolutePath));
    }


    // Try all image services one by one (basic strategy for now)
    bool isLoaded = false;

    // Try to guess which service we need by looking at the file extension, and then check of there is a image service available for the format
    const ImageFormat imageFormatBasedOnExt = ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath);
    if (imageFormatBasedOnExt != ImageFormat::Undefined)
    {
      const auto itrFind = m_formatToImageLibrary.find(imageFormatBasedOnExt);
      if (itrFind != m_formatToImageLibrary.end())
      {
        auto itrCurrent = itrFind->second->begin();
        const auto itrCurrentEnd = itrFind->second->end();
        while (itrCurrent != itrCurrentEnd && !isLoaded)
        {
          isLoaded = TryLoadViaImageService(*itrCurrent, rTexture, absolutePath, desiredPixelFormat, desiredOrigin, preferredChannelOrder);
          ++itrCurrent;
        }
      }
    }

    // No such luck, so lets just try all the registered image services
    for (auto itr = m_imageLibraryServices.begin(); !isLoaded && itr != m_imageLibraryServices.end(); ++itr)
    {
      isLoaded = TryLoadViaImageService(*itr, rTexture, absolutePath, desiredPixelFormat, desiredOrigin, preferredChannelOrder);
    }

    if (!isLoaded)
    {
      Bitmap tmpBitmap;
      Read(tmpBitmap, absolutePath, desiredPixelFormat, desiredOrigin);

      assert(desiredPixelFormat == PixelFormat::Undefined || desiredPixelFormat == tmpBitmap.GetPixelFormat());
      assert(desiredOrigin == BitmapOrigin::Undefined || desiredOrigin == tmpBitmap.GetOrigin());

      rTexture = ContainerTypeConvert::Convert(std::move(tmpBitmap));
    }

    const bool isCompressed = PixelFormatUtil::IsCompressed(rTexture.GetPixelFormat());
    if (isCompressed && desiredOrigin != BitmapOrigin::Undefined && rTexture.GetBitmapOrigin() != desiredOrigin)
    {
      throw NotSupportedException("The origin of compressed formats can not be modified");
    }

    const auto usedDesiredPixelFormat = (desiredPixelFormat != PixelFormat::Undefined ? desiredPixelFormat : rTexture.GetPixelFormat());

    if (rTexture.GetPixelFormat() != usedDesiredPixelFormat || rTexture.GetBitmapOrigin() != desiredOrigin)
    {
      m_bitmapConverter->Convert(rTexture, usedDesiredPixelFormat, desiredOrigin);
    }

    // When loading a undefined pixel format we prefer the unorm variant
    if (desiredPixelFormat == PixelFormat::Undefined)
    {
      rTexture.TrySetCompatiblePixelFormatFlag(PixelFormatFlags::NF_UNorm);
    }
  }


  void ImageBasicService::Write(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      throw std::invalid_argument(fmt::format("Write only takes absolute paths: {}", absolutePath));
    }

    const auto usedDesiredPixelFormat = (desiredPixelFormat != PixelFormat::Undefined ? desiredPixelFormat : bitmap.GetPixelFormat());
    const auto usedImageFormat =
      (imageFormat != ImageFormat::Undefined ? imageFormat : ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath));

    if (bitmap.GetPixelFormat() == usedDesiredPixelFormat)
    {
      DoWrite(absolutePath, bitmap, usedImageFormat);
    }
    else
    {
      Bitmap tmpBitmap = bitmap;
      m_bitmapConverter->Convert(tmpBitmap, usedDesiredPixelFormat);
      DoWrite(absolutePath, tmpBitmap, usedImageFormat);
    }
  }


  void ImageBasicService::WriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                          const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      throw std::invalid_argument(fmt::format("WriteExactImage only takes absolute paths: '{}'", absolutePath));
    }

    const auto usedDesiredPixelFormat = (desiredPixelFormat != PixelFormat::Undefined ? desiredPixelFormat : bitmap.GetPixelFormat());
    const auto usedImageFormat =
      (imageFormat != ImageFormat::Undefined ? imageFormat : ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath));

    if (bitmap.GetPixelFormat() == usedDesiredPixelFormat)
    {
      DoWriteExactImage(absolutePath, bitmap, usedImageFormat);
    }
    else
    {
      Bitmap tmpBitmap = bitmap;
      m_bitmapConverter->Convert(tmpBitmap, usedDesiredPixelFormat);
      DoWriteExactImage(absolutePath, tmpBitmap, usedImageFormat);
    }
  }


  bool ImageBasicService::TryRead(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat,
                                  const BitmapOrigin desiredOrigin, const PixelChannelOrder preferredChannelOrder) const
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      FSLLOG3_DEBUG_WARNING("Read only takes absolute paths: '{}'", absolutePath);
      return false;
    }

    if (!IO::File::Exists(absolutePath))
    {
      FSLLOG3_DEBUG_WARNING("File not found: '{}'", absolutePath);
      return false;
    }

    // For now we just reuse the exception based one
    try
    {
      Read(rBitmap, absolutePath, desiredPixelFormat, desiredOrigin, preferredChannelOrder);
      return true;
    }
    catch (const std::exception& ex)
    {
      FSL_PARAM_NOT_USED(ex);
      FSLLOG3_DEBUG_WARNING("TryRead failed with {}", ex.what());
      return false;
    }
  }


  bool ImageBasicService::TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                   const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      FSLLOG3_DEBUG_WARNING("TryWrite only takes absolute paths: {}", absolutePath);
      return false;
    }


    // For now we just reuse the exception based one
    try
    {
      Write(absolutePath, bitmap, imageFormat, desiredPixelFormat);
      return true;
    }
    catch (const std::exception& ex)
    {
      FSL_PARAM_NOT_USED(ex);
      FSLLOG3_DEBUG_WARNING("TryWrite failed with {}", ex.what());
      return false;
    }
  }


  bool ImageBasicService::TryWriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                             const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      FSLLOG3_DEBUG_WARNING("TryWriteExactImage only takes absolute paths: {}", absolutePath);
      return false;
    }

    // For now we just reuse the exception based one
    try
    {
      WriteExactImage(absolutePath, bitmap, imageFormat, desiredPixelFormat);
      return true;
    }
    catch (const std::exception& ex)
    {
      FSL_PARAM_NOT_USED(ex);
      FSLLOG3_DEBUG_WARNING("TryWriteExactImage failed with {}", ex.what());
      return false;
    }
  }


  void ImageBasicService::DoWrite(const IO::Path& absPath, const Bitmap& bitmap, const ImageFormat imageFormat)
  {
    // If there is a image service available for the format
    const auto itrFind = m_formatToImageLibrary.find(imageFormat);
    if (itrFind != m_formatToImageLibrary.end())
    {
      auto itrCurrent = itrFind->second->begin();
      const auto itrCurrentEnd = itrFind->second->end();
      while (itrCurrent != itrCurrentEnd)
      {
        if ((*itrCurrent)->TryWrite(absPath, bitmap, imageFormat, true))
        {
          return;
        }
        ++itrCurrent;
      }
    }

    // No such luck, so lets just try all the registered image service
    for (auto itr = m_imageLibraryServices.begin(); itr != m_imageLibraryServices.end(); ++itr)
    {
      if ((*itr)->TryWrite(absPath, bitmap, imageFormat, true))
      {
        return;
      }
    }

    // Still no luck, lets try the bmp fall back writer
    if (imageFormat == ImageFormat::Bmp)
    {
      // This only supports pixel format layout B8G8R8A8
      if (PixelFormatUtil::GetPixelFormatLayout(bitmap.GetPixelFormat()) == PixelFormatLayout::B8G8R8A8)
      {
        BMPUtil::Save(absPath, bitmap);
      }
      else
      {
        // Convert to a supported pixel format if possible
        Bitmap tmpBitmap = bitmap;
        m_bitmapConverter->Convert(tmpBitmap, PixelFormat::B8G8R8A8_UINT);
        BMPUtil::Save(absPath, tmpBitmap);
      }
      return;
    }

    // We tried the options we had available, so fail
    throw NotSupportedException("The desired image format and pixel format combination is not supported");
  }


  void ImageBasicService::DoWriteExactImage(const IO::Path& absPath, const Bitmap& bitmap, const ImageFormat imageFormat)
  {
    assert(imageFormat != ImageFormat::Undefined);

    // If there is a image service available for the format
    const auto itrFind = m_formatToImageLibrary.find(imageFormat);
    if (itrFind != m_formatToImageLibrary.end())
    {
      auto itrCurrent = itrFind->second->begin();
      const auto itrCurrentEnd = itrFind->second->end();
      while (itrCurrent != itrCurrentEnd)
      {
        if ((*itrCurrent)->TryWrite(absPath, bitmap, imageFormat, true))
        {
          return;
        }
      }
    }


    // No such luck, so lets just try all the registered image service
    for (auto itr = m_imageLibraryServices.begin(); itr != m_imageLibraryServices.end(); ++itr)
    {
      if ((*itr)->TryWrite(absPath, bitmap, imageFormat, true))
      {
        return;
      }
    }

    // Still no luck, lets try the bmp fall back writer
    if (imageFormat == ImageFormat::Bmp)
    {
      // This only supports B8G8R8A8_UINT
      BMPUtil::Save(absPath, bitmap);
      return;
    }

    // We tried the options we had available, so fail
    throw NotSupportedException("The desired image format and pixel format combination is not supported. Try a different format or 'Write' instead");
  }
}
