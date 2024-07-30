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

#include "HDR02_FBBasicToneMapping.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <Shared/HDR/BasicScene/API/Vulkan/CommonMethods.hpp>
#include <Shared/HDR/BasicScene/API/Vulkan/SimpleMeshUtil.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <array>
#include <cmath>

namespace Fsl
{
  namespace
  {
    constexpr auto VertexBufferBindId = 0;
    constexpr uint32_t SubpassRender = 0;
    constexpr uint32_t SubpassTonemap = 1;

    constexpr Vector3 DefaultCameraPosition(0.0f, 0.0f, 0.0f);
    constexpr Vector3 DefaultCameraTarget(0.0f, 0.0f, -4.0f);


    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      setup.SubpassSystemUI = SubpassTonemap;
      return setup;
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 4> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

      // Binding 1 : Fragment shader uniform buffer
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 2 : sampler
      setLayoutBindings[2].binding = 2;
      setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[2].descriptorCount = 1;
      setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 3 : input attachment
      setLayoutBindings[3].binding = 3;
      setLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
      setLayoutBindings[3].descriptorCount = 1;
      setLayoutBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses two ubo and one image sampler
      std::array<VkDescriptorPoolSize, 3> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count * 2;
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


    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer,
                                        const Vulkan::VUBufferMemory& fragUboBuffer, const Vulkan::VUTexture& texture,
                                        const Vulkan::VUImageMemoryView& attachment)

    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(vertUboBuffer.IsValid());
      assert(fragUboBuffer.IsValid());
      assert(texture.IsValid());
      assert(attachment.IsValid());

      std::array<VkWriteDescriptorSet, 4> writeDescriptorSets{};
      // Binding 0 : Vertex shader uniform buffer
      auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      // Binding 1 : Fragment shader uniform buffer
      auto fragUboBufferInfo = fragUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[1].pBufferInfo = &fragUboBufferInfo;

      // Binding 2 : sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[2].dstSet = descriptorSet;
      writeDescriptorSets[2].dstBinding = 2;
      writeDescriptorSets[2].descriptorCount = 1;
      writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[2].pImageInfo = &textureImageInfo;

      // Binding 3 : input attachment
      auto attachmentImageInfo = attachment.GetDescriptorImageInfo();
      writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[3].dstSet = descriptorSet;
      writeDescriptorSets[3].dstBinding = 3;
      writeDescriptorSets[3].descriptorCount = 1;
      writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
      writeDescriptorSets[3].pImageInfo = &attachmentImageInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

      return descriptorSet;
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

