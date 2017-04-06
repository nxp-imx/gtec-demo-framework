#ifndef VULKAN_COMPUTEPARTICLES_COMPUTEPARTICLES_HPP
#define VULKAN_COMPUTEPARTICLES_COMPUTEPARTICLES_HPP
/*
* Vulkan Example - Attraction based compute shader particle system
*
* Updated compute shader by Lukas Bergdoll (https://github.com/Voultapher)
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// Based on a example called 'ComputeParticles' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <VulkanWillemsDemoAppExperimental/VulkanBuffer.hpp>
#include <FslGraphicsVulkan1_0/Buffer.hpp>
#include <FslGraphicsVulkan1_0/CommandBuffer.hpp>
#include <FslGraphicsVulkan1_0/CommandPool.hpp>
#include <FslGraphicsVulkan1_0/ComputePipeline.hpp>
#include <FslGraphicsVulkan1_0/Device.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSetLayout.hpp>
#include <FslGraphicsVulkan1_0/DescriptorPool.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSet.hpp>
#include <FslGraphicsVulkan1_0/Fence.hpp>
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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Fsl
{
#ifdef __ANDROID__
  const uint32_t PARTICLE_COUNT = (256 * 1024) / 10;
#else
  const uint32_t PARTICLE_COUNT = 256 * 1024;
#endif

  class ComputeParticles : public Willems::VulkanWillemsDemoApp
  {
    struct Textures
    {
      Willems::VulkanTexture Particle;
      Willems::VulkanTexture Gradient;
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

    // Resources for the graphics part of the example
    struct Graphics
    {
      Vulkan::DescriptorSetLayout DescriptorSetLayout;  // Particle system rendering shader binding layout
      VkDescriptorSet DescriptorSet;                    // Particle system rendering shader bindings
      Vulkan::PipelineLayout PipelineLayout;            // Layout of the graphics pipeline
      Vulkan::GraphicsPipeline Pipeline;                // Particle rendering pipeline
    };

    // Resources for the compute part of the example
    struct Compute
    {
      struct ComputeUBO                           //  Compute shader uniform block object
      {
        float DeltaT;                             //    Frame delta time
        float DestX;                              //    x position of the attractor
        float DestY;                              //    y position of the attractor
        int32_t ParticleCount = PARTICLE_COUNT;

        ComputeUBO()
          : DeltaT(0)
          , DestX(0)
          , DestY(0)
        {
        }
      } ubo;

      Willems::VulkanBuffer StorageBuffer;              // (Shader) storage buffer object containing the particles
      Willems::VulkanBuffer UniformBuffer;              // Uniform buffer object containing particle system parameters
      VkQueue Queue;                                    // Separate queue for compute commands (queue family may differ from the one used for graphics)
      Vulkan::CommandPool CommandPool;                  // Use a separate command pool (queue family may differ from the one used for graphics)
      Vulkan::CommandBuffer CommandBuffer;              // Command buffer storing the dispatch commands and barriers
      Vulkan::Fence Fence;                              // Synchronization fence to avoid rewriting compute CB if still in use
      Vulkan::DescriptorSetLayout DescriptorSetLayout;  // Compute shader binding layout
      VkDescriptorSet DescriptorSet;                    // Compute shader bindings
      Vulkan::PipelineLayout PipelineLayout;            // Layout of the compute pipeline
      Vulkan::ComputePipeline Pipeline;                 // Compute pipeline for updating particle positions
      ComputeUBO Ubo;

      Compute()
        : Queue(VK_NULL_HANDLE)
      {
      }
    };

    // SSBO particle declaration
    struct Particle
    {
      glm::vec2 Pos;                // Particle position
      glm::vec2 Vel;                // Particle velocity
      glm::vec4 GradientPos;        // Texture coordinates for the gradient ramp map
    };

    float m_timer = 0.0f;
    float m_animStart = 20.0f;
    bool m_animate = true;

    // LoadTextures
    Textures m_textures;
    // PrepareStorageBuffers
    Compute m_compute;
    Vertices m_vertices;
    // SetupDescriptorSetLayout
    Graphics m_graphics;
    // SetupDescriptorPool
    Vulkan::DescriptorPool m_descriptorPool;

  public:
    ComputeParticles(const DemoAppConfig& config);
    ~ComputeParticles();
  protected:
    virtual void Prepare() override;
    virtual void GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay) override;
    virtual void OnKeyEvent(const KeyEvent& event) override;
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
    virtual void BuildCommandBuffers() override;
  private:
    void LoadTextures();
    void PrepareStorageBuffers();
    void PrepareUniformBuffers();
    void UpdateUniformBuffers();
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSet();
    void PrepareCompute();
    void BuildComputeCommandBuffer();
  };
}

#endif
