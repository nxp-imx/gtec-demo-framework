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
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/IO/BMPUtil.hpp>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <sstream>

// BMP format explained:
// http://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
// http://www.fileformat.info/format/bmp/egff.htm

// Nasty hack for dealing with UTF8 file names on windows,
// since its the only supported platform that doesn't allow UTF8 strings
// but instead provides its own 'hack' for opening wstring's
#ifdef _WIN32
#include <FslBase/System/Platform/PlatformWin32.hpp>
#define PATH_GET_NAME(X) PlatformWin32::Widen(X.ToUTF8String())
#else
#define PATH_GET_NAME(X) X.ToUTF8String()
#endif


namespace Fsl
{
  namespace
  {
    // These would be called BI_RGB etc when searching the web for them
    enum class BmpCompression
    {
      RGB = 0,
      RLE8 = 1,
      RLE4 = 2,
      BITFIELDS = 3,
      JPEG = 4,
      PNG = 5,
      ALPHABITFIELDS = 6,
      CMYK = 11,
      CMYKRLE8 = 12,
      CMYKTLE4 = 13
    };


    struct BMPFileHeader
    {
      uint16_t FileType;
      // This field only stores a useful value when the bitmap data is compressed, and this value is usually zero in uncompressed BMP files.
      uint32_t FileSize;
      uint16_t Reserved1;
      uint16_t Reserved2;
      // BitmapOffset is the starting offset of the bitmap data from the beginning of the file in bytes.
      uint32_t BitmapOffset;

      BMPFileHeader()
        : FileType(0)
        , FileSize(0)
        , Reserved1(0)
        , Reserved2(0)
        , BitmapOffset(0)
      {
      }
    };


    struct BMPBitmapV3InfoHeader
    {
      uint32_t      DIBHeaderSize;
      int32_t       ImageWidth;
      int32_t       ImageHeight;
      uint16_t      Planes;
      uint16_t      BitsPerPixel;
      uint32_t      Compression;
      uint32_t      ImageSize;
      int32_t       XPixelsPerMeter;
      int32_t       YPixelsPerMeter;
      uint32_t      ColorsInColorTable;
      uint32_t      ImportantColorCount;
      uint32_t      RedMask;
      uint32_t      GreenMask;
      uint32_t      BlueMask;
      uint32_t      AlphaMask;

      BMPBitmapV3InfoHeader()
        : DIBHeaderSize(0)
        , ImageWidth(0)
        , ImageHeight(0)
        , Planes(0)
        , BitsPerPixel(0)
        , Compression(0)
        , ImageSize(0)
        , XPixelsPerMeter(0)
        , YPixelsPerMeter(0)
        , ColorsInColorTable(0)
        , ImportantColorCount(0)
        , RedMask(0)
        , GreenMask(0)
        , BlueMask(0)
        , AlphaMask(0)
      {
      }
    };

    const int32_t PIXELS_PER_METER_DEFAULT = 100; // ? magic number

    // The max bitmap size if 512MB
    const uint64_t MAX_BITMAP_SIZE = 1024 * 1024 * 512;

    const uint16_t SIZE_FILEHEADER = 14;
    const uint16_t SIZE_BITMAPINFOHEADER = 40;
    const uint16_t SIZE_BITMAPV2INFOHEADER = 52;
    const uint16_t SIZE_BITMAPV3INFOHEADER = 56;
    const uint16_t SIZE_BITMAPV4HEADER = 108;
    const uint16_t SIZE_BITMAPV5HEADER = 124;

    const uint16_t MAX_BITMAPHEADER_SIZE = SIZE_BITMAPV5HEADER;


    const int32_t FILEHEADER_OFFSET_FileType = 0;
    const int32_t FILEHEADER_OFFSET_FileSize = 2;
    const int32_t FILEHEADER_OFFSET_Reserved1 = 6;
    const int32_t FILEHEADER_OFFSET_Reserved2 = 8;
    const int32_t FILEHEADER_OFFSET_BitmapOffset = 10;

    const int32_t CB_BITMAPHEADER_SIZE_FIELD = 4;


