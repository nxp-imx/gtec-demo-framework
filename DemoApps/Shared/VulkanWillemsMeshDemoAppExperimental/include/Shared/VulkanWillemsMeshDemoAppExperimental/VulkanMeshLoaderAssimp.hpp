#ifndef SHARED_VULKANWILLEMSMESHDEMOAPPEXPERIMENTAL_VULKANMESHLOADERASSIMP_HPP
#define SHARED_VULKANWILLEMSMESHDEMOAPPEXPERIMENTAL_VULKANMESHLOADERASSIMP_HPP
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

#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/VulkanMeshLoader.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Fsl
{
  class IContentManager;

  namespace Willems
  {
    class VulkanMeshLoaderAssimp : public VulkanMeshLoader
    {
      Assimp::Importer Importer;

    public:
      static const int DefaultFlags =
        aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;

      VulkanMeshLoaderAssimp(const std::shared_ptr<IContentManager>& contentManager);
      ~VulkanMeshLoaderAssimp();

    protected:
      //! See VulkanMeshLoader
      void LoadMeshNow(const std::string& relativePath, std::vector<MeshEntry>& rEntries, Dimension& rDim) override;
      void LoadMeshNow(const std::string& relativePath, const int flags, std::vector<MeshEntry>& rEntries, Dimension& rDim) override;

    private:
      //! @brief Read mesh data from ASSIMP mesh to an internal mesh representation that can be used to generate Vulkan buffers
      //! @param meshEntry Pointer to the target MeshEntry structure for the mesh data
      //! @param paiMesh ASSIMP mesh to get the data from
      //! @param pScene Scene file of the ASSIMP mesh
      static void InitMesh(MeshEntry& rMeshEntry, const aiMesh* const pAiMesh, const aiScene* const pScene, Dimension& rDim);
    };
  }
}

#endif
