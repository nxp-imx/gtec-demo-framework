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

#include <FslAssimp/MeshHelper.hpp>
#include <FslAssimp/MeshImporter.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Span/SpanUtil_Create.hpp>
#include <FslBase/Span/TypedFlexSpanUtil.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics3D/BasicScene/Mesh.hpp>
#include <cassert>
#include <utility>

namespace Fsl
{
  using namespace Graphics3D;

  namespace
  {
    //! @brief extract indices as the requested type.
    //! @note Throws if the index doesn't fit the TIndex type
    template <typename TIndex>
    std::size_t DoFastExtractIndices(Span<TIndex> dstSpan, const aiMesh* const pSrcMesh)
    {
      assert(pSrcMesh != nullptr);

      const aiFace* pSrc = pSrcMesh->mFaces;
      if (pSrc == nullptr)
      {
        throw NotSupportedException("Model did not contain indices");
      }

      const std::size_t srcEntries = pSrcMesh->mNumFaces;

      std::size_t dstIndexCount = 0;
      for (std::size_t faceIndex = 0; faceIndex < srcEntries; ++faceIndex)
      {
        if (pSrc[faceIndex].mNumIndices != 3)
        {
          throw NotSupportedException("We only support triangle mesh faces");
        }

        auto* const srcIndices = pSrc[faceIndex].mIndices;
        for (std::size_t i = 0; i < 3; ++i)
        {
          if (dstIndexCount >= dstSpan.size())
          {
            throw IndexOutOfRangeException("The supplied buffer could not contain all the indices");
          }

          dstSpan[dstIndexCount] = static_cast<TIndex>(srcIndices[i]);
          ++dstIndexCount;
        }
      }
      return dstIndexCount;
    }


    void DoFastExtractVector2(TypedFlexSpan<Vector2> dst, const ReadOnlySpan<aiVector3D> src)
    {
      dst.Copy(src, [](const aiVector3D& srcVal) { return Vector2(srcVal.x, srcVal.y); });
    }

    void DoFastExtractVector3(TypedFlexSpan<Vector3> dst, const ReadOnlySpan<aiVector3D> src)
    {
      dst.Copy(src, [](const aiVector3D& srcVal) { return Vector3(srcVal.x, srcVal.y, srcVal.z); });
    }


    void DoFastExtractColors(TypedFlexSpan<Vector4> dst, const ReadOnlySpan<aiColor4D> src)
    {
      dst.Copy(src, [](const aiColor4D& srcVal) { return Vector4(srcVal.r, srcVal.g, srcVal.b, srcVal.a); });
    }

    void DoFastExtractColors(TypedFlexSpan<Color> dst, const ReadOnlySpan<aiColor4D> src)
    {
      dst.Copy(src, [](const aiColor4D& srcVal) { return Color(srcVal.r, srcVal.g, srcVal.b, srcVal.a); });
    }


    inline void MeshExtractPositions(const RawMeshContentEx& dstRawMeshContent, VertexDeclarationSpan dstVertexDeclaration,
                                     const aiMesh* const pSrcMesh, const Vector3& positionMod, const Vector3& scale)
    {
      // Extract the vertex positions (if present in both the model and the mesh we extract to)
      const auto indexVertices = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Position, 0);
      if (pSrcMesh->mVertices != nullptr && indexVertices >= 0)
      {
        VertexElement vertexElement = dstVertexDeclaration.At(indexVertices);
        if (vertexElement.Format != VertexElementFormat::Vector3)
        {
          throw NotSupportedException("We only support vertex positions of the type Vector3");
        }
        TypedFlexSpan<Vector3> dstSpan = TypedFlexSpanUtil::UnsafeVoidAsSpan<Vector3>(dstRawMeshContent.pVertices, dstRawMeshContent.VertexCount,
                                                                                      vertexElement.Offset, dstRawMeshContent.VertexStride);
        MeshImporter::FastExtractVertexPosition(dstSpan, pSrcMesh, positionMod, scale);
      }
    }


