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

#include <FslGraphics/TextureAtlas/BinaryTextureAtlasLoader.hpp>
#include <FslBase/Bits/ByteSpanUtil_ReadLE.hpp>
#include <FslBase/Compression/ValueCompression_Span.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Math/Pixel/PxRectangleU.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Optional.hpp>
#include <FslBase/ReadOnlySpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/String/UTF8String.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <fmt/format.h>
#include <array>
#include <cassert>
#include <fstream>
#include <vector>
#include <utility>


// Nasty hack for dealing with UTF8 file names on windows,
// since its the only supported platform that doesn't allow UTF8 strings
// but instead provides its own 'hack' for opening wstring's
#ifdef _WIN32
#include <FslBase/System/Platform/PlatformWin32.hpp>
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PATH_GET_NAME(X) PlatformWin32::Widen(X.ToUTF8String())
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PATH_GET_NAME(X) X.ToUTF8String()
#endif

namespace Fsl
{
  namespace
  {
    namespace BTAFormat
    {
      constexpr const uint32_t DEFAULT_DP = 160;

      constexpr const uint32_t MAX_PARENT_COUNT = 64;

      namespace Header
      {
        constexpr const uint32_t OffsetMagic = 0;
        constexpr const uint32_t OffsetVersion = OffsetMagic + 4;
        constexpr const uint32_t OffsetSize = OffsetVersion + 4;
        constexpr const uint32_t HeaderSize = OffsetSize + 4;

        constexpr const uint32_t HeaderMagicValue = 0x00415442;
      }

      namespace Chunk
      {
        constexpr const uint32_t OffsetMagic = 0;
        constexpr const uint32_t OffsetSize = OffsetMagic + 4;
        constexpr const uint32_t HeaderSize = OffsetSize + 4;

        constexpr const uint32_t HeaderMagicValue = 0x6b6843;
      }

      constexpr const uint32_t BTA_VERSION1 = 0x0001;
      constexpr const uint32_t BTA_VERSION2 = 0x1001;
      constexpr const uint32_t BTA_VERSION3 = 0x2001;
      constexpr const uint32_t BTA_VERSION4 = 0x4000;

      constexpr const uint32_t CHUNKTYPE_NINESLICE_VERSION = 1;
    }

    enum class ChunkType
    {
      NineSlices = 0x1
    };

    // const uint32_t MAX_ENCODED_VALUE_SIZE = 5;

    struct BTAHeader
    {
      uint32_t Magic{0};
      uint32_t Version{0};
      uint32_t Size{0};
    };

    struct MinimalChunkHeader
    {
      uint32_t Magic{0};
      uint32_t Size{0};
    };


    struct EncodedPath
    {
      uint32_t ParentPathIndex{};
      IO::Path Path;
    };


    struct BTA2AtlasEntry
    {
      Rectangle SrcRectangle;
      Rectangle TrimmedRectangle;
      uint32_t ParentPathIndex{};
      IO::Path Path;
    };


    struct BTA3AtlasEntry
    {
      PxRectangleU RectanglePx;
      PxThicknessU TrimPx;
      uint32_t Dpi{};
      uint32_t ParentPathIndex{};
      IO::Path Path;
    };

    bool TryStreamRead(std::ifstream& rStream, void* const pDst, const std::size_t cbRead)
    {
      rStream.read(reinterpret_cast<char*>(pDst), cbRead);
      return rStream.good();
    }

    void StreamRead(std::ifstream& rStream, void* const pDst, const std::size_t cbRead)
    {
      if (!TryStreamRead(rStream, pDst, cbRead))
      {
        throw FormatException("Failed to read the expected amount of bytes");
      }
    }


