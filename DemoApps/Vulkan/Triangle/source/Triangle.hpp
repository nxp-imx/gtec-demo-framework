#ifndef VULKAN_TRIANGLE_TRIANGLE_HPP
#define VULKAN_TRIANGLE_TRIANGLE_HPP
/*
* Vulkan Example - Basic indexed triangle rendering
*
* Note:
* This is a "pedal to the metal" example to show off how to get Vulkan up an displaying something
* Contrary to the other examples, this one won't make use of helper functions or initializers
* Except in a few cases (swap chain setup e.g.)
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// Based on a example called 'Triangle' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)


#include <VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <FslGraphicsVulkan1_0/Buffer.hpp>
#include <FslGraphicsVulkan1_0/DescriptorPool.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSet.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSetLayout.hpp>
#include <FslGraphicsVulkan1_0/Fence.hpp>
#include <FslGraphicsVulkan1_0/GraphicsPipeline.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <FslGraphicsVulkan1_0/PipelineLayout.hpp>
#include <FslGraphicsVulkan1_0/Semaphore.hpp>

namespace Fsl
{
  class Triangle : public Willems::VulkanWillemsDemoApp
  {
    // Vertex buffer and attributes
    struct VertexBuffer
    {
      Vulkan::Memory Memory;                              // Handle to the device memory for this buffer
      Vulkan::Buffer Buffer;                                // Handle to the Vulkan buffer object that the memory is bound to
      VkPipelineVertexInputStateCreateInfo InputState;
      VkVertexInputBindingDescription InputBinding;
      std::vector<VkVertexInputAttributeDescription> InputAttributes;

      VertexBuffer()
        : Memory()
        , Buffer()
        , InputState{}
        , InputBinding{}
        , InputAttributes()
      {
      }
    };

    // Index buffer
    struct IndexBuffer
    {
      Vulkan::Memory Memory;
      Vulkan::Buffer Buffer;
      uint32_t Count;

      IndexBuffer()
        : Memory()
        , Buffer()
        , Count(0)
      {
      }
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
      glm::mat4 ProjectionMatrix;
      glm::mat4 ModelMatrix;
      glm::mat4 ViewMatrix;
    };

    // Uniform block object
    struct UniformDataVS
    {
      Vulkan::Memory Memory;
      Vulkan::Buffer Buffer;
      VkDescriptorBufferInfo Descriptor;
    } ;


    // PrepareSynchronizationPrimitives
    Vulkan::Semaphore m_presentCompleteSemaphore;
    Vulkan::Semaphore m_renderCompleteSemaphore;
    std::vector<Vulkan::Fence> m_waitFences;
    // PrepareVertices
    VertexBuffer m_vertices;
    IndexBuffer m_indices;
    // PrepareUniformBuffers
    UboVS m_uboVS;
    UniformDataVS m_uniformDataVS;
    // SetupDescriptorSetLayout
    Vulkan::DescriptorSetLayout m_descriptorSetLayout;
    Vulkan::PipelineLayout m_pipelineLayout;
    // PreparePipelines
    Vulkan::GraphicsPipeline m_pipeline;
    // SetupDescriptorPool
    Vulkan::DescriptorPool m_descriptorPool;
    // SetupDescriptorSet
    // We use the native type here since this is managed by a pool
    VkDescriptorSet m_descriptorSet;

  public:
    Triangle(const DemoAppConfig& config);
    ~Triangle();
  protected:
    virtual void Prepare() override;
    virtual void SetupDepthStencil(const VkFormat depthFormat) override;
    virtual void SetupRenderPass(const VkFormat colorFormat, const VkFormat depthFormat) override;
    virtual void SetupFrameBuffer() override;

    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void PrepareSynchronizationPrimitives();
    void PrepareVertices(const bool useStagingBuffers);
    void PrepareUniformBuffers();
    void UpdateUniformBuffers();
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSet();
  protected:
    virtual void BuildCommandBuffers() override;
  };
}

#endif
