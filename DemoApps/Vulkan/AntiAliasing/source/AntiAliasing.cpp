/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include "AntiAliasing.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxRectangle.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Span/ReadOnlyFlexSpanUtil_Array.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/Service/Texture/ITextureService.hpp>
#include <FslGraphics/NativeTextureAreaUtilEx.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexPosition2.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/DeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView CarModelPath("Models/FuturisticCar/Futuristic_Car.3ds");
      constexpr IO::PathView CarModelShaderPath("");
      constexpr IO::PathView CarModelTextureDiffuse("Models/FuturisticCar/Futuristic_Car_C.jpg");
      constexpr IO::PathView CarModelTextureSpecular("Models/FuturisticCar/Futuristic_Car_S.jpg");
      constexpr IO::PathView CarModelTextureNormal("Models/FuturisticCar/Futuristic_Car_N.jpg");

      constexpr IO::PathView KnightModelPath("Models/Knight2/armor.obj");
      constexpr IO::PathView KnightModelShaderPath("");
      constexpr IO::PathView KnightModelTextureDiffuse("Models/Knight2/armor_default_color.jpg");
      constexpr IO::PathView KnightModelTextureSpecular("Models/Knight2/armor_default_metalness.jpg");
      constexpr IO::PathView KnightModelTextureNormal("Models/Knight2/armor_default_nmap.jpg");

      constexpr float CarModelScale = 1.0f;
      constexpr float KnightModelScale = 0.75f;

      constexpr uint32_t InitialLineCapacity = 64u;

      constexpr uint32_t PostprocessVertexBufferBindId = 0u;
      constexpr uint32_t ZoomVertexBufferBindId = 0u;
    }

    constexpr std::array<VertexPosition2, 3> PostProcessVertices = {
      VertexPosition2(-1.0f, -1.0f),    // Bottom-left
      VertexPosition2(3.0f, -1.0f),     // Bottom-right (overdraws)
      VertexPosition2(-1.0f, 3.0f)      // Top-left (overdraws)
    };


    RapidVulkan::RenderPass CreateCustomZoomRenderPass(const VkDevice device, const VkFormat renderFormat)
    {
      assert(device != VK_NULL_HANDLE);

      VkAttachmentReference colorAttachmentReference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

      std::array<VkSubpassDescription, 1> subpassDescription{};
      // Rendering to a offscreen buffer
      subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription[0].colorAttachmentCount = 1;
      subpassDescription[0].pColorAttachments = &colorAttachmentReference;
      subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

      std::array<VkSubpassDependency, 2> subpassDependency{};
      // The offscreen buffer
      subpassDependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[0].dstSubpass = 0;
      subpassDependency[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      subpassDependency[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
      subpassDependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpassDependency[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      // output
      subpassDependency[1].srcSubpass = 0;
      subpassDependency[1].dstSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpassDependency[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      subpassDependency[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      subpassDependency[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      std::array<VkAttachmentDescription, 1> attachments{};
      // color
      attachments[0].format = renderFormat;
      attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      return {device,
              0,
              UncheckedNumericCast<uint32_t>(attachments.size()),
              attachments.data(),
              UncheckedNumericCast<uint32_t>(subpassDescription.size()),
              subpassDescription.data(),
              UncheckedNumericCast<uint32_t>(subpassDependency.size()),
              subpassDependency.data()};
    }


    RapidVulkan::RenderPass CreateRenderPass(const VkDevice device, const VkFormat renderFormat, const VkFormat depthImageFormat)
    {
      assert(device != VK_NULL_HANDLE);
      assert(depthImageFormat != VK_FORMAT_UNDEFINED);

      VkAttachmentReference colorAttachmentReference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
      VkAttachmentReference depthAttachmentReference = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

      std::array<VkSubpassDescription, 1> subpassDescription{};
      // Rendering to a offscreen buffer
      subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription[0].colorAttachmentCount = 1;
      subpassDescription[0].pColorAttachments = &colorAttachmentReference;
      subpassDescription[0].pDepthStencilAttachment = &depthAttachmentReference;
      subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

      std::array<VkSubpassDependency, 2> subpassDependency{};
      // The offscreen buffer
      subpassDependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[0].dstSubpass = 0;
      subpassDependency[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      subpassDependency[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
      subpassDependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      subpassDependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      subpassDependency[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      // output
      subpassDependency[1].srcSubpass = 0;
      subpassDependency[1].dstSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      subpassDependency[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      subpassDependency[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      subpassDependency[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      subpassDependency[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      std::array<VkAttachmentDescription, 2> attachments{};
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

      return {device,
              0,
              UncheckedNumericCast<uint32_t>(attachments.size()),
              attachments.data(),
              UncheckedNumericCast<uint32_t>(subpassDescription.size()),
              subpassDescription.data(),
              UncheckedNumericCast<uint32_t>(subpassDependency.size()),
              subpassDependency.data()};
    }

    RapidVulkan::RenderPass CreateMSAARenderPass(const VkDevice device, const VkFormat renderFormat, const VkFormat depthImageFormat,
                                                 const VkSampleCountFlagBits sampleCount)
    {
      assert(device != VK_NULL_HANDLE);
      assert(depthImageFormat != VK_FORMAT_UNDEFINED);

      VkAttachmentReference colorAttachmentReference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
      VkAttachmentReference depthAttachmentReference = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
      VkAttachmentReference colorResolveAttachmentReference = {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

      std::array<VkSubpassDescription, 1> subpassDescription{};
      // Rendering to a offscreen buffer
      subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription[0].colorAttachmentCount = 1;
      subpassDescription[0].pColorAttachments = &colorAttachmentReference;
      subpassDescription[0].pDepthStencilAttachment = &depthAttachmentReference;
      subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription[0].colorAttachmentCount = 1;
      subpassDescription[0].pColorAttachments = &colorAttachmentReference;
      subpassDescription[0].pDepthStencilAttachment = &depthAttachmentReference;
      subpassDescription[0].pResolveAttachments = &colorResolveAttachmentReference;

      std::array<VkSubpassDependency, 2> subpassDependency{};
      // The offscreen buffer
      subpassDependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[0].dstSubpass = 0;
      subpassDependency[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      subpassDependency[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
      subpassDependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      subpassDependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      subpassDependency[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      // output
      subpassDependency[1].srcSubpass = 0;
      subpassDependency[1].dstSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      subpassDependency[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      subpassDependency[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      subpassDependency[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      subpassDependency[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      std::array<VkAttachmentDescription, 3> attachments{};
      // color
      attachments[0].format = renderFormat;
      attachments[0].samples = sampleCount;
      attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;    // We only care about the resolved image
      attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      // Depth
      attachments[1].format = depthImageFormat;
      attachments[1].samples = sampleCount;
      attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      // resolve
      attachments[2].format = renderFormat;
      attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[2].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      return {device,
              0,
              UncheckedNumericCast<uint32_t>(attachments.size()),
              attachments.data(),
              UncheckedNumericCast<uint32_t>(subpassDescription.size()),
              subpassDescription.data(),
              UncheckedNumericCast<uint32_t>(subpassDependency.size()),
              subpassDependency.data()};
    }


    Vulkan::VUTexture CreateRenderAttachment(const Vulkan::VUDevice& device, const VkExtent2D& extent, const VkFormat format,
                                             const VkSampleCountFlagBits sampleCount, const std::string& name, const bool useLinearFiltering)
    {
      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      imageCreateInfo.format = format;
      imageCreateInfo.extent = {extent.width, extent.height, 1};
      imageCreateInfo.mipLevels = 1;
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.samples = sampleCount;
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

      const VkFilter filter = useLinearFiltering ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = filter;
      samplerCreateInfo.minFilter = filter;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = 1.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      Vulkan::VUTexture finalTexture(std::move(imageMemoryView), samplerCreateInfo);
      // We know the renderPass is configured to transform the image to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL layout before we need to sample it
      // So we store that in the image for now (even though it will only be true at the point in time the attachment is used via a sampler)
      finalTexture.SetImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
      return finalTexture;
    }


    RapidVulkan::Sampler CreateSampler(const Vulkan::VUDevice& device)
    {
      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
      samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = 1.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
      return {device.Get(), samplerCreateInfo};
    }


    RapidVulkan::Framebuffer CreateOffscreenFramebuffer(const VkDevice device, const VkImageView colorImageView, const VkImageView depthImageView,
                                                        const VkRenderPass renderPass, const VkExtent2D& extent)
    {
      std::array<VkImageView, 2> imageViews = {colorImageView, depthImageView};

      return {device, 0, renderPass, UncheckedNumericCast<uint32_t>(imageViews.size()), imageViews.data(), extent.width, extent.height, 1};
    }

    RapidVulkan::Framebuffer CreateCustomZoomFramebuffer(const VkDevice device, const VkImageView colorImageView, const VkRenderPass renderPass,
                                                         const VkExtent2D& extent)
    {
      std::array<VkImageView, 1> imageViews = {colorImageView};
      return {device, 0, renderPass, UncheckedNumericCast<uint32_t>(imageViews.size()), imageViews.data(), extent.width, extent.height, 1};
    }


    RapidVulkan::Framebuffer CreateOffscreenMSAAFramebuffer(const VkDevice device, const VkImageView colorImageView, const VkImageView depthImageView,
                                                            const VkImageView resolveColorImageView, const VkRenderPass renderPass,
                                                            const VkExtent2D& extent)
    {
      std::array<VkImageView, 3> imageViews = {colorImageView, depthImageView, resolveColorImageView};

      return {device, 0, renderPass, UncheckedNumericCast<uint32_t>(imageViews.size()), imageViews.data(), extent.width, extent.height, 1};
    }


    RapidVulkan::DescriptorSetLayout CreatePostProcessDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings{};
      // Binding 0 : Fragment shader image sampler
      setLayoutBindings[0].binding = 1;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }

    RapidVulkan::DescriptorSetLayout CreateZoomDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 1 : Fragment shader image sampler
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
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


    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet,
                                        const VkDescriptorImageInfo& textureDescriptorImageInfo)
    {
      assert(descriptorSet != VK_NULL_HANDLE);

      std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};
      // Binding 0 : Fragment shader texture sampler
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].pNext = nullptr;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 1;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[0].pImageInfo = &textureDescriptorImageInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

      return descriptorSet;
    }


    VkDescriptorSet UpdateZoomDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer,
                                            const VkDescriptorImageInfo& textureDescriptorImageInfo)
    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(vertUboBuffer.IsValid());

      std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
      // Binding 0 : Vertex shader uniform buffer
      auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      // Binding 1 : Fragment shader texture sampler
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureDescriptorImageInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
      return descriptorSet;
    }


    Vulkan::VUBufferMemory CreateUBO(const Vulkan::VUDevice& device, const VkDeviceSize size)
    {
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = size;
      bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      Vulkan::VUBufferMemory buffer(device, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      // Keep the buffer mapped as we update it each frame
      buffer.Map();
      return buffer;
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

      return {descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo};
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[0].descriptorCount = 3 * count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[1].descriptorCount = 2 * count;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count * 3;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }

    RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                 const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                 const VkSampleCountFlagBits sampleCount, const VkVertexInputBindingDescription& bindingDescription,
                                                 const ReadOnlySpan<VkVertexInputAttributeDescription> attributeDescription,
                                                 const VkRenderPass renderPass, const uint32_t subpass, const bool depthCheckEnabled)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);

      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};

      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].flags = 0;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].flags = 0;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
      pipelineShaderStageCreateInfo[1].pName = "main";
      pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

      VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
      pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      pipelineVertexInputCreateInfo.flags = 0;
      pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(attributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.flags = 0;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(extent.width);
      viewport.height = static_cast<float>(extent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{{0, 0}, extent};

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
      pipelineMultisampleStateCreateInfo.rasterizationSamples = sampleCount;
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

      const VkBool32 depthEnable = depthCheckEnabled ? VK_TRUE : VK_FALSE;
      VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
      depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilStateCreateInfo.depthTestEnable = depthEnable;
      depthStencilStateCreateInfo.depthWriteEnable = depthEnable;
      depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
      depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.front = {};
      depthStencilStateCreateInfo.back = {};
      depthStencilStateCreateInfo.minDepthBounds = 0.0f;
      depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

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
      graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
      graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
      graphicsPipelineCreateInfo.pDynamicState = nullptr;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = subpass;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return {pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo};
    }


    constexpr SupportedSampleCountFlags ToSupportedSampleCountFlags(const VkSampleCountFlags flags) noexcept
    {
      SupportedSampleCountFlags res = SupportedSampleCountFlags::NoFlags;
      res |= ((flags & VK_SAMPLE_COUNT_1_BIT) != 0u) ? SupportedSampleCountFlags::Samples1 : SupportedSampleCountFlags::NoFlags;
      res |= ((flags & VK_SAMPLE_COUNT_2_BIT) != 0u) ? SupportedSampleCountFlags::Samples2 : SupportedSampleCountFlags::NoFlags;
      res |= ((flags & VK_SAMPLE_COUNT_4_BIT) != 0u) ? SupportedSampleCountFlags::Samples4 : SupportedSampleCountFlags::NoFlags;
      res |= ((flags & VK_SAMPLE_COUNT_8_BIT) != 0u) ? SupportedSampleCountFlags::Samples8 : SupportedSampleCountFlags::NoFlags;
      res |= ((flags & VK_SAMPLE_COUNT_16_BIT) != 0u) ? SupportedSampleCountFlags::Samples16 : SupportedSampleCountFlags::NoFlags;
      res |= ((flags & VK_SAMPLE_COUNT_32_BIT) != 0u) ? SupportedSampleCountFlags::Samples32 : SupportedSampleCountFlags::NoFlags;
      res |= ((flags & VK_SAMPLE_COUNT_64_BIT) != 0u) ? SupportedSampleCountFlags::Samples64 : SupportedSampleCountFlags::NoFlags;
      return res;
    }


    AntiAliasingConfig CreateConfig(const Vulkan::VUPhysicalDeviceRecord& physicalDeviceRecord) noexcept
    {
      return AntiAliasingConfig(ToSupportedSampleCountFlags(physicalDeviceRecord.Properties.limits.framebufferColorSampleCounts));
    }

    NativeTextureArea ApplyZoomFactor(const NativeTextureArea& texCoord, const UI::Custom::ZoomInfo& zoomInfo, const float zoomFactor)
    {
      const PxSize1D deltaXPx = zoomInfo.WindowAreaPx.Width();
      const PxSize1D deltaYPx = zoomInfo.WindowAreaPx.Height();
      const PxValue relativeXPx = zoomInfo.WindowMousePositionPx.X - zoomInfo.WindowAreaPx.Left();
      const PxValue relativeYPx = zoomInfo.WindowMousePositionPx.Y - zoomInfo.WindowAreaPx.Top();

      const float percentageX = MathHelper::Clamp(static_cast<float>(relativeXPx.Value) / static_cast<float>(deltaXPx.RawValue()), 0.0f, 1.0f);
      const float percentageY = MathHelper::Clamp(static_cast<float>(relativeYPx.Value) / static_cast<float>(deltaYPx.RawValue()), 0.0f, 1.0f);

      const float du = texCoord.X1 - texCoord.X0;
      const float dv = texCoord.Y1 - texCoord.Y0;
      const float cursorDeltaU = (du * percentageX);
      const float cursorDeltaV = (dv * percentageY);
      const float cursorU = texCoord.X0 + cursorDeltaU;
      const float cursorV = texCoord.Y0 + cursorDeltaV;

      const float lenU0 = (cursorDeltaU) / zoomFactor;
      const float lenV0 = (cursorDeltaV) / zoomFactor;
      const float lenU1 = (du - cursorDeltaU) / zoomFactor;
      const float lenV1 = (dv - cursorDeltaV) / zoomFactor;

      return {cursorU - lenU0, cursorV - lenV0, cursorU + lenU1, cursorV + lenV1};
    }


    Matrix CreateMatWorldViewProjection(const PxSize2D sizePx)
    {
      const auto screenWidth = static_cast<float>(sizePx.RawWidth());
      const auto screenHeight = static_cast<float>(sizePx.RawHeight());
      const float screenOffsetX = screenWidth / 2.0f;
      const float screenOffsetY = screenHeight / 2.0f;

      return Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, -2.0f) * Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
    }


    CustomUIConfig CreateCustomUIConfig()
    {
      return CustomUIConfig(false, false, false);
    }

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      VulkanBasic::DemoAppVulkanSetup setup;
      setup.DepthBuffer = VulkanBasic::DepthBufferMode::Enabled;
      return setup;
    }

    VkSampleCountFlagBits ToVkSampleCountFlagBits(const AntiAliasingMethod method)
    {
      switch (method)
      {
      case AntiAliasingMethod::Disabled:
      case AntiAliasingMethod::SSAA2X:
        return VK_SAMPLE_COUNT_1_BIT;
      case AntiAliasingMethod::MSAA2X:
        return VK_SAMPLE_COUNT_2_BIT;
      case AntiAliasingMethod::MSAA4X:
        return VK_SAMPLE_COUNT_4_BIT;
      case AntiAliasingMethod::MSAA8X:
        return VK_SAMPLE_COUNT_8_BIT;
      }
      throw NotSupportedException("AA method not supported");
    }
  }


  AntiAliasing::AntiAliasing(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_shared(config, CreateConfig(m_physicalDevice), CreateCustomUIConfig())
    , m_textureService(config.DemoServiceProvider.TryGet<ITextureService>())
    , m_bufferManager(
        std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
    , m_resources(
        CreateResources(m_device, m_deviceQueue, m_bufferManager, m_textureService.get(), GetContentManager(), GetRenderConfig().MaxFramesInFlight))
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_shared.GetUIDemoAppExtension());
  }


  AntiAliasing::~AntiAliasing() = default;


  void AntiAliasing::OnKeyEvent(const KeyEvent& event)
  {
    base_type::OnKeyEvent(event);

    m_shared.OnKeyEvent(event);
  }


  void AntiAliasing::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    base_type::ConfigurationChanged(windowMetrics);
    m_shared.OnConfigurationChanged(windowMetrics);
  }


  void AntiAliasing::Update(const DemoTime& demoTime)
  {
    base_type::Update(demoTime);

    m_shared.Update(demoTime);
    m_resources.RenderModelCar->Update(demoTime, m_shared.IsPaused());
    m_resources.RenderModelKnight->Update(demoTime, m_shared.IsPaused());

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    m_linesVertexUboData.MatViewProjection = m_shared.GetViewMatrix() * m_shared.GetProjectionMatrix() * vulkanClipMatrix;
    if (m_dependentResources.CustomZoom.FB.IsValid())
    {
      m_customZoomVertexUboData.MatViewProjection = CreateMatWorldViewProjection(m_dependentResources.CustomZoom.GetFrameBufferSize());
    }
    else
    {
      m_customZoomVertexUboData.MatViewProjection = Matrix::GetIdentity();
    }
    m_zoomVertexUboData.MatViewProjection = CreateMatWorldViewProjection(GetWindowSizePx());

    RecreateFrameBufferIfNecessary();
  }


  void AntiAliasing::Resolve(const DemoTime& demoTime)
  {
    base_type::Resolve(demoTime);
    m_shared.Resolve(demoTime);
  }


  void AntiAliasing::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    // const auto windowSizePx = GetWindowSizePx();
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;
    // Upload the changes
    m_resources.Line.Render.UpdateVertexUBO(&m_linesVertexUboData, sizeof(VertexUBOData), currentFrameIndex);
    m_dividerLinesVertexUboData.MatViewProjection = CreateMatWorldViewProjection(m_dependentResources.Offscreen.GetFrameBufferSize());
    m_resources.DividerLine.Render.UpdateVertexUBO(&m_dividerLinesVertexUboData, sizeof(VertexUBOData), currentFrameIndex);
    m_resources.MainFrameResources[currentFrameIndex].CustomZoomVertUboBuffer.Upload(0u, &m_customZoomVertexUboData, sizeof(VertexUBOData));
    m_resources.MainFrameResources[currentFrameIndex].ZoomVertUboBuffer.Upload(0u, &m_zoomVertexUboData, sizeof(VertexUBOData));

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      DrawToMultisampleFB(rCmdBuffers, drawContext, demoTime);


      const auto zoomInfo = m_shared.GetResolvedZoomInfo();
      const bool useCustomZoom = m_dependentResources.Offscreen.ScalePx != PxSize1D::Create(1);

      if (useCustomZoom)
      {
        DrawToZoomFB(m_resources.MainFrameResources[currentFrameIndex], rCmdBuffers, drawContext, zoomInfo,
                     m_dependentResources.Offscreen.GetFrameBufferSize(), m_dependentResources.Offscreen.ScalePx);
      }

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
        DrawFinalComposite(m_resources.MainFrameResources[currentFrameIndex], hCmdBuffer, drawContext.SwapchainImageExtent, zoomInfo, useCustomZoom);

        // Allow the shared part to draw as well
        m_shared.Draw(demoTime);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass AntiAliasing::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    FSLLOG3_VERBOSE2("Creating dependent resources");

    const auto windowSizePx = GetWindowSizePx();
    const auto aaMethod = m_shared.GetActiveAntiAliasingMethod();

    m_dependentResources.MainRenderPass = CreateBasicRenderPass();


    m_dependentResources.Offscreen = CreateFramebufferResources(m_device, context, windowSizePx, aaMethod);
    if (m_dependentResources.Offscreen.ScalePx != PxSize1D::Create(1))
    {
      m_dependentResources.CustomZoom = CreateCustomZoomResources(m_device, context, m_shared.GetResolvedZoomInfo().MaxSizePx);
    }

    const auto offscreenSampleCount = m_dependentResources.Offscreen.GetSampleCountFlagBit();
    const auto offscreenExtent = m_dependentResources.Offscreen.GetFrameBufferExtent();
    m_resources.Line.Render.BuildResources(offscreenExtent, m_resources.Line.ProgramInfo.VertShaderModule.Get(),
                                           m_resources.Line.ProgramInfo.FragShaderModule.Get(), m_dependentResources.Offscreen.RP.Get(), 0u, true,
                                           true, offscreenSampleCount);
    m_resources.DividerLine.Render.BuildResources(offscreenExtent, m_resources.DividerLine.ProgramInfo.VertShaderModule.Get(),
                                                  m_resources.DividerLine.ProgramInfo.FragShaderModule.Get(), m_dependentResources.Offscreen.RP.Get(),
                                                  0u, false, false, offscreenSampleCount);
    m_resources.RenderModelCar->BuildResources(m_dependentResources.Offscreen.RP.Get(), 0, offscreenSampleCount);
    m_resources.RenderModelKnight->BuildResources(m_dependentResources.Offscreen.RP.Get(), 0, offscreenSampleCount);


    {    // Update the preallocated descriptor set with the 'dependent' render attachment
      VkDescriptorImageInfo descriptorImageInfo = m_dependentResources.Offscreen.GetDescriptorImageInfo();
      VkDescriptorImageInfo nearestDescriptorImageInfo = descriptorImageInfo;
      nearestDescriptorImageInfo.sampler = m_dependentResources.Offscreen.NearestSampler.Get();
      for (auto& rFrame : m_resources.MainFrameResources)
      {
        UpdateDescriptorSet(m_device.Get(), rFrame.DescriptorSetEffect, descriptorImageInfo);
        if (m_dependentResources.CustomZoom.FB.IsValid())
        {
          // Render with linear sampling to the custom offscreen buffer to get the data we zoom on in the correct resolution so we can scale it
          // up correctly for the zoom
          UpdateZoomDescriptorSet(m_device.Get(), rFrame.CustomZoomDescriptorSet, rFrame.CustomZoomVertUboBuffer, descriptorImageInfo);
          UpdateZoomDescriptorSet(m_device.Get(), rFrame.ZoomDescriptorSet, rFrame.ZoomVertUboBuffer,
                                  m_dependentResources.CustomZoom.Color.GetDescriptorImageInfo());
        }
        else
        {
          UpdateZoomDescriptorSet(m_device.Get(), rFrame.CustomZoomDescriptorSet, rFrame.CustomZoomVertUboBuffer, nearestDescriptorImageInfo);
          UpdateZoomDescriptorSet(m_device.Get(), rFrame.ZoomDescriptorSet, rFrame.ZoomVertUboBuffer, nearestDescriptorImageInfo);
        }
      }
    }

    {
      VkVertexInputBindingDescription bindingDescription{};
      std::array<VkVertexInputAttributeDescription, 1> attributeDescription{};
      {
        // Generate attribute description by matching shader layout with the vertex declarations
        std::array<VertexElementUsage, 1> shaderAttribOrder = {VertexElementUsage::Position};

        Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(attributeDescription, shaderAttribOrder,
                                                                        m_resources.PostProcess.VertexBuffer);
        bindingDescription.stride = m_resources.PostProcess.VertexBuffer.GetElementStride();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      }

      m_dependentResources.PostProcessPipeline = CreatePipeline(
        m_resources.PostProcess.PipelineLayout, context.SwapchainImageExtent, m_resources.PostProcess.ProgramInfo.VertShaderModule.Get(),
        m_resources.PostProcess.ProgramInfo.FragShaderModule.Get(), VK_SAMPLE_COUNT_1_BIT, bindingDescription,
        SpanUtil::AsReadOnlySpan(attributeDescription), m_dependentResources.MainRenderPass.Get(), 0, false);
    }

    {
      VkVertexInputBindingDescription bindingDescription{};
      std::array<VkVertexInputAttributeDescription, 2> attributeDescription{};
      {
        // Generate attribute description by matching shader layout with the vertex declarations
        std::array<VertexElementUsage, 2> shaderAttribOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};

        Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(attributeDescription, shaderAttribOrder, m_resources.Zoom.VertexBuffer);
        bindingDescription.stride = m_resources.Zoom.VertexBuffer.GetElementStride();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      }

      if (m_dependentResources.CustomZoom.RP.IsValid())
      {
        m_dependentResources.CustomZoomPipeline = CreatePipeline(
          m_resources.Zoom.PipelineLayout, m_dependentResources.CustomZoom.GetFrameBufferExtent(),
          m_resources.Zoom.ProgramInfo.VertShaderModule.Get(), m_resources.Zoom.ProgramInfo.FragShaderModule.Get(), VK_SAMPLE_COUNT_1_BIT,
          bindingDescription, SpanUtil::AsReadOnlySpan(attributeDescription), m_dependentResources.CustomZoom.RP.Get(), 0, false);
      }

      m_dependentResources.ZoomPipeline =
        CreatePipeline(m_resources.Zoom.PipelineLayout, context.SwapchainImageExtent, m_resources.Zoom.ProgramInfo.VertShaderModule.Get(),
                       m_resources.Zoom.ProgramInfo.FragShaderModule.Get(), VK_SAMPLE_COUNT_1_BIT, bindingDescription,
                       SpanUtil::AsReadOnlySpan(attributeDescription), m_dependentResources.MainRenderPass.Get(), 0, false);
    }

    return m_dependentResources.MainRenderPass.Get();
  }


  void AntiAliasing::OnFreeResources()
  {
    FSLLOG3_VERBOSE2("Freeing dependent resources");
    m_resources.RenderModelKnight->FreeResources();
    m_resources.RenderModelCar->FreeResources();
    m_resources.DividerLine.Render.FreeResources();
    m_resources.Line.Render.FreeResources();
    m_dependentResources.Reset();
  }


  AntiAliasing::FrameBufferResources AntiAliasing::CreateFramebufferResources(const Vulkan::VUDevice& device,
                                                                              const VulkanBasic::BuildResourcesContext& context,
                                                                              const PxSize2D sizePx, const AntiAliasingMethod aaMethod)
  {
    const auto offscreenRenderFormat = context.SwapchainImageFormat;
    const auto sampleCount = ToVkSampleCountFlagBits(aaMethod);
    const bool isMSAA = sampleCount != VK_SAMPLE_COUNT_1_BIT;

    const auto scalePx = aaMethod == AntiAliasingMethod::SSAA2X ? PxSize1D::Create(2) : PxSize1D::Create(1);
    const auto scaledSizePx = sizePx * scalePx;
    const VkExtent2D offscreenExtent = TypeConverter::UncheckedTo<VkExtent2D>(scaledSizePx);

    Vulkan::VUImageMemoryView depthImageView(CreateBasicDepthImageView(device, offscreenExtent, context.MainCommandPool, sampleCount));
    RapidVulkan::RenderPass renderPass(isMSAA ? CreateMSAARenderPass(device.Get(), offscreenRenderFormat, context.DepthBufferImageFormat, sampleCount)
                                              : CreateRenderPass(device.Get(), offscreenRenderFormat, context.DepthBufferImageFormat));
    Vulkan::VUTexture color(
      CreateRenderAttachment(device, offscreenExtent, offscreenRenderFormat, sampleCount, isMSAA ? "UnresolvedMSAAColor" : "ResolvedColor", true));
    Vulkan::VUTexture resolvedColor(
      isMSAA ? CreateRenderAttachment(device, offscreenExtent, offscreenRenderFormat, VK_SAMPLE_COUNT_1_BIT, "ResolvedColor", true)
             : Vulkan::VUTexture());

    auto nearestSampler = CreateSampler(device);

    RapidVulkan::Framebuffer fb(
      isMSAA ? CreateOffscreenMSAAFramebuffer(device.Get(), color.GetImageView(), depthImageView.GetImageView(), resolvedColor.GetImageView(),
                                              renderPass.Get(), offscreenExtent)
             : CreateOffscreenFramebuffer(device.Get(), color.GetImageView(), depthImageView.GetImageView(), renderPass.Get(), offscreenExtent));

    return {aaMethod,
            scaledSizePx,
            sampleCount,
            std::move(depthImageView),
            std::move(renderPass),
            std::move(color),
            std::move(resolvedColor),
            std::move(nearestSampler),
            std::move(fb),
            scalePx};
  }


  AntiAliasing::CustomZoomFrameBufferResources AntiAliasing::CreateCustomZoomResources(const Vulkan::VUDevice& device,
                                                                                       const VulkanBasic::BuildResourcesContext& context,
                                                                                       const PxSize2D maxZoomAreaSizePx)
  {
    const auto offscreenRenderFormat = context.SwapchainImageFormat;
    const VkExtent2D maxZoomAreaExtent = TypeConverter::UncheckedTo<VkExtent2D>(maxZoomAreaSizePx);
    RapidVulkan::RenderPass renderPass(CreateCustomZoomRenderPass(device.Get(), offscreenRenderFormat));
    Vulkan::VUTexture color(
      CreateRenderAttachment(device, maxZoomAreaExtent, offscreenRenderFormat, VK_SAMPLE_COUNT_1_BIT, "CustomZoomColor", false));
    RapidVulkan::Framebuffer fb(CreateCustomZoomFramebuffer(device.Get(), color.GetImageView(), renderPass.Get(), maxZoomAreaExtent));
    return {maxZoomAreaSizePx, std::move(renderPass), std::move(color), std::move(fb)};
  }


  AntiAliasing::LineProgramInfo AntiAliasing::CreateLineProgram(const Vulkan::VUDevice& device,
                                                                const std::shared_ptr<IContentManager>& contentManager)
  {
    LineProgramInfo info;
    info.VertShaderModule.Reset(device.Get(), 0, contentManager->ReadBytes("VertexColorLine.vert.spv"));
    info.FragShaderModule.Reset(device.Get(), 0, contentManager->ReadBytes("VertexColorLine.frag.spv"));
    return info;
  }


  AntiAliasing::PostProcessProgramInfo AntiAliasing::CreatePostProcessProgram(const Vulkan::VUDevice& device,
                                                                              const std::shared_ptr<IContentManager>& contentManager)
  {
    PostProcessProgramInfo info;
    info.VertShaderModule.Reset(device.Get(), 0, contentManager->ReadBytes("FullscreenTriangle.vert.spv"));
    info.FragShaderModule.Reset(device.Get(), 0, contentManager->ReadBytes("FullscreenTriangle.frag.spv"));
    return info;
  }


  AntiAliasing::ZoomProgramInfo AntiAliasing::CreateZoomProgram(const Vulkan::VUDevice& device,
                                                                const std::shared_ptr<IContentManager>& contentManager)
  {
    ZoomProgramInfo info;
    info.VertShaderModule.Reset(device.Get(), 0, contentManager->ReadBytes("Zoom.vert.spv"));
    info.FragShaderModule.Reset(device.Get(), 0, contentManager->ReadBytes("Zoom.frag.spv"));
    return info;
  }


  AntiAliasing::Resources AntiAliasing::CreateResources(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                        const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                        ITextureService* pTextureService, const std::shared_ptr<IContentManager>& contentManager,
                                                        const uint32_t maxFramesInFlight)
  {
    LineDrawResources line;
    line.ProgramInfo = CreateLineProgram(device, contentManager);
    line.Render.Reset(device, bufferManager, maxFramesInFlight, sizeof(VertexUBOData), LocalConfig::InitialLineCapacity);

    LineDrawResources dividerLine;
    dividerLine.ProgramInfo = CreateLineProgram(device, contentManager);
    dividerLine.Render.Reset(device, bufferManager, maxFramesInFlight, sizeof(VertexUBOData), LocalConfig::InitialLineCapacity);

    PostProcessResources postProcessResources;
    postProcessResources.ProgramInfo = CreatePostProcessProgram(device, contentManager);
    postProcessResources.VertexBuffer.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(PostProcessVertices), Vulkan::VMBufferUsage::STATIC);
    postProcessResources.DescriptorSetLayout = CreatePostProcessDescriptorSetLayout(device);
    postProcessResources.PipelineLayout = CreatePipelineLayout(postProcessResources.DescriptorSetLayout);

    constexpr std::array<VertexPositionTexture, 6> ZoomVertices = {};
    ZoomResources zoomResources;
    zoomResources.ProgramInfo = CreateZoomProgram(device, contentManager);
    zoomResources.VertexBuffer.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(ZoomVertices), Vulkan::VMBufferUsage::DYNAMIC);
    zoomResources.VertexBufferCustomZoom.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(ZoomVertices), Vulkan::VMBufferUsage::DYNAMIC);
    zoomResources.DescriptorSetLayout = CreateZoomDescriptorSetLayout(device);
    zoomResources.PipelineLayout = CreatePipelineLayout(zoomResources.DescriptorSetLayout);

    RapidVulkan::DescriptorPool postProcessDescriptorPool = CreateDescriptorPool(device, maxFramesInFlight);
    std::vector<FrameResources> mainFrameResources(maxFramesInFlight);

    FSLLOG3_VERBOSE("Creating frame resources");
    for (auto& rFrame : mainFrameResources)
    {
      FSLLOG3_VERBOSE("- Creating effect descriptor set");
      rFrame.DescriptorSetEffect = CreateDescriptorSet(postProcessDescriptorPool, postProcessResources.DescriptorSetLayout);
      rFrame.CustomZoomVertUboBuffer = CreateUBO(device, sizeof(VertexUBOData));
      rFrame.ZoomVertUboBuffer = CreateUBO(device, sizeof(VertexUBOData));
      FSLLOG3_VERBOSE("- Creating custom zoom descriptor set");
      rFrame.CustomZoomDescriptorSet = CreateDescriptorSet(postProcessDescriptorPool, zoomResources.DescriptorSetLayout);
      FSLLOG3_VERBOSE("- Creating zoom descriptor set");
      rFrame.ZoomDescriptorSet = CreateDescriptorSet(postProcessDescriptorPool, zoomResources.DescriptorSetLayout);
    }
    FSLLOG3_VERBOSE("Frame resources completed");


    return {std::move(line),
            std::move(dividerLine),
            std::move(postProcessResources),
            std::move(zoomResources),
            std::make_unique<RenderModel>(device, deviceQueue, bufferManager, pTextureService, *contentManager, LocalConfig::CarModelPath,
                                          LocalConfig::CarModelShaderPath, LocalConfig::CarModelTextureDiffuse, LocalConfig::CarModelTextureSpecular,
                                          LocalConfig::CarModelTextureNormal, LocalConfig::CarModelScale, maxFramesInFlight),
            std::make_unique<RenderModel>(device, deviceQueue, bufferManager, pTextureService, *contentManager, LocalConfig::KnightModelPath,
                                          LocalConfig::KnightModelShaderPath, LocalConfig::KnightModelTextureDiffuse,
                                          LocalConfig::KnightModelTextureSpecular, LocalConfig::KnightModelTextureNormal,
                                          LocalConfig::KnightModelScale, maxFramesInFlight),
            std::move(postProcessDescriptorPool),
            std::move(mainFrameResources)};
  }


  void AntiAliasing::RecreateFrameBufferIfNecessary()
  {
    const auto activeAntiAliasingMethod = m_shared.GetActiveAntiAliasingMethod();
    if (activeAntiAliasingMethod != m_dependentResources.Offscreen.ActiveAntiAliasingMethod)
    {
      FSLLOG3_VERBOSE2("Recreating frame buffers");
      TryRebuildResources();
    }
  }


  PxSize2D AntiAliasing::ApplyViewport(const VkCommandBuffer hCmdBuffer, const ContentWindowId contentWindowId)
  {
    const PxRectangle windowPx = m_shared.GetContentWindowRectangleInScreenspace(contentWindowId) * m_dependentResources.Offscreen.ScalePx;

    const VkViewport viewport{static_cast<float>(windowPx.RawLeft()),
                              static_cast<float>(windowPx.RawTop()),
                              static_cast<float>(windowPx.RawWidth()),
                              static_cast<float>(windowPx.RawHeight()),
                              0.0f,
                              1.0f};
    vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);

    const VkRect2D scissorRect = TypeConverter::UncheckedTo<VkRect2D>(windowPx);
    vkCmdSetScissor(hCmdBuffer, 0, 1, &scissorRect);

    return windowPx.GetSize();
  }


  void AntiAliasing::GenerateCustomZoomVertices(Vulkan::VMVertexBuffer& rVertexBuffer, const PxSize2D textureSizePx, const PxSize1D texScalePx,
                                                const UI::Custom::ZoomInfo& zoomInfo)
  {
    std::array<VertexPositionTexture, 6> vertices = {};

    const auto x0 = 0.0f;
    const auto x1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawWidth());
    const auto y0 = 0.0f;
    const auto y1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawHeight());
    const float zpos = 0.0f;

    const NativeTextureArea texCoord = NativeTextureAreaUtil::CalcNativeTextureAreaClipped(zoomInfo.ScreenSpaceAreaPx * texScalePx, textureSizePx);

    vertices[0] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));
    vertices[1] = VertexPositionTexture(Vector3(x0, y0, zpos), Vector2(texCoord.X0, texCoord.Y0));
    vertices[2] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));

    vertices[3] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));
    vertices[4] = VertexPositionTexture(Vector3(x1, y1, zpos), Vector2(texCoord.X1, texCoord.Y1));
    vertices[5] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));

    rVertexBuffer.SetData(0, ReadOnlyFlexSpanUtil::AsSpan(vertices));
  }


  void AntiAliasing::GenerateCustomZoomVertices2(Vulkan::VMVertexBuffer& rVertexBuffer, const PxSize2D sizePx, const UI::Custom::ZoomInfo& zoomInfo)
  {
    std::array<VertexPositionTexture, 6> vertices = {};

    const auto x0 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawLeft());
    const auto x1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawRight());
    const auto y0 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawTop());
    const auto y1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawBottom());
    const float zpos = 0.0f;

    const PxRectangle rectPx(PxPoint2(), zoomInfo.ScreenSpaceAreaPx.GetSize());
    NativeTextureArea texCoord = NativeTextureAreaUtil::CalcNativeTextureAreaClipped(rectPx, sizePx);
    texCoord = ApplyZoomFactor(texCoord, zoomInfo, zoomInfo.ZoomFactor);

    vertices[0] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));
    vertices[1] = VertexPositionTexture(Vector3(x0, y0, zpos), Vector2(texCoord.X0, texCoord.Y0));
    vertices[2] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));

    vertices[3] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));
    vertices[4] = VertexPositionTexture(Vector3(x1, y1, zpos), Vector2(texCoord.X1, texCoord.Y1));
    vertices[5] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));

    rVertexBuffer.SetData(0, ReadOnlyFlexSpanUtil::AsSpan(vertices));
  }


  void AntiAliasing::GenerateZoomVertices(Vulkan::VMVertexBuffer& rVertexBuffer, const PxSize2D textureSizePx, const PxSize1D texScalePx,
                                          const UI::Custom::ZoomInfo& zoomInfo)
  {
    std::array<VertexPositionTexture, 6> vertices = {};

    const auto x0 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawLeft());
    const auto x1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawRight());
    const auto y0 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawTop());
    const auto y1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawBottom());
    const float zpos = 0.0f;

    NativeTextureArea texCoord = NativeTextureAreaUtil::CalcNativeTextureAreaClipped(zoomInfo.ScreenSpaceAreaPx * texScalePx, textureSizePx);
    texCoord = ApplyZoomFactor(texCoord, zoomInfo, zoomInfo.ZoomFactor);

    vertices[0] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));
    vertices[1] = VertexPositionTexture(Vector3(x0, y0, zpos), Vector2(texCoord.X0, texCoord.Y0));
    vertices[2] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));

    vertices[3] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));
    vertices[4] = VertexPositionTexture(Vector3(x1, y1, zpos), Vector2(texCoord.X1, texCoord.Y1));
    vertices[5] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));

    rVertexBuffer.SetData(0, ReadOnlyFlexSpanUtil::AsSpan(vertices));
  }


  void AntiAliasing::DrawToMultisampleFB(RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext,
                                         const DemoTime& demoTime)
  {
    const auto currentFrameIndex = drawContext.CurrentFrameIndex;
    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];

    std::array<VkClearValue, 3> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    clearValues[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_dependentResources.Offscreen.RP.Get();
    renderPassBeginInfo.framebuffer = m_dependentResources.Offscreen.FB.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = m_dependentResources.Offscreen.GetFrameBufferExtent();
    renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
      ApplyViewport(hCmdBuffer, ContentWindowId::WireframeCube);

      m_resources.Line.Render.Draw(hCmdBuffer, m_shared.GetLineBuilder().GetVertexSpan(), currentFrameIndex);
      m_resources.DividerLine.Render.Draw(hCmdBuffer, m_shared.GetDividerLineBuilder().GetVertexSpan(), currentFrameIndex);
      m_resources.RenderModelKnight->Draw(hCmdBuffer, drawContext, demoTime,
                                          m_shared.GetContentWindowRectangleInScreenspace(ContentWindowId::Knight) *
                                            m_dependentResources.Offscreen.ScalePx);
      m_resources.RenderModelCar->Draw(hCmdBuffer, drawContext, demoTime,
                                       m_shared.GetContentWindowRectangleInScreenspace(ContentWindowId::Car) *
                                         m_dependentResources.Offscreen.ScalePx);
    }
    rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
  }


  void AntiAliasing::DrawToZoomFB(FrameResources& rFrameResources, RapidVulkan::CommandBuffers& rCmdBuffers,
                                  const VulkanBasic::DrawContext& drawContext, const UI::Custom::ZoomInfo& zoomInfo, const PxSize2D srcSizePx,
                                  const PxSize1D srcTexScalePx)
  {
    const auto currentFrameIndex = drawContext.CurrentFrameIndex;
    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_dependentResources.CustomZoom.RP.Get();
    renderPassBeginInfo.framebuffer = m_dependentResources.CustomZoom.FB.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = m_dependentResources.CustomZoom.GetFrameBufferExtent();
    renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
      GenerateCustomZoomVertices(m_resources.Zoom.VertexBufferCustomZoom, srcSizePx, srcTexScalePx, zoomInfo);

      // Draw zoom area
      vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.Zoom.PipelineLayout.Get(), 0, 1,
                              &rFrameResources.CustomZoomDescriptorSet, 0, nullptr);
      vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.CustomZoomPipeline.Get());

      VkDeviceSize offsets = 0;
      vkCmdBindVertexBuffers(hCmdBuffer, LocalConfig::ZoomVertexBufferBindId, 1, m_resources.Zoom.VertexBufferCustomZoom.GetBufferPointer(),
                             &offsets);
      vkCmdDraw(hCmdBuffer, m_resources.Zoom.VertexBufferCustomZoom.GetVertexCount(), m_resources.Zoom.VertexBufferCustomZoom.GetVertexCount() / 3u,
                0, 0);
    }
    rCmdBuffers.CmdEndRenderPass(currentFrameIndex);

    // const PxSize2D zoomAreaSizePx = zoomInfo.MaxSizePx;
    // if (!rZoomFramebuffer.IsValid())
    //{
    //   rZoomFramebuffer = CreateMSAAResolveFramebuffer(zoomAreaSizePx);
    // }
    // else if (zoomAreaSizePx.Width() > rZoomFramebuffer.GetSize().Width() || zoomAreaSizePx.Height() > rZoomFramebuffer.GetSize().Height())
    //{
    //   rZoomFramebuffer.Reset();
    //   rZoomFramebuffer = CreateMSAAResolveFramebuffer(zoomAreaSizePx);
    // }
  }


  void AntiAliasing::DrawFinalComposite(FrameResources& rFrameResources, const VkCommandBuffer hCmdBuffer, const VkExtent2D& extent,
                                        const UI::Custom::ZoomInfo& zoomInfo, const bool useCustomZoom)
  {
    vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.PostProcess.PipelineLayout.Get(), 0, 1,
                            &rFrameResources.DescriptorSetEffect, 0, nullptr);
    vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PostProcessPipeline.Get());

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(hCmdBuffer, LocalConfig::PostprocessVertexBufferBindId, 1, m_resources.PostProcess.VertexBuffer.GetBufferPointer(),
                           &offsets);
    vkCmdDraw(hCmdBuffer, m_resources.PostProcess.VertexBuffer.GetVertexCount(), 1, 0, 0);


    if (zoomInfo.Enabled)
    {
      if (!useCustomZoom)
      {
        GenerateZoomVertices(m_resources.Zoom.VertexBuffer, m_dependentResources.Offscreen.GetFrameBufferSize(),
                             m_dependentResources.Offscreen.ScalePx, zoomInfo);

        // Draw zoom area
        vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.Zoom.PipelineLayout.Get(), 0, 1,
                                &rFrameResources.ZoomDescriptorSet, 0, nullptr);
        vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.ZoomPipeline.Get());

        vkCmdBindVertexBuffers(hCmdBuffer, LocalConfig::ZoomVertexBufferBindId, 1, m_resources.Zoom.VertexBuffer.GetBufferPointer(), &offsets);
        vkCmdDraw(hCmdBuffer, m_resources.Zoom.VertexBuffer.GetVertexCount(), m_resources.Zoom.VertexBuffer.GetVertexCount() / 3u, 0, 0);
      }
      else
      {
        GenerateCustomZoomVertices2(m_resources.Zoom.VertexBuffer, m_dependentResources.CustomZoom.GetFrameBufferSize(), zoomInfo);

        // Draw zoom area
        vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.Zoom.PipelineLayout.Get(), 0, 1,
                                &rFrameResources.ZoomDescriptorSet, 0, nullptr);
        vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.ZoomPipeline.Get());

        vkCmdBindVertexBuffers(hCmdBuffer, LocalConfig::ZoomVertexBufferBindId, 1, m_resources.Zoom.VertexBuffer.GetBufferPointer(), &offsets);
        vkCmdDraw(hCmdBuffer, m_resources.Zoom.VertexBuffer.GetVertexCount(), m_resources.Zoom.VertexBuffer.GetVertexCount() / 3u, 0, 0);
      }
    }
  }
}
