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

#include <FslDemoApp/Service/ImageLibrary/IImageLibraryService.hpp>
#include <FslDemoApp/Service/BitmapConverter/IBitmapConverter.hpp>
#include <FslDemoHost/Service/Image/ImageService.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/IO/File.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
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
    bool TryLoadViaImageService(const std::shared_ptr<IImageLibraryService>& imageLibraryService, Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin usedOriginHint)
    {
      if (!imageLibraryService)
        return false;

      // If pixel format is set to undefined we try to load into the 'native' format of the image
      return imageLibraryService->TryRead(rBitmap, absolutePath, desiredPixelFormat, usedOriginHint);
    }


    bool TryLoadViaImageService(const std::shared_ptr<IImageLibraryService>& imageLibraryService, Texture& rTexture, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin usedOriginHint)
    {
      if (!imageLibraryService)
        return false;
      // If pixel format is set to undefined we try to load into the 'native' format of the image
      return imageLibraryService->TryRead(rTexture, absolutePath, desiredPixelFormat, usedOriginHint);
    }

    void BitmapToTexture(Texture& rTexture, const Bitmap& srcBitmap)
    {
      RawBitmap rawSrcBitmap;
      Bitmap::ScopedDirectAccess directAccess(srcBitmap, rawSrcBitmap);

      const auto minStride = PixelFormatUtil::CalcMinimumStride(srcBitmap.Width(), srcBitmap.GetPixelFormat());
      if (rawSrcBitmap.Stride() != minStride)
        throw NotSupportedException("We currently only support the minimum required stride");

      const auto bitmapExtent = rawSrcBitmap.GetExtent();
      const Extent3D extent(bitmapExtent.Width, bitmapExtent.Height, 1);
      const TextureInfo textureInfo(1, 1, 1);

      TextureBlobBuilder builder(TextureType::Tex2D, extent, rawSrcBitmap.GetPixelFormat(), textureInfo, rawSrcBitmap.GetOrigin(), rawSrcBitmap.GetBufferLength());
      builder.SetDefaultBlobLayout();
      rTexture.Reset(rawSrcBitmap.Content(), rawSrcBitmap.GetBufferLength(), std::move(builder));
    }

  }


  ImageService::ImageService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_bitmapConverter(serviceProvider.Get<IBitmapConverter>())
    , m_imageLibraryServices()
    , m_bitmapOrigin(BitmapOrigin::UpperLeft)
  {
    serviceProvider.Get<IImageLibraryService>(m_imageLibraryServices);

    // Sort the library services by name to ensure that we have a consistent order
    auto funcByNameComp = [](const std::shared_ptr<IImageLibraryService>& lhs, const std::shared_ptr<IImageLibraryService>& rhs) {return lhs->GetName() < rhs->GetName(); };
    std::sort(m_imageLibraryServices.begin(), m_imageLibraryServices.end(), funcByNameComp);

    // Query all libs for their supported extensions (note its optional for them to provide this list)
    std::deque<ImageFormat> formats;
    for (auto itr = m_imageLibraryServices.begin(); itr != m_imageLibraryServices.end(); ++itr)
    {
      formats.clear();
      (*itr)->ExtractSupportedImageFormats(formats);
      for (auto itrFormat = formats.begin(); itrFormat != formats.end(); ++itrFormat)
      {
        if (m_formatToImageLibrary.find(*itrFormat) == m_formatToImageLibrary.end())
        {
          m_formatToImageLibrary[*itrFormat] = *itr;
        }
      }
    }
  }


  ImageService::~ImageService()
  {
  }


  BitmapOrigin ImageService::GetPreferredBitmapOrigin() const
  {
    return m_bitmapOrigin;
  }


  void ImageService::Read(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin) const
  {
    if (!IO::Path::IsPathRooted(absolutePath))
      throw std::invalid_argument(std::string("Read only takes absolute paths: ") + absolutePath.ToUTF8String());

    if (!IO::File::Exists(absolutePath))
      throw std::invalid_argument(std::string("File not found: ") + absolutePath.ToUTF8String());


    const auto usedOrigin = (desiredOrigin != BitmapOrigin::Undefined ? desiredOrigin : m_bitmapOrigin);

    // Try all image services one by one (basic strategy for now)
    bool isLoaded = false;

    // Try to guess which service we need by looking at the file extension, and then check of there is a image service available for the format
    const ImageFormat imageFormatBasedOnExt = ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath);
    if (imageFormatBasedOnExt != ImageFormat::Undefined)
    {
      const auto itrFind = m_formatToImageLibrary.find(imageFormatBasedOnExt);
      if (itrFind != m_formatToImageLibrary.end())
        isLoaded = TryLoadViaImageService(itrFind->second, rBitmap, absolutePath, desiredPixelFormat, usedOrigin);
    }

    // No such luck, so lets just try all the registered image services
    for (auto itr = m_imageLibraryServices.begin(); !isLoaded && itr != m_imageLibraryServices.end(); ++itr)
    {
      isLoaded = TryLoadViaImageService(*itr, rBitmap, absolutePath, desiredPixelFormat, usedOrigin);
    }

    // The fall back loader only supports BMP
    if (!isLoaded)
      BMPUtil::Load(rBitmap, absolutePath, usedOrigin);

    if (!isLoaded)
      throw NotSupportedException(std::string("None of the available image libraries could load: ") + absolutePath.ToUTF8String());

    const auto usedDesiredPixelFormat = (desiredPixelFormat != PixelFormat::Undefined ? desiredPixelFormat : rBitmap.GetPixelFormat());

    if (rBitmap.GetPixelFormat() != usedDesiredPixelFormat || rBitmap.GetOrigin() != usedOrigin)
      m_bitmapConverter->Convert(rBitmap, usedDesiredPixelFormat, usedOrigin);
  }


  void ImageService::Read(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin) const
  {
    if (!IO::Path::IsPathRooted(absolutePath))
      throw std::invalid_argument(std::string("Read only takes absolute paths: ") + absolutePath.ToUTF8String());

    if (!IO::File::Exists(absolutePath))
      throw std::invalid_argument(std::string("File not found: ") + absolutePath.ToUTF8String());

    const auto usedOrigin = (desiredOrigin != BitmapOrigin::Undefined ? desiredOrigin : m_bitmapOrigin);

    // Try all image services one by one (basic strategy for now)
    bool isLoaded = false;

    // Try to guess which service we need by looking at the file extension, and then check of there is a image service available for the format
    const ImageFormat imageFormatBasedOnExt = ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath);
    if (imageFormatBasedOnExt != ImageFormat::Undefined)
    {
      const auto itrFind = m_formatToImageLibrary.find(imageFormatBasedOnExt);
      if (itrFind != m_formatToImageLibrary.end())
        isLoaded = TryLoadViaImageService(itrFind->second, rTexture, absolutePath, desiredPixelFormat, usedOrigin);
    }

    // No such luck, so lets just try all the registered image services
    for (auto itr = m_imageLibraryServices.begin(); !isLoaded && itr != m_imageLibraryServices.end(); ++itr)
    {
      isLoaded = TryLoadViaImageService(*itr, rTexture, absolutePath, desiredPixelFormat, usedOrigin);
    }

    if (!isLoaded)
    {
      Bitmap tmpBitmap;
      Read(tmpBitmap, absolutePath, desiredPixelFormat, desiredOrigin);

      assert(desiredPixelFormat == PixelFormat::Undefined || desiredPixelFormat == tmpBitmap.GetPixelFormat());
      assert(desiredOrigin == BitmapOrigin::Undefined || desiredOrigin == tmpBitmap.GetOrigin());

      BitmapToTexture(rTexture, tmpBitmap);
    }

    const bool isCompressed = PixelFormatUtil::IsCompressed(rTexture.GetPixelFormat());
    if (isCompressed && desiredOrigin != BitmapOrigin::Undefined && rTexture.GetBitmapOrigin() != usedOrigin)
      throw NotSupportedException("The origin of compressed formats can not be modified");

    const auto usedDesiredPixelFormat = (desiredPixelFormat != PixelFormat::Undefined ? desiredPixelFormat : rTexture.GetPixelFormat());

    if (rTexture.GetPixelFormat() != usedDesiredPixelFormat || rTexture.GetBitmapOrigin() != usedOrigin)
      m_bitmapConverter->Convert(rTexture, usedDesiredPixelFormat, usedOrigin);
  }


  void ImageService::Write(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
      throw std::invalid_argument(std::string("Read only takes absolute paths: ") + absolutePath.ToUTF8String());

    const auto usedDesiredPixelFormat = (desiredPixelFormat != PixelFormat::Undefined ? desiredPixelFormat : bitmap.GetPixelFormat());
    const auto usedImageFormat = (imageFormat != ImageFormat::Undefined ? imageFormat : ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath));

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


  void ImageService::WriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
      throw std::invalid_argument(std::string("Read only takes absolute paths: ") + absolutePath.ToUTF8String());

    const auto usedDesiredPixelFormat = (desiredPixelFormat != PixelFormat::Undefined ? desiredPixelFormat : bitmap.GetPixelFormat());
    const auto usedImageFormat = (imageFormat != ImageFormat::Undefined ? imageFormat : ImageFormatUtil::TryDetectImageFormatFromExtension(absolutePath));

    if (bitmap.GetPixelFormat() == usedDesiredPixelFormat)
    {
      DoWrite(absolutePath, bitmap, usedImageFormat);
    }
    else
    {
      Bitmap tmpBitmap = bitmap;
      m_bitmapConverter->Convert(tmpBitmap, usedDesiredPixelFormat);
      DoWriteExactImage(absolutePath, tmpBitmap, usedImageFormat);
    }
  }


  bool ImageService::TryRead(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin) const
  {
    // For now we just reuse the exception based one
    try
    {
      Read(rBitmap, absolutePath, desiredPixelFormat, desiredOrigin);
      return true;
    }
    catch (const std::exception&)
    {
      return false;
    }
  }


  bool ImageService::TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const PixelFormat desiredPixelFormat)
  {
    // For now we just reuse the exception based one
    try
    {
      Write(absolutePath, bitmap, imageFormat, desiredPixelFormat);
      return true;
    }
    catch (const std::exception&)
    {
      return false;
    }
  }


  bool ImageService::TryWriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const PixelFormat desiredPixelFormat)
  {
    // For now we just reuse the exception based one
    try
    {
      WriteExactImage(absolutePath, bitmap, imageFormat, desiredPixelFormat);
      return true;
    }
    catch (const std::exception&)
    {
      return false;
    }
  }


  void ImageService::SetPreferredBitmapOrigin(const BitmapOrigin origin)
  {
    m_bitmapOrigin = origin;
  }


  void ImageService::DoWrite(const IO::Path& absPath, const Bitmap& bitmap, const ImageFormat imageFormat)
  {
    // If there is a image service available for the format
    const auto itrFind = m_formatToImageLibrary.find(imageFormat);
    if (itrFind != m_formatToImageLibrary.end() && itrFind->second->TryWrite(absPath, bitmap, imageFormat, true))
      return;

    // No such luck, so lets just try all the registered image service
    for (auto itr = m_imageLibraryServices.begin(); itr != m_imageLibraryServices.end(); ++itr)
    {
      if ((*itr)->TryWrite(absPath, bitmap, imageFormat, true))
        return;
    }

    // Still no luck, lets try the bmp fall back writer
    if (imageFormat == ImageFormat::Bmp)
    {
      // This only supports pixel format layout B8G8R8A8
      if (PixelFormatUtil::GetPixelFormatLayout(bitmap.GetPixelFormat()) == PixelFormatLayout::B8G8R8A8)
        BMPUtil::Save(absPath, bitmap);
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


  void ImageService::DoWriteExactImage(const IO::Path& absPath, const Bitmap& bitmap, const ImageFormat imageFormat)
  {
    assert(imageFormat != ImageFormat::Undefined);

    // If there is a image service available for the format
    const auto itrFind = m_formatToImageLibrary.find(imageFormat);
    if (itrFind != m_formatToImageLibrary.end() && itrFind->second->TryWrite(absPath, bitmap, imageFormat, true))
      return;

    // No such luck, so lets just try all the registered image service
    for (auto itr = m_imageLibraryServices.begin(); itr != m_imageLibraryServices.end(); ++itr)
    {
      if ((*itr)->TryWrite(absPath, bitmap, imageFormat, true))
        return;
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
