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

#include <FslAssimp/MeshImporter.hpp>
#include <FslAssimp/MeshHelper.hpp>
#include <FslGraphics3D/BasicScene/Mesh.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <cassert>

namespace Fsl
{
  using namespace Graphics3D;

  namespace
  {
    //! @brief extract indices as the requested type.
    //! @note Throws if the index doesn't fit the TIndex type
    template<typename TIndex>
    std::size_t DoFastExtractIndices(TIndex*const pDst, const std::size_t dstCapacity, const aiMesh*const pSrcMesh)
    {
      assert(pDst != nullptr);
      assert(pSrcMesh != nullptr);

      const aiFace* pSrc = pSrcMesh->mFaces;
      if (pSrc == nullptr)
        throw NotSupportedException("Model did not contain indices");

      const std::size_t srcEntries = pSrcMesh->mNumFaces;

      std::size_t dstIndexCount = 0;
      for (std::size_t faceIndex = 0; faceIndex < srcEntries; ++faceIndex)
      {
        if (pSrc[faceIndex].mNumIndices != 3)
          throw NotSupportedException("We only support triangle mesh faces");

        const auto srcIndices = pSrc[faceIndex].mIndices;
        for (std::size_t i = 0; i < 3; ++i)
        {
          if (dstIndexCount >= dstCapacity)
            throw IndexOutOfRangeException("The supplied buffer could not contain all the indices");

          pDst[dstIndexCount] = static_cast<TIndex>(srcIndices[i]);
          ++dstIndexCount;
        }
      }
      return dstIndexCount;
    }


    void DoFastExtractVector2(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiVector3D*const pSrc, const std::size_t numSrcEntries)
    {
      assert(pDst != nullptr);
      assert((dstEntryOffset + sizeof(Vector2)) <= dstStride);

      if (pSrc == nullptr)
        throw NotSupportedException("The mesh did not contain the requested entry");

      if ((numSrcEntries * dstStride) > cbDst)
        throw IndexOutOfRangeException("The buffer could not contain all the entries");

      uint8_t* pDstBuffer = static_cast<uint8_t*>(pDst)+dstEntryOffset;
      for (std::size_t i = 0; i < numSrcEntries; ++i)
      {
        *reinterpret_cast<Vector2*>(pDstBuffer) = Vector2(pSrc[i].x, pSrc[i].y);
        pDstBuffer += dstStride;
      }
    }

    void DoFastExtractVector3(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiVector3D*const pSrc, const std::size_t numSrcEntries)
    {
      assert(pDst != nullptr);
      assert((dstEntryOffset + sizeof(Vector3)) <= dstStride);

      if(pSrc == nullptr)
        throw NotSupportedException("The mesh did not contain the requested entry");

      if ((numSrcEntries * dstStride) > cbDst)
        throw IndexOutOfRangeException("The buffer could not contain all the entries");

      uint8_t* pDstBuffer = static_cast<uint8_t*>(pDst)+dstEntryOffset;
      for (std::size_t i = 0; i < numSrcEntries; ++i)
      {
        *reinterpret_cast<Vector3*>(pDstBuffer) = Vector3(pSrc[i].x, pSrc[i].y, pSrc[i].z);
        pDstBuffer += dstStride;
      }
    }


    void DoFastExtractVector4(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiColor4D*const pSrc, const std::size_t numSrcEntries)
    {
      assert(pDst != nullptr);
      assert((dstEntryOffset + sizeof(Vector4)) <= dstStride);

      if (pSrc == nullptr)
        throw NotSupportedException("The mesh did not contain the requested entry");

      if ((numSrcEntries * dstStride) > cbDst)
        throw IndexOutOfRangeException("The buffer could not contain all the entries");

      uint8_t* pDstBuffer = static_cast<uint8_t*>(pDst)+dstEntryOffset;
      for (std::size_t i = 0; i < numSrcEntries; ++i)
      {
        *reinterpret_cast<Vector4*>(pDstBuffer) = Vector4(pSrc[i].r, pSrc[i].g, pSrc[i].b, pSrc[i].a);
        pDstBuffer += dstStride;
      }
    }


