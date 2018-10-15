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

#include <FslDemoHost/Base/Service/Image/ImageService.hpp>
#include <FslDemoApp/Base/Service/ImageBasic/IImageBasicService.hpp>
#include <FslDemoApp/Base/Service/AsyncImage/IAsyncImageService.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
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
  // Basically just provides the original image loading capabilities.
  // This includes the ability to load a image using the 'default' origin set by the framework.
  ImageService::ImageService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_imageBasic(serviceProvider.TryGet<IImageBasicService>())
    , m_asyncImage(serviceProvider.TryGet<IAsyncImageService>())
    , m_bitmapOrigin(BitmapOrigin::UpperLeft)
  {
    if (!m_imageBasic && !m_asyncImage)
    {
      throw NotSupportedException("ImageService needs IImageBasicService or IAsyncImageService to be available");
    }
  }


  ImageService::~ImageService() = default;


  BitmapOrigin ImageService::GetPreferredBitmapOrigin() const
  {
    return m_bitmapOrigin;
  }


  void ImageService::Read(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                          const PixelChannelOrder preferredChannelOrder) const
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      throw std::invalid_argument(std::string("Read only takes absolute paths: ") + absolutePath.ToUTF8String());
    }

    // if (!IO::File::Exists(absolutePath))
    //  throw std::invalid_argument(std::string("File not found: ") + absolutePath.ToUTF8String());

    const auto usedOrigin = (desiredOrigin != BitmapOrigin::Undefined ? desiredOrigin : m_bitmapOrigin);

    if (m_imageBasic)
    {
      m_imageBasic->Read(rBitmap, absolutePath, desiredPixelFormat, usedOrigin, preferredChannelOrder);
    }
    else
    {
      rBitmap = m_asyncImage->ReadBitmap(absolutePath, desiredPixelFormat, usedOrigin, preferredChannelOrder).get();
    }
  }


  void ImageService::Read(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                          const PixelChannelOrder preferredChannelOrder) const
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      throw std::invalid_argument(std::string("Read only takes absolute paths: ") + absolutePath.ToUTF8String());
    }

    // if (!IO::File::Exists(absolutePath))
    //  throw std::invalid_argument(std::string("File not found: ") + absolutePath.ToUTF8String());

    const auto usedOrigin = (desiredOrigin != BitmapOrigin::Undefined ? desiredOrigin : m_bitmapOrigin);

    if (m_imageBasic)
    {
      m_imageBasic->Read(rTexture, absolutePath, desiredPixelFormat, usedOrigin, preferredChannelOrder);
    }
    else
    {
      rTexture = m_asyncImage->ReadTexture(absolutePath, desiredPixelFormat, usedOrigin, preferredChannelOrder).get();
    }
  }


  void ImageService::Write(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      throw std::invalid_argument(std::string("Read only takes absolute paths: ") + absolutePath.ToUTF8String());
    }

    if (m_imageBasic)
    {
      m_imageBasic->Write(absolutePath, bitmap, imageFormat, desiredPixelFormat);
    }
    else
    {
      m_asyncImage->Write(absolutePath, bitmap, imageFormat, desiredPixelFormat).get();
    }
  }


  void ImageService::WriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                     const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      throw std::invalid_argument(std::string("Read only takes absolute paths: ") + absolutePath.ToUTF8String());
    }

    if (m_imageBasic)
    {
      m_imageBasic->WriteExactImage(absolutePath, bitmap, imageFormat, desiredPixelFormat);
    }
    else
    {
      m_asyncImage->WriteExactImage(absolutePath, bitmap, imageFormat, desiredPixelFormat).get();
    }
  }


  bool ImageService::TryRead(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                             const PixelChannelOrder preferredChannelOrder) const
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      FSLLOG_DEBUG_WARNING("TryRead only takes absolute paths: " << absolutePath);
      return false;
    }

    if (!IO::File::Exists(absolutePath))
    {
      FSLLOG_DEBUG_WARNING("File not found: " << absolutePath);
      return false;
    }

    const auto usedOrigin = (desiredOrigin != BitmapOrigin::Undefined ? desiredOrigin : m_bitmapOrigin);

    try
    {
      if (m_imageBasic)
      {
        return m_imageBasic->TryRead(rBitmap, absolutePath, desiredPixelFormat, usedOrigin, preferredChannelOrder);
      }


      auto res = m_asyncImage->TryRead(absolutePath, desiredPixelFormat, usedOrigin, preferredChannelOrder).get();
      if (res.first)
      {
        rBitmap = std::move(res.second);
      }
      return res.first;
    }
    catch (const std::exception& ex)
    {
      FSL_PARAM_NOT_USED(ex);
      FSLLOG_DEBUG_WARNING("TryRead failed with: " << ex.what());
      return false;
    }
  }


  bool ImageService::TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      FSLLOG_DEBUG_WARNING("TryWrite only takes absolute paths: " << absolutePath);
      return false;
    }

    try
    {
      if (m_imageBasic)
      {
        return m_imageBasic->TryWrite(absolutePath, bitmap, imageFormat, desiredPixelFormat);
      }

      return m_asyncImage->TryWrite(absolutePath, bitmap, imageFormat, desiredPixelFormat).get();
    }
    catch (const std::exception& ex)
    {
      FSL_PARAM_NOT_USED(ex);
      FSLLOG_DEBUG_WARNING("TryWrite failed with: " << ex.what());
      return false;
    }
  }


  bool ImageService::TryWriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                        const PixelFormat desiredPixelFormat)
  {
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      FSLLOG_DEBUG_WARNING("TryWriteExactImage only takes absolute paths: " << absolutePath);
      return false;
    }

    try
    {
      if (m_imageBasic)
      {
        return m_imageBasic->TryWriteExactImage(absolutePath, bitmap, imageFormat, desiredPixelFormat);
      }

      return m_asyncImage->TryWriteExactImage(absolutePath, bitmap, imageFormat, desiredPixelFormat).get();
    }
    catch (const std::exception& ex)
    {
      FSL_PARAM_NOT_USED(ex);
      FSLLOG_DEBUG_WARNING("TryWriteExactImage failed with: " << ex.what());
      return false;
    }
  }


  void ImageService::SetPreferredBitmapOrigin(const BitmapOrigin origin)
  {
    m_bitmapOrigin = origin;
  }
}
