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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <cassert>
#include <cstring>
#include <cctype>

namespace Fsl
{
  namespace ImageFormatUtil
  {
    namespace
    {
      inline ImageFormat TryDetectImageFormat(const char* const pSrc, const std::size_t srcLength)
      {
        if (pSrc == nullptr || srcLength >= 5 || srcLength == 0)
        {
          return ImageFormat::Undefined;
        }

        static_assert('a' > 'A', "We expect ASCII");

        char ext[6]{};

        // This should be ok as we are working on ASCII chars
        for (std::size_t i = 0; i < srcLength; ++i)
        {
          char ch = pSrc[i];
          if (ch >= 'A' && ch <= 'Z')
          {
            ch += ('a' - 'A');
          }
          ext[i] = ch;
        }

        // Skip the leading '.' if it exists
        const char* pszExt = ext[0] == '.' ? ext + 1 : ext;

        if (strcmp(pszExt, "jpg") == 0 || strcmp(pszExt, "jpeg") == 0)
        {
          return ImageFormat::Jpeg;
        }
        if (strcmp(pszExt, "png") == 0)
        {
          return ImageFormat::Png;
        }
        if (strcmp(pszExt, "dds") == 0)
        {
          return ImageFormat::DDS;
        }
        if (strcmp(pszExt, "bmp") == 0)
        {
          return ImageFormat::Bmp;
        }
        if (strcmp(pszExt, "hdr") == 0)
        {
          return ImageFormat::Hdr;
        }
        if (strcmp(pszExt, "exr") == 0)
        {
          return ImageFormat::Exr;
        }
        if (strcmp(pszExt, "ktx") == 0)
        {
          return ImageFormat::KTX;
        }
        if (strcmp(pszExt, "tga") == 0)
        {
          return ImageFormat::Tga;
        }
        return ImageFormat::Undefined;
      }
    }

    ImageFormat TryDetectImageFormat(const std::string& extension)
    {
      const auto size = extension.size();
      return TryDetectImageFormat(extension.c_str(), size);
    }


    ImageFormat TryDetectImageFormat(const IO::Path& extension)
    {
      return TryDetectImageFormat(extension.ToAsciiString());
    }


    ImageFormat TryDetectImageFormatFromExtension(const IO::Path& path)
    {
      auto extension = IO::Path::GetExtension(path);
      if (extension.GetByteSize() <= 0)
      {
        return ImageFormat::Undefined;
      }

      return TryDetectImageFormat(extension.ToAsciiString());
    }


    const char* GetDefaultExtension(const ImageFormat imageFormat)
    {
      switch (imageFormat)
      {
      case ImageFormat::Bmp:
        return ".bmp";
      case ImageFormat::DDS:
        return ".dds";
      case ImageFormat::Exr:
        return ".exr";
      case ImageFormat::Hdr:
        return ".hdr";
      case ImageFormat::KTX:
        return ".ktx";
      case ImageFormat::Png:
        return ".png";
      case ImageFormat::Jpeg:
        return ".jpg";
      case ImageFormat::Tga:
        return ".tga";
      default:
        throw NotSupportedException("Unsupported image format");
      }
    }

    bool IsLossless(const ImageFormat imageFormat)
    {
      switch (imageFormat)
      {
      case ImageFormat::Bmp:
      case ImageFormat::DDS:
      case ImageFormat::Hdr:
      case ImageFormat::Exr:    // This is probably oversimplified as exr can be lossy
      case ImageFormat::KTX:
      case ImageFormat::Png:
      case ImageFormat::Tga:
        return true;
      case ImageFormat::Jpeg:
        return false;
      default:
        throw NotSupportedException("Unsupported image format");
      }
    }
  }
}
