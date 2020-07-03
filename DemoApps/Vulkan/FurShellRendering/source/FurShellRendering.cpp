/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include "FurShellRendering.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslGraphics3D/Procedural/MeshBuilder.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslGraphics3D/Procedural/BoxGenerator.hpp>
#include <FslGraphics3D/Procedural/TorusGenerator.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <RapidVulkan/Check.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/VUTextureUtil.hpp>
#include <Shared/FurShellRendering/FurTexture.hpp>
#include <Shared/FurShellRendering/OptionParser.hpp>
#include <vulkan/vulkan.h>
#include "RenderMode.hpp"
#include <array>

namespace Fsl
{
  namespace
  {
    const auto VERTEX_BUFFER_BIND_ID = 0;


    enum class ProceduralPrimitive
    {
      Torus,
      Box
    };

    struct ProceduralConfig
    {
      ProceduralPrimitive Primitive{ProceduralPrimitive::Torus};
      WindingOrder::Enum Winding{WindingOrder::CW};
      float Radius{};
      float RingRadius{};
    };

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      return setup;
    }


    IO::Path GetDemoIdTextureName(const int demoId)
    {
      switch (demoId)
      {
      case 0:
      case 2:
      case 3:
        return "Textures/Seamless/Fur/SeamlessFur.png";
      default:
        return "Textures/Seamless/GrassPattern/Seamless.jpg";
      }
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

    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Texture& texture,
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

      return imageCreator.CreateTexture(texture, samplerCreateInfo);
    }


