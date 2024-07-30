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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/System/Platform/PlatformPathTransform.hpp>
#include <FslGraphics/Vertices/IndexConverter.hpp>
#include <FslGraphics/Vertices/VertexConverter.hpp>
#include <FslGraphics3D/SceneFormat/BasicSceneFormat.hpp>
#include <FslGraphics3D/SceneFormat/ChunkType.hpp>
#include <FslGraphics3D/SceneFormat/PrimitiveType.hpp>
#include <FslGraphics3D/SceneFormat/VertexElementFormat.hpp>
#include <FslGraphics3D/SceneFormat/VertexElementUsage.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <fstream>
#include <limits>
#include <utility>
#include <vector>
#include "Conversion.hpp"
#include "SFVertexElement.hpp"

//! workaround for GCC complaint
#ifdef __GNUC__
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define REMOVE_WARNING_VARIABLE_IS_NOT_USED __attribute__((unused))
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define REMOVE_WARNING_VARIABLE_IS_NOT_USED
#endif

//! The load/save routines ought to work on most endian type machines.
//! However we do require:
//! - sizeof(float) == 4
//! - endianess of float and uint32_t must be equal
//! We verify this at runtime!
//!
//! WARNING:
//! - The mesh conversion process has not been optimized at all.
//! - Since there are no standard compiler flags containing endian information the code here uses some very basic endian assumptions that are
//! verified at runtime.

namespace Fsl::SceneFormat
{
  using namespace Graphics3D;

  struct InternalMeshRecord
  {
    std::shared_ptr<Graphics3D::Mesh> SourceMesh;
    uint32_t IndexByteSize;
    uint8_t VertexDeclarationIndex;


    InternalMeshRecord(const std::shared_ptr<Graphics3D::Mesh>& sourceMesh, const uint8_t vertexDeclarationIndex)
      : SourceMesh(sourceMesh)
      , IndexByteSize(sourceMesh->GenericDirectAccess().IndexStride)
      , VertexDeclarationIndex(vertexDeclarationIndex)
    {
      if (IndexByteSize != 1 && IndexByteSize != 2)
      {
        throw NotSupportedException("Index stride is not supported");
      }
    }
  };

  struct InternalVertexDeclaration : public SFVertexDeclaration
  {
    uint32_t VertexByteSize{0};

    InternalVertexDeclaration() = default;

    explicit InternalVertexDeclaration(const SFVertexDeclaration& declaration)
      : SFVertexDeclaration(declaration)
    {
      for (auto itr = declaration.Elements.begin(); itr != declaration.Elements.end(); ++itr)
      {
        VertexByteSize += CalcByteSize(*itr);
      }
    }

    static uint32_t CalcByteSize(const SFVertexElement& element)
    {
      switch (element.Format)
      {
      case SceneFormat::VertexElementFormat::Single:
        return 4;
      case SceneFormat::VertexElementFormat::Vector2:
        return 4 * 2;
      case SceneFormat::VertexElementFormat::Vector3:
        return 4 * 3;
      case SceneFormat::VertexElementFormat::Vector4:
        return 4 * 4;
      default:
        throw NotSupportedException("unsupported vertex element format");
      }
    }
  };

  class InternalSceneRecord
  {
  public:
    std::deque<InternalVertexDeclaration> VertexDeclarations;
    std::deque<InternalMeshRecord> Meshes;

    void Clear()
    {
      VertexDeclarations.clear();
      Meshes.clear();
    }
  };


  namespace
  {
    // FSF
    constexpr uint32_t FormatMagic = 0x00465346;
    constexpr uint32_t FormatCurrentVersion = 0;

    constexpr uint16_t ChunkVersionVertexDeclaration = 0;
    constexpr uint16_t ChunkVersionMeshes = 0;
    constexpr uint16_t ChunkVersionNodes = 0;


    constexpr uint32_t FormatheaderOffsetMagic = 0;
    constexpr uint32_t FormatheaderOffsetVersion = FormatheaderOffsetMagic + sizeof(uint32_t);
    constexpr uint32_t SizeOfFormatheader = FormatheaderOffsetVersion + sizeof(uint32_t);

    // Basic format header
    struct FormatHeader
    {
      uint32_t Magic;
      uint32_t Version;

      FormatHeader()
        : Magic(FormatMagic)
        , Version(FormatCurrentVersion)
      {
      }

      FormatHeader(const uint32_t magic, const uint32_t version)
        : Magic(magic)
        , Version(version)
      {
      }
    };

    constexpr uint32_t ChunkheaderOffsetByteSize = 0;
    constexpr uint32_t ChunkheaderOffsetType = ChunkheaderOffsetByteSize + sizeof(uint32_t);
    constexpr uint32_t ChunkheaderOffsetReserved = ChunkheaderOffsetType + sizeof(uint8_t);
    constexpr uint32_t ChunkheaderOffsetVersion = ChunkheaderOffsetReserved + sizeof(uint8_t);
    constexpr uint32_t SizeOfChunkheader = ChunkheaderOffsetVersion + sizeof(uint16_t);

    struct ChunkHeader
    {
      // The byte size excluding the header
      uint32_t ByteSize{0};
      // The type of the chunk
      ChunkType Type{ChunkType::VertexDeclarations};    // uint8_t
      uint8_t Reserved{0};
      uint16_t Version{0};

      ChunkHeader() = default;

      ChunkHeader(const uint32_t byteSize, const ChunkType chunkType, const uint16_t version)
        : ByteSize(byteSize)
        , Type(chunkType)
        , Version(version)
      {
      }
    };

    // int32_t Offset;
    // VertexElementFormat Format;
    // VertexElementUsage Usage;
    // int32_t UsageIndex;

    constexpr uint32_t SizeofVertexDeclarationListHeader = 4;

    // struct VertexDeclarations
    //{
    //  uint32_t Entries;
    //};

    constexpr uint32_t SizeofVertexDeclarationHeader = 2;

    // struct VertexDeclaration
    //{
    //  uint16_t ElementCount;
    //};

    constexpr uint32_t VertexelementOffsetFormat = 0;
    constexpr uint32_t VertexelementOffsetUsage = VertexelementOffsetFormat + sizeof(uint8_t);
    constexpr uint32_t VertexelementOffsetUsageIndex = VertexelementOffsetUsage + sizeof(uint8_t);
    constexpr uint32_t SizeOfVertexelement = VertexelementOffsetUsageIndex + sizeof(uint8_t);

    constexpr uint32_t BufferEntrySize1 =
      SizeofVertexDeclarationListHeader >= SizeofVertexDeclarationHeader ? SizeofVertexDeclarationListHeader : SizeofVertexDeclarationHeader;
    constexpr uint32_t BufferEntrySize2 = BufferEntrySize1 >= SizeOfVertexelement ? BufferEntrySize1 : SizeOfVertexelement;
    constexpr uint32_t BufferEntrySize = BufferEntrySize2;
    // struct ChunkVertexElement
    //{
    //  uint8_t Format;         // FS3::VertexElementFormat
    //  uint8_t Usage;          // FS3::VertexElementUsage
    //  uint8_t UsageIndex;
    //};

    constexpr uint32_t SizeofMeshListHeader = 4;

    // struct MeshList
    //{
    //  uint32_t Entries;
    //};

    constexpr uint32_t MeshOffsetMaterialIndex = 0;
    constexpr uint32_t MeshOffsetVertexCount = MeshOffsetMaterialIndex + sizeof(uint32_t);
    constexpr uint32_t MeshOffsetIndexCount = MeshOffsetVertexCount + sizeof(uint32_t);
    constexpr uint32_t MeshOffsetNameLength = MeshOffsetIndexCount + sizeof(uint32_t);
    constexpr uint32_t MeshOffsetPrimitiveType = MeshOffsetNameLength + sizeof(uint32_t);
    constexpr uint32_t MeshOffsetVertexDeclarationIndex = MeshOffsetPrimitiveType + sizeof(uint8_t);
    constexpr uint32_t MeshOffsetIndexByteSize = MeshOffsetVertexDeclarationIndex + sizeof(uint8_t);
    constexpr uint32_t SizeofMeshHeader = MeshOffsetIndexByteSize + sizeof(uint8_t);

    constexpr uint32_t MeshHeaders = (SizeofMeshHeader > SizeofMeshListHeader ? SizeofMeshHeader : SizeofMeshListHeader);

