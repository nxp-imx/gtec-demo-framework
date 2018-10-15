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

#include <FslAssimp/SceneImporter.hpp>
#include <FslAssimp/SceneHelper.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <cassert>

namespace Fsl
{
  using namespace Graphics3D;

  namespace
  {
    Matrix Convert(const aiMatrix4x4& matrix)
    {
      return Matrix(matrix.a1, matrix.a2, matrix.a3, matrix.a4, matrix.b1, matrix.b2, matrix.b3, matrix.b4, matrix.c1, matrix.c2, matrix.c3,
                    matrix.c4, matrix.d1, matrix.d2, matrix.d3, matrix.d4);
    }


    void ProcessSceneNodes(const std::shared_ptr<Scene>& dstScene, const std::shared_ptr<SceneNode>& dstNode, const aiNode* const pNode)
    {
      assert(pNode != nullptr);

      UTF8String name(pNode->mName.C_Str());
      dstNode->SetName(name);
      dstNode->SetTransformation(Convert(pNode->mTransformation));

      // Process the meshes
      for (std::size_t i = 0; i < pNode->mNumMeshes; ++i)
      {
        dstNode->AddMesh(pNode->mMeshes[i]);
      }

      // Process the children
      for (std::size_t i = 0; i < pNode->mNumChildren; ++i)
      {
        std::shared_ptr<SceneNode> dstchildNode(new SceneNode());
        ProcessSceneNodes(dstScene, dstchildNode, pNode->mChildren[i]);
        dstNode->AddChild(dstchildNode);
      }
    }


    void ProcessSceneNodes(const std::shared_ptr<Scene>& dstScene, const aiScene* const pScene)
    {
      assert(pScene != nullptr);
      assert(dstScene);

      if (pScene->mRootNode == nullptr)
      {
        return;
      }

      std::shared_ptr<SceneNode> rootNode(new SceneNode());
      ProcessSceneNodes(dstScene, rootNode, pScene->mRootNode);
      dstScene->SetRootNode(rootNode);
    }

    std::shared_ptr<Scene> ProcessScene(const SceneAllocatorFunc& sceneAllocator, MeshImporter& meshImporter, const aiScene* const pScene,
                                        const float desiredSize, const bool centerModel)
    {
      assert(pScene != nullptr);

      Vector3 boundingMin, boundingMax;
      SceneHelper::GetBoundingBox(pScene, boundingMin, boundingMax);
      const auto delta = boundingMax - boundingMin;
      float currentMax = std::max(delta.X, std::max(delta.Y, delta.Z));
      float scaleMod = currentMax != 0.0f ? desiredSize / currentMax : 1.0f;
      Vector3 scale(scaleMod, scaleMod, scaleMod);
      Vector3 sceneMod = (boundingMin + (delta * 0.5f)) * -1.0f;


      std::shared_ptr<Scene> scene(sceneAllocator(pScene->mNumMeshes));

      auto sceneMeshAllocator = scene->GetMeshAllocator();
      if (!sceneMeshAllocator)
      {
        throw NotSupportedException("The scene did not contain a mesh allocator");
      }

      // Run though all the meshes and convert them
      for (std::size_t i = 0; i < pScene->mNumMeshes; ++i)
      {
        scene->AddMesh(Fsl::MeshImporter::ExtractMesh(sceneMeshAllocator, pScene->mMeshes[i], sceneMod, scale));
      }

      ProcessSceneNodes(scene, pScene);
      // FIX: various things missing

      scene->SetScaleFactor(scaleMod);
      return scene;
    }


    std::shared_ptr<Scene> ProcessScene(const SceneAllocatorFunc& sceneAllocator, MeshImporter& meshImporter, const aiScene* const pScene)
    {
      assert(pScene != nullptr);

      std::shared_ptr<Scene> scene(sceneAllocator(pScene->mNumMeshes));

      auto sceneMeshAllocator = scene->GetMeshAllocator();
      if (!sceneMeshAllocator)
      {
        throw NotSupportedException("The scene did not contain a mesh allocator");
      }

      // Run though all the meshes and convert them
      for (std::size_t i = 0; i < pScene->mNumMeshes; ++i)
      {
        scene->AddMesh(Fsl::MeshImporter::ExtractMesh(sceneMeshAllocator, pScene->mMeshes[i]));
      }

      ProcessSceneNodes(scene, pScene);
      // FIX: various things missing
      return scene;
    }
  }


  SceneImporter::SceneImporter()
    : m_sceneAllocator(SceneAllocator::AllocateComplex)
  {
  }


  SceneImporter::SceneImporter(const Graphics3D::SceneAllocatorFunc& sceneAllocator)
    : m_sceneAllocator(sceneAllocator)
  {
  }


  std::shared_ptr<Graphics3D::Scene> SceneImporter::Load(const IO::Path& filename, unsigned int pFlags)
  {
    return Load(m_sceneAllocator, filename, pFlags);
  }


  std::shared_ptr<Graphics3D::Scene> SceneImporter::Load(const IO::Path& filename, const float desiredSize, const bool centerModel,
                                                         unsigned int pFlags)
  {
    return Load(m_sceneAllocator, filename, desiredSize, centerModel, pFlags);
  }


  std::shared_ptr<Graphics3D::Scene> SceneImporter::Load(const SceneAllocatorFunc& sceneAllocator, const IO::Path& filename, unsigned int pFlags)
  {
    if (!IO::File::Exists(filename))
    {
      throw std::invalid_argument(std::string("Could not locate file: ") + filename.ToAsciiString());
    }

    const std::string strFile = filename.ToAsciiString();

    // beware that ReadFile deletes any previously loaded aiScene object. So we process it completely before calling ReadFile again.
    const aiScene* pSrcScene = m_importer.ReadFile(strFile, pFlags);
    try
    {
      if (pSrcScene == nullptr)
      {
        throw IOException(std::string("Failed to read the requested model: ") + strFile);
      }

      // Process the model and everything will be cleaned up by the importer destructor
      auto scene = ProcessScene(sceneAllocator, m_meshImporter, pSrcScene);
      m_importer.FreeScene();
      return scene;
    }
    catch (const std::exception&)
    {
      m_importer.FreeScene();
      throw;
    }
  }


  std::shared_ptr<Graphics3D::Scene> SceneImporter::Load(const SceneAllocatorFunc& sceneAllocator, const IO::Path& filename, const float desiredSize,
                                                         const bool centerModel, unsigned int pFlags)
  {
    if (!IO::File::Exists(filename))
    {
      throw std::invalid_argument(std::string("Could not locate file: ") + filename.ToAsciiString());
    }

    const std::string strFile = filename.ToAsciiString();

    // beware that ReadFile deletes any previously loaded aiScene object. So we process it completely before calling ReadFile again.
    const aiScene* pSrcScene = m_importer.ReadFile(strFile, pFlags);
    try
    {
      if (pSrcScene == nullptr)
      {
        throw IOException(std::string("Failed to read the requested model: ") + strFile);
      }

      // Process the model and everything will be cleaned up by the importer destructor
      auto scene = ProcessScene(sceneAllocator, m_meshImporter, pSrcScene, desiredSize, centerModel);
      m_importer.FreeScene();
      return scene;
    }
    catch (const std::exception&)
    {
      m_importer.FreeScene();
      throw;
    }
  }
}
