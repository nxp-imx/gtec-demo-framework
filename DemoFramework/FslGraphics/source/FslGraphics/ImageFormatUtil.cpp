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

#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <array>
#include <cassert>
#include <cctype>
#include <cstring>

namespace Fsl::ImageFormatUtil
{
  namespace
  {
    inline ImageFormat TryDetectImageFormat(const StringViewLite& strView)
    {
      if (strView.size() >= 5u || strView.empty())
      {
        return ImageFormat::Undefined;
      }

      static_assert('a' > 'A', "We expect ASCII");

      std::array<char, 6> ext{};

      // This should be ok as we are working on ASCII chars
      for (std::size_t i = 0; i < strView.size(); ++i)
      {
        char ch = strView[i];
        if (ch >= 'A' && ch <= 'Z')
        {
          ch += ('a' - 'A');
        }
        ext[i] = ch;
      }

      // Skip the leading '.' if it exists
      const StringViewLite strExtView = ext[0] == '.' ? strView.substr(1) : strView;

      if (strExtView == "jpg" || strExtView == "jpeg")
      {
        return ImageFormat::Jpeg;
      }
      if (strExtView == "png")
      {
        return ImageFormat::Png;
      }
      if (strExtView == "dds")
      {
        return ImageFormat::DDS;
      }
      if (strExtView == "bmp")
      {
        return ImageFormat::Bmp;
      }
      if (strExtView == "hdr")
      {
        return ImageFormat::Hdr;
      }
      if (strExtView == "exr")
      {
        return ImageFormat::Exr;
      }
      if (strExtView == "ktx")
      {
        return ImageFormat::KTX;
      }
      if (strExtView == "tga")
      {
        return ImageFormat::Tga;
      }
      return ImageFormat::Undefined;
    }
  }

  ImageFormat TryDetectImageFormat(const std::string& extension)
  {
    return TryDetectImageFormat(StringViewLiteUtil::AsStringViewLite(extension));
  }


  ImageFormat TryDetectImageFormat(const IO::Path& extension)
  {
    return TryDetectImageFormat(extension.AsStringViewLite());
  }


  ImageFormat TryDetectImageFormatFromExtension(const IO::Path& path)
  {
    auto extension = IO::Path::GetExtensionView(path.AsPathView());
    if (extension.empty())
    {
      return ImageFormat::Undefined;
    }

    return TryDetectImageFormat(extension);
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