    //! Create the main texture
    Vulkan::VUTexture CreateMainTexture(const std::shared_ptr<IContentManager>& contentManager, const Vulkan::VUDevice& device,
                                        const Vulkan::VUDeviceQueueRecord& deviceQueue, const int demoId)
    {
      auto strPath = GetDemoIdTextureName(demoId);
      auto texture = contentManager->ReadTexture(strPath, PixelFormat::R8G8B8A8_UNORM);
      return CreateTexture(device, deviceQueue, texture, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }

    //! Create the fur 'density' bitmap
    Vulkan::VUTexture CreateFurDensityTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const int demoId,
                                              const Point2& furTextureDim, const float hairDensity, const int layerCount)
    {
      // if (furTextureDim.X != 1024 || furTextureDim.Y != 512)
      if (demoId != 2)
      {
        const std::vector<uint8_t> furBitmapContent = FurTexture::GenerateSmooth(furTextureDim.X, furTextureDim.Y, hairDensity, layerCount);
        const RawBitmap furBitmap(&furBitmapContent[0], furTextureDim.X, furTextureDim.Y, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
        return CreateTexture(device, deviceQueue, furBitmap, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
      }
      const std::vector<uint8_t> furBitmapContent = FurTexture::GenerateWave(furTextureDim.X, furTextureDim.Y, hairDensity, layerCount);
      const RawBitmap furBitmap(&furBitmapContent[0], furTextureDim.X, furTextureDim.Y, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
      return CreateTexture(device, deviceQueue, furBitmap, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);

      // std::string strPath("Density2.png");
      // auto bitmap = contentManager->ReadBitmap(strPath, PixelFormat::R8G8B8A8_UNORM);
      // return CreateTexture(device, deviceQueue, bitmap, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }

    Vulkan::VUTexture CreateMainAtlasTexture(const std::shared_ptr<IContentManager>& contentManager, const Vulkan::VUDevice& device,
                                             const Vulkan::VUDeviceQueueRecord& deviceQueue)
    {
      auto bitmap = contentManager->ReadBitmap("MainAtlas.png", PixelFormat::R8G8B8A8_UNORM);
      return CreateTexture(device, deviceQueue, bitmap, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }

    AtlasTextureInfo CreateMainAtlasTextureInfo(const std::shared_ptr<IContentManager>& contentManager)
    {
      BasicTextureAtlas atlas;
      contentManager->Read(atlas, "MainAtlas.bta");
      return TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Banners");
    }

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t uniformBuffers,
                                                     const uint32_t combinedImageSamplers, const uint32_t maxSets, const uint32_t frameCount)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = uniformBuffers * frameCount;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = combinedImageSamplers * frameCount;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = maxSets * frameCount;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return RapidVulkan::DescriptorPool(device.Get(), descriptorPoolInfo);
    }

    VertexBufferInfo<2> BuildVB(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const BoxF& coords, const BoxF& uv)
    {
      std::array<VertexPositionTexture, 4> vertices = {
        VertexPositionTexture(Vector3(coords.X1, coords.Y2, 0.0f), Vector2(uv.X1, uv.Y2)),
        VertexPositionTexture(Vector3(coords.X1, coords.Y1, 0.0f), Vector2(uv.X1, uv.Y1)),
        VertexPositionTexture(Vector3(coords.X2, coords.Y2, 0.0f), Vector2(uv.X2, uv.Y2)),
        VertexPositionTexture(Vector3(coords.X2, coords.Y1, 0.0f), Vector2(uv.X2, uv.Y1)),
      };

      VertexBufferInfo<2> info;
      info.VertexBuffer.Reset(bufferManager, vertices, Vulkan::VMBufferUsage::STATIC);

      // Generate attribute description by matching shader layout with the vertex declarations
      std::array<VertexElementUsage, 2> shaderAttribOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};
      static_assert(shaderAttribOrder.size() == info.AttributeDescription.size(), "We expect the sizes to match");

      for (std::size_t i = 0; i < info.AttributeDescription.size(); ++i)
      {
        const auto& vertexDeclElement = info.VertexBuffer.GetVertexElement(shaderAttribOrder[i], 0);

        info.AttributeDescription[i].location = UncheckedNumericCast<uint32_t>(i);
        info.AttributeDescription[i].binding = 0;
        info.AttributeDescription[i].format = vertexDeclElement.NativeFormat;
        info.AttributeDescription[i].offset = vertexDeclElement.Offset;
      }

      info.BindingDescription.stride = info.VertexBuffer.GetElementStride();
      info.BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      return info;
    }


    Procedural::BasicMesh CreateMesh(const ProceduralConfig& proceduralConfig, const PxSize2D& tex1Size, const Point2& textureRepeatCount,
                                     const int torusMajorSegments, const int torusMinorSegments, const bool useTriangleStrip)
    {
      TextureRectangle texRect(PxRectangle(0, 0, tex1Size.Width(), tex1Size.Height()), tex1Size);
      const NativeTextureArea texArea(Vulkan::VUTextureUtil::CalcTextureArea(texRect, textureRepeatCount.X, textureRepeatCount.Y));
      if (proceduralConfig.Primitive == ProceduralPrimitive::Box)
      {
        std::array<NativeTextureArea, 6> texAreas = {texArea, texArea, texArea, texArea, texArea, texArea};
        if (useTriangleStrip)
        {
          return Procedural::BoxGenerator::GenerateStrip(Vector3::Zero(), 30, 30, 30, texAreas.data(), UncheckedNumericCast<int32_t>(texAreas.size()),
                                                         proceduralConfig.Winding);
        }
        return Procedural::BoxGenerator::GenerateList(Vector3::Zero(), 30, 30, 30, texAreas.data(), UncheckedNumericCast<int32_t>(texAreas.size()),
                                                      proceduralConfig.Winding);
      }


      if (useTriangleStrip)
      {
        return Procedural::TorusGenerator::GenerateStrip(torusMajorSegments, torusMinorSegments, proceduralConfig.Radius, proceduralConfig.RingRadius,
                                                         texArea, proceduralConfig.Winding);
      }
      return Procedural::TorusGenerator::GenerateList(torusMajorSegments, torusMinorSegments, proceduralConfig.Radius, proceduralConfig.RingRadius,
                                                      texArea, proceduralConfig.Winding);
    }


    VkDescriptorSet CreateDescDescriptorSet(const RapidVulkan::DescriptorPool& descriptorPool,
                                            const RapidVulkan::DescriptorSetLayout& descriptorSetLayout, const Vulkan::VUTexture& texture)
    {
      assert(descriptorPool.IsValid());
      assert(descriptorSetLayout.IsValid());
      assert(texture.IsValid());

      // Allocate a new descriptor set from the global descriptor pool
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool.Get();
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      VkDescriptorSet descriptorSet = nullptr;
      RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets", __FILE__,
                              __LINE__);

      std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};

      // Binding 0 : Fragment shader texture sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].pNext = nullptr;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[0].pImageInfo = &textureImageInfo;

      vkUpdateDescriptorSets(descriptorPool.GetDevice(), UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0,
                             nullptr);

      return descriptorSet;
    }