    void FillVector4(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const Vector4& val, std::size_t numEntries)
    {
      assert(pDst != nullptr);
      assert((dstEntryOffset + sizeof(Vector4)) <= dstStride);

      if ((numEntries * dstStride) > cbDst)
        throw IndexOutOfRangeException("The buffer could not contain all the entries");

      uint8_t* pDstBuffer = static_cast<uint8_t*>(pDst)+dstEntryOffset;
      for (std::size_t i = 0; i < numEntries; ++i)
      {
        *reinterpret_cast<Vector4*>(pDstBuffer) = val;
        pDstBuffer += dstStride;
      }
    }


    inline void MeshExtractPositions(const RawMeshContentEx& dstRawMeshContent, const VertexDeclaration& dstVertexDeclaration, const aiMesh*const pSrcMesh, const Vector3& positionMod, const Vector3& scale)
    {
      // Extract the vertex positions (if present in both the model and the mesh we extract to)
      const auto indexVertices = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Position, 0);
      if (pSrcMesh->mVertices != nullptr && indexVertices >= 0)
      {
        VertexElementEx vertexElement = dstVertexDeclaration.At(indexVertices);
        if (vertexElement.Format != VertexElementFormat::Vector3)
          throw NotSupportedException("We only support vertex positions of the type Vector3");
        uint8_t* pDst = static_cast<uint8_t*>(dstRawMeshContent.pVertices);

        const std::size_t cbDstBuffer = (dstRawMeshContent.VertexCount * dstRawMeshContent.VertexStride);
        MeshImporter::FastExtractVertexPosition(pDst, cbDstBuffer, dstRawMeshContent.VertexStride, static_cast<size_t>(vertexElement.Offset), pSrcMesh, positionMod, scale);
      }
    }


