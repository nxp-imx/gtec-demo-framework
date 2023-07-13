/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/Bits/ByteArrayUtil.hpp>
#include <FslBase/Compression/ValueCompression.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/String/UTF8String.hpp>
#include <FslBase/System/Platform/PlatformPathTransform.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/BinaryFontBasicKerningLoader.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <limits>
#include <vector>

namespace Fsl
{
  namespace
  {
    constexpr const uint32_t FILEHEADER_OFFSET_Magic = 0;
    constexpr const uint32_t FILEHEADER_OFFSET_Version = FILEHEADER_OFFSET_Magic + 4;
    constexpr const uint32_t FILEHEADER_OFFSET_Size = FILEHEADER_OFFSET_Version + 4;
    constexpr const uint32_t SIZE_FILEHEADER = FILEHEADER_OFFSET_Size + 4;

    constexpr const uint32_t HEADER_MAGIC = 0x004B4246;
    constexpr const uint32_t EXPECTED_VERSION = 1;

    // const uint32_t MAX_ENCODED_VALUE_SIZE = 5;

    struct FBKHeader
    {
      uint32_t Magic{0};
      uint32_t Version{0};
      uint32_t Size{0};
      FBKHeader() = default;
    };

    void StreamRead(std::ifstream& rStream, void* const pDst, const std::size_t cbRead)
    {
      rStream.read(reinterpret_cast<char*>(pDst), NumericCast<std::streamsize>(cbRead));
      if (!rStream.good())
      {
        throw FormatException("Failed to read the expected amount of bytes");
      }
    }


    FBKHeader ReadAndValidateHeader(std::ifstream& rStream)
    {
      std::array<uint8_t, SIZE_FILEHEADER> fileHeader{};
      // Try to read the file header
      StreamRead(rStream, fileHeader.data(), SIZE_FILEHEADER);

      FBKHeader header;
      header.Magic = ByteArrayUtil::ReadUInt32LE(fileHeader.data(), SIZE_FILEHEADER, FILEHEADER_OFFSET_Magic);
      header.Version = ByteArrayUtil::ReadUInt32LE(fileHeader.data(), SIZE_FILEHEADER, FILEHEADER_OFFSET_Version);
      header.Size = ByteArrayUtil::ReadUInt32LE(fileHeader.data(), SIZE_FILEHEADER, FILEHEADER_OFFSET_Size);

      if (header.Magic != HEADER_MAGIC)
      {
        throw FormatException("File not of the expected type");
      }

      if (header.Version != EXPECTED_VERSION)
      {
        throw FormatException(fmt::format("Unsupported version {} expected {}", header.Version, EXPECTED_VERSION));
      }
      return header;
    }


    // int32_t ReadRectangle(Rectangle& rResult, const uint8_t*const pSrc, const int32_t srcLength, const int32_t index)
    //{
    //  int32_t srcRectX, srcRectY;
    //  uint32_t srcRectWidth, srcRectHeight;
    //  int32_t currentIndex = index;
    //  currentIndex += ValueCompression::ReadSimple(srcRectX, pSrc, srcLength, currentIndex);
    //  currentIndex += ValueCompression::ReadSimple(srcRectY, pSrc, srcLength, currentIndex);
    //  currentIndex += ValueCompression::ReadSimple(srcRectWidth, pSrc, srcLength, currentIndex);
    //  currentIndex += ValueCompression::ReadSimple(srcRectHeight, pSrc, srcLength, currentIndex);

    //  if (srcRectWidth > static_cast<uint32_t>(std::numeric_limits<int32_t>::max()) || srcRectWidth >
    //  static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
    //    throw NotSupportedException("The rectangle is of a unsupported size");

    //  rResult = Rectangle(srcRectX, srcRectY, static_cast<int32_t>(srcRectWidth), static_cast<int32_t>(srcRectHeight));
    //  return currentIndex - index;
    //}

