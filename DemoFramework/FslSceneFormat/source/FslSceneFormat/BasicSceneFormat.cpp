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

#include <FslSceneFormat/BasicSceneFormat.hpp>
#include <FslBase/Bits/ByteArrayUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Vertices/IndexConverter.hpp>
#include <FslGraphics/Vertices/VertexConverter.hpp>
#include <FslSceneFormat/ChunkType.hpp>
#include <FslSceneFormat/PrimitiveType.hpp>
#include <FslSceneFormat/VertexElementFormat.hpp>
#include <FslSceneFormat/VertexElementUsage.hpp>
#include "SFVertexElement.hpp"
#include "Conversion.hpp"
#include <algorithm>
#include <cassert>
#include <deque>
#include <fstream>
#include <limits>
#include <vector>

// Nasty hack for dealing with UTF8 file names on windows,
// since its the only supported platform that doesn't allow UTF8 strings
// but instead provides its own 'hack' for opening wstring's
#if _WIN32
#define PATH_GET_NAME(X) X.ToWString()
#else
#define PATH_GET_NAME(X) X.ToUTF8String()
#endif

//! workaround for GCC complaint
#ifdef __GNUC__
#define REMOVE_WARNING_VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
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
//! - Since there are no standard compiler flags containing endian information the code here uses some very basic endian assumptions that are verified at runtime.

