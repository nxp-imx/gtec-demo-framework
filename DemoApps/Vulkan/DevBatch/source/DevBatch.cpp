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

#include "DevBatch.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Memory.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <thread>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    const auto VERTEX_SHADER_NAME = "vertex_only_ndc.vert.spv";
    const auto FRAGMENT_SHADER_NAME = "red.frag.spv";

    const auto MAIN_SUBPASS = 0;

    const float FORCED_TEX_WIDTH = 200.0f;
    const float FORCED_TEX_HEIGHT = 100.0f;

    Vulkan::VMVertexBuffer CreateVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager)
    {
      std::array<VertexPosition, 3> vertices = {VertexPosition(-0.5f, 0.5f, 0.0f), VertexPosition(0.5f, 0.5f, 0.0f),
                                                VertexPosition(0.0f, -0.5f, 0.0f)};
      return VMVertexBuffer(bufferManager, vertices, Vulkan::VMBufferUsage::STATIC);
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const VkDevice device)
    {
      return RapidVulkan::PipelineLayout(device, 0, 0, nullptr, 0, nullptr);
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
      pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicState.size());
      pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState.data();

      VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
      graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      graphicsPipelineCreateInfo.flags = 0;
      graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfo.size());
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

      return RapidVulkan::GraphicsPipeline(device, VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    }
  }


  DevBatch::DevBatch(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_bufferManager(std::make_shared<VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
  {
    m_resources.Textures = CreateTextures(m_device, m_deviceQueue, GetContentManager());
    m_resources.VertexBuffer = CreateVertexBuffer(m_bufferManager);
    CreateShaders();
    m_resources.MainPipelineLayout = CreatePipelineLayout(m_device.Get());

    const auto vertexShaderBinary = GetContentManager()->ReadBytes("QuadBatch/QuadBatch.vert.spv");
    const auto fragmentShaderBinary = GetContentManager()->ReadBytes("QuadBatch/QuadBatch.frag.spv");

    m_test.reset(new QuadBatch(vertexShaderBinary, fragmentShaderBinary, 100));
    m_batch2DQuad.reset(new QuadBatch(vertexShaderBinary, fragmentShaderBinary, GenericBatch2D_DEFAULT_CAPACITY));
    m_batch2D.reset(new Batch2D(m_batch2DQuad, GetScreenResolution()));

    m_test->CreateDeviceResources(m_physicalDevice, m_device.Get());
    m_batch2DQuad->CreateDeviceResources(m_physicalDevice, m_device.Get());
  }


  void DevBatch::Resized(const Point2& size)
  {
    VulkanBasic::DemoAppVulkanBasic::Resized(size);
    if (m_batch2D)
    {
      m_batch2D->SetScreenResolution(size);
    }
  }


  void DevBatch::Update(const DemoTime& demoTime)
  {
    auto res = GetScreenResolution();

    m_currentPos2 = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (4 * FORCED_TEX_WIDTH);
    m_currentPos3 = ((-std::cos(m_currentAngle) + 1.0f) / 2.0f) * (4 * FORCED_TEX_WIDTH);

    m_currentPos.X = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (res.X - FORCED_TEX_WIDTH);
    m_currentPos.Y = ((std::sin(m_currentAngle) + 1.0f) / 2.0f) * (res.Y - FORCED_TEX_HEIGHT);

    m_currentPos4.X = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (400.0f - FORCED_TEX_WIDTH);
    m_currentPos4.Y = ((std::sin(m_currentAngle) + 1.0f) / 2.0f) * (400.0f - FORCED_TEX_HEIGHT);

    m_currentAngle += m_currentSpeed * demoTime.DeltaTime;
    // Clamp the angle so we dont run into precession issues over time
    m_currentAngle = MathHelper::WrapAngle(m_currentAngle);
  }


  void DevBatch::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const auto currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;

    auto hCmdBuffer = rCmdBuffers.Get(currentSwapBufferIndex);
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {0.4f, 0.4f, 0.4f, 1.0f};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
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

        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(hCmdBuffer, 0, 1, m_resources.VertexBuffer.GetBufferPointer(), offsets);
        vkCmdDraw(hCmdBuffer, 3, 1, 0, 0);

        // Draw the quads
        DrawQuads(currentSwapBufferIndex, hCmdBuffer);

        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }

  VkRenderPass DevBatch::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    const auto mainRenderPass = m_dependentResources.MainRenderPass.Get();

    m_dependentResources.Pipeline =
      CreatePipeline(m_device.Get(), m_resources.VertexShaderModule.Get(), m_resources.FragmentShaderModule.Get(), m_resources.VertexBuffer,
                     context.SwapchainImageExtent, m_resources.MainPipelineLayout.Get(), mainRenderPass);

    const auto swapchainImagesCount = context.SwapchainImagesCount;

    auto screenExtent = GetScreenExtent();
    if (m_batch2DQuad)
    {
      m_batch2DQuad->CreateDependentResources(swapchainImagesCount, mainRenderPass, MAIN_SUBPASS, screenExtent);
    }
    if (m_test)
    {
      m_test->CreateDependentResources(swapchainImagesCount, mainRenderPass, MAIN_SUBPASS, screenExtent);
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

    m_dependentResources = {};
  }


  DevBatch::AppTextures DevBatch::CreateTextures(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                 const std::shared_ptr<IContentManager>& contentManger)
  {
    auto texture = contentManger->ReadTexture("Texturing.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture2 = contentManger->ReadTexture("Icons/boardgamewizard.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture3 = contentManger->ReadBitmap("Icons/hero_bard.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture4 = contentManger->ReadBitmap("Knight/Idle1.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture4Pre = contentManger->ReadBitmap("Knight/Idle1_pre.png", PixelFormat::R8G8B8A8_UNORM);

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
    }
    return textures;
  }


  void DevBatch::CreateShaders()
  {
    const auto contentManager = GetContentManager();

    m_resources.VertexShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(VERTEX_SHADER_NAME));
    m_resources.FragmentShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(FRAGMENT_SHADER_NAME));
  }


  void DevBatch::DrawQuads(const uint32_t swapBufferIndex, const VkCommandBuffer commandBuffer)
  {
    auto& rTest = m_test;
    auto& rBatch2DQuad = m_batch2DQuad;
    auto& rBatch2D = m_batch2D;

    const auto screenRes = GetScreenResolution();
    const auto& texture1 = m_resources.Textures.Texture1;
    const auto& texture2 = m_resources.Textures.Texture2;
    const auto& texture3 = m_resources.Textures.Texture3;
    const auto& texture4 = m_resources.Textures.Texture4;
    const auto& texture4Pre = m_resources.Textures.Texture4Pre;
    {
      auto col = Color::White().ToVector4();

      VertexPositionColorTexture testQuad[8]{};
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
      rTest->Begin(screenRes, BlendState::Opaque, true);
      rTest->DrawQuads(testQuad, 1, texture1);

      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      rTest->DrawQuads(testQuad, 1, texture1);

      x = 100;
      y = 100;
      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      rTest->DrawQuads(testQuad, 1, texture2);

      x = 0;
      y = 200;
      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      rTest->DrawQuads(testQuad, 1, texture3);

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
      rTest->DrawQuads(testQuad, 2, texture3);

      rTest->End();
      rTest->EndFrame();
    }

    const auto colMovingPre = Color(0.7f, 0.7f, 0.7f, 0.7f);

    const auto col1 = Color::Red();
    const auto col2 = Color::Green();
    const auto col3 = Color::Blue();
    const auto col4 = Color::Olive();
    const auto col5 = Color::White();

    rBatch2DQuad->BeginFrame(commandBuffer, swapBufferIndex);

    rBatch2D->Begin(BlendState::NonPremultiplied);
    // Bottom left
    rBatch2D->Draw(texture4, Rect(0 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col1);
    rBatch2D->Draw(texture4, Rect(1 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col2);
    rBatch2D->Draw(texture4, Rect(2 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col3);
    rBatch2D->Draw(texture4, Rect(3 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col4);
    rBatch2D->Draw(texture4, Rect(4 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col5);
    rBatch2D->Draw(texture4Pre, Rect(m_currentPos2, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), colMovingPre);
    rBatch2D->End();

    rBatch2D->Begin(BlendState::AlphaBlend);
    // Bottom left
    rBatch2D->Draw(texture4Pre, Rect(0 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col1);
    rBatch2D->Draw(texture4Pre, Rect(1 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col2);
    rBatch2D->Draw(texture4Pre, Rect(2 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col3);
    rBatch2D->Draw(texture4Pre, Rect(3 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col4);
    rBatch2D->Draw(texture4Pre, Rect(4 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col5);

    rBatch2D->Draw(texture4Pre, Rect(m_currentPos.X, m_currentPos.Y, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), colMovingPre);
    rBatch2D->Draw(texture4Pre, Rect(m_currentPos2, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), colMovingPre);
    rBatch2D->Draw(texture4Pre, Rect(m_currentPos4.X, m_currentPos4.Y, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), Color::White());
    rBatch2D->Draw(texture4Pre, Rect(screenRes.X - 200.0f, 50.0f, 200.0f, 200.0f), Color::White());
    // rBatch2D->Draw(texture4, m_currentPos, Color(1.0f, 1.0f, 1.0f, 0.70f));
    rBatch2D->End();

    rBatch2D->Begin(BlendState::Additive);
    rBatch2D->Draw(texture4Pre, Rect(m_currentPos3, screenRes.Y - (1.5f * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), colMovingPre);
    rBatch2D->End();

    rBatch2DQuad->EndFrame();
  }
}
