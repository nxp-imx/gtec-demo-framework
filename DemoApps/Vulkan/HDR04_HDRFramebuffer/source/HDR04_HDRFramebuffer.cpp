/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include "HDR04_HDRFramebuffer.hpp"
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
#include "OptionParserEx.hpp"

namespace Fsl
{
  namespace
  {
    const auto VERTEX_BUFFER_BIND_ID = 0;
    const uint32_t SUBPASS_RENDER = 0;
    const uint32_t SUBPASS_TONEMAP = 1;

    const Vector3 DEFAULT_CAMERA_POSITION(0.0f, 0.0f, -20.0f);
    const Vector3 DEFAULT_CAMERA_TARGET(0.0f, 0.0f, -25.0f);


    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      setup.SubpassSystemUI = SUBPASS_TONEMAP;
      return setup;
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 3> setLayoutBindings{};
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

      // Binding 2 : Fragment shader image sampler
      setLayoutBindings[2].binding = 2;
      setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[2].descriptorCount = 1;
      setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }

    RapidVulkan::DescriptorSetLayout CreateTonemapDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 3> setLayoutBindings{};
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

      // Binding 2 : input attachment
      setLayoutBindings[2].binding = 2;
      setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
      setLayoutBindings[2].descriptorCount = 1;
      setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

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
                                        const Vulkan::VUBufferMemory& fragUboBuffer, const Vulkan::VUTexture& texture)

    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(vertUboBuffer.IsValid());
      assert(fragUboBuffer.IsValid());
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};
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

      // Binding 2 : Fragment shader texture sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[2].dstSet = descriptorSet;
      writeDescriptorSets[2].dstBinding = 2;
      writeDescriptorSets[2].descriptorCount = 1;
      writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[2].pImageInfo = &textureImageInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

      return descriptorSet;
    }


    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer,
                                        const Vulkan::VUBufferMemory& fragUboBuffer, const Vulkan::VUImageMemoryView& attachment)

    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(vertUboBuffer.IsValid());
      assert(fragUboBuffer.IsValid());
      assert(attachment.IsValid());

      std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};
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

      // Binding 2 : input attachment
      auto attachmentImageInfo = attachment.GetDescriptorImageInfo();
      writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[2].dstSet = descriptorSet;
      writeDescriptorSets[2].dstBinding = 2;
      writeDescriptorSets[2].descriptorCount = 1;
      writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
      writeDescriptorSets[2].pImageInfo = &attachmentImageInfo;

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
  }


  HDR04_HDRFramebuffer::HDR04_HDRFramebuffer(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_hasHDRFramebuffer(GetSurfaceFormatInfo().ColorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    , m_bufferManager(
        std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
    , m_menuUI(config, !m_hasHDRFramebuffer ? ColorSpace::SRGBNonLinear : ColorSpace::SCRGBLinear)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_useDebugPattern(true)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    const auto options = config.GetOptions<OptionParserEx>();
    m_useDebugPattern = !options->IsPatternDisabled();

    if (m_hasHDRFramebuffer)
    {
      switch (GetSurfaceFormatInfo().ColorSpace)
      {
      case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
        FSLLOG3_INFO("Using HDR framebuffer with colorspace: VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT");
        break;
      case VK_COLOR_SPACE_DCI_P3_LINEAR_EXT:
        FSLLOG3_INFO("Using HDR framebuffer with colorspace: VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT");
        break;
      case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
        FSLLOG3_INFO("Using HDR framebuffer with colorspace: VK_COLOR_SPACE_BT2020_LINEAR_EXT");
        break;
      default:
        FSLLOG3_INFO("Using HDR framebuffer with colorspace: Unknown");
        break;
      }
      FSLLOG3_INFO_IF(m_useDebugPattern, "Debug pattern: on");
    }

    if (!m_hasHDRFramebuffer)
    {
      m_menuUI.SetNoteLabel("HDRFramebuffer: Not available");
    }
    else
    {
      m_menuUI.SetNoteLabel("HDRFramebuffer: Enabled");
    }
    m_menuUI.SetCaptionLeft("Linear");
    m_menuUI.SetCaptionRight(m_hasHDRFramebuffer ? "Bad tonemapper" : "Tonemapped");
    m_menuUI.SetMenuTextLeft("Linear");
    m_menuUI.SetMenuTextRight("Tonemapped");

    m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());

    const auto contentManager = GetContentManager();

    m_fragmentUboData = PrepareLights();

    auto textureFilename = CommonMethods::GetTextureFile(m_deviceActiveFeatures);

    m_resources.TexSRGB = CommonMethods::CreateTexture(m_device, m_deviceQueue, contentManager, textureFilename);

    m_resources.VertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shader.vert.spv"));
    m_resources.FragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shader.frag.spv"));

    const auto* const filenameTonemapLinear = (m_hasHDRFramebuffer ? "TonemapperLinear.frag.spv" : "TonemapperLinearLDR.frag.spv");
    const auto* const filenameTonemapLinearDebug = (m_hasHDRFramebuffer ? "TonemapperLinearDebug.frag.spv" : "TonemapperLinearLDRDebug.frag.spv");
    // NOTE: The Uncharted2Lum shader is really intended for LDR displays so it does not look nice on a real HDR framebuffer
    //       This will be changed in a upcoming update.
    const auto* const filenameTonemap = (m_hasHDRFramebuffer ? "TonemapperUncharted2Lum.frag.spv" : "TonemapperUncharted2LumLDR.frag.spv");
    const auto* const filenameTonemapDebug =
      (m_hasHDRFramebuffer ? "TonemapperUncharted2LumDebug.frag.spv" : "TonemapperUncharted2LumLDRDebug.frag.spv");

    FSLLOG3_VERBOSE("Shader TonemapLinear: {}", filenameTonemapLinear);
    FSLLOG3_VERBOSE("Shader TonemapLinearDebug: {}", filenameTonemapLinearDebug);
    FSLLOG3_VERBOSE("Shader Tonemap: {}", filenameTonemap);
    FSLLOG3_VERBOSE("Shader TonemapDebug: {}", filenameTonemapDebug);

    m_resources.TonemapperVertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Tonemapper.vert.spv"));
    m_resources.TonemapperLinearFragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(filenameTonemapLinear));
    m_resources.TonemapperLinearDebugFragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(filenameTonemapLinearDebug));
    m_resources.TonemapperFragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(filenameTonemap));
    m_resources.TonemapperDebugFragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(filenameTonemapDebug));

    std::array<VertexElementUsage, 3> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::Normal,
                                                         VertexElementUsage::TextureCoordinate};

    std::array<VertexElementUsage, 2> quadShaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};

    m_resources.MeshTunnel = SimpleMeshUtil::CreateTunnelVertexArray(m_bufferManager, shaderBindOrder);
    m_resources.MeshQuad = SimpleMeshUtil::CreateQuadVertexArray(m_bufferManager, quadShaderBindOrder);

    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);
    m_resources.TonemapDescriptorSetLayout = CreateTonemapDescriptorSetLayout(m_device);
    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    // * 2 because we need the the Render and Tonemapping
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight * 2);
    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.VertUboBuffer = CommonMethods::CreateUBO(m_device, sizeof(VertexUBOData));
      rFrame.FragUboBuffer = CommonMethods::CreateUBO(m_device, sizeof(FragmentUBOData));
      rFrame.DescriptorSetRender = CommonMethods::CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
      rFrame.DescriptorSetTonemap = CommonMethods::CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.TonemapDescriptorSetLayout);

      UpdateDescriptorSet(m_device.Get(), rFrame.DescriptorSetRender, rFrame.VertUboBuffer, rFrame.FragUboBuffer, m_resources.TexSRGB);
    }
    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);
    m_resources.TonemapPipelineLayout = CreatePipelineLayout(m_resources.TonemapDescriptorSetLayout);
  }


  void HDR04_HDRFramebuffer::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);

    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::P:
    case VirtualKey::GamePadButtonY:
      m_useDebugPattern = !m_useDebugPattern;
      break;
    default:
      break;
    }
  }


  void HDR04_HDRFramebuffer::OnMouseButtonEvent(const MouseButtonEvent& event)
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
        m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }

  void HDR04_HDRFramebuffer::Update(const DemoTime& demoTime)
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


  void HDR04_HDRFramebuffer::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers,
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


  VkRenderPass HDR04_HDRFramebuffer::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
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
      UpdateDescriptorSet(m_device.Get(), rFrame.DescriptorSetTonemap, rFrame.VertUboBuffer, rFrame.FragUboBuffer,
                          m_dependentResources.RenderAttachment);
    }

    m_dependentResources.PipelineRender = CommonMethods::CreatePipeline(
      m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.VertShaderModule.Get(), m_resources.FragShaderModule.Get(),
      m_resources.MeshTunnel, m_dependentResources.MainRenderPass.Get(), SUBPASS_RENDER);

    m_dependentResources.PipelineTonemapper = CommonMethods::CreatePipeline(
      m_resources.TonemapPipelineLayout, context.SwapchainImageExtent, m_resources.TonemapperVertShaderModule.Get(),
      m_resources.TonemapperFragShaderModule.Get(), m_resources.MeshQuad, m_dependentResources.MainRenderPass.Get(), SUBPASS_TONEMAP);

    m_dependentResources.PipelineTonemapperDebug = CommonMethods::CreatePipeline(
      m_resources.TonemapPipelineLayout, context.SwapchainImageExtent, m_resources.TonemapperVertShaderModule.Get(),
      m_resources.TonemapperDebugFragShaderModule.Get(), m_resources.MeshQuad, m_dependentResources.MainRenderPass.Get(), SUBPASS_TONEMAP);

    m_dependentResources.PipelineTonemapperLinear = CommonMethods::CreatePipeline(
      m_resources.TonemapPipelineLayout, context.SwapchainImageExtent, m_resources.TonemapperVertShaderModule.Get(),
      m_resources.TonemapperLinearFragShaderModule.Get(), m_resources.MeshQuad, m_dependentResources.MainRenderPass.Get(), SUBPASS_TONEMAP);

    m_dependentResources.PipelineTonemapperLinearDebug = CommonMethods::CreatePipeline(
      m_resources.TonemapPipelineLayout, context.SwapchainImageExtent, m_resources.TonemapperVertShaderModule.Get(),
      m_resources.TonemapperLinearDebugFragShaderModule.Get(), m_resources.MeshQuad, m_dependentResources.MainRenderPass.Get(), SUBPASS_TONEMAP);

    return m_dependentResources.MainRenderPass.Get();
  }


  void HDR04_HDRFramebuffer::OnFreeResources()
  {
    m_dependentResources.Reset();
  }

  RapidVulkan::Framebuffer HDR04_HDRFramebuffer::CreateFramebuffer(const VulkanBasic::FrameBufferCreateContext& frameBufferCreateContext)
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


  void HDR04_HDRFramebuffer::DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer)
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
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.TonemapPipelineLayout.Get(), 0, 1,
                            &frame.DescriptorSetTonemap, 0, nullptr);

    if (showingScene1)
    {
      if (useClip)
      {
        VkRect2D scissor{{0, 0}, {splitX, res.Height.Value}};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
      }

      const VkPipeline tonemapPipeline =
        m_useDebugPattern ? m_dependentResources.PipelineTonemapperLinearDebug.Get() : m_dependentResources.PipelineTonemapperLinear.Get();
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, tonemapPipeline);
      DrawTonemappedScene(frame, commandBuffer);
    }
    if (showingScene2)
    {
      if (useClip)
      {
        VkRect2D scissor{{static_cast<int32_t>(splitX), 0}, {remainderX, res.Height.Value}};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
      }

      const VkPipeline tonemapPipeline =
        m_useDebugPattern ? m_dependentResources.PipelineTonemapperDebug.Get() : m_dependentResources.PipelineTonemapper.Get();
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, tonemapPipeline);
      DrawTonemappedScene(frame, commandBuffer);
    }
  }


  void HDR04_HDRFramebuffer::UpdateInput(const DemoTime& demoTime)
  {
    const auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
    m_menuUI.UpdateExposureInput(demoTime, keyboardState);
  }


  void HDR04_HDRFramebuffer::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
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


  void HDR04_HDRFramebuffer::DrawScene(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    FSL_PARAM_NOT_USED(frame);

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.MeshTunnel.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(commandBuffer, m_resources.MeshTunnel.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  void HDR04_HDRFramebuffer::DrawTonemappedScene(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    FSL_PARAM_NOT_USED(frame);

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.MeshQuad.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(commandBuffer, m_resources.MeshQuad.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  HDR04_HDRFramebuffer::FragmentUBOData HDR04_HDRFramebuffer::PrepareLights()
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
