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

#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Bits/ByteArrayUtil.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/System/Platform/PlatformPathTransform.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/IO/BMPUtil.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <fmt/format.h>
#include <array>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <limits>

// BMP format explained:
// http://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
// http://www.fileformat.info/format/bmp/egff.htm

namespace Fsl
{
  namespace
  {
    // These would be called BI_RGB etc when searching the web for them
    enum class BmpCompression
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      RGB = 0,
      // NOLINTNEXTLINE(readability-identifier-naming)
      RLE8 = 1,
      // NOLINTNEXTLINE(readability-identifier-naming)
      RLE4 = 2,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BITFIELDS = 3,
      // NOLINTNEXTLINE(readability-identifier-naming)
      JPEG = 4,
      // NOLINTNEXTLINE(readability-identifier-naming)
      PNG = 5,
      // NOLINTNEXTLINE(readability-identifier-naming)
      ALPHABITFIELDS = 6,
      // NOLINTNEXTLINE(readability-identifier-naming)
      CMYK = 11,
      // NOLINTNEXTLINE(readability-identifier-naming)
      CMYKRLE8 = 12,
      // NOLINTNEXTLINE(readability-identifier-naming)
      CMYKTLE4 = 13
    };


    struct BMPFileHeader
    {
      uint16_t FileType{0};
      // This field only stores a useful value when the bitmap data is compressed, and this value is usually zero in uncompressed BMP files.
      uint32_t FileSize{0};
      uint16_t Reserved1{0};
      uint16_t Reserved2{0};
      // BitmapOffset is the starting offset of the bitmap data from the beginning of the file in bytes.
      uint32_t BitmapOffset{0};

      BMPFileHeader() = default;
    };


    struct BMPBitmapV3InfoHeader
    {
      uint32_t DIBHeaderSize{0};
      int32_t ImageWidth{0};
      int32_t ImageHeight{0};
      uint16_t Planes{0};
      uint16_t BitsPerPixel{0};
      uint32_t Compression{0};
      uint32_t ImageSize{0};
      int32_t XPixelsPerMeter{0};
      int32_t YPixelsPerMeter{0};
      uint32_t ColorsInColorTable{0};
      uint32_t ImportantColorCount{0};
      uint32_t RedMask{0};
      uint32_t GreenMask{0};
      uint32_t BlueMask{0};
      uint32_t AlphaMask{0};

      BMPBitmapV3InfoHeader() = default;
    };

    constexpr int32_t PixelsPerMeterDefault = 100;    // ? magic number

    // The max bitmap size if 512MB
    constexpr uint64_t MaxBitmapSize = 1024 * 1024 * 512;

    constexpr uint16_t SizeFileheader = 14;
    constexpr uint16_t SizeBitmapinfoheader = 40;
    constexpr uint16_t SizeBitmapV2Infoheader = 52;
    constexpr uint16_t SizeBitmapV3Infoheader = 56;
    constexpr uint16_t SizeBitmapV4Header = 108;
    constexpr uint16_t SizeBitmapV5Header = 124;

    constexpr uint16_t MaxBitmapheaderSize = SizeBitmapV5Header;


    constexpr int32_t FileheaderOffsetFileType = 0;
    constexpr int32_t FileheaderOffsetFileSize = 2;
    constexpr int32_t FileheaderOffsetReserved1 = 6;
    constexpr int32_t FileheaderOffsetReserved2 = 8;
    constexpr int32_t FileheaderOffsetBitmapOffset = 10;

    constexpr int32_t CbBitmapheaderSizeField = 4;


