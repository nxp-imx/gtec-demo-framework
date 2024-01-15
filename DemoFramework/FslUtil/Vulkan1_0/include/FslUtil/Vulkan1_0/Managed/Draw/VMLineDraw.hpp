#ifndef FSLUTIL_VULKAN1_0_MANAGED_DRAW_VMLINEDRAW_HPP
#define FSLUTIL_VULKAN1_0_MANAGED_DRAW_VMLINEDRAW_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
#include <FslGraphics/Vertices/VertexSpan.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <array>
#include <vector>

namespace Fsl::Vulkan
{
  class VMLineDraw
  {
    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      FrameResources(const FrameResources&) = delete;
      FrameResources& operator=(const FrameResources&) = delete;

      FrameResources() = default;

      Vulkan::VUBufferMemory VertUboBuffer;
      VkDescriptorSet DescriptorSet{};
      Vulkan::VMVertexBuffer LineVertBuffer;

      void Reset() noexcept
      {
        // Use destruction order
        LineVertBuffer.Reset();
        DescriptorSet = {};
        VertUboBuffer.Reset();
      }

      FrameResources(FrameResources&& other) noexcept
        : VertUboBuffer(std::move(other.VertUboBuffer))
        , DescriptorSet(other.DescriptorSet)
        , LineVertBuffer(std::move(other.LineVertBuffer))
      {
        // Remove the data from other
        other.DescriptorSet = {};
      }

      FrameResources& operator=(FrameResources&& other) noexcept
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          Reset();

          // Claim ownership here
          VertUboBuffer = std::move(other.VertUboBuffer);
          DescriptorSet = other.DescriptorSet;
          LineVertBuffer = std::move(other.LineVertBuffer);

          // Remove the data from other
          other.DescriptorSet = {};
        }
        return *this;
      }
    };

    struct Resources
    {
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;

      Resources() = default;

      bool IsValid{false};
      std::shared_ptr<VMBufferManager> BufferManager;
      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      RapidVulkan::PipelineLayout MainPipelineLayout;
      std::vector<FrameResources> MainFrameResources;

      void Reset()
      {
        // Use destruction order
        MainFrameResources.clear();
        MainPipelineLayout.Reset();
        MainDescriptorPool.Reset();
        MainDescriptorSetLayout.Reset();
        BufferManager.reset();
        IsValid = false;
      }
    };

    struct DependentResources
    {
      DependentResources(const DependentResources&) = delete;
      DependentResources& operator=(const DependentResources&) = delete;
      DependentResources(DependentResources&& other) noexcept = delete;
      DependentResources& operator=(DependentResources&& other) noexcept = delete;

      DependentResources() = default;

      bool IsValid{false};
      RapidVulkan::GraphicsPipeline PipelineRender;

      void Reset()
      {
        // Use destruction order
        PipelineRender.Reset();
        IsValid = false;
      }
    };

    Resources m_resources;
    DependentResources m_dependentResources;

  public:
    VMLineDraw() = default;
    VMLineDraw(const VUDevice& device, const std::shared_ptr<VMBufferManager>& bufferManager, const uint32_t maxFrames,
               const std::size_t sizeOfVertexUBOData, const uint32_t initialLineCapacity);

    void Reset() noexcept;
    void Reset(const VUDevice& device, const std::shared_ptr<VMBufferManager>& bufferManager, const uint32_t maxFrames,
               const std::size_t sizeOfVertexUBOData, const uint32_t initialLineCapacity);

    void BuildResources(const VkExtent2D& extent, const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                        const VkRenderPass renderPass, const uint32_t subpass, const bool dynamicScissor = false);
    void FreeResources() noexcept;

    void UpdateVertexUBO(const void* pVertexUBOData, const std::size_t& sizeOfVertexUBOData, const uint32_t frameIndex);

    void Draw(const VkCommandBuffer commandBuffer, const VertexPositionColor* pVertices, const std::size_t vertexCount, const uint32_t frameIndex);

    void Draw(const VkCommandBuffer commandBuffer, const VertexSpan<VertexPositionColor>& vertexSpan, const uint32_t frameIndex)
    {
      Draw(commandBuffer, vertexSpan.pVertices, vertexSpan.VertexCount, frameIndex);
    }

    template <std::size_t TSize>
    void Draw(const VkCommandBuffer commandBuffer, const std::array<VertexPositionColor, TSize>& vertices, const uint32_t frameIndex)
    {
      Draw(commandBuffer, vertices.data(), vertices.size(), frameIndex);
    }

    void Draw(const VkCommandBuffer commandBuffer, const std::vector<VertexPositionColor>& vertices, const uint32_t frameIndex)
    {
      Draw(commandBuffer, vertices.data(), vertices.size(), frameIndex);
    }

    bool IsValid() const
    {
      return m_resources.IsValid;
    }
  };
}

#endif
