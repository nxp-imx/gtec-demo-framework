/*
* Mesh loader for creating Vulkan resources from models loaded with ASSIMP
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in VulkanMeshLoader to make it easier
// to port samples. It is not a straight port, but it has instead been converted to
// follow the RAII principle used in this framework

#include <VulkanWillemsMeshDemoAppExperimental/VulkanMeshLoaderAssimp.hpp>
#include <VulkanWillemsDemoAppExperimental/Config.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/File.hpp>
#include <FslDemoApp/Service/Content/IContentManager.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphicsVulkan1_0/Buffer.hpp>
#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/ConvertUtil.hpp>
#include <FslGraphicsVulkan1_0/Fence.hpp>
#include <FslGraphicsVulkan1_0/MemoryTypeHelper.hpp>
#include <FslGraphicsVulkan1_0/VulkanHelper.hpp>
#include <FslGraphicsVulkan1_0/Extend/Convert.hpp>
#include <algorithm>
#include <cstring>
#include <utility>

namespace Fsl
{
  using namespace Vulkan;

  namespace Willems
  {
    namespace
    {
      const IO::Path ToAbsolutePath(const IO::Path& trustedAbsPath, const IO::Path& notTrustedRelativePath)
      {
        assert(!trustedAbsPath.IsEmpty());

        // Do a lot of extra validation
        if (notTrustedRelativePath.IsEmpty())
          throw std::invalid_argument(std::string("path is invalid: ") + notTrustedRelativePath.ToAsciiString());
        if (IO::Path::IsPathRooted(notTrustedRelativePath))
          throw std::invalid_argument(std::string("not a relative path: ") + notTrustedRelativePath.ToAsciiString());
        if (notTrustedRelativePath.Contains(".."))
          throw std::invalid_argument(std::string("\"..\" not allowed in the relative path: ") + notTrustedRelativePath.ToAsciiString());

        return IO::Path::Combine(trustedAbsPath, notTrustedRelativePath);
      }
    }

    VulkanMeshLoaderAssimp::VulkanMeshLoaderAssimp(const std::shared_ptr<IContentManager>& contentManager)
      : VulkanMeshLoader(contentManager)
    {
      if (!contentManager)
        throw std::invalid_argument("contentManager can not be null");
    }


    VulkanMeshLoaderAssimp::~VulkanMeshLoaderAssimp()
    {
    }


    void VulkanMeshLoaderAssimp::LoadMeshNow(const std::string& relativePath, std::vector<MeshEntry>& rEntries, Dimension& rDim)
    {
      LoadMeshNow(relativePath, DefaultFlags, rEntries, rDim);
    }


    void VulkanMeshLoaderAssimp::LoadMeshNow(const std::string& relativePath, const int flags, std::vector<MeshEntry>& rEntries, Dimension& rDim)
    {
      const IO::Path absPath(ToAbsolutePath(GetContentPath(), relativePath));


      auto pScene = Importer.ReadFile(absPath.ToAsciiString().c_str(), flags);
      if (! pScene)
        throw NotSupportedException(std::string("Could not read file: ") + absPath.ToAsciiString());

      rEntries.clear();
      rEntries.resize(pScene->mNumMeshes);
      // Read in all meshes in the scene
      uint32_t numVertices = 0;
      for (std::size_t i = 0; i < rEntries.size(); ++i)
      {
        rEntries[i].VertexBase = numVertices;
        numVertices += pScene->mMeshes[i]->mNumVertices;
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(rEntries[i], paiMesh, pScene, rDim);
      }
    }


    void VulkanMeshLoaderAssimp::InitMesh(MeshEntry& rMeshEntry, const aiMesh*const pAiMesh, const aiScene*const pScene, Dimension& rDim)
    {
      assert(pAiMesh != nullptr);
      assert(pScene != nullptr);

      rMeshEntry.MaterialIndex = pAiMesh->mMaterialIndex;

      aiColor3D pColor(0.f, 0.f, 0.f);
      pScene->mMaterials[pAiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

      aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

      rMeshEntry.Vertices.clear();
      rMeshEntry.Vertices.resize(pAiMesh->mNumVertices);

      for (unsigned int i = 0; i < pAiMesh->mNumVertices; i++)
      {
        aiVector3D* pPos = &(pAiMesh->mVertices[i]);
        aiVector3D* pNormal = &(pAiMesh->mNormals[i]);
        aiVector3D* pTexCoord = (pAiMesh->HasTextureCoords(0)) ? &(pAiMesh->mTextureCoords[0][i]) : &Zero3D;
        aiVector3D* pTangent = (pAiMesh->HasTangentsAndBitangents()) ? &(pAiMesh->mTangents[i]) : &Zero3D;
        aiVector3D* pBiTangent = (pAiMesh->HasTangentsAndBitangents()) ? &(pAiMesh->mBitangents[i]) : &Zero3D;

        Vertex v(
          glm::vec3(pPos->x, -pPos->y, pPos->z),
          glm::vec2(pTexCoord->x, pTexCoord->y),
          glm::vec3(pNormal->x, pNormal->y, pNormal->z),
          glm::vec3(pTangent->x, pTangent->y, pTangent->z),
          glm::vec3(pBiTangent->x, pBiTangent->y, pBiTangent->z),
          glm::vec3(pColor.r, pColor.g, pColor.b)
          );

        rDim.max.x = std::max(pPos->x, rDim.max.x);
        rDim.max.y = std::max(pPos->y, rDim.max.y);
        rDim.max.z = std::max(pPos->z, rDim.max.z);

        rDim.min.x = std::min(pPos->x, rDim.min.x);
        rDim.min.y = std::min(pPos->y, rDim.min.y);
        rDim.min.z = std::min(pPos->z, rDim.min.z);

        rMeshEntry.Vertices[i] = v;
      }

      rDim.size = rDim.max - rDim.min;

      uint32_t indexBase = static_cast<uint32_t>(rMeshEntry.Indices.size());
      for (unsigned int i = 0; i < pAiMesh->mNumFaces; i++)
      {
        const aiFace& Face = pAiMesh->mFaces[i];
        if (Face.mNumIndices == 3)
        {
          rMeshEntry.Indices.push_back(indexBase + Face.mIndices[0]);
          rMeshEntry.Indices.push_back(indexBase + Face.mIndices[1]);
          rMeshEntry.Indices.push_back(indexBase + Face.mIndices[2]);
        }
      }
    }


  }
}
