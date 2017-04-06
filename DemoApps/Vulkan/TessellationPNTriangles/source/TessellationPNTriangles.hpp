#ifndef VULKAN_TESSELLATIONPNTRIANGLES_TESSELLATIONPNTRIANGLES_HPP
#define VULKAN_TESSELLATIONPNTRIANGLES_TESSELLATIONPNTRIANGLES_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/

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
