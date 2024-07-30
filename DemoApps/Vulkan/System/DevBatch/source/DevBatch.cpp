/****************************************************************************************************************************************************
 * Copyright 2017, 2022-2024 NXP
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

#include "DevBatch.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxRectangleU16.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
// #include <FslBase/Log/Math/Pixel/FmtPxRectangleU32.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/BitmapFontDecoder.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Memory.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <memory>
#include <thread>

namespace Fsl
{
  using namespace Vulkan;

  namespace Local
  {
    constexpr const IO::PathView VertexShaderName("vertex_only_ndc.vert.spv");
    constexpr const IO::PathView FragmentShaderName("red.frag.spv");

    constexpr const IO::PathView PathFontAtlas("NonLinear/Font/NormalAtlas/FontAtlas.bta");
    constexpr const IO::PathView PathFontInfoOld("NonLinear/Font/NormalAtlas/FontAtlas_Font.fbk");
    constexpr const IO::PathView PathFontInfoNew("NonLinear/Font/NormalAtlas/FontAtlas_Font.nbf");
    constexpr const IO::PathView PathFontTexture("NonLinear/Font/NormalAtlas/FontAtlas.png");

    constexpr const IO::PathView PathSdfFontAtlas("Linear/Font/SdfAtlas/FontAtlas.bta");
    constexpr const IO::PathView PathSdfFontInfoOld("Linear/Font/SdfAtlas/FontAtlas_Font.fbk");
    constexpr const IO::PathView PathSdfFontInfoNew("Linear/Font/SdfAtlas/FontAtlas_Font.nbf");
    constexpr const IO::PathView PathSdfFontTexture("Linear/Font/SdfAtlas/FontAtlas.png");

    constexpr const auto MainSubPass = 0;

    constexpr const float ForcedTexWidth = 200.0f;
    constexpr const float ForcedTexHeight = 100.0f;
  }

  namespace
  {
    Vulkan::VMVertexBuffer CreateVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager)
    {
      constexpr std::array<VertexPosition, 3> Vertices = {VertexPosition(-0.5f, 0.5f, 0.0f), VertexPosition(0.5f, 0.5f, 0.0f),
                                                          VertexPosition(0.0f, -0.5f, 0.0f)};
      return {bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(Vertices), Vulkan::VMBufferUsage::STATIC};
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const VkDevice device)
    {
      return {device, 0, 0, nullptr, 0, nullptr};
    }


    RapidVulkan::GraphicsPipeline CreatePipeline(const VkDevice device, const VkShaderModule vertShaderModule, const VkShaderModule fragShaderModule,
                                                 const Vulkan::VMVertexBuffer& vertexBuffer, const VkExtent2D& swapchainImageExtent,
                                                 const VkPipelineLayout pipelineLayout, const VkRenderPass renderPass)
    {
      assert(device != VK_NULL_HANDLE);
      assert(vertShaderModule != VK_NULL_HANDLE);
      assert(fragShaderModule != VK_NULL_HANDLE);
      assert(vertexBuffer.IsValid());
      assert(pipelineLayout != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);

      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};
      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertShaderModule;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragShaderModule;
      pipelineShaderStageCreateInfo[1].pName = "main";
      pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

      VkVertexInputBindingDescription vertexInputBindingDescription{};
      vertexInputBindingDescription.binding = 0;
      vertexInputBindingDescription.stride = vertexBuffer.GetElementStride();
      vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      const auto vertexElement = vertexBuffer.GetVertexElement(VertexElementUsage::Position, 0);
      VkVertexInputAttributeDescription vertexInputAttributeDescription{};
      vertexInputAttributeDescription.location = 0;
      vertexInputAttributeDescription.binding = 0;
      vertexInputAttributeDescription.format = vertexElement.NativeFormat;
      vertexInputAttributeDescription.offset = vertexElement.Offset;

      VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
      pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      pipelineVertexInputCreateInfo.flags = 0;
      pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = 1;
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = &vertexInputAttributeDescription;

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.flags = 0;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(swapchainImageExtent.width);
      viewport.height = static_cast<float>(swapchainImageExtent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor = {{0, 0}, swapchainImageExtent};

      VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
      pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      pipelineViewportStateCreateInfo.flags = 0;
      pipelineViewportStateCreateInfo.viewportCount = 1;
      pipelineViewportStateCreateInfo.pViewports = &viewport;
      pipelineViewportStateCreateInfo.scissorCount = 1;
      pipelineViewportStateCreateInfo.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
      pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      pipelineRasterizationStateCreateInfo.flags = 0;
      pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
      pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
      pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
      pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
      pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
      pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

      VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
      pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      pipelineMultisampleStateCreateInfo.flags = 0;
      pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
      pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
      pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
      pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
      pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

      VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
      pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
      pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
      pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
      pipelineColorBlendAttachmentState.colorWriteMask = 0xf;

      VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
      pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      pipelineColorBlendStateCreateInfo.flags = 0;
      pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
      pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
      pipelineColorBlendStateCreateInfo.attachmentCount = 1;
      pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
      pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

      std::array<VkDynamicState, 2> dynamicState = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

      VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
      pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      pipelineDynamicStateCreateInfo.flags = 0;
      pipelineDynamicStateCreateInfo.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicState.size());
      pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState.data();

      VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
      graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      graphicsPipelineCreateInfo.flags = 0;
      graphicsPipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(pipelineShaderStageCreateInfo.size());
      graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
      graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
      graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
      graphicsPipelineCreateInfo.pTessellationState = nullptr;
      graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
      graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
      graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
      graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
      graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
      graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
      graphicsPipelineCreateInfo.layout = pipelineLayout;
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = 0;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return {device, VK_NULL_HANDLE, graphicsPipelineCreateInfo};
    }


    TextureAtlasSpriteFont ReadFont(const IContentManager& contentManager, const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                    const uint32_t densityDpi, const PxExtent2D textureExtentPx, const IO::Path& pathFontAtlas,
                                    const IO::Path& pathFontInfo)
    {
      FSLLOG3_INFO("  Basic texture atlas '{}'", pathFontAtlas);
      BasicTextureAtlas basicTextureAtlas;
      contentManager.Read(basicTextureAtlas, pathFontAtlas);

      FSLLOG3_INFO("  Basic font kerning '{}'", pathFontInfo);
      BasicFontKerning basicFontKerning;
      contentManager.Read(basicFontKerning, pathFontInfo);
      return {spriteNativeAreaCalc, textureExtentPx, basicTextureAtlas, basicFontKerning, densityDpi};
    }

    TextureAtlasSpriteFont ReadFont(const IContentManager& contentManager, const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                    const uint32_t densityDpi, const PxExtent2D textureExtentPx, const IO::Path& pathFontInfo)
    {
      FSLLOG3_INFO("  BitmapFont '{}'", pathFontInfo);
      auto bytes = contentManager.ReadAllBytes(pathFontInfo);

      return TextureAtlasSpriteFont(spriteNativeAreaCalc, textureExtentPx, BitmapFontDecoder::Decode(SpanUtil::AsReadOnlySpan(bytes)), densityDpi);
    }

    void ValidateFont(const TextureAtlasSpriteFont& lhs, const TextureAtlasSpriteFont& rhs)
    {
      if (lhs.BaseLinePx() != rhs.BaseLinePx())
      {
        throw std::runtime_error("font baseLine does not match");
      }
      if (lhs.LineSpacingPx() != rhs.LineSpacingPx())
      {
        throw std::runtime_error("font lineSpacing does not match");
      }
      for (uint32_t i = 32; i < 255; ++i)
      {
        const auto* pLhsChar = lhs.TryGetChar(i);
        const auto* pRhsChar = rhs.TryGetChar(i);
        if (pLhsChar == nullptr)
        {
          if (pRhsChar != nullptr)
          {
            throw std::runtime_error(fmt::format("font character id {0} does not match (lhs == nullptr && rhs != nullptr)", i));
          }
          // both are nullptr
        }
        else if (pRhsChar == nullptr)
        {
          throw std::runtime_error(fmt::format("font character id {0} does not match (lhs != nullptr && rhs == nullptr)", i));
        }
        else if (*pLhsChar != *pRhsChar)
        {
          assert(pLhsChar->CharInfo.Id == i);
          assert(pRhsChar->CharInfo.Id == i);
          std::string res;
          if (pLhsChar->CharInfo.OffsetPx != pRhsChar->CharInfo.OffsetPx)
          {
            res = fmt::format("OffsetPx {}!={}", pLhsChar->CharInfo.OffsetPx, pRhsChar->CharInfo.OffsetPx);
          }
          if (pLhsChar->CharInfo.SrcTextureRectPx != pRhsChar->CharInfo.SrcTextureRectPx)
          {
            if (!res.empty())
            {
              res += "|";
            }
            res = fmt::format("SrcTextureRectPx {}!={}", pLhsChar->CharInfo.SrcTextureRectPx, pRhsChar->CharInfo.SrcTextureRectPx);
          }
          if (pLhsChar->CharInfo.XAdvancePx != pRhsChar->CharInfo.XAdvancePx)
          {
            if (!res.empty())
            {
              res += "|";
            }
            res = fmt::format("XAdvancePx {}!={}", pLhsChar->CharInfo.XAdvancePx, pRhsChar->CharInfo.XAdvancePx);
          }
          throw std::runtime_error(fmt::format("font character id {} does not match ({})", i, res));
        }
      }
    }
  }


  DevBatch::DevBatch(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_bufferManager(std::make_shared<VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
  {
    const auto contentManagerEx = GetContentManager();
    const auto& contentManager = *contentManagerEx;
    SpriteNativeAreaCalc spriteNativeAreaCalc(false);
    const uint32_t densityDpi = 160;

    m_resources.Textures = CreateTextures(m_device, m_deviceQueue, GetContentManager());
    m_resources.VertexBuffer = CreateVertexBuffer(m_bufferManager);

    const PxExtent2D normalFontTexExtent = TypeConverter::To<PxExtent2D>(m_resources.Textures.NormalFontTexture.GetSize());
    const PxExtent2D sdfFontTexExtent = TypeConverter::To<PxExtent2D>(m_resources.Textures.SdfFontTexture.GetSize());

    m_resources.NormalFontOld =
      ReadFont(contentManager, spriteNativeAreaCalc, densityDpi, normalFontTexExtent, Local::PathFontAtlas, Local::PathFontInfoOld);
    m_resources.NormalFontNew = ReadFont(contentManager, spriteNativeAreaCalc, densityDpi, normalFontTexExtent, Local::PathFontInfoNew);

    ValidateFont(m_resources.NormalFontOld, m_resources.NormalFontNew);

    m_resources.SdfFontOld =
      ReadFont(contentManager, spriteNativeAreaCalc, densityDpi, sdfFontTexExtent, Local::PathSdfFontAtlas, Local::PathSdfFontInfoOld);
    m_resources.SdfFontNew = ReadFont(contentManager, spriteNativeAreaCalc, densityDpi, sdfFontTexExtent, Local::PathSdfFontInfoNew);

    ValidateFont(m_resources.SdfFontOld, m_resources.SdfFontNew);

    CreateShaders();
    m_resources.MainPipelineLayout = CreatePipelineLayout(m_device.Get());

    const auto vertexShaderBinary = contentManager.ReadBytes("QuadBatch/QuadBatch.vert.spv");
    const auto fragmentShaderBinary = contentManager.ReadBytes("QuadBatch/QuadBatch.frag.spv");
    const auto sdfFragmentShaderBinary = contentManager.ReadBytes("QuadBatch/QuadBatchSdf.frag.spv");

    m_test = std::make_shared<QuadBatch>(vertexShaderBinary, fragmentShaderBinary, sdfFragmentShaderBinary, 100);
    m_batch2DQuad = std::make_shared<QuadBatch>(vertexShaderBinary, fragmentShaderBinary, sdfFragmentShaderBinary, GenericBatch2DDefaultCapacity);
    m_batch2D = std::make_shared<Batch2D>(m_batch2DQuad, GetScreenExtent());

    m_test->CreateDeviceResources(m_physicalDevice, m_device.Get());
    m_batch2DQuad->CreateDeviceResources(m_physicalDevice, m_device.Get());
  }


  void DevBatch::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::Space:
      m_newFontFormat = !m_newFontFormat;
      break;
    case VirtualKey::K:
      m_kerningEnabled = !m_kerningEnabled;
      break;
    default:
      break;
    }
  }


  void DevBatch::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    VulkanBasic::DemoAppVulkanBasic::ConfigurationChanged(windowMetrics);
    if (m_batch2D)
    {
      m_batch2D->SetScreenExtent(windowMetrics.ExtentPx);
    }
  }


  void DevBatch::Update(const DemoTime& demoTime)
  {
    const auto resPx = GetWindowSizePx();

    m_currentPos2 = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (4 * Local::ForcedTexWidth);
    m_currentPos3 = ((-std::cos(m_currentAngle) + 1.0f) / 2.0f) * (4 * Local::ForcedTexWidth);

    m_currentPos.X = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (static_cast<float>(resPx.RawWidth()) - Local::ForcedTexWidth);
    m_currentPos.Y = ((std::sin(m_currentAngle) + 1.0f) / 2.0f) * (static_cast<float>(resPx.RawHeight()) - Local::ForcedTexHeight);

    m_currentPos4.X = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (400.0f - Local::ForcedTexWidth);
    m_currentPos4.Y = ((std::sin(m_currentAngle) + 1.0f) / 2.0f) * (400.0f - Local::ForcedTexHeight);

    m_currentAngle += m_currentSpeed * demoTime.DeltaTime;
    // Clamp the angle so we dont run into precession issues over time
    m_currentAngle = MathHelper::WrapAngle(m_currentAngle);
  }


  void DevBatch::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const auto currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers.Get(currentFrameIndex);
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {{0.4f, 0.4f, 0.4f, 1.0f}};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.Pipeline.Get());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(drawContext.SwapchainImageExtent.width);
        viewport.height = static_cast<float>(drawContext.SwapchainImageExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);

        VkRect2D scissor{{0, 0}, drawContext.SwapchainImageExtent};
        vkCmdSetScissor(hCmdBuffer, 0, 1, &scissor);

        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(hCmdBuffer, 0, 1, m_resources.VertexBuffer.GetBufferPointer(), &offsets);
        vkCmdDraw(hCmdBuffer, 3, 1, 0, 0);

        // Draw the quads
        DrawQuads(currentFrameIndex, hCmdBuffer);

        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }

  VkRenderPass DevBatch::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    const VkRenderPass mainRenderPass = m_dependentResources.MainRenderPass.Get();

    m_dependentResources.Pipeline =
      CreatePipeline(m_device.Get(), m_resources.VertexShaderModule.Get(), m_resources.FragmentShaderModule.Get(), m_resources.VertexBuffer,
                     context.SwapchainImageExtent, m_resources.MainPipelineLayout.Get(), mainRenderPass);

    const auto swapchainImagesCount = context.SwapchainImagesCount;

    auto screenExtent = GetScreenExtent();
    if (m_batch2DQuad)
    {
      m_batch2DQuad->CreateDependentResources(swapchainImagesCount, mainRenderPass, Local::MainSubPass, screenExtent);
    }
    if (m_test)
    {
      m_test->CreateDependentResources(swapchainImagesCount, mainRenderPass, Local::MainSubPass, screenExtent);
    }
    return mainRenderPass;
  }


  void DevBatch::OnFreeResources()
  {
    if (m_batch2DQuad)
    {
      m_batch2DQuad->DestroyDependentResources();
    }
    if (m_test)
    {
      m_test->DestroyDependentResources();
    }

    m_dependentResources.Reset();
  }


  DevBatch::AppTextures DevBatch::CreateTextures(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                 const std::shared_ptr<IContentManager>& contentManger)
  {
    auto texture = contentManger->ReadTexture("Texturing.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture2 = contentManger->ReadTexture("Icons/boardgamewizard.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture3 = contentManger->ReadBitmap("Icons/hero_bard.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture4 = contentManger->ReadBitmap("Knight/Idle1.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture4Pre = contentManger->ReadBitmap("Knight/Idle1_pre.png", PixelFormat::R8G8B8A8_UNORM);
    auto textureNormalFont = contentManger->ReadBitmap(Local::PathFontTexture, PixelFormat::R8G8B8A8_UNORM);
    ;
    auto textureSdfFont = contentManger->ReadBitmap(Local::PathSdfFontTexture, PixelFormat::R8G8B8A8_UNORM);

    AppTextures textures;
    {
      VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      bool useStaging = true;
      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      // Set max level-of- detail to mip level count of the texture
      samplerCreateInfo.maxLod = (useStaging) ? static_cast<float>(texture.GetLevels()) : 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
      samplerCreateInfo.maxAnisotropy = 1.0f;

      textures.Texture1 = imageCreator.CreateTexture(texture, samplerCreateInfo);
      textures.Texture2 = imageCreator.CreateTexture(texture2, samplerCreateInfo);
      textures.Texture3 = imageCreator.CreateTexture(texture3, samplerCreateInfo);
      textures.Texture4 = imageCreator.CreateTexture(texture4, samplerCreateInfo);
      textures.Texture4Pre = imageCreator.CreateTexture(texture4Pre, samplerCreateInfo);
      textures.NormalFontTexture = imageCreator.CreateTexture(textureNormalFont, samplerCreateInfo);
      textures.SdfFontTexture = imageCreator.CreateTexture(textureSdfFont, samplerCreateInfo);
    }
    return textures;
  }


  void DevBatch::CreateShaders()
  {
    const auto contentManager = GetContentManager();

    m_resources.VertexShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(Local::VertexShaderName));
    m_resources.FragmentShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(Local::FragmentShaderName));
  }


  void DevBatch::DrawQuads(const uint32_t swapBufferIndex, const VkCommandBuffer commandBuffer)
  {
    auto& rTest = m_test;
    auto& rBatch2DQuad = m_batch2DQuad;
    auto& rBatch2D = m_batch2D;

    const auto windowSizePx = GetWindowSizePx();
    const auto& texture1 = m_resources.Textures.Texture1;
    const auto& texture2 = m_resources.Textures.Texture2;
    const auto& texture3 = m_resources.Textures.Texture3;
    const auto& texture4 = m_resources.Textures.Texture4;
    const auto& texture4Pre = m_resources.Textures.Texture4Pre;
    const auto& textureNormalFont = m_resources.Textures.NormalFontTexture;
    const auto& textureSdfFont = m_resources.Textures.SdfFontTexture;
    {
      auto col = Colors::White();

      std::array<VertexPositionColorTexture, 8> testQuad{};
      testQuad[0].Color = col;
      testQuad[1].Color = col;
      testQuad[2].Color = col;
      testQuad[3].Color = col;
      testQuad[4].Color = col;
      testQuad[5].Color = col;
      testQuad[6].Color = col;
      testQuad[7].Color = col;

      const float v0 = 0.0f;
      const float v1 = 1.0f;

      testQuad[0].TextureCoordinate = Vector2(0.0f, v0);
      testQuad[1].TextureCoordinate = Vector2(1.0f, v0);
      testQuad[2].TextureCoordinate = Vector2(0.0f, v1);
      testQuad[3].TextureCoordinate = Vector2(1.0f, v1);
      testQuad[4].TextureCoordinate = Vector2(0.0f, v0);
      testQuad[5].TextureCoordinate = Vector2(1.0f, v0);
      testQuad[6].TextureCoordinate = Vector2(0.0f, v1);
      testQuad[7].TextureCoordinate = Vector2(1.0f, v1);

      float z = 0;
      float x = 200;
      float y = 200;

      testQuad[0].Position = Vector3(0, 0, z);
      testQuad[1].Position = Vector3(100, 0, z);
      testQuad[2].Position = Vector3(0, 100, z);
      testQuad[3].Position = Vector3(100, 100, z);

      rTest->BeginFrame(commandBuffer, swapBufferIndex);
      rTest->Begin(windowSizePx, BlendState::Opaque, BatchSdfRenderConfig(), true);
      rTest->DrawQuads(testQuad.data(), 1, texture1);

      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      rTest->DrawQuads(testQuad.data(), 1, texture1);

      x = 100;
      y = 100;
      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      rTest->DrawQuads(testQuad.data(), 1, texture2);

      x = 0;
      y = 200;
      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      rTest->DrawQuads(testQuad.data(), 1, texture3);

      x = 400;
      y = 400;
      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      x = 200;
      y = 400;
      testQuad[4].Position = Vector3(x + 0, y + 0, z);
      testQuad[5].Position = Vector3(x + 100, y + 0, z);
      testQuad[6].Position = Vector3(x + 0, y + 100, z);
      testQuad[7].Position = Vector3(x + 100, y + 100, z);
      rTest->DrawQuads(testQuad.data(), 2, texture3);


      rTest->End();
      rTest->EndFrame();
    }

    const auto colMovingPre = Color(0.7f, 0.7f, 0.7f, 0.7f);

    const auto col1 = Colors::Red();
    const auto col2 = Colors::Green();
    const auto col3 = Colors::Blue();
    const auto col4 = Colors::Olive();
    const auto col5 = Colors::White();

    const auto heightPx = windowSizePx.RawHeight();

    const auto& normalFont = !m_newFontFormat ? m_resources.NormalFontOld : m_resources.NormalFontNew;
    const auto& sdfFont = !m_newFontFormat ? m_resources.SdfFontOld : m_resources.SdfFontNew;

    const StringViewLite strInfo(!m_newFontFormat ? (m_kerningEnabled ? "fbk, kerning: on" : "fbk, kerning: off")
                                                  : (m_kerningEnabled ? "nbf, kerning: on" : "nbf, kerning: off"));

    rBatch2DQuad->BeginFrame(commandBuffer, swapBufferIndex);

    Vector2 textOffsetPxf(400, 0);

    const auto sdfFontLineSpacingPxf = static_cast<float>(sdfFont.LineSpacingPx().RawValue());

    BitmapFontConfig fontConfig(1.0f, m_kerningEnabled);
    BitmapFontConfig fontConfigZoom(2.1f, m_kerningEnabled);
    rBatch2D->Begin(BlendState::NonPremultiplied);

    auto sizeInfo = normalFont.MeasureString(strInfo);
    rBatch2D->DrawString(textureNormalFont, normalFont, fontConfig, strInfo, Vector2(windowSizePx.RawWidth() - sizeInfo.RawWidth(), 0),
                         Colors::White());

    rBatch2D->DrawString(textureNormalFont, normalFont, fontConfig, "Hello World. P. (Normal) NonPremultipliedShader)", textOffsetPxf,
                         Colors::White());
    textOffsetPxf.Y += static_cast<float>(normalFont.LineSpacingPx().RawValue());
    rBatch2D->DrawString(textureNormalFont, normalFont, fontConfigZoom, "Hello World. P. (Normal 2.1x) NonPremultipliedShader)", textOffsetPxf,
                         Colors::White());
    textOffsetPxf.Y += static_cast<float>(normalFont.LineSpacingPx(fontConfigZoom).Value);

    rBatch2D->ChangeTo(BlendState::Sdf);
    rBatch2D->DrawString(textureSdfFont, sdfFont, fontConfig, "Hello World. P. (SDF) SdfShader", textOffsetPxf, Colors::White());
    textOffsetPxf.Y += sdfFontLineSpacingPxf;
    rBatch2D->DrawString(textureSdfFont, sdfFont, fontConfigZoom, "Hello World. P. (SDF 2.1x) SdfShader", textOffsetPxf, Colors::White());
    textOffsetPxf.Y += static_cast<float>(sdfFont.LineSpacingPx(fontConfigZoom).Value);
    rBatch2D->ChangeTo(BlendState::NonPremultiplied);
    rBatch2D->DrawString(textureSdfFont, sdfFont, fontConfig, "Hello World. P. (SDF) NonPremultipliedShader)", textOffsetPxf, Colors::White());
    textOffsetPxf.Y += sdfFontLineSpacingPxf;
    rBatch2D->End();

    const auto heightPxf = static_cast<float>(heightPx);
    rBatch2D->Begin(BlendState::NonPremultiplied);
    // Bottom left
    rBatch2D->Draw(
      texture4,
      PxAreaRectangleF::Create(0 * Local::ForcedTexWidth, heightPxf - Local::ForcedTexHeight, Local::ForcedTexWidth, Local::ForcedTexHeight), col1);
    rBatch2D->Draw(
      texture4,
      PxAreaRectangleF::Create(1 * Local::ForcedTexWidth, heightPxf - Local::ForcedTexHeight, Local::ForcedTexWidth, Local::ForcedTexHeight), col2);
    rBatch2D->Draw(
      texture4,
      PxAreaRectangleF::Create(2 * Local::ForcedTexWidth, heightPxf - Local::ForcedTexHeight, Local::ForcedTexWidth, Local::ForcedTexHeight), col3);
    rBatch2D->Draw(
      texture4,
      PxAreaRectangleF::Create(3 * Local::ForcedTexWidth, heightPxf - Local::ForcedTexHeight, Local::ForcedTexWidth, Local::ForcedTexHeight), col4);
    rBatch2D->Draw(
      texture4,
      PxAreaRectangleF::Create(4 * Local::ForcedTexWidth, heightPxf - Local::ForcedTexHeight, Local::ForcedTexWidth, Local::ForcedTexHeight), col5);
    rBatch2D->Draw(texture4Pre,
                   PxAreaRectangleF::Create(m_currentPos2, heightPxf - Local::ForcedTexHeight, Local::ForcedTexWidth, Local::ForcedTexHeight),
                   colMovingPre);
    rBatch2D->End();

    rBatch2D->Begin(BlendState::AlphaBlend);
    // Bottom left
    rBatch2D->Draw(
      texture4Pre,
      PxAreaRectangleF::Create(0 * Local::ForcedTexWidth, heightPxf - (2 * Local::ForcedTexHeight), Local::ForcedTexWidth, Local::ForcedTexHeight),
      col1);
    rBatch2D->Draw(
      texture4Pre,
      PxAreaRectangleF::Create(1 * Local::ForcedTexWidth, heightPxf - (2 * Local::ForcedTexHeight), Local::ForcedTexWidth, Local::ForcedTexHeight),
      col2);
    rBatch2D->Draw(
      texture4Pre,
      PxAreaRectangleF::Create(2 * Local::ForcedTexWidth, heightPxf - (2 * Local::ForcedTexHeight), Local::ForcedTexWidth, Local::ForcedTexHeight),
      col3);
    rBatch2D->Draw(
      texture4Pre,
      PxAreaRectangleF::Create(3 * Local::ForcedTexWidth, heightPxf - (2 * Local::ForcedTexHeight), Local::ForcedTexWidth, Local::ForcedTexHeight),
      col4);
    rBatch2D->Draw(
      texture4Pre,
      PxAreaRectangleF::Create(4 * Local::ForcedTexWidth, heightPxf - (2 * Local::ForcedTexHeight), Local::ForcedTexWidth, Local::ForcedTexHeight),
      col5);

    rBatch2D->Draw(texture4Pre, PxAreaRectangleF::Create(m_currentPos.X, m_currentPos.Y, Local::ForcedTexWidth, Local::ForcedTexHeight),
                   colMovingPre);
    rBatch2D->Draw(texture4Pre,
                   PxAreaRectangleF::Create(m_currentPos2, heightPxf - (2 * Local::ForcedTexHeight), Local::ForcedTexWidth, Local::ForcedTexHeight),
                   colMovingPre);
    rBatch2D->Draw(texture4Pre, PxAreaRectangleF::Create(m_currentPos4.X, m_currentPos4.Y, Local::ForcedTexWidth, Local::ForcedTexHeight),
                   Colors::White());
    rBatch2D->Draw(texture4Pre, PxAreaRectangleF::Create(static_cast<float>(windowSizePx.RawWidth()) - 200.0f, 50.0f, 200.0f, 200.0f),
                   Colors::White());
    // rBatch2D->Draw(texture4, m_currentPos, Color(1.0f, 1.0f, 1.0f, 0.70f));
    rBatch2D->End();

    rBatch2D->Begin(BlendState::Additive);
    rBatch2D->Draw(texture4Pre,
                   PxAreaRectangleF::Create(m_currentPos3, static_cast<float>(heightPx) - (1.5f * Local::ForcedTexHeight), Local::ForcedTexWidth,
                                            Local::ForcedTexHeight),
                   colMovingPre);
    rBatch2D->End();


    rBatch2DQuad->EndFrame();
  }
}
