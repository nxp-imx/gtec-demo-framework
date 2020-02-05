/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <Shared/ModelViewer/MeshUtil.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <algorithm>
#include <map>
#include <utility>

namespace Fsl
{
  namespace MeshUtil
  {
    namespace
    {
      bool TryAddEdge(std::map<uint16_t, std::vector<uint16_t>>& rEdgeDict, uint16_t i0, uint16_t i1)
      {
        if (i1 < i0)
        {
          std::swap(i0, i1);
        }
        auto itrFind = rEdgeDict.find(i0);
        if (itrFind == rEdgeDict.end())
        {
          // first entry
          std::vector<uint16_t> entries = {i1};
          rEdgeDict.emplace(i0, std::move(entries));
          return true;
        }


        auto itrFind2 = std::find(itrFind->second.begin(), itrFind->second.end(), i1);
        if (itrFind2 == itrFind->second.end())
        {
          itrFind->second.push_back(i1);
          return true;
        }

        return false;
      }
    }

    SingleMesh ExtractToSingleMesh(const TestScene& scene)
    {
      std::size_t vertexCount = 0;
      std::size_t indexCount = 0;
      for (const auto& mesh : scene.Meshes)
      {
        vertexCount += mesh->GetVertexCount();
        indexCount += mesh->GetIndexCount();
      }
      FSLLOG3_INFO("Total vertex count: {}, Total index count: {}, SubMesh count: {}", vertexCount, indexCount, scene.GetMeshCount());

      // copy the vertices and indices to one large buffer
      SingleMesh finalMesh(vertexCount, indexCount);
      {
        uint32_t vertexOffset = 0u;
        uint32_t indexOffset = 0u;
        const auto meshCount = scene.GetMeshCount();
        for (int32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
          const auto& mesh = scene.Meshes[meshIndex];
          const auto meshVertexCount = mesh->GetVertexCount();
          const auto meshIndexCount = mesh->GetIndexCount();
          const auto& srcVertices = mesh->GetVertexArray();
          auto startVertexOffset = vertexOffset;
          for (std::size_t i = 0; i < meshVertexCount; ++i)
          {
            finalMesh.Vertices[vertexOffset] = srcVertices[i];
            ++vertexOffset;
          }
          const auto& srcIndices = mesh->GetIndexArray();
          for (std::size_t i = 0; i < meshIndexCount; ++i)
          {
            finalMesh.Indices[indexOffset] = startVertexOffset + srcIndices[i];
            ++indexOffset;
          }
        }
      }
      return finalMesh;
    }

    SingleMesh ExtractMeshEdges(const TestScene& scene)
    {
      auto mesh = ExtractToSingleMesh(scene);

      FSLLOG3_INFO("Finding unique edges");
      std::map<uint16_t, std::vector<uint16_t>> edgeDict;
      uint32_t duplicatedEdgeCount = 0u;
      for (std::size_t i = 0; i < mesh.Indices.size(); i += 3)
      {
        auto i0 = mesh.Indices[i];
        auto i1 = mesh.Indices[i + 1];
        auto i2 = mesh.Indices[i + 2];

        duplicatedEdgeCount += TryAddEdge(edgeDict, i0, i1) ? 1 : 0;
        duplicatedEdgeCount += TryAddEdge(edgeDict, i0, i2) ? 1 : 0;
        duplicatedEdgeCount += TryAddEdge(edgeDict, i1, i2) ? 1 : 0;
      }

      std::size_t lineCount = 0u;
      for (const auto& entry : edgeDict)
      {
        lineCount += entry.second.size();
      }
      FSLLOG3_INFO("Lines: {}, duplicated lines skipped: {}", lineCount, duplicatedEdgeCount);

      mesh.Indices.clear();
      mesh.Indices.resize(lineCount * 2);
      {
        std::size_t indexOffset = 0u;
        for (const auto& entry : edgeDict)
        {
          for (const auto dstIndex : entry.second)
          {
            mesh.Indices[indexOffset] = entry.first;
            mesh.Indices[indexOffset + 1] = dstIndex;
            indexOffset += 2;
          }
        }
      }
      return mesh;
    }
  }
}
