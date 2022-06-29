#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_MESHLOADER_VULKANMESHLOADER_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_MESHLOADER_VULKANMESHLOADER_HPP
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
// The Assimp dependent parts were also split into their own class to make it
// possible to builds apps without the Assimp dependency.

#include <FslBase/IO/Path.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshBuffer.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshCreateInfo.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/VertexLayout.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/VertexSize.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.h>
#include <cstdlib>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace Fsl
{
  class IContentManager;

  namespace Willems
  {
    class VulkanDevice;
    class VulkanMeshLoader
    {
    protected:
      struct Vertex
      {
        glm::vec3 m_pos{};
        glm::vec2 m_tex{};
        glm::vec3 m_normal{};
        glm::vec3 m_color{};
        glm::vec3 m_tangent{};
        glm::vec3 m_binormal{};

        Vertex() = default;

        Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent,
               const glm::vec3& color)
        {
          m_pos = pos;
          m_tex = tex;
          m_normal = normal;
          m_color = color;
          m_tangent = tangent;
          m_binormal = bitangent;
        }
      };

      struct MeshEntry
      {
        uint32_t NumIndices{0};
        uint32_t MaterialIndex{0};
        uint32_t VertexBase{0};
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
        MeshEntry() = default;
      };

      struct Dimension
      {
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::min());
        glm::vec3 size;
      };

    private:
      std::shared_ptr<IContentManager> m_contentManager;
      std::vector<MeshEntry> m_entries;
      Dimension m_dim;
      IO::Path m_contentPath;

    public:
      VulkanMeshLoader(const VulkanMeshLoader&) = delete;
      VulkanMeshLoader& operator=(const VulkanMeshLoader&) = delete;

      explicit VulkanMeshLoader(const std::shared_ptr<IContentManager>& contentManager);
      virtual ~VulkanMeshLoader() = default;

      void LoadMesh(const std::string& relativePath);
      void LoadMesh(const std::string& relativePath, const int flags);
      virtual std::size_t GetMeshCount() const;

      //! @brief Create Vulkan buffers for the index and vertex buffer using a vertex layout
      //! @note Only does staging if a valid command buffer and transfer queue are passed
      //! @param layout Vertex layout for the vertex buffer
      //! @param createInfo Structure containing information for mesh creation time (center, scaling, etc.)
      //! @param useStaging If true, buffers are staged to device local memory
      //! @param copyCmd (Required for staging) Command buffer to put the copy commands into
      //! @param copyQueue (Required for staging) Queue to put copies into
      MeshLoader::MeshBuffer CreateBuffers(VulkanDevice& rVulkanDevice, const std::vector<MeshLoader::VertexLayout>& layout,
                                           const MeshLoader::MeshCreateInfo* const pCreateInfo, const bool useStaging, const VkCommandBuffer copyCmd,
                                           const VkQueue copyQueue);


    protected:
      virtual void LoadMeshNow(const std::string& relativePath, std::vector<MeshEntry>& rEntries, Dimension& rDim) = 0;
      virtual void LoadMeshNow(const std::string& relativePath, const int flags, std::vector<MeshEntry>& rEntries, Dimension& rDim) = 0;

      IO::Path GetContentPath() const
      {
        return m_contentPath;
      }
    };
  }
}

#endif
