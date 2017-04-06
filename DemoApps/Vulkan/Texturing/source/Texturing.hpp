#ifndef VULKAN_TEXTURING_TEXTURING_HPP
#define VULKAN_TEXTURING_TEXTURING_HPP
/*
 * Vulkan Example - Texture loading (and display) example (including mip maps)
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called 'Texture' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <FslGraphicsVulkan1_0/Buffer.hpp>
#include <FslGraphicsVulkan1_0/CommandBuffer.hpp>
#include <FslGraphicsVulkan1_0/CommandPool.hpp>
#include <FslGraphicsVulkan1_0/Device.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSetLayout.hpp>
#include <FslGraphicsVulkan1_0/DescriptorPool.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSet.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <FslGraphicsVulkan1_0/Framebuffer.hpp>
#include <FslGraphicsVulkan1_0/Instance.hpp>
#include <FslGraphicsVulkan1_0/ImageView.hpp>
#include <FslGraphicsVulkan1_0/GraphicsPipeline.hpp>
#include <FslGraphicsVulkan1_0/PipelineLayout.hpp>
#include <FslGraphicsVulkan1_0/RenderPass.hpp>
#include <FslGraphicsVulkan1_0/Sampler.hpp>
#include <FslGraphicsVulkan1_0/Semaphore.hpp>
#include <FslGraphicsVulkan1_0/ShaderModule.hpp>
#include <FslGraphicsVulkan1_0/Extend/SwapchainKHREx.hpp>
#include <VulkanExperimental/DeviceQueue.hpp>
#include <deque>
#include <vector>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Fsl
{
  struct BufferData
  {
    Vulkan::Buffer Buffer;
    Vulkan::Memory Memory;
    VkDescriptorBufferInfo Descriptor;
    VkDeviceSize Size;
    VkDeviceSize Alignment;
    void* pMapped;

    //! @brief Usage flags to be filled by external source at buffer creation (to query at some later point)
    VkBufferUsageFlags usageFlags;
    //! @brief Memory properties flags to be filled by external source at buffer creation (to query at some later point)
    VkMemoryPropertyFlags memoryPropertyFlags;

    BufferData()
      : Buffer()
      , Memory()
      , Descriptor{}
      , Size(0)
      , Alignment(0)
      , pMapped(nullptr)
    {
    }
  };


  class Texturing : public Willems::VulkanWillemsDemoApp
  {
    struct Vertices
    {
      VkPipelineVertexInputStateCreateInfo InputState;
      std::vector<VkVertexInputBindingDescription> BindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;

      Vertices()
        : InputState{}
        , BindingDescriptions()
        , AttributeDescriptions()
      {
      }
    };

    struct UboVS
    {
      glm::mat4 Projection;
      glm::mat4 Model;
      glm::vec4 ViewPos;
      float LodBias;

      UboVS()
        : LodBias(0.0f)
      {
      }
    };

    UboVS m_uboVS;

    struct TextureData
    {
      Vulkan::Sampler Sampler;
      Vulkan::Image Image;
      VkImageLayout ImageLayout;
      Vulkan::Memory DeviceMemory;
      Vulkan::ImageView View;
      VkDescriptorImageInfo Descriptor;
      uint32_t Width;
      uint32_t Height;
      uint32_t MipLevels;

      TextureData()
        : Width(0)
        , Height(0)
        , MipLevels(0)
      {
      }
    };


    glm::vec3 m_cameraPos;

    // Generate quad
    uint32_t m_indexCount;
    BufferData m_vertexBuffer;
    BufferData m_indexBuffer;
    BufferData m_uniformBufferVS;
    Vertices m_vertices;
    TextureData m_texture;

    // SetupDescriptorSetLayout
    Vulkan::DescriptorSetLayout m_descriptorSetLayout;
    Vulkan::PipelineLayout m_pipelineLayout;
    // PreparePipelines
    Vulkan::GraphicsPipeline m_graphicsPipeline;
    // SetupDescriptorPool
    Vulkan::DescriptorPool m_descriptorPool;
    // SetupDescriptorSet
    // We use the native type here since this is managed by a pool
    VkDescriptorSet m_descriptorSet;
  public:
    Texturing(const DemoAppConfig& config);
    ~Texturing();
  protected:
    virtual void Prepare() override;
    virtual void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    virtual void BuildCommandBuffers() override;
    virtual void OnViewChanged() override;
    virtual void OnKeyEvent(const KeyEvent& event) override;
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:

    void GenerateQuad();
    void SetupVertexDescriptions();
    void PrepareUniformBuffers();
    void UpdateUniformBuffers();
    void LoadTexture(const std::string& fileName, const bool forceLinearTiling);
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSet();

    void ChangeLodBias(const float delta);
  };
}

#endif
