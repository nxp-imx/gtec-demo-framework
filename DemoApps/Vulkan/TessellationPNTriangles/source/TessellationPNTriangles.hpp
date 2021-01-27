#ifndef VULKAN_TESSELLATIONPNTRIANGLES_TESSELLATIONPNTRIANGLES_HPP
#define VULKAN_TESSELLATIONPNTRIANGLES_TESSELLATIONPNTRIANGLES_HPP
/*
 * Vulkan Example - Tessellation shader PN triangles
 *
 * Based on http://alex.vlachos.com/graphics/CurvedPNTriangles.pdf
 * Shaders based on http://onrendering.blogspot.de/2011/12/tessellation-on-gpu-curved-pn-triangles.html
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called 'PN-Triangles' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <Shared/VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>

namespace Fsl
{
  class TessellationPNTriangles : public Willems::VulkanWillemsMeshDemoApp
  {
    struct Textures
    {
      Willems::VulkanTexture ColorMap;
    };

    struct Meshes
    {
      Willems::MeshLoader::MeshBuffer Object;
    };


    struct Vertices
    {
      VkPipelineVertexInputStateCreateInfo InputState{};
      std::vector<VkVertexInputBindingDescription> BindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;

      Vertices() = default;
    };


    struct UboTC
    {
      float TessLevel = 3.0f;
    };

    struct UboTE
    {
      glm::mat4 Projection{};
      glm::mat4 Model{};
      float TessAlpha = 1.0f;
    };


    struct Pipelines
    {
      RapidVulkan::GraphicsPipeline Solid;
      RapidVulkan::GraphicsPipeline Wire;
      RapidVulkan::GraphicsPipeline SolidPassThrough;
      RapidVulkan::GraphicsPipeline WirePassThrough;
    };

    Textures m_textures;
    Meshes m_meshes;
    Vertices m_vertices;

    Willems::VulkanUniformData m_uniformDataTE;
    Willems::VulkanUniformData m_uniformDataTC;

    UboTC m_uboTC;
    UboTE m_uboTE;

    RapidVulkan::DescriptorSetLayout m_descriptorSetLayout;
    RapidVulkan::PipelineLayout m_pipelineLayout;

    Pipelines m_pipelines;

    RapidVulkan::DescriptorPool m_descriptorPool;

    // We use the native type here since this is managed by a pool
    VkDescriptorSet m_descriptorSet;

    RapidVulkan::GraphicsPipeline* m_pPipelineLeft;
    RapidVulkan::GraphicsPipeline* m_pPipelineRight;
    bool m_splitScreen;

  public:
    explicit TessellationPNTriangles(const DemoAppConfig& config);
    ~TessellationPNTriangles() override;

  protected:
    void Prepare() override;
    void BuildCommandBuffers() override;
    void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    void OnViewChanged() override;
    void OnKeyEvent(const KeyEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void LoadTextures();
    void LoadMeshes();
    void SetupVertexDescriptions();
    void PrepareUniformBuffers();
    void UpdateUniformBuffers();
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSet();

    void ReBuildCommandBuffers();
    void ChangeTessellationLevel(const float delta);
    void TogglePipelines();
    void ToggleSplitScreen();
  };
}

#endif