namespace Fsl
{
  namespace SceneFormat
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
        if ( IndexByteSize != 1 && IndexByteSize != 2)
          throw NotSupportedException("Index stride is not supported");
      }
    };

    struct InternalVertexDeclaration : public SFVertexDeclaration
    {
      uint32_t VertexByteSize;

      InternalVertexDeclaration()
        : SFVertexDeclaration()
        , VertexByteSize(0)
      {
      }

      InternalVertexDeclaration(const SFVertexDeclaration& declaration)
        : SFVertexDeclaration(declaration)
        , VertexByteSize(0)
      {
        for (auto itr = declaration.Elements.begin(); itr != declaration.Elements.end(); ++itr)
          VertexByteSize += CalcByteSize(*itr);
      }

      static uint32_t CalcByteSize(const SFVertexElement& element)
      {
        switch (element.Format)
        {
        case SceneFormat::VertexElementFormat::Single:
          return 4;
        case SceneFormat::VertexElementFormat::Vector2:
          return 4*2;
        case SceneFormat::VertexElementFormat::Vector3:
          return 4*3;
        case SceneFormat::VertexElementFormat::Vector4:
          return 4*4;
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
      const uint32_t FORMAT_MAGIC = 0x00465346;
      const uint32_t FORMAT_CURRENT_VERSION = 0;

      const uint16_t CHUNK_VERSION_VertexDeclaration = 0;
      const uint16_t CHUNK_VERSION_Meshes = 0;
      const uint16_t CHUNK_VERSION_Nodes = 0;


      const uint32_t FORMATHEADER_OFFSET_Magic = 0;
      const uint32_t FORMATHEADER_OFFSET_Version = FORMATHEADER_OFFSET_Magic + sizeof(uint32_t);
      const uint32_t SIZE_OF_FORMATHEADER = FORMATHEADER_OFFSET_Version + sizeof(uint32_t);

      // Basic format header
      struct FormatHeader
      {
        uint32_t Magic;
        uint32_t Version;

        FormatHeader()
          : Magic(FORMAT_MAGIC)
          , Version(FORMAT_CURRENT_VERSION)
        {
        }

        FormatHeader(const uint32_t magic, const uint32_t version)
          : Magic(magic)
          , Version(version)
        {
        }
      };

      const uint32_t CHUNKHEADER_OFFSET_ByteSize = 0;
      const uint32_t CHUNKHEADER_OFFSET_Type = CHUNKHEADER_OFFSET_ByteSize + sizeof(uint32_t);
      const uint32_t CHUNKHEADER_OFFSET_Reserved = CHUNKHEADER_OFFSET_Type + sizeof(uint8_t);
      const uint32_t CHUNKHEADER_OFFSET_Version = CHUNKHEADER_OFFSET_Reserved + sizeof(uint8_t);
      const uint32_t SIZE_OF_CHUNKHEADER = CHUNKHEADER_OFFSET_Version + sizeof(uint16_t);

      struct ChunkHeader
      {
        // The byte size excluding the header
        uint32_t ByteSize;
        // The type of the chunk
        ChunkType Type; // uint8_t
        uint8_t Reserved;
        uint16_t Version;

        ChunkHeader()
          : ByteSize(0)
          , Type(ChunkType::VertexDeclarations)
          , Reserved(0)
          , Version(0)
        {
        }

        ChunkHeader(const uint32_t byteSize, const ChunkType chunkType, const uint16_t version)
          : ByteSize(byteSize)
          , Type(chunkType)
          , Reserved(0)
          , Version(version)
        {
        }
      };

      //int32_t Offset;
      //VertexElementFormat Format;
      //VertexElementUsage Usage;
      //int32_t UsageIndex;

      const uint32_t SIZEOF_VERTEX_DECLARATION_LIST_HEADER = 4;

      //struct VertexDeclarations
      //{
      //  uint32_t Entries;
      //};

      const uint32_t SIZEOF_VERTEX_DECLARATION_HEADER = 2;

      //struct VertexDeclaration
      //{
      //  uint16_t ElementCount;
      //};

      const uint32_t VERTEXELEMENT_OFFSET_Format = 0;
      const uint32_t VERTEXELEMENT_OFFSET_Usage = VERTEXELEMENT_OFFSET_Format + sizeof(uint8_t);
      const uint32_t VERTEXELEMENT_OFFSET_UsageIndex = VERTEXELEMENT_OFFSET_Usage + sizeof(uint8_t);
      const uint32_t SIZE_OF_VERTEXELEMENT = VERTEXELEMENT_OFFSET_UsageIndex + sizeof(uint8_t);

      const uint32_t BUFFER_ENTRY_SIZE_1 = SIZEOF_VERTEX_DECLARATION_LIST_HEADER >= SIZEOF_VERTEX_DECLARATION_HEADER ? SIZEOF_VERTEX_DECLARATION_LIST_HEADER : SIZEOF_VERTEX_DECLARATION_HEADER;
      const uint32_t BUFFER_ENTRY_SIZE_2 = BUFFER_ENTRY_SIZE_1 >= SIZE_OF_VERTEXELEMENT ? BUFFER_ENTRY_SIZE_1 : SIZE_OF_VERTEXELEMENT;
      const uint32_t BUFFER_ENTRY_SIZE = BUFFER_ENTRY_SIZE_2;
      //struct ChunkVertexElement
      //{
      //  uint8_t Format;         // FS3::VertexElementFormat
      //  uint8_t Usage;          // FS3::VertexElementUsage
      //  uint8_t UsageIndex;
      //};

      const uint32_t SIZEOF_MESH_LIST_HEADER = 4;

      //struct MeshList
      //{
      //  uint32_t Entries;
      //};

      const uint32_t MESH_OFFSET_MaterialIndex = 0;
      const uint32_t MESH_OFFSET_VertexCount = MESH_OFFSET_MaterialIndex + sizeof(uint32_t);
      const uint32_t MESH_OFFSET_IndexCount = MESH_OFFSET_VertexCount + sizeof(uint32_t);
      const uint32_t MESH_OFFSET_NameLength = MESH_OFFSET_IndexCount + sizeof(uint32_t);
      const uint32_t MESH_OFFSET_PrimitiveType = MESH_OFFSET_NameLength + sizeof(uint32_t);
      const uint32_t MESH_OFFSET_VertexDeclarationIndex = MESH_OFFSET_PrimitiveType + sizeof(uint8_t);
      const uint32_t MESH_OFFSET_IndexByteSize = MESH_OFFSET_VertexDeclarationIndex + sizeof(uint8_t);
      const uint32_t SIZEOF_MESH_HEADER = MESH_OFFSET_IndexByteSize + sizeof(uint8_t);

      const uint32_t MESH_HEADERS = (SIZEOF_MESH_HEADER > SIZEOF_MESH_LIST_HEADER ? SIZEOF_MESH_HEADER : SIZEOF_MESH_LIST_HEADER);

      //struct ChunkMeshHeader
      //{
      //  uint32_t MaterialIndex;
      //  uint32_t VertexCount;
      //  uint32_t IndexCount;
      //  uint32_t NameLength;
      //  uint8_t PrimitiveType;
      //  uint8_t VertexDeclarationIndex;
      //  uint8_t IndexByteSize;
      //};

      const uint32_t SIZEOF_NODELIST_HEADER = sizeof(uint32_t);

      const uint16_t NODE_OFFSET_Transformation = 0;
      const uint16_t NODE_OFFSET_NameLength = NODE_OFFSET_Transformation + (4 * 4 * 4);
      const uint16_t NODE_OFFSET_MeshCount = NODE_OFFSET_NameLength + sizeof(uint16_t);
      const uint16_t NODE_OFFSET_ChildCount = NODE_OFFSET_MeshCount + sizeof(uint8_t);
      const uint16_t SIZEOF_NODE_HEADER = NODE_OFFSET_ChildCount + sizeof(uint8_t);

      //struct ChunkNodeHeader
      //{
      //  Matrix Transformation;
      //  uint16_t NameLength;
      //  uint8_t MeshCount;
      //  uint8_t ChildCount;
      //};


      FormatHeader ReadHeader(std::ifstream& rStream)
      {
        uint8_t buffer[SIZE_OF_FORMATHEADER];
        rStream.read(reinterpret_cast<char*>(buffer), SIZE_OF_FORMATHEADER);
        if (!rStream.good())
          throw FormatException("Failed to read the expected data");

        const uint32_t magic = ByteArrayUtil::ReadUInt32LE(buffer, SIZE_OF_FORMATHEADER, FORMATHEADER_OFFSET_Magic);
        const uint32_t version = ByteArrayUtil::ReadUInt32LE(buffer, SIZE_OF_FORMATHEADER, FORMATHEADER_OFFSET_Version);

        if (magic != FORMAT_MAGIC || version != FORMAT_CURRENT_VERSION)
          throw NotSupportedException("File format not supported");
        return FormatHeader(magic, version);
      }


      void WriteHeader(std::ofstream& rStream, const FormatHeader& header)
      {
        uint8_t buffer[SIZE_OF_FORMATHEADER];
        ByteArrayUtil::WriteUInt32LE(buffer, SIZE_OF_FORMATHEADER, FORMATHEADER_OFFSET_Magic, header.Magic);
        ByteArrayUtil::WriteUInt32LE(buffer, SIZE_OF_FORMATHEADER, FORMATHEADER_OFFSET_Version, header.Version);
        rStream.write(reinterpret_cast<const char*>(buffer), SIZE_OF_FORMATHEADER);
      }


      ChunkHeader ReadChunkHeader(std::ifstream& rStream)
      {
        uint8_t buffer[SIZE_OF_CHUNKHEADER];
        rStream.read(reinterpret_cast<char*>(buffer), SIZE_OF_CHUNKHEADER);
        if (!rStream.good())
          throw FormatException("Failed to read the expected data");

        const uint32_t byteSize = ByteArrayUtil::ReadUInt32LE(buffer, SIZE_OF_FORMATHEADER, CHUNKHEADER_OFFSET_ByteSize);
        const uint8_t chunkType = ByteArrayUtil::ReadUInt8LE(buffer, SIZE_OF_FORMATHEADER, CHUNKHEADER_OFFSET_Type);
        //const uint8_t reserved = ByteArrayUtil::ReadUInt8LE(buffer, SIZE_OF_FORMATHEADER, CHUNKHEADER_OFFSET_Reserved);
        const uint16_t version = ByteArrayUtil::ReadUInt16LE(buffer, SIZE_OF_FORMATHEADER, CHUNKHEADER_OFFSET_Version);
        return ChunkHeader(byteSize, static_cast<ChunkType>(chunkType), version);
      }


      void WriteChunkHeader(std::ofstream& rStream, const ChunkHeader& header)
      {
        uint8_t buffer[SIZE_OF_CHUNKHEADER];
        ByteArrayUtil::WriteUInt32LE(buffer, SIZE_OF_FORMATHEADER, CHUNKHEADER_OFFSET_ByteSize, header.ByteSize);
        ByteArrayUtil::WriteUInt8LE(buffer, SIZE_OF_FORMATHEADER, CHUNKHEADER_OFFSET_Type, static_cast<uint8_t>(header.Type));
        ByteArrayUtil::WriteUInt8LE(buffer, SIZE_OF_FORMATHEADER, CHUNKHEADER_OFFSET_Reserved, header.Reserved);
        ByteArrayUtil::WriteUInt16LE(buffer, SIZE_OF_FORMATHEADER, CHUNKHEADER_OFFSET_Version, header.Version);
        rStream.write(reinterpret_cast<const char*>(buffer), SIZE_OF_CHUNKHEADER);
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
          const SFVertexDeclaration vertexDeclaration = Conversion::Convert(mesh->GetVertexDeclaration());
          const auto itrFind = std::find(rVertexDeclarations.begin(), rVertexDeclarations.end(), vertexDeclaration);
          const auto vertexDeclarationIndex = std::distance(rVertexDeclarations.begin(), itrFind);
          if (itrFind == rVertexDeclarations.end())
            rVertexDeclarations.push_back(vertexDeclaration);

          if (vertexDeclarationIndex > 255)
            throw NotSupportedException("The maximum number of vertex declarations exceeded");

          rMeshes.push_back(InternalMeshRecord(mesh, static_cast<uint8_t>(vertexDeclarationIndex)));
        }
      }


      uint32_t CalcByteSize(const std::deque<InternalVertexDeclaration>& vertexDeclarations)
      {
        uint32_t size = SIZEOF_VERTEX_DECLARATION_LIST_HEADER;

        auto itrDecl = vertexDeclarations.begin();
        while (itrDecl != vertexDeclarations.end())
        {
          // Write the vertex element count
          assert(itrDecl->Elements.size() <= std::numeric_limits<uint16_t>::max());
          const uint16_t elementCount = static_cast<uint16_t>(itrDecl->Elements.size());
          size += SIZEOF_VERTEX_DECLARATION_HEADER + (elementCount * SIZE_OF_VERTEXELEMENT);
          ++itrDecl;
        }
        return size;
      }


      SceneFormat::VertexElementFormat ConvertToFormat(const uint8_t value)
      {
        if (value <= static_cast<uint32_t>(SceneFormat::VertexElementFormat::SYS_COUNT))
          return static_cast<SceneFormat::VertexElementFormat>(value);
        throw FormatException("invalid vertex element format");
      }


      SceneFormat::VertexElementUsage ConvertToUsage(const uint8_t value)
      {
        if (value <= static_cast<uint32_t>(SceneFormat::VertexElementUsage::SYS_COUNT))
          return static_cast<SceneFormat::VertexElementUsage>(value);
        throw FormatException("invalid vertex element usage");
      }


      SceneFormat::PrimitiveType ConvertToPrimitiveType(const uint8_t value)
      {
        if (value <= static_cast<uint32_t>(SceneFormat::PrimitiveType::SYS_COUNT))
          return static_cast<SceneFormat::PrimitiveType>(value);
        throw FormatException("invalid primitive type");
      }


      //! @brief Read the unique vertex declarations
      void ReadVertexDeclarationsChunk(std::ifstream& rStream, std::deque<InternalVertexDeclaration>& rUniqueEntries)
      {
        const ChunkHeader header = ReadChunkHeader(rStream);
        if (header.Type != ChunkType::VertexDeclarations)
          throw NotSupportedException("format not correct");
        if (header.Version != CHUNK_VERSION_VertexDeclaration)
          throw FormatException("Unsupported vertex declaration chunk version");

        const auto startPos = rStream.tellg();

        // Read the vertex declarations
        uint8_t buffer[BUFFER_ENTRY_SIZE];
        rStream.read(reinterpret_cast<char*>(buffer), SIZEOF_VERTEX_DECLARATION_LIST_HEADER);
        if (!rStream.good())
          throw FormatException("Failed to read the expected data");

        // Read the number of vertex declarations
        const uint32_t numVertexDeclarations = ByteArrayUtil::ReadUInt32LE(buffer, BUFFER_ENTRY_SIZE, 0);

        for (uint32_t declarationIndex = 0; declarationIndex < numVertexDeclarations; ++declarationIndex)
        {
          SFVertexDeclaration vertexDecl;

          rStream.read(reinterpret_cast<char*>(buffer), SIZEOF_VERTEX_DECLARATION_HEADER);
          if (!rStream.good())
            throw FormatException("Failed to read the expected data");

          const uint16_t elementCount = ByteArrayUtil::ReadUInt16LE(buffer, BUFFER_ENTRY_SIZE, 0);
          for (uint16_t elementIndex = 0; elementIndex < elementCount; ++elementIndex)
          {
            rStream.read(reinterpret_cast<char*>(buffer), SIZE_OF_VERTEXELEMENT);
            if (!rStream.good())
              throw FormatException("Failed to read the expected data");

            const uint8_t format = ByteArrayUtil::ReadUInt8LE(buffer, BUFFER_ENTRY_SIZE, VERTEXELEMENT_OFFSET_Format);
            const uint8_t usage = ByteArrayUtil::ReadUInt8LE(buffer, BUFFER_ENTRY_SIZE, VERTEXELEMENT_OFFSET_Usage);
            const uint8_t usageIndex = ByteArrayUtil::ReadUInt8LE(buffer, BUFFER_ENTRY_SIZE, VERTEXELEMENT_OFFSET_UsageIndex);

            vertexDecl.Elements.push_back(SFVertexElement(ConvertToFormat(format), ConvertToUsage(usage), usageIndex));
          }

          rUniqueEntries.push_back(vertexDecl);
        }

        const auto endPos = rStream.tellg();
        assert(endPos >= startPos);
        if (static_cast<uint32_t>(endPos - startPos) != header.ByteSize)
          throw FormatException("VertexDeclarationChunk was of a unexpected size");
      }


      //! @brief Write the unique vertex declarations
      void WriteVertexDeclarationsChunk(std::ofstream& rStream, const std::deque<InternalVertexDeclaration>& uniqueEntries)
      {
        if (uniqueEntries.size() > std::numeric_limits<uint32_t>::max())
          throw NotSupportedException("Too many unique entries");

        const uint32_t cbContent = CalcByteSize(uniqueEntries);

        ChunkHeader header(cbContent, ChunkType::VertexDeclarations, CHUNK_VERSION_VertexDeclaration);
        WriteChunkHeader(rStream, header);

        // Write the vertex declarations
        uint8_t buffer[BUFFER_ENTRY_SIZE];
        // Write the number of vertex declarations
        ByteArrayUtil::WriteUInt32LE(buffer, BUFFER_ENTRY_SIZE, 0, static_cast<uint32_t>(uniqueEntries.size()));
        rStream.write(reinterpret_cast<const char*>(buffer), SIZEOF_VERTEX_DECLARATION_LIST_HEADER);

        auto itrDecl = uniqueEntries.begin();
        while (itrDecl != uniqueEntries.end())
        {
          // Write the vertex element count
          assert(itrDecl->Elements.size() <= std::numeric_limits<uint16_t>::max());
          const uint16_t elementCount = static_cast<uint16_t>(itrDecl->Elements.size());
          ByteArrayUtil::WriteUInt16LE(buffer, BUFFER_ENTRY_SIZE, 0, elementCount);
          rStream.write(reinterpret_cast<const char*>(buffer), SIZEOF_VERTEX_DECLARATION_HEADER);

          for (std::size_t i = 0; i < elementCount; ++i)
          {
            assert(static_cast<uint32_t>(itrDecl->Elements[i].Format) <= 255);
            assert(static_cast<uint32_t>(itrDecl->Elements[i].Usage) <= 255);
            assert(itrDecl->Elements[i].UsageIndex >= 0 && itrDecl->Elements[i].UsageIndex <= 255);

            const uint8_t format = static_cast<uint8_t>(itrDecl->Elements[i].Format);
            const uint8_t usage = static_cast<uint8_t>(itrDecl->Elements[i].Usage);
            const uint8_t usageIndex = itrDecl->Elements[i].UsageIndex;

            ByteArrayUtil::WriteUInt8LE(buffer, BUFFER_ENTRY_SIZE, VERTEXELEMENT_OFFSET_Format, format);
            ByteArrayUtil::WriteUInt8LE(buffer, BUFFER_ENTRY_SIZE, VERTEXELEMENT_OFFSET_Usage, usage);
            ByteArrayUtil::WriteUInt8LE(buffer, BUFFER_ENTRY_SIZE, VERTEXELEMENT_OFFSET_UsageIndex, usageIndex);
            rStream.write(reinterpret_cast<const char*>(buffer), SIZE_OF_VERTEXELEMENT);
          }

          ++itrDecl;
        }

        // TODO: Decide if we want to pad this chunk with zeros so that each chunk starts at a 32bit aligned address
      }


      //! @brief Examine the meshes in the scene and calculate the exact byte size needed to store the mesh chunk (without the chunk header)
      uint32_t CalcMeshesSize(const InternalSceneRecord& scene)
      {
        uint32_t cbMeshes = SIZEOF_MESH_LIST_HEADER;

        const auto& vertexDeclarations = scene.VertexDeclarations;
        const auto& meshes = scene.Meshes;
        std::shared_ptr<Mesh> mesh;

        for (auto itr = meshes.begin(); itr != meshes.end(); ++itr)
        {
          assert(itr->SourceMesh);
          const auto& mesh = itr->SourceMesh;
          cbMeshes += SIZEOF_MESH_HEADER;
          cbMeshes += vertexDeclarations[itr->VertexDeclarationIndex].VertexByteSize * mesh->GetVertexCount();
          cbMeshes += itr->IndexByteSize * mesh->GetIndexCount();
          cbMeshes += mesh->GetName().GetByteSize() + 1; // +1 because we write the terminating zero as well
        }
        return cbMeshes;
      }


      void ConvertFloat1ArrayLE(uint8_t*const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset, const std::size_t entries)
      {
        const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4;
        assert(pSrcArray != nullptr);
        assert((srcInterleaveOffset + elementSize) <= srcStride);
        assert((entries * srcStride) <= cbSrc);

        uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
        const uint8_t*const pSrcEnd = pSrcArray + cbSrc;

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


      void WriteFloat1ArrayLE(uint8_t* pDst, const std::size_t cbDst, const std::size_t dstIndex, const std::size_t dstStride, const std::size_t dstInterleaveOffset, const uint8_t*const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset, const std::size_t entries)
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
        const uint8_t*const pSrcEnd = pSrcArray + cbSrc;

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


      void ConvertFloat2ArrayLE(uint8_t*const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset, const std::size_t entries)
      {
        const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 2;
        assert(pSrcArray != nullptr);
        assert((srcInterleaveOffset + elementSize) <= srcStride);
        assert((entries * srcStride) <= cbSrc);

        uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
        const uint8_t*const pSrcEnd = pSrcArray + cbSrc;

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


      void WriteFloat2ArrayLE(uint8_t* pDst, const std::size_t cbDst, const std::size_t dstIndex, const std::size_t dstStride, const std::size_t dstInterleaveOffset, const uint8_t*const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset, const std::size_t entries)
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
        const uint8_t*const pSrcEnd = pSrcArray + cbSrc;

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


      void ConvertFloat3ArrayLE(uint8_t*const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset, const std::size_t entries)
      {
        const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 3;
        assert(pSrcArray != nullptr);
        assert((srcInterleaveOffset + elementSize) <= srcStride);
        assert((entries * srcStride) <= cbSrc);

        uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
        const uint8_t*const pSrcEnd = pSrcArray + cbSrc;

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


      void WriteFloat3ArrayLE(uint8_t* pDst, const std::size_t cbDst, const std::size_t dstIndex, const std::size_t dstStride, const std::size_t dstInterleaveOffset, const uint8_t*const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset, const std::size_t entries)
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
        const uint8_t*const pSrcEnd = pSrcArray + cbSrc;

        std::size_t currentDstIndex = dstIndex + dstInterleaveOffset;
        while (pSrc < pSrcEnd)
        {
          assert((currentDstIndex + 12) <= cbDst);
          // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
          //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
          ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex, *reinterpret_cast<const uint32_t*>(pSrc));
          ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex+4, *reinterpret_cast<const uint32_t*>(pSrc + 4));
          ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex+8, *reinterpret_cast<const uint32_t*>(pSrc + 8));
          pSrc += srcStride;
          currentDstIndex += dstStride;
        }
      }


      void ConvertFloat4ArrayLE(uint8_t*const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset, const std::size_t entries)
      {
        const std::size_t REMOVE_WARNING_VARIABLE_IS_NOT_USED elementSize = 4 * 4;
        assert(pSrcArray != nullptr);
        assert((srcInterleaveOffset + elementSize) <= srcStride);
        assert((entries * srcStride) <= cbSrc);

        uint8_t* pSrc = pSrcArray + srcInterleaveOffset;
        const uint8_t*const pSrcEnd = pSrcArray + cbSrc;

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


      void WriteFloat4ArrayLE(uint8_t* pDst, const std::size_t cbDst, const std::size_t dstIndex, const std::size_t dstStride, const std::size_t dstInterleaveOffset, const uint8_t*const pSrcArray, const std::size_t cbSrc, const uint32_t srcStride, const std::size_t srcInterleaveOffset, const std::size_t entries)
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
        const uint8_t*const pSrcEnd = pSrcArray + cbSrc;

        std::size_t currentDstIndex = dstIndex + dstInterleaveOffset;
        while (pSrc < pSrcEnd)
        {
          assert((currentDstIndex + 16) <= cbDst);
          // NOTE: this code assumes that a float is stored in the same endian format as a uint32 that is unfortunately not always the case :/
          //       But we do a runtime check earlier to verify that, so it ought to be detected before we get here
          ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex, *reinterpret_cast<const uint32_t*>(pSrc));
          ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex+4, *reinterpret_cast<const uint32_t*>(pSrc + 4));
          ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex+8, *reinterpret_cast<const uint32_t*>(pSrc + 8));
          ByteArrayUtil::WriteUInt32LE(pDst, cbDst, currentDstIndex+12, *reinterpret_cast<const uint32_t*>(pSrc + 12));
          pSrc += srcStride;
          currentDstIndex += dstStride;
        }

      }


      std::size_t LocateOffset(const VertexDeclaration& vertexDeclaration, const SceneFormat::VertexElementUsage usage, const uint8_t usageIndex)
      {
        for (std::size_t i = 0; i < vertexDeclaration.Count(); ++i)
        {
          const VertexElementEx element = vertexDeclaration.At(i);
          if (Conversion::Convert(element.Usage) == usage && element.UsageIndex == usageIndex)
            return element.Offset;
        }
        throw NotFoundException("Could not locate the requested vertex element");
      }


      //! @brief Convert the vertices in place from little endian format into the host format
      void ConvertVerticesLE(uint8_t*const pSrc, const std::size_t srcLength, const InternalVertexDeclaration& srcVertexDeclaration, const std::size_t vertexCount)
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
      std::size_t WriteVerticesLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const InternalMeshRecord& record, const std::deque<InternalVertexDeclaration>& dstVertexDeclarations)
      {
        const auto meshVertexDeclaration = record.SourceMesh->GetVertexDeclaration();
        const auto meshContent = record.SourceMesh->GenericDirectAccess();
        const auto srcVertexCount = meshContent.VertexCount;
        const auto srcVertexStride = meshContent.VertexStride;
        const uint8_t* pVertices = static_cast<const uint8_t*>(meshContent.pVertices);
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
            WriteFloat1ArrayLE(pDst, dstLength, dstIndex, dstStride, dstInterleaveOffset, pVertices, cbSrcVertices, srcVertexStride, srcInterleaveOffset, srcVertexCount);
            dstInterleaveOffset += 4;
            break;
          case SceneFormat::VertexElementFormat::Vector2:
            WriteFloat2ArrayLE(pDst, dstLength, dstIndex, dstStride, dstInterleaveOffset, pVertices, cbSrcVertices, srcVertexStride, srcInterleaveOffset, srcVertexCount);
            dstInterleaveOffset += 4 * 2;
            break;
          case SceneFormat::VertexElementFormat::Vector3:
            WriteFloat3ArrayLE(pDst, dstLength, dstIndex, dstStride, dstInterleaveOffset, pVertices, cbSrcVertices, srcVertexStride, srcInterleaveOffset, srcVertexCount);
            dstInterleaveOffset += 4 * 3;
            break;
          case SceneFormat::VertexElementFormat::Vector4:
            WriteFloat4ArrayLE(pDst, dstLength, dstIndex, dstStride, dstInterleaveOffset, pVertices, cbSrcVertices, srcVertexStride, srcInterleaveOffset, srcVertexCount);
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
      void ConvertIndicesLE(uint8_t*const pSrc, const std::size_t srcLength, const std::size_t srcIndexByteSize, const std::size_t indexCount)
      {
        assert(pSrc != nullptr);
        const std::size_t indexOffsetEnd = indexCount * srcIndexByteSize;

        if (srcIndexByteSize == 1)
          return;
        else if (srcIndexByteSize == 2)
        {
          for (std::size_t indexOffset = 0; indexOffset < indexOffsetEnd; indexOffset += srcIndexByteSize)
            *reinterpret_cast<uint16_t*>(pSrc + indexOffset) = ByteArrayUtil::ReadUInt16LE(pSrc, srcLength, indexOffset);
        }
        else
          throw NotSupportedException("Index byte size not supported");
      }


      //! @brief Read the indices in the host format and write them in little endian format
      std::size_t WriteIndicesLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const InternalMeshRecord& record)
      {
        assert(pDst != nullptr);

        const auto meshContent = record.SourceMesh->GenericDirectAccess();
        const auto indexCount = meshContent.IndexCount;
        const auto indexStride = meshContent.IndexStride;
        const uint8_t* pIndices = static_cast<const uint8_t*>(meshContent.pIndices);

        const std::size_t indexOffsetEnd = indexCount * indexStride;

        std::size_t currentDstIndex = dstIndex;
        if (record.IndexByteSize == 1)
        {
          for (std::size_t indexOffset = 0; indexOffset < indexOffsetEnd; indexOffset += indexStride)
            currentDstIndex += ByteArrayUtil::WriteUInt8LE(pDst, dstLength, currentDstIndex, *(pIndices + indexOffset));
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
          throw NotSupportedException("Index byte size not supported");
        return currentDstIndex - dstIndex;
      }


      //! @brief Validate that the string has the expected length and is zero terminated
      void ValidateStringLength(const uint8_t* pStr, const std::size_t expectedLength)
      {
        assert(pStr != nullptr);
        if (pStr[expectedLength] != 0)
          throw FormatException("No zero termination found at the end of the string");

        for (std::size_t i = 0; i < expectedLength; ++i)
        {
          if (pStr[i] == 0)
            throw FormatException("Encountered zero termination before string end");
        }
      }

      //! @Brief create a vertex declaration matching the InternalVertexDeclaration
      VertexDeclaration Create(const InternalVertexDeclaration& internalVertexDeclaration)
      {
        std::vector<VertexElementEx> elements(internalVertexDeclaration.Elements.size());

        auto dstItr = elements.begin();
        int32_t offset = 0;
        for (auto itr = internalVertexDeclaration.Elements.begin(); itr != internalVertexDeclaration.Elements.end(); ++itr)
        {
          *dstItr = VertexElementEx(offset, Conversion::Convert(itr->Format), Conversion::Convert(itr->Usage), itr->UsageIndex);
          ++dstItr;
          offset += InternalVertexDeclaration::CalcByteSize(*itr);
        }
        assert(offset == static_cast<int32_t>(internalVertexDeclaration.VertexByteSize));
        return VertexDeclaration(elements.data(), elements.size(), internalVertexDeclaration.VertexByteSize);
      }


      void AddMeshToScene(Scene& rScene, const MeshAllocatorFunc& meshAllocator,
                          const void*const pVertices, const std::size_t vertexCount, const InternalVertexDeclaration& srcInternalVertexDeclaration,
                          const void*const pIndices, const std::size_t indexCount, const uint8_t indexByteSize,
                          const SceneFormat::PrimitiveType primitiveType, const uint32_t materialIndex, const char*const pszName,
                          const void*const pDstDefaultValues, const int32_t cbDstDefaultValues)
      {
        if (materialIndex >= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
          throw FormatException("Material index is invalid");

        // Create the vertex declaration for the loaded file
        const VertexDeclaration srcVertexDeclaration(Create(srcInternalVertexDeclaration));

        // Create the mesh container object
        std::shared_ptr<Mesh> mesh = meshAllocator(vertexCount, indexCount, Conversion::Convert(primitiveType));
        mesh->SetMaterialIndex(static_cast<int32_t>(materialIndex));
        mesh->SetName(UTF8String(pszName));

        const auto cbSrcVertices = srcVertexDeclaration.VertexStride() * vertexCount;
        const auto cbSrcIndices = indexByteSize * indexCount;

        RawMeshContentEx rawDst = mesh->GenericDirectAccess();
        VertexConverter::GenericConvert(rawDst.pVertices, rawDst.VertexStride * rawDst.VertexCount, mesh->GetVertexDeclaration(), pVertices, cbSrcVertices, srcVertexDeclaration, vertexCount, pDstDefaultValues, cbDstDefaultValues);
        IndexConverter::GenericConvert(rawDst.pIndices, rawDst.IndexStride * rawDst.IndexCount, rawDst.IndexStride, pIndices, cbSrcIndices, indexByteSize, indexCount);

        rScene.AddMesh(mesh);
      }


      std::shared_ptr<Scene> ReadMeshesChunk(std::ifstream& rStream, const std::deque<InternalVertexDeclaration>& vertexDeclarations, const Graphics3D::SceneAllocatorFunc& sceneAllocator, const void*const pDstDefaultValues, const int32_t cbDstDefaultValues, const bool hostIsLittleEndian)
      {
        const ChunkHeader header = ReadChunkHeader(rStream);
        if (header.Type != ChunkType::Meshes)
          throw FormatException("Did not find the expected mesh chunk");
        if (header.Version != CHUNK_VERSION_Meshes)
          throw FormatException("Unsupported mesh chunk version");

        const auto startPos = rStream.tellg();

        uint8_t buffer[MESH_HEADERS];
        rStream.read(reinterpret_cast<char*>(buffer), SIZEOF_MESH_LIST_HEADER);
        if (!rStream.good())
          throw FormatException("Failed to read the expected data");

        const uint32_t meshCount = ByteArrayUtil::ReadUInt32LE(buffer, SIZEOF_MESH_LIST_HEADER, 0);

        // Create the scene
        std::shared_ptr<Scene> scene = sceneAllocator(meshCount);
        MeshAllocatorFunc meshAllocator = scene->GetMeshAllocator();

        for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
          rStream.read(reinterpret_cast<char*>(buffer), SIZEOF_MESH_HEADER);
          if (!rStream.good())
            throw FormatException("Failed to read the expected data");

          const uint32_t materialIndex = ByteArrayUtil::ReadUInt32LE(buffer, MESH_HEADERS, MESH_OFFSET_MaterialIndex);
          const uint32_t vertexCount = ByteArrayUtil::ReadUInt32LE(buffer, MESH_HEADERS, MESH_OFFSET_VertexCount);
          const uint32_t indexCount = ByteArrayUtil::ReadUInt32LE(buffer, MESH_HEADERS, MESH_OFFSET_IndexCount);
          const uint32_t nameLength = ByteArrayUtil::ReadUInt32LE(buffer, MESH_HEADERS, MESH_OFFSET_NameLength);
          const auto primitiveType = ConvertToPrimitiveType(ByteArrayUtil::ReadUInt8LE(buffer, MESH_HEADERS, MESH_OFFSET_PrimitiveType));
          const uint8_t vertexDeclarationIndex = ByteArrayUtil::ReadUInt8LE(buffer, MESH_HEADERS, MESH_OFFSET_VertexDeclarationIndex);
          const uint8_t indexByteSize = ByteArrayUtil::ReadUInt8LE(buffer, MESH_HEADERS, MESH_OFFSET_IndexByteSize);

          if (vertexDeclarationIndex >= vertexDeclarations.size())
            throw FormatException("Referenced a invalid vertex declaration");
          if (nameLength == 0)
            throw FormatException("the name is expected to be zero terminated, so a min length of 1 is expected");

          const uint32_t cbVertex = vertexDeclarations[vertexDeclarationIndex].VertexByteSize;
          const uint32_t cbVertices = cbVertex * vertexCount;
          const uint32_t cbIndices = indexByteSize * indexCount;
          std::vector<uint8_t> content(cbVertices + cbIndices + nameLength);

          uint8_t* pContent = content.data();

          // Read the actual data
          rStream.read(reinterpret_cast<char*>(pContent), content.size());
          if (!rStream.good())
            throw FormatException("Failed to read the vertices");

          const uint8_t*const pVertices = pContent;
          const uint8_t*const pIndices = pContent + cbVertices;
          if (hostIsLittleEndian)
          {
            ConvertVerticesLE(pContent, cbVertices, vertexDeclarations[vertexDeclarationIndex], vertexCount);
            pContent += cbVertices;

            assert(pContent == pIndices);
            ConvertIndicesLE(pContent, cbIndices, indexByteSize, indexCount);
            pContent += cbIndices;
          }
          else
            pContent += cbVertices + cbIndices;

          // Update mesh with the name
          ValidateStringLength(pContent, nameLength - 1);

          // Create the mesh and add it to the scene
          AddMeshToScene(*scene, meshAllocator, pVertices, vertexCount, vertexDeclarations[vertexDeclarationIndex], pIndices, indexCount, indexByteSize, primitiveType, materialIndex, reinterpret_cast<const char*>(pContent), pDstDefaultValues, cbDstDefaultValues);
        }

        const auto endPos = rStream.tellg();
        assert(endPos >= startPos);
        if (static_cast<uint32_t>(endPos - startPos) != header.ByteSize)
          throw FormatException("MeshesChunk was of a unexpected size");
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
        ChunkHeader header(cbMeshes, ChunkType::Meshes, CHUNK_VERSION_Meshes);
        WriteChunkHeader(rStream, header);

        const auto& vertexDeclarations = scene.VertexDeclarations;
        const auto& meshes = scene.Meshes;
        std::shared_ptr<Mesh> mesh;

        std::vector<uint8_t> content(cbMeshes);

        if (meshes.size() > std::numeric_limits<uint32_t>::max())
          throw NotSupportedException("Only support up to 32bit mesh count");

        std::size_t dstIndex = 0;
        dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, static_cast<uint32_t>(meshes.size()));
#if ! defined(NDEBUG)
        auto dstStartIndex = dstIndex;
#endif
        for (auto itr = meshes.begin(); itr != meshes.end(); ++itr)
        {
          assert(itr->SourceMesh);
          const auto& mesh = itr->SourceMesh;

          const int32_t vertexCount = mesh->GetVertexCount();
          const int32_t indexCount = mesh->GetIndexCount();
          const int32_t nameLength = mesh->GetName().GetByteSize() + 1; // +1 to write the terminating zero as well
          const int32_t materialIndex = mesh->GetMaterialIndex();
          const uint32_t primitiveType = static_cast<uint32_t>(Conversion::Convert(mesh->GetPrimitiveType()));
          assert(itr->IndexByteSize <= 255);
          const uint8_t indexType = static_cast<uint8_t>(itr->IndexByteSize);
          uint8_t vertexDeclarationIndex = itr->VertexDeclarationIndex;

          if (vertexCount < 0 || indexCount < 0 || nameLength < 0 || materialIndex < 0 || primitiveType > 255)
            throw NotSupportedException("Mesh format unsupported");

          // Write the mesh header
          assert((dstIndex - dstStartIndex) == (MESH_OFFSET_MaterialIndex));
          dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, static_cast<uint32_t>(materialIndex));
          assert((dstIndex - dstStartIndex) == (MESH_OFFSET_VertexCount));
          dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, static_cast<uint32_t>(vertexCount));
          assert((dstIndex - dstStartIndex) == (MESH_OFFSET_IndexCount));
          dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, static_cast<uint32_t>(indexCount));
          assert((dstIndex - dstStartIndex) == (MESH_OFFSET_NameLength));
          dstIndex += ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), dstIndex, static_cast<uint32_t>(nameLength));
          assert((dstIndex - dstStartIndex) == (MESH_OFFSET_PrimitiveType));
          dstIndex += ByteArrayUtil::WriteUInt8LE(content.data(), content.size(), dstIndex, static_cast<uint8_t>(primitiveType));
          assert((dstIndex - dstStartIndex) == (MESH_OFFSET_VertexDeclarationIndex));
          dstIndex += ByteArrayUtil::WriteUInt8LE(content.data(), content.size(), dstIndex, vertexDeclarationIndex);
          assert((dstIndex - dstStartIndex) == (MESH_OFFSET_IndexByteSize));
          dstIndex += ByteArrayUtil::WriteUInt8LE(content.data(), content.size(), dstIndex, indexType);

          // write vertex data
          dstIndex += WriteVerticesLE(content.data(), content.size(), dstIndex, *itr, vertexDeclarations);
          // write index data
          dstIndex += WriteIndicesLE(content.data(), content.size(), dstIndex, *itr);
          // write name
          dstIndex += ByteArrayUtil::WriteBytes(content.data(), content.size(), dstIndex, reinterpret_cast<const uint8_t*>(mesh->GetName().ToUTF8String().c_str()), nameLength);
