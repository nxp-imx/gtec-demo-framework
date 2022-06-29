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

#include <FslAssimp/SceneHelper.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <assimp/Importer.hpp>    //OO version Header!
#include <assimp/postprocess.h>
#include <algorithm>
#include <limits>

namespace Fsl
{
  void SceneHelper::GetBoundingBox(const aiScene* pScene, Vector3& rMin, Vector3& rMax)
  {
    rMin.X = rMin.Y = rMin.Z = std::numeric_limits<float>::max();
    rMax.X = rMax.Y = rMax.Z = std::numeric_limits<float>::lowest();
    GetBoundingBoxForNode(pScene, pScene->mRootNode, rMin, rMax);
  }


  void SceneHelper::GetBoundingBoxForNode(const aiScene* pScene, const aiNode* pNode, Vector3& rMin, Vector3& rMax)
  {
    aiMatrix4x4 prev;
    for (std::size_t meshIndex = 0; meshIndex < pNode->mNumMeshes; ++meshIndex)
    {
      const aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[meshIndex]];
      for (std::size_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
      {
        aiVector3D tmp = mesh->mVertices[vertexIndex];

        rMin.X = std::min(rMin.X, tmp.x);
        rMin.Y = std::min(rMin.Y, tmp.y);
        rMin.Z = std::min(rMin.Z, tmp.z);

        rMax.X = std::max(rMax.X, tmp.x);
        rMax.Y = std::max(rMax.Y, tmp.y);
        rMax.Z = std::max(rMax.Z, tmp.z);
      }
    }

    for (std::size_t i = 0; i < pNode->mNumChildren; ++i)
    {
      GetBoundingBoxForNode(pScene, pNode->mChildren[i], rMin, rMax);
    }
  }
}
