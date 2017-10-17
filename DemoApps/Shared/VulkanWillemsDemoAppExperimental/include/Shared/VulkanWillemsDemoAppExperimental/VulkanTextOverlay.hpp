#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANTEXTOVERLAY_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANTEXTOVERLAY_HPP
/*
* Text overlay class for displaying debug information
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <vulkan/vulkan.h>
#include <FslBase/Math/Extent2D.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanDevice.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanBuffer.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/CommandBuffers.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/Fence.hpp>
#include <RapidVulkan/Framebuffer.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/Image.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/PipelineCache.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/Sampler.hpp>
#include <array>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

namespace Fsl
{
  namespace Willems
  {
    class VulkanTextOverlay
    {
      // WARNING: nasty pointer to object we dont own
      VulkanDevice* m_pVulkanDevice;
      VkQueue m_queue;
      VkFormat m_colorFormat;
      VkFormat m_depthFormat;
      Extent2D m_framebufferExtent;
      // WARNING: nasty pointer to object we dont own
      const std::vector<RapidVulkan::Framebuffer>* m_pFramebuffers;
      std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

      RapidVulkan::CommandPool m_commandPool;
      VulkanBuffer m_vertexBuffer;
      RapidVulkan::Image m_image;
      RapidVulkan::Memory m_imageMemory;
      RapidVulkan::ImageView m_imageView;
      RapidVulkan::Sampler m_sampler;
      RapidVulkan::DescriptorPool m_descriptorPool;
      RapidVulkan::DescriptorSetLayout m_descriptorSetLayout;
      RapidVulkan::PipelineLayout m_pipelineLayout;
      // We use the native type here since this is managed by a pool
      VkDescriptorSet m_descriptorSet;
      RapidVulkan::PipelineCache m_pipelineCache;
      RapidVulkan::Fence m_fence;
      RapidVulkan::RenderPass m_renderPass;
      RapidVulkan::GraphicsPipeline m_pipeline;

      // Used during text updates
      glm::vec4* m_pMappedLocal;
      uint32_t m_numLetters;

    public:
      enum class TextAlign
      {
        Left, Center, Right
      };

      bool Visible;
      bool Invalidated;

      RapidVulkan::CommandBuffers CmdBuffers;
      //! @brief Default constructor
      //! @param vulkanDevice Pointer to a valid VulkanDevice
      VulkanTextOverlay(Willems::VulkanDevice* pVulkanDevice, const VkQueue queue, const std::vector<RapidVulkan::Framebuffer>* pFramebuffers,
        const VkFormat colorformat, const VkFormat depthformat, const Extent2D& framebufferExtent,
        const std::vector<VkPipelineShaderStageCreateInfo>& shaderstages);
      ~VulkanTextOverlay();

      //! @brief Prepare all vulkan resources required to render the font
      //!        The text overlay uses separate resources for descriptors (pool, sets, layouts), pipelines and command buffers
      void PrepareResources();

      //! @brief Prepare a separate render pass for rendering the text as an overlay
      void PrepareRenderPass();

      //! @brief Prepare a separate pipeline for the font rendering decoupled from the main application
      void PreparePipeline();

      //! @brief Maps the buffer, resets letter count
      void BeginTextUpdate();

      //! @brief Add text to the current buffer
      //! @param text Text to add
      //! @param x x position of the text to add in window coordinate space
      //! @param y y position of the text to add in window coordinate space
      //! @param align Alignment for the new text (left, right, center)
      void AddText(const std::string& text, const float x, const float y, const TextAlign align);

      //! @brief Unmap buffer and update command buffers
      void EndTextUpdate();

      //! @brief Update the command buffers to reflect text changes
      void UpdateCommandBuffers();

      //! @brief Submit the text command buffers to a queue
      void Submit(const VkQueue queue, const uint32_t bufferindex, VkSubmitInfo submitInfo);

      //! @brief Reallocate command buffers for the text overlay
      //! @note Frees the existing command buffers
      void ReallocateCommandBuffers();
    };
  }
}

#endif