    // struct ChunkMeshHeader
    //{
    //  uint32_t MaterialIndex;
    //  uint32_t VertexCount;
    //  uint32_t IndexCount;
    //  uint32_t NameLength;
    //  uint8_t PrimitiveType;
    //  uint8_t VertexDeclarationIndex;
    //  uint8_t IndexByteSize;
    //};

    constexpr uint32_t SizeofNodelistHeader = sizeof(uint32_t);

    constexpr uint16_t NodeOffsetTransformation = 0;
    constexpr uint16_t NodeOffsetNameLength = NodeOffsetTransformation + (4 * 4 * 4);
    constexpr uint16_t NodeOffsetMeshCount = NodeOffsetNameLength + sizeof(uint16_t);
    constexpr uint16_t NodeOffsetChildCount = NodeOffsetMeshCount + sizeof(uint8_t);
    constexpr uint16_t SizeofNodeHeader = NodeOffsetChildCount + sizeof(uint8_t);

    // struct ChunkNodeHeader
    //{
    //  Matrix Transformation;
    //  uint16_t NameLength;
    //  uint8_t MeshCount;
    //  uint8_t ChildCount;
    //};


    FormatHeader ReadHeader(std::ifstream& rStream)
    {
      std::array<uint8_t, SizeOfFormatheader> buffer{};
      rStream.read(reinterpret_cast<char*>(buffer.data()), SizeOfFormatheader);
      if (!rStream.good())
      {
        throw FormatException("Failed to read the expected data");
      }

      const uint32_t magic = ByteArrayUtil::ReadUInt32LE(buffer.data(), buffer.size(), FormatheaderOffsetMagic);
      const uint32_t version = ByteArrayUtil::ReadUInt32LE(buffer.data(), buffer.size(), FormatheaderOffsetVersion);

      if (magic != FormatMagic || version != FormatCurrentVersion)
      {
        throw NotSupportedException("File format not supported");
      }
      return {magic, version};
    }


    void WriteHeader(std::ofstream& rStream, const FormatHeader& header)
    {
      std::array<uint8_t, SizeOfFormatheader> buffer{};
      ByteArrayUtil::WriteUInt32LE(buffer.data(), buffer.size(), FormatheaderOffsetMagic, header.Magic);
      ByteArrayUtil::WriteUInt32LE(buffer.data(), buffer.size(), FormatheaderOffsetVersion, header.Version);
      rStream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    }


    ChunkHeader ReadChunkHeader(std::ifstream& rStream)
    {
      std::array<uint8_t, SizeOfChunkheader> buffer{};
      rStream.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
      if (!rStream.good())
      {
        throw FormatException("Failed to read the expected data");
      }

      const uint32_t byteSize = ByteArrayUtil::ReadUInt32LE(buffer.data(), buffer.size(), ChunkheaderOffsetByteSize);
      const uint8_t chunkType = ByteArrayUtil::ReadUInt8LE(buffer.data(), buffer.size(), ChunkheaderOffsetType);
      // const uint8_t reserved = ByteArrayUtil::ReadUInt8LE(buffer.data(), buffer.size(), CHUNKHEADER_OFFSET_Reserved);
      const uint16_t version = ByteArrayUtil::ReadUInt16LE(buffer.data(), buffer.size(), ChunkheaderOffsetVersion);
      return {byteSize, static_cast<ChunkType>(chunkType), version};
    }


    void WriteChunkHeader(std::ofstream& rStream, const ChunkHeader& header)
    {
      std::array<uint8_t, SizeOfChunkheader> buffer{};
      ByteArrayUtil::WriteUInt32LE(buffer.data(), buffer.size(), ChunkheaderOffsetByteSize, header.ByteSize);
      ByteArrayUtil::WriteUInt8LE(buffer.data(), buffer.size(), ChunkheaderOffsetType, static_cast<uint8_t>(header.Type));
      ByteArrayUtil::WriteUInt8LE(buffer.data(), buffer.size(), ChunkheaderOffsetReserved, header.Reserved);
      ByteArrayUtil::WriteUInt16LE(buffer.data(), buffer.size(), ChunkheaderOffsetVersion, header.Version);
      rStream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    }


    void ExtractUniqueVertexDeclarations(InternalSceneRecord& rSceneRecord, const Scene& scene)
    {
      int32_t count = scene.GetMeshCount();
      std::shared_ptr<Mesh> mesh;

      auto& rMeshes = rSceneRecord.Meshes;
      auto& rVertexDeclarations = rSceneRecord.VertexDeclarations;

      for (int32_t i = 0; i < count; ++i)
      {
        mesh = scene.GetMeshAt(i);
        assert(mesh);
        const SFVertexDeclaration vertexDeclaration = Conversion::Convert(mesh->AsVertexDeclarationSpan());
        const auto itrFind = std::find(rVertexDeclarations.begin(), rVertexDeclarations.end(), vertexDeclaration);
        const auto vertexDeclarationIndex = std::distance(rVertexDeclarations.begin(), itrFind);
        if (itrFind == rVertexDeclarations.end())
        {
          rVertexDeclarations.emplace_back(vertexDeclaration);
        }

        if (vertexDeclarationIndex > 255)
        {
          throw NotSupportedException("The maximum number of vertex declarations exceeded");
        }

        rMeshes.emplace_back(mesh, static_cast<uint8_t>(vertexDeclarationIndex));
      }
    }


    uint32_t CalcByteSize(const std::deque<InternalVertexDeclaration>& vertexDeclarations)
    {
      uint32_t size = SizeofVertexDeclarationListHeader;

      auto itrDecl = vertexDeclarations.begin();
      while (itrDecl != vertexDeclarations.end())
      {
        // Write the vertex element count
        assert(itrDecl->Elements.size() <= std::numeric_limits<uint16_t>::max());
        const auto elementCount = static_cast<uint16_t>(itrDecl->Elements.size());
        size += SizeofVertexDeclarationHeader + (elementCount * SizeOfVertexelement);
        ++itrDecl;
      }
      return size;
    }


    SceneFormat::VertexElementFormat ConvertToFormat(const uint8_t value)
    {
      if (value <= static_cast<uint32_t>(SceneFormat::VertexElementFormat::SYS_COUNT))
      {
        return static_cast<SceneFormat::VertexElementFormat>(value);
      }
      throw FormatException("invalid vertex element format");
    }


    SceneFormat::VertexElementUsage ConvertToUsage(const uint8_t value)
    {
      if (value <= static_cast<uint32_t>(SceneFormat::VertexElementUsage::SYS_COUNT))
      {
        return static_cast<SceneFormat::VertexElementUsage>(value);
      }
      throw FormatException("invalid vertex element usage");
    }


    SceneFormat::PrimitiveType ConvertToPrimitiveType(const uint8_t value)
    {
      if (value <= static_cast<uint32_t>(SceneFormat::PrimitiveType::SYS_COUNT))
      {
        return static_cast<SceneFormat::PrimitiveType>(value);
      }
      throw FormatException("invalid primitive type");
    }


    //! @brief Read the unique vertex declarations
    void ReadVertexDeclarationsChunk(std::ifstream& rStream, std::deque<InternalVertexDeclaration>& rUniqueEntries)
    {
      const ChunkHeader header = ReadChunkHeader(rStream);
      if (header.Type != ChunkType::VertexDeclarations)
      {
        throw NotSupportedException("format not correct");
      }
      if (header.Version != ChunkVersionVertexDeclaration)
      {
        throw FormatException("Unsupported vertex declaration chunk version");
      }

      const auto startPos = rStream.tellg();

      // Read the vertex declarations
      std::array<uint8_t, BufferEntrySize> buffer{};
      rStream.read(reinterpret_cast<char*>(buffer.data()), SizeofVertexDeclarationListHeader);
      if (!rStream.good())
      {
        throw FormatException("Failed to read the expected data");
      }

      // Read the number of vertex declarations
      const uint32_t numVertexDeclarations = ByteArrayUtil::ReadUInt32LE(buffer.data(), buffer.size(), 0);

      for (uint32_t declarationIndex = 0; declarationIndex < numVertexDeclarations; ++declarationIndex)
      {
        SFVertexDeclaration vertexDecl;

        rStream.read(reinterpret_cast<char*>(buffer.data()), SizeofVertexDeclarationHeader);
        if (!rStream.good())
        {
          throw FormatException("Failed to read the expected data");
        }

        const uint16_t elementCount = ByteArrayUtil::ReadUInt16LE(buffer.data(), buffer.size(), 0);
        for (uint16_t elementIndex = 0; elementIndex < elementCount; ++elementIndex)
        {
          rStream.read(reinterpret_cast<char*>(buffer.data()), SizeOfVertexelement);
          if (!rStream.good())
          {
            throw FormatException("Failed to read the expected data");
          }

          const uint8_t format = ByteArrayUtil::ReadUInt8LE(buffer.data(), buffer.size(), VertexelementOffsetFormat);
          const uint8_t usage = ByteArrayUtil::ReadUInt8LE(buffer.data(), buffer.size(), VertexelementOffsetUsage);
          const uint8_t usageIndex = ByteArrayUtil::ReadUInt8LE(buffer.data(), buffer.size(), VertexelementOffsetUsageIndex);

          vertexDecl.Elements.emplace_back(ConvertToFormat(format), ConvertToUsage(usage), usageIndex);
        }

        rUniqueEntries.emplace_back(vertexDecl);
      }

      const auto endPos = rStream.tellg();
      assert(endPos >= startPos);
      if (static_cast<uint32_t>(endPos - startPos) != header.ByteSize)
      {
        throw FormatException("VertexDeclarationChunk was of a unexpected size");
      }
    }


