#ifndef VULKAN_DISPLACEMENTMAPPING_DISPLACEMENTMAPPING_HPP
#define VULKAN_DISPLACEMENTMAPPING_DISPLACEMENTMAPPING_HPP
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

#include <Shared/VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshBuffer.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <vector>

namespace Fsl
{
  class DisplacementMapping : public Willems::VulkanWillemsMeshDemoApp
  {
    struct Meshes
    {
      Willems::MeshLoader::MeshBuffer Object;
    };

    struct Textures
    {
      Willems::VulkanTexture ColorHeightMap;
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
      float TessLevel = 64.0f;
    };

    struct UboTE
    {
      glm::mat4 Projection;
      glm::mat4 Model;
      glm::vec4 LightPos = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
      float TessAlpha = 1.0f;
      float TessStrength = 0.1f;
    };

    struct Pipelines
    {
      RapidVulkan::GraphicsPipeline Solid;
      RapidVulkan::GraphicsPipeline Wireframe;
    };

    Meshes m_meshes;
    Textures m_textures;
    Vertices m_vertices;
    Willems::VulkanUniformData m_uniformDataTC;
    Willems::VulkanUniformData m_uniformDataTE;
    UboTC m_uboTC;
    UboTE m_uboTE;
    RapidVulkan::DescriptorSetLayout m_descriptorSetLayout;
    RapidVulkan::PipelineLayout m_pipelineLayout;
    Pipelines m_pipelines;
    RapidVulkan::DescriptorPool m_descriptorPool;
    // We use the native type here since this is managed by a pool
    VkDescriptorSet m_descriptorSet;

    bool m_displacement;
    bool m_splitScreen;

  public:
    explicit DisplacementMapping(const DemoAppConfig& config);
    ~DisplacementMapping() override;

  protected:
    void Prepare() override;
    void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    void OnViewChanged() override;
    void BuildCommandBuffers() override;
    void OnKeyEvent(const KeyEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void LoadMeshes();
    void LoadTextures();
    void SetupVertexDescriptions();
    void PrepareUniformBuffers();
    void UpdateUniformBuffers();
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSet();

    void ReBuildCommandBuffers();

    void ChangeTessellationLevel(const float delta);
    void ChangeTessellationStrength(const float delta);
    void ToggleSplitScreen();
    void ToggleDisplacement();
  };
}

#endif
