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

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/Device.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSet.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/Framebuffer.hpp>
#include <RapidVulkan/Instance.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/Sampler.hpp>
#include <RapidVulkan/Semaphore.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <deque>
#include <vector>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Fsl
{
  struct BufferData
  {
    RapidVulkan::Buffer Buffer;
    RapidVulkan::Memory Memory;
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
      , usageFlags(0)
      , memoryPropertyFlags(0)
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
      RapidVulkan::Sampler Sampler;
      RapidVulkan::Image Image;
      VkImageLayout ImageLayout;
      RapidVulkan::Memory DeviceMemory;
      RapidVulkan::ImageView View;
      VkDescriptorImageInfo Descriptor;
      uint32_t Width;
      uint32_t Height;
      uint32_t MipLevels;

      TextureData()
        : ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
        , Descriptor{}
        , Width(0)
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
    RapidVulkan::DescriptorSetLayout m_descriptorSetLayout;
    RapidVulkan::PipelineLayout m_pipelineLayout;
    // PreparePipelines
    RapidVulkan::GraphicsPipeline m_graphicsPipeline;
    // SetupDescriptorPool
    RapidVulkan::DescriptorPool m_descriptorPool;
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