    std::size_t ReadString(UTF8String& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
    {
      uint32_t stringLength = 0;
      std::size_t currentIndex = index;
      currentIndex += ValueCompression::ReadSimple(stringLength, pSrc, srcLength, currentIndex);

      rResult = StringViewLite(reinterpret_cast<const char* const>(pSrc), srcLength).substr(currentIndex, stringLength);

      currentIndex += stringLength;
      assert(index <= currentIndex);
      return currentIndex - index;
    }

    std::size_t ReadRanges(BasicFontKerning& rTextureAtlas, const std::vector<uint8_t>& content, const std::size_t index)
    {
      std::size_t currentIndex = index;

      uint32_t entries = 0;
      currentIndex += ValueCompression::ReadSimple(entries, content.data(), content.size(), currentIndex);

      const auto count = NumericCast<int32_t>(entries);
      rTextureAtlas.SetRangeCapacity(count);
      for (int32_t i = 0; i < count; ++i)
      {
        uint32_t rangeFrom = 0;
        uint32_t rangeLength = 0;
        currentIndex += ValueCompression::ReadSimple(rangeFrom, content.data(), content.size(), currentIndex);
        currentIndex += ValueCompression::ReadSimple(rangeLength, content.data(), content.size(), currentIndex);
        rTextureAtlas.SetRange(i, FontGlyphRange(NumericCast<int32_t>(rangeFrom), NumericCast<int32_t>(rangeLength)));
      }
      assert(index <= currentIndex);
      return currentIndex - index;
    }


    std::size_t ReadGlyphKernings(BasicFontKerning& rTextureAtlas, const std::vector<uint8_t>& content, const std::size_t index)
    {
      std::size_t currentIndex = index;

      uint32_t entries = 0;
      currentIndex += ValueCompression::ReadSimple(entries, content.data(), content.size(), currentIndex);

      const auto count = NumericCast<int32_t>(entries);
      rTextureAtlas.SetGlyphKerningCapacity(count);
      for (int32_t i = 0; i < count; ++i)
      {
        int32_t offsetX = 0;
        int32_t offsetY = 0;
        uint32_t layoutWidth = 0;
        currentIndex += ValueCompression::ReadSimple(offsetX, content.data(), content.size(), currentIndex);
        currentIndex += ValueCompression::ReadSimple(offsetY, content.data(), content.size(), currentIndex);
        currentIndex += ValueCompression::ReadSimple(layoutWidth, content.data(), content.size(), currentIndex);

        offsetX = MathHelper::Clamp(offsetX, static_cast<int32_t>(std::numeric_limits<int16_t>::min()),
                                    static_cast<int32_t>(std::numeric_limits<int16_t>::max()));
        offsetY = MathHelper::Clamp(offsetY, static_cast<int32_t>(std::numeric_limits<int16_t>::min()),
                                    static_cast<int32_t>(std::numeric_limits<int16_t>::max()));
        uint16_t layoutWidth2 = MathHelper::Clamp(layoutWidth, static_cast<uint32_t>(std::numeric_limits<uint16_t>::min()),
                                                  static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()));

        assert(i <= std::numeric_limits<int32_t>::max());
        rTextureAtlas.SetGlyphKerning(
          i, FontGlyphBasicKerning(static_cast<int16_t>(offsetX), static_cast<int16_t>(offsetY), PxValueU16::Create(layoutWidth2)));
      }
      assert(index <= currentIndex);
      return currentIndex - index;
    }


    std::size_t ReadDescription(BasicFontKerning& rTextureAtlas, const std::vector<uint8_t>& content, const std::size_t index)
    {
      std::size_t currentIndex = index;

      uint32_t lineSpacing = 0;
      uint32_t baseLine = 0;
      uint32_t maxGlyphLeadingOverdrawAreaX = 0;
      uint32_t maxGlyphLeadingOverdrawAreaY = 0;
      currentIndex += ValueCompression::ReadSimple(lineSpacing, content.data(), content.size(), currentIndex);
      currentIndex += ValueCompression::ReadSimple(baseLine, content.data(), content.size(), currentIndex);
      currentIndex += ValueCompression::ReadSimple(maxGlyphLeadingOverdrawAreaX, content.data(), content.size(), currentIndex);
      currentIndex += ValueCompression::ReadSimple(maxGlyphLeadingOverdrawAreaY, content.data(), content.size(), currentIndex);

      rTextureAtlas.SetDesc(FontDesc(NumericCast<int32_t>(lineSpacing), NumericCast<int32_t>(baseLine),
                                     Point2(NumericCast<int32_t>(maxGlyphLeadingOverdrawAreaX), NumericCast<int32_t>(maxGlyphLeadingOverdrawAreaY))));
      assert(index <= currentIndex);
      return currentIndex - index;
    }


    void ReadEntries(BasicFontKerning& rTextureAtlas, std::ifstream& rStream, const uint32_t contentSize)
    {
      std::vector<uint8_t> content(contentSize);
      StreamRead(rStream, content.data(), content.size());

      std::size_t currentIndex = 0;
      currentIndex += ReadRanges(rTextureAtlas, content, currentIndex);
      currentIndex += ReadGlyphKernings(rTextureAtlas, content, currentIndex);
      currentIndex += ReadDescription(rTextureAtlas, content, currentIndex);

      UTF8String strTmp;
      currentIndex += ReadString(strTmp, content.data(), content.size(), currentIndex);
      rTextureAtlas.SetName(strTmp);
      ReadString(strTmp, content.data(), content.size(), currentIndex);
      rTextureAtlas.SetPathName(IO::Path(strTmp));
    }
  }


  void BinaryFontBasicKerningLoader::Load(BasicFontKerning& rTextureAtlas, const IO::Path& strFilename)
  {
    std::ifstream file(PlatformPathTransform::ToSystemPath(strFilename), std::ios::in | std::ios::binary);
    if (file.good())
    {
      Load(rTextureAtlas, file);
    }
    else
    {
      throw FormatException(fmt::format("File not found: '{}'", strFilename));
    }
  }


  void BinaryFontBasicKerningLoader::Load(BasicFontKerning& rTextureAtlas, std::ifstream& rStream)
  {
    const FBKHeader header = ReadAndValidateHeader(rStream);
    ReadEntries(rTextureAtlas, rStream, header.Size);

    if (!rTextureAtlas.IsValid())
    {
      throw FormatException("The file format appears to be invalid");
    }
  }
}
