/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include "SdfFonts.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <Shared/SdfFonts/AppHelper.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const uint32_t VertexBufferBindId = 0;

      constexpr const float DefaultZPos = 0.0f;

      constexpr const IO::PathView NormalFontAtlasTexturePath("BasicUI/Font/NormalAtlas/FontAtlas.png");
      constexpr const IO::PathView NormalFontPath("BasicUI/Font/NormalAtlas/FontAtlas_Font.nbf");

      constexpr const IO::PathView SdfFontAtlasTexturePath("BasicUI/Font/SdfAtlas/FontAtlas.png");
      constexpr const IO::PathView SdfFontPath("BasicUI/Font/SdfAtlas/FontAtlas_Font.nbf");

      // constexpr const IO::PathView FillTexturePath("Fill/Fill.png");

      constexpr const IO::PathView TextVertShader("Text.vert.spv");
      constexpr const IO::PathView TextFragShader("Text.frag.spv");
      constexpr const IO::PathView TextSdfFragShader("Text-sdf.frag.spv");
      constexpr const IO::PathView TextSdfOutlineFragShader("Text-sdfOutline.frag.spv");
      constexpr const IO::PathView TextSdfShadowFragShader("Text-sdfDropShadow.frag.spv");
      constexpr const IO::PathView TextSdfShadowAndOutlineFragShader("Text-sdfDropShadowAndOutline.frag.spv");

      constexpr const StringViewLite TextLine0("The quick brown fox jumps over the lazy dog! Hello World.");
      // constexpr StringViewLite TextLine1("abcdefghijklmnopqrstuvwxyz");
      // constexpr StringViewLite TextLine2("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      // constexpr StringViewLite TextLine3("0123456789!\".:,;(){}");

      static_assert(sizeof(Matrix) == ((4 * 4) * 4), "We expect a matrix to be four by four floats of size 4");
    }

    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Bitmap& bitmap,
                                    const VkFilter filter, const VkSamplerAddressMode addressMode)
    {
      Vulkan::VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = filter;
      samplerCreateInfo.minFilter = filter;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = addressMode;
      samplerCreateInfo.addressModeV = addressMode;
      samplerCreateInfo.addressModeW = addressMode;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(bitmap, samplerCreateInfo);
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings{};
      // Binding 0 : Fragment shader image sampler
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return RapidVulkan::DescriptorSetLayout(device.Get(), descriptorLayout);
    }

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 1> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[0].descriptorCount = count;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return RapidVulkan::DescriptorPool(device.Get(), descriptorPoolInfo);
    }

    VkDescriptorSet CreateDescriptorSet(const RapidVulkan::DescriptorPool& descriptorPool,
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout)
    {
      assert(descriptorPool.IsValid());
      assert(descriptorSetLayout.IsValid());

      // Allocate a new descriptor set from the global descriptor pool
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool.Get();
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
      RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets", __FILE__,
                              __LINE__);

      return descriptorSet;
    }

    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUTexture& texture)
    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};

      // Binding 0 : Fragment shader texture sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[0].pImageInfo = &textureImageInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
      return descriptorSet;
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout, const uint32_t cbPushConstants)
    {
      VkPushConstantRange pushConstantRange{};
      pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
      pushConstantRange.offset = 0;
      pushConstantRange.size = cbPushConstants;

      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();
      pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
      pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

      return RapidVulkan::PipelineLayout(descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo);
    }

    RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                 VkBlendFactor srcBlendFactor, const VkShaderModule vertexShaderModule,
                                                 const VkShaderModule fragmentShaderModule, const MeshDescription& meshDesc,
                                                 const VkRenderPass renderPass, const uint32_t subpass)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);
      assert(!meshDesc.VertexInputBindingDescription.empty());
      assert(!meshDesc.VertexAttributeDescription.empty());

      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};
      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
      pipelineShaderStageCreateInfo[1].pName = "main";
      pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;


      VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
      pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = UncheckedNumericCast<uint32_t>(meshDesc.VertexInputBindingDescription.size());
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = meshDesc.VertexInputBindingDescription.data();
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(meshDesc.VertexAttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = meshDesc.VertexAttributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.width = static_cast<float>(extent.width);
      viewport.height = static_cast<float>(extent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{{0, 0}, extent};

      VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
      pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      pipelineViewportStateCreateInfo.viewportCount = 1;
      pipelineViewportStateCreateInfo.pViewports = &viewport;
      pipelineViewportStateCreateInfo.scissorCount = 1;
      pipelineViewportStateCreateInfo.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
      pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
      pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
      pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
      pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
      pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
      pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

      VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
      pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
      pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
      pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
      pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
      pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

      VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
      pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
      pipelineColorBlendAttachmentState.srcColorBlendFactor = srcBlendFactor;
      pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
      pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
      pipelineColorBlendAttachmentState.colorWriteMask = 0xf;

      VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
      pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
      pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
      pipelineColorBlendStateCreateInfo.attachmentCount = 1;
      pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
      pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

      VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
      depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
      depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
      depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
      depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.front = {};
      depthStencilStateCreateInfo.back = {};
      depthStencilStateCreateInfo.minDepthBounds = 0.0f;
      depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

      VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
      graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      graphicsPipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(pipelineShaderStageCreateInfo.size());
      graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
      graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
      graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
      graphicsPipelineCreateInfo.pTessellationState = nullptr;
      graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
      graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
      graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
      graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
      graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
      graphicsPipelineCreateInfo.pDynamicState = nullptr;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = subpass;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return RapidVulkan::GraphicsPipeline(pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    }

    Vulkan::VUTexture ReadTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                  const IContentManager& contentManager, const IO::Path& path)
    {
      auto bitmap = contentManager.ReadBitmap(path, PixelFormat::R8G8B8A8_UNORM);
      return CreateTexture(device, deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    }
  }


  SdfFonts::SdfFonts(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_shared(config)
    , m_nativeBatch(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D())
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_shared.GetUIDemoAppExtension());

    m_resources.BufferManager =
      std::make_shared<Vulkan::VMBufferManager>(m_device.GetPhysicalDevice(), m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

    auto contentManager = GetContentManager();

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;

    const int32_t line0YPx = 0;
    const uint32_t densityDpi = config.WindowMetrics.DensityDpi;
    const SpriteNativeAreaCalc& spriteNativeAreaCalc = m_shared.GetUIDemoAppExtension()->GetSpriteNativeAreaCalc();
    m_resources.Normal =
      PrepareExample(maxFramesInFlight, m_resources.BufferManager, *contentManager, line0YPx, LocalConfig::NormalFontPath,
                     LocalConfig::NormalFontAtlasTexturePath, LocalConfig::TextLine0, spriteNativeAreaCalc, densityDpi, m_positionsScratchpad);
    const int32_t line1YPx = m_resources.Normal.Font.Font.LineSpacingPx();
    m_resources.Sdf =
      PrepareExample(maxFramesInFlight, m_resources.BufferManager, *contentManager, line1YPx, LocalConfig::SdfFontPath,
                     LocalConfig::SdfFontAtlasTexturePath, LocalConfig::TextLine0, spriteNativeAreaCalc, densityDpi, m_positionsScratchpad);

    m_resources.VertShader.Reset(m_device.Get(), 0, contentManager->ReadBytes(LocalConfig::TextVertShader));
    m_resources.FragShaderText.Reset(m_device.Get(), 0, contentManager->ReadBytes(LocalConfig::TextFragShader));
    m_resources.FragShaderSdf.Reset(m_device.Get(), 0, contentManager->ReadBytes(LocalConfig::TextSdfFragShader));
    m_resources.FragShaderSdfOutline.Reset(m_device.Get(), 0, contentManager->ReadBytes(LocalConfig::TextSdfOutlineFragShader));
    m_resources.FragShaderSdfShadow.Reset(m_device.Get(), 0, contentManager->ReadBytes(LocalConfig::TextSdfShadowFragShader));
    m_resources.FragShaderSdfShadowAndOutline.Reset(m_device.Get(), 0, contentManager->ReadBytes(LocalConfig::TextSdfShadowAndOutlineFragShader));
    m_resources.VertShader.Reset(m_device.Get(), 0, contentManager->ReadBytes(LocalConfig::TextVertShader));

    m_resources.FillTexture = m_shared.GetFillTexture();

    FSLLOG3_INFO("Preparing content");

    {    // Vulkan setup

      m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);
      const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
      m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight);

      m_resources.DescriptorSetNormal = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
      m_resources.DescriptorSetSdf = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
      UpdateDescriptorSet(m_device.Get(), m_resources.DescriptorSetNormal, m_resources.Normal.Font.Texture);
      UpdateDescriptorSet(m_device.Get(), m_resources.DescriptorSetSdf, m_resources.Sdf.Font.Texture);

      m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout, sizeof(PushConstantRecord));
    }

    FSLLOG3_INFO("Ready");
  }


  SdfFonts::~SdfFonts() = default;

  void SdfFonts::OnKeyEvent(const KeyEvent& event)
  {
    VulkanBasic::DemoAppVulkanBasic::OnKeyEvent(event);

    m_shared.OnKeyEvent(event);
  }


  void SdfFonts::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    VulkanBasic::DemoAppVulkanBasic::ConfigurationChanged(windowMetrics);

    m_shared.OnConfigurationChanged(windowMetrics);
  }


  void SdfFonts::Update(const DemoTime& demoTime)
  {
    // Setup the shader
    const auto screenWidth = static_cast<float>(GetWindowSizePx().Width());
    const auto screenHeight = static_cast<float>(GetWindowSizePx().Height());
    const float screenOffsetX = screenWidth / 2.0f;
    const float screenOffsetY = screenHeight / 2.0f;

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    // const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    m_pushConstants.ModelViewProj =
      Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, -2.0f) * Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);

    m_shared.Update(demoTime);
  }


  void SdfFonts::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};

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
        DrawNow(hCmdBuffer, currentFrameIndex);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }

  VkRenderPass SdfFonts::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();


    const auto premultipliedBlendFactor = VK_BLEND_FACTOR_ONE;
    m_dependentResources.PipelineNormal = CreatePipeline(
      m_resources.MainPipelineLayout, context.SwapchainImageExtent, premultipliedBlendFactor, m_resources.VertShader.Get(),
      m_resources.FragShaderText.Get(), m_resources.Normal.Mesh.Mesh.GetMeshDescription(), m_dependentResources.MainRenderPass.Get(), 0);

    const MeshDescription sdfMeshDescription(m_resources.Sdf.Mesh.Mesh.GetMeshDescription());

    const auto sdfBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    m_dependentResources.PipelineSdf =
      CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, sdfBlendFactor, m_resources.VertShader.Get(),
                     m_resources.FragShaderSdf.Get(), sdfMeshDescription, m_dependentResources.MainRenderPass.Get(), 0);

    m_dependentResources.PipelineSdfOutline =
      CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, sdfBlendFactor, m_resources.VertShader.Get(),
                     m_resources.FragShaderSdfOutline.Get(), sdfMeshDescription, m_dependentResources.MainRenderPass.Get(), 0);

    m_dependentResources.PipelineSdfShadow =
      CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, sdfBlendFactor, m_resources.VertShader.Get(),
                     m_resources.FragShaderSdfShadow.Get(), sdfMeshDescription, m_dependentResources.MainRenderPass.Get(), 0);

    m_dependentResources.PipelineSdfShadowAndOutline =
      CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, sdfBlendFactor, m_resources.VertShader.Get(),
                     m_resources.FragShaderSdfShadowAndOutline.Get(), sdfMeshDescription, m_dependentResources.MainRenderPass.Get(), 0);

    return m_dependentResources.MainRenderPass.Get();
  }


  void SdfFonts::OnFreeResources()
  {
    m_dependentResources = {};
  }


  void SdfFonts::DrawNow(const VkCommandBuffer hCmdBuffer, const uint32_t currentFrameIndex)
  {
    const PxSize2D currentSizePx = GetWindowSizePx();

    auto contentOffset = m_shared.GetContentOffset();
    const auto fontDrawConfig = m_shared.GetFontDrawConfig();
    const auto fontSdfMode = m_shared.GetSdfMode();
    const auto& fontSdfPipeline = SelectPipeline(fontSdfMode);

    const PxPoint2 line0Px(contentOffset.X, contentOffset.Y + 0);
    const PxPoint2 line1Px(contentOffset.X, line0Px.Y + m_resources.Normal.Font.Font.LineSpacingPx());
    const PxPoint2 line2Px(contentOffset.X, line1Px.Y + m_resources.Sdf.Font.Font.LineSpacingPx());
    const PxPoint2 line3Px(contentOffset.X, line2Px.Y + int32_t(std::round(m_resources.Normal.Font.Font.LineSpacingPx() * fontDrawConfig.FontScale)));

    const bool enableKerning = m_shared.GetKerningEnabled();
    const BitmapFontConfig fontConfigNormal(1.0f, enableKerning);
    const BitmapFontConfig fontConfigScaled(fontDrawConfig.FontScale, enableKerning);

    RegenerateMeshOnDemand(m_resources.Normal.Mesh, currentFrameIndex, line0Px, m_resources.Normal.Font, fontConfigNormal, LocalConfig::TextLine0,
                           m_positionsScratchpad);
    RegenerateMeshOnDemand(m_resources.Sdf.Mesh, currentFrameIndex, line1Px, m_resources.Sdf.Font, fontConfigNormal, LocalConfig::TextLine0,
                           m_positionsScratchpad);
    RegenerateMeshOnDemand(m_resources.Normal.ScaledMesh, currentFrameIndex, line2Px, m_resources.Normal.Font, fontConfigScaled,
                           LocalConfig::TextLine0, m_positionsScratchpad);
    RegenerateMeshOnDemand(m_resources.Sdf.ScaledMesh, currentFrameIndex, line3Px, m_resources.Sdf.Font, fontConfigScaled, LocalConfig::TextLine0,
                           m_positionsScratchpad);

    const auto baseLine0Px = line0Px + PxPoint2(0, m_resources.Normal.Font.Font.BaseLinePx());
    const auto baseLine1Px = line1Px + PxPoint2(0, m_resources.Sdf.Font.Font.BaseLinePx());
    const auto baseLine2Px = line2Px + PxPoint2(0, int32_t(std::round(m_resources.Normal.Font.Font.BaseLinePx() * fontDrawConfig.FontScale)));
    const auto baseLine3Px = line3Px + PxPoint2(0, int32_t(std::round(m_resources.Sdf.Font.Font.BaseLinePx() * fontDrawConfig.FontScale)));

    // Draw baselines
    {
      constexpr auto baseLineColor = Color(0xFF404040);

      m_nativeBatch->Begin();
      m_nativeBatch->DebugDrawLine(m_resources.FillTexture, Vector2(baseLine0Px.X, baseLine0Px.Y),
                                   Vector2(baseLine0Px.X + currentSizePx.Width(), baseLine0Px.Y), baseLineColor);
      m_nativeBatch->DebugDrawLine(m_resources.FillTexture, Vector2(baseLine1Px.X, baseLine1Px.Y),
                                   Vector2(baseLine1Px.X + currentSizePx.Width(), baseLine1Px.Y), baseLineColor);
      m_nativeBatch->DebugDrawLine(m_resources.FillTexture, Vector2(baseLine2Px.X, baseLine2Px.Y),
                                   Vector2(baseLine2Px.X + currentSizePx.Width(), baseLine2Px.Y), baseLineColor);
      m_nativeBatch->DebugDrawLine(m_resources.FillTexture, Vector2(baseLine3Px.X, baseLine3Px.Y),
                                   Vector2(baseLine3Px.X + currentSizePx.Width(), baseLine3Px.Y), baseLineColor);

      m_nativeBatch->End();
    }

    DrawMeshes(hCmdBuffer, currentFrameIndex, fontDrawConfig, fontSdfPipeline);

    // Draw bounding boxes
    if (m_shared.GetBoundingBoxesEnabled())
    {
      m_nativeBatch->Begin(BlendState::Opaque);
      m_shared.DrawBoundingBoxes(*m_nativeBatch, line0Px, LocalConfig::TextLine0, m_resources.Normal.Font.Font, fontConfigNormal,
                                 m_positionsScratchpad);
      m_shared.DrawBoundingBoxes(*m_nativeBatch, line1Px, LocalConfig::TextLine0, m_resources.Sdf.Font.Font, fontConfigNormal, m_positionsScratchpad);
      m_shared.DrawBoundingBoxes(*m_nativeBatch, line2Px, LocalConfig::TextLine0, m_resources.Normal.Font.Font, fontConfigScaled,
                                 m_positionsScratchpad);
      m_shared.DrawBoundingBoxes(*m_nativeBatch, line3Px, LocalConfig::TextLine0, m_resources.Sdf.Font.Font, fontConfigScaled, m_positionsScratchpad);
      m_nativeBatch->End();
    }

    m_shared.Draw();
  }

  void SdfFonts::DrawMeshes(const VkCommandBuffer hCmdBuffer, const uint32_t currentFrameIndex, const FontDrawConfig& fontDrawConfig,
                            const RapidVulkan::GraphicsPipeline& sdfPipeline)
  {
    FontDrawConfig drawConfig(fontDrawConfig);
    drawConfig.FontScale = 1.0f;

    {    // draw normal
      const auto& example = m_resources.Normal;
      DrawTextMesh(hCmdBuffer, currentFrameIndex, example.Mesh, example.Font.Texture, m_dependentResources.PipelineNormal,
                   m_resources.DescriptorSetNormal, drawConfig);
      DrawTextMesh(hCmdBuffer, currentFrameIndex, example.ScaledMesh, example.Font.Texture, m_dependentResources.PipelineNormal,
                   m_resources.DescriptorSetNormal, fontDrawConfig);
    }

    {    // draw sdf
      const auto& example = m_resources.Sdf;
      DrawTextMesh(hCmdBuffer, currentFrameIndex, example.Mesh, example.Font.Texture, sdfPipeline, m_resources.DescriptorSetSdf, drawConfig);
      DrawTextMesh(hCmdBuffer, currentFrameIndex, example.ScaledMesh, example.Font.Texture, sdfPipeline, m_resources.DescriptorSetSdf,
                   fontDrawConfig);
    }
  }


  void SdfFonts::DrawTextMesh(const VkCommandBuffer hCmdBuffer, const uint32_t currentFrameIndex, const MeshRecord& mesh,
                              const Vulkan::VUTexture& /*texture*/, const RapidVulkan::GraphicsPipeline& pipeline,
                              const VkDescriptorSet descriptorSet, const FontDrawConfig& fontDrawConfig)
  {
    m_pushConstants.OutlineDistance = (fontDrawConfig.OutlineDistance > 0.0f ? 0.5f * fontDrawConfig.OutlineDistance : 0.0f);
    m_pushConstants.Smoothing = 0.25f / (fontDrawConfig.FontSdfSpread * fontDrawConfig.FontScale);
    {
      auto maxOffsetX = float(fontDrawConfig.FontSdfSpread) / float(m_resources.Sdf.Font.Texture.GetSize().Width());
      auto maxOffsetY = float(fontDrawConfig.FontSdfSpread) / float(m_resources.Sdf.Font.Texture.GetSize().Height());
      m_pushConstants.ShadowOffsetX = fontDrawConfig.ShadowOffset.X != 0.0f ? maxOffsetX * fontDrawConfig.ShadowOffset.X : 0.0f;
      m_pushConstants.ShadowOffsetY = fontDrawConfig.ShadowOffset.Y != 0.0f ? maxOffsetY * fontDrawConfig.ShadowOffset.Y : 0.0f;
    }
    m_pushConstants.ShadowSmoothing = (fontDrawConfig.ShadowSmoothing > 0.0f ? 0.5f * fontDrawConfig.ShadowSmoothing : 0.0f);

    vkCmdPushConstants(hCmdBuffer, m_resources.MainPipelineLayout.Get(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(m_pushConstants), &m_pushConstants);

    vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &descriptorSet, 0, nullptr);
    vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.Get());

    mesh.Mesh.Touch(currentFrameIndex);
    VkDeviceSize offsets = mesh.Mesh.GetVertexStartOffset();
    vkCmdBindVertexBuffers(hCmdBuffer, LocalConfig::VertexBufferBindId, 1, mesh.Mesh.GetVBBufferPointer(), &offsets);
    vkCmdBindIndexBuffer(hCmdBuffer, mesh.Mesh.GetIBBuffer(), 0, VK_INDEX_TYPE_UINT16);
    auto indexRange = mesh.Mesh.GetIndexSpanRange();
    vkCmdDrawIndexed(hCmdBuffer, indexRange.Length, 1, indexRange.Start, 0, 0);
  }

  SdfFonts::ExampleRecord SdfFonts::PrepareExample(const uint32_t maxFramesInFlight, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                   const IContentManager& contentManager, const int32_t lineYPx, const IO::Path& bitmapFontPath,
                                                   const IO::Path& fontAtlasTexturePath, const StringViewLite& strView,
                                                   const SpriteNativeAreaCalc& spriteNativeAreaCalc, const uint32_t densityDpi,
                                                   std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad)
  {
    FSLLOG3_INFO("Preparing example");
    ExampleRecord result;

    FSLLOG3_INFO("- Loading font");
    result.Font.Texture = ReadTexture(m_device, m_deviceQueue, contentManager, fontAtlasTexturePath);
    result.Font.Font = AppHelper::ReadFont(spriteNativeAreaCalc, TypeConverter::To<PxExtent2D>(result.Font.Texture.GetSize()), contentManager,
                                           bitmapFontPath, densityDpi);

    FSLLOG3_INFO("- Generating mesh");
    const BitmapFontConfig fontConfig(1.0f);
    result.Mesh = GenerateMesh(maxFramesInFlight, bufferManager, PxPoint2(0, lineYPx), result.Font, fontConfig, strView, rPositionsScratchpad);
    result.ScaledMesh = GenerateMesh(maxFramesInFlight, bufferManager, PxPoint2(0, lineYPx), result.Font, fontConfig, strView, rPositionsScratchpad);
    return result;
  }


  SdfFonts::MeshRecord SdfFonts::GenerateMesh(const uint32_t maxFramesInFlight, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                              const PxPoint2& dstPositionPx, const FontRecord& fontRecord, const BitmapFontConfig& fontConfig,
                                              const StringViewLite& strView, std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad)
  {
    const TextureAtlasSpriteFont& font = fontRecord.Font;
    const PxSize2D fontTextureSize = fontRecord.Texture.GetSize();

    std::vector<VertexPositionTexture> vertices(strView.size() * 4);
    std::vector<uint16_t> indices(strView.size() * (3 * 2));

    if (strView.size() > rPositionsScratchpad.size())
    {
      rPositionsScratchpad.resize(strView.size());
    }

    // Extract the render rules
    auto scratchpadSpan = SpanUtil::AsSpan(rPositionsScratchpad);
    const bool gotRules = font.ExtractRenderRules(scratchpadSpan, strView);

    auto dstVertexSpan = SpanUtil::AsSpan(vertices);
    auto dstIndexSpan = SpanUtil::AsSpan(indices);
    const auto positionsSpan = scratchpadSpan.subspan(0, gotRules ? strView.size() : 0);

    AppHelper::GenerateVertices(dstVertexSpan, dstPositionPx, positionsSpan, LocalConfig::DefaultZPos, fontTextureSize);
    AppHelper::GenerateIndices(dstIndexSpan, positionsSpan);

    return {dstPositionPx, fontConfig, DynamicMesh(bufferManager, vertices, dstIndexSpan, maxFramesInFlight)};
  }

  void SdfFonts::RegenerateMeshOnDemand(MeshRecord& rMeshRecord, const uint32_t currentFrameIndex, const PxPoint2& dstPositionPx,
                                        const FontRecord& fontRecord, const BitmapFontConfig fontConfig, const StringViewLite& strView,
                                        std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad)
  {
    if (rMeshRecord.Offset == dstPositionPx && rMeshRecord.FontConfig == fontConfig)
    {
      return;
    }
    rMeshRecord.Offset = dstPositionPx;
    rMeshRecord.FontConfig = fontConfig;

    const TextureAtlasSpriteFont& font = fontRecord.Font;
    const PxSize2D fontTextureSize = fontRecord.Texture.GetSize();

    if (strView.size() > rPositionsScratchpad.size())
    {
      rPositionsScratchpad.resize(strView.size());
    }
    auto scratchpadSpan = SpanUtil::AsSpan(rPositionsScratchpad);
    const bool gotRules = font.ExtractRenderRules(scratchpadSpan, strView, fontConfig);
    const auto positionsSpan = scratchpadSpan.subspan(0, gotRules ? strView.size() : 0);

    auto dstVertexSpan = rMeshRecord.Mesh.BeginWrite(currentFrameIndex);
    {
      AppHelper::GenerateVertices(dstVertexSpan, dstPositionPx, positionsSpan, LocalConfig::DefaultZPos, fontTextureSize);
    }
    rMeshRecord.Mesh.EndWrite();
  }

  const RapidVulkan::GraphicsPipeline& SdfFonts::SelectPipeline(const SdfFontMode fontSdfMode)
  {
    switch (fontSdfMode)
    {
    case SdfFontMode::Outline:
      return m_dependentResources.PipelineSdfOutline;
    case SdfFontMode::Shadow:
      return m_dependentResources.PipelineSdfShadow;
    case SdfFontMode::ShadowAndOutline:
      return m_dependentResources.PipelineSdfShadowAndOutline;
    default:
      return m_dependentResources.PipelineSdf;
    }
  }

}
