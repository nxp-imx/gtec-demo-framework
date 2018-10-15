#ifndef FSLUTIL_VULKAN1_0_BATCH_QUADBATCH_HPP
#define FSLUTIL_VULKAN1_0_BATCH_QUADBATCH_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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
#include <FslBase/Math/Extent2D.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/PipelineCache.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUTextureInfo.hpp>
#include <vector>
#include "QuadBatchDescriptorSets.hpp"
#include "QuadBatchVertexBuffers.hpp"

namespace Fsl
{
  namespace Vulkan
  {
    class QuadBatch
    {
      //! Contains things associated with a command buffer
      class RenderRecord
      {
      public:
        RenderRecord(const RenderRecord&) = delete;
        RenderRecord& operator=(const RenderRecord&) = delete;

        QuadBatchDescriptorSets TextureDescriptorSets;
        QuadBatchVertexBuffers VertexBuffers;

        RenderRecord() = default;

        bool IsValid() const
        {
          return TextureDescriptorSets.IsValid() && VertexBuffers.IsValid();
        }

        void Reset()
        {
          TextureDescriptorSets.Reset();
          VertexBuffers.Reset();
        }

        void Reset(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkDescriptorSetLayout descriptorSetLayout,
                   const uint32_t minimumVertexCountRequest)
        {
          Reset();
          TextureDescriptorSets.Reset(device, descriptorSetLayout);
          VertexBuffers.Reset(physicalDevice, device, minimumVertexCountRequest);
        }

        void Clear()
        {
          TextureDescriptorSets.Clear();
          VertexBuffers.Clear();
        }
      };

      uint32_t m_commandBufferCount;
      uint32_t m_quadCapacity;
      uint32_t m_vertexCapacity;

      VUBuffer m_uniformBuffer;

      RapidVulkan::DescriptorSetLayout m_descriptorSetLayoutUniform;
      RapidVulkan::DescriptorPool m_descriptorPool;
      // We use the native type here since this is managed by a pool
      VkDescriptorSet m_descriptorSetUniform;

      RapidVulkan::DescriptorSetLayout m_descriptorSetTexture;
      std::vector<RenderRecord> m_render;

      RapidVulkan::ShaderModule m_vertexShader;
      RapidVulkan::ShaderModule m_fragmentShader;

      RapidVulkan::PipelineLayout m_pipelineLayout;
      RapidVulkan::PipelineCache m_pipelineCache;

      //! Additive
      RapidVulkan::GraphicsPipeline m_pipelineAdditive;
      //! AlphaBlend
      RapidVulkan::GraphicsPipeline m_pipelineAlphaBlend;
      //! NonPremultiplied
      RapidVulkan::GraphicsPipeline m_pipelineNonPremultiplied;
      //! Opaque blending
      RapidVulkan::GraphicsPipeline m_pipelineOpaque;


      Extent2D m_cachedScreenExtent;
      VkPipeline m_cachedPipeline = VK_NULL_HANDLE;
      VUTextureInfo m_cachedTextureInfo;
      VkDescriptorSet m_cachedTextureDescriptorSet = VK_NULL_HANDLE;

      uint32_t m_activeCommandBufferIndex = 0;
      VkPipeline m_activePipeline = VK_NULL_HANDLE;
      VkCommandBuffer m_activeCommandBuffer = VK_NULL_HANDLE;

    public:
      QuadBatch(const QuadBatch&) = delete;
      QuadBatch& operator=(const QuadBatch&) = delete;

      //! @param commandBufferCount the number of different command buffers that will be passed to BeginFrame,
      //!        this is often equal to the swapchain image count.
      QuadBatch(const VUDevice& device, const VkRenderPass renderPass, const uint32_t commandBufferCount,
                const std::vector<uint8_t>& vertexShaderBinary, const std::vector<uint8_t>& fragmentShaderBinary, const uint32_t quadCapacityHint);

      //! @param commandBufferIndex the index (must be < commandBufferCount supplied during construction)
      void BeginFrame(const uint32_t commandBufferIndex, const VkCommandBuffer commandBuffer, const Extent2D& screenExtent);
      void Begin(const Point2& screenResolution, const BlendState blendState, const bool restoreState);
      void DrawQuads(const VertexPositionColorTexture* const pVertices, const uint32_t length, const VUTextureInfo& textureInfo);
      void End();
      void EndFrame();
    };
  }
}

#endif