    // size of header in bytes
    const int32_t BITMAPINFOHEADER_OFFSET_Size = 0;
    // image width in pixels
    // Width does not include any scan - line boundary padding.
    const int32_t BITMAPINFOHEADER_OFFSET_ImageWidth = 4;
    // image height in pixels
    // If Height is a positive number, then the image is a "bottom-up" bitmap with the origin in the lower - left corner.
    // If Height is a negative number, then the image is a "top-down" bitmap with the origin in the upper - left corner.
    const int32_t BITMAPINFOHEADER_OFFSET_ImageHeight = 8;
    // the number of color planes
    const int32_t BITMAPINFOHEADER_OFFSET_Planes = 12;
    // Bits per pixel
    const int32_t BITMAPINFOHEADER_OFFSET_BitsPerPixel = 14;
    // Compression
    const int32_t BITMAPINFOHEADER_OFFSET_Compression = 16;
    // Size of bitmap (can be zero for uncompressed bitmaps)
    const int32_t BITMAPINFOHEADER_OFFSET_ImageSize = 20;
    // Horizontal resolution in pixels per meter
    const int32_t BITMAPINFOHEADER_OFFSET_XPixelsPerMeter = 24;
    // Vertical resolution in pixels per meter
    const int32_t BITMAPINFOHEADER_OFFSET_YPixelsPerMeter = 28;
    // number of colors in the image
    const int32_t BITMAPINFOHEADER_OFFSET_ColorsInColorTable = 32;
    // minimum number of important colors
    const int32_t BITMAPINFOHEADER_OFFSET_ImportantColorCount = 36;
    // Red mask
    const int32_t BITMAPV2INFOHEADER_OFFSET_RedMask = 40;
    // Green mask
    const int32_t BITMAPV2INFOHEADER_OFFSET_GreenMask = 44;
    // Blue mask
    const int32_t BITMAPV2INFOHEADER_OFFSET_BlueMask = 48;

    // Alpha mask
    const int32_t BITMAPV3INFOHEADER_OFFSET_AlphaMask = 52;


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
      bool Swizzle;
      int8_t SwizzleR;
      int8_t SwizzleG;
      int8_t SwizzleB;
      int8_t SwizzleA;

