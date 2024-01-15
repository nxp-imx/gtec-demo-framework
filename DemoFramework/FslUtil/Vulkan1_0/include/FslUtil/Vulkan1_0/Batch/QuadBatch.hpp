#ifndef FSLUTIL_VULKAN1_0_BATCH_QUADBATCH_HPP
#define FSLUTIL_VULKAN1_0_BATCH_QUADBATCH_HPP
/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslGraphics/Render/BatchSdfRenderConfig.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Render/Stats/NativeBatch2DStats.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatchRenderRecord.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUTextureInfo.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/PipelineCache.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <vector>
#include "QuadBatchDescriptorSets.hpp"
#include "QuadBatchVertexBuffers.hpp"

namespace Fsl::Vulkan
{
  //! @brief Render quads on demand.
  //! @note If BeginFrame is called while the Vulkan resources are not ready we enter 'Void' Draw mode. This means that all draw commands
  //!       will do no drawing, but the calls will appear to work. This can prevent that nasty crashes occur.
  class QuadBatch final
  {
    enum class FrameState
    {
      NotReady,
      DrawFrame,
      DrawVoidFrame,
    };

    struct PushConstantRecord
    {
      float Spread{};

      constexpr PushConstantRecord() = default;

      constexpr explicit PushConstantRecord(const float spread)
        : Spread(spread)
      {
      }

      constexpr bool operator==(const PushConstantRecord& rhs) const
      {
        return Spread == rhs.Spread;
      }

      constexpr bool operator!=(const PushConstantRecord& rhs) const
      {
        return !(*this == rhs);
      }
    };

    struct DeviceResources
    {
      bool IsValid = false;
      VUPhysicalDeviceRecord PhysicalDevice;
      VUBufferMemory UniformBuffer;

      RapidVulkan::DescriptorSetLayout DescriptorSetLayoutUniform;
      RapidVulkan::DescriptorPool DescriptorPool;
      // We use the native type here since this is managed by a pool
      VkDescriptorSet DescriptorSetUniform = VK_NULL_HANDLE;

      RapidVulkan::DescriptorSetLayout DescriptorSetTexture;
      std::vector<QuadBatchRenderRecord> Render;

      RapidVulkan::ShaderModule VertexShader;
      RapidVulkan::ShaderModule FragmentShader;
      RapidVulkan::ShaderModule SdfFragmentShader;

      RapidVulkan::PipelineLayout PipelineLayout;
      RapidVulkan::PipelineLayout PipelineLayoutSdf;
      RapidVulkan::PipelineCache PipelineCache;

      PxExtent2D CachedScreenExtentPx;
    };

    struct DependentResources
    {
      bool IsValid = false;
      uint32_t MaxFramesInFlight = 0;
      //! Additive
      RapidVulkan::GraphicsPipeline PipelineAdditive;
      //! AlphaBlend
      RapidVulkan::GraphicsPipeline PipelineAlphaBlend;
      //! NonPremultiplied
      RapidVulkan::GraphicsPipeline PipelineNonPremultiplied;
      //! Opaque blending
      RapidVulkan::GraphicsPipeline PipelineOpaque;
      //! Sdf
      RapidVulkan::GraphicsPipeline PipelineSdf;
    };

    //! Information that is valid during a Begin/End sequence
    struct ActiveBlock
    {
      bool IsValid{false};
      VkPipeline ActivePipeline{VK_NULL_HANDLE};
      VkPipelineLayout ActivePipelineLayout{VK_NULL_HANDLE};
      VkPipeline CachedPipeline{VK_NULL_HANDLE};
      VUTextureInfo CachedTextureInfo;
      PushConstantRecord CachedPushConstants{-1.0f};

      bool CheckIsEmpty() const
      {
        return !IsValid && ActivePipeline == VK_NULL_HANDLE && ActivePipelineLayout == VK_NULL_HANDLE && CachedPipeline == VK_NULL_HANDLE &&
               CachedTextureInfo == VUTextureInfo() && CachedPushConstants == PushConstantRecord(-1.0f);
      }
    };

    //! Information that is valid during a BeginFrame/EndFrame sequence
    struct ActiveFrame
    {
      FrameState CurrentState = FrameState::NotReady;
      VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
      uint32_t FrameIndex = 0;
      ActiveBlock Block;

      bool CheckIsEmpty() const
      {
        return CurrentState == FrameState::NotReady && CommandBuffer == VK_NULL_HANDLE && FrameIndex == 0 && Block.CheckIsEmpty();
      }
    };

    std::vector<uint8_t> m_vertexShaderBinary;
    std::vector<uint8_t> m_fragmentShaderBinary;
    std::vector<uint8_t> m_sdfFragmentShaderBinary;

    uint32_t m_quadCapacity;
    // uint32_t m_vertexCapacity;
    bool m_logEnabled;

    DeviceResources m_deviceResource;
    DependentResources m_dependentResource;

    ActiveFrame m_activeFrame;
    PushConstantRecord m_pushConstantsDefault;
    PushConstantRecord m_pushConstants;

    NativeBatch2DStats m_stats{};

  public:
    QuadBatch(const QuadBatch&) = delete;
    QuadBatch& operator=(const QuadBatch&) = delete;

    QuadBatch(std::vector<uint8_t> vertexShaderBinary, std::vector<uint8_t> fragmentShaderBinary, std::vector<uint8_t> sdfFragmentShaderBinary,
              const uint32_t quadCapacityHint, const bool logEnabled = true);

    ~QuadBatch();

    //! @param frameIndex the index (must be < maxFramesInFlight supplied during construction)
    void BeginFrame(const VkCommandBuffer commandBuffer, const uint32_t frameIndex);
    void Begin(const PxSize2D& sizePx, const BlendState blendState, const BatchSdfRenderConfig& sdfRenderConfig, const bool restoreState);
    void DrawQuads(const VertexPositionColorTexture* const pVertices, const uint32_t length, const VUTextureInfo& textureInfo);
    void End();
    void EndFrame();

    NativeBatch2DStats GetStats() const
    {
      return m_stats;
    }

    //! @param device the active device
    void CreateDeviceResources(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device);

    //! @brief Destroy all device resources (if there are dependent resources allocated this will destroy those too)
    //! @warning If called while inside a Begin/End or BeginFrame/EndFrame scope the scope will be terminated and a warning will be logged.
    void DestroyDeviceResources() noexcept;

    //! @brief Create all dependent resources
    //! @param maxFramesInFlight the number of different command buffers that will be passed to BeginFrame,
    //!        this is often equal to the swapchain image count.
    void CreateDependentResources(const uint32_t maxFramesInFlight, const VkRenderPass renderPass, const uint32_t subpass,
                                  const PxExtent2D& screenExtentPx);

    //! @brief Destroy all dependent resources
    //! @warning If called while inside a Begin/End or BeginFrame/EndFrame scope the scope will be terminated and a warning will be logged.
    void DestroyDependentResources() noexcept;

    void EnforceCommandBufferCount(const uint32_t maxFramesInFlight);

  private:
    void DoEnd() noexcept;
    void DoEndFrame() noexcept;
  };
}

#endif