    // Extract the vertex normals (if present in both the model and the mesh we extract to)
    inline void MeshExtractNormals(const RawMeshContentEx& dstRawMeshContent, const VertexDeclaration& dstVertexDeclaration, const aiMesh*const pSrcMesh)
    {
      const auto indexNormals = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Normal, 0);
      if (pSrcMesh->mNormals != nullptr && indexNormals >= 0)
      {
        VertexElementEx vertexElement = dstVertexDeclaration.At(indexNormals);
        if (vertexElement.Format != VertexElementFormat::Vector3)
          throw NotSupportedException("We only support vertex normals of the type Vector3");
        uint8_t* pDst = static_cast<uint8_t*>(dstRawMeshContent.pVertices);

        const std::size_t cbDstBuffer = (dstRawMeshContent.VertexCount * dstRawMeshContent.VertexStride);
        MeshImporter::FastExtractVertexNormals(pDst, cbDstBuffer, dstRawMeshContent.VertexStride, static_cast<size_t>(vertexElement.Offset), pSrcMesh);
      }
    }


    // Extract the vertex tangent (if present in both the model and the mesh we extract to)
    inline void MeshExtractTangents(const RawMeshContentEx& dstRawMeshContent, const VertexDeclaration& dstVertexDeclaration, const aiMesh*const pSrcMesh)
    {
      const auto indexTangents = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Tangent, 0);
      if (pSrcMesh->mTangents != nullptr && indexTangents >= 0)
      {
        VertexElementEx vertexElement = dstVertexDeclaration.At(indexTangents);
        if (vertexElement.Format != VertexElementFormat::Vector3)
          throw NotSupportedException("We only support vertex tangents of the type Vector3");
        uint8_t* pDst = static_cast<uint8_t*>(dstRawMeshContent.pVertices);

        const std::size_t cbDstBuffer = (dstRawMeshContent.VertexCount * dstRawMeshContent.VertexStride);
        MeshImporter::FastExtractVertexTangents(pDst, cbDstBuffer, dstRawMeshContent.VertexStride, static_cast<size_t>(vertexElement.Offset), pSrcMesh);
      }
    }


    // Extract the vertex bitangents (if present in both the model and the mesh we extract to)
    inline void MeshExtractBitangents(const RawMeshContentEx& dstRawMeshContent, const VertexDeclaration& dstVertexDeclaration, const aiMesh*const pSrcMesh)
    {
      const auto indexBitangents = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Bitangent, 0);
      if (pSrcMesh->mBitangents != nullptr && indexBitangents >= 0)
      {
        VertexElementEx vertexElement = dstVertexDeclaration.At(indexBitangents);
        if (vertexElement.Format != VertexElementFormat::Vector3)
          throw NotSupportedException("We only support vertex bitangents of the type Vector3");
        uint8_t* pDst = static_cast<uint8_t*>(dstRawMeshContent.pVertices);

        const std::size_t cbDstBuffer = (dstRawMeshContent.VertexCount * dstRawMeshContent.VertexStride);
        MeshImporter::FastExtractVertexBitangents(pDst, cbDstBuffer, dstRawMeshContent.VertexStride, static_cast<size_t>(vertexElement.Offset), pSrcMesh);
      }
    }


    // Extract the vertex texture coordinates (if present in both the model and the mesh we extract to)
    inline void MeshExtractTextureCoordinates(const RawMeshContentEx& dstRawMeshContent, const VertexDeclaration& dstVertexDeclaration, const aiMesh*const pSrcMesh)
    {
      for (int32_t usageIndex = 0; usageIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++usageIndex)
      {
        const auto indexTexture = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, usageIndex);
        if (pSrcMesh->mTextureCoords[usageIndex] != nullptr && indexTexture >= 0)
        {
          VertexElementEx vertexElement = dstVertexDeclaration.At(indexTexture);
          uint8_t* pDst = static_cast<uint8_t*>(dstRawMeshContent.pVertices);
          const std::size_t cbDstBuffer = (dstRawMeshContent.VertexCount * dstRawMeshContent.VertexStride);
          const std::size_t dstEntryOffset = static_cast<size_t>(vertexElement.Offset);
          switch (vertexElement.Format)
          {
          case VertexElementFormat::Vector2:
            MeshImporter::FastExtractTextureCoordinates2(pDst, cbDstBuffer, dstRawMeshContent.VertexStride, dstEntryOffset, pSrcMesh, usageIndex);
            break;
          case VertexElementFormat::Vector3:
            MeshImporter::FastExtractTextureCoordinates3(pDst, cbDstBuffer, dstRawMeshContent.VertexStride, dstEntryOffset, pSrcMesh, usageIndex);
            break;
          default:
            throw NotSupportedException("We only support vertex texture coordinates of the type Vector2 or Vector3");
          }
        }
      }
    }

    // Extract the vertex colors (if present in both the model and the mesh we extract to)
    inline void MeshExtractColors(const RawMeshContentEx& dstRawMeshContent, const VertexDeclaration& dstVertexDeclaration, const aiMesh*const pSrcMesh)
    {
      for (int32_t usageIndex = 0; usageIndex < AI_MAX_NUMBER_OF_COLOR_SETS; ++usageIndex)
      {
        const auto indexColor = dstVertexDeclaration.VertexElementIndexOf(VertexElementUsage::Color, usageIndex);
        if (indexColor >= 0)
        {
          VertexElementEx vertexElement = dstVertexDeclaration.At(indexColor);
          if (vertexElement.Format != VertexElementFormat::Vector4)
            throw NotSupportedException("We only support vertex colors of the type Vector4");

          uint8_t* pDst = static_cast<uint8_t*>(dstRawMeshContent.pVertices);
          const std::size_t cbDstBuffer = (dstRawMeshContent.VertexCount * dstRawMeshContent.VertexStride);
          if (pSrcMesh->mColors[usageIndex] != nullptr)
            MeshImporter::FastExtractColors(pDst, cbDstBuffer, dstRawMeshContent.VertexStride, static_cast<size_t>(vertexElement.Offset), pSrcMesh, usageIndex);
          else
            FillVector4(pDst, cbDstBuffer, dstRawMeshContent.VertexStride, static_cast<size_t>(vertexElement.Offset), Vector4::One(), pSrcMesh->mNumVertices);
        }
      }
    }

  }


  MeshImporter::MeshImporter()
    : m_meshAllocator(MeshAllocator::AllocateComplex)
  {
  }


  MeshImporter::MeshImporter(const MeshAllocatorFunc& meshAllocator)
    : m_meshAllocator(meshAllocator)
  {
  }


  MeshAllocatorFunc MeshImporter::GetDefaultAllocator() const
  {
    return m_meshAllocator;
  }


  std::shared_ptr<Mesh> MeshImporter::ExtractMesh(const aiMesh*const pSrcMesh)
  {
    return ExtractMesh(m_meshAllocator, pSrcMesh);
  }


  std::shared_ptr<Mesh> MeshImporter::ExtractMesh(const aiMesh*const pSrcMesh, const Vector3& positionMod, const Vector3& scale)
  {
    return ExtractMesh(m_meshAllocator, pSrcMesh, positionMod, scale);
  }


  std::shared_ptr<Mesh> MeshImporter::ExtractMesh(const MeshAllocatorFunc& meshAllocator, const aiMesh*const pSrcMesh)
  {
    // FIX: use a non scaling version
    return ExtractMesh(meshAllocator, pSrcMesh, Vector3::Zero(), Vector3::One());
  }


  std::shared_ptr<Mesh> MeshImporter::ExtractMesh(const MeshAllocatorFunc& meshAllocator, const aiMesh*const pSrcMesh, const Vector3& positionMod, const Vector3& scale)
  {
    if (pSrcMesh == nullptr)
      throw std::invalid_argument("pSrcMesh can not be null");

    const std::size_t numVertices = pSrcMesh->mNumVertices;
    const std::size_t numIndices = MeshHelper::CountIndices(pSrcMesh);

    // allocate a mesh instance
    auto mesh = meshAllocator(numVertices, numIndices, PrimitiveType::TriangleList);
    if (!mesh)
      throw UsageErrorException("The allocator failed to allocate a mesh");

    auto vertexDeclaration = mesh->GetVertexDeclaration();
    RawMeshContentEx rawMeshContent = mesh->GenericDirectAccess();

    assert(rawMeshContent.pVertices != nullptr);
    assert(rawMeshContent.pIndices != nullptr);
    assert(rawMeshContent.IndexCount == numIndices);
    assert(rawMeshContent.VertexCount == numVertices);
    assert(rawMeshContent.VertexStride == std::size_t(vertexDeclaration.VertexStride()));

    { // Extract the indices
      FastExtractIndices(rawMeshContent.pIndices, rawMeshContent.IndexCount, rawMeshContent.IndexStride, pSrcMesh);
    }

    { // Extract the material index
      assert(pSrcMesh->mMaterialIndex <= std::size_t(std::numeric_limits<int32_t>::max()));
      mesh->SetMaterialIndex(static_cast<int32_t>(pSrcMesh->mMaterialIndex));
    }

    { // Extract the name
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


  std::size_t MeshImporter::FastExtractVertexPosition(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiMesh*const pSrcMesh)
  {
    assert(pSrcMesh != nullptr);
    assert((dstEntryOffset + sizeof(Vector3)) <= dstStride);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(pDst, cbDst, dstStride, dstEntryOffset, pSrcMesh->mVertices, numSrcEntries);
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractVertexPosition(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiMesh*const pSrcMesh, const Vector3& positionMod, const Vector3& scale)
  {
    assert(pDst != nullptr);
    assert(pSrcMesh != nullptr);
    assert((dstEntryOffset + sizeof(Vector3)) <= dstStride);

    const aiVector3D* pSrc = pSrcMesh->mVertices;
    const std::size_t srcEntries = pSrcMesh->mNumVertices;

    if (pSrc == nullptr)
      throw NotSupportedException("The mesh did not contain any vertex positions");
    if ((srcEntries * dstStride) > cbDst)
      throw IndexOutOfRangeException("The buffer could not contain all the vertex positions");

    uint8_t* pDstBuffer = static_cast<uint8_t*>(pDst) + dstEntryOffset;
    for (std::size_t i = 0; i < srcEntries; ++i)
    {
      *reinterpret_cast<Vector3*>(pDstBuffer) = Vector3((pSrc[i].x + positionMod.X) * scale.X, (pSrc[i].y + positionMod.Y) * scale.Y, (pSrc[i].z + positionMod.Z) * scale.Z);
      pDstBuffer += dstStride;
    }
    return srcEntries;
  }


  std::size_t MeshImporter::FastExtractVertexNormals(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiMesh*const pSrcMesh)
  {
    assert(pSrcMesh != nullptr);
    assert((dstEntryOffset + sizeof(Vector3)) <= dstStride);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(pDst, cbDst, dstStride, dstEntryOffset, pSrcMesh->mNormals, numSrcEntries);
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractVertexTangents(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiMesh*const pSrcMesh)
  {
    assert(pSrcMesh != nullptr);
    assert((dstEntryOffset + sizeof(Vector3)) <= dstStride);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(pDst, cbDst, dstStride, dstEntryOffset, pSrcMesh->mTangents, numSrcEntries);
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractVertexBitangents(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiMesh*const pSrcMesh)
  {
    assert(pSrcMesh != nullptr);
    assert((dstEntryOffset + sizeof(Vector3)) <= dstStride);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(pDst, cbDst, dstStride, dstEntryOffset, pSrcMesh->mBitangents, numSrcEntries);
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractTextureCoordinates2(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiMesh*const pSrcMesh, const int32_t srcSetIndex)
  {
    assert(pSrcMesh != nullptr);
    assert(srcSetIndex >= 0);
    assert(srcSetIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS);
    assert((dstEntryOffset + sizeof(Vector2)) <= dstStride);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector2(pDst, cbDst, dstStride, dstEntryOffset, pSrcMesh->mTextureCoords[srcSetIndex], numSrcEntries);
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractTextureCoordinates3(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiMesh*const pSrcMesh, const int32_t srcSetIndex)
  {
    assert(pSrcMesh != nullptr);
    assert(srcSetIndex >= 0);
    assert(srcSetIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS);
    assert((dstEntryOffset + sizeof(Vector3)) <= dstStride);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector3(pDst, cbDst, dstStride, dstEntryOffset, pSrcMesh->mTextureCoords[srcSetIndex], numSrcEntries);
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractColors(void* pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const aiMesh*const pSrcMesh, const int32_t srcSetIndex)
  {
    assert(pSrcMesh != nullptr);
    assert(srcSetIndex >= 0);
    assert(srcSetIndex < AI_MAX_NUMBER_OF_COLOR_SETS);
    assert((dstEntryOffset + sizeof(Vector4)) <= dstStride);

    const std::size_t numSrcEntries = pSrcMesh->mNumVertices;
    DoFastExtractVector4(pDst, cbDst, dstStride, dstEntryOffset, pSrcMesh->mColors[srcSetIndex], numSrcEntries);
    return numSrcEntries;
  }


  std::size_t MeshImporter::FastExtractIndices(void* pDst, const std::size_t dstCapacity, const std::size_t cbDstIndexEntry, const aiMesh*const pSrcMesh)
  {
    switch (cbDstIndexEntry)
    {
    case 1:
      return FastExtractIndicesUInt8(static_cast<uint8_t*>(pDst), dstCapacity, pSrcMesh);
    case 2:
      return FastExtractIndicesUInt16(static_cast<uint16_t*>(pDst), dstCapacity, pSrcMesh);
    default:
      throw NotSupportedException("The index size is not supported");
    }
  }


  std::size_t MeshImporter::FastExtractIndicesUInt8(uint8_t*const pDst, const std::size_t dstCapacity, const aiMesh*const pSrcMesh)
  {
    return DoFastExtractIndices(pDst, dstCapacity, pSrcMesh);
  }

  std::size_t MeshImporter::FastExtractIndicesUInt16(uint16_t*const pDst, const std::size_t dstCapacity, const aiMesh*const pSrcMesh)
  {
    return DoFastExtractIndices(pDst, dstCapacity, pSrcMesh);
  }

}