    //! @brief Write the unique vertex declarations
    void WriteVertexDeclarationsChunk(std::ofstream& rStream, const std::deque<InternalVertexDeclaration>& uniqueEntries)
    {
      if (uniqueEntries.size() > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("Too many unique entries");
      }

      const uint32_t cbContent = CalcByteSize(uniqueEntries);

      ChunkHeader header(cbContent, ChunkType::VertexDeclarations, ChunkVersionVertexDeclaration);
      WriteChunkHeader(rStream, header);

      // Write the vertex declarations
      std::array<uint8_t, BufferEntrySize> buffer{};
      // Write the number of vertex declarations
      ByteArrayUtil::WriteUInt32LE(buffer.data(), buffer.size(), 0, static_cast<uint32_t>(uniqueEntries.size()));
      rStream.write(reinterpret_cast<const char*>(buffer.data()), SizeofVertexDeclarationListHeader);

      auto itrDecl = uniqueEntries.begin();
      while (itrDecl != uniqueEntries.end())
      {
        // Write the vertex element count
        assert(itrDecl->Elements.size() <= std::numeric_limits<uint16_t>::max());
        const auto elementCount = static_cast<uint16_t>(itrDecl->Elements.size());
        ByteArrayUtil::WriteUInt16LE(buffer.data(), buffer.size(), 0, elementCount);
        rStream.write(reinterpret_cast<const char*>(buffer.data()), SizeofVertexDeclarationHeader);

        for (std::size_t i = 0; i < elementCount; ++i)
        {
          assert(static_cast<uint32_t>(itrDecl->Elements[i].Format) <= 255);
          assert(static_cast<uint32_t>(itrDecl->Elements[i].Usage) <= 255);
          // assert(itrDecl->Elements[i].UsageIndex >= 0 && itrDecl->Elements[i].UsageIndex <= 255);

          const auto format = static_cast<uint8_t>(itrDecl->Elements[i].Format);
          const auto usage = static_cast<uint8_t>(itrDecl->Elements[i].Usage);
          const uint8_t usageIndex = itrDecl->Elements[i].UsageIndex;

          ByteArrayUtil::WriteUInt8LE(buffer.data(), buffer.size(), VertexelementOffsetFormat, format);
          ByteArrayUtil::WriteUInt8LE(buffer.data(), buffer.size(), VertexelementOffsetUsage, usage);
          ByteArrayUtil::WriteUInt8LE(buffer.data(), buffer.size(), VertexelementOffsetUsageIndex, usageIndex);
          rStream.write(reinterpret_cast<const char*>(buffer.data()), SizeOfVertexelement);
        }

        ++itrDecl;
      }

      // TODO: Decide if we want to pad this chunk with zeros so that each chunk starts at a 32bit aligned address
    }


    //! @brief Examine the meshes in the scene and calculate the exact byte size needed to store the mesh chunk (without the chunk header)
    uint32_t CalcMeshesSize(const InternalSceneRecord& scene)
    {
      uint32_t cbMeshes = SizeofMeshListHeader;

      const auto& vertexDeclarations = scene.VertexDeclarations;
      const auto& meshes = scene.Meshes;

      for (auto itr = meshes.begin(); itr != meshes.end(); ++itr)
      {
        assert(itr->SourceMesh);
        const auto& mesh = itr->SourceMesh;
        cbMeshes += SizeofMeshHeader;
        cbMeshes += vertexDeclarations[itr->VertexDeclarationIndex].VertexByteSize * mesh->GetVertexCount();
        cbMeshes += itr->IndexByteSize * mesh->GetIndexCount();
        cbMeshes += mesh->GetName().GetByteSize() + 1;    // +1 because we write the terminating zero as well
      }
      return cbMeshes;
    }


    void ConvertFloat1ArrayLE(uint8_t* const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset,
                              [[maybe_unused]] const std::size_t entries)
    {
      const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4;
      assert(pSrcArray != nullptr);
      assert((srcInterleaveOffset + elementSize) <= srcStride);
      assert((entries * srcStride) <= cbSrc);

      uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
      const uint8_t* const pSrcEnd = pSrcArray + cbSrc;

      std::size_t currentSrcIndex = srcInterleaveOffset;
      while (pSrc < pSrcEnd)
      {
        assert((currentSrcIndex + elementSize) <= cbSrc);

        // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
        //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
        *reinterpret_cast<uint32_t*>(pSrc) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex);
        pSrc += srcStride;
        currentSrcIndex += srcStride;
      }
    }


    void WriteFloat1ArrayLE(uint8_t* pDst, const std::size_t cbDst, const std::size_t dstIndex, const std::size_t dstStride,
                            const std::size_t dstInterleaveOffset, const uint8_t* const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride,
                            const std::size_t srcInterleaveOffset, [[maybe_unused]] const std::size_t entries)
    {
      const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4;
      assert(pDst != nullptr);
      assert((dstInterleaveOffset + elementSize) <= dstStride);
      assert(dstIndex < cbDst || (dstIndex == cbDst && entries == 0));
      assert((dstIndex + (entries * dstStride)) <= cbDst);
      assert(pSrcArray != nullptr);
      assert((srcInterleaveOffset + elementSize) <= srcStride);
      assert((entries * srcStride) <= cbSrc);

      const uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
      const uint8_t* const pSrcEnd = pSrcArray + cbSrc;

      std::size_t currentDstIndex = dstIndex + dstInterleaveOffset;
      while (pSrc < pSrcEnd)
      {
        assert((pSrc + elementSize) <= pSrcEnd);
        assert((currentDstIndex + elementSize) <= cbDst);
        // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
        //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex, *reinterpret_cast<const uint32_t*>(pSrc));
        pSrc += srcStride;
        currentDstIndex += dstStride;
      }
    }


