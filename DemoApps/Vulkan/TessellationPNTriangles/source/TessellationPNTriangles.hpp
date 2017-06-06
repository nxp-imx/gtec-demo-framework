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

#include <VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>
#include <VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <FslGraphicsVulkan1_0/DescriptorPool.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSetLayout.hpp>
#include <FslGraphicsVulkan1_0/GraphicsPipeline.hpp>
#include <FslGraphicsVulkan1_0/PipelineLayout.hpp>

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
      VkPipelineVertexInputStateCreateInfo InputState;
      std::vector<VkVertexInputBindingDescription> BindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;

      Vertices()
        : InputState{}
      {
      }
    };


    struct UboTC
    {
      float TessLevel = 3.0f;
    };

    struct UboTE
    {
      glm::mat4 Projection;
      glm::mat4 Model;
      float TessAlpha = 1.0f;
    };


    struct Pipelines
    {
      Vulkan::GraphicsPipeline Solid;
      Vulkan::GraphicsPipeline Wire;
      Vulkan::GraphicsPipeline SolidPassThrough;
      Vulkan::GraphicsPipeline WirePassThrough;
    };

    Textures m_textures;
    Meshes m_meshes;
    Vertices m_vertices;

    Willems::VulkanUniformData m_uniformDataTE;
    Willems::VulkanUniformData m_uniformDataTC;

    UboTC m_uboTC;
    UboTE m_uboTE;

    Vulkan::DescriptorSetLayout m_descriptorSetLayout;
    Vulkan::PipelineLayout m_pipelineLayout;

    Pipelines m_pipelines;

    Vulkan::DescriptorPool m_descriptorPool;

    // We use the native type here since this is managed by a pool
    VkDescriptorSet m_descriptorSet;

    Vulkan::GraphicsPipeline* m_pPipelineLeft;
    Vulkan::GraphicsPipeline* m_pPipelineRight;
    bool m_splitScreen;
  public:
    TessellationPNTriangles(const DemoAppConfig& config);
    ~TessellationPNTriangles();
  protected:
    virtual void Prepare() override;
    virtual void BuildCommandBuffers() override;
    virtual void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    virtual void OnViewChanged() override;
    virtual void OnKeyEvent(const KeyEvent& event) override;
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
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
