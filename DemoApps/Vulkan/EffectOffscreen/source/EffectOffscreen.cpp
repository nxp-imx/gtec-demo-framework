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

#include "EffectOffscreen.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionTextureTexture.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <array>
#include <cmath>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    constexpr const float BOTTOM_HALF_SPLIT_PERCENTAGE = 0.25f;
    const uint32_t SIZE_OFFSCREEN = 512;

    const auto VERTEX_BUFFER_BIND_ID = 0;

    // B D
    // |\|
    // A C
    // A = 1.0
    const float CUBE_DIMENSIONS = 100.0f;

    const float CUBE_CEILING = CUBE_DIMENSIONS;
    const float CUBE_FLOOR = -CUBE_DIMENSIONS;

    const float CUBE_LEFT = -CUBE_DIMENSIONS;
    const float CUBE_RIGHT = CUBE_DIMENSIONS;
    const float CUBE_BACK = CUBE_DIMENSIONS;      // zBack
    const float CUBE_FRONT = -CUBE_DIMENSIONS;    // zFront
    const float CUBE_U0 = 0.0f;
    const float CUBE_U1 = 1.0f;
    const float CUBE_V0 = 1.0f;
    const float CUBE_V1 = 0.0f;

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      // This allows us to reuse the main depth buffer even when the actual 'screen' is too small for our offscreen buffer
      setup.DepthBufferMinimumExtent = PxExtent2D(SIZE_OFFSCREEN, SIZE_OFFSCREEN);
      return setup;
    }

    const std::array<VertexPositionTexture, 6 * 6> g_vertices = {
      // Floor
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),      // LB
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V1)),     // LF
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),    // RF

      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),      // LB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),    // RF
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V0)),     // RB

      // Ceiling
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V1)),     // LF
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),      // LB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),    // RF

      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),    // RF
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),      // LB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V0)),     // RB

      // Back wall
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U1, CUBE_V0)),
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),

      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U1, CUBE_V0)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V0)),

      // Front wall
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V1)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V1)),

      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V1)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),

      //// Right wall
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),       // FB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V1)),      // FF
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),    // CF

      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),       // FB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),    // CF
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U1, CUBE_V0)),     // CB

      // Left wall
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V1)),      // FF
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),       // FB
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),    // CF

      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),    // CF
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),       // FB
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V0)),     // CB
    };

    constexpr std::array<VertexPositionTextureTexture, 12> CreateDoubleQuadVertexArray(const float size)
    {
      // B D
      // |\|
      // A C
      // A = 1.0

      constexpr const float ySplitPercentage = BOTTOM_HALF_SPLIT_PERCENTAGE;

      const float x0 = -size;
      const float x1 = size;
      const float y0 = -size;
      const float y1 = size * ySplitPercentage;
      const float y2 = size;
      const float zPos = 0.0f;

      const float u0 = 0.0f;
      const float u1 = 1.0f;
      const float v0 = 0.0f;
      const float v1 = std::min(0.5f + (0.5f * ySplitPercentage), 1.0f);
      const float v2 = 0.0f;

      const float u0b = 0.0f;
      const float u1b = 1.0f;
      const float v0b = 0.0f;
      const float v1b = 0.0f;
      const float v2b = 1.0f;

      std::array<VertexPositionTextureTexture, 12> vertices = {
        VertexPositionTextureTexture(Vector3(x0, y1, zPos), Vector2(u0, v1), Vector2(u0b, v1b)),
        VertexPositionTextureTexture(Vector3(x0, y0, zPos), Vector2(u0, v0), Vector2(u0b, v0b)),
        VertexPositionTextureTexture(Vector3(x1, y0, zPos), Vector2(u1, v0), Vector2(u1b, v0b)),

        VertexPositionTextureTexture(Vector3(x0, y1, zPos), Vector2(u0, v1), Vector2(u0b, v1b)),
        VertexPositionTextureTexture(Vector3(x1, y0, zPos), Vector2(u1, v0), Vector2(u1b, v0b)),
        VertexPositionTextureTexture(Vector3(x1, y1, zPos), Vector2(u1, v1), Vector2(u1b, v1b)),

        VertexPositionTextureTexture(Vector3(x0, y2, zPos), Vector2(u0, v2), Vector2(u0b, v2b)),
        VertexPositionTextureTexture(Vector3(x0, y1, zPos), Vector2(u0, v1), Vector2(u0b, v1b)),
        VertexPositionTextureTexture(Vector3(x1, y1, zPos), Vector2(u1, v1), Vector2(u1b, v1b)),

        VertexPositionTextureTexture(Vector3(x0, y2, zPos), Vector2(u0, v2), Vector2(u0b, v2b)),
        VertexPositionTextureTexture(Vector3(x1, y1, zPos), Vector2(u1, v1), Vector2(u1b, v1b)),
        VertexPositionTextureTexture(Vector3(x1, y2, zPos), Vector2(u1, v2), Vector2(u1b, v2b)),
      };
      return vertices;
    }

    VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                            const std::shared_ptr<IContentManager>& contentManager, const IO::Path& filename, const bool repeat,
                            const bool generateMipMaps)
    {
      VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      Texture texture =
        contentManager->ReadTexture(filename, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::Undefined, PixelChannelOrder::Undefined, generateMipMaps);

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = !repeat ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.addressModeV = !repeat ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.addressModeW = !repeat ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = static_cast<float>(texture.GetLevels());
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(texture, samplerCreateInfo);
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

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

    RapidVulkan::DescriptorSetLayout CreateEffectDescriptorSetLayout(const VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 3> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 1 : Fragment shader image sampler
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count * 2;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }

    Vulkan::VUBufferMemory CreateVertexShaderUBO(const Vulkan::VUDevice& device, const VkDeviceSize size)
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

    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const VUBufferMemory& uboBuffer,
                                        const VUTexture& texture)
    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(uboBuffer.IsValid());
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
      // Binding 0 : Vertex shader uniform buffer
      auto uboBufferInfo = uboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].pNext = nullptr;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &uboBufferInfo;

      // Binding 1 : Fragment shader texture sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].pNext = nullptr;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureImageInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

      return descriptorSet;
    }


    VkDescriptorSet UpdateEffectDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const VUBufferMemory& uboBuffer,
                                              const VUTexture& texture0, const VUTexture& texture1)
    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(uboBuffer.IsValid());
      assert(texture0.IsValid());
      assert(texture1.IsValid());

      std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};
      // Binding 0 : Vertex shader uniform buffer
      auto uboBufferInfo = uboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].pNext = nullptr;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &uboBufferInfo;

      // Binding 1 : Fragment shader texture sampler
      auto textureImageInfo0 = texture0.GetDescriptorImageInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].pNext = nullptr;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureImageInfo0;

      // Binding 2 : Fragment shader texture sampler
      auto textureImageInfo1 = texture1.GetDescriptorImageInfo();
      writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[2].pNext = nullptr;
      writeDescriptorSets[2].dstSet = descriptorSet;
      writeDescriptorSets[2].dstBinding = 2;
      writeDescriptorSets[2].descriptorCount = 1;
      writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[2].pImageInfo = &textureImageInfo1;

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

    Vulkan::VUTexture CreateRenderAttachment(const Vulkan::VUDevice& device, const VkExtent2D& extent, const VkFormat format, const std::string& name)
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

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
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
  }


  EffectOffscreen::EffectOffscreen(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_bufferManager(std::make_shared<VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
  {
    const std::shared_ptr<IContentManager> contentManager = GetContentManager();

    m_resources.MainProgramInfo.VertexShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shader.vert.spv"));
    m_resources.MainProgramInfo.FragmentShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shader.frag.spv"));

    m_resources.EffectVertexShader.Reset(m_device.Get(), 0, contentManager->ReadBytes("Effect.vert.spv"));
    m_resources.EffectFragmentShaderTop.Reset(m_device.Get(), 0, contentManager->ReadBytes("EffectTop.frag.spv"));
    m_resources.EffectFragmentShaderBottom.Reset(m_device.Get(), 0, contentManager->ReadBytes("EffectBottom.frag.spv"));

    m_resources.Texture = CreateTexture(m_device, m_deviceQueue, contentManager, "Textures/GPUSdk/SquareLogo512x512.jpg", false, true);
    m_resources.EffectTexture = CreateTexture(m_device, m_deviceQueue, contentManager, "Textures/VFX/Distortion.png", true, false);
    m_resources.CubeVertexBufferInfo = CreateCubeVertexBuffer(m_bufferManager);
    m_resources.DoubleQuadVertexBufferInfo = CreateDoubleQuadVertexBuffer(m_bufferManager);
    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);
    m_resources.EffectDescriptorSetLayout = CreateEffectDescriptorSetLayout(m_device);

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    // * 2 due to the two descriptor sets
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight * 3);
    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.UboBuffer = CreateVertexShaderUBO(m_device, sizeof(VertexUBOData));
      rFrame.DescriptorSetScene = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
      rFrame.EffectUboBuffer = CreateVertexShaderUBO(m_device, sizeof(EffectFragUBOData));

      rFrame.DescriptorSetEffect = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.EffectDescriptorSetLayout);

      UpdateDescriptorSet(m_device.Get(), rFrame.DescriptorSetScene, rFrame.UboBuffer, m_resources.Texture);
    }

    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);
    m_resources.EffectBottomPipelineLayout = CreatePipelineLayout(m_resources.EffectDescriptorSetLayout);

    const auto aspectRatio = GetWindowAspectRatio();
    m_matProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(75.0f), aspectRatio, 1.0f, 1000.0f);
    m_matTranslate = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
  }


  void EffectOffscreen::Update(const DemoTime& demoTime)
  {
    m_angle.X -= 0.40f * demoTime.DeltaTime;
    m_angle.Y -= 0.30f * demoTime.DeltaTime;
    m_angle.Z -= 0.20f * demoTime.DeltaTime;
    m_angle.X = MathHelper::WrapAngle(m_angle.X);
    m_angle.Y = MathHelper::WrapAngle(m_angle.Y);
    m_angle.Z = MathHelper::WrapAngle(m_angle.Z);

    m_effectUboData.Time = TimeSpanUtil::ToSecondsF(demoTime.TotalTime);

    // Rotate and translate the model view matrix
    m_matModel = Matrix::CreateRotationX(m_angle.X) * Matrix::CreateRotationY(m_angle.Y) * Matrix::CreateRotationZ(m_angle.Z) * m_matTranslate;
  }

  void EffectOffscreen::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    FSL_PARAM_NOT_USED(demoTime);

    const auto currentFrameIndex = drawContext.CurrentFrameIndex;
    assert(currentFrameIndex < m_resources.MainFrameResources.size());

    // Upload the changes
    VertexUBOData buffer;
    buffer.MatModelView = m_matModel;
    buffer.MatProj = m_matProj;
    m_resources.MainFrameResources[currentFrameIndex].UboBuffer.Upload(0, &buffer, sizeof(VertexUBOData));

    m_resources.MainFrameResources[currentFrameIndex].EffectUboBuffer.Upload(0, &m_effectUboData, sizeof(EffectFragUBOData));

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];

    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      DrawOffscreenRenderpass(rCmdBuffers, drawContext);

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
        DrawToCommandBuffer(m_resources.MainFrameResources[currentFrameIndex], hCmdBuffer, drawContext.SwapchainImageExtent);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass EffectOffscreen::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    auto offscreenExtent = VkExtent2D{SIZE_OFFSCREEN, SIZE_OFFSCREEN};
    const auto offscreenRenderFormat = context.SwapchainImageFormat;

    m_dependentResources.Offscreen.Extent = offscreenExtent;
    m_dependentResources.Offscreen.RP = CreateRenderPass(m_device.Get(), offscreenRenderFormat, context.DepthBufferImageFormat);
    m_dependentResources.Offscreen.Color = CreateRenderAttachment(m_device, offscreenExtent, offscreenRenderFormat, "RenderOffscreen");
    m_dependentResources.Offscreen.FB = CreateOffscreenFramebuffer(m_dependentResources.Offscreen.Color.ImageView().Get(), context.DepthImageView,
                                                                   m_dependentResources.Offscreen.RP.Get(), offscreenExtent);


    // Update the preallocated tone-mapping descriptor set with the 'dependent' render attachment
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      UpdateEffectDescriptorSet(m_device.Get(), rFrame.DescriptorSetEffect, rFrame.EffectUboBuffer, m_dependentResources.Offscreen.Color,
                                m_resources.EffectTexture);
    }

    m_dependentResources.ScenePipeline = CreateScenePipeline(
      m_resources.MainPipelineLayout, offscreenExtent, m_resources.MainProgramInfo.VertexShaderModule.Get(),
      m_resources.MainProgramInfo.FragmentShaderModule.Get(), m_resources.CubeVertexBufferInfo, m_dependentResources.Offscreen.RP.Get(), 0);

    m_dependentResources.PipelineEffectBottom = CreatePipeline(m_resources.EffectBottomPipelineLayout, context.SwapchainImageExtent,
                                                               m_resources.EffectVertexShader.Get(), m_resources.EffectFragmentShaderBottom.Get(),
                                                               m_resources.DoubleQuadVertexBufferInfo, m_dependentResources.MainRenderPass.Get(), 0);
    return m_dependentResources.MainRenderPass.Get();
  }


  void EffectOffscreen::OnFreeResources()
  {
    m_dependentResources = {};
  }


  RapidVulkan::Framebuffer EffectOffscreen::CreateOffscreenFramebuffer(const VkImageView colorImageView, const VkImageView depthImageView,
                                                                       const VkRenderPass renderPass, const VkExtent2D& extent)
  {
    std::array<VkImageView, 2> imageViews = {colorImageView, depthImageView};

    return {m_device.Get(), 0, renderPass, UncheckedNumericCast<uint32_t>(imageViews.size()), imageViews.data(), extent.width, extent.height, 1};
  }


  void EffectOffscreen::DrawOffscreenRenderpass(RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const auto currentFrameIndex = drawContext.CurrentFrameIndex;
    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_dependentResources.Offscreen.RP.Get();
    renderPassBeginInfo.framebuffer = m_dependentResources.Offscreen.FB.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = m_dependentResources.Offscreen.Extent;
    renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(m_dependentResources.Offscreen.Extent.width);
      viewport.height = static_cast<float>(m_dependentResources.Offscreen.Extent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);
      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = m_dependentResources.Offscreen.Extent;
      vkCmdSetScissor(hCmdBuffer, 0, 1, &scissor);

      DrawSceneToCommandBuffer(m_resources.MainFrameResources[currentFrameIndex], hCmdBuffer);
    }
    rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
  }


  void EffectOffscreen::DrawSceneToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &frame.DescriptorSetScene, 0,
                            nullptr);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.ScenePipeline.Get());

    DrawCube(frame, commandBuffer, m_resources.MainProgramInfo, m_matModel);
  }

  void EffectOffscreen::DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer, const VkExtent2D& extent)
  {
    VkDeviceSize offsets = 0;

    auto vertexCount = m_resources.DoubleQuadVertexBufferInfo.VertexBuffer.GetVertexCount() / 2;

    {    // Top
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(extent.width);
      viewport.height = static_cast<float>(extent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = {extent.width, static_cast<uint32_t>(static_cast<float>(extent.height) * (0.5f + (0.5f * BOTTOM_HALF_SPLIT_PERCENTAGE)))};
      vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

      DrawSceneToCommandBuffer(frame, commandBuffer);
    }

    {    // Bottom water part
      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.EffectBottomPipelineLayout.Get(), 0, 1,
                              &frame.DescriptorSetEffect, 0, nullptr);

      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineEffectBottom.Get());

      vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.DoubleQuadVertexBufferInfo.VertexBuffer.GetBufferPointer(),
                             &offsets);
      vkCmdDraw(commandBuffer, vertexCount, 1, vertexCount, 0);
    }
  }


  void EffectOffscreen::DrawCube(const FrameResources& frame, const VkCommandBuffer commandBuffer, const ProgramInfo& programInfo,
                                 const Matrix& matModel)
  {
    FSL_PARAM_NOT_USED(frame);
    FSL_PARAM_NOT_USED(programInfo);
    FSL_PARAM_NOT_USED(matModel);

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.CubeVertexBufferInfo.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(commandBuffer, m_resources.CubeVertexBufferInfo.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  EffectOffscreen::VertexBufferInfo EffectOffscreen::CreateCubeVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager)
  {
    VertexBufferInfo info;
    info.VertexBuffer.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(g_vertices), VMBufferUsage::STATIC);

    // Generate attribute description by matching shader layout with the vertex declarations
    std::array<VertexElementUsage, 2> shaderAttribOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};

    Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(info.AttributeDescription, shaderAttribOrder, info.VertexBuffer);

    info.BindingDescription.stride = info.VertexBuffer.GetElementStride();
    info.BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return info;
  }


  EffectOffscreen::EffectVertexBufferInfo EffectOffscreen::CreateDoubleQuadVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager)
  {
    EffectVertexBufferInfo info;

    const auto vertices = CreateDoubleQuadVertexArray(1.0f);
    info.VertexBuffer.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(vertices), VMBufferUsage::STATIC);

    // Generate attribute description by matching shader layout with the vertex declarations
    //{
    //   std::array<VertexElementUsage, 3> shaderAttribOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate,
    //                                                         VertexElementUsage::TextureCoordinate};

    //   Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(info.AttributeDescription, shaderAttribOrder, info.VertexBuffer);
    //}

    {
      const auto& vertexDeclElement = info.VertexBuffer.GetVertexElement(VertexElementUsage::Position, 0);
      info.AttributeDescription[0].location = 0u;
      info.AttributeDescription[0].binding = 0;
      info.AttributeDescription[0].format = vertexDeclElement.NativeFormat;
      info.AttributeDescription[0].offset = vertexDeclElement.Offset;
    }
    {
      const auto& vertexDeclElement = info.VertexBuffer.GetVertexElement(VertexElementUsage::TextureCoordinate, 0);
      info.AttributeDescription[1].location = 1u;
      info.AttributeDescription[1].binding = 0;
      info.AttributeDescription[1].format = vertexDeclElement.NativeFormat;
      info.AttributeDescription[1].offset = vertexDeclElement.Offset;
    }
    {
      const auto& vertexDeclElement = info.VertexBuffer.GetVertexElement(VertexElementUsage::TextureCoordinate, 1);
      info.AttributeDescription[2].location = 2u;
      info.AttributeDescription[2].binding = 0;
      info.AttributeDescription[2].format = vertexDeclElement.NativeFormat;
      info.AttributeDescription[2].offset = vertexDeclElement.Offset;
    }

    info.BindingDescription.stride = info.VertexBuffer.GetElementStride();
    info.BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return info;
  }

  RapidVulkan::GraphicsPipeline EffectOffscreen::CreateScenePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                                     const VkShaderModule vertexShaderModule,
                                                                     const VkShaderModule fragmentShaderModule,
                                                                     const VertexBufferInfo& vertexBufferInfo, const VkRenderPass renderPass,
                                                                     const uint32_t subpass)
  {
    assert(pipelineLayout.IsValid());
    assert(vertexShaderModule != VK_NULL_HANDLE);
    assert(fragmentShaderModule != VK_NULL_HANDLE);
    assert(vertexBufferInfo.VertexBuffer.IsValid());
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
    pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexBufferInfo.BindingDescription;
    pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(vertexBufferInfo.AttributeDescription.size());
    pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = vertexBufferInfo.AttributeDescription.data();

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

    std::array<VkDynamicState, 2> dynamicState = {VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT};

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

  RapidVulkan::GraphicsPipeline EffectOffscreen::CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                                const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                                const EffectVertexBufferInfo& vertexBufferInfo, const VkRenderPass renderPass,
                                                                const uint32_t subpass)
  {
    assert(pipelineLayout.IsValid());
    assert(vertexShaderModule != VK_NULL_HANDLE);
    assert(fragmentShaderModule != VK_NULL_HANDLE);
    assert(vertexBufferInfo.VertexBuffer.IsValid());
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
    pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexBufferInfo.BindingDescription;
    pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(vertexBufferInfo.AttributeDescription.size());
    pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = vertexBufferInfo.AttributeDescription.data();

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
}
