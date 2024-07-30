/****************************************************************************************************************************************************
 * Copyright 2017, 2022, 2024 NXP
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

#include <FslBase/Bits/AlignmentUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Font/SdfFontUtil.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslGraphics/Vertices/VertexElementFormat.hpp>
#include <FslUtil/Vulkan1_0/Batch/ConfigHelper.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatch.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VUBufferMemoryUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Debug/Strings/VkResult.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <utility>

namespace Fsl::Vulkan
{
  namespace
  {
    namespace LocalConfig
    {
      // const uint32_t DEFAULT_COMMAND_BUFFER_COUNT = 2;
      constexpr uint32_t QuadMinBatchSize = 2048;
      constexpr int32_t InternalQuadVertexCount = 6;
    }

    VUBufferMemory CreateBuffer(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkDeviceSize bufferByteSize,
                                const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags)
    {
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = bufferByteSize;
      bufferCreateInfo.usage = usageFlags;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      return {physicalDevice, device, bufferCreateInfo, memoryPropertyFlags};
    }


    VUBufferMemory CreateUniformBuffer(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice& device)
    {
      const auto bufferByteSize = AlignmentUtil::GetByteSize(sizeof(Matrix), 16);
      return CreateBuffer(physicalDevice, device, bufferByteSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const VkDevice device, const VkDescriptorType descriptorType,
                                                               const VkShaderStageFlags stageFlags)
    {
      // Descriptor set layout
      VkDescriptorSetLayoutBinding setLayoutBindings{};
      setLayoutBindings.binding = 0;
      setLayoutBindings.descriptorType = descriptorType;
      setLayoutBindings.descriptorCount = 1;
      setLayoutBindings.stageFlags = stageFlags;

      VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
      descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorSetLayoutInfo.pNext = nullptr;
      descriptorSetLayoutInfo.bindingCount = 1;
      descriptorSetLayoutInfo.pBindings = &setLayoutBindings;

      return {device, descriptorSetLayoutInfo};
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const VkDevice device, const VkDescriptorType type)
    {
      VkDescriptorPoolSize poolSize{};
      poolSize.type = type;
      poolSize.descriptorCount = 1;

      VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
      descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolCreateInfo.maxSets = 1;
      descriptorPoolCreateInfo.poolSizeCount = 1;
      descriptorPoolCreateInfo.pPoolSizes = &poolSize;

      return {device, descriptorPoolCreateInfo};
    }


    VkDescriptorSet CreateDescriptorSet(const VkDevice device, const RapidVulkan::DescriptorPool& descriptorPool,
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout)
    {
      VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
      descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      descriptorSetAllocInfo.descriptorPool = descriptorPool.Get();
      descriptorSetAllocInfo.descriptorSetCount = 1;
      descriptorSetAllocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      VkDescriptorSet descriptorSet{};
      RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(device, &descriptorSetAllocInfo, &descriptorSet));
      return descriptorSet;
    }


    RapidVulkan::PipelineCache CreatePipelineCache(const VkDevice device)
    {
      // Pipeline cache
      VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
      pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
      return {device, pipelineCacheCreateInfo};
    }

    void UpdateDescriptorSetUniform(const VkDevice device, const VkDescriptorSet descriptorSet, const VkDescriptorBufferInfo& bufferInfo)
    {
      VkWriteDescriptorSet writeDescriptorSet{};
      // Binding 0: Vertex shader uniform buffer
      writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSet.dstSet = descriptorSet;
      writeDescriptorSet.dstBinding = 0;
      writeDescriptorSet.descriptorCount = 1;
      writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSet.pBufferInfo = &bufferInfo;

      vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
    }

    void UpdateDescriptorSetTexture(const VkDevice device, const VkDescriptorSet descriptorSet, const VkDescriptorImageInfo& imageInfo)
    {
      VkWriteDescriptorSet writeDescriptorSet{};
      // Binding 0: Fragment shader texture sampler
      writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSet.dstSet = descriptorSet;
      writeDescriptorSet.dstBinding = 0;
      writeDescriptorSet.descriptorCount = 1;
      writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSet.pImageInfo = &imageInfo;

      vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
    }
  }


  QuadBatch::QuadBatch(std::vector<uint8_t> vertexShaderBinary, std::vector<uint8_t> fragmentShaderBinary,
                       std::vector<uint8_t> sdfFragmentShaderBinary, const uint32_t quadCapacityHint, const bool logEnabled)
    : m_vertexShaderBinary(std::move(vertexShaderBinary))
    , m_fragmentShaderBinary(std::move(fragmentShaderBinary))
    , m_sdfFragmentShaderBinary(std::move(sdfFragmentShaderBinary))
    , m_quadCapacity(std::max(quadCapacityHint, LocalConfig::QuadMinBatchSize))
    //, m_vertexCapacity(LocalConfig::InternalQuadVertexCount * m_quadCapacity)
    , m_logEnabled(logEnabled)
    , m_pushConstantsDefault(SdfFontUtil::CalcSmooth(4.0f, 1.0f))
  {
  }


  QuadBatch::~QuadBatch()
  {
    DestroyDeviceResources();
  }


  void QuadBatch::BeginFrame(const VkCommandBuffer commandBuffer, const uint32_t frameIndex)
  {
    // First we check that 'activeFrame' state is correct (to ensure that the 'Usage' is correct)
    if (m_activeFrame.CurrentState != FrameState::NotReady)
    {
      throw UsageErrorException("Can not call BeginFrame while inside a BeginFrame/EndFrame block");
    }

    // Then we check the 'resource' state to determine how to proceed
    if (!m_dependentResource.IsValid)
    {
      // We just log a warning about the QuadBatch operating in 'null' mode.
      // This means that all draw operations do nothing except log warnings if called.
      FSLLOG3_WARNING_IF(m_logEnabled && commandBuffer == VK_NULL_HANDLE, "Resources not ready, void mode enabled.");
      FSLLOG3_WARNING_IF(m_logEnabled && commandBuffer != VK_NULL_HANDLE,
                         "Resources not ready, void mode enabled. Supplied command buffer in null mode ought to be VK_NULL_HANDLE");
      m_activeFrame.CurrentState = FrameState::DrawVoidFrame;
      return;
    }

    // Finally we validate the input parameters
    if (commandBuffer == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("commandBuffer can not be invalid");
    }
    if (frameIndex > m_dependentResource.MaxFramesInFlight)
    {
      throw std::invalid_argument("frameIndex should be less than maxFramesInFlight");
    }


    assert(m_deviceResource.IsValid);

    // Prepare the device render resources for the given command index for a new frame
    assert(m_deviceResource.Render[frameIndex].IsValid());
    m_deviceResource.Render[frameIndex].Clear();

    // Reset the push constants to their default value
    m_pushConstants = m_pushConstantsDefault;
    m_activeFrame.CurrentState = FrameState::DrawFrame;
    m_activeFrame.CommandBuffer = commandBuffer;
    m_activeFrame.FrameIndex = frameIndex;
    m_stats = {};
  }


  void QuadBatch::Begin(const PxSize2D& sizePx, const BlendState blendState, const BatchSdfRenderConfig& sdfRenderConfig, const bool /*restoreState*/)
  {
    // First we ensure that we that the 'activeFrame' state is correct
    if (m_activeFrame.CurrentState == FrameState::NotReady)
    {
      throw UsageErrorException("Call BeginFrame before Begin");
    }
    if (m_activeFrame.Block.IsValid)
    {
      throw UsageErrorException("Calling Begin inside a Begin 'scope' is not allowed");
    }

    // Then we check the input parameters
    if ((static_cast<uint32_t>(sizePx.RawWidth()) != m_deviceResource.CachedScreenExtentPx.Width.Value ||
         static_cast<uint32_t>(sizePx.RawHeight()) != m_deviceResource.CachedScreenExtentPx.Height.Value) &&
        m_activeFrame.CurrentState != FrameState::DrawVoidFrame)
    {
      throw NotSupportedException("Dynamic changes of the screen resolution is not supported");
    }

    // Finally we proceed with the actual 'Begin' operation
    assert(m_activeFrame.CommandBuffer != VK_NULL_HANDLE || m_activeFrame.CurrentState == FrameState::DrawVoidFrame);
    assert(!m_activeFrame.Block.IsValid);
    VkPipeline selectedPipeline = VK_NULL_HANDLE;
    VkPipelineLayout selectedPipelineLayout = VK_NULL_HANDLE;
    switch (blendState)
    {
    case BlendState::Additive:
      selectedPipeline = m_dependentResource.PipelineAdditive.Get();
      selectedPipelineLayout = m_deviceResource.PipelineLayout.Get();
      break;
    case BlendState::AlphaBlend:
      selectedPipeline = m_dependentResource.PipelineAlphaBlend.Get();
      selectedPipelineLayout = m_deviceResource.PipelineLayout.Get();
      break;
    case BlendState::NonPremultiplied:
      selectedPipeline = m_dependentResource.PipelineNonPremultiplied.Get();
      selectedPipelineLayout = m_deviceResource.PipelineLayout.Get();
      break;
    case BlendState::Opaque:
      selectedPipeline = m_dependentResource.PipelineOpaque.Get();
      selectedPipelineLayout = m_deviceResource.PipelineLayout.Get();
      break;
    case BlendState::Sdf:
      selectedPipeline = m_dependentResource.PipelineSdf.Get();
      selectedPipelineLayout = m_deviceResource.PipelineLayoutSdf.Get();
      m_pushConstants.Spread = SdfFontUtil::CalcSmooth(sdfRenderConfig.Spread, sdfRenderConfig.Scale);
      break;
    default:
      throw NotSupportedException("Unsupported blend state");
    }
    assert(selectedPipeline != VK_NULL_HANDLE || m_activeFrame.CurrentState == FrameState::DrawVoidFrame);
    m_activeFrame.Block.IsValid = true;
    m_activeFrame.Block.ActivePipeline = selectedPipeline;
    m_activeFrame.Block.ActivePipelineLayout = selectedPipelineLayout;
  }

  // Basic quad vertex format
  // 0-1
  // | |
  // 2-3
  void QuadBatch::DrawQuads(const VertexPositionColorTexture* const pVertices, const uint32_t length, const VUTextureInfo& textureInfo)
  {
    if (!m_activeFrame.Block.IsValid)
    {
      throw UsageErrorException("Call Begin before DrawQuads");
    }
    assert(m_activeFrame.CurrentState != FrameState::NotReady);
    if (m_activeFrame.CurrentState == FrameState::DrawVoidFrame)
    {
      // In void draw mode we just ignore draw calls
      return;
    }

    auto& rRender = m_deviceResource.Render[m_activeFrame.FrameIndex];

    const VertexPositionColorTexture* pSrcVertices = pVertices;
    const VertexPositionColorTexture* const pSrcVerticesEnd = pVertices + (length * 4);

    uint32_t remainingQuads = length;
    while (pSrcVertices < pSrcVerticesEnd)
    {
      auto current = rRender.VertexBuffers.NextFree(remainingQuads * LocalConfig::InternalQuadVertexCount);

      auto* pDst = current.pMapped;
      auto* pDstEnd = current.pMapped + current.VertexCapacity;

      static_assert(LocalConfig::InternalQuadVertexCount == 6, "the code below expects the internal quad vertex count to be six");
      while (pDst < pDstEnd)
      {
        // Expand the quad to two counter clockwise triangles
        pDst[0] = pSrcVertices[2];
        pDst[1] = pSrcVertices[3];
        pDst[2] = pSrcVertices[0];

        pDst[3] = pSrcVertices[0];
        pDst[4] = pSrcVertices[3];
        pDst[5] = pSrcVertices[1];

        pSrcVertices += 4;
        pDst += LocalConfig::InternalQuadVertexCount;
        --remainingQuads;
      }

      assert(m_activeFrame.CommandBuffer != VK_NULL_HANDLE);
      assert(m_activeFrame.Block.ActivePipeline != VK_NULL_HANDLE);

      if (m_activeFrame.Block.ActivePipelineLayout == m_deviceResource.PipelineLayoutSdf.Get() &&
          m_pushConstants != m_activeFrame.Block.CachedPushConstants)
      {
        vkCmdPushConstants(m_activeFrame.CommandBuffer, m_activeFrame.Block.ActivePipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(m_pushConstants), &m_pushConstants);
        m_activeFrame.Block.CachedPushConstants = m_pushConstants;
      }

      VkPipeline currentPipeline = m_activeFrame.Block.ActivePipeline;
      if (currentPipeline != m_activeFrame.Block.CachedPipeline)
      {
        m_activeFrame.Block.CachedPipeline = currentPipeline;
        vkCmdBindPipeline(m_activeFrame.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline);
      }

      if (textureInfo != m_activeFrame.Block.CachedTextureInfo)
      {
        m_activeFrame.Block.CachedTextureInfo = textureInfo;
        VkDescriptorSet textureDescriptorSet = rRender.TextureDescriptorSets.NextFree();

        assert(textureDescriptorSet != VK_NULL_HANDLE);

        UpdateDescriptorSetTexture(m_deviceResource.UniformBuffer.GetDevice(), textureDescriptorSet, textureInfo.ImageInfo);

        std::array<VkDescriptorSet, 2> descriptorSets = {m_deviceResource.DescriptorSetUniform, textureDescriptorSet};
        vkCmdBindDescriptorSets(m_activeFrame.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_activeFrame.Block.ActivePipelineLayout, 0,
                                static_cast<int32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
      }

      VkDeviceSize offsets = 0;
      vkCmdBindVertexBuffers(m_activeFrame.CommandBuffer, 0, 1, &current.VertexBuffer, &offsets);
      vkCmdDraw(m_activeFrame.CommandBuffer, current.VertexCapacity, 1, current.UsedStartIndex, 0);
      ++m_stats.DrawCalls;
    }
    m_stats.Vertices += (length * 6);
  }


  void QuadBatch::End()
  {
    if (!m_activeFrame.Block.IsValid)
    {
      throw UsageErrorException("Call Begin before End");
    }
    DoEnd();
  }


  void QuadBatch::EndFrame()
  {
    if (m_activeFrame.Block.IsValid)
    {
      throw UsageErrorException("Can not call EndFrame inside a Begin/End block");
    }
    if (m_activeFrame.CurrentState == FrameState::NotReady)
    {
      throw UsageErrorException("Can not call EndFrame without a BeginFrame");
    }
    DoEndFrame();
  }


  void QuadBatch::CreateDeviceResources(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device)
  {
    if (!physicalDevice.IsValid())
    {
      throw std::invalid_argument("physicalDevice must be valid");
    }
    if (device == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("device must be valid");
    }

    try
    {
      // We set IsValid here to ensure that the DestroyDeviceResources tries to free everything if its called
      // Just don't call any other method that works on m_deviceResources before its been fully filled
      m_deviceResource.IsValid = true;
      m_deviceResource.PhysicalDevice = physicalDevice;
      m_deviceResource.UniformBuffer = CreateUniformBuffer(physicalDevice, device);
      m_deviceResource.DescriptorSetLayoutUniform = CreateDescriptorSetLayout(device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
      m_deviceResource.DescriptorPool = CreateDescriptorPool(device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
      m_deviceResource.DescriptorSetUniform =
        CreateDescriptorSet(device, m_deviceResource.DescriptorPool, m_deviceResource.DescriptorSetLayoutUniform);
      m_deviceResource.DescriptorSetTexture =
        CreateDescriptorSetLayout(device, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
      m_deviceResource.VertexShader.Reset(device, 0, m_vertexShaderBinary);
      m_deviceResource.FragmentShader.Reset(device, 0, m_fragmentShaderBinary);
      m_deviceResource.SdfFragmentShader.Reset(device, 0, m_sdfFragmentShaderBinary);

      m_deviceResource.PipelineLayout =
        ConfigHelper::CreatePipelineLayout(device, m_deviceResource.DescriptorSetLayoutUniform.Get(), m_deviceResource.DescriptorSetTexture.Get());
      m_deviceResource.PipelineLayoutSdf = ConfigHelper::CreatePipelineLayoutWithPushConstant(
        device, m_deviceResource.DescriptorSetLayoutUniform.Get(), m_deviceResource.DescriptorSetTexture.Get(), sizeof(PushConstantRecord));

      m_deviceResource.PipelineCache = CreatePipelineCache(device);

      m_deviceResource.Render.clear();
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR_IF(m_logEnabled, "Exception occurred during QuadBatch device resource creation: {}", ex.what());
      DestroyDeviceResources();
      throw;
    }
  }


  void QuadBatch::DestroyDeviceResources() noexcept
  {
    if (!m_deviceResource.IsValid)
    {
      return;
    }

    // Dependent resources, we also rely on DestroyDependent resources to fix any broke state and do the necessary waits
    DestroyDependentResources();

    assert(!m_activeFrame.Block.IsValid);
    assert(m_activeFrame.CurrentState == FrameState::NotReady);

    m_deviceResource = {};
  }


  void QuadBatch::CreateDependentResources(const uint32_t maxFramesInFlight, const VkRenderPass renderPass, const uint32_t subpass,
                                           const PxExtent2D& screenExtentPx)
  {
    if (m_activeFrame.CurrentState != FrameState::NotReady)
    {
      throw UsageErrorException("Can not RecreateResources during BeginFrame/EndFrame");
    }
    if (!m_deviceResource.IsValid)
    {
      throw UsageErrorException("Can't create dependent resources before the normal resources");
    }
    if (renderPass == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("renderPass can not be null");
    }
    if (maxFramesInFlight <= 0)
    {
      throw std::invalid_argument("maxFramesInFlight should be >= 1");
    }


    if (screenExtentPx != m_deviceResource.CachedScreenExtentPx)
    {
      m_deviceResource.CachedScreenExtentPx = screenExtentPx;

      // Setup the shader
      const auto screenWidth = static_cast<float>(screenExtentPx.Width.Value);
      const auto screenHeight = static_cast<float>(screenExtentPx.Height.Value);
      const float screenOffsetX = screenWidth / 2.0f;
      const float screenOffsetY = screenHeight / 2.0f;

      const Matrix matViewProj =
        Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, -2.0f) * Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      VUBufferMemoryUtil::Upload(m_deviceResource.UniformBuffer, 0, matViewProj);
      UpdateDescriptorSetUniform(m_deviceResource.UniformBuffer.GetDevice(), m_deviceResource.DescriptorSetUniform,
                                 m_deviceResource.UniformBuffer.GetDescriptorBufferInfo());
    }

    EnforceCommandBufferCount(maxFramesInFlight);

    try
    {
      VkDevice const device = m_deviceResource.UniformBuffer.GetDevice();

      const auto vertexDecl = VertexPositionColorTexture::AsVertexDeclarationSpan();

      // We set IsValid here to ensure that the DestroyDependentResources tries to free everything if its called.
      // Just don't call any other method that works on m_dependentResource before its been fully filled

      const VkShaderModule hVertexShader = m_deviceResource.VertexShader.Get();
      const VkShaderModule hFragShader = m_deviceResource.FragmentShader.Get();
      const VkShaderModule hFragShaderSdf = m_deviceResource.SdfFragmentShader.Get();
      const VkPipelineCache hPipelineCache = m_deviceResource.PipelineCache.Get();
      const VkPipelineLayout pipelineLayout = m_deviceResource.PipelineLayout.Get();
      const VkPipelineLayout pipelineLayoutSdf = m_deviceResource.PipelineLayoutSdf.Get();
      constexpr BasicCullMode CullMode = BasicCullMode::Back;
      constexpr BasicFrontFace FrontFace = BasicFrontFace::CounterClockwise;

      m_dependentResource.IsValid = true;
      m_dependentResource.MaxFramesInFlight = maxFramesInFlight;
      m_dependentResource.PipelineAdditive =
        ConfigHelper::CreateGraphicsPipeline(device, hVertexShader, hFragShader, pipelineLayout, hPipelineCache, renderPass, subpass, screenExtentPx,
                                             BlendState::Additive, CullMode, FrontFace, vertexDecl);

      m_dependentResource.PipelineAlphaBlend =
        ConfigHelper::CreateGraphicsPipeline(device, hVertexShader, hFragShader, pipelineLayout, hPipelineCache, renderPass, subpass, screenExtentPx,
                                             BlendState::AlphaBlend, CullMode, FrontFace, vertexDecl);

      m_dependentResource.PipelineNonPremultiplied =
        ConfigHelper::CreateGraphicsPipeline(device, hVertexShader, hFragShader, pipelineLayout, hPipelineCache, renderPass, subpass, screenExtentPx,
                                             BlendState::NonPremultiplied, CullMode, FrontFace, vertexDecl);

      m_dependentResource.PipelineOpaque =
        ConfigHelper::CreateGraphicsPipeline(device, hVertexShader, hFragShader, pipelineLayout, hPipelineCache, renderPass, subpass, screenExtentPx,
                                             BlendState::Opaque, CullMode, FrontFace, vertexDecl);

      m_dependentResource.PipelineSdf =
        ConfigHelper::CreateGraphicsPipeline(device, hVertexShader, hFragShaderSdf, pipelineLayoutSdf, hPipelineCache, renderPass, subpass,
                                             screenExtentPx, BlendState::Sdf, CullMode, FrontFace, vertexDecl);
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR_IF(m_logEnabled, "Exception occurred during QuadBatch dependent resource creation: {}", ex.what());
      DestroyDependentResources();
      throw;
    }
  }

  void QuadBatch::DestroyDependentResources() noexcept
  {
    if (m_activeFrame.Block.IsValid)
    {
      FSLLOG3_ERROR_IF(m_logEnabled, "DestroyDependentResources called inside a Begin/End block, force ending it");
      DoEnd();
    }
    if (m_activeFrame.CurrentState != FrameState::NotReady)
    {
      FSLLOG3_ERROR_IF(m_logEnabled, "DestroyDependentResources called inside a BeginFrame/EndFrame block, force ending it");
      DoEndFrame();
    }

    if (!m_deviceResource.IsValid)
    {
      assert(!m_dependentResource.IsValid);
      return;
    }

    auto waitResult = vkDeviceWaitIdle(m_deviceResource.UniformBuffer.GetDevice());
    if (waitResult != VK_SUCCESS)
    {
      FSLLOG3_ERROR_IF(m_logEnabled, "vkDeviceWaitIdle failed with: {}({}) at {} line: {}", RapidVulkan::Debug::ToString(waitResult),
                       static_cast<int64_t>(waitResult), __FILE__, __LINE__);
    }

    if (!m_dependentResource.IsValid)
    {
      return;
    }
    assert(m_deviceResource.IsValid);

    // Release all dependent resources
    m_dependentResource = {};

    // Sanity check that the m_activeFrame is in its default state
    assert(m_activeFrame.CheckIsEmpty());
  }

  void QuadBatch::EnforceCommandBufferCount(const uint32_t maxFramesInFlight)
  {
    assert(m_deviceResource.IsValid);
    if (maxFramesInFlight == m_deviceResource.Render.size())
    {
      return;
    }
    m_deviceResource.Render.resize(maxFramesInFlight);
    VkDevice device = m_deviceResource.UniformBuffer.GetDevice();
    assert(device != VK_NULL_HANDLE);
    for (auto& rEntry : m_deviceResource.Render)
    {
      if (!rEntry.IsValid())
      {
        rEntry.Reset(m_deviceResource.PhysicalDevice, device, m_deviceResource.DescriptorSetTexture.Get(), LocalConfig::InternalQuadVertexCount);
      }
    }
  }

  void QuadBatch::DoEnd() noexcept
  {
    assert(m_activeFrame.Block.IsValid);
    assert(m_activeFrame.CurrentState != FrameState::NotReady);

    m_activeFrame.Block = {};
    assert(m_activeFrame.Block.CheckIsEmpty());
  }

  void QuadBatch::DoEndFrame() noexcept
  {
    assert(!m_activeFrame.Block.IsValid);
    assert(m_activeFrame.CurrentState != FrameState::NotReady);

    // Sanity check that the m_activeFrame.Block has been returned to its default state
    assert(m_activeFrame.Block.CheckIsEmpty());
    m_activeFrame = {};

    // Sanity check that the m_activeFrame is in its default state
    assert(m_activeFrame.CheckIsEmpty());
  }
}
