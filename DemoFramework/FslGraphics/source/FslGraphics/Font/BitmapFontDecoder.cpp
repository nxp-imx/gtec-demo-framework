/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslBase/Bits/ByteSpanUtil_ReadLE.hpp>
#include <FslBase/Compression/ValueCompression.hpp>
#include <FslBase/Compression/ValueCompression_Span.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslGraphics/Font/BitmapFontDecoder.hpp>
// #include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/IO/File.hpp>
#include <fmt/format.h>
#include <utility>

namespace Fsl
{
  namespace
  {
    namespace NBFHeader
    {
      constexpr const uint32_t Version2 = 2;
      constexpr const uint32_t Version3 = 3;

      // NBF, since this is written as little endian it becomes FBN in the file :/
      constexpr const uint32_t Magic = 0x004E4246;
      constexpr const uint32_t MinVersion = Version2;
      constexpr const uint32_t MaxVersion = Version3;

      constexpr const uint32_t HeaderOffsetMagic = 0;
      constexpr const uint32_t HeaderOffsetVersion = 4;
      constexpr const uint32_t HeaderOffsetContentSize = 8;
      constexpr const uint32_t SizeOfHeader = 4 * 3;
    }

    ReadOnlySpan<uint8_t> ReadAndValidateHeader(const ReadOnlySpan<uint8_t>& header, uint32_t& rVersion)
    {
      auto magic = ByteSpanUtil::ReadUInt32LE(header, NBFHeader::HeaderOffsetMagic);
      auto version = ByteSpanUtil::ReadUInt32LE(header, NBFHeader::HeaderOffsetVersion);
      auto contentSize = ByteSpanUtil::ReadUInt32LE(header, NBFHeader::HeaderOffsetContentSize);
      if (magic != NBFHeader::Magic)
      {
        throw FormatException("invalid header");
      }
      if (version < NBFHeader::MinVersion || version > NBFHeader::MaxVersion)
      {
        throw FormatException(fmt::format("unsupported NBF version {} the currently supported version range is {} to {}", version,
                                          NBFHeader::MinVersion, NBFHeader::MaxVersion));
      }
      auto remainingSpan = header.subspan(NBFHeader::SizeOfHeader);
      if (contentSize != remainingSpan.size())
      {
        throw FormatException("content is not of the expected size");
      }
      rVersion = version;
      return remainingSpan;
    }

    std::string ReadString(ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t stringLength = ValueCompression::ReadSimpleUInt32(rSpan);
      std::string res(reinterpret_cast<const char* const>(rSpan.data()), stringLength);
      rSpan = rSpan.subspan(stringLength);
      return res;
    }

    BitmapFontType ReadBitmapFontType(ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t value = ValueCompression::ReadSimpleUInt32(rSpan);
      switch (static_cast<BitmapFontType>(value))
      {
      case BitmapFontType::Bitmap:
        return BitmapFontType::Bitmap;
      case BitmapFontType::SDF:
        return BitmapFontType::SDF;
      default:
        throw FormatException("Unsupported bitmap font type");
      }
    }

    PxRectangleU32 ReadPxRectangleU(ReadOnlySpan<uint8_t>& rSpan)
    {
      const auto x = ValueCompression::ReadSimpleUInt32(rSpan);
      const auto y = ValueCompression::ReadSimpleUInt32(rSpan);
      const auto width = ValueCompression::ReadSimpleUInt32(rSpan);
      const auto height = ValueCompression::ReadSimpleUInt32(rSpan);
      return {x, y, width, height};
    }


    PxPoint2 ReadPxPoint2(ReadOnlySpan<uint8_t>& rSpan)
    {
      const auto x = ValueCompression::ReadSimpleInt32(rSpan);
      const auto y = ValueCompression::ReadSimpleInt32(rSpan);
      return {x, y};
    }

