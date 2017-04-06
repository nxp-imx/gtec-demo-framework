#ifdef FSL_ENABLE_DEVIL
/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Noncopyable.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/IO/BMPUtil.hpp>
#include <FslDemoPlatform/Service/ImageLibrary/ImageLibraryServiceDevIL.hpp>
#include <cassert>
#include <sstream>
#include <IL/il.h>

namespace Fsl
{
  namespace
  {
    struct DevILPixelFormat
    {
      ILint Format;
      ILint Type;

      DevILPixelFormat()
        : Format(0)
        , Type(0)
      {
      }

      DevILPixelFormat(const ILint format, const ILint type)
        : Format(format)
        , Type(type)
      {
      }


      bool operator==(const DevILPixelFormat& rhs) const
      {
        return Format == rhs.Format && Type == rhs.Type;
      }


      bool operator!=(const DevILPixelFormat& rhs) const
      {
        return !(*this == rhs);
      }
    };


    ILenum Convert(BitmapOrigin& rOriginHint)
    {
      switch (rOriginHint)
      {
      case BitmapOrigin::UpperLeft:
        return IL_ORIGIN_UPPER_LEFT;
      case BitmapOrigin::LowerLeft:
        return IL_ORIGIN_LOWER_LEFT;
      default:
        FSLLOG_WARNING("Unsupported origin " << (int32_t)rOriginHint);
        rOriginHint = BitmapOrigin::UpperLeft;
        return IL_ORIGIN_UPPER_LEFT;
      }
    }


    bool TryConvert(const DevILPixelFormat& format, PixelFormat& rPixelFormat)
    {
      switch (format.Format)
      {
      case IL_RGB:
        if (format.Type == IL_UNSIGNED_BYTE)
        {
          rPixelFormat = PixelFormat::R8G8B8_UINT;
          return true;
        }
        break;
      case IL_BGR:
        if (format.Type == IL_UNSIGNED_BYTE)
        {
          rPixelFormat = PixelFormat::B8G8R8_UINT;
          return true;
        }
        break;
      case IL_RGBA:
        if (format.Type == IL_UNSIGNED_BYTE)
        {
          rPixelFormat = PixelFormat::R8G8B8A8_UINT;
          return true;
        }
        else if (format.Type == IL_FLOAT)
        {
          rPixelFormat = PixelFormat::R32G32B32A32_SFLOAT;
          return true;
        }
        else if (format.Type == IL_HALF)
        {
          rPixelFormat = PixelFormat::R16G16B16A16_SFLOAT;
          return true;
        }
        break;
      case IL_BGRA:
        if (format.Type == IL_UNSIGNED_BYTE)
        {
          rPixelFormat = PixelFormat::B8G8R8A8_UINT;
          return true;
        }
        break;
      case IL_ALPHA:
        if (format.Type == IL_UNSIGNED_BYTE)
        {
          rPixelFormat = PixelFormat::EX_ALPHA8_UNORM;
          return true;
        }
        break;
      case IL_LUMINANCE:
        if (format.Type == IL_UNSIGNED_BYTE)
        {
          rPixelFormat = PixelFormat::EX_LUMINANCE8_UNORM;
          return true;
        }
        break;
      case IL_LUMINANCE_ALPHA:
        if (format.Type == IL_UNSIGNED_BYTE)
        {
          rPixelFormat = PixelFormat::EX_LUMINANCE8_ALPHA8_UNORM;
          return true;
        }
        break;
      default:
        break;
      }
      return false;
    }


    PixelFormat Convert(const DevILPixelFormat& format)
    {
      PixelFormat value;
      if (TryConvert(format, value))
        return value;
      throw GraphicsException("Unsupported image format");
    }


    DevILPixelFormat Convert(const PixelFormat format)
    {
      // Handle the special formats first
      switch (format)
      {
      case PixelFormat::EX_ALPHA8_UNORM:
        return DevILPixelFormat(IL_ALPHA, IL_UNSIGNED_BYTE);
      case PixelFormat::EX_LUMINANCE8_UNORM:
        return DevILPixelFormat(IL_LUMINANCE, IL_UNSIGNED_BYTE);
      case PixelFormat::EX_LUMINANCE8_ALPHA8_UNORM:
        return DevILPixelFormat(IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);
      default:
        break;
      }

      switch (PixelFormatUtil::GetPixelFormatLayout(format))
      {
      case PixelFormatLayout::R8G8B8:
        return DevILPixelFormat(IL_RGB, IL_UNSIGNED_BYTE);
      case PixelFormatLayout::B8G8R8:
        return DevILPixelFormat(IL_BGR, IL_UNSIGNED_BYTE);
      case PixelFormatLayout::R8G8B8A8:
        return DevILPixelFormat(IL_RGBA, IL_UNSIGNED_BYTE);
      case PixelFormatLayout::B8G8R8A8:
        return DevILPixelFormat(IL_BGRA, IL_UNSIGNED_BYTE);
      case PixelFormatLayout::R32G32B32A32:
        return DevILPixelFormat(IL_RGBA, IL_FLOAT);
      case PixelFormatLayout::R16G16B16A16:
        return DevILPixelFormat(IL_RGBA, IL_HALF);
      default:
        throw UnsupportedPixelFormatException(format);
      }
    }

    struct ScopedDevILImage : private Noncopyable
    {
      ILuint Id;

