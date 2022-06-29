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

#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <Shared/VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>
#include <glm/vec3.hpp>
#include "OptionParserEx.hpp"

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
      RapidVulkan::Buffer Buffer;
      RapidVulkan::Memory Memory;
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
      glm::mat4 Projection{};
      glm::mat4 View{};
      float Time = 0.0f;
    };

    struct UniformData
    {
      Willems::VulkanUniformData VsScene;
    };

    struct Pipelines
    {
      RapidVulkan::GraphicsPipeline Solid;
    };

    std::shared_ptr<OptionParserEx> m_optionParser;
    Textures m_textures;
    Meshes m_meshes;
    InstanceBuffer m_instanceBuffer;
    Vertices m_vertices;
    UboVS m_uboVS;
    UniformData m_uniformData;
    RapidVulkan::DescriptorSetLayout m_descriptorSetLayout;
    RapidVulkan::PipelineLayout m_pipelineLayout;
    Pipelines m_pipelines;
    RapidVulkan::DescriptorPool m_descriptorPool;
    // We use the native type here since this is managed by a pool
    VkDescriptorSet m_descriptorSet;

  public:
    explicit MeshInstancing(const DemoAppConfig& config);
    ~MeshInstancing() override;

  protected:
    void Prepare() override;
    void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    void BuildCommandBuffers() override;
    void OnViewChanged() override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

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