    BitmapFontChar DecodeChar(ReadOnlySpan<uint8_t>& rSpan)
    {
      const auto id = ValueCompression::ReadSimpleUInt32(rSpan);
      const PxRectangleU32 srcTextureRectPx = ReadPxRectangleU(rSpan);
      const auto offsetPx = ReadPxPoint2(rSpan);
      const auto xAdvancePx = ValueCompression::ReadSimpleUInt16(rSpan);
      return {id, srcTextureRectPx, offsetPx, xAdvancePx};
    }

    BitmapFontKerning DecodeKerning(ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t first = ValueCompression::ReadSimpleUInt32(rSpan);
      const uint32_t second = ValueCompression::ReadSimpleUInt32(rSpan);
      const auto amountPx = NumericCast<int16_t>(ValueCompression::ReadSimpleInt32(rSpan));
      return {first, second, amountPx};
    }

    std::vector<BitmapFontChar> DecodeChars(ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t entries = ValueCompression::ReadSimpleUInt32(rSpan);
      std::vector<BitmapFontChar> result(entries);
      for (std::size_t i = 0; i < result.size(); ++i)
      {
        result[i] = DecodeChar(rSpan);
      }
      return result;
    }

    std::vector<BitmapFontKerning> DecodeKernings(ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t entries = ValueCompression::ReadSimpleUInt32(rSpan);
      std::vector<BitmapFontKerning> result(entries);
      for (std::size_t i = 0; i < result.size(); ++i)
      {
        result[i] = DecodeKerning(rSpan);
      }
      return result;
    }
  }

  BitmapFont BitmapFontDecoder::Load(const IO::Path& strFilename)
  {
    auto content = IO::File::ReadAllBytes(strFilename);
    return Decode(ReadOnlySpan<uint8_t>(content.data(), content.size()));
  }

  BitmapFont BitmapFontDecoder::Decode(const ReadOnlySpan<uint8_t>& content)
  {
    uint32_t currentVersion{0};
    auto remainingContent = ReadAndValidateHeader(content, currentVersion);

    auto name = ReadString(remainingContent);
    auto dpi = ValueCompression::ReadSimpleUInt16(remainingContent);
    auto size = ValueCompression::ReadSimpleUInt16(remainingContent);
    auto lineSpacingPx = ValueCompression::ReadSimpleUInt16(remainingContent);
    auto baseLinePx = ValueCompression::ReadSimpleUInt16(remainingContent);
    uint16_t sdfSpread = 0;
    uint16_t sdfDesiredBaseLinePx = 0;
    uint16_t paddingLeft = 0;
    uint16_t paddingTop = 0;
    uint16_t paddingRight = 0;
    uint16_t paddingBottom = 0;
    if (currentVersion == NBFHeader::Version3)
    {
      paddingLeft = ValueCompression::ReadSimpleUInt16(remainingContent);
      paddingTop = ValueCompression::ReadSimpleUInt16(remainingContent);
      paddingRight = ValueCompression::ReadSimpleUInt16(remainingContent);
      paddingBottom = ValueCompression::ReadSimpleUInt16(remainingContent);
      sdfSpread = ValueCompression::ReadSimpleUInt16(remainingContent);
      sdfDesiredBaseLinePx = ValueCompression::ReadSimpleUInt16(remainingContent);
    }
    auto textureName = ReadString(remainingContent);
    auto fontType = ReadBitmapFontType(remainingContent);

    auto chars = DecodeChars(remainingContent);
    auto kernings = DecodeKernings(remainingContent);
    if (!remainingContent.empty())
    {
      throw FormatException("Failed to read all content");
    }

    const float sdfScale = sdfDesiredBaseLinePx == 0 ? 1.0f : static_cast<float>(sdfDesiredBaseLinePx) / static_cast<float>(baseLinePx);
    const PxThicknessU16 paddingPx(paddingLeft, paddingTop, paddingRight, paddingBottom);
    BitmapFont::SdfParams sdfParams(sdfSpread, sdfScale);
    return {std::move(name),        dpi,      size,      lineSpacingPx,    baseLinePx,         paddingPx,
            std::move(textureName), fontType, sdfParams, std::move(chars), std::move(kernings)};
  }
}
