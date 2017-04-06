#ifndef VULKAN_MESHINSTANCING_MESHINSTANCING_HPP
#define VULKAN_MESHINSTANCING_MESHINSTANCING_HPP
/*
* Vulkan Example - Instanced mesh rendering, uses a separate vertex buffer for instanced data
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// Based on a example called 'Mesh instancing' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>
#include <VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <FslGraphicsVulkan1_0/DescriptorPool.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSetLayout.hpp>
#include <FslGraphicsVulkan1_0/GraphicsPipeline.hpp>
#include <FslGraphicsVulkan1_0/PipelineLayout.hpp>
#include <glm/vec3.hpp>

namespace Fsl
{
  class MeshInstancing : public Willems::VulkanWillemsMeshDemoApp
  {
    struct Textures
    {
      Willems::VulkanTexture ColorMap;
    };
    struct Meshes
    {
      Willems::MeshLoader::MeshBuffer Example;
    };

    // Per-instance data block
    struct InstanceData
    {
      glm::vec3 pos;
      glm::vec3 rot;
      float scale = 0.0f;
      uint32_t texIndex = 0;
    };

    // Contains the instanced data
    struct InstanceBuffer
    {
      Vulkan::Buffer Buffer;
      Vulkan::Memory Memory;
      uint32_t Size = 0;
      VkDescriptorBufferInfo Descriptor{};
    };

    struct Vertices
    {
      VkPipelineVertexInputStateCreateInfo InputState{};
      std::vector<VkVertexInputBindingDescription> BindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;
    };

    struct UboVS
    {
      glm::mat4 Projection;
      glm::mat4 View;
      float Time = 0.0f;
    };

    struct UniformData
    {
      Willems::VulkanUniformData VsScene;
    };

    struct Pipelines
    {
      Vulkan::GraphicsPipeline Solid;
    };

    Textures m_textures;
    Meshes m_meshes;
    InstanceBuffer m_instanceBuffer;
    Vertices m_vertices;
    UboVS m_uboVS;
    UniformData m_uniformData;
    Vulkan::DescriptorSetLayout m_descriptorSetLayout;
    Vulkan::PipelineLayout m_pipelineLayout;
    Pipelines m_pipelines;
    Vulkan::DescriptorPool m_descriptorPool;
    // We use the native type here since this is managed by a pool
    VkDescriptorSet m_descriptorSet;

  public:
    MeshInstancing(const DemoAppConfig& config);
    ~MeshInstancing();
  protected:
    virtual void Prepare() override;
    virtual void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    virtual void BuildCommandBuffers() override;
    virtual void OnViewChanged() override;
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void LoadTextures();
    void LoadMeshes();
    void PrepareInstanceData();
    void SetupVertexDescriptions();
    void PrepareUniformBuffers();
    void UpdateUniformBuffer(const bool viewChanged);
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSet();
  };
}

#endif
