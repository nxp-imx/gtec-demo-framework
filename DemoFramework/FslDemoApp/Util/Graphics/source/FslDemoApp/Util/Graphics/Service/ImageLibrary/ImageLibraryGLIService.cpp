#ifdef FSL_FEATURE_GLI
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

#include <FslDemoApp/Base/Service/ImageLibrary/IImageLibraryService.hpp>
#include <FslDemoApp/Util/Graphics/Service/ImageLibrary/ImageLibraryGLIService.hpp>
#include <FslBase/BlobRecord.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <FslGraphics/IO/BMPUtil.hpp>
#include <FslGraphics/PixelFormatLayoutUtil.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/TextureBlobBuilder.hpp>
#include "GLIConversionHelper.hpp"

//#include <gli/convert.hpp>

//#include <gli/convert.hpp>
#include <gli/load.hpp>

namespace Fsl
{
  namespace
  {
    bool TrySwizzle(gli::texture& tex, const PixelFormatLayout srcFormatLayout, const PixelFormatLayout dstFormatLayout)
    {
      FSL_PARAM_NOT_USED(tex);
      FSL_PARAM_NOT_USED(srcFormatLayout);
      FSL_PARAM_NOT_USED(dstFormatLayout);
      // switch (dstFormatLayout)
      //{
      // case PixelFormatLayout::R8G8B8:
      //{
      //  //tex.swizzle(gli::swizzles(gli::SWIZZLE_RED, gli::SWIZZLE_GREEN, gli::SWIZZLE_BLUE, gli::SWIZZLE_ALPHA));
      //}
      // case PixelFormatLayout::B8G8R8:
      //  break;
      // case PixelFormatLayout::R8G8B8A8:
      //  break;
      // case PixelFormatLayout::B8G8R8A8:
      //  break;
      //}
      return false;
    }


    bool TryConvert(gli::texture& texture, const PixelFormat srcFormat, const PixelFormat dstFormat)
    {
      {    // TODO: try to re-enable this once we upgrade GLI
           // gli::format gliDstFormat = GLIConversionHelper::TryConvert(dstFormat);
           // if ( gliDstFormat == gli::FORMAT_UNDEFINED )
           //  return false;

        // gli::convert(texture, gliDstFormat);
        // return true;
      }

      const auto srcFormatLayout = PixelFormatUtil::GetPixelFormatLayout(srcFormat);
      const auto dstFormatLayout = PixelFormatUtil::GetPixelFormatLayout(dstFormat);

      if (PixelFormatLayoutUtil::IsSwizzleCompatible(srcFormatLayout, dstFormatLayout))
      {
        return TrySwizzle(texture, srcFormatLayout, dstFormatLayout);
      }
      return false;
    }
  }


  ImageLibraryGLIService::ImageLibraryGLIService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  ImageLibraryGLIService::~ImageLibraryGLIService() = default;


  std::string ImageLibraryGLIService::GetName() const
  {
    return std::string("-ImageLibraryGLIService-");
  }


  void ImageLibraryGLIService::ExtractSupportedImageFormats(std::deque<ImageFormat>& rFormats)
  {
    rFormats.push_back(ImageFormat::KTX);
    rFormats.push_back(ImageFormat::DDS);
  }


  bool ImageLibraryGLIService::TryRead(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat pixelFormatHint,
                                       const BitmapOrigin originHint, const PixelChannelOrder preferredChannelOrderHint)
  {
    FSL_PARAM_NOT_USED(pixelFormatHint);
    FSL_PARAM_NOT_USED(preferredChannelOrderHint);
    FSL_PARAM_NOT_USED(originHint);
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      return false;
    }

    gli::texture tex = gli::load(absolutePath.ToUTF8String());
    if (tex.empty())
    {
      return false;
    }

    // Bitmaps can only contain 1d and 2d data
    if (tex.target() != gli::TARGET_1D && tex.target() != gli::TARGET_2D)
    {
      return false;
    }

    // Convert the gli pixel-format to something we understand
    const auto pixelFormat = GLIConversionHelper::TryConvert(tex.format());
    if (pixelFormat == PixelFormat::Undefined)
    {
      return false;
    }

    // Bitmaps can only hold uncompressed data
    if (PixelFormatUtil::IsCompressed(pixelFormat))
    {
      return false;
    }

    // Just load the 'largest' image if multiple mips are available
    const auto cbTexL0 = tex.size(0);
    const auto extentL0 = tex.extent(0);
    const auto strideL0 = cbTexL0 / tex.extent().y;
    if (strideL0 > std::numeric_limits<uint32_t>::max())
    {
      throw UnsupportedStrideExceptionEx("Stride is limited to a uint32", strideL0);
    }