#if ! defined(NDEBUG)
          dstStartIndex = dstIndex;
#endif
        }

        // Verify that we wrote the expected amount of bytes
        assert(static_cast<uint32_t>(dstIndex) == cbMeshes);

        rStream.write(reinterpret_cast<const char*>(content.data()), content.size());
      }


      struct NodeInfo
      {
        std::shared_ptr<SceneNode> Node;
        uint32_t Index;

        NodeInfo(const std::shared_ptr<SceneNode>& node, const uint32_t index)
          : Node(node)
          , Index(index)
        {
        }
      };

      struct NodeComp
      {
        const std::shared_ptr<SceneNode> Node;
        explicit NodeComp(const std::shared_ptr<SceneNode>& node)
          : Node(node)
        {
        }

        inline bool operator()(const NodeInfo& entry) const { return entry.Node == Node; }
      };


      std::size_t ReadNode(std::ifstream& rStream, std::deque<std::shared_ptr<SceneNode> >& rNodes, const std::vector<uint8_t>& srcBuffer, const std::size_t srcOffset, const uint32_t sceneMeshCount, const bool hostIsLittleEndian)
      {
        Matrix transform;
        uint32_t* pTransform = reinterpret_cast<uint32_t*>(transform.DirectAccess());

        std::size_t srcIndex = srcOffset;
        assert((srcIndex - srcOffset) == NODE_OFFSET_Transformation);
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
        assert((srcIndex - srcOffset) == NODE_OFFSET_NameLength);
        const uint16_t nameLength = ByteArrayUtil::ReadUInt16LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
        srcIndex += sizeof(uint16_t);
        assert((srcIndex - srcOffset) == NODE_OFFSET_MeshCount);
        const uint8_t nodeMeshCount = ByteArrayUtil::ReadUInt8LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
        srcIndex += sizeof(uint8_t);
        assert((srcIndex - srcOffset) == NODE_OFFSET_ChildCount);
        const uint8_t nodeChildCount = ByteArrayUtil::ReadUInt8LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
        srcIndex += sizeof(uint8_t);
        assert((srcIndex - srcOffset) == SIZEOF_NODE_HEADER);

        if (nameLength == 0)
          throw FormatException("the name is expected to be zero terminated, so a min length of 1 is expected");

        auto node = std::make_shared<SceneNode>(nodeMeshCount);

        // Read mesh indices
        for (std::size_t i = 0; i < nodeMeshCount; ++i)
        {
          const uint32_t index = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
          srcIndex += sizeof(uint32_t);
          if (index >= sceneMeshCount)
            throw FormatException("Invalid mesh index encountered");
          node->AddMesh(index);
        }

        // Read node indices
        for (std::size_t i = 0; i < nodeChildCount; ++i)
        {
          const uint32_t index = ByteArrayUtil::ReadUInt32LE(srcBuffer.data(), srcBuffer.size(), srcIndex);
          srcIndex += sizeof(uint32_t);
          if (index >= rNodes.size() )
            throw FormatException("Invalid child node index encountered");

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
        const std::size_t nameLength = (info.Node->GetName().GetByteSize() + 1); // +1 to write the terminating zero as well

        if (nameLength >= std::numeric_limits<uint16_t>::max())
          throw FormatException("Name size not supported");
        if (nodeMeshCount < 0 || nodeMeshCount >= std::numeric_limits<uint8_t>::max())
          throw FormatException("Mesh count not supported");
        if (nodeChildCount < 0 || nodeChildCount >= std::numeric_limits<uint8_t>::max())
          throw FormatException("Child count not supported");

        Matrix transform = info.Node->GetTransformation();
        const uint32_t* pTransform = reinterpret_cast<const uint32_t*>(transform.DirectAccess());

        std::size_t dstIndex = 0;
        assert(dstIndex == NODE_OFFSET_Transformation);
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
        assert(dstIndex == NODE_OFFSET_NameLength);
        dstIndex += ByteArrayUtil::WriteUInt16LE(rContent.data(), rContent.size(), dstIndex, static_cast<uint16_t>(nameLength));
        assert(dstIndex == NODE_OFFSET_MeshCount);
        dstIndex += ByteArrayUtil::WriteUInt8LE(rContent.data(), rContent.size(), dstIndex, static_cast<uint8_t>(nodeMeshCount));
        assert(dstIndex == NODE_OFFSET_ChildCount);
        dstIndex += ByteArrayUtil::WriteUInt8LE(rContent.data(), rContent.size(), dstIndex, static_cast<uint8_t>(nodeChildCount));
        assert(dstIndex == SIZEOF_NODE_HEADER);

        // write mesh indices
        for (int32_t i = 0; i < nodeMeshCount; ++i)
        {
          const int32_t meshIndex = info.Node->GetMeshAt(i);
          if (meshIndex < 0)
            throw FormatException("Negative mesh indices not supported");
          dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, static_cast<uint32_t>(meshIndex));
        }


        // write node indices
        // WARNING: the method used for finding the node index is far from optimal, but it works for now
        for (int32_t i = 0; i < nodeChildCount; ++i)
        {
          auto child = info.Node->GetChildAt(i);
          const auto itr = std::find_if(allNodes.begin(), allNodes.end(), NodeComp(child));
          if (itr == allNodes.end())
            throw FormatException("Node not found");
          dstIndex += ByteArrayUtil::WriteUInt32LE(rContent.data(), rContent.size(), dstIndex, itr->Index);
        }

        // write name
        dstIndex += ByteArrayUtil::WriteBytes(rContent.data(), rContent.size(), dstIndex, reinterpret_cast<const uint8_t*>(info.Node->GetName().ToUTF8String().c_str()), nameLength);

        rStream.write(reinterpret_cast<const char*>(rContent.data()), dstIndex);
      }


      void ReadNodesChunk(std::ifstream& rStream, Scene& rScene, const bool hostIsLittleEndian)
      {
        const ChunkHeader header = ReadChunkHeader(rStream);
        if (header.Type != ChunkType::Nodes)
          throw FormatException("Did not find the expected nodes chunk");
        if (header.Version != CHUNK_VERSION_Nodes )
          throw FormatException("Unsupported node chunk version");

        const auto startPos = rStream.tellg();

        std::vector<uint8_t> buffer(header.ByteSize);
        rStream.read(reinterpret_cast<char*>(buffer.data()), header.ByteSize);
        if (!rStream.good())
          throw FormatException("Failed to read the expected data");

        const uint32_t nodeCount = ByteArrayUtil::ReadUInt32LE(buffer.data(), SIZEOF_NODELIST_HEADER, 0);

        const uint32_t sceneMeshCount = static_cast<uint32_t>(rScene.GetMeshCount());
        std::deque<std::shared_ptr<SceneNode> > nodes;
        std::size_t srcOffset = SIZEOF_NODELIST_HEADER;
        for (uint32_t childIndex = 0; childIndex < nodeCount; ++childIndex)
          srcOffset += ReadNode(rStream, nodes, buffer, srcOffset, sceneMeshCount, hostIsLittleEndian);

        const auto endPos = rStream.tellg();
        assert(endPos >= startPos);
        if (static_cast<uint32_t>(endPos - startPos) != header.ByteSize)
          throw FormatException("NodeChunk was of a unexpected size");

        if (nodes.size() > 0)
          rScene.SetRootNode(nodes.back());
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
          return NodesInfo(0, 0);


        const auto nodeMeshCount = node->GetMeshCount();
        const auto nodeChildCount = node->GetChildCount();
        if (nodeMeshCount < 0 || nodeChildCount < 0)
          throw FormatException("Unsupported mesh or child count");


        std::size_t nodeByteSize = SIZEOF_NODE_HEADER;
        nodeByteSize += nodeMeshCount * sizeof(uint32_t);
        nodeByteSize += nodeChildCount * sizeof(uint32_t);
        nodeByteSize += node->GetName().GetByteSize() + 1; // +1 to write the terminating zero as well

        NodesInfo totalInfo(nodeByteSize, nodeByteSize);
        for (int32_t childIndex = 0; childIndex < nodeChildCount; ++childIndex)
        {
          NodesInfo info = ExamineNodes(rAllNodes, node->GetChildAt(childIndex));
          totalInfo.TotalByteSize += info.TotalByteSize;
          totalInfo.MaxNodeByteSize = std::max(totalInfo.MaxNodeByteSize, info.TotalByteSize);
        }

        // depth first storage
        assert(rAllNodes.size() <= std::numeric_limits<uint32_t>::max());
        rAllNodes.push_back(NodeInfo(node, static_cast<uint32_t>(rAllNodes.size())));
        return totalInfo;
      }


      void WriteNodesChunk(std::ofstream& rStream, const Scene& scene)
      {
        std::deque<NodeInfo> allNodes;
        NodesInfo nodesInfo = ExamineNodes(allNodes, scene.GetRootNode());

        assert(nodesInfo.TotalByteSize <= (std::numeric_limits<uint32_t>::max() - SIZEOF_NODELIST_HEADER));
        ChunkHeader header(static_cast<uint32_t>(SIZEOF_NODELIST_HEADER + nodesInfo.TotalByteSize), ChunkType::Nodes, CHUNK_VERSION_Nodes);
        WriteChunkHeader(rStream, header);

        std::vector<uint8_t> content(nodesInfo.MaxNodeByteSize);
        // Write the total node count
        assert(allNodes.size() <= std::numeric_limits<uint32_t>::max());
        ByteArrayUtil::WriteUInt32LE(content.data(), content.size(), 0, static_cast<uint32_t>(allNodes.size()));
        rStream.write(reinterpret_cast<const char*>(content.data()), SIZEOF_NODELIST_HEADER);

        for (auto itr = allNodes.begin(); itr != allNodes.end(); ++itr)
          WriteNode(rStream, content, *itr, allNodes);
      }


      //! @brief Verify that a float is 4bytes big and that the endian used by float and uint32_t is the same.
      //! @return true if the host is little endian
      bool CheckEndianAssumptions()
      {
        if (sizeof(float) != 4)
          throw NotSupportedException("We expect a float to be four bytes");

        // Runtime verification of endian assumptions
        uint32_t tmp;
        uint32_t* pUInt32 = reinterpret_cast<uint32_t*>(&tmp);
        float* pFloat = reinterpret_cast<float*>(&tmp);
        uint8_t* pTmp = reinterpret_cast<uint8_t*>(&tmp);
        *pUInt32 = 0x01020304;
        if (pTmp[0] == 0x04 && pTmp[1] == 0x03 && pTmp[2] == 0x02 && pTmp[3] == 0x01)
        {
          // little endian
          *pFloat = 0x01020304;
          if (pTmp[0] != 0x82 || pTmp[1] != 0x01 || pTmp[2] != 0x81 || pTmp[3] != 0x4B)
            throw NotSupportedException("The float is either not of the expected encoding or not stored in the same endian as a uint32_t");
          return true;
        }
        else if (pTmp[0] == 0x01 && pTmp[1] == 0x02 && pTmp[2] == 0x03 && pTmp[3] == 0x04)
        {
          // big endian
          *pFloat = 0x01020304;
          if (pTmp[0] != 0x4B || pTmp[1] != 0x81 || pTmp[2] != 0x01 || pTmp[3] != 0x82)
            throw NotSupportedException("The float is either not of the expected encoding or not stored in the same endian as a uint32_t");
          return false;
        }
        else
          throw NotSupportedException("Unsupported endian format");
      }
    }


    BasicSceneFormat::BasicSceneFormat()
      : m_sceneScratchpad(std::make_shared<InternalSceneRecord>())
      , m_hostIsLittleEndian(CheckEndianAssumptions())
    {
    }


    BasicSceneFormat::~BasicSceneFormat()
    {
    }


    std::shared_ptr<Graphics3D::Scene> BasicSceneFormat::GenericLoad(const IO::Path& filename, const Graphics3D::SceneAllocatorFunc& sceneAllocator, const void*const pDstDefaultValues, const int32_t cbDstDefaultValues)
    {
      std::ifstream fileStream(PATH_GET_NAME(filename), std::ios::in | std::ios::binary);
      return GenericLoad(fileStream, sceneAllocator, pDstDefaultValues, cbDstDefaultValues);
    }


    std::shared_ptr<Graphics3D::Scene> BasicSceneFormat::GenericLoad(std::ifstream& rStream, const Graphics3D::SceneAllocatorFunc& sceneAllocator, const void*const pDstDefaultValues, const int32_t cbDstDefaultValues)
    {
      try
      {
        // Read and validate header
        ReadHeader(rStream);
        m_sceneScratchpad->Clear();
        ReadVertexDeclarationsChunk(rStream, m_sceneScratchpad->VertexDeclarations);

        std::shared_ptr<Scene> scene = ReadMeshesChunk(rStream, m_sceneScratchpad->VertexDeclarations, sceneAllocator, pDstDefaultValues, cbDstDefaultValues, m_hostIsLittleEndian);

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
      std::ofstream fileStream(PATH_GET_NAME(strFilename), std::ios::out | std::ios::binary);
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
        FormatHeader header(FORMAT_MAGIC, FORMAT_CURRENT_VERSION);
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
}
