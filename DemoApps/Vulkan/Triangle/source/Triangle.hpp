#ifndef VULKAN_TRIANGLE_TRIANGLE_HPP
#define VULKAN_TRIANGLE_TRIANGLE_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
// GLM Headers for standard math operations
#include <glm/glm.hpp>
#include <utility>

namespace Fsl
{
  struct BufferAndMemory
  {
    RapidVulkan::Memory Memory;    // Handle to the device memory for this buffer
    RapidVulkan::Buffer Buffer;    // Handle to the Vulkan buffer object that the memory is bound to
  };

  struct VertexBuffer
  {
    BufferAndMemory Content;
    VkVertexInputBindingDescription InputBinding{};
    std::vector<VkVertexInputAttributeDescription> InputAttributes{};
  };

  class Triangle final : public VulkanBasic::DemoAppVulkanBasic
  {
    struct CameraInfo
    {
      glm::vec3 Rotation{};
      float Zoom{0};
    };

    // Vertex buffer and attributes

    // Index buffer
    struct IndexBuffer
    {
      BufferAndMemory Content;
      uint32_t Count{0};
    };

    struct Mesh
    {
      VertexBuffer Vertices;
      IndexBuffer Indices;
    };

    // For simplicity we use the same uniform block layout as in the shader:
    //
    //  layout(set = 0, binding = 0) uniform UBO
    //  {
    //    mat4 projectionMatrix;
    //    mat4 modelMatrix;
    //    mat4 viewMatrix;
    //  } ubo;
    //
    // This way we can just memcopy the ubo data to the ubo
    // Note: You should use data types that align with the GPU in order to avoid manual padding (vec4, mat4)
    struct UboVS
    {
      glm::mat4 ProjectionMatrix{};
      glm::mat4 ModelMatrix{};
      glm::mat4 ViewMatrix{};
    };

    // Uniform block object
    struct UniformDataVS
    {
      RapidVulkan::Memory Memory;
      RapidVulkan::Buffer Buffer;
      VkDescriptorBufferInfo Descriptor{};
    };

    struct Resources
    {
      // PrepareVertices
      Mesh TriangleMesh;

      UniformDataVS UniformData;

      // SetupDescriptorSetLayout
      RapidVulkan::DescriptorSetLayout DescriptorSetLayout;
      RapidVulkan::PipelineLayout PipelineLayout;

      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;

      // SetupDescriptorPool
      RapidVulkan::DescriptorPool DescriptorPool;

      // SetupDescriptorSet
      // We use the native type here since this is managed by a pool
      VkDescriptorSet DescriptorSet{VK_NULL_HANDLE};
    };


    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::GraphicsPipeline Pipeline;
    };

    CameraInfo m_cameraInfo;
    Resources m_resources;
    DependentResources m_dependentResources;

    UboVS m_uboVS;

  public:
    explicit Triangle(const DemoAppConfig& config);

  protected:
    void Update(const DemoTime& demoTime) final;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) final;
    void DrawTriangle(VkCommandBuffer hCmdBuffer, const uint32_t cmdBufferIndex);

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) final;
    void OnFreeResources() final;

  private:
    static Mesh PrepareVertices(const Vulkan::VUDevice& device, Vulkan::VUDeviceQueueRecord& rDeviceQueue, const VkCommandPool cmdPool,
                                const bool useStagingBuffers);
    static UniformDataVS PrepareUniformBuffers(const Vulkan::VUDevice& device, const CameraInfo& cameraInfo, const float aspectRatio);
    static UboVS UpdateUniformBuffers(const Vulkan::VUDevice& device, const CameraInfo& cameraInfo, const float aspectRatio,
                                      UniformDataVS& rUniformData);
  };
}

#endif
