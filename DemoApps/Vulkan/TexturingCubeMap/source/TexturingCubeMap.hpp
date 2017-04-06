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

#include <VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>
#include <VulkanWillemsDemoAppExperimental/MeshLoader/MeshBuffer.hpp>
#include <VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <FslGraphicsVulkan1_0/DescriptorPool.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSetLayout.hpp>
#include <FslGraphicsVulkan1_0/GraphicsPipeline.hpp>
#include <FslGraphicsVulkan1_0/PipelineCache.hpp>
#include <FslGraphicsVulkan1_0/PipelineLayout.hpp>
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
      float LodBias;

      UboVS()
        : LodBias(0.0f)
      {
      }
    };

    struct Pipelines
    {
      Vulkan::GraphicsPipeline Skybox;
      Vulkan::GraphicsPipeline Reflect;
    };

    struct DescriptorSet
    {
      // // We use the native type here since this is managed by a pool
      VkDescriptorSet Object;
      // // We use the native type here since this is managed by a pool
      VkDescriptorSet Skybox;

      DescriptorSet()
        : Object(VK_NULL_HANDLE)
        , Skybox(VK_NULL_HANDLE)
      {
      }
    };

    bool m_displaySkybox;
    Meshes m_meshes;
    Vertices m_vertices;
    UniformData m_uniformData;
    UboVS m_uboVS;

    Willems::VulkanTexture m_cubeMap;

    Vulkan::DescriptorSetLayout m_descriptorSetLayout;
    Vulkan::PipelineLayout m_pipelineLayout;

    Pipelines m_pipelines;
    Vulkan::DescriptorPool m_descriptorPool;
    DescriptorSet m_descriptorSets;

  public:
    TexturingCubeMap(const DemoAppConfig& config);
    ~TexturingCubeMap();
  protected:
    virtual void Prepare() override;
    virtual void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    virtual void BuildCommandBuffers() override;
    virtual void OnViewChanged() override;
    virtual void OnKeyEvent(const KeyEvent& event) override;
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
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
