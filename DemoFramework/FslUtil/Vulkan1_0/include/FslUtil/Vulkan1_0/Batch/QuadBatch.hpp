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
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/Semaphore.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUTextureInfo.hpp>
#include <vector>
#include "QuadBatchCommandBuffers.hpp"
#include "QuadBatchDescriptorSets.hpp"
#include "QuadBatchVertexBuffers.hpp"

namespace Fsl
{
  namespace Vulkan
  {
    class QuadBatch
    {
      uint32_t m_quadCapacity;
      uint32_t m_vertexCapacity;

      RapidVulkan::Semaphore m_quadRenderSemaphore;

      VUBuffer m_uniformBuffer;

      RapidVulkan::DescriptorSetLayout m_descriptorSetLayoutUniform;
      RapidVulkan::DescriptorPool m_descriptorPool;
      // We use the native type here since this is managed by a pool
      VkDescriptorSet m_descriptorSetUniform;
      QuadBatchDescriptorSets m_textureDescriptorSets;

      RapidVulkan::ShaderModule m_vertexShader;
      RapidVulkan::ShaderModule m_fragmentShader;

      RapidVulkan::PipelineLayout m_pipelineLayout;
      RapidVulkan::PipelineCache m_pipelineCache;
      RapidVulkan::RenderPass m_renderPass;
      // FIX: add different pipes for the blend modes
      RapidVulkan::GraphicsPipeline m_pipelineOpaque;

      QuadBatchVertexBuffers m_vertexBuffers;
      QuadBatchCommandBuffers m_commandBuffers;

      Extent2D m_cachedScreenExtent;
      VUTextureInfo m_cachedTextureInfo;
      VkDescriptorSet m_cachedTextureDescriptorSet;

      VkFramebuffer m_activeFramebuffer;
      VkRect2D m_activeFramebufferRect;
    public:
      QuadBatch(const VUDevice& device, const VkFormat colorFormat, const uint32_t queueFamilyIndex,
        const std::vector<uint8_t>& vertexShaderBinary, const std::vector<uint8_t>& fragmentShaderBinary, const uint32_t quadCapacityHint);

      VkSemaphore GetSignalSemaphore() const
      {
        return m_quadRenderSemaphore.Get();
      }

      void BeginFrame(const VkFramebuffer framebuffer, const Extent2D& screenExtent);
      void Begin(const Point2& screenResolution, const BlendState blendState, const bool restoreState);
      void DrawQuads(const VertexPositionColorTexture*const pVertices, const uint32_t length, const VUTextureInfo& textureInfo);
      void End();
      //! @brief Do the actual rendering
      //! @param pWaitSemaphores the semaphore that indicates we are ready to start the quad rendering
      void EndFrame(const VkQueue queue, const VkSemaphore*const pWaitSemaphores, const VkPipelineStageFlags*const pWaitDstStageMask, const uint32_t waitSemaphoreCount);

    };
  }
}

#endif