    void ConvertFloat2ArrayLE(uint8_t* const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset,
                              [[maybe_unused]] const std::size_t entries)
    {
      const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 2;
      assert(pSrcArray != nullptr);
      assert((srcInterleaveOffset + elementSize) <= srcStride);
      assert((entries * srcStride) <= cbSrc);

      uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
      const uint8_t* const pSrcEnd = pSrcArray + cbSrc;

      std::size_t currentSrcIndex = srcInterleaveOffset;
      while (pSrc < pSrcEnd)
      {
        assert((currentSrcIndex + elementSize) <= cbSrc);

        // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
        //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
        *reinterpret_cast<uint32_t*>(pSrc) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex);
        *reinterpret_cast<uint32_t*>(pSrc + 4) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex + 4);
        pSrc += srcStride;
        currentSrcIndex += srcStride;
      }
    }


    void WriteFloat2ArrayLE(uint8_t* pDst, const std::size_t cbDst, const std::size_t dstIndex, const std::size_t dstStride,
                            const std::size_t dstInterleaveOffset, const uint8_t* const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride,
                            const std::size_t srcInterleaveOffset, [[maybe_unused]] const std::size_t entries)
    {
      const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 2;
      assert(pDst != nullptr);
      assert((dstInterleaveOffset + elementSize) <= dstStride);
      assert(dstIndex < cbDst || (dstIndex == cbDst && entries == 0));
      assert((dstIndex + (entries * dstStride)) <= cbDst);
      assert(pSrcArray != nullptr);
      assert((srcInterleaveOffset + elementSize) <= srcStride);
      assert((entries * srcStride) <= cbSrc);

      const uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
      const uint8_t* const pSrcEnd = pSrcArray + cbSrc;

      std::size_t currentDstIndex = dstIndex + dstInterleaveOffset;
      while (pSrc < pSrcEnd)
      {
        assert((pSrc + elementSize) <= pSrcEnd);
        assert((currentDstIndex + elementSize) <= cbDst);
        // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
        //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex, *reinterpret_cast<const uint32_t*>(pSrc));
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex + 4, *reinterpret_cast<const uint32_t*>(pSrc + 4));
        pSrc += srcStride;
        currentDstIndex += dstStride;
      }
    }


    void ConvertFloat3ArrayLE(uint8_t* const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset,
                              [[maybe_unused]] const std::size_t entries)
    {
      const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 3;
      assert(pSrcArray != nullptr);
      assert((srcInterleaveOffset + elementSize) <= srcStride);
      assert((entries * srcStride) <= cbSrc);

      uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
      const uint8_t* const pSrcEnd = pSrcArray + cbSrc;

      std::size_t currentSrcIndex = srcInterleaveOffset;
      while (pSrc < pSrcEnd)
      {
        assert((currentSrcIndex + elementSize) <= cbSrc);

        // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
        //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
        *reinterpret_cast<uint32_t*>(pSrc) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex);
        *reinterpret_cast<uint32_t*>(pSrc + 4) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex + 4);
        *reinterpret_cast<uint32_t*>(pSrc + 8) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex + 8);
        pSrc += srcStride;
        currentSrcIndex += srcStride;
      }
    }


    void WriteFloat3ArrayLE(uint8_t* pDst, const std::size_t cbDst, const std::size_t dstIndex, const std::size_t dstStride,
                            const std::size_t dstInterleaveOffset, const uint8_t* const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride,
                            const std::size_t srcInterleaveOffset, [[maybe_unused]] const std::size_t entries)
    {
      const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 3;
      assert(pDst != nullptr);
      assert((dstInterleaveOffset + elementSize) <= dstStride);
      assert(dstIndex < cbDst || (dstIndex == cbDst && entries == 0));
      assert((dstIndex + (entries * dstStride)) <= cbDst);
      assert(pSrcArray != nullptr);
      assert((srcInterleaveOffset + elementSize) <= srcStride);
      assert((entries * srcStride) <= cbSrc);

      const uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
      const uint8_t* const pSrcEnd = pSrcArray + cbSrc;

      std::size_t currentDstIndex = dstIndex + dstInterleaveOffset;
      while (pSrc < pSrcEnd)
      {
        assert((currentDstIndex + 12) <= cbDst);
        // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
        //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex, *reinterpret_cast<const uint32_t*>(pSrc));
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex + 4, *reinterpret_cast<const uint32_t*>(pSrc + 4));
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex + 8, *reinterpret_cast<const uint32_t*>(pSrc + 8));
        pSrc += srcStride;
        currentDstIndex += dstStride;
      }
    }


    void ConvertFloat4ArrayLE(uint8_t* const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset,
                              [[maybe_unused]] const std::size_t entries)
    {
      const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 4;
      assert(pSrcArray != nullptr);
      assert((srcInterleaveOffset + elementSize) <= srcStride);
      assert((entries * srcStride) <= cbSrc);

      uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
      const uint8_t* const pSrcEnd = pSrcArray + cbSrc;

      std::size_t currentSrcIndex = srcInterleaveOffset;
      while (pSrc < pSrcEnd)
      {
        assert((currentSrcIndex + elementSize) <= cbSrc);

        // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
        //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
        *reinterpret_cast<uint32_t*>(pSrc) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex);
        *reinterpret_cast<uint32_t*>(pSrc + 4) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex + 4);
        *reinterpret_cast<uint32_t*>(pSrc + 8) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex + 8);
        *reinterpret_cast<uint32_t*>(pSrc + 12) = ByteArrayUtil::ReadUInt32LE(pSrcArray, cbSrc, currentSrcIndex + 12);
        pSrc += srcStride;
        currentSrcIndex += srcStride;
      }
    }


    void WriteFloat4ArrayLE(uint8_t* pDst, const std::size_t cbDst, const std::size_t dstIndex, const std::size_t dstStride,
                            const std::size_t dstInterleaveOffset, const uint8_t* const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride,
                            const std::size_t srcInterleaveOffset, [[maybe_unused]] const std::size_t entries)
    {
      const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 3;
      assert(pDst != nullptr);
      assert((dstInterleaveOffset + elementSize) <= dstStride);
      assert(dstIndex < cbDst || (dstIndex == cbDst && entries == 0));
      assert((dstIndex + (entries * dstStride)) <= cbDst);
      assert(pSrcArray != nullptr);
      assert((srcInterleaveOffset + elementSize) <= srcStride);
      assert((entries * srcStride) <= cbSrc);

      const uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
      const uint8_t* const pSrcEnd = pSrcArray + cbSrc;

      std::size_t currentDstIndex = dstIndex + dstInterleaveOffset;
      while (pSrc < pSrcEnd)
      {
        assert((currentDstIndex + 16) <= cbDst);
        // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
        //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex, *reinterpret_cast<const uint32_t*>(pSrc));
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex + 4, *reinterpret_cast<const uint32_t*>(pSrc + 4));
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex + 8, *reinterpret_cast<const uint32_t*>(pSrc + 8));
        ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex + 12, *reinterpret_cast<const uint32_t*>(pSrc + 12));
        pSrc += srcStride;
        currentDstIndex += dstStride;
      }
    }


    std::size_t LocateOffset(VertexDeclarationSpan vertexDeclaration, const SceneFormat::VertexElementUsage usage, const uint8_t usageIndex)
    {
      for (std::size_t i = 0; i < vertexDeclaration.Count(); ++i)
      {
        const VertexElement element = vertexDeclaration.At(i);
        if (Conversion::Convert(element.Usage) == usage && element.UsageIndex == usageIndex)
        {
          return element.Offset;
        }
      }
      throw NotFoundException("Could not locate the requested vertex element");
    }


    //! @brief Convert the vertices in place from little endian format into the host format
    void ConvertVerticesLE(uint8_t* const pSrc, const std::size_t srcLength, const InternalVertexDeclaration& srcVertexDeclaration,
                           const std::size_t vertexCount)
    {
      assert(pSrc != nullptr);

      const auto srcVertexStride = srcVertexDeclaration.VertexByteSize;

      std::size_t srcInterleaveOffset = 0;
      for (auto itr = srcVertexDeclaration.Elements.begin(); itr != srcVertexDeclaration.Elements.end(); ++itr)
      {
        switch (itr->Format)
        {
        case SceneFormat::VertexElementFormat::Single:
          ConvertFloat1ArrayLE(pSrc, srcLength, srcVertexStride, srcInterleaveOffset, vertexCount);
          srcInterleaveOffset += 4;
          break;
        case SceneFormat::VertexElementFormat::Vector2:
          ConvertFloat2ArrayLE(pSrc, srcLength, srcVertexStride, srcInterleaveOffset, vertexCount);
          srcInterleaveOffset += 4 * 2;
          break;
        case SceneFormat::VertexElementFormat::Vector3:
          ConvertFloat3ArrayLE(pSrc, srcLength, srcVertexStride, srcInterleaveOffset, vertexCount);
          srcInterleaveOffset += 4 * 3;
          break;
        case SceneFormat::VertexElementFormat::Vector4:
          ConvertFloat4ArrayLE(pSrc, srcLength, srcVertexStride, srcInterleaveOffset, vertexCount);
          srcInterleaveOffset += 4 * 4;
          break;
        default:
          throw NotSupportedException("Vertex element format not supported");
        }
      }
    }


    //! @brief Read the vertices in the host format and write them in little endian format
    //!        While at the same time compacting the vertex data to its minimal byte size.
    std::size_t WriteVerticesLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const InternalMeshRecord& record,
                                const std::deque<InternalVertexDeclaration>& dstVertexDeclarations)
    {
      const auto meshVertexDeclaration = record.SourceMesh->AsVertexDeclarationSpan();
      const auto meshContent = record.SourceMesh->GenericDirectAccess();
      const auto srcVertexCount = meshContent.VertexCount;
      const auto srcVertexStride = meshContent.VertexStride;
      const auto* pVertices = static_cast<const uint8_t*>(meshContent.pVertices);
      const auto cbSrcVertices = srcVertexCount * srcVertexStride;
      const InternalVertexDeclaration& dstVertexDeclaration = dstVertexDeclarations[record.VertexDeclarationIndex];
      const auto dstStride = dstVertexDeclaration.VertexByteSize;

      std::size_t dstInterleaveOffset = 0;
      for (auto itr = dstVertexDeclaration.Elements.begin(); itr != dstVertexDeclaration.Elements.end(); ++itr)
      {
        const std::size_t srcInterleaveOffset = LocateOffset(meshVertexDeclaration, itr->Usage, itr->UsageIndex);
        switch (itr->Format)
        {
        case SceneFormat::VertexElementFormat::Single:
          WriteFloat1ArrayLE(pDst, dstLength, dstIndex, dstStride, dstInterleaveOffset, pVertices, cbSrcVertices, srcVertexStride,
                             srcInterleaveOffset, srcVertexCount);
          dstInterleaveOffset += 4;
          break;
        case SceneFormat::VertexElementFormat::Vector2:
          WriteFloat2ArrayLE(pDst, dstLength, dstIndex, dstStride, dstInterleaveOffset, pVertices, cbSrcVertices, srcVertexStride,
                             srcInterleaveOffset, srcVertexCount);
          dstInterleaveOffset += 4 * 2;
          break;
        case SceneFormat::VertexElementFormat::Vector3:
          WriteFloat3ArrayLE(pDst, dstLength, dstIndex, dstStride, dstInterleaveOffset, pVertices, cbSrcVertices, srcVertexStride,
                             srcInterleaveOffset, srcVertexCount);
          dstInterleaveOffset += 4 * 3;
          break;
        case SceneFormat::VertexElementFormat::Vector4:
          WriteFloat4ArrayLE(pDst, dstLength, dstIndex, dstStride, dstInterleaveOffset, pVertices, cbSrcVertices, srcVertexStride,
                             srcInterleaveOffset, srcVertexCount);
          dstInterleaveOffset += 4 * 4;
          break;
        default:
          throw NotSupportedException("Vertex element format not supported");
        }
      }

      assert(dstInterleaveOffset == dstVertexDeclaration.VertexByteSize);
      return srcVertexCount * dstVertexDeclaration.VertexByteSize;
    }


    //! @brief Read the indices in little endian format writing them in host format
    void ConvertIndicesLE(uint8_t* const pSrc, const std::size_t srcLength, const std::size_t srcIndexByteSize, const std::size_t indexCount)
    {
      assert(pSrc != nullptr);
      const std::size_t indexOffsetEnd = indexCount * srcIndexByteSize;

      if (srcIndexByteSize == 1)
      {
        return;
      }
      if (srcIndexByteSize == 2)
      {
        for (std::size_t indexOffset = 0; indexOffset < indexOffsetEnd; indexOffset += srcIndexByteSize)
        {
          *reinterpret_cast<uint16_t*>(pSrc + indexOffset) = ByteArrayUtil::ReadUInt16LE(pSrc, srcLength, indexOffset);
        }
      }
      else
      {
        throw NotSupportedException("Index byte size not supported");
      }
    }


    //! @brief Read the indices in the host format and write them in little endian format
    std::size_t WriteIndicesLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const InternalMeshRecord& record)
    {
      assert(pDst != nullptr);

      const auto meshContent = record.SourceMesh->GenericDirectAccess();
      const auto indexCount = meshContent.IndexCount;
      const auto indexStride = meshContent.IndexStride;
      const auto* pIndices = static_cast<const uint8_t*>(meshContent.pIndices);

      const std::size_t indexOffsetEnd = indexCount * indexStride;

      std::size_t currentDstIndex = dstIndex;
      if (record.IndexByteSize == 1)
      {
        for (std::size_t indexOffset = 0; indexOffset < indexOffsetEnd; indexOffset += indexStride)
        {
          currentDstIndex += ByteArrayUtil::WriteUInt8LE(pDst, dstLength, currentDstIndex, *(pIndices + indexOffset));
        }
      }
      else if (record.IndexByteSize == 2)
      {
        for (std::size_t indexOffset = 0; indexOffset < indexOffsetEnd; indexOffset += indexStride)
        {
          const uint16_t value = *reinterpret_cast<const uint16_t*>(pIndices + indexOffset);
          currentDstIndex += ByteArrayUtil::WriteUInt16LE(pDst, dstLength, currentDstIndex, value);
        }
      }
      else
      {
        throw NotSupportedException("Index byte size not supported");
      }
      return currentDstIndex - dstIndex;
    }


    //! @brief Validate that the string has the expected length and is zero terminated
    void ValidateStringLength(const uint8_t* pStr, const std::size_t expectedLength)
    {
      assert(pStr != nullptr);
      if (pStr[expectedLength] != 0)
      {
        throw FormatException("No zero termination found at the end of the string");
      }

      for (std::size_t i = 0; i < expectedLength; ++i)
      {
        if (pStr[i] == 0)
        {
          throw FormatException("Encountered zero termination before string end");
        }
      }
    }

    //! @Brief create a vertex declaration matching the InternalVertexDeclaration
    VertexDeclaration Create(const InternalVertexDeclaration& internalVertexDeclaration)
    {
      std::vector<VertexElement> elements(internalVertexDeclaration.Elements.size());

      auto dstItr = elements.begin();
      uint32_t offset = 0;
      for (auto itr = internalVertexDeclaration.Elements.begin(); itr != internalVertexDeclaration.Elements.end(); ++itr)
      {
        *dstItr = VertexElement(offset, Conversion::Convert(itr->Format), Conversion::Convert(itr->Usage), itr->UsageIndex);
        ++dstItr;
        offset += InternalVertexDeclaration::CalcByteSize(*itr);
      }
      assert(offset == internalVertexDeclaration.VertexByteSize);
      return {elements.data(), elements.size(), internalVertexDeclaration.VertexByteSize};
    }


    void AddMeshToScene(Scene& rScene, const MeshAllocatorFunc& meshAllocator, const void* const pVertices, const std::size_t vertexCount,
                        const InternalVertexDeclaration& srcInternalVertexDeclaration, const void* const pIndices, const std::size_t indexCount,
                        const uint8_t indexByteSize, const SceneFormat::PrimitiveType primitiveType, const uint32_t materialIndex,
                        const char* const pszName, const void* const pDstDefaultValues, const int32_t cbDstDefaultValues)
    {
      if (materialIndex >= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
      {
        throw FormatException("Material index is invalid");
      }

      // Create the vertex declaration for the loaded file
      const VertexDeclaration srcVertexDeclaration(Create(srcInternalVertexDeclaration));

      // Create the mesh container object
      std::shared_ptr<Mesh> mesh = meshAllocator(vertexCount, indexCount, Conversion::Convert(primitiveType));
      mesh->SetMaterialIndex(static_cast<int32_t>(materialIndex));
      mesh->SetName(UTF8String(pszName));

      const auto cbSrcVertices = srcVertexDeclaration.VertexStride() * vertexCount;
      const auto cbSrcIndices = indexByteSize * indexCount;

      RawMeshContentEx rawDst = mesh->GenericDirectAccess();
      VertexConverter::GenericConvert(rawDst.pVertices, rawDst.VertexStride * rawDst.VertexCount, mesh->AsVertexDeclarationSpan(), pVertices,
                                      cbSrcVertices, srcVertexDeclaration.AsSpan(), vertexCount, pDstDefaultValues, cbDstDefaultValues);
      IndexConverter::GenericConvert(rawDst.pIndices, rawDst.IndexStride * rawDst.IndexCount, rawDst.IndexStride, pIndices, cbSrcIndices,
                                     indexByteSize, indexCount);

      rScene.AddMesh(mesh);
    }


    std::shared_ptr<Scene> ReadMeshesChunk(std::ifstream& rStream, const std::deque<InternalVertexDeclaration>& vertexDeclarations,
                                           const Graphics3D::SceneAllocatorFunc& sceneAllocator, const void* const pDstDefaultValues,
                                           const int32_t cbDstDefaultValues, const bool hostIsLittleEndian)
    {
      const ChunkHeader header = ReadChunkHeader(rStream);
      if (header.Type != ChunkType::Meshes)
      {
        throw FormatException("Did not find the expected mesh chunk");
      }
      if (header.Version != ChunkVersionMeshes)
      {
        throw FormatException("Unsupported mesh chunk version");
      }

      const auto startPos = rStream.tellg();

      std::array<uint8_t, MeshHeaders> buffer{};
      rStream.read(reinterpret_cast<char*>(buffer.data()), SizeofMeshListHeader);
      if (!rStream.good())
      {
        throw FormatException("Failed to read the expected data");
      }

      const uint32_t meshCount = ByteArrayUtil::ReadUInt32LE(buffer.data(), SizeofMeshListHeader, 0);

      // Create the scene
      std::shared_ptr<Scene> scene = sceneAllocator(meshCount);
      MeshAllocatorFunc meshAllocator = scene->GetMeshAllocator();

      for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
      {
        rStream.read(reinterpret_cast<char*>(buffer.data()), SizeofMeshHeader);
        if (!rStream.good())
        {
          throw FormatException("Failed to read the expected data");
        }

        const uint32_t materialIndex = ByteArrayUtil::ReadUInt32LE(buffer.data(), buffer.size(), MeshOffsetMaterialIndex);
        const uint32_t vertexCount = ByteArrayUtil::ReadUInt32LE(buffer.data(), buffer.size(), MeshOffsetVertexCount);
        const uint32_t indexCount = ByteArrayUtil::ReadUInt32LE(buffer.data(), buffer.size(), MeshOffsetIndexCount);
        const uint32_t nameLength = ByteArrayUtil::ReadUInt32LE(buffer.data(), buffer.size(), MeshOffsetNameLength);
        const auto primitiveType = ConvertToPrimitiveType(ByteArrayUtil::ReadUInt8LE(buffer.data(), buffer.size(), MeshOffsetPrimitiveType));
        const uint8_t vertexDeclarationIndex = ByteArrayUtil::ReadUInt8LE(buffer.data(), buffer.size(), MeshOffsetVertexDeclarationIndex);
        const uint8_t indexByteSize = ByteArrayUtil::ReadUInt8LE(buffer.data(), buffer.size(), MeshOffsetIndexByteSize);

        if (vertexDeclarationIndex >= vertexDeclarations.size())
        {
          throw FormatException("Referenced a invalid vertex declaration");
        }
        if (nameLength == 0)
        {
          throw FormatException("the name is expected to be zero terminated, so a min length of 1 is expected");
        }

        const uint32_t cbVertex = vertexDeclarations[vertexDeclarationIndex].VertexByteSize;
        const uint32_t cbVertices = cbVertex * vertexCount;
        const uint32_t cbIndices = indexByteSize * indexCount;
        std::vector<uint8_t> content(cbVertices + cbIndices + nameLength);

        uint8_t* pContent = content.data();

        // Read the actual data
        rStream.read(reinterpret_cast<char*>(pContent), NumericCast<std::streamsize>(content.size()));
        if (!rStream.good())
        {
          throw FormatException("Failed to read the vertices");
        }

        const uint8_t* const pVertices = pContent;
        const uint8_t* const pIndices = pContent + cbVertices;
        if (hostIsLittleEndian)
        {
          ConvertVerticesLE(pContent, cbVertices, vertexDeclarations[vertexDeclarationIndex], vertexCount);
          pContent += cbVertices;

          assert(pContent == pIndices);
          ConvertIndicesLE(pContent, cbIndices, indexByteSize, indexCount);
          pContent += cbIndices;
        }
        else
        {
          pContent += cbVertices + cbIndices;
        }

        // Update mesh with the name
        ValidateStringLength(pContent, nameLength - 1);

        // Create the mesh and add it to the scene
        AddMeshToScene(*scene, meshAllocator, pVertices, vertexCount, vertexDeclarations[vertexDeclarationIndex], pIndices, indexCount, indexByteSize,
                       primitiveType, materialIndex, reinterpret_cast<const char*>(pContent), pDstDefaultValues, cbDstDefaultValues);
      }

      const auto endPos = rStream.tellg();
      assert(endPos >= startPos);
      if (static_cast<uint32_t>(endPos - startPos) != header.ByteSize)
      {
        throw FormatException("MeshesChunk was of a unexpected size");
      }
      return scene;
    }

    /*
          uint8_t GetIndexType(const uint32_t indexByteSize)
          {
            switch (indexByteSize)
            {
            case 1:
              return 0;
            case 2:
              return 1;
            default:
              throw NotSupportedException("Index type not supported");
            }
          }
    */

    void WriteMeshesChunk(std::ofstream& rStream, const InternalSceneRecord& scene)
    {
      const uint32_t cbMeshes = CalcMeshesSize(scene);
      ChunkHeader header(cbMeshes, ChunkType::Meshes, ChunkVersionMeshes);
      WriteChunkHeader(rStream, header);

      const auto& vertexDeclarations = scene.VertexDeclarations;
      const auto& meshes = scene.Meshes;

      std::vector<uint8_t> content(cbMeshes);

      if (meshes.size() > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("Only support up to 32bit mesh count");
      }

      std::size_t dstIndex = 0;
      dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, static_cast<uint32_t>(meshes.size()));
#if !defined(NDEBUG)
      auto dstStartIndex = dstIndex;
#endif
      for (auto itr = meshes.begin(); itr != meshes.end(); ++itr)
      {
        assert(itr->SourceMesh);
        const auto& mesh = itr->SourceMesh;

        const uint32_t vertexCount = mesh->GetVertexCount();
        const uint32_t indexCount = mesh->GetIndexCount();
        const int32_t nameLength = mesh->GetName().GetByteSize() + 1;    // +1 to write the terminating zero as well
        const uint32_t materialIndex = mesh->GetMaterialIndex();
        const auto primitiveType = static_cast<uint32_t>(Conversion::Convert(mesh->GetPrimitiveType()));
        assert(itr->IndexByteSize <= 255);
        const auto indexType = static_cast<uint8_t>(itr->IndexByteSize);
        uint8_t vertexDeclarationIndex = itr->VertexDeclarationIndex;

        if (nameLength < 0 || primitiveType > 255)
        {
          throw NotSupportedException("Mesh format unsupported");
        }

        // Write the mesh header
        assert((dstIndex - dstStartIndex) == (MeshOffsetMaterialIndex));
        dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, materialIndex);
        assert((dstIndex - dstStartIndex) == (MeshOffsetVertexCount));
        dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, vertexCount);
        assert((dstIndex - dstStartIndex) == (MeshOffsetIndexCount));
        dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, indexCount);
        assert((dstIndex - dstStartIndex) == (MeshOffsetNameLength));
        dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, UncheckedNumericCast<uint32_t>(nameLength));
        assert((dstIndex - dstStartIndex) == (MeshOffsetPrimitiveType));
        dstIndex += ByteArrayUtil::WriteUInt8LE(content.data(), content.size(), dstIndex, UncheckedNumericCast<uint8_t>(primitiveType));
        assert((dstIndex - dstStartIndex) == (MeshOffsetVertexDeclarationIndex));
        dstIndex += ByteArrayUtil::WriteUInt8LE(content.data(), content.size(), dstIndex, vertexDeclarationIndex);
        assert((dstIndex - dstStartIndex) == (MeshOffsetIndexByteSize));
        dstIndex += ByteArrayUtil::WriteUInt8LE(content.data(), content.size(), dstIndex, indexType);

        // write vertex data
        dstIndex += WriteVerticesLE(content.data(), content.size(), dstIndex, *itr, vertexDeclarations);
        // write index data
        dstIndex += WriteIndicesLE(content.data(), content.size(), dstIndex, *itr);
        // write name
        dstIndex += ByteArrayUtil::WriteBytes(content.data(), content.size(), dstIndex,
                                              reinterpret_cast<const uint8_t*>(mesh->GetName().ToUTF8String().c_str()), nameLength);