    try
    {
      // NOTE: gli does not seem to have a origin concept so we have no way of knowing the 'origin' of the content
      rBitmap.Reset(tex.data(), cbTexL0, Extent2D(extentL0.x, extentL0.y), pixelFormat, static_cast<uint32_t>(strideL0), BitmapOrigin::UpperLeft);
    }
    catch (const std::exception&)
    {
      return false;
    }

    // TODO: implement conversion once we update to a 'gli' version where it works
    // NOTE: the pixelFormatHint can be set to undefined, meaning use the source pixel format
    return true;
  }


  bool ImageLibraryGLIService::TryRead(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat pixelFormatHint,
                                       const BitmapOrigin originHint, const PixelChannelOrder preferredChannelOrderHint)
  {
    FSL_PARAM_NOT_USED(originHint);
    FSL_PARAM_NOT_USED(preferredChannelOrderHint);
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      return false;
    }

    gli::texture tex = gli::load(absolutePath.ToUTF8String());
    if (tex.empty())
    {
      return false;
    }

    // Convert the gli pixel-format to something we understand
    const auto pixelFormat = GLIConversionHelper::TryConvert(tex.format());
    if (pixelFormat == PixelFormat::Undefined)
    {
      return false;
    }

    // Convert the target to texture type
    const auto textureType = GLIConversionHelper::TryConvert(tex.target());
    if (textureType == TextureType::Undefined)
    {
      return false;
    }

    // We assume that all images have a origin of UpperLeft for now
    const BitmapOrigin currentOrigin = BitmapOrigin::UpperLeft;

    try
    {
      if (!PixelFormatUtil::IsCompressed(pixelFormat) && pixelFormatHint != PixelFormat::Undefined && pixelFormat != pixelFormatHint)
      {
        // we ignore the return code for now as its not a critical error if the swizzle fails
        TryConvert(tex, pixelFormat, pixelFormatHint);
      }

      const auto gliExtent = tex.extent();
      Extent3D extent(gliExtent.x, gliExtent.y, gliExtent.z);

      if (tex.faces() > std::numeric_limits<uint32_t>::max())
      {
        FSLLOG3_DEBUG_WARNING("Face count exceeded uint32_t capacity");
        return false;
      }

      if (tex.max_level() > (std::numeric_limits<uint32_t>::max() - 1))
      {
        FSLLOG3_DEBUG_WARNING("levels count exceeded uint32_t capacity");
        return false;
      }
      if (tex.max_layer() > (std::numeric_limits<uint32_t>::max() - 1))
      {
        FSLLOG3_DEBUG_WARNING("layers count exceeded uint32_t capacity");
        return false;
      }

      const auto faces = static_cast<uint32_t>(tex.faces());
      const auto levels = static_cast<uint32_t>(tex.max_level() + 1);
      const auto layers = static_cast<uint32_t>(tex.max_layer() + 1);

      const TextureInfo textureInfo(levels, faces, layers);
      TextureBlobBuilder blobBuilder(textureType, extent, pixelFormat, textureInfo, currentOrigin, tex.size());
      for (uint32_t level = 0; level < levels; ++level)
      {
        const auto gliLevelExtent = tex.extent(level);
        Extent3D levelExtent(gliLevelExtent.x, gliLevelExtent.y, gliLevelExtent.z);
        if (levelExtent != blobBuilder.GetExtent(level))
        {
          FSLLOG3_DEBUG_WARNING("The blobBuilder and GLI did not agree on the extent size for level: {}", level);
          return false;
        }
        const auto levelSize = tex.size(level);
        for (uint32_t layer = 0; layer < layers; ++layer)
        {
          for (uint32_t face = 0; face < faces; ++face)
          {
            const std::ptrdiff_t offsetPtrDiff = static_cast<const uint8_t*>(tex.data(layer, face, level)) - static_cast<const uint8_t*>(tex.data());
            assert(offsetPtrDiff >= 0);
            assert(static_cast<std::size_t>(offsetPtrDiff) <= std::numeric_limits<uint32_t>::max());
            blobBuilder.SetBlob(BlobRecord(static_cast<uint32_t>(offsetPtrDiff), levelSize), level, face, layer);
          }
        }
      }
      rTexture.Reset(tex.data(), tex.size(), std::move(blobBuilder));
    }
    catch (const std::exception&)
    {
      return false;
    }
    return true;
  }


  bool ImageLibraryGLIService::TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat, const bool allowOverwrite)
  {
    FSL_PARAM_NOT_USED(bitmap);
    FSL_PARAM_NOT_USED(imageFormat);
    FSL_PARAM_NOT_USED(allowOverwrite);
    if (!IO::Path::IsPathRooted(absolutePath))
    {
      return false;
    }

    return false;
  }
}
#endif