    BTAHeader ReadAndValidateHeader(std::ifstream& rStream)
    {
      std::array<uint8_t, BTAFormat::Header::HeaderSize> fileheader{};
      // Try to read the file header
      StreamRead(rStream, fileheader.data(), fileheader.size());

      const auto fileHeaderSpan = ReadOnlySpanUtil::AsSpan(fileheader);

      BTAHeader header;
      header.Magic = ByteSpanUtil::ReadUInt32LE(fileHeaderSpan.subspan(BTAFormat::Header::OffsetMagic));
      header.Version = ByteSpanUtil::ReadUInt32LE(fileHeaderSpan.subspan(BTAFormat::Header::OffsetVersion));
      header.Size = ByteSpanUtil::ReadUInt32LE(fileHeaderSpan.subspan(BTAFormat::Header::OffsetSize));

      if (header.Magic != BTAFormat::Header::HeaderMagicValue)
      {
        throw FormatException("File not of the expected type");
      }

      if (header.Version != BTAFormat::BTA_VERSION1 && header.Version != BTAFormat::BTA_VERSION2 && header.Version != BTAFormat::BTA_VERSION3 &&
          header.Version != BTAFormat::BTA_VERSION4)
      {
        throw FormatException(fmt::format("Unsupported version {} expected {}, {}, {} or {}", header.Version, BTAFormat::BTA_VERSION1,
                                          BTAFormat::BTA_VERSION2, BTAFormat::BTA_VERSION3, BTAFormat::BTA_VERSION4));
      }
      return header;
    }

    ChunkType ReadChunkType(ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t chunkContentType = ValueCompression::ReadSimpleUInt32(rSpan);
      switch (chunkContentType)
      {
      case uint32_t(ChunkType::NineSlices):
        return ChunkType::NineSlices;
      default:
        throw NotSupportedException(fmt::format("Unsupported chunk content type: ", chunkContentType));
      }
    }


