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

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanBuffer.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/ComputePipeline.hpp>
#include <RapidVulkan/Device.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSet.hpp>
#include <RapidVulkan/Fence.hpp>
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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "OptionParserEx.hpp"

namespace Fsl
{
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
      RapidVulkan::DescriptorSetLayout DescriptorSetLayout;  // Particle system rendering shader binding layout
      VkDescriptorSet DescriptorSet;                         // Particle system rendering shader bindings
      RapidVulkan::PipelineLayout PipelineLayout;            // Layout of the graphics pipeline
      RapidVulkan::GraphicsPipeline Pipeline;                // Particle rendering pipeline
    };

    // Resources for the compute part of the example
    struct Compute
    {
      struct ComputeUBO                           //  Compute shader uniform block object
      {
        float DeltaT;                             //    Frame delta time
        float DestX;                              //    x position of the attractor
        float DestY;                              //    y position of the attractor
        int32_t ParticleCount;

        ComputeUBO(const int32_t particleCount)
          : DeltaT(0)
          , DestX(0)
          , DestY(0)
          , ParticleCount(particleCount)
        {
        }
      };

      Willems::VulkanBuffer StorageBuffer;                  // (Shader) storage buffer object containing the particles
      Willems::VulkanBuffer UniformBuffer;                  // Uniform buffer object containing particle system parameters
      VkQueue Queue;                                        // Separate queue for compute commands (queue family may differ from the one used for graphics)
      RapidVulkan::CommandPool CommandPool;                 // Use a separate command pool (queue family may differ from the one used for graphics)
      RapidVulkan::CommandBuffer CommandBuffer;             // Command buffer storing the dispatch commands and barriers
      RapidVulkan::Fence Fence;                             // Synchronization fence to avoid rewriting compute CB if still in use
      RapidVulkan::DescriptorSetLayout DescriptorSetLayout; // Compute shader binding layout
      VkDescriptorSet DescriptorSet;                        // Compute shader bindings
      RapidVulkan::PipelineLayout PipelineLayout;           // Layout of the compute pipeline
      RapidVulkan::ComputePipeline Pipeline;                // Compute pipeline for updating particle positions
      ComputeUBO Ubo;

      Compute(const int32_t particleCount)
        : Queue(VK_NULL_HANDLE)
        , DescriptorSet(VK_NULL_HANDLE)
        , Ubo(particleCount)
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

    std::shared_ptr<OptionParserEx> m_optionParser;

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
    RapidVulkan::DescriptorPool m_descriptorPool;

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