    // size of header in bytes
    constexpr int32_t BitmapinfoheaderOffsetSize = 0;
    // image width in pixels
    // Width does not include any scan - line boundary padding.
    constexpr int32_t BitmapinfoheaderOffsetImageWidth = 4;
    // image height in pixels
    // If Height is a positive number, then the image is a "bottom-up" bitmap with the origin in the lower - left corner.
    // If Height is a negative number, then the image is a "top-down" bitmap with the origin in the upper - left corner.
    constexpr int32_t BitmapinfoheaderOffsetImageHeight = 8;
    // the number of color planes
    constexpr int32_t BitmapinfoheaderOffsetPlanes = 12;
    // Bits per pixel
    constexpr int32_t BitmapinfoheaderOffsetBitsPerPixel = 14;
    // Compression
    constexpr int32_t BitmapinfoheaderOffsetCompression = 16;
    // Size of bitmap (can be zero for uncompressed bitmaps)
    constexpr int32_t BitmapinfoheaderOffsetImageSize = 20;
    // Horizontal resolution in pixels per meter
    constexpr int32_t BitmapinfoheaderOffsetXPixelsPerMeter = 24;
    // Vertical resolution in pixels per meter
    constexpr int32_t BitmapinfoheaderOffsetYPixelsPerMeter = 28;
    // number of colors in the image
    constexpr int32_t BitmapinfoheaderOffsetColorsInColorTable = 32;
    // minimum number of important colors
    constexpr int32_t BitmapinfoheaderOffsetImportantColorCount = 36;
    // Red mask
    constexpr int32_t BitmapV2InfoheaderOffsetRedMask = 40;
    // Green mask
    constexpr int32_t BitmapV2InfoheaderOffsetGreenMask = 44;
    // Blue mask
    constexpr int32_t BitmapV2InfoheaderOffsetBlueMask = 48;

    // Alpha mask
    constexpr int32_t BitmapV3InfoheaderOffsetAlphaMask = 52;


    struct BitmapHeader
    {
      int32_t ImageWidth;
      int32_t ImageHeight;
      uint16_t BitsPerPixel;
      BmpCompression Compression;
      uint32_t ImageSize;
      uint32_t MaskR;
      uint32_t MaskG;
      uint32_t MaskB;
      uint32_t MaskA;
      bool IsYFlipped;
      bool Swizzle{false};
      int8_t SwizzleR{0};
      int8_t SwizzleG{1};
      int8_t SwizzleB{2};
      int8_t SwizzleA{3};
      PixelFormat ActivePixelFormat{PixelFormat::Undefined};

      explicit BitmapHeader(const BMPBitmapV3InfoHeader& header, BmpCompression compression)
        : ImageWidth(header.ImageWidth)
        , ImageHeight(std::abs(header.ImageHeight))
        , BitsPerPixel(header.BitsPerPixel)
        , Compression(compression)
        , ImageSize(header.ImageSize)
        , MaskR(header.RedMask)
        , MaskG(header.GreenMask)
        , MaskB(header.BlueMask)
        , MaskA(header.AlphaMask)
        , IsYFlipped(header.ImageHeight > 0)