      return {descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo};
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
      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      imageCreateInfo.format = format;
      imageCreateInfo.extent = {extent.width, extent.height, 1};
      imageCreateInfo.mipLevels = 1;
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
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
  }


  HDR02_FBBasicToneMapping::HDR02_FBBasicToneMapping(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_bufferManager(
        std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
    , m_menuUI(config, UI::UIColorSpace::SRGBNonLinear)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());

    const auto contentManager = GetContentManager();

    m_fragmentUboData = PrepareLights();

    auto textureFilename = CommonMethods::GetTextureFile(m_deviceActiveFeatures);

    m_resources.TexSRGB = CommonMethods::CreateTexture(m_device, m_deviceQueue, contentManager, textureFilename);

    m_resources.VertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shader.vert.spv"));
    m_resources.FragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shader.frag.spv"));
    m_resources.TonemapperVertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Tonemapper.vert.spv"));
    m_resources.TonemapperLDRFragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("TonemapperLDR.frag.spv"));
    m_resources.TonemapperHDRFragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("TonemapperHDR.frag.spv"));

    std::array<VertexElementUsage, 3> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::Normal,
                                                         VertexElementUsage::TextureCoordinate};

    std::array<VertexElementUsage, 2> quadShaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};

    m_resources.MeshTunnel = SimpleMeshUtil::CreateTunnelVertexArray(m_bufferManager, shaderBindOrder);
    m_resources.MeshQuad = SimpleMeshUtil::CreateQuadVertexArray(m_bufferManager, quadShaderBindOrder);

    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);
    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    // * 2 because we need the the Render and Tonemapping
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight * 2);
    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.VertUboBuffer = CommonMethods::CreateUBO(m_device, sizeof(VertexUBOData));
      rFrame.FragUboBuffer = CommonMethods::CreateUBO(m_device, sizeof(FragmentUBOData));
      rFrame.DescriptorSetRender = CommonMethods::CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
    }
    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);
  }


  void HDR02_FBBasicToneMapping::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);
  }


  void HDR02_FBBasicToneMapping::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
      {
        const bool mouseCapture = event.IsPressed();
        if (m_demoAppControl->TryEnableMouseCaptureMode(mouseCapture))
        {
          m_mouseCaptureEnabled = mouseCapture;
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


  void HDR02_FBBasicToneMapping::Update(const DemoTime& demoTime)
  {
    UpdateInput(demoTime);
    m_menuUI.Update(demoTime);

    m_vertexUboData.MatModel = Matrix::GetIdentity();
    m_vertexUboData.MatView = m_camera.GetViewMatrix();
    float aspect = GetWindowAspectRatio();    // ok since we divide both by two when we show four screens

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    m_vertexUboData.MatProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 100.0f) * vulkanClipMatrix;
  }


  void HDR02_FBBasicToneMapping::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers,
                                            const VulkanBasic::DrawContext& drawContext)
  {
    FSL_PARAM_NOT_USED(demoTime);

    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    // Upload the changes
    m_fragmentUboData.Exposure = m_menuUI.GetExposure();
    m_resources.MainFrameResources[currentFrameIndex].VertUboBuffer.Upload(0, &m_vertexUboData, sizeof(VertexUBOData));
    m_resources.MainFrameResources[currentFrameIndex].FragUboBuffer.Upload(0, &m_fragmentUboData, sizeof(FragmentUBOData));

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
        DrawToCommandBuffer(m_resources.MainFrameResources[currentFrameIndex], hCmdBuffer);

        // Calling this last allows the UI to draw on top of everything.
        m_menuUI.Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass HDR02_FBBasicToneMapping::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    VkFormat renderFormat = context.SwapchainImageFormat;

    // Table 47. Mandatory format support : 16 - bit channels
    // https://www.khronos.org/registry/vulkan/specs/1.0/pdf/vkspec.pdf
    // This format is mandatory, so it should be safe to use
    renderFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

    m_dependentResources.MainRenderPass =
      CreateRenderPass(m_device.Get(), context.SwapchainImageFormat, context.DepthBufferImageFormat, renderFormat);
    m_dependentResources.RenderAttachment = CreateRenderAttachment(m_device, context.SwapchainImageExtent, renderFormat, "RenderHDR");

    // Update the preallocated tone-mapping descriptor set with the 'dependent' render attachment
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      UpdateDescriptorSet(m_device.Get(), rFrame.DescriptorSetRender, rFrame.VertUboBuffer, rFrame.FragUboBuffer, m_resources.TexSRGB,
                          m_dependentResources.RenderAttachment);
    }

    m_dependentResources.PipelineRender = CommonMethods::CreatePipeline(
      m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.VertShaderModule.Get(), m_resources.FragShaderModule.Get(),
      m_resources.MeshTunnel, m_dependentResources.MainRenderPass.Get(), SubpassRender);

    m_dependentResources.PipelineTonemapperHDR = CommonMethods::CreatePipeline(
      m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.TonemapperVertShaderModule.Get(),
      m_resources.TonemapperHDRFragShaderModule.Get(), m_resources.MeshQuad, m_dependentResources.MainRenderPass.Get(), SubpassTonemap);

    m_dependentResources.PipelineTonemapperLDR = CommonMethods::CreatePipeline(
      m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.TonemapperVertShaderModule.Get(),
      m_resources.TonemapperLDRFragShaderModule.Get(), m_resources.MeshQuad, m_dependentResources.MainRenderPass.Get(), SubpassTonemap);

    return m_dependentResources.MainRenderPass.Get();
  }


  void HDR02_FBBasicToneMapping::OnFreeResources()
  {
    m_dependentResources.Reset();
  }

  RapidVulkan::Framebuffer HDR02_FBBasicToneMapping::CreateFramebuffer(const VulkanBasic::FrameBufferCreateContext& frameBufferCreateContext)
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


  void HDR02_FBBasicToneMapping::DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    auto res = GetScreenExtent();

    {
      VkRect2D scissor{{0, 0}, TypeConverter::UncheckedTo<VkExtent2D>(res)};
      vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    const auto splitX = static_cast<uint32_t>(std::round(m_menuUI.SplitX.GetValue() * static_cast<float>(res.Width.Value)));
    const uint32_t remainderX = res.Width.Value >= splitX ? res.Width.Value - splitX : 0u;

    const bool inTransition = !m_menuUI.SplitX.IsCompleted();
    const bool useClip = m_menuUI.GetState() == SceneState::Split2 || inTransition;
    const bool showingScene1 = useClip || m_menuUI.GetState() == SceneState::Scene1;
    const bool showingScene2 = useClip || m_menuUI.GetState() == SceneState::Scene2;

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &frame.DescriptorSetRender, 0,
                            nullptr);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineRender.Get());
    DrawScene(frame, commandBuffer);

    vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

    if (showingScene1)
    {
      if (useClip)
      {
        VkRect2D scissor{{0, 0}, {splitX, res.Height.Value}};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
      }
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineTonemapperLDR.Get());
      DrawTonemappedScene(frame, commandBuffer);
    }
    if (showingScene2)
    {
      if (useClip)
      {
        VkRect2D scissor{{static_cast<int32_t>(splitX), 0}, {remainderX, res.Height.Value}};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
      }
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineTonemapperHDR.Get());
      DrawTonemappedScene(frame, commandBuffer);
    }
  }


  void HDR02_FBBasicToneMapping::UpdateInput(const DemoTime& demoTime)
  {
    const auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
    m_menuUI.UpdateExposureInput(demoTime, keyboardState);
  }


  void HDR02_FBBasicToneMapping::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      if (!m_mouseCaptureEnabled)
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


  void HDR02_FBBasicToneMapping::DrawScene(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    FSL_PARAM_NOT_USED(frame);

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VertexBufferBindId, 1, m_resources.MeshTunnel.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(commandBuffer, m_resources.MeshTunnel.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  void HDR02_FBBasicToneMapping::DrawTonemappedScene(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    FSL_PARAM_NOT_USED(frame);

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VertexBufferBindId, 1, m_resources.MeshQuad.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(commandBuffer, m_resources.MeshQuad.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  HDR02_FBBasicToneMapping::FragmentUBOData HDR02_FBBasicToneMapping::PrepareLights()
  {
    FragmentUBOData fragmentUBO;
    // lighting info
    fragmentUBO.LightPositions[0] = Vector4(0.0f, 0.0f, -31.5f, 1.0f);
    fragmentUBO.LightPositions[1] = Vector4(-1.4f, -1.9f, -9.0f, 1.0f);
    fragmentUBO.LightPositions[2] = Vector4(0.0f, -1.8f, -4.0f, 1.0f);
    // fragmentUBO.LightPositions[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    fragmentUBO.LightPositions[3] = Vector4(0.8f, -1.7f, -6.0f, 1.0f);

    fragmentUBO.LightColors[0] = Vector4(200.0f, 200.0f, 200.0f, 0.0f);
    fragmentUBO.LightColors[1] = Vector4(0.1f, 0.0f, 0.0f, 0.0f);
    fragmentUBO.LightColors[2] = Vector4(0.0f, 0.0f, 0.2f, 0.0f);
    fragmentUBO.LightColors[3] = Vector4(0.0f, 0.1f, 0.0f, 0.0f);
    // fragmentUBO.LightColors[3] = Vector3(221.0f, 1.1f, 1.0f)

    fragmentUBO.Exposure = 1.0f;
    return fragmentUBO;
  }
}