    Rectangle ReadRectangle(ReadOnlySpan<uint8_t>& rSpan)
    {
      const int32_t srcRectX = ValueCompression::ReadSimpleInt32(rSpan);
      const int32_t srcRectY = ValueCompression::ReadSimpleInt32(rSpan);
      const uint32_t srcRectWidth = ValueCompression::ReadSimpleUInt32(rSpan);
      const uint32_t srcRectHeight = ValueCompression::ReadSimpleUInt32(rSpan);

      if (srcRectWidth > static_cast<uint32_t>(std::numeric_limits<int32_t>::max()) ||
          srcRectHeight > static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
      {
        throw NotSupportedException("The rectangle is of a unsupported size");
      }
      return {srcRectX, srcRectY, UncheckedNumericCast<int32_t>(srcRectWidth), UncheckedNumericCast<int32_t>(srcRectHeight)};
    }

    PxRectangleU ReadRectangleU(ReadOnlySpan<uint8_t>& rSpan)
    {
      return {ValueCompression::ReadSimpleUInt32(rSpan), ValueCompression::ReadSimpleUInt32(rSpan), ValueCompression::ReadSimpleUInt32(rSpan),
              ValueCompression::ReadSimpleUInt32(rSpan)};
    }


    PxThicknessU ReadThicknessU(ReadOnlySpan<uint8_t>& rSpan)
    {
      return {ValueCompression::ReadSimpleUInt32(rSpan), ValueCompression::ReadSimpleUInt32(rSpan), ValueCompression::ReadSimpleUInt32(rSpan),
              ValueCompression::ReadSimpleUInt32(rSpan)};
    }

    IO::Path ReadPath(ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t stringLength = ValueCompression::ReadSimpleUInt32(rSpan);
      if (rSpan.length() < stringLength)
      {
        throw FormatException("Could not read a string of the expected length");
      }

      StringViewLite result(reinterpret_cast<const char* const>(rSpan.data()), stringLength);
      rSpan = rSpan.subspan(stringLength);
      return result;
    }

    IO::Path ReconstructPath(const IO::Path& path, const uint32_t parentPathIndex, const std::vector<EncodedPath>& paths)
    {
      if (parentPathIndex <= 0)
      {
        return path;
      }
      IO::Path finalPath(path);

      auto currentParentPathIndex = parentPathIndex;
      uint32_t parentCount = 0;
      while (currentParentPathIndex > 0)
      {
        auto index = currentParentPathIndex - 1;
        if (index >= paths.size())
        {
          throw IndexOutOfRangeException("parent path index out of range");
        }
        finalPath = IO::Path::Combine(paths[index].Path, finalPath);
        currentParentPathIndex = paths[index].ParentPathIndex;
        ++parentCount;
        if (parentCount >= BTAFormat::MAX_PARENT_COUNT)
        {
          throw NotSupportedException("path parent count exceeded limit");
        }
      }
      return finalPath;
    }


    void SetLegacyEntry(BasicTextureAtlas& rTextureAtlas, const int32_t index, const Rectangle& srcRect, const Rectangle& trimmedRect, IO::Path path)
    {
      if (trimmedRect.X() < 0 || trimmedRect.Y() < 0 || trimmedRect.Width() < 0 || trimmedRect.Height() < 0)
      {
        throw NotSupportedException("trimmed rectangle can not contain negative values");
      }

      auto trimLeft = trimmedRect.Left() - srcRect.Left();
      auto trimTop = trimmedRect.Top() - srcRect.Top();
      auto trimRight = srcRect.Right() - trimmedRect.Right();
      auto trimBottom = srcRect.Bottom() - trimmedRect.Bottom();
      if (trimLeft < 0 || trimTop < 0 || trimRight < 0 || trimBottom < 0)
      {
        throw NotSupportedException("srcRect must fully contain the trimmed rectangle");
      }

      const auto rectanglePx = TypeConverter::UncheckedTo<PxRectangleU>(trimmedRect);

      PxThicknessU trimPx(static_cast<uint32_t>(trimLeft), static_cast<uint32_t>(trimTop), static_cast<uint32_t>(trimRight),
                          static_cast<uint32_t>(trimBottom));

      rTextureAtlas.SetEntry(index, rectanglePx, trimPx, BTAFormat::DEFAULT_DP, std::move(path));
    }

    void ReadBTA1Entries(BasicTextureAtlas& rTextureAtlas, std::ifstream& rStream, const uint32_t contentSize)
    {
      std::vector<uint8_t> content(contentSize);
      StreamRead(rStream, content.data(), content.size());

      auto contentSpan = ReadOnlySpanUtil::AsSpan(content);

      const uint32_t numEntries = ValueCompression::ReadSimpleUInt32(contentSpan);
      rTextureAtlas.Reset(numEntries);

      // Read the entries
      for (uint32_t index = 0; index < numEntries; ++index)
      {
        Rectangle srcRectangle = ReadRectangle(contentSpan);
        Rectangle trimmedRectangle = ReadRectangle(contentSpan);
        IO::Path pathName = ReadPath(contentSpan);
        SetLegacyEntry(rTextureAtlas, index, srcRectangle, trimmedRectangle, std::move(pathName));
      }
    }

    std::vector<EncodedPath> ReadBTAPathEntries(ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t entryCount = ValueCompression::ReadSimpleUInt32(rSpan);
      std::vector<EncodedPath> entries(entryCount);

      // Read the strings
      for (uint32_t i = 0; i < entryCount; ++i)
      {
        entries[i].ParentPathIndex = ValueCompression::ReadSimpleUInt32(rSpan);
        entries[i].Path = ReadPath(rSpan);
      }
      return entries;
    }


    void ReadBTA2AtlasEntries(BasicTextureAtlas& rTextureAtlas, const std::vector<EncodedPath>& paths, ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t entryCount = ValueCompression::ReadSimpleUInt32(rSpan);

      // Prepare the atlas
      rTextureAtlas.Reset(entryCount);

      BTA2AtlasEntry entry;
      for (uint32_t i = 0; i < entryCount; ++i)
      {
        // Parse the content
        entry.SrcRectangle = ReadRectangle(rSpan);
        entry.TrimmedRectangle = ReadRectangle(rSpan);
        entry.ParentPathIndex = ValueCompression::ReadSimpleUInt32(rSpan);
        entry.Path = ReadPath(rSpan);

        auto finalPath = ReconstructPath(entry.Path, entry.ParentPathIndex, paths);
        SetLegacyEntry(rTextureAtlas, i, entry.SrcRectangle, entry.TrimmedRectangle, std::move(finalPath));
      }
    }


    void ReadBTA3AtlasEntries(BasicTextureAtlas& rTextureAtlas, const std::vector<EncodedPath>& paths, ReadOnlySpan<uint8_t>& rSpan)
    {
      const uint32_t entryCount = ValueCompression::ReadSimpleUInt32(rSpan);

      // Prepare the atlas
      rTextureAtlas.Reset(entryCount);

      BTA3AtlasEntry entry;
      for (uint32_t i = 0; i < entryCount; ++i)
      {
        // Parse the content
        entry.RectanglePx = ReadRectangleU(rSpan);
        entry.TrimPx = ReadThicknessU(rSpan);
        entry.Dpi = ValueCompression::ReadSimpleUInt32(rSpan);
        entry.ParentPathIndex = ValueCompression::ReadSimpleUInt32(rSpan);
        entry.Path = ReadPath(rSpan);

        auto finalPath = ReconstructPath(entry.Path, entry.ParentPathIndex, paths);
        rTextureAtlas.SetEntry(i, entry.RectanglePx, entry.TrimPx, entry.Dpi, std::move(finalPath));
      }
    }


    void ReadBTA2Entries(BasicTextureAtlas& rTextureAtlas, std::ifstream& rStream, const uint32_t contentSize)
    {
      std::vector<uint8_t> content(contentSize);
      StreamRead(rStream, content.data(), content.size());

      auto contentSpan = ReadOnlySpanUtil::AsSpan(content);
      auto pathEntries = ReadBTAPathEntries(contentSpan);
      ReadBTA2AtlasEntries(rTextureAtlas, pathEntries, contentSpan);
    }


    void ReadBTA3Entries(BasicTextureAtlas& rTextureAtlas, std::ifstream& rStream, const uint32_t contentSize)
    {
      std::vector<uint8_t> content(contentSize);
      StreamRead(rStream, content.data(), content.size());

      auto contentSpan = ReadOnlySpanUtil::AsSpan(content);
      auto pathEntries = ReadBTAPathEntries(contentSpan);
      ReadBTA3AtlasEntries(rTextureAtlas, pathEntries, contentSpan);
    }

    Optional<MinimalChunkHeader> TryReadMinimalChunkHeader(std::ifstream& rStream)
    {
      // Try to read the header
      std::array<uint8_t, BTAFormat::Chunk::HeaderSize> header{};
      if (!TryStreamRead(rStream, header.data(), header.size()))
      {
        return {};
      }
      const auto headerSpan = ReadOnlySpanUtil::AsSpan(header);
      MinimalChunkHeader minimalHeader;
      minimalHeader.Magic = ByteSpanUtil::ReadUInt32LE(headerSpan, BTAFormat::Chunk::OffsetMagic);
      minimalHeader.Size = ByteSpanUtil::ReadUInt32LE(headerSpan, BTAFormat::Chunk::OffsetSize);
      return Optional<MinimalChunkHeader>(minimalHeader);
    }


    void ProcessNineSliceChunk(BasicTextureAtlas& rTextureAtlas, ReadOnlySpan<uint8_t>& rSpan, const uint32_t chunkVersion)
    {
      if (chunkVersion != BTAFormat::CHUNKTYPE_NINESLICE_VERSION)
      {
        throw NotSupportedException("Unsupported chunk version");
      }

      const uint32_t nineSliceEntries = ValueCompression::ReadSimpleUInt32(rSpan);
      for (uint32_t i = 0; i < nineSliceEntries; ++i)
      {
        // Read all the nine-slice entries
        auto nineSlice = ReadThicknessU(rSpan);
        auto contentMargin = ReadThicknessU(rSpan);
        const uint32_t textureIndex = ValueCompression::ReadSimpleUInt32(rSpan);
        rTextureAtlas.AddNineSlice(textureIndex, nineSlice, contentMargin);
      }

      if (!rSpan.empty())
      {
        throw FormatException("The nine-slice chunk was not of the expected format");
      }
    }

    bool TryReadBTA4Chunk(BasicTextureAtlas& rTextureAtlas, std::ifstream& rStream)
    {
      // Try to read the chunk header
      Optional<MinimalChunkHeader> chunkHeader = TryReadMinimalChunkHeader(rStream);
      if (!chunkHeader.HasValue())
      {
        return false;
      }
      if (chunkHeader.Value().Magic != BTAFormat::Chunk::HeaderMagicValue)
      {
        throw FormatException("Chunk not of the expected format");
      }
      if (chunkHeader.Value().Size < BTAFormat::Chunk::HeaderSize)
      {
        throw FormatException("Invalid chunk");
      }

      // Read the remaining chunk content
      const uint32_t chunkContentSize = chunkHeader.Value().Size - BTAFormat::Chunk::HeaderSize;
      std::vector<uint8_t> chunkContent(chunkContentSize);
      StreamRead(rStream, chunkContent.data(), chunkContent.size());

      auto chunkContentSpan = ReadOnlySpanUtil::AsSpan(chunkContent);

      // Just after the basic chunk header, there is a extended chunk header
      // ChunkType = EncodedUInt32
      // ChunkVersion = EncodedUInt32

      const ChunkType chunkType = ReadChunkType(chunkContentSpan);
      const uint32_t chunkVersion = ValueCompression::ReadSimpleUInt32(chunkContentSpan);
      switch (chunkType)
      {
      case ChunkType::NineSlices:
        ProcessNineSliceChunk(rTextureAtlas, chunkContentSpan, chunkVersion);
        break;
      default:
        throw NotSupportedException(fmt::format("Unsupported chunk content type: ", uint32_t(chunkType)));
      }
      return true;
    }


    void ReadBTA4OptionalChunks(BasicTextureAtlas& rTextureAtlas, std::ifstream& rStream)
    {
      FSLLOG3_VERBOSE5("Trying to read a optional chunk");
      while (TryReadBTA4Chunk(rTextureAtlas, rStream))
      {
        FSLLOG3_VERBOSE5("Trying to read another optional BTA chunk");
      }
    }

  }


