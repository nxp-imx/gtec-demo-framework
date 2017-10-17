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

#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/VulkanMeshLoader.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanDevice.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <RapidVulkan/Check.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>

namespace Fsl
{
  class IContentManager;

  namespace Willems
  {
    using namespace MeshLoader;

    VulkanMeshLoader::VulkanMeshLoader(const std::shared_ptr<IContentManager>& contentManager)
      : m_contentManager(contentManager)
    {
      if (!contentManager)
        throw std::invalid_argument("arguments can not be null");
      m_contentPath = contentManager->GetContentPath();
    }




    void VulkanMeshLoader::LoadMesh(const std::string& relativePath)
    {
      LoadMeshNow(relativePath, m_entries, m_dim);
    }


    void VulkanMeshLoader::LoadMesh(const std::string& relativePath, const int flags)
    {
      LoadMeshNow(relativePath, flags, m_entries, m_dim);
    }


    std::size_t VulkanMeshLoader::GetMeshCount() const
    {
      return m_entries.size();
    }


    MeshBuffer VulkanMeshLoader::CreateBuffers(VulkanDevice& rVulkanDevice,
                                               const std::vector<VertexLayout>& layout, const MeshCreateInfo*const pCreateInfo,
                                               const bool useStaging, const VkCommandBuffer copyCmd, const VkQueue copyQueue)
    {
      glm::vec3 scale;
      glm::vec2 uvscale;
      glm::vec3 center;
      if (pCreateInfo == nullptr)
      {
        scale = glm::vec3(1.0f);
        uvscale = glm::vec2(1.0f);
        center = glm::vec3(0.0f);
      }
      else
      {
        scale = pCreateInfo->Scale;
        uvscale = pCreateInfo->UVScale;
        center = pCreateInfo->Center;
      }

      std::vector<float> vertexBuffer;
      for (std::size_t m = 0; m < m_entries.size(); ++m)
      {
        for (std::size_t i = 0; i < m_entries[m].Vertices.size(); ++i)
        {
          // Push vertex data depending on layout
          for (auto& layoutDetail : layout)
          {
            // Position
            if (layoutDetail == VertexLayout::VERTEX_LAYOUT_POSITION)
            {
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_pos.x * scale.x + center.x);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_pos.y * scale.y + center.y);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_pos.z * scale.z + center.z);
            }
            // Normal
            if (layoutDetail == VertexLayout::VERTEX_LAYOUT_NORMAL)
            {
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_normal.x);
              vertexBuffer.push_back(-m_entries[m].Vertices[i].m_normal.y);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_normal.z);
            }
            // Texture coordinates
            if (layoutDetail == VertexLayout::VERTEX_LAYOUT_UV)
            {
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_tex.s * uvscale.s);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_tex.t * uvscale.t);
            }
            // Color
            if (layoutDetail == VertexLayout::VERTEX_LAYOUT_COLOR)
            {
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_color.r);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_color.g);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_color.b);
            }
            // Tangent
            if (layoutDetail == VertexLayout::VERTEX_LAYOUT_TANGENT)
            {
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_tangent.x);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_tangent.y);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_tangent.z);
            }
            // Bitangent
            if (layoutDetail == VertexLayout::VERTEX_LAYOUT_BITANGENT)
            {
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_binormal.x);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_binormal.y);
              vertexBuffer.push_back(m_entries[m].Vertices[i].m_binormal.z);
            }
            // Dummy layout components for padding
            if (layoutDetail == VertexLayout::VERTEX_LAYOUT_DUMMY_FLOAT)
            {
              vertexBuffer.push_back(0.0f);
            }
            if (layoutDetail == VertexLayout::VERTEX_LAYOUT_DUMMY_VEC4)
            {
              vertexBuffer.push_back(0.0f);
              vertexBuffer.push_back(0.0f);
              vertexBuffer.push_back(0.0f);
              vertexBuffer.push_back(0.0f);
            }
          }
        }
      }

      std::vector<MeshDescriptor> meshDescriptors;
      std::vector<uint32_t> indexBuffer;
      { // Create mesh descriptors and index buffer
        for (std::size_t m = 0; m < m_entries.size(); ++m)
        {
          uint32_t indexBase = static_cast<uint32_t>(indexBuffer.size());
          for (uint32_t i = 0; i < m_entries[m].Indices.size(); i++)
          {
            indexBuffer.push_back(m_entries[m].Indices[i] + indexBase);
          }
          MeshDescriptor descriptor;
          descriptor.IndexBase = indexBase;
          descriptor.IndexCount = static_cast<uint32_t>(m_entries[m].Indices.size());
          descriptor.VertexCount = static_cast<uint32_t>(m_entries[m].Vertices.size());
          meshDescriptors.push_back(descriptor);
        }
      }



      MeshBufferInfo meshVertices;
      MeshBufferInfo meshIndices;
      meshVertices.Size = vertexBuffer.size() * sizeof(float);
      meshIndices.Size = indexBuffer.size() * sizeof(uint32_t);

      // Use staging buffer to move vertex and index buffer to device local memory
      if (useStaging && copyQueue != VK_NULL_HANDLE && copyCmd != VK_NULL_HANDLE)
      {
        struct Buffers
        {
          RapidVulkan::Buffer Buffer;
          RapidVulkan::Memory Memory;
        };

        // Create staging buffers
        Buffers vertexStaging;
        Buffers indexStaging;

        // Vertex buffer
        rVulkanDevice.CreateBuffer(vertexStaging.Buffer, vertexStaging.Memory,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
          meshVertices.Size, vertexBuffer.data());

        // Index buffer
        rVulkanDevice.CreateBuffer(indexStaging.Buffer, indexStaging.Memory,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
          meshIndices.Size, indexBuffer.data());

        // Create device local target buffers
        // Vertex buffer
        rVulkanDevice.CreateBuffer(meshVertices.Buffer, meshVertices.Memory,
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
          meshVertices.Size);

        // Index buffer
        rVulkanDevice.CreateBuffer(meshIndices.Buffer, meshIndices.Memory,
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
          meshIndices.Size);

        // Copy from staging buffers
        VkCommandBufferBeginInfo cmdBufferBeginInfo{};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufferBeginInfo.pNext = nullptr;

        RAPIDVULKAN_CHECK(vkBeginCommandBuffer(copyCmd, &cmdBufferBeginInfo));
        {
          VkBufferCopy copyRegion{};

          copyRegion.size = meshVertices.Size;
          vkCmdCopyBuffer(copyCmd, vertexStaging.Buffer.Get(), meshVertices.Buffer.Get(), 1, &copyRegion);

          copyRegion.size = meshIndices.Size;
          vkCmdCopyBuffer(copyCmd, indexStaging.Buffer.Get(), meshIndices.Buffer.Get(), 1, &copyRegion);
        }
        RAPIDVULKAN_CHECK(vkEndCommandBuffer(copyCmd));

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &copyCmd;

        RAPIDVULKAN_CHECK(vkQueueSubmit(copyQueue, 1, &submitInfo, VK_NULL_HANDLE));
        RAPIDVULKAN_CHECK(vkQueueWaitIdle(copyQueue));
      }
      else
      {
        // Generate vertex buffer
        rVulkanDevice.CreateBuffer(meshVertices.Buffer, meshVertices.Memory,
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
          meshVertices.Size, vertexBuffer.data());

        // Generate index buffer
        rVulkanDevice.CreateBuffer(meshIndices.Buffer, meshIndices.Memory,
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
          meshIndices.Size, indexBuffer.data());
      }


      const auto meshIndexCount = static_cast<uint32_t>(indexBuffer.size());
      // Note the original code modified the 'loaded mesh' so the next mesh would be affected
      // this was removed as it's unexpected side effect.
      auto meshDim = m_dim;
      //meshDim.min *= scale;
      //meshDim.max *= scale;
      meshDim.size *= scale;
      return MeshBuffer(std::move(meshDescriptors), std::move(meshVertices), std::move(meshIndices), meshIndexCount, meshDim.size);
    }
  }
}