      ScopedDevILImage()
      {
        ilGenImages(1, &Id);
        if (ilGetError() != IL_NO_ERROR)
          throw GraphicsException("Failed to create DevIL image");
      }
      ~ScopedDevILImage()
      {
        ilDeleteImages(1, &Id);
      }
    };


    bool LoadILImage(Bitmap& rBitmap, const IO::Path& path, const DevILPixelFormat& pixelFormatHint, const bool convertFormat, BitmapOrigin bitmapOrigin)
    {
      // Check if we should try a format conversion or not
      // the conversion is just a hint and if we don't support it we should just not do any conversion
      bool doFormatConversion = convertFormat;
      if (doFormatConversion)
      {
        PixelFormat tmpFormat;
        if (!TryConvert(pixelFormatHint, tmpFormat))
          doFormatConversion = false;
      }

      ScopedDevILImage image;

      ilBindImage(image.Id);
      // FIX: investigate if DevIL supports UTF8 on all platforms.
      // Since the ToAsciiString() conversion breaks UTF8 support.
      // Its quite likely that UTF8 works on linux based platforms,
      // but windows might require a different solution.

#ifdef _WIN32
      // Loads into the current bound image
      ilLoadImage(path.ToWString().c_str());
#else
      ilLoadImage(path.ToAsciiString().c_str());
#endif

      ILuint devilError = ilGetError();
      if (devilError != IL_NO_ERROR)
      {
        FSLLOG_WARNING("devIL image loading not successfull.\n");
        return false;
      }

      //int intFormatMode = ilGetInteger(IL_FORMAT_MODE);
      //int bpp = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
      //int bytesPP = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

      int width = ilGetInteger(IL_IMAGE_WIDTH);
      int height = ilGetInteger(IL_IMAGE_HEIGHT);
      const int imageFormat = ilGetInteger(IL_IMAGE_FORMAT);
      const int imageType = ilGetInteger(IL_IMAGE_TYPE);
      DevILPixelFormat currentPixelFormat(imageFormat, imageType);

      if (doFormatConversion && currentPixelFormat != pixelFormatHint)
      {
        // The ilCopyPixels crash when using IL_ALPHA this prevents that
        if ( pixelFormatHint.Format != IL_ALPHA )
          currentPixelFormat = pixelFormatHint;
      }


      // Extract the image.
      {
        // TODO: Remove the temporary content array by copying it into the bitmap directly instead
        const PixelFormat activePixelFormat = Convert(currentPixelFormat);
        const DevILPixelFormat activeImageFormat = Convert(activePixelFormat);
        const int bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(activePixelFormat);

        // WARNING: IL_ALPHA conversions appear to be broken in devil (as it crashes in ilCopyPixels)

        std::vector<uint8_t> content(width * height * bytesPerPixel);
        ilCopyPixels(0, 0, 0, width, height, 1, activeImageFormat.Format, activeImageFormat.Type, content.data());
        rBitmap.Reset(content.data(), Extent2D(width, height), activePixelFormat, bitmapOrigin);
      }

      devilError = ilGetError();

      // Log any error that occurs
      FSLLOG_WARNING_IF(devilError != IL_NO_ERROR, "devIL image conversion not successfull (" << devilError << ").\n");
      return (devilError == IL_NO_ERROR);
    }
  }


  ImageLibraryServiceDevIL::ImageLibraryServiceDevIL(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_lastOrigin(BitmapOrigin::UpperLeft)
  {
    ILuint devilError = IL_NO_ERROR;
    ilInit();
    devilError = ilGetError();
    if (devilError != IL_NO_ERROR)
    {
      throw std::runtime_error("devIL lib init failed");
    }

    // Make sure the images are using the expected origin
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    ilEnable(IL_ORIGIN_SET);
  }


  ImageLibraryServiceDevIL::~ImageLibraryServiceDevIL()
  {
  }



  std::string ImageLibraryServiceDevIL::GetName() const
  {
    return std::string("ImageLibraryServiceDevIL");
  }


  void ImageLibraryServiceDevIL::ExtractSupportedImageFormats(std::deque<ImageFormat>& rFormats)
  {
    rFormats.push_back(ImageFormat::Bmp);
    rFormats.push_back(ImageFormat::Png);
    rFormats.push_back(ImageFormat::Jpeg);
  }


  bool ImageLibraryServiceDevIL::TryRead(Bitmap& rBitmap, const IO::Path& path, const PixelFormat pixelFormatHint, const BitmapOrigin originHint)
  {
    try
    {
      BitmapOrigin origin = originHint;
      if (originHint != m_lastOrigin)
      {
        m_lastOrigin = originHint;
        const ILenum devOrigin = Convert(origin);
        ilOriginFunc(devOrigin);
      }

      const bool convertFormat = pixelFormatHint != PixelFormat::Undefined;
      DevILPixelFormat format;
      if (convertFormat)
        format = Convert(pixelFormatHint);

      return LoadILImage(rBitmap, path, format, convertFormat, origin);
    }
    catch (std::exception&ex)
    {
      FSLLOG_WARNING("devIL image conversion not successfull (" << ex.what() << ").\n");
      return false;
    }
  }


  bool ImageLibraryServiceDevIL::TryRead(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat pixelFormatHint, const BitmapOrigin originHint)
  {
    return false;
  }


  bool ImageLibraryServiceDevIL::TryWrite(const IO::Path& path, const Bitmap& bitmap, const ImageFormat imageFormat, const bool allowOverwrite)
  {
    // TODO: implement DevIL save support
    return false;
  }

}
#endif