    // Extract the vertex normals (if present in both the model and the mesh we extract to)
    inline void MeshExtractNormals(const RawMeshContentEx& dstRawMeshContent, VertexDeclarationSpan dstVertexDeclaration,
                                   const aiMesh* const pSrcMesh)
    {
      const auto indexNormals = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Normal, 0);
      if (pSrcMesh->mNormals != nullptr && indexNormals >= 0)
      {
        VertexElement vertexElement = dstVertexDeclaration.At(indexNormals);
        if (vertexElement.Format != VertexElementFormat::Vector3)
        {
          throw NotSupportedException("We only support vertex normals of the type Vector3");
        }

        TypedFlexSpan<Vector3> dstSpan = TypedFlexSpanUtil::UnsafeVoidAsSpan<Vector3>(dstRawMeshContent.pVertices, dstRawMeshContent.VertexCount,
                                                                                      vertexElement.Offset, dstRawMeshContent.VertexStride);
        MeshImporter::FastExtractVertexNormals(dstSpan, pSrcMesh);
      }
    }


    // Extract the vertex tangent (if present in both the model and the mesh we extract to)
    inline void MeshExtractTangents(const RawMeshContentEx& dstRawMeshContent, VertexDeclarationSpan dstVertexDeclaration,
                                    const aiMesh* const pSrcMesh)
    {
      const auto indexTangents = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Tangent, 0);
      if (pSrcMesh->mTangents != nullptr && indexTangents >= 0)
      {
        VertexElement vertexElement = dstVertexDeclaration.At(indexTangents);
        if (vertexElement.Format != VertexElementFormat::Vector3)
        {
          throw NotSupportedException("We only support vertex tangents of the type Vector3");
        }
        TypedFlexSpan<Vector3> dstSpan = TypedFlexSpanUtil::UnsafeVoidAsSpan<Vector3>(dstRawMeshContent.pVertices, dstRawMeshContent.VertexCount,
                                                                                      vertexElement.Offset, dstRawMeshContent.VertexStride);
        MeshImporter::FastExtractVertexTangents(dstSpan, pSrcMesh);
      }
    }


    // Extract the vertex bitangents (if present in both the model and the mesh we extract to)
    inline void MeshExtractBitangents(const RawMeshContentEx& dstRawMeshContent, VertexDeclarationSpan dstVertexDeclaration,
                                      const aiMesh* const pSrcMesh)
    {
      const auto indexBitangents = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Bitangent, 0);
      if (pSrcMesh->mBitangents != nullptr && indexBitangents >= 0)
      {
        VertexElement vertexElement = dstVertexDeclaration.At(indexBitangents);
        if (vertexElement.Format != VertexElementFormat::Vector3)
        {
          throw NotSupportedException("We only support vertex bitangents of the type Vector3");
        }

        TypedFlexSpan<Vector3> dstSpan = TypedFlexSpanUtil::UnsafeVoidAsSpan<Vector3>(dstRawMeshContent.pVertices, dstRawMeshContent.VertexCount,
                                                                                      vertexElement.Offset, dstRawMeshContent.VertexStride);
        MeshImporter::FastExtractVertexBitangents(dstSpan, pSrcMesh);
      }
    }


    // Extract the vertex texture coordinates (if present in both the model and the mesh we extract to)
    inline void MeshExtractTextureCoordinates(const RawMeshContentEx& dstRawMeshContent, VertexDeclarationSpan dstVertexDeclaration,
                                              const aiMesh* const pSrcMesh)
    {
      for (int32_t usageIndex = 0; usageIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++usageIndex)
      {
        const auto indexTexture = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, usageIndex);
        if (pSrcMesh->mTextureCoords[usageIndex] != nullptr && indexTexture >= 0)
        {
          VertexElement vertexElement = dstVertexDeclaration.At(indexTexture);
          switch (vertexElement.Format)
          {
          case VertexElementFormat::Vector2:
            {
              TypedFlexSpan<Vector2> dstSpan = TypedFlexSpanUtil::UnsafeVoidAsSpan<Vector2>(
                dstRawMeshContent.pVertices, dstRawMeshContent.VertexCount, vertexElement.Offset, dstRawMeshContent.VertexStride);
              MeshImporter::FastExtractTextureCoordinates2(dstSpan, pSrcMesh, usageIndex);
              break;
            }
          case VertexElementFormat::Vector3:
            {
              TypedFlexSpan<Vector3> dstSpan = TypedFlexSpanUtil::UnsafeVoidAsSpan<Vector3>(
                dstRawMeshContent.pVertices, dstRawMeshContent.VertexCount, vertexElement.Offset, dstRawMeshContent.VertexStride);
              MeshImporter::FastExtractTextureCoordinates3(dstSpan, pSrcMesh, usageIndex);
              break;
            }
          default:
            throw NotSupportedException("We only support vertex texture coordinates of the type Vector2 or Vector3");
          }
        }
      }
    }

    // Extract the vertex colors (if present in both the model and the mesh we extract to)
    inline void MeshExtractColors(const RawMeshContentEx& dstRawMeshContent, VertexDeclarationSpan dstVertexDeclaration, const aiMesh* const pSrcMesh)
    {
      for (int32_t usageIndex = 0; usageIndex < AI_MAX_NUMBER_OF_COLOR_SETS; ++usageIndex)
      {
        const auto indexColor = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Color, usageIndex);
        if (indexColor >= 0)
        {
          VertexElement vertexElement = dstVertexDeclaration.At(indexColor);
          if (pSrcMesh->mNumVertices > dstRawMeshContent.VertexCount)
          {
            throw IndexOutOfRangeException("The buffer could not contain all the entries");
          }

          if (vertexElement.Format == VertexElementFormat::Vector4)
          {
            TypedFlexSpan<Vector4> dstSpan = TypedFlexSpanUtil::UnsafeVoidAsSpan<Vector4>(dstRawMeshContent.pVertices, dstRawMeshContent.VertexCount,
                                                                                          vertexElement.Offset, dstRawMeshContent.VertexStride)
                                               .subspan(0, pSrcMesh->mNumVertices);

            MeshImporter::FastExtractColors(dstSpan, pSrcMesh, usageIndex, Vector4::One());
          }
          else if (vertexElement.Format == VertexElementFormat::X8Y8Z8W8_UNORM)
          {
            TypedFlexSpan<Color> dstSpan = TypedFlexSpanUtil::UnsafeVoidAsSpan<Color>(dstRawMeshContent.pVertices, dstRawMeshContent.VertexCount,
                                                                                      vertexElement.Offset, dstRawMeshContent.VertexStride)
                                             .subspan(0, pSrcMesh->mNumVertices);

            MeshImporter::FastExtractColors(dstSpan, pSrcMesh, usageIndex, Colors::White());
          }
          else
          {
            throw NotSupportedException("We only support vertex colors of the type: Vector4, X8Y8Z8W8_UNORM");
          }
        }
      }
    }
  }


  MeshImporter::MeshImporter()
    : m_meshAllocator(MeshAllocator::AllocateComplex)
  {
  }


  MeshImporter::MeshImporter(MeshAllocatorFunc meshAllocator)
    : m_meshAllocator(std::move(meshAllocator))
  {
  }


  MeshAllocatorFunc MeshImporter::GetDefaultAllocator() const
  {
    return m_meshAllocator;
  }


  std::shared_ptr<Mesh> MeshImporter::ExtractMesh(const aiMesh* const pSrcMesh)
  {
    return ExtractMesh(m_meshAllocator, pSrcMesh);
  }


  std::shared_ptr<Mesh> MeshImporter::ExtractMesh(const aiMesh* const pSrcMesh, const Vector3& positionMod, const Vector3& scale)
  {
    return ExtractMesh(m_meshAllocator, pSrcMesh, positionMod, scale);
  }


  std::shared_ptr<Mesh> MeshImporter::ExtractMesh(const MeshAllocatorFunc& meshAllocator, const aiMesh* const pSrcMesh)
  {
    // FIX: use a non scaling version
    return ExtractMesh(meshAllocator, pSrcMesh, Vector3::Zero(), Vector3::One());
  }


  std::shared_ptr<Mesh> MeshImporter::ExtractMesh(const MeshAllocatorFunc& meshAllocator, const aiMesh* const pSrcMesh, const Vector3& positionMod,
                                                  const Vector3& scale)
  {
    if (pSrcMesh == nullptr)
    {
      throw std::invalid_argument("pSrcMesh can not be null");
    }

    const std::size_t numVertices = pSrcMesh->mNumVertices;
    const std::size_t numIndices = MeshHelper::CountIndices(pSrcMesh);

    // allocate a mesh instance
    auto mesh = meshAllocator(numVertices, numIndices, PrimitiveType::TriangleList);
    if (!mesh)
    {
      throw UsageErrorException("The allocator failed to allocate a mesh");
    }

    auto vertexDeclaration = mesh->AsVertexDeclarationSpan();
    RawMeshContentEx rawMeshContent = mesh->GenericDirectAccess();

    assert(rawMeshContent.pVertices != nullptr);
    assert(rawMeshContent.pIndices != nullptr);
    assert(rawMeshContent.IndexCount == numIndices);
    assert(rawMeshContent.VertexCount == numVertices);
    assert(rawMeshContent.VertexStride == std::size_t(vertexDeclaration.VertexStride()));

    {    // Extract the indices
      FastExtractIndices(rawMeshContent.pIndices, rawMeshContent.IndexCount, rawMeshContent.IndexStride, pSrcMesh);
    }

    {    // Extract the material index
      assert(pSrcMesh->mMaterialIndex <= std::size_t(std::numeric_limits<int32_t>::max()));
      mesh->SetMaterialIndex(static_cast<int32_t>(pSrcMesh->mMaterialIndex));
    }

    {    // Extract the name
      const UTF8String strName(pSrcMesh->mName.C_Str());
      mesh->SetName(strName);
    }

    MeshExtractPositions(rawMeshContent, vertexDeclaration, pSrcMesh, positionMod, scale);
    MeshExtractNormals(rawMeshContent, vertexDeclaration, pSrcMesh);
    MeshExtractTangents(rawMeshContent, vertexDeclaration, pSrcMesh);
    MeshExtractBitangents(rawMeshContent, vertexDeclaration, pSrcMesh);
    MeshExtractTextureCoordinates(rawMeshContent, vertexDeclaration, pSrcMesh);
    MeshExtractColors(rawMeshContent, vertexDeclaration, pSrcMesh);

    // FIX: missing extracts
    // - Bones
    return mesh;
  }


  std::size_t MeshImporter::FastExtractVertexPosition(TypedFlexSpan<Vector3> dst, const aiMesh* const pSrcMesh)
  {
    assert(pSrcMesh != nullptr);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(dst, SpanUtil::CreateReadOnly(pSrcMesh->mVertices, numSrcEntries));
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractVertexPosition(TypedFlexSpan<Vector3> dst, const aiMesh* const pSrcMesh, const Vector3& positionMod,
                                                      const Vector3& scale)
  {
    assert(pSrcMesh != nullptr);

    const auto src = SpanUtil::CreateReadOnly(pSrcMesh->mVertices, pSrcMesh->mNumVertices);

    dst.Copy(src, [positionMod, scale](const aiVector3D& srcVal)
             { return Vector3((srcVal.x + positionMod.X) * scale.X, (srcVal.y + positionMod.Y) * scale.Y, (srcVal.z + positionMod.Z) * scale.Z); });

    return src.size();
  }


  std::size_t MeshImporter::FastExtractVertexNormals(TypedFlexSpan<Vector3> dst, const aiMesh* const pSrcMesh)
  {
    assert(pSrcMesh != nullptr);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(dst, SpanUtil::CreateReadOnly(pSrcMesh->mNormals, numSrcEntries));
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractVertexTangents(TypedFlexSpan<Vector3> dst, const aiMesh* const pSrcMesh)
  {
    assert(pSrcMesh != nullptr);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(dst, SpanUtil::CreateReadOnly(pSrcMesh->mTangents, numSrcEntries));
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractVertexBitangents(TypedFlexSpan<Vector3> dst, const aiMesh* const pSrcMesh)
  {
    assert(pSrcMesh != nullptr);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(dst, SpanUtil::CreateReadOnly(pSrcMesh->mBitangents, numSrcEntries));
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractTextureCoordinates2(TypedFlexSpan<Vector2> dst, const aiMesh* const pSrcMesh, const int32_t srcSetIndex)
  {
    assert(pSrcMesh != nullptr);
    assert(srcSetIndex >= 0);
    assert(srcSetIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector2(dst, SpanUtil::CreateReadOnly(pSrcMesh->mTextureCoords[srcSetIndex], numSrcEntries));
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractTextureCoordinates3(TypedFlexSpan<Vector3> dst, const aiMesh* const pSrcMesh, const int32_t srcSetIndex)
  {
    assert(pSrcMesh != nullptr);
    assert(srcSetIndex >= 0);
    assert(srcSetIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(dst, SpanUtil::CreateReadOnly(pSrcMesh->mTextureCoords[srcSetIndex], numSrcEntries));
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractColors(TypedFlexSpan<Vector4> dst, const aiMesh* const pSrcMesh, const int32_t srcSetIndex)
  {
    assert(pSrcMesh != nullptr);
    assert(srcSetIndex >= 0);
    assert(srcSetIndex < AI_MAX_NUMBER_OF_COLOR_SETS);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractColors(dst, SpanUtil::CreateReadOnly(pSrcMesh->mColors[srcSetIndex], numSrcEntries));
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractColors(TypedFlexSpan<Color> dst, const aiMesh* const pSrcMesh, const int32_t srcSetIndex)
  {
    assert(pSrcMesh != nullptr);
    assert(srcSetIndex >= 0);
    assert(srcSetIndex < AI_MAX_NUMBER_OF_COLOR_SETS);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractColors(dst, SpanUtil::CreateReadOnly(pSrcMesh->mColors[srcSetIndex], numSrcEntries));
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractIndices(void* pDst, const std::size_t dstCapacity, const std::size_t cbDstIndexEntry,
                                               const aiMesh* const pSrcMesh)
  {
    switch (cbDstIndexEntry)
    {
    case 1:
      return FastExtractIndicesUInt8(SpanUtil::Create(static_cast<uint8_t*>(pDst), dstCapacity), pSrcMesh);
    case 2:
      return FastExtractIndicesUInt16(SpanUtil::Create(static_cast<uint16_t*>(pDst), dstCapacity), pSrcMesh);
    default:
      throw NotSupportedException("The index size is not supported");
    }
  }


  std::size_t MeshImporter::FastExtractIndicesUInt8(Span<uint8_t> dstSpan, const aiMesh* const pSrcMesh)
  {
    return DoFastExtractIndices(dstSpan, pSrcMesh);
  }

  std::size_t MeshImporter::FastExtractIndicesUInt16(Span<uint16_t> dstSpan, const aiMesh* const pSrcMesh)
  {
    return DoFastExtractIndices(dstSpan, pSrcMesh);
  }
}
