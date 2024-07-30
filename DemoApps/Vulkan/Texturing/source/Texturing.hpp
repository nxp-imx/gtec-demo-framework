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

#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSet.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/Device.hpp>
#include <RapidVulkan/Framebuffer.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/Instance.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/Sampler.hpp>
#include <RapidVulkan/Semaphore.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <deque>
#include <vector>

namespace Fsl
{
  struct BufferData
  {
    RapidVulkan::Buffer Buffer;
    RapidVulkan::Memory Memory;
    VkDescriptorBufferInfo Descriptor{};
    VkDeviceSize Size{0};
    VkDeviceSize Alignment{0};
    // NOLINTNEXTLINE(readability-identifier-naming)
    void* pMapped{nullptr};

    //! @brief Usage flags to be filled by external source at buffer creation (to query at some later point)
    VkBufferUsageFlags UsageFlags{0};
    //! @brief Memory properties flags to be filled by external source at buffer creation (to query at some later point)
    VkMemoryPropertyFlags MemoryPropertyFlags{0};

    BufferData() = default;
  };


  class Texturing : public Willems::VulkanWillemsDemoApp
  {
    struct Vertices
    {
      VkPipelineVertexInputStateCreateInfo InputState{};
      std::vector<VkVertexInputBindingDescription> BindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;

      Vertices() = default;
    };

    struct UboVS
    {
      glm::mat4 Projection{};
      glm::mat4 Model{};
      glm::vec4 ViewPos{};
      float LodBias{0.0f};

      UboVS() = default;
    };

    UboVS m_uboVS;

    struct TextureData
    {
      RapidVulkan::Sampler Sampler;
      RapidVulkan::Image Image;
      VkImageLayout ImageLayout{VK_IMAGE_LAYOUT_UNDEFINED};
      RapidVulkan::Memory DeviceMemory;
      RapidVulkan::ImageView View;
      VkDescriptorImageInfo Descriptor{};
      uint32_t Width{0};
      uint32_t Height{0};
      uint32_t MipLevels{0};

      TextureData()

        = default;
    };

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
    explicit Texturing(const DemoAppConfig& config);
    ~Texturing() override;

  protected:
    void Prepare() override;
    void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    void BuildCommandBuffers() override;
    void OnViewChanged() override;
    void OnKeyEvent(const KeyEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void GenerateQuad();
    void SetupVertexDescriptions();
    void PrepareUniformBuffers();
    void UpdateUniformBuffers();
    void LoadTexture(const IO::Path& fileName, const bool forceLinearTiling);
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSet();

    void ChangeLodBias(const float delta);
  };
}

#endif
