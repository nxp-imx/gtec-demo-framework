#ifndef VULKAN_TEXTURINGCUBEMAP_TEXTURINGCUBEMAP_HPP
#define VULKAN_TEXTURINGCUBEMAP_TEXTURINGCUBEMAP_HPP
/*
 * Vulkan Example - Cube map texture loading and displaying
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called '(Texture mapping) Cube maps' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <Shared/VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshBuffer.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineCache.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <glm/glm.hpp>

namespace Fsl
{
  class TexturingCubeMap : public Willems::VulkanWillemsMeshDemoApp
  {
    struct Meshes
    {
      Willems::MeshLoader::MeshBuffer Skybox;
      std::vector<Willems::MeshLoader::MeshBuffer> Objects;
      uint32_t ObjectIndex = 0;
    };
    struct Vertices
    {
      VkPipelineVertexInputStateCreateInfo InputState;
      std::vector<VkVertexInputBindingDescription> BindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;

      Vertices()
        : InputState{}
      {
      }
    };

    struct UniformData
    {
      Willems::VulkanUniformData ObjectVS;
      Willems::VulkanUniformData SkyboxVS;
    };

    struct UboVS
    {
      glm::mat4 Projection;
      glm::mat4 Model;
      float LodBias{0.0f};

      UboVS() = default;
    };

    struct Pipelines
    {
      RapidVulkan::GraphicsPipeline Skybox;
      RapidVulkan::GraphicsPipeline Reflect;
    };

    struct DescriptorSet
    {
      // // We use the native type here since this is managed by a pool
      VkDescriptorSet Object{VK_NULL_HANDLE};
      // // We use the native type here since this is managed by a pool
      VkDescriptorSet Skybox{VK_NULL_HANDLE};

      DescriptorSet() = default;
    };

    bool m_displaySkybox;
    Meshes m_meshes;
    Vertices m_vertices;
    UniformData m_uniformData;
    UboVS m_uboVS;

    Willems::VulkanTexture m_cubeMap;

    RapidVulkan::DescriptorSetLayout m_descriptorSetLayout;
    RapidVulkan::PipelineLayout m_pipelineLayout;

    Pipelines m_pipelines;
    RapidVulkan::DescriptorPool m_descriptorPool;
    DescriptorSet m_descriptorSets;

  public:
    TexturingCubeMap(const DemoAppConfig& config);
    ~TexturingCubeMap() override;

  protected:
    void Prepare() override;
    void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    void BuildCommandBuffers() override;
    void OnViewChanged() override;
    void OnKeyEvent(const KeyEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void LoadMeshes();
    void SetupVertexDescriptions();
    void PrepareUniformBuffers();
    void UpdateUniformBuffers();
    Willems::VulkanTexture LoadCubemap(const std::string& filename, const VkFormat format, const bool forceLinearTiling);
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSets();

    void ReBuildCommandBuffers();
    void ToggleSkyBox();
    void ToggleObject();
    void ChangeLodBias(const float delta);
  };
}

#endif
