/****************************************************************************************************************************************************
 * Copyright 2018, 2022, 2024 NXP
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

#include "HDR03_SkyboxToneMapping.hpp"
#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <Shared/CubeMapping/API/Vulkan/CommonMethods.hpp>
#include <Shared/CubeMapping/API/Vulkan/TextureUtil.hpp>
#include <vulkan/vulkan.h>
#include <array>

namespace Fsl
{
  namespace
  {
    constexpr Vector3 DefaultCameraPosition(0.0f, 0.0f, 0.0f);
    constexpr Vector3 DefaultCameraTarget(-4.0f, 0.0f, 0.0f);

    constexpr uint32_t VertexBufferBindId = 0;
    constexpr uint32_t SubpassRender = 0;
    constexpr uint32_t SubpassTonemap = 1;

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      setup.SubpassSystemUI = SubpassTonemap;
      return setup;
    }

    const char* GetTonemapperShaderName(const Tonemapper::Enum tonemapper)
    {
      switch (tonemapper)
      {
      case Tonemapper::LinearNoGamma:
        return "TonemapperLinearNoGamma.frag.spv";
      case Tonemapper::Linear:
        return "TonemapperLinear.frag.spv";
      case Tonemapper::ReinhardLum:
        return "TonemapperReinhardLum.frag.spv";
      case Tonemapper::Uncharted2Lum:
        return "TonemapperUncharted2Lum.frag.spv";
      case Tonemapper::Custom:
        return "TonemapperCustom.frag.spv";
      default:
        throw NotSupportedException("Unsupported tonemapper");
      }
    }


    RapidVulkan::DescriptorSetLayout CreateTonemapDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 1 : input attachment
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 3> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count;
      poolSizes[2].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
      poolSizes[2].descriptorCount = count;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }


    RapidVulkan::RenderPass CreateRenderPass(const VkDevice device, const VkFormat swapchainImageFormat, const VkFormat depthImageFormat,
                                             const VkFormat renderFormat)
    {
      assert(device != VK_NULL_HANDLE);
      assert(swapchainImageFormat != VK_FORMAT_UNDEFINED);
      assert(depthImageFormat != VK_FORMAT_UNDEFINED);

      VkAttachmentReference colorAttachmentReference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
      VkAttachmentReference depthAttachmentReference = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
      VkAttachmentReference finalColorAttachmentReference = {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

      std::array<VkSubpassDescription, 2> subpassDescription{};
      // Main rendering to a HDR buffer
      subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription[0].colorAttachmentCount = 1;
      subpassDescription[0].pColorAttachments = &colorAttachmentReference;
      subpassDescription[0].pDepthStencilAttachment = &depthAttachmentReference;

      // Tone-mapping pass
      VkAttachmentReference inputReferencesTonemapping = {0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
      subpassDescription[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription[1].inputAttachmentCount = 1;
      subpassDescription[1].pInputAttachments = &inputReferencesTonemapping;
      subpassDescription[1].colorAttachmentCount = 1;
      subpassDescription[1].pColorAttachments = &finalColorAttachmentReference;

      std::array<VkSubpassDependency, 2> subpassDependency{};
      // Main rendering to a HDR buffer
      subpassDependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[0].dstSubpass = 0;
      subpassDependency[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency[0].srcAccessMask = 0;
      subpassDependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpassDependency[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      // Tone-mapping pass
      subpassDependency[1].srcSubpass = 0;
      subpassDependency[1].dstSubpass = 1;
      subpassDependency[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
      subpassDependency[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpassDependency[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


      std::array<VkAttachmentDescription, 3> attachments{};
      // color
      attachments[0].format = renderFormat;
      attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      // Depth
      attachments[1].format = depthImageFormat;
      attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      // final color
      attachments[2].format = swapchainImageFormat;
      attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      return {device,
              0,
              UncheckedNumericCast<uint32_t>(attachments.size()),
              attachments.data(),
              UncheckedNumericCast<uint32_t>(subpassDescription.size()),
              subpassDescription.data(),
              UncheckedNumericCast<uint32_t>(subpassDependency.size()),
              subpassDependency.data()};
    }

    Vulkan::VUImageMemoryView CreateRenderAttachment(const Vulkan::VUDevice& device, const VkExtent2D& extent, const VkFormat format,
                                                     const std::string& name)
    {
      // const auto depthFormat = device.GetPhysicalDevice().FindDepthFormat();

      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      imageCreateInfo.format = format;
      imageCreateInfo.extent = {extent.width, extent.height, 1};
      imageCreateInfo.mipLevels = 1;
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

      VkImageSubresourceRange subresourceRange{};
      subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = 1;
      subresourceRange.baseArrayLayer = 0;
      subresourceRange.layerCount = 1;

      Vulkan::VUImageMemoryView imageMemoryView(device, imageCreateInfo, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, name);

      // We know the renderPass is configured to transform the image to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL layout before we need to sample it
      // So we store that in the image for now (even though it will only be true at the point in time the attachment is used via a sampler)
      imageMemoryView.SetImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
      return imageMemoryView;
    }


    RapidVulkan::GraphicsPipeline CreateTonemapPipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                        const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                        const HDR03_SkyboxToneMapping::SimpleMesh& mesh, const VkRenderPass renderPass,
                                                        const uint32_t subpass)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);
      assert(!mesh.VertexAttributeDescription.empty());

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
      pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &mesh.VertexInputBindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(mesh.VertexAttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = mesh.VertexAttributeDescription.data();

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
      pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
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
      pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
      pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
      pipelineColorBlendStateCreateInfo.attachmentCount = 1;
      pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
      pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

      std::array<VkDynamicState, 1> dynamicState = {VK_DYNAMIC_STATE_SCISSOR};

      VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
      pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      pipelineDynamicStateCreateInfo.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicState.size());
      pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState.data();

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
      graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = subpass;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return {pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo};
    }
  }


  HDR03_SkyboxToneMapping::HDR03_SkyboxToneMapping(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_menuUI(config, Tonemapper::COUNT)
    , m_bufferManager(
        std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_rotationSpeed(5.4f, 0.0f)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    const auto options = config.GetOptions<OptionParser>();

    m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());

    const auto contentManager = GetContentManager();

    PrepareScene(contentManager, m_resources.MainScene);
    m_resources.TonemapperVertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Tonemapper.vert.spv"));
    m_resources.ProgramTonemap = CreateTonemappers(m_device.Get(), contentManager);
    m_resources.TonemapDescriptorSetLayout = CreateTonemapDescriptorSetLayout(m_device);

    std::array<VertexElementUsage, 2> quadShaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};
    m_resources.MeshQuad = CreateQuadVertexArray(m_bufferManager, quadShaderBindOrder);

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight * 2);

    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.SceneVertUboBuffer = CommonMethods::CreateUBO(m_device, sizeof(VertexUBOData));
      rFrame.SceneDescriptorSet = CommonMethods::CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainScene.SceneDescriptorSetLayout);

      rFrame.TonemapVertUboBuffer = CommonMethods::CreateUBO(m_device, sizeof(TonemapperUBOData));
      rFrame.TonemapDescriptorSet = CommonMethods::CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.TonemapDescriptorSetLayout);
      CommonMethods::UpdateDescriptorSet(m_device.Get(), rFrame.SceneDescriptorSet, rFrame.SceneVertUboBuffer, m_resources.MainScene.CubemapTexture);
    }
    m_resources.TonemapPipelineLayout = CommonMethods::CreatePipelineLayout(m_resources.TonemapDescriptorSetLayout);
  }


  void HDR03_SkyboxToneMapping::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);
  }


  void HDR03_SkyboxToneMapping::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
      {
        m_rightMouseDown = event.IsPressed();
        if (m_demoAppControl->TryEnableMouseCaptureMode(m_rightMouseDown))
        {
          m_mouseCaptureEnabled = m_rightMouseDown;
        }
        else
        {
          m_mouseCaptureEnabled = false;
        }
        event.Handled();
        break;
      }
    case VirtualMouseButton::Middle:
      if (event.IsPressed())
      {
        m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void HDR03_SkyboxToneMapping::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    VulkanBasic::DemoAppVulkanBasic::ConfigurationChanged(windowMetrics);

    m_menuUI.SetWindowMetrics(windowMetrics);
  }


  void HDR03_SkyboxToneMapping::Update(const DemoTime& demoTime)
  {
    UpdateInput(demoTime);
    m_menuUI.Update(demoTime);

    // m_vertexUboData.MatModel = Matrix::GetIdentity();
    m_vertexUboData.MatView = m_camera.GetViewMatrix();
    float aspect = GetWindowAspectRatio();    // ok since we divide both by two when we show four screens

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    // The ordering in the monogame based Matrix library is the reverse of glm (so perspective * clip instead of clip * perspective)
    m_vertexUboData.MatProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 100.0f) * vulkanClipMatrix;

    m_tonemapUboData.Exposure = m_menuUI.GetExposure();
  }


  void HDR03_SkyboxToneMapping::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers,
                                           const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    // Upload the changes
    m_resources.MainFrameResources[currentFrameIndex].SceneVertUboBuffer.Upload(0, &m_vertexUboData, sizeof(VertexUBOData));
    m_resources.MainFrameResources[currentFrameIndex].TonemapVertUboBuffer.Upload(0, &m_tonemapUboData, sizeof(TonemapperUBOData));

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
      clearValues[1].depthStencil = {1.0f, 0};

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
        DrawIt(demoTime, m_resources.MainFrameResources[currentFrameIndex], hCmdBuffer);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass HDR03_SkyboxToneMapping::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    // Table 47. Mandatory format support : 16 - bit channels
    // https://www.khronos.org/registry/vulkan/specs/1.0/pdf/vkspec.pdf
    // This format is mandatory, so it should be safe to use
    VkFormat renderFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

    m_dependentResources.MainRenderPass =
      CreateRenderPass(m_device.Get(), context.SwapchainImageFormat, context.DepthBufferImageFormat, renderFormat);
    m_dependentResources.RenderAttachment = CreateRenderAttachment(m_device, context.SwapchainImageExtent, renderFormat, "RenderHDR");

    // Update the preallocated tone-mapping descriptor set with the 'dependent' render attachment
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      CommonMethods::UpdateDescriptorSet(m_device.Get(), rFrame.TonemapDescriptorSet, rFrame.TonemapVertUboBuffer,
                                         m_dependentResources.RenderAttachment);
    }

    const auto& scene = m_resources.MainScene;
    m_dependentResources.ScenePipeline =
      CommonMethods::CreatePipeline(scene.ScenePipelineLayout, context.SwapchainImageExtent, scene.VertShaderModule.Get(),
                                    scene.FragShaderModule.Get(), scene.Mesh, m_dependentResources.MainRenderPass.Get(), SubpassRender);

    for (std::size_t i = 0; i < m_dependentResources.PipelineTonemapper.size(); ++i)
    {
      m_dependentResources.PipelineTonemapper[i] =
        CreateTonemapPipeline(m_resources.TonemapPipelineLayout, context.SwapchainImageExtent, m_resources.TonemapperVertShaderModule.Get(),
                              m_resources.ProgramTonemap[i].Get(), m_resources.MeshQuad, m_dependentResources.MainRenderPass.Get(), SubpassTonemap);
    }

    return m_dependentResources.MainRenderPass.Get();
  }


  void HDR03_SkyboxToneMapping::OnFreeResources()
  {
    m_dependentResources.Reset();
  }

  RapidVulkan::Framebuffer HDR03_SkyboxToneMapping::CreateFramebuffer(const VulkanBasic::FrameBufferCreateContext& frameBufferCreateContext)
  {
    assert(m_dependentResources.RenderAttachment.IsValid());

    std::array<VkImageView, 3> imageViews = {m_dependentResources.RenderAttachment.ImageView().Get(), frameBufferCreateContext.DepthBufferImageView,
                                             frameBufferCreateContext.SwapchainImageView};

    return {m_device.Get(),
            0,
            frameBufferCreateContext.RenderPass,
            UncheckedNumericCast<uint32_t>(imageViews.size()),
            imageViews.data(),
            frameBufferCreateContext.SwapChainImageExtent.width,
            frameBufferCreateContext.SwapChainImageExtent.height,
            1};
  }

  void HDR03_SkyboxToneMapping::DrawIt(const DemoTime& demoTime, const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    auto res = GetScreenExtent();

    DrawScene(frame, commandBuffer, m_resources.MainScene);

    vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.TonemapPipelineLayout.Get(), 0, 1,
                            &frame.TonemapDescriptorSet, 0, nullptr);


    auto rRenderRecords = m_menuUI.GetRenderRecords();
    assert(rRenderRecords.size() == m_resources.ProgramTonemap.size());
    assert(m_dependentResources.PipelineTonemapper.size() == m_resources.ProgramTonemap.size());
    uint32_t startX = 0;
    for (std::size_t i = 0; i < m_dependentResources.PipelineTonemapper.size(); ++i)
    {
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineTonemapper[i].Get());
      auto endX = static_cast<uint32_t>(rRenderRecords[i].SplitX.GetValue());
      VkRect2D scissor{{static_cast<int32_t>(startX), 0}, {endX - startX, res.Height.Value}};
      vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
      startX = endX;

      DrawTonemappedScene(frame, commandBuffer);
    }

    m_menuUI.Draw(demoTime);
  }


  void HDR03_SkyboxToneMapping::UpdateInput(const DemoTime& demoTime)
  {
    if (!m_rightMouseDown)
    {
      m_camera.RotateByRadians(m_rotationSpeed * demoTime.DeltaTime);
    }

    const auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
    m_menuUI.UpdateExposureInput(demoTime, keyboardState);
  }


  void HDR03_SkyboxToneMapping::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      if (!m_rightMouseDown)
      {
        const bool rotateCamera = mouseState.IsRightButtonPressed();
        m_camera.RotateViaPosition(rotateCamera, mouseState.Position);
      }
      else
      {
        if (mouseState.IsRightButtonPressed())
        {
          const auto rawPosition = Vector2(mouseState.RawPosition.X.Value, -mouseState.RawPosition.Y.Value);
          m_camera.Rotate(rawPosition);
        }
      }
    }

    // Keyboard camera movement
    const float movementSpeed = 2.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::W))
    {
      m_camera.MoveForward(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::S))
    {
      m_camera.MoveBackwards(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::A))
    {
      m_camera.MoveLeft(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::D))
    {
      m_camera.MoveRight(movementSpeed);
    }
  }


  void HDR03_SkyboxToneMapping::DrawScene(const FrameResources& frame, const VkCommandBuffer commandBuffer, const Scene& scene)
  {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, scene.ScenePipelineLayout.Get(), 0, 1, &frame.SceneDescriptorSet, 0,
                            nullptr);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.ScenePipeline.Get());

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VertexBufferBindId, 1, scene.Mesh.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(commandBuffer, scene.Mesh.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  void HDR03_SkyboxToneMapping::DrawTonemappedScene(const FrameResources& /*frame*/, const VkCommandBuffer commandBuffer)
  {
    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VertexBufferBindId, 1, m_resources.MeshQuad.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(commandBuffer, m_resources.MeshQuad.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  void HDR03_SkyboxToneMapping::PrepareScene(const std::shared_ptr<IContentManager>& contentManager, Scene& rScene)
  {
    FSLLOG3_INFO("Preparing scene");
    FSLLOG3_INFO("- loading cubemaps")

    Vulkan::VulkanImageCreator imageCreator(m_device, m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);
    // rScene.CubemapTexture = TextureUtil::CreateCubemapTextureFromSix(contentManager, "floral_tent/1024", imageCreator,
    // PixelFormat::R16G16B16A16_SFLOAT);
    rScene.CubemapTexture =
      TextureUtil::CreateCubemapTextureFromSix(contentManager, "Textures/Cubemap/HDR_Lookout/1024", imageCreator, PixelFormat::R16G16B16A16_SFLOAT);

    FSLLOG3_INFO("- loading shaders")
    rScene.VertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("skybox.vert.spv"));
    rScene.FragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("skybox.frag.spv"));

    std::array<VertexElementUsage, 2> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};
    rScene.Mesh = CommonMethods::CreateSkyboxMesh(m_bufferManager, shaderBindOrder);
    rScene.SceneDescriptorSetLayout = CommonMethods::CreateDescriptorSetLayout(m_device);
    rScene.ScenePipelineLayout = CommonMethods::CreatePipelineLayout(rScene.SceneDescriptorSetLayout);
  }


  std::vector<RapidVulkan::ShaderModule> HDR03_SkyboxToneMapping::CreateTonemappers(const VkDevice /*device*/,
                                                                                    const std::shared_ptr<IContentManager>& contentManager)
  {
    std::vector<RapidVulkan::ShaderModule> shaderModules(Tonemapper::COUNT);
    for (std::size_t i = 0; i < shaderModules.size(); ++i)
    {
      const auto* const fragmentShaderName = GetTonemapperShaderName(static_cast<Tonemapper::Enum>(i));
      shaderModules[i].Reset(m_device.Get(), 0, contentManager->ReadBytes(fragmentShaderName));
    }
    return shaderModules;
  }


  HDR03_SkyboxToneMapping::SimpleMesh HDR03_SkyboxToneMapping::CreateQuadVertexArray(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                                                     const std::array<VertexElementUsage, 2>& quadShaderBindOrder)
  {
    SimpleMesh mesh;
    // B D
    // |\|
    // A C
    // A = 1.0

    const float size = 1.0f;
    const float x0 = -size;
    const float x1 = size;
    const float y0 = -size;
    const float y1 = size;
    const float zPos = 0.0f;

    const float u0 = 0.0f;
    const float u1 = 1.0f;
    const float v0 = 0.0f;
    const float v1 = 1.0f;

    std::array<VertexPositionTexture, 6> vertices = {
      // Floor
      VertexPositionTexture(Vector3(x0, y1, zPos), Vector2(u0, v1)), VertexPositionTexture(Vector3(x0, y0, zPos), Vector2(u0, v0)),
      VertexPositionTexture(Vector3(x1, y0, zPos), Vector2(u1, v0)),

      VertexPositionTexture(Vector3(x0, y1, zPos), Vector2(u0, v1)), VertexPositionTexture(Vector3(x1, y0, zPos), Vector2(u1, v0)),
      VertexPositionTexture(Vector3(x1, y1, zPos), Vector2(y1, v1)),
    };

    mesh.VertexBuffer.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(vertices), Vulkan::VMBufferUsage::STATIC);

    Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(mesh.VertexAttributeDescription, quadShaderBindOrder, mesh.VertexBuffer);
    mesh.VertexInputBindingDescription.binding = 0;
    mesh.VertexInputBindingDescription.stride = mesh.VertexBuffer.GetElementStride();
    mesh.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return mesh;
  }
}