  void BinaryTextureAtlasLoader::Load(BasicTextureAtlas& rTextureAtlas, const IO::Path& strFilename)
  {
    std::ifstream file(PATH_GET_NAME(strFilename), std::ios::in | std::ios::binary);
    if (file.good())
    {
      Load(rTextureAtlas, file);
    }
    else
    {
      throw FormatException(fmt::format("File not found: '{}'", strFilename));
    }
  }


  void BinaryTextureAtlasLoader::Load(BasicTextureAtlas& rTextureAtlas, std::ifstream& rStream)
  {
    const BTAHeader header = ReadAndValidateHeader(rStream);
    switch (header.Version)
    {
    case BTAFormat::BTA_VERSION1:
      ReadBTA1Entries(rTextureAtlas, rStream, header.Size);
      break;
    case BTAFormat::BTA_VERSION2:
      ReadBTA2Entries(rTextureAtlas, rStream, header.Size);
      break;
    case BTAFormat::BTA_VERSION3:
      ReadBTA3Entries(rTextureAtlas, rStream, header.Size);
      break;
    case BTAFormat::BTA_VERSION4:
      ReadBTA3Entries(rTextureAtlas, rStream, header.Size);
      ReadBTA4OptionalChunks(rTextureAtlas, rStream);
      break;
    default:
      throw NotSupportedException("BTA format not supported");
    }
  }
}