      BitmapHeader(const BMPBitmapV3InfoHeader& header, BmpCompression compression)
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
        , Swizzle(false)
        , SwizzleR(0)
        , SwizzleG(1)
        , SwizzleB(2)
        , SwizzleA(3)
      {
        if (header.ImageWidth < 0)
          throw FormatException("Negative width bitmaps not supported");

        if (header.Planes != 1)
          throw FormatException("Bitmap did not contain the expected amount of planes");

        // Check for mask overlaps
        if ((header.RedMask & header.GreenMask) != 0 ||
            (header.RedMask & header.BlueMask) != 0 ||
            (header.RedMask & header.AlphaMask) != 0 ||
            (header.GreenMask & header.BlueMask) != 0 ||
            (header.GreenMask & header.AlphaMask) != 0 ||
            (header.BlueMask & header.AlphaMask) != 0 )
        {
          throw FormatException("The bitmap uses a unsupported mask");
        }

        // Quick resolution
        if (header.RedMask != 0x00FF0000 || header.GreenMask != 0x0000FF00 || header.BlueMask == 0x000000FF)
        {
          const int shiftR = BitsUtil::IndexOf(header.RedMask);
          const int shiftG = BitsUtil::IndexOf(header.GreenMask);
          const int shiftB = BitsUtil::IndexOf(header.BlueMask);
          if (shiftR < 0 || shiftG < 0 || shiftB < 0)
            throw FormatException("The bitmap uses a unsupported mask");

          if ((header.RedMask >> shiftR) != 0xFF || (header.GreenMask >> shiftG) != 0xFF || (header.BlueMask >> shiftB) != 0xFF)
            throw FormatException("The bitmap uses a unsupported mask");

          // We only support masks that are shifts of 0, 8, 16, 24
          if ((shiftR & 7) != 0 || (shiftG & 7) != 0 || (shiftB & 7) != 0 )
            throw FormatException("The bitmap uses a unsupported mask");

          Swizzle = true;
          SwizzleR = shiftR / 8;
          SwizzleG = shiftG / 8;
          SwizzleB = shiftB / 8;
        }

        if (header.AlphaMask != 0xFF000000 && header.AlphaMask != 0x00000000)
        {
          const int shiftA = BitsUtil::IndexOf(header.AlphaMask);
          if (header.AlphaMask != 0 && (header.AlphaMask >> shiftA) != 0xFF)
            throw FormatException("The bitmap uses a unsupported alpha mask");

          // We only support masks that are shifts of 0, 8, 16, 24
          if ((shiftA & 7) != 0)
            throw FormatException("The bitmap uses a unsupported alpha mask");

          assert(Swizzle);
          SwizzleA = shiftA / 8;
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


    void StreamRead(std::ifstream& rStream, void*const pDst, const std::size_t cbRead)
    {
      rStream.read(reinterpret_cast<char*>(pDst), cbRead);
      if (! rStream.good())
        throw FormatException("Failed to read the expected amount of bytes");
    }


    const BMPFileHeader ReadFileHeader(std::ifstream& rStream)
    {
      uint8_t fileheader[SIZE_FILEHEADER];
      // Try to read the file header
      StreamRead(rStream, fileheader, SIZE_FILEHEADER);

      BMPFileHeader header;
      header.FileType = ByteArrayUtil::ReadUInt16LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_FileType);
      header.FileSize = ByteArrayUtil::ReadUInt32LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_FileSize);
      header.Reserved1 = ByteArrayUtil::ReadUInt16LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_Reserved1);
      header.Reserved2 = ByteArrayUtil::ReadUInt16LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_Reserved2);
      header.BitmapOffset = ByteArrayUtil::ReadUInt16LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_BitmapOffset);

      if (header.FileType != 0x4D42)
        throw FormatException("Unsupported BMP file format");
      return header;
    }


    const BitmapHeader ReadBitmapHeader(std::ifstream& rStream)
    {
      uint8_t bitmapHeader[MAX_BITMAPHEADER_SIZE];

      // Read uint32 that describe the size of the bitmap header
      StreamRead(rStream, reinterpret_cast<char*>(bitmapHeader), CB_BITMAPHEADER_SIZE_FIELD);

      BMPBitmapV3InfoHeader header;
      header.DIBHeaderSize = ByteArrayUtil::ReadUInt32LE(bitmapHeader, 4, 0);

      // Check if its one of the common header sizes
      if (header.DIBHeaderSize != SIZE_BITMAPINFOHEADER && header.DIBHeaderSize != SIZE_BITMAPV2INFOHEADER &&
          header.DIBHeaderSize != SIZE_BITMAPV3INFOHEADER && header.DIBHeaderSize != SIZE_BITMAPV4HEADER &&
          header.DIBHeaderSize != SIZE_BITMAPV5HEADER )
      {
        throw FormatException("bitmap header not of the supported size");
      }

      assert(header.DIBHeaderSize <= MAX_BITMAPHEADER_SIZE);

      // Read the rest of the header
      StreamRead(rStream, bitmapHeader + CB_BITMAPHEADER_SIZE_FIELD, header.DIBHeaderSize - CB_BITMAPHEADER_SIZE_FIELD);

      // Decode the standard BITMAPINFOHEADER
      header.ImageWidth  = ByteArrayUtil::ReadInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_ImageWidth);
      header.ImageHeight = ByteArrayUtil::ReadInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_ImageHeight);
      header.Planes = ByteArrayUtil::ReadUInt16LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_Planes);
      header.BitsPerPixel = ByteArrayUtil::ReadUInt16LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_BitsPerPixel);
      header.Compression = ByteArrayUtil::ReadUInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_Compression);
      header.ImageSize = ByteArrayUtil::ReadUInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_ImageSize);
      header.XPixelsPerMeter = ByteArrayUtil::ReadInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_XPixelsPerMeter);
      header.YPixelsPerMeter = ByteArrayUtil::ReadInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_YPixelsPerMeter);
      header.ColorsInColorTable = ByteArrayUtil::ReadUInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_ColorsInColorTable);
      header.ImportantColorCount = ByteArrayUtil::ReadUInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPINFOHEADER_OFFSET_ImportantColorCount);

      const BmpCompression compression = IdentifyCompression(header.Compression);

      if (header.DIBHeaderSize >= SIZE_BITMAPV2INFOHEADER && header.BitsPerPixel > 24)
      {
        // Read the BITMAPV2INFOHEADER extensions
        header.RedMask = ByteArrayUtil::ReadUInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPV2INFOHEADER_OFFSET_RedMask);
        header.GreenMask = ByteArrayUtil::ReadUInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPV2INFOHEADER_OFFSET_GreenMask);
        header.BlueMask = ByteArrayUtil::ReadUInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPV2INFOHEADER_OFFSET_BlueMask);
      }
      else
      {
        header.RedMask = 0x00FF0000;
        header.GreenMask = 0x0000FF00;
        header.BlueMask = 0x000000FF;
      }

      if (header.DIBHeaderSize >= SIZE_BITMAPV3INFOHEADER)
      {
        // Read the  BITMAPV3INFOHEADER extensions
        header.AlphaMask = ByteArrayUtil::ReadUInt32LE(bitmapHeader, header.DIBHeaderSize, BITMAPV3INFOHEADER_OFFSET_AlphaMask);
      }
      else
      {
        header.AlphaMask = (header.BitsPerPixel == 32 ? 0xFF000000 : 0);
      }

      return BitmapHeader(header, compression);

    }


    uint32_t CalcBitmapSize(const BitmapHeader& bitmapHeader, const uint32_t minimumStride, const uint32_t bytesPerPixel, const bool allowZero)
    {
      const uint64_t calculatedSize = uint64_t(minimumStride) * uint64_t(bitmapHeader.ImageHeight);
      if (calculatedSize > MAX_BITMAP_SIZE)
        throw FormatException("The bitmap size was exceeded");

      const auto expectedBitmapSize = static_cast<uint32_t>(calculatedSize);
      if (bitmapHeader.Compression != BmpCompression::RGB)
      {
        if (bitmapHeader.ImageSize != expectedBitmapSize && !(allowZero && bitmapHeader.ImageSize == 0u))
          throw FormatException("The bitmap size is incorrect");
      }
      return expectedBitmapSize;
    }


    void ReadBitmapContent(std::ifstream& rStream, Bitmap& rBitmap, const uint32_t cbBitmap, const uint32_t minStride, const BitmapHeader& bitmapHeader, const BitmapOrigin originHint)
    {
      RawBitmapEx rawBitmap;
      Bitmap::ScopedDirectAccess scopedAccess(rBitmap, rawBitmap);

      rawBitmap.SetBitmapOrigin(!bitmapHeader.IsYFlipped ? BitmapOrigin::UpperLeft : BitmapOrigin::LowerLeft);

      assert(rawBitmap.Stride() == minStride);
      if (cbBitmap > rawBitmap.GetBufferLength())
        throw FormatException("Decode BMP failed due to unexpected buffer size");

      // Since the raw bitmap is in the exact same format we can just read directly into it.
      StreamRead(rStream, rawBitmap.Content(), cbBitmap);

      if (rawBitmap.GetOrigin() != originHint )
        RawBitmapUtil::FlipHorizontal(rawBitmap);
      if (bitmapHeader.Swizzle)
        RawBitmapUtil::Swizzle(rawBitmap, bitmapHeader.SwizzleR, bitmapHeader.SwizzleG, bitmapHeader.SwizzleB, bitmapHeader.SwizzleA);
    }


    void DecodeBitmap24BPP(Bitmap& rBitmap, std::ifstream& rStream, const BitmapHeader& bitmapHeader, const BitmapOrigin originHint)
    {
      if (bitmapHeader.BitsPerPixel == 24 && bitmapHeader.Compression != BmpCompression::RGB)
        throw FormatException("Bitmap 24bpp not using uncompressed compression");

      // Uncompressed data is a series of values representing either color palette indices or actual RGB color values.
      // Pixels are packed into bytes and arranged as scan lines.Each scan line must end on a 4 - byte boundary,
      // so one, two, or three bytes of padding may follow each scan line.
      const uint32_t bytesPerPixel = 3;
      const auto minimumBmpStride = PixelFormatUtil::CalcMinimumStride(bitmapHeader.ImageWidth, bytesPerPixel, StrideRequirement::MinimumAlign32Bit);
      const auto cbBitmap = CalcBitmapSize(bitmapHeader, minimumBmpStride, bytesPerPixel, true);

      // Ensure that the bitmap can hold the image (dont modify because we fill eventual padding with content from the bmp)
      const PixelFormat pf = !bitmapHeader.Swizzle ? PixelFormat::B8G8R8_UINT : PixelFormat::R8G8B8_UINT;
      rBitmap.Reset(Extent2D(bitmapHeader.ImageWidth, bitmapHeader.ImageHeight), pf, minimumBmpStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);

      ReadBitmapContent(rStream, rBitmap, cbBitmap, minimumBmpStride, bitmapHeader, originHint);
    }


    void DecodeBitmap32BPP(Bitmap& rBitmap, std::ifstream& rStream, const BitmapHeader& bitmapHeader, const BitmapOrigin originHint)
    {
      if (bitmapHeader.BitsPerPixel == 32 && bitmapHeader.Compression != BmpCompression::BITFIELDS && bitmapHeader.Compression != BmpCompression::RGB)
        throw FormatException("Bitmap 32bpp not using bit-fields compression");

      const uint32_t bytesPerPixel = 4;
      const auto minStride = PixelFormatUtil::CalcMinimumStride(bitmapHeader.ImageWidth, bytesPerPixel);
      const auto cbBitmap = CalcBitmapSize(bitmapHeader, minStride, bytesPerPixel, false);

      // Ensure that the bitmap can hold the image (We use dont modify because there is no padding to clear and everything will be overwritten)
      const PixelFormat pf = !bitmapHeader.Swizzle ? PixelFormat::B8G8R8A8_UINT : PixelFormat::R8G8B8A8_UINT;
      rBitmap.Reset(Extent2D(bitmapHeader.ImageWidth, bitmapHeader.ImageHeight), pf, minStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);

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


    void WriteBitmapInfoHeader(std::ofstream& stream, const RawBitmap& bitmap, const uint32_t cbBitmap)
    {
      const int32_t width = bitmap.GetExtent().Width;
      const int32_t height = bitmap.GetExtent().Height;

      uint8_t bitmapHeader[SIZE_BITMAPV2INFOHEADER];
      ByteArrayUtil::WriteUInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_Size, SIZE_BITMAPINFOHEADER);
      ByteArrayUtil::WriteInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_ImageWidth, width);
      ByteArrayUtil::WriteInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_ImageHeight, -height);
      ByteArrayUtil::WriteUInt16LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_Planes, 1);
      ByteArrayUtil::WriteUInt16LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_BitsPerPixel, 32);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_Compression, static_cast<uint32_t>(BmpCompression::RGB));
      ByteArrayUtil::WriteUInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_ImageSize, cbBitmap);
      ByteArrayUtil::WriteInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_XPixelsPerMeter, PIXELS_PER_METER_DEFAULT);
      ByteArrayUtil::WriteInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_YPixelsPerMeter, PIXELS_PER_METER_DEFAULT);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_ColorsInColorTable, 0);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPINFOHEADER_OFFSET_ImportantColorCount, 0);

      ByteArrayUtil::WriteUInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPV2INFOHEADER_OFFSET_RedMask, 0x00FF0000);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPV2INFOHEADER_OFFSET_GreenMask, 0x0000FF00);
      ByteArrayUtil::WriteUInt32LE(bitmapHeader, SIZE_BITMAPV2INFOHEADER, BITMAPV2INFOHEADER_OFFSET_BlueMask, 0x000000FF);

      stream.write(reinterpret_cast<const char*>(bitmapHeader), SIZE_BITMAPV2INFOHEADER);
    }
  }


  void BMPUtil::Load(Bitmap& rBitmap, const IO::Path& strFilename)
  {
    Load(rBitmap, strFilename, BitmapOrigin::UpperLeft);
  }


  void BMPUtil::Load(Bitmap& rBitmap, const IO::Path& strFilename, const BitmapOrigin originHint)
  {
    std::ifstream file(PATH_GET_NAME(strFilename), std::ios::in | std::ios::binary);
    if (file.good())
      Load(rBitmap, file, originHint);
    else
    {
      std::stringstream strStream;
      strStream << "File not found: '" << strFilename.ToAsciiString() << "'";
      throw FormatException(strStream.str());
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
      throw FormatException("Failed to seek to the bitmap data area");

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
    catch (const std::exception)
    {
      return false;
    }
  }


  void BMPUtil::Save(const IO::Path& strFilename, const Bitmap& bitmap)
  {
    RawBitmap rawBitmap;
    Bitmap::ScopedDirectAccess scopedAccess(bitmap, rawBitmap);
    Save(strFilename, rawBitmap);
  }


  void BMPUtil::Save(const IO::Path& strFilename, const RawBitmap& bitmap)
  {
    std::ofstream file(PATH_GET_NAME(strFilename), std::ios::out | std::ios::binary);
    Save(file, bitmap);
  }


  void BMPUtil::Save(std::ofstream& stream, const Bitmap& bitmap)
  {
    RawBitmap rawBitmap;
    Bitmap::ScopedDirectAccess scopedAccess(bitmap, rawBitmap);
    Save(stream, rawBitmap);
  }


  void BMPUtil::Save(std::ofstream& stream, const RawBitmap& bitmap)
  {
    if (bitmap.GetPixelFormatLayout() != PixelFormatLayout::B8G8R8A8)
      throw UnsupportedPixelFormatException(bitmap.GetPixelFormat());

    const auto minimumStride = PixelFormatUtil::CalcMinimumStride(bitmap.Width(), bitmap.GetPixelFormat(), StrideRequirement::MinimumAlign32Bit);

    const uint32_t bitmapOffset = SIZE_FILEHEADER + SIZE_BITMAPV2INFOHEADER;
    const uint32_t cbBitmap = (minimumStride * bitmap.Height());
    const uint32_t cbFile = bitmapOffset + cbBitmap;
    uint8_t fileheader[SIZE_FILEHEADER];
    ByteArrayUtil::WriteUInt16LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_FileType, 0x4D42);
    ByteArrayUtil::WriteUInt32LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_FileSize, cbFile);
    ByteArrayUtil::WriteUInt16LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_Reserved1, 0);
    ByteArrayUtil::WriteUInt16LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_Reserved2, 0);
    ByteArrayUtil::WriteUInt32LE(fileheader, SIZE_FILEHEADER, FILEHEADER_OFFSET_BitmapOffset, bitmapOffset);
    stream.write(reinterpret_cast<const char*>(fileheader), SIZE_FILEHEADER);

    WriteBitmapInfoHeader(stream, bitmap, cbFile);

    if (minimumStride == bitmap.Stride() && bitmap.GetOrigin() == BitmapOrigin::UpperLeft)
    {
      // Since its the minimum stride we can just write the entire image with one call
      stream.write(reinterpret_cast<const char*>(bitmap.Content()), bitmap.Height() * bitmap.Stride());
    }
    else
    {
      if (bitmap.GetOrigin() == BitmapOrigin::UpperLeft)
      {
        // We have to write the image data one scan line at a time to 'compress' the stride to the minimum
        const auto srcStride = bitmap.Stride();
        const uint8_t* pSrc = static_cast<const uint8_t*>(bitmap.Content());
        const uint8_t*const pSrcEnd = pSrc + (srcStride * bitmap.Height());
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
        const uint8_t*const pSrcStart = static_cast<const uint8_t*>(bitmap.Content());
        const uint8_t* pSrc = pSrcStart + (srcStride * (bitmap.Height() - 1));
        while (pSrc >= pSrcStart)
        {
          stream.write(reinterpret_cast<const char*>(pSrc), minimumStride);
          pSrc -= srcStride;
        }
      }
    }
  }
}
