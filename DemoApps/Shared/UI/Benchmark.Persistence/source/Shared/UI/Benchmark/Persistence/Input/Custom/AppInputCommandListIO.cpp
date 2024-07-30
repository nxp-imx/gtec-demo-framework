/****************************************************************************************************************************************************
 * Copyright 2021-2023 NXP
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

#include "AppInputCommandListIO.hpp"
#include <FslBase/Bits/ByteArrayUtil.hpp>
#include <FslBase/Bits/ByteSpanUtil_ReadLE.hpp>
#include <FslBase/Bits/ByteSpanUtil_WriteLE.hpp>
#include <FslBase/Compression/ValueCompression.hpp>
#include <FslBase/Compression/ValueCompression_Span.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <fmt/format.h>
#include <limits>
#include <utility>

namespace Fsl::AppInputCommandListIO
{
  namespace
  {
    enum class VersionId : uint32_t
    {
      Version1 = 1,
      Version2 = 2
    };

    namespace Header
    {
      // LCN, since this is written as little endian it becomes NCL in the file
      constexpr const uint32_t Magic = 0x004C434E;
      constexpr const uint32_t MinVersion = static_cast<uint32_t>(VersionId::Version1);
      constexpr const uint32_t MaxVersion = static_cast<uint32_t>(VersionId::Version2);

      constexpr const uint32_t HeaderOffsetMagic = 0;
      constexpr const uint32_t HeaderOffsetVersion = 4;
      constexpr const uint32_t HeaderOffsetContentSize = 8;
      constexpr const uint32_t SizeOfHeader = 4 * 3;
    }

    ReadOnlySpan<uint8_t> ReadAndValidateHeader(const ReadOnlySpan<uint8_t>& header, VersionId& rVersion)
    {
      auto magic = ByteSpanUtil::ReadUInt32LE(header, Header::HeaderOffsetMagic);
      auto version = ByteSpanUtil::ReadUInt32LE(header, Header::HeaderOffsetVersion);
      auto contentSize = ByteSpanUtil::ReadUInt32LE(header, Header::HeaderOffsetContentSize);
      if (magic != Header::Magic)
      {
        throw FormatException("invalid header");
      }
      if (version < Header::MinVersion || version > Header::MaxVersion)
      {
        throw FormatException(fmt::format("unsupported AppInputCommandList version {} the currently supported version range is {} to {}", version,
                                          Header::MinVersion, Header::MaxVersion));
      }
      auto remainingSpan = header.subspan(Header::SizeOfHeader);
      if (contentSize != remainingSpan.size())
      {
        throw FormatException("content is not of the expected size");
      }
      rVersion = static_cast<VersionId>(version);
      return remainingSpan;
    }

    constexpr std::size_t CalcInputCommandRecordMaxSize() noexcept
    {
      std::size_t size = 0;
      size += ValueCompression::Details::MaxByteSizeUInt32;       // uint32_t FrameIndex       - uint32_t
      size += ValueCompression::Details::MaxByteSizeUInt32;       // InputCommandId CommandId  - uint32_t
      size += sizeof(uint64_t);                                   // CustomWindowId WindowId   - uint64_t
      size += ValueCompression::Details::MaxByteSizeInt32 * 4;    // PxRectangle WindowRectPx  - 4x int32_t
      size += ValueCompression::Details::MaxByteSizeInt32 * 4;    // PxPoint2 MousePosition    - 2x int32_t
      return size;
    }

    std::size_t CalcMaxSize(const AppInputCommandList& content)
    {
      std::size_t size = Header::SizeOfHeader;
      size += ValueCompression::Details::MaxByteSizeUInt32;                 // uint32_t FrameCount - uint32_t
      size += ValueCompression::Details::MaxByteSizeUInt32;                 // uint32_t CommandCount - uint32_t
      size += CalcInputCommandRecordMaxSize() * content.GetFrameCount();    // Commands
      return size;
    }

    constexpr Span<uint8_t> WriteHeader(Span<uint8_t> span)
    {
      assert(span.size() >= Header::SizeOfHeader);
      ByteSpanUtil::WriteUInt32LE(span, Header::HeaderOffsetMagic, Header::Magic);
      ByteSpanUtil::WriteUInt32LE(span, Header::HeaderOffsetVersion, static_cast<uint32_t>(VersionId::Version2));
      ByteSpanUtil::WriteUInt32LE(span, Header::HeaderOffsetContentSize, 0);
      return span.subspan(Header::SizeOfHeader);
    }

    Span<uint8_t> Write(Span<uint8_t> dstSpan, const PxRectangle& value)
    {
      dstSpan = ValueCompression::WriteSimpleInt32(dstSpan, value.RawLeft());
      dstSpan = ValueCompression::WriteSimpleInt32(dstSpan, value.RawTop());
      dstSpan = ValueCompression::WriteSimpleInt32(dstSpan, value.RawWidth());
      dstSpan = ValueCompression::WriteSimpleInt32(dstSpan, value.RawHeight());
      return dstSpan;
    }

    PxRectangle ReadPxRectangle(ReadOnlySpan<uint8_t>& rSrcSpan)
    {
      const auto x = ValueCompression::ReadSimpleInt32(rSrcSpan);
      const auto y = ValueCompression::ReadSimpleInt32(rSrcSpan);
      const auto width = ValueCompression::ReadSimpleInt32(rSrcSpan);
      const auto height = ValueCompression::ReadSimpleInt32(rSrcSpan);
      return PxRectangle::Create(x, y, width, height);
    }

    Span<uint8_t> Write(Span<uint8_t> dstSpan, const PxPoint2& value)
    {
      dstSpan = ValueCompression::WriteSimpleInt32(dstSpan, value.X.Value);
      dstSpan = ValueCompression::WriteSimpleInt32(dstSpan, value.Y.Value);
      return dstSpan;
    }

    PxPoint2 ReadPxPoint2(ReadOnlySpan<uint8_t>& rSrcSpan)
    {
      const auto x = ValueCompression::ReadSimpleInt32(rSrcSpan);
      const auto y = ValueCompression::ReadSimpleInt32(rSrcSpan);
      return PxPoint2::Create(x, y);
    }

    Span<uint8_t> Write(Span<uint8_t> dstSpan, const PxSize2D& value)
    {
      dstSpan = ValueCompression::WriteSimpleInt32(dstSpan, value.RawWidth());
      dstSpan = ValueCompression::WriteSimpleInt32(dstSpan, value.RawHeight());
      return dstSpan;
    }

    PxSize2D ReadPxSize2D(ReadOnlySpan<uint8_t>& rSrcSpan)
    {
      const auto width = ValueCompression::ReadSimpleInt32(rSrcSpan);
      const auto height = ValueCompression::ReadSimpleInt32(rSrcSpan);
      return PxSize2D::Create(width, height);
    }


    InputCommandId ReadInputCommandId(ReadOnlySpan<uint8_t>& rSrcSpan)
    {
      const auto commandId = static_cast<InputCommandId>(ValueCompression::ReadSimpleUInt32(rSrcSpan));
      switch (static_cast<InputCommandId>(commandId))
      {
      case InputCommandId::Invalid:
      case InputCommandId::MouseDown:
      case InputCommandId::MouseUp:
      case InputCommandId::MouseDownMove:
      case InputCommandId::MouseMove:
      case InputCommandId::MouseMoveClear:
        return commandId;
      default:
        throw InvalidFormatException("invalid format, unsupported command id");
      }
    }


    Span<uint8_t> WriteInputCommandRecord(Span<uint8_t> dstSpan, const InputCommandRecord& record)
    {
      dstSpan = ValueCompression::WriteSimpleUInt32(dstSpan, record.FrameIndex);
      dstSpan = ValueCompression::WriteSimpleUInt32(dstSpan, static_cast<uint32_t>(record.CommandId));
      dstSpan = ValueCompression::WriteSimpleUInt64(dstSpan, record.WindowId.Value);
      dstSpan = Write(dstSpan, record.WindowRectPx);
      dstSpan = Write(dstSpan, record.MousePositionPx);
      dstSpan = ValueCompression::WriteSimpleUInt32(dstSpan, record.IsTouch ? 1u : 0u);
      return dstSpan;
    }

    InputCommandRecord ReadInputCommandRecord(ReadOnlySpan<uint8_t>& rSrcSpan, const VersionId currentVersion)
    {
      const uint32_t frameIndex = ValueCompression::ReadSimpleUInt32(rSrcSpan);
      const InputCommandId commandId = ReadInputCommandId(rSrcSpan);
      const uint64_t windowId = ValueCompression::ReadSimpleUInt64(rSrcSpan);
      const PxRectangle windowRectPx = ReadPxRectangle(rSrcSpan);
      const PxPoint2 mousePositionPx = ReadPxPoint2(rSrcSpan);
      bool isTouch = false;
      if (currentVersion >= VersionId::Version2)
      {
        isTouch = ValueCompression::ReadSimpleUInt32(rSrcSpan) != 0;
      }
      return {frameIndex, commandId, CustomWindowId(windowId), windowRectPx, mousePositionPx, isTouch};
    }


    AppInputCommandList ReadAppInputCommandList(ReadOnlySpan<uint8_t>& rSrcSpan, const VersionId currentVersion)
    {
      const PxSize2D recordResolution = ReadPxSize2D(rSrcSpan);
      const uint32_t recordDensityDpi = ValueCompression::ReadSimpleUInt32(rSrcSpan);
      const uint32_t frameCount = ValueCompression::ReadSimpleUInt32(rSrcSpan);
      const uint32_t commandListCount = ValueCompression::ReadSimpleUInt32(rSrcSpan);
      std::vector<InputCommandRecord> commandList(commandListCount);
      for (uint32_t i = 0; i < commandListCount; ++i)
      {
        commandList[i] = ReadInputCommandRecord(rSrcSpan, currentVersion);
      }
      return {recordResolution, recordDensityDpi, std ::move(commandList), frameCount};
    }
  }

  std::vector<uint8_t> Encode(const AppInputCommandList& content)
  {
    std::vector<uint8_t> encodedContent(CalcMaxSize(content));
    auto dstSpan = SpanUtil::AsSpan(encodedContent);
    dstSpan = WriteHeader(dstSpan);

    {    // Write the content
      const ReadOnlySpan<InputCommandRecord> commandListSpan = content.AsSpan();
      if (commandListSpan.size() >= std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("Max command list size exceeded");
      }

      // Record resolution
      dstSpan = Write(dstSpan, content.GetRecordResolution());
      // Record density dpi
      dstSpan = ValueCompression::WriteSimpleUInt32(dstSpan, content.GetRecordDensityDpi());
      // Frame count
      dstSpan = ValueCompression::WriteSimpleUInt32(dstSpan, content.GetFrameCount());
      // Command list entries
      const auto commandListCount = UncheckedNumericCast<uint32_t>(commandListSpan.size());
      dstSpan = ValueCompression::WriteSimpleUInt32(dstSpan, commandListCount);
      for (uint32_t i = 0; i < commandListCount; ++i)
      {
        dstSpan = WriteInputCommandRecord(dstSpan, commandListSpan[i]);
      }
    }

    // Calc the actual size
    const auto finalSize = dstSpan.data() - encodedContent.data();

    {    // Patch the header content size
      const auto contentSize = NumericCast<uint32_t>(finalSize - Header::SizeOfHeader);
      ByteArrayUtil::WriteUInt32LE(encodedContent.data(), encodedContent.size(), Header::HeaderOffsetContentSize, contentSize);
    }

    encodedContent.resize(finalSize);
    return encodedContent;
  }

  AppInputCommandList Decode(const ReadOnlySpan<uint8_t>& content)
  {
    VersionId currentVersion{0};
    auto remainingContent = ReadAndValidateHeader(content, currentVersion);
    AppInputCommandList commandList = ReadAppInputCommandList(remainingContent, currentVersion);
    if (!remainingContent.empty())
    {
      throw InvalidFormatException("All content was not consumed");
    }
    return commandList;
  }
}