    RapidVulkan::DescriptorSetLayout CreateDescDescriptorSetLayout(const Vulkan::VUDevice& device)
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


    RapidVulkan::PipelineLayout CreateDescPipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

      return RapidVulkan::PipelineLayout(descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo);
    }

    RapidVulkan::GraphicsPipeline CreateDescPipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                     const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                     const VertexBufferInfo<2>& vertexBufferInfo, const VkRenderPass renderPass)
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
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
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

      VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
      pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

      VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
      depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
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
      graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = 0;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return RapidVulkan::GraphicsPipeline(pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    }
  }

  FurShellRendering::FurShellRendering(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_config(config.GetOptions<OptionParser>()->GetConfig())
    , m_bufferManager(
        std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
    , m_perspectiveZ(400.0f)
    , m_xAngle(0)
    , m_yAngle(MathHelper::ToRadians(270))
    , m_zAngle(0)
    , m_xSpeed(0)
    , m_ySpeed(0)
    , m_zSpeed(0)
    , m_cameraAngleX(0)
    , m_cameraPosY(0)
    , m_gravity(0, -1.0f, 0)
    , m_radians(0.0f)
    , m_enableDepthTest(true)
    , m_enableForce(false)
  {
    const Point2 furTextureDim = m_config.GetFurTextureDimensions();
    const float hairDensity = m_config.GetHairDensity();
    float hairLength = m_config.GetHairLength();
    const int layerCount = m_config.GetLayerCount();

    const Vector4 color(Color(m_config.GetBackgroundColor()).ToVector4());
    m_backgroundColor = Vector3(color.X, color.Y, color.Z);

    auto contentManager = GetContentManager();

    m_resources.Tex1 = CreateMainTexture(contentManager, m_device, m_deviceQueue, m_config.GetDemoId());
    m_resources.Tex2 = CreateFurDensityTexture(m_device, m_deviceQueue, m_config.GetDemoId(), furTextureDim, hairDensity, layerCount);

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, 5, 9, 6, maxFramesInFlight);

    LightInfo lightInfo;
    ProceduralConfig proceduralConfig;
    {
      switch (m_config.GetDemoId())
      {
      case 1:
        proceduralConfig.Winding = WindingOrder::CW;
        proceduralConfig.Radius = 60;
        proceduralConfig.RingRadius = 40;
        lightInfo.Direction = Vector3(0.0f, 0.0f, -1.0f);
        lightInfo.Color = Vector3(0.9f, 0.9f, 0.9f);
        lightInfo.AmbientColor = Vector3(0.2f, 0.2f, 0.2f);
        m_xSpeed = 0.0f;
        m_ySpeed = 0.12f;
        m_zSpeed = 0.14f;
        m_gravity = Vector3(0.0f, -1.0f, 0.0f);
        break;
      case 2:
        proceduralConfig.Primitive = ProceduralPrimitive::Box;
        proceduralConfig.Winding = WindingOrder::CCW;
        proceduralConfig.Radius = 30;
        proceduralConfig.RingRadius = 4;
        lightInfo.Direction = Vector3(0.0f, 0.2f, -1.0f);
        lightInfo.Color = Vector3(0.9f, 0.9f, 0.9f);
        lightInfo.AmbientColor = Vector3(0.4f, 0.4f, 0.4f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.32f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.44f;
        m_gravity = Vector3(0.0f, -0.7f, 0.0f);
        hairLength *= 0.50f;
        break;
      case 3:
        proceduralConfig.Winding = WindingOrder::CCW;
        proceduralConfig.Radius = 30;
        proceduralConfig.RingRadius = 4;
        lightInfo.Direction = Vector3(0.0f, 0.2f, -1.0f);
        lightInfo.Color = Vector3(0.9f, 0.9f, 0.9f);
        lightInfo.AmbientColor = Vector3(0.4f, 0.4f, 0.4f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.32f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.44f;
        m_gravity = Vector3(0.0f, -0.4f, 0.0f);
        hairLength *= 0.50f;
        break;
      case 0:
      default:
        proceduralConfig.Winding = WindingOrder::CCW;
        proceduralConfig.Radius = 30;
        proceduralConfig.RingRadius = 4;
        lightInfo.Direction = Vector3(0.0f, 0.2f, -1.0f);
        lightInfo.Color = Vector3(0.9f, 0.9f, 0.9f);
        lightInfo.AmbientColor = Vector3(0.4f, 0.4f, 0.4f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.0f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.0f;
        m_xAngle = 0.0f;
        m_cameraAngleX = 20 * MathHelper::TO_RADS;
        m_cameraPosY = -0.5f;
        m_gravity = Vector3(0.0f, 0.0f, 0.0f);
        m_enableForce = false;
        hairLength *= 0.50f;
        break;
      }

      Point2 textureRepeatCount(m_config.GetTextureRepeatCountX(), m_config.GetTextureRepeatCountY());
      PxSize2D tex1Size(static_cast<int32_t>(m_resources.Tex1.GetExtent().width), static_cast<int32_t>(m_resources.Tex1.GetExtent().height));
      auto mesh = CreateMesh(proceduralConfig, tex1Size, textureRepeatCount, m_config.GetTorusMajorSegments(), m_config.GetTorusMinorSegments(),
                             m_config.GetUseTriangleStrip());

      // OpenGL ES expects that the index count is <= 0xFFFF
      if (mesh.GetIndexCount() > 0xFFFF)
      {
        throw NotSupportedException("Maximum IndexCount exceeded");
      }

      m_resources.MeshStuff = std::make_unique<MeshStuffRecord>(*contentManager, m_device, m_bufferManager, m_resources.MainDescriptorPool,
                                                                maxFramesInFlight, mesh, m_resources.Tex1, m_resources.Tex2,
                                                                m_config.GetUseHighShaderPrecision(), m_config.GetLightCount(), m_enableDepthTest);
    }


    lightInfo.Direction.Normalize();

    {    // Prepare the shader
      auto& rRender = m_resources.MeshStuff->RenderVB;
      rRender.SetMaxHairLength(hairLength);
      for (int i = 0; i < m_config.GetLightCount(); ++i)
      {
        rRender.SetLightDirection(i, lightInfo.Direction);
        rRender.SetLightColor(i, lightInfo.Color);
      }
      rRender.SetLightAmbientColor(lightInfo.AmbientColor);
    }
    {    // Prepare the shader
      auto& rRender = m_resources.MeshStuff->RenderInstanced;
      rRender.SetMaxHairLength(hairLength);
      for (int i = 0; i < m_config.GetLightCount(); ++i)
      {
        rRender.SetLightDirection(i, lightInfo.Direction);
        rRender.SetLightColor(i, lightInfo.Color);
      }
      rRender.SetLightAmbientColor(lightInfo.AmbientColor);
      rRender.SetInstanceCount(layerCount);
      rRender.SetDrawOpaque(false);
    }
    {    // Prepare the shader
      auto& rRender = m_resources.MeshStuff->RenderInstancedLayer0;
      rRender.SetMaxHairLength(hairLength);
      rRender.SetLightDirection(0, lightInfo.Direction);
      rRender.SetLightColor(0, lightInfo.Color);
      rRender.SetLightAmbientColor(lightInfo.AmbientColor);
      rRender.SetInstanceCount(1);
      rRender.SetDrawOpaque(true);
    }
    {    // Prepare the shader
      auto& rRender = m_resources.MeshStuff->RenderInstancedLayerN;
      rRender.SetMaxHairLength(hairLength);
      rRender.SetLightDirection(0, lightInfo.Direction);
      rRender.SetLightColor(0, lightInfo.Color);
      rRender.SetLightAmbientColor(lightInfo.AmbientColor);
      rRender.SetInstanceCount(layerCount - 1);
      rRender.SetDrawOpaque(false);
    }

    // Build the description VB
    {
      m_resources.TexDescriptionAtlas = CreateMainAtlasTexture(contentManager, m_device, m_deviceQueue);
      m_resources.TexDescription = CreateMainAtlasTextureInfo(contentManager);

      const Vector2 res(config.ScreenResolution.X / 2, config.ScreenResolution.Y / 2);
      const Vector2 atlasSize(static_cast<float>(m_resources.TexDescriptionAtlas.GetExtent().width),
                              static_cast<float>(m_resources.TexDescriptionAtlas.GetExtent().height));

      // texSize.X / tex
      float x1 = -1.0f - (m_resources.TexDescription.OffsetPx.X / res.X);
      float x2 = x1 + (m_resources.TexDescription.TrimmedRectPx.Width / res.X);
      float y1 = -1.0f - (m_resources.TexDescription.OffsetPx.Y / res.Y);
      float y2 = y1 + (m_resources.TexDescription.TrimmedRectPx.Height / res.Y);

      float u1 = m_resources.TexDescription.TrimmedRectPx.Left() / atlasSize.X;
      float v1 = m_resources.TexDescription.TrimmedRectPx.Top() / atlasSize.Y;
      float u2 = m_resources.TexDescription.TrimmedRectPx.Right() / atlasSize.X;
      float v2 = m_resources.TexDescription.TrimmedRectPx.Bottom() / atlasSize.Y;

      m_resources.VBDescription = BuildVB(m_bufferManager, BoxF(x1, y1, x2, y2), BoxF(u1, v1, u2, v2));

      m_resources.BasicProgramVertShader.Reset(m_device.Get(), 0, contentManager->ReadBytes("BasicShader.vert.spv"));
      m_resources.BasicProgramFragShader.Reset(m_device.Get(), 0, contentManager->ReadBytes("BasicShader.frag.spv"));
    }

    m_resources.DescDescriptorSetLayout = CreateDescDescriptorSetLayout(m_device);

    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      // rFrame.UboBuffer = CreateVertexShaderUBO(m_device, sizeof(VertexUBOData));
      rFrame.DescDescriptorSet =
        CreateDescDescriptorSet(m_resources.MainDescriptorPool, m_resources.DescDescriptorSetLayout, m_resources.TexDescriptionAtlas);
    }

    m_resources.DescPipelineLayout = CreateDescPipelineLayout(m_resources.DescDescriptorSetLayout);
  }

  void FurShellRendering::Update(const DemoTime& demoTime)
  {
    if (m_enableForce)
    {
      Vector3 forceDirection(std::sin(m_radians), 0, 0);
      m_displacement = m_gravity + forceDirection;
    }
    else
    {
      m_displacement = m_gravity;
    }

    // m_xAngle = 0;
    // m_yAngle = 0;
    // m_zAngle = 0;
    m_world = Matrix::CreateRotationX(-m_xAngle);
    m_world *= Matrix::CreateRotationY(m_yAngle);
    m_world *= Matrix::CreateRotationZ(m_zAngle);

    const float cameraDistance = 100.0f;
    m_view = Matrix::CreateRotationX(m_cameraAngleX);
    m_view *= Matrix::CreateTranslation(0.0f, m_cameraPosY, -cameraDistance);

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    const auto aspectRatio = GetWindowAspectRatio();
    m_perspective = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectRatio, 1, m_perspectiveZ) * vulkanClipMatrix;
    m_MVP = m_world * m_view * m_perspective;

    m_radians += 1.00f * demoTime.DeltaTime;
    m_xAngle += m_xSpeed * demoTime.DeltaTime;
    m_yAngle += m_ySpeed * demoTime.DeltaTime;
    m_zAngle += m_xSpeed * demoTime.DeltaTime;
  }


  void FurShellRendering::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers,
                                     const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;
    const auto frameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {{m_backgroundColor.X, m_backgroundColor.Y, m_backgroundColor.Z, 1.0f}};
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

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        DrawToCommandBuffer(m_resources.MainFrameResources[frameIndex], hCmdBuffer, currentSwapBufferIndex, frameIndex);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }

  AppDrawResult FurShellRendering::TrySwapBuffers(const DemoTime& demoTime)
  {
    ForceFinishEachFrame();
    return DemoAppVulkanBasic::TrySwapBuffers(demoTime);
  }

  VkRenderPass FurShellRendering::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    m_dependentResources.DescPipeline =
      CreateDescPipeline(m_resources.DescPipelineLayout, context.SwapchainImageExtent, m_resources.BasicProgramVertShader.Get(),
                         m_resources.BasicProgramFragShader.Get(), m_resources.VBDescription, m_dependentResources.MainRenderPass.Get());

    const VkRenderPass renderPass = m_dependentResources.MainRenderPass.Get();
    m_resources.MeshStuff->OnBuildResources(context, renderPass);
    return renderPass;
  }


  void FurShellRendering::OnFreeResources()
  {
    m_resources.MeshStuff->OnFreeResources();
    m_dependentResources = {};
  }

  void FurShellRendering::DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer hCmdBuffer, const uint32_t /*cmdBufferIndex*/,
                                              const uint32_t frameIndex)
  {
    const int layerCount = m_config.GetLayerCount();

    switch (static_cast<RenderMode>(m_config.GetRenderMode()))
    {
    case RenderMode::MultiPass:
      throw NotSupportedException("RenderMode::MultiPass");
    case RenderMode::MultiPassVB:
      DrawMultipass(hCmdBuffer, frameIndex, m_world, m_view, m_perspective, m_displacement, layerCount);
      break;
    case RenderMode::Instanced:
      DrawInstanced(m_resources.MeshStuff->RenderInstanced, hCmdBuffer, frameIndex, m_world, m_view, m_perspective, m_displacement);
      break;
    case RenderMode::Instanced2:
      DrawInstanced(m_resources.MeshStuff->RenderInstancedLayer0, hCmdBuffer, frameIndex, m_world, m_view, m_perspective, m_displacement);
      if (layerCount > 1)
      {
        DrawInstanced(m_resources.MeshStuff->RenderInstancedLayerN, hCmdBuffer, frameIndex, m_world, m_view, m_perspective, m_displacement);
      }
      break;
    default:
      break;
    }

    // Draw normals
    if (m_config.GetShowNormals())
    {
      auto& rRender = m_resources.MeshStuff->RenderNormals;

      rRender.SetWorldViewProjection(m_MVP);

      rRender.Bind(hCmdBuffer, frameIndex);
      rRender.Draw(hCmdBuffer);
      rRender.Unbind();
    }

    {    // Draw the description using the static vertex buffer
      vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.DescPipelineLayout.Get(), 0, 1, &frame.DescDescriptorSet, 0,
                              nullptr);
      vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.DescPipeline.Get());


      VkDeviceSize offsets = 0;
      vkCmdBindVertexBuffers(hCmdBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.VBDescription.VertexBuffer.GetBufferPointer(), &offsets);
      vkCmdDraw(hCmdBuffer, m_resources.VBDescription.VertexBuffer.GetVertexCount(), 1, 0, 0);
    }
  }

  void FurShellRendering::ForceFinishEachFrame()
  {
    if (m_config.GetForceFinishEachFrame())
    {
      vkDeviceWaitIdle(m_device.Get());
    }
  }

  void FurShellRendering::DrawMultipass(const VkCommandBuffer hCmdBuffer, const uint32_t frameIndex, const Matrix& world, const Matrix& view,
                                        const Matrix& perspective, const Vector3& displacement, const int layerCount)
  {
    auto& rRender = m_resources.MeshStuff->RenderVB;

    rRender.SetWorld(world);
    rRender.SetView(view);
    rRender.SetProjection(perspective);
    rRender.SetDisplacement(displacement);

    float layerAdd = (layerCount > 1 ? 1.0f / float(layerCount - 1) : 1.0f);
    float layer = 0.0f;

    rRender.SetDrawOpaque(true);
    rRender.SetCurrentLayer(hCmdBuffer, layer);
    rRender.Bind(hCmdBuffer, frameIndex);
    layer += layerAdd;
    rRender.Draw(hCmdBuffer);
    rRender.Unbind();

    rRender.SetDrawOpaque(false);
    rRender.Bind(hCmdBuffer, frameIndex);
    for (int i = 1; i < layerCount; ++i)
    {
      rRender.SetCurrentLayer(hCmdBuffer, layer);
      rRender.Draw(hCmdBuffer);
      layer += layerAdd;
    }
    rRender.Unbind();
  }

  void FurShellRendering::DrawInstanced(MeshRenderVBInstanced& rRender, const VkCommandBuffer hCmdBuffer, const uint32_t frameIndex,
                                        const Matrix& world, const Matrix& view, const Matrix& perspective, const Vector3& displacement)
  {
    rRender.SetWorld(world);
    rRender.SetView(view);
    rRender.SetProjection(perspective);
    rRender.SetDisplacement(displacement);

    rRender.Bind(hCmdBuffer, frameIndex);
    rRender.Draw(hCmdBuffer);
    rRender.Unbind();
  }
}
