#ifndef VULKAN_DYNAMICTERRAINTESSELLATION_DYNAMICTERRAINTESSELLATION_HPP
#define VULKAN_DYNAMICTERRAINTESSELLATION_DYNAMICTERRAINTESSELLATION_HPP
/*
 * Vulkan Example - Dynamic terrain tessellation
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called 'DynamicTerrainTesselation' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <Shared/VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/Frustrum.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/QueryPool.hpp>

namespace Fsl
{
  class DynamicTerrainTessellation : public Willems::VulkanWillemsMeshDemoApp
  {
    struct Meshes
    {
      Willems::MeshLoader::MeshBuffer Terrain;
      Willems::MeshLoader::MeshBuffer Skysphere;
    };
    struct Textures
    {
      Willems::VulkanTexture HeightMap;
      Willems::VulkanTexture SkySphere;
      Willems::VulkanTexture TerrainArray;
    };

    // Pipeline statistics
    struct QueryResult
    {
      RapidVulkan::Buffer Buffer;
      RapidVulkan::Memory Memory;
    };

    struct Vertices
    {
      VkPipelineVertexInputStateCreateInfo InputState{};
      std::vector<VkVertexInputBindingDescription> BindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;

      Vertices() = default;
    };

    struct UniformData
    {
      Willems::VulkanUniformData TerrainTessellation;
      Willems::VulkanUniformData SkysphereVertex;
    };

    // Shared values for tessellation control and evaluation stages
    struct UboTess
    {
      glm::mat4 Projection;
      glm::mat4 Modelview;
      glm::vec4 LightPos = glm::vec4(-48.0f, -40.0f, 46.0f, 0.0f);
      glm::vec4 FrustumPlanes[6];    // NOLINT(modernize-avoid-c-arrays)
      float DisplacementFactor = 32.0f;
      float TessellationFactor = 0.75f;
      glm::vec2 ViewportDim;
      // Desired size of tessellated quad patch edge
      float TessellatedEdgeSize = 20.0f;
    };


    // Skysphere vertex shader stage
    struct UboVS
    {
      glm::mat4 mvp;
    };

    struct DescriptorSetLayouts
    {
      RapidVulkan::DescriptorSetLayout Terrain;
      RapidVulkan::DescriptorSetLayout Skysphere;
    };

    struct PipelineLayouts
    {
      RapidVulkan::PipelineLayout Terrain;
      RapidVulkan::PipelineLayout Skysphere;
    };

    struct Pipelines
    {
      RapidVulkan::GraphicsPipeline Terrain;
      RapidVulkan::GraphicsPipeline Wireframe;
      RapidVulkan::GraphicsPipeline Skysphere;
    };

    struct DescriptorSets
    {
      // We use the native type here since this is managed by a pool
      VkDescriptorSet Terrain{VK_NULL_HANDLE};
      VkDescriptorSet Skysphere{VK_NULL_HANDLE};

      DescriptorSets() = default;
    };


    Meshes m_meshes;
    Textures m_textures;
    QueryResult m_queryResult;
    RapidVulkan::QueryPool m_queryPool;
    uint64_t m_pipelineStats[2]{};    // NOLINT(modernize-avoid-c-arrays)
    Vertices m_vertices;
    UniformData m_uniformData;
    // View frustum passed to tessellation control shader for culling
    Willems::Frustum m_frustum{};
    UboTess m_uboTess;
    UboVS m_uboVS{};
    DescriptorSetLayouts m_descriptorSetLayouts;
    PipelineLayouts m_pipelineLayouts;
    Pipelines m_pipelines;
    RapidVulkan::DescriptorPool m_descriptorPool;
    DescriptorSets m_descriptorSets;

    bool m_tessellation;
    bool m_wireframe;

  public:
    explicit DynamicTerrainTessellation(const DemoAppConfig& config);
    ~DynamicTerrainTessellation() override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void Prepare() override;
    void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    void OnViewChanged() override;
    void BuildCommandBuffers() override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void LoadMeshes();
    void LoadTextures();
    void GenerateTerrain();
    void SetupQueryResultBuffer();
    void SetupVertexDescriptions();
    void PrepareUniformBuffers();
    void UpdateUniformBuffers();
    void SetupDescriptorSetLayouts();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSets();

    void ReBuildCommandBuffers();

    void ChangeTessellationFactor(const float delta);
    void ToggleWireframe();
    void ToggleTessellation();
    // Retrieves the results of the pipeline statistics query submitted to the command buffer
    void GetQueryResults();
  };
}

#endif
