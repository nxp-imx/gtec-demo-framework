#ifndef VULKAN_TEXTURINGARRAYS_TEXTURINGARRAYS_HPP
#define VULKAN_TEXTURINGARRAYS_TEXTURINGARRAYS_HPP
/*
* Vulkan Example - Texture arrays and instanced rendering
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// Based on a example called 'Texture Arrays' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>

namespace Fsl
{
  // Vertex layout for this example
  struct Vertex
  {
    float pos[3];
    float uv[2];
  };

  class TexturingArrays : public Willems::VulkanWillemsDemoApp
  {
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

    struct Meshes
    {
      Willems::MeshLoader::MeshBuffer Quad;
    };

    struct ProjectionViewMatrices
    {
      glm::mat4 Projection;
      glm::mat4 View;
    };

    struct UboInstanceData
    {
      // Model matrix
      glm::mat4 Model;
      // Texture array index
      // Vec4 due to padding
      glm::vec4 ArrayIndex;
    };

    struct UboVS
    {
      // Global matrices
      ProjectionViewMatrices Matrices;
      // Separate data for each instance
      std::vector<UboInstanceData> Instance;
    };

    struct UniformData
    {
      Willems::VulkanUniformData VertexShader;
    };

    struct Pipelines
    {
      RapidVulkan::GraphicsPipeline Solid;
    };


    Vertices m_vertices;
    Willems::VulkanTexture m_textureArray;
    Meshes m_meshes;
    UboVS m_uboVS;
    UniformData m_uniformData;
    RapidVulkan::DescriptorSetLayout m_descriptorSetLayout;
    RapidVulkan::PipelineLayout m_pipelineLayout;
    Pipelines m_pipelines;
    RapidVulkan::DescriptorPool m_descriptorPool;
    // We use the native type here since this is managed by a pool
    VkDescriptorSet m_descriptorSet;


  public:
    TexturingArrays(const DemoAppConfig& config);
    ~TexturingArrays();
  protected:
    virtual void Prepare() override;
    virtual void BuildCommandBuffers() override;
    virtual void OnViewChanged();
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void SetupVertexDescriptions();
    void LoadTextures();
    Willems::VulkanTexture LoadTextureArray(const std::string& filename, const VkFormat format);
    void GenerateQuad();
    void PrepareUniformBuffers();
    void UpdateUniformBufferMatrices();
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSet();
  };
}

#endif