#if !defined(NDEBUG)
        dstStartIndex = dstIndex;
#endif
      }

      // Verify that we wrote the expected amount of bytes
      assert(static_cast<uint32_t>(dstIndex) == cbMeshes);

      rStream.write(reinterpret_cast<const char*>(content.data()), NumericCast<std::streamsize>(content.size()));
    }


    struct NodeInfo
    {
      std::shared_ptr<SceneNode> Node;
      uint32_t Index;

      NodeInfo(std::shared_ptr<SceneNode> node, const uint32_t index)
        : Node(std::move(node))
        , Index(index)
      {
      }
    };

    struct NodeComp
    {
      const std::shared_ptr<SceneNode> Node;
      explicit NodeComp(std::shared_ptr<SceneNode> node)
        : Node(std::move(node))
      {
      }

      inline bool operator()(const NodeInfo& entry) const
      {
        return entry.Node == Node;
      }
    };


    std::size_t ReadNode(std::ifstream& rStream, std::deque<std::shared_ptr<SceneNode>>& rNodes, const std::vector<uint8_t>& srcBuffer,
                         const std::size_t srcOffset, const uint32_t sceneMeshCount, const bool hostIsLittleEndian)
    {
      FSL_PARAM_NOT_USED(rStream);
      FSL_PARAM_NOT_USED(hostIsLittleEndian);

      Matrix transform;
      auto* pTransform = reinterpret_cast<uint32_t*>(transform.DirectAccess());

      std::size_t srcIndex = srcOffset;
      assert((srcIndex - srcOffset) == NodeOffsetTransformation);
      // Read the transform matrix
      pTransform[0 + (4 * 0)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 0));
      pTransform[1 + (4 * 0)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 1));
      pTransform[2 + (4 * 0)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 2));
      pTransform[3 + (4 * 0)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 3));
      pTransform[0 + (4 * 1)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 4));
      pTransform[1 + (4 * 1)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 5));
      pTransform[2 + (4 * 1)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 6));
      pTransform[3 + (4 * 1)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 7));
      pTransform[0 + (4 * 2)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 8));
      pTransform[1 + (4 * 2)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 9));
      pTransform[2 + (4 * 2)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 10));
      pTransform[3 + (4 * 2)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 11));
      pTransform[0 + (4 * 3)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 12));
      pTransform[1 + (4 * 3)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 13));
      pTransform[2 + (4 * 3)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 14));
      pTransform[3 + (4 * 3)] = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex + (4 * 15));
      srcIndex += (4 * 16);
      assert((srcIndex - srcOffset) == NodeOffsetNameLength);
      const uint16_t nameLength = ByteArrayUtil::ReadUInt16LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
      srcIndex += sizeof(uint16_t);
      assert((srcIndex - srcOffset) == NodeOffsetMeshCount);
      const uint8_t nodeMeshCount = ByteArrayUtil::ReadUInt8LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
      srcIndex += sizeof(uint8_t);
      assert((srcIndex - srcOffset) == NodeOffsetChildCount);
      const uint8_t nodeChildCount = ByteArrayUtil::ReadUInt8LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
      srcIndex += sizeof(uint8_t);
      assert((srcIndex - srcOffset) == SizeofNodeHeader);

      if (nameLength == 0)
      {
        throw FormatException("the name is expected to be zero terminated, so a min length of 1 is expected");
      }

      auto node = std::make_shared<SceneNode>(nodeMeshCount);

      // Read mesh indices
      for (std::size_t i = 0; i < nodeMeshCount; ++i)
      {
        const uint32_t index = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
        srcIndex += sizeof(uint32_t);
        if (index >= sceneMeshCount)
        {
          throw FormatException("Invalid mesh index encountered");
        }
        node->AddMesh(index);
      }

      // Read node indices
      for (std::size_t i = 0; i < nodeChildCount; ++i)
      {
        const uint32_t index = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
        srcIndex += sizeof(uint32_t);
        if (index >= rNodes.size())
        {
          throw FormatException("Invalid child node index encountered");
        }

        node->AddChild(rNodes[index]);
      }

      // Update node with the name
      ValidateStringLength(srcBuffer.data() + srcIndex, nameLength - 1);
      node->SetName(UTF8String(reinterpret_cast<const char*>(srcBuffer.data() + srcIndex)));
      srcIndex += nameLength;

      rNodes.push_back(node);
      return srcIndex - srcOffset;
    }


    void WriteNode(std::ofstream& rStream, std::vector<uint8_t>& rContent, const NodeInfo& info, const std::deque<NodeInfo>& allNodes)
    {
      assert(info.Node);

      const auto nodeChildCount = info.Node->GetChildCount();
      const auto nodeMeshCount = info.Node->GetMeshCount();
      const std::size_t nameLength = (info.Node->GetName().GetByteSize() + 1);    // +1 to write the terminating zero as well

      if (nameLength >= std::numeric_limits<uint16_t>::max())
      {
        throw FormatException("Name size not supported");
      }
      if (nodeMeshCount < 0 || nodeMeshCount >= std::numeric_limits<uint8_t>::max())
      {
        throw FormatException("Mesh count not supported");
      }
      if (nodeChildCount < 0 || nodeChildCount >= std::numeric_limits<uint8_t>::max())
      {
        throw FormatException("Child count not supported");
      }

      Matrix transform = info.Node->GetTransformation();
      const auto* pTransform = reinterpret_cast<const uint32_t*>(transform.DirectAccess());

      std::size_t dstIndex = 0;
      assert(dstIndex == NodeOffsetTransformation);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[0 + (4 * 0)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[1 + (4 * 0)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[2 + (4 * 0)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[3 + (4 * 0)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[0 + (4 * 1)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[1 + (4 * 1)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[2 + (4 * 1)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[3 + (4 * 1)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[0 + (4 * 2)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[1 + (4 * 2)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[2 + (4 * 2)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[3 + (4 * 2)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[0 + (4 * 3)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[1 + (4 * 3)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[2 + (4 * 3)]);
      dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, pTransform[3 + (4 * 3)]);
      assert(dstIndex == NodeOffsetNameLength);
      dstIndex += ByteArrayUtil::WriteUInt16LE(rContent.data(), rContent.size(), dstIndex, static_cast<uint16_t>(nameLength));
      assert(dstIndex == NodeOffsetMeshCount);
      dstIndex += ByteArrayUtil::WriteUInt8LE(rContent.data(), rContent.size(), dstIndex, static_cast<uint8_t>(nodeMeshCount));
      assert(dstIndex == NodeOffsetChildCount);
      dstIndex += ByteArrayUtil::WriteUInt8LE(rContent.data(), rContent.size(), dstIndex, static_cast<uint8_t>(nodeChildCount));
      assert(dstIndex == SizeofNodeHeader);

      // write mesh indices
      for (int32_t i = 0; i < nodeMeshCount; ++i)
      {
        const int32_t meshIndex = info.Node->GetMeshAt(i);
        if (meshIndex < 0)
        {
          throw FormatException("Negative mesh indices not supported");
        }
        dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, static_cast<uint32_t>(meshIndex));
      }


      // write node indices
      // WARNING: the method used for finding the node index is far from optimal, but it works for now
      for (int32_t i = 0; i < nodeChildCount; ++i)
      {
        auto child = info.Node->GetChildAt(i);
        const auto itr = std::find_if(allNodes.begin(), allNodes.end(), NodeComp(child));
        if (itr == allNodes.end())
        {
          throw FormatException("Node not found");
        }
        dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, itr->Index);
      }

      // write name
      dstIndex += ByteArrayUtil::WriteBytes(rContent.data(), rContent.size(), dstIndex,
                                            reinterpret_cast<const uint8_t*>(info.Node->GetName().ToUTF8String().c_str()), nameLength);

      rStream.write(reinterpret_cast<const char*>(rContent.data()), NumericCast<std::streamsize>(dstIndex));
    }


    void ReadNodesChunk(std::ifstream& rStream, Scene& rScene, const bool hostIsLittleEndian)
    {
      const ChunkHeader header = ReadChunkHeader(rStream);
      if (header.Type != ChunkType::Nodes)
      {
        throw FormatException("Did not find the expected nodes chunk");
      }
      if (header.Version != ChunkVersionNodes)
      {
        throw FormatException("Unsupported node chunk version");
      }

      const auto startPos = rStream.tellg();

      std::vector<uint8_t> buffer(header.ByteSize);
      rStream.read(reinterpret_cast<char*>(buffer.data()), header.ByteSize);
      if (!rStream.good())
      {
        throw FormatException("Failed to read the expected data");
      }

      const uint32_t nodeCount = ByteArrayUtil::ReadUInt32LE(buffer.data(), SizeofNodelistHeader, 0);

      const auto sceneMeshCount = static_cast<uint32_t>(rScene.GetMeshCount());
      std::deque<std::shared_ptr<SceneNode>> nodes;
      std::size_t srcOffset = SizeofNodelistHeader;
      for (uint32_t childIndex = 0; childIndex < nodeCount; ++childIndex)
      {
        srcOffset += ReadNode(rStream, nodes, buffer, srcOffset, sceneMeshCount, hostIsLittleEndian);
      }

      const auto endPos = rStream.tellg();
      assert(endPos >= startPos);
      if (static_cast<uint32_t>(endPos - startPos) != header.ByteSize)
      {
        throw FormatException("NodeChunk was of a unexpected size");
      }

      if (!nodes.empty())
      {
        rScene.SetRootNode(nodes.back());
      }
    }


    struct NodesInfo
    {
      std::size_t TotalByteSize;
      std::size_t MaxNodeByteSize;

      NodesInfo(const std::size_t totalByteSize, const std::size_t maxNodeByteSize)
        : TotalByteSize(totalByteSize)
        , MaxNodeByteSize(maxNodeByteSize)
      {
      }
    };


    NodesInfo ExamineNodes(std::deque<NodeInfo>& rAllNodes, const std::shared_ptr<SceneNode>& node)
    {
      if (!node)
      {
        return {0, 0};
      }


      const auto nodeMeshCount = node->GetMeshCount();
      const auto nodeChildCount = node->GetChildCount();
      if (nodeMeshCount < 0 || nodeChildCount < 0)
      {
        throw FormatException("Unsupported mesh or child count");
      }


      std::size_t nodeByteSize = SizeofNodeHeader;
      nodeByteSize += nodeMeshCount * sizeof(uint32_t);
      nodeByteSize += nodeChildCount * sizeof(uint32_t);
      nodeByteSize += node->GetName().GetByteSize() + 1;    // +1 to write the terminating zero as well

      NodesInfo totalInfo(nodeByteSize, nodeByteSize);
      for (int32_t childIndex = 0; childIndex < nodeChildCount; ++childIndex)
      {
        NodesInfo info = ExamineNodes(rAllNodes, node->GetChildAt(childIndex));
        totalInfo.TotalByteSize += info.TotalByteSize;
        totalInfo.MaxNodeByteSize = std::max(totalInfo.MaxNodeByteSize, info.TotalByteSize);
      }

      // depth first storage
      assert(rAllNodes.size() <= std::numeric_limits<uint32_t>::max());
      rAllNodes.emplace_back(node, static_cast<uint32_t>(rAllNodes.size()));
      return totalInfo;
    }


    void WriteNodesChunk(std::ofstream& rStream, const Scene& scene)
    {
      std::deque<NodeInfo> allNodes;
      NodesInfo nodesInfo = ExamineNodes(allNodes, scene.GetRootNode());

      assert(nodesInfo.TotalByteSize <= (std::numeric_limits<uint32_t>::max() - SizeofNodelistHeader));
      ChunkHeader header(static_cast<uint32_t>(SizeofNodelistHeader + nodesInfo.TotalByteSize), ChunkType::Nodes, ChunkVersionNodes);
      WriteChunkHeader(rStream, header);

      std::vector<uint8_t> content(nodesInfo.MaxNodeByteSize);
      // Write the total node count
      assert(allNodes.size() <= std::numeric_limits<uint32_t>::max());
      ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), 0, static_cast<uint32_t>(allNodes.size()));
      rStream.write(reinterpret_cast<const char*>(content.data()), SizeofNodelistHeader);

      for (auto itr = allNodes.begin(); itr != allNodes.end(); ++itr)
      {
        WriteNode(rStream, content, *itr, allNodes);
      }
    }


    //! @brief Verify that a float is 4bytes big and that the endian used by float and uint32_t is the same.
    //! @return true if the host is little endian
    bool CheckEndianAssumptions()
    {
      static_assert(sizeof(float) == 4, "We expect a float to be four bytes");

      // Runtime verification of endian assumptions
      uint32_t tmp = 0;
      auto* pUInt32 = reinterpret_cast<uint32_t*>(&tmp);
      auto* pFloat = reinterpret_cast<float*>(&tmp);
      auto* pTmp = reinterpret_cast<uint8_t*>(&tmp);
      *pUInt32 = 0x01020304;
      if (pTmp[0] == 0x04 && pTmp[1] == 0x03 && pTmp[2] == 0x02 && pTmp[3] == 0x01)
      {
        // little endian
        // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions)
        *pFloat = 0x01020304;
        if (pTmp[0] != 0x82 || pTmp[1] != 0x01 || pTmp[2] != 0x81 || pTmp[3] != 0x4B)
        {
          throw NotSupportedException("The float is either not of the expected encoding or not stored in the same endian as a uint32_t");
        }
        return true;
      }
      if (pTmp[0] == 0x01 && pTmp[1] == 0x02 && pTmp[2] == 0x03 && pTmp[3] == 0x04)
      {
        // big endian
        // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions)
        *pFloat = 0x01020304;
        if (pTmp[0] != 0x4B || pTmp[1] != 0x81 || pTmp[2] != 0x01 || pTmp[3] != 0x82)
        {
          throw NotSupportedException("The float is either not of the expected encoding or not stored in the same endian as a uint32_t");
        }
        return false;
      }

      throw NotSupportedException("Unsupported endian format");
    }
  }


  BasicSceneFormat::BasicSceneFormat()
    : m_sceneScratchpad(std::make_shared<InternalSceneRecord>())
    , m_hostIsLittleEndian(CheckEndianAssumptions())
  {
  }


  BasicSceneFormat::~BasicSceneFormat() = default;


  std::shared_ptr<Graphics3D::Scene> BasicSceneFormat::GenericLoad(const IO::Path& filename, const Graphics3D::SceneAllocatorFunc& sceneAllocator,
                                                                   const void* const pDstDefaultValues, const int32_t cbDstDefaultValues)
  {
    std::ifstream fileStream(PlatformPathTransform::ToSystemPath(filename), std::ios::in | std::ios::binary);
    return GenericLoad(fileStream, sceneAllocator, pDstDefaultValues, cbDstDefaultValues);
  }


  std::shared_ptr<Graphics3D::Scene> BasicSceneFormat::GenericLoad(std::ifstream& rStream, const Graphics3D::SceneAllocatorFunc& sceneAllocator,
                                                                   const void* const pDstDefaultValues, const int32_t cbDstDefaultValues)
  {
    try
    {
      // Read and validate header
      ReadHeader(rStream);
      m_sceneScratchpad->Clear();
      ReadVertexDeclarationsChunk(rStream, m_sceneScratchpad->VertexDeclarations);

      std::shared_ptr<Scene> scene =
        ReadMeshesChunk(rStream, m_sceneScratchpad->VertexDeclarations, sceneAllocator, pDstDefaultValues, cbDstDefaultValues, m_hostIsLittleEndian);

      ReadNodesChunk(rStream, *scene, m_hostIsLittleEndian);

      m_sceneScratchpad->Clear();
      return scene;
    }
    catch (const std::exception&)
    {
      m_sceneScratchpad->Clear();
      throw;
    }
  }


  void BasicSceneFormat::Save(const IO::Path& strFilename, const Scene& scene)
  {
    std::ofstream fileStream(PlatformPathTransform::ToSystemPath(strFilename), std::ios::out | std::ios::binary);
    Save(fileStream, scene);
  }


  void BasicSceneFormat::Save(std::ofstream& rStream, const Scene& scene)
  {
    m_sceneScratchpad->Clear();
    try
    {
      // Extract the unique vertex declarations used by the meshes in the scene
      ExtractUniqueVertexDeclarations(*m_sceneScratchpad, scene);

      // Write the scene content
      FormatHeader header(FormatMagic, FormatCurrentVersion);
      WriteHeader(rStream, header);
      WriteVertexDeclarationsChunk(rStream, m_sceneScratchpad->VertexDeclarations);
      WriteMeshesChunk(rStream, *m_sceneScratchpad);
      WriteNodesChunk(rStream, scene);

      m_sceneScratchpad->Clear();
    }
    catch (const std::exception&)
    {
      m_sceneScratchpad->Clear();
      throw;
    }
  }
}