      {
        if (header.ImageWidth < 0)
        {
          throw FormatException("Negative width bitmaps not supported");
        }

        if (header.Planes != 1)
        {
          throw FormatException("Bitmap did not contain the expected amount of planes");
        }

        // Check for mask overlaps
        if ((header.RedMask & header.GreenMask) != 0 || (header.RedMask & header.BlueMask) != 0 || (header.RedMask & header.AlphaMask) != 0 ||
            (header.GreenMask & header.BlueMask) != 0 || (header.GreenMask & header.AlphaMask) != 0 || (header.BlueMask & header.AlphaMask) != 0)
        {
          throw FormatException("The bitmap uses a unsupported mask");
        }

        // Quick resolution

        const bool isB8G8R8 = header.RedMask == 0x00FF0000 && header.GreenMask == 0x0000FF00 && header.BlueMask == 0x000000FF;
        const bool isR8G8B8 = header.RedMask == 0x000000FF && header.GreenMask == 0x0000FF00 && header.BlueMask == 0x00FF0000;
        if (!isR8G8B8 && !isB8G8R8)
        {
          const int shiftR = BitsUtil::IndexOf(header.RedMask);
          const int shiftG = BitsUtil::IndexOf(header.GreenMask);
          const int shiftB = BitsUtil::IndexOf(header.BlueMask);
          if (shiftR < 0 || shiftG < 0 || shiftB < 0)
          {
            throw FormatException("The bitmap uses a unsupported mask");
          }

          if ((header.RedMask >> shiftR) != 0xFF || (header.GreenMask >> shiftG) != 0xFF || (header.BlueMask >> shiftB) != 0xFF)
          {
            throw FormatException("The bitmap uses a unsupported mask");
          }

          // We only support masks that are shifts of 0, 8, 16, 24
          if ((shiftR & 7) != 0 || (shiftG & 7) != 0 || (shiftB & 7) != 0)
          {
            throw FormatException("The bitmap uses a unsupported mask");
          }

          Swizzle = true;
          assert((shiftR / 8) <= std::numeric_limits<int8_t>::max());
          assert((shiftR / 8) >= std::numeric_limits<int8_t>::min());
          assert((shiftG / 8) <= std::numeric_limits<int8_t>::max());
          assert((shiftG / 8) >= std::numeric_limits<int8_t>::min());
          assert((shiftB / 8) <= std::numeric_limits<int8_t>::max());
          assert((shiftB / 8) >= std::numeric_limits<int8_t>::min());
          SwizzleR = static_cast<int8_t>(shiftR / 8);
          SwizzleG = static_cast<int8_t>(shiftG / 8);
          SwizzleB = static_cast<int8_t>(shiftB / 8);
        }

        if (header.AlphaMask != 0xFF000000 && header.AlphaMask != 0x00000000)
        {
          const int shiftA = BitsUtil::IndexOf(header.AlphaMask);
          if (shiftA < 0 || (header.AlphaMask != 0 && (header.AlphaMask >> shiftA) != 0xFF))
          {
            throw FormatException("The bitmap uses a unsupported alpha mask");
          }

          // We only support masks that are shifts of 0, 8, 16, 24
          if ((shiftA & 7) != 0)
          {
            throw FormatException("The bitmap uses a unsupported alpha mask");
          }

          assert(Swizzle);
          assert((shiftA / 8) <= std::numeric_limits<int8_t>::max());
          assert((shiftA / 8) >= std::numeric_limits<int8_t>::min());
          SwizzleA = static_cast<int8_t>(shiftA / 8);
        }

        if (header.AlphaMask == 0xFF000000)
        {
          if (isR8G8B8)    // NOLINT(bugprone-branch-clone)
          {
            ActivePixelFormat = PixelFormat::R8G8B8A8_UINT;
          }
          else if (isB8G8R8)
          {
            ActivePixelFormat = PixelFormat::B8G8R8A8_UINT;
          }
          else
          {
            ActivePixelFormat = PixelFormat::R8G8B8A8_UINT;
          }
        }
        else
        {
          if (isR8G8B8)    // NOLINT(bugprone-branch-clone)
          {
            ActivePixelFormat = PixelFormat::R8G8B8_UINT;
          }
          else if (isB8G8R8)
          {
            ActivePixelFormat = PixelFormat::B8G8R8_UINT;
          }
          else
          {
            ActivePixelFormat = PixelFormat::R8G8B8_UINT;
          }
        }
      }
    };


    BmpCompression IdentifyCompression(const uint32_t compression)
    {
      switch (compression)
      {
      case 0:
        return BmpCompression::RGB;
      case 1:
        return BmpCompression::RLE8;
      case 2:
        return BmpCompression::RLE4;
      case 3:
        return BmpCompression::BITFIELDS;
      case 4:
        return BmpCompression::JPEG;
      case 5:
        return BmpCompression::PNG;
      case 6:
        return BmpCompression::ALPHABITFIELDS;
      case 11:
        return BmpCompression::CMYK;
      case 12:
        return BmpCompression::CMYKRLE8;
      case 13:
        return BmpCompression::CMYKTLE4;
      default:
        throw FormatException("Unsupported compression type");
      }
    }


    void StreamRead(std::ifstream& rStream, void* const pDst, const std::size_t cbRead)
    {
      rStream.read(reinterpret_cast<char*>(pDst), NumericCast<std::streamsize>(cbRead));
      if (!rStream.good())
      {
        throw FormatException("Failed to read the expected amount of bytes");
      }
    }


    BMPFileHeader ReadFileHeader(std::ifstream& rStream)
    {
      std::array<uint8_t, SizeFileheader> fileheader{};
      // Try to read the file header
      StreamRead(rStream, fileheader.data(), SizeFileheader);

      BMPFileHeader header;
      header.FileType = ByteArrayUtil::ReadUInt16LE(fileheader.data(), fileheader.size(), FileheaderOffsetFileType);
      header.FileSize = ByteArrayUtil::ReadUInt32LE(fileheader.data(), fileheader.size(), FileheaderOffsetFileSize);
      header.Reserved1 = ByteArrayUtil::ReadUInt16LE(fileheader.data(), fileheader.size(), FileheaderOffsetReserved1);
      header.Reserved2 = ByteArrayUtil::ReadUInt16LE(fileheader.data(), fileheader.size(), FileheaderOffsetReserved2);
      header.BitmapOffset = ByteArrayUtil::ReadUInt16LE(fileheader.data(), fileheader.size(), FileheaderOffsetBitmapOffset);

      if (header.FileType != 0x4D42)
      {
        throw FormatException("Unsupported BMP file format");
      }
      return header;
    }


    BitmapHeader ReadBitmapHeader(std::ifstream& rStream)
    {
      std::array<uint8_t, MaxBitmapheaderSize> bitmapHeader{};

      // Read uint32 that describe the size of the bitmap header
      StreamRead(rStream, reinterpret_cast<char*>(bitmapHeader.data()), CbBitmapheaderSizeField);

      BMPBitmapV3InfoHeader header;
      header.DIBHeaderSize = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), 4, 0);

      // Check if its one of the common header sizes
      if (header.DIBHeaderSize != SizeBitmapinfoheader && header.DIBHeaderSize != SizeBitmapV2Infoheader &&
          header.DIBHeaderSize != SizeBitmapV3Infoheader && header.DIBHeaderSize != SizeBitmapV4Header && header.DIBHeaderSize != SizeBitmapV5Header)
      {
        throw FormatException("bitmap header not of the supported size");
      }

      assert(header.DIBHeaderSize <= MaxBitmapheaderSize);

      // Read the rest of the header
      StreamRead(rStream, bitmapHeader.data() + CbBitmapheaderSizeField, header.DIBHeaderSize - CbBitmapheaderSizeField);

      // Decode the standard BITMAPINFOHEADER
      header.ImageWidth = ByteArrayUtil::ReadInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetImageWidth);
      header.ImageHeight = ByteArrayUtil::ReadInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetImageHeight);
      header.Planes = ByteArrayUtil::ReadUInt16LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetPlanes);
      header.BitsPerPixel = ByteArrayUtil::ReadUInt16LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetBitsPerPixel);
      header.Compression = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetCompression);
      header.ImageSize = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetImageSize);
      header.XPixelsPerMeter = ByteArrayUtil::ReadInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetXPixelsPerMeter);
      header.YPixelsPerMeter = ByteArrayUtil::ReadInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetYPixelsPerMeter);
      header.ColorsInColorTable = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetColorsInColorTable);
      header.ImportantColorCount = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapinfoheaderOffsetImportantColorCount);

      const BmpCompression compression = IdentifyCompression(header.Compression);

      if (header.DIBHeaderSize >= SizeBitmapV2Infoheader && header.BitsPerPixel > 24)
      {
        // Read the BITMAPV2INFOHEADER extensions
        header.RedMask = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapV2InfoheaderOffsetRedMask);
        header.GreenMask = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapV2InfoheaderOffsetGreenMask);
        header.BlueMask = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapV2InfoheaderOffsetBlueMask);
      }
      else
      {
        header.RedMask = 0x00FF0000;
        header.GreenMask = 0x0000FF00;
        header.BlueMask = 0x000000FF;
      }

      if (header.DIBHeaderSize >= SizeBitmapV3Infoheader)
      {
        // Read the  BITMAPV3INFOHEADER extensions
        header.AlphaMask = ByteArrayUtil::ReadUInt32LE(bitmapHeader.data(), header.DIBHeaderSize, BitmapV3InfoheaderOffsetAlphaMask);
      }
      else
      {
        header.AlphaMask = (header.BitsPerPixel == 32 ? 0xFF000000 : 0);
      }

      return BitmapHeader(header, compression);
    }


    uint32_t CalcBitmapSize(const BitmapHeader& bitmapHeader, const uint32_t minimumStride, const uint32_t bytesPerPixel, const bool allowZero)
    {
      FSL_PARAM_NOT_USED(bytesPerPixel);
      const uint64_t calculatedSize = static_cast<uint64_t>(minimumStride) * static_cast<uint64_t>(bitmapHeader.ImageHeight);
      if (calculatedSize > MaxBitmapSize)
      {
        throw FormatException("The bitmap size was exceeded");
      }

      const auto expectedBitmapSize = static_cast<uint32_t>(calculatedSize);
      if (bitmapHeader.Compression != BmpCompression::RGB)
      {
        if (bitmapHeader.ImageSize != expectedBitmapSize && (!allowZero || bitmapHeader.ImageSize != 0u))
        {
          throw FormatException("The bitmap size is incorrect");
        }
      }
      return expectedBitmapSize;
    }


    void ReadBitmapContent(std::ifstream& rStream, Bitmap& rBitmap, const uint32_t cbBitmap, [[maybe_unused]] const uint32_t minStride,
                           const BitmapHeader& bitmapHeader, const BitmapOrigin originHint)
    {
      Bitmap::ScopedDirectReadWriteAccess scopedAccess(rBitmap);

      scopedAccess.AsRawBitmap().SetBitmapOrigin(!bitmapHeader.IsYFlipped ? BitmapOrigin::UpperLeft : BitmapOrigin::LowerLeft);

      assert(scopedAccess.AsRawBitmap().Stride() == minStride);
      if (cbBitmap > scopedAccess.AsRawBitmap().GetByteSize())
      {
        throw FormatException("Decode BMP failed due to unexpected buffer size");
      }

      // Since the raw bitmap is in the exact same format we can just read directly into it.
      StreamRead(rStream, scopedAccess.AsRawBitmap().Content(), cbBitmap);

      if (scopedAccess.AsRawBitmap().GetOrigin() != originHint)
      {
        RawBitmapUtil::FlipHorizontal(scopedAccess.AsRawBitmap());
      }
      if (bitmapHeader.Swizzle)
      {
        RawBitmapUtil::Swizzle(scopedAccess.AsRawBitmap(), bitmapHeader.SwizzleR, bitmapHeader.SwizzleG, bitmapHeader.SwizzleB,
                               bitmapHeader.SwizzleA);
      }
    }


    void DecodeBitmap24BPP(Bitmap& rBitmap, std::ifstream& rStream, const BitmapHeader& bitmapHeader, const BitmapOrigin originHint)
    {
      if (bitmapHeader.BitsPerPixel == 24 && bitmapHeader.Compression != BmpCompression::RGB)
      {
        throw FormatException("Bitmap 24bpp not using uncompressed compression");
      }

      // Uncompressed data is a series of values representing either color palette indices or actual RGB color values.
      // Pixels are packed into bytes and arranged as scan lines.Each scan line must end on a 4 - byte boundary,
      // so one, two, or three bytes of padding may follow each scan line.
      const uint32_t bytesPerPixel = 3;
      const auto minimumBmpStride =
        PixelFormatUtil::CalcMinimumStride(PxSize1D::Create(bitmapHeader.ImageWidth), bytesPerPixel, StrideRequirement::MinimumAlign32Bit);
      const auto cbBitmap = CalcBitmapSize(bitmapHeader, minimumBmpStride, bytesPerPixel, true);

      // Ensure that the bitmap can hold the image (dont modify because we fill eventual padding with content from the bmp)
      const PixelFormat pf = bitmapHeader.ActivePixelFormat;
      rBitmap.Reset(PxSize2D::Create(bitmapHeader.ImageWidth, bitmapHeader.ImageHeight), pf, minimumBmpStride, rBitmap.GetOrigin(),
                    BitmapClearMethod::DontModify);

      ReadBitmapContent(rStream, rBitmap, cbBitmap, minimumBmpStride, bitmapHeader, originHint);
    }


    void DecodeBitmap32BPP(Bitmap& rBitmap, std::ifstream& rStream, const BitmapHeader& bitmapHeader, const BitmapOrigin originHint)
    {
      if (bitmapHeader.BitsPerPixel == 32 && bitmapHeader.Compression != BmpCompression::BITFIELDS && bitmapHeader.Compression != BmpCompression::RGB)
      {
        throw FormatException("Bitmap 32bpp not using bit-fields compression");
      }

      const uint32_t bytesPerPixel = 4;
      const auto minStride = PixelFormatUtil::CalcMinimumStride(PxSize1D::Create(bitmapHeader.ImageWidth), bytesPerPixel);
      const auto cbBitmap = CalcBitmapSize(bitmapHeader, minStride, bytesPerPixel, false);

      // Ensure that the bitmap can hold the image (We use dont modify because there is no padding to clear and everything will be overwritten)
      const PixelFormat pf = bitmapHeader.ActivePixelFormat;
      rBitmap.Reset(PxSize2D::Create(bitmapHeader.ImageWidth, bitmapHeader.ImageHeight), pf, minStride, rBitmap.GetOrigin(),
                    BitmapClearMethod::DontModify);

      ReadBitmapContent(rStream, rBitmap, cbBitmap, minStride, bitmapHeader, originHint);
    }


    void DecodeBitmap(Bitmap& rBitmap, std::ifstream& rStream, const BitmapHeader& bitmapHeader, const BitmapOrigin originHint)
    {
      switch (bitmapHeader.BitsPerPixel)
      {
      case 24:
        DecodeBitmap24BPP(rBitmap, rStream, bitmapHeader, originHint);
        break;
      case 32:
        DecodeBitmap32BPP(rBitmap, rStream, bitmapHeader, originHint);
        break;
      default:
        throw FormatException("Bitmap does no contain 24 or 32 bits per pixel");
      }
    }


    void WriteBitmapInfoHeader(std::ofstream& stream, const ReadOnlyRawBitmap& bitmap, const uint32_t cbBitmap)
    {
      const auto width = NumericCast<int32_t>(bitmap.GetExtent().Width.Value);
      const auto height = NumericCast<int32_t>(bitmap.GetExtent().Height.Value);

      std::array<uint8_t, SizeBitmapV2Infoheader> bitmapHeader{};
      ByteArrayUtil::WriteUInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetSize, SizeBitmapinfoheader);
      ByteArrayUtil::WriteInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetImageWidth, width);
      ByteArrayUtil::WriteInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetImageHeight, -height);
      ByteArrayUtil::WriteUInt16LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetPlanes, 1);
      ByteArrayUtil::WriteUInt16LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetBitsPerPixel, 32);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetCompression,
                                   static_cast<uint32_t>(BmpCompression::RGB));
      ByteArrayUtil::WriteUInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetImageSize, cbBitmap);
      ByteArrayUtil::WriteInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetXPixelsPerMeter, PixelsPerMeterDefault);
      ByteArrayUtil::WriteInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetYPixelsPerMeter, PixelsPerMeterDefault);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetColorsInColorTable, 0);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapinfoheaderOffsetImportantColorCount, 0);

      ByteArrayUtil::WriteUInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapV2InfoheaderOffsetRedMask, 0x00FF0000);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapV2InfoheaderOffsetGreenMask, 0x0000FF00);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader.data(), bitmapHeader.size(), BitmapV2InfoheaderOffsetBlueMask, 0x000000FF);

      stream.write(reinterpret_cast<const char*>(bitmapHeader.data()), SizeBitmapV2Infoheader);
    }
  }


  void BMPUtil::Load(Bitmap& rBitmap, const IO::Path& strFilename)
  {
    Load(rBitmap, strFilename, BitmapOrigin::UpperLeft);
  }


  void BMPUtil::Load(Bitmap& rBitmap, const IO::Path& strFilename, const BitmapOrigin originHint)
  {
    std::ifstream file(PlatformPathTransform::ToSystemPath(strFilename), std::ios::in | std::ios::binary);
    if (file.good())
    {
      Load(rBitmap, file, originHint);
    }
    else
    {
      throw FormatException(fmt::format("File not found: '{}'", strFilename));
    }
  }


  void BMPUtil::Load(Bitmap& rBitmap, std::ifstream& rStream)
  {
    Load(rBitmap, rStream, BitmapOrigin::UpperLeft);
  }


  void BMPUtil::Load(Bitmap& rBitmap, std::ifstream& rStream, const BitmapOrigin originHint)
  {
    const BMPFileHeader fileHeader = ReadFileHeader(rStream);
    BitmapHeader bitmapHeader = ReadBitmapHeader(rStream);

    // Seek to the bitmap data area
    rStream.seekg(fileHeader.BitmapOffset, std::ios_base::beg);
    if (!rStream.good())
    {
      throw FormatException("Failed to seek to the bitmap data area");
    }

    // At his point we know we have a 24bit bgr or 32bit bgra bitmap
    DecodeBitmap(rBitmap, rStream, bitmapHeader, originHint);
  }


  bool BMPUtil::TryLoad(Bitmap& rBitmap, const IO::Path& strFilename, const BitmapOrigin originHint)
  {
    // Good enough for now
    try
    {
      Load(rBitmap, strFilename, originHint);
      return true;
    }
    catch (const std::exception&)
    {
      return false;
    }
  }


  void BMPUtil::Save(const IO::Path& strFilename, const Bitmap& bitmap)
  {
    const Bitmap::ScopedDirectReadAccess scopedAccess(bitmap);
    Save(strFilename, scopedAccess.AsRawBitmap());
  }


  void BMPUtil::Save(const IO::Path& strFilename, const ReadOnlyRawBitmap& bitmap)
  {
    std::ofstream file(PlatformPathTransform::ToSystemPath(strFilename), std::ios::out | std::ios::binary);
    Save(file, bitmap);
  }


  void BMPUtil::Save(std::ofstream& stream, const Bitmap& bitmap)
  {
    const Bitmap::ScopedDirectReadAccess scopedAccess(bitmap);
    Save(stream, scopedAccess.AsRawBitmap());
  }


  void BMPUtil::Save(std::ofstream& stream, const ReadOnlyRawBitmap& bitmap)
  {
    if (bitmap.GetPixelFormatLayout() != PixelFormatLayout::B8G8R8A8)
    {
      throw UnsupportedPixelFormatException(bitmap.GetPixelFormat());
    }

    const auto minimumStride = PixelFormatUtil::CalcMinimumStride(bitmap.Width(), bitmap.GetPixelFormat(), StrideRequirement::MinimumAlign32Bit);

    const uint32_t bitmapOffset = SizeFileheader + SizeBitmapV2Infoheader;
    const uint32_t cbBitmap = (minimumStride * bitmap.RawUnsignedHeight());
    const uint32_t cbFile = bitmapOffset + cbBitmap;
    std::array<uint8_t, SizeFileheader> fileHeader{};
    ByteArrayUtil::WriteUInt16LE(fileHeader.data(), fileHeader.size(), FileheaderOffsetFileType, 0x4D42);
    ByteArrayUtil::WriteUInt32LE(fileHeader.data(), fileHeader.size(), FileheaderOffsetFileSize, cbFile);
    ByteArrayUtil::WriteUInt16LE(fileHeader.data(), fileHeader.size(), FileheaderOffsetReserved1, 0);
    ByteArrayUtil::WriteUInt16LE(fileHeader.data(), fileHeader.size(), FileheaderOffsetReserved2, 0);
    ByteArrayUtil::WriteUInt32LE(fileHeader.data(), fileHeader.size(), FileheaderOffsetBitmapOffset, bitmapOffset);
    stream.write(reinterpret_cast<const char*>(fileHeader.data()), fileHeader.size());

    WriteBitmapInfoHeader(stream, bitmap, cbFile);

    if (minimumStride == bitmap.Stride() && bitmap.GetOrigin() == BitmapOrigin::UpperLeft)
    {
      // Since its the minimum stride we can just write the entire image with one call
      const std::streamsize bitmapStride = bitmap.Stride();
      const auto cbBitmap2 = bitmap.RawUnsignedHeight() * bitmapStride;
      stream.write(reinterpret_cast<const char*>(bitmap.Content()), cbBitmap2);
    }
    else
    {
      if (bitmap.GetOrigin() == BitmapOrigin::UpperLeft)
      {
        // We have to write the image data one scan line at a time to 'compress' the stride to the minimum
        const auto srcStride = bitmap.Stride();
        const auto* pSrc = static_cast<const uint8_t*>(bitmap.Content());
        const uint8_t* const pSrcEnd = pSrc + (srcStride * bitmap.RawUnsignedHeight());
        while (pSrc < pSrcEnd)
        {
          stream.write(reinterpret_cast<const char*>(pSrc), minimumStride);
          pSrc += srcStride;
        }
      }
      else
      {
        // We have to write the image data one scan line at a time to 'compress' the stride to the minimum or to flip it
        const auto srcStride = bitmap.Stride();
        const auto* const pSrcStart = static_cast<const uint8_t*>(bitmap.Content());
        const uint8_t* pSrc = pSrcStart + (srcStride * (bitmap.RawUnsignedHeight() - 1));
        while (pSrc >= pSrcStart)
        {
          stream.write(reinterpret_cast<const char*>(pSrc), minimumStride);
          pSrc -= srcStride;
        }
      }
    }
  }
}
