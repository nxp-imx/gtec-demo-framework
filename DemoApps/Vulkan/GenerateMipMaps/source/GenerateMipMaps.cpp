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

#include "GenerateMipMaps.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Texture/TextureMipMapUtil.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include "OptionParser.hpp"

namespace Fsl
{
  namespace
  {
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

    float Clamp(const float value)
    {
      // Basic clamping that works ok at low speeds
      if (value >= MathHelper::ToRadians(360))
      {
        return value - MathHelper::ToRadians(360);
      }
      if (value <= -MathHelper::ToRadians(360))
      {
        return value + MathHelper::ToRadians(360);
      }
      return value;
    }

    Vector3 Clamp(const Vector3& value)
    {
      return {Clamp(value.X), Clamp(value.Y), Clamp(value.Z)};
    }

    struct MipSelectionInfo
    {
      uint32_t FinalMipLod{};
      uint32_t MaxMipMapLevel{};

      MipSelectionInfo() = default;
      MipSelectionInfo(const uint32_t finalMipLod, const uint32_t maxMipMapLevel)
        : FinalMipLod(finalMipLod)
        , MaxMipMapLevel(maxMipMapLevel)
      {
      }
    };

    RapidVulkan::Sampler CreateSampler(const VkDevice device, const float minLod, const float maxLod)
    {
      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
      samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = minLod;
      samplerCreateInfo.maxLod = maxLod;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
      return {device, samplerCreateInfo};
    }

    std::vector<RapidVulkan::Sampler> CreateSamplers(const VkDevice device, const uint32_t maxMipLevels)
    {
      std::vector<RapidVulkan::Sampler> res(maxMipLevels);
      for (uint32_t i = 0; i < maxMipLevels; ++i)
      {
        res[i] = CreateSampler(device, static_cast<float>(i), static_cast<float>(i));
      }
      return res;
    }

    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                    const std::shared_ptr<IContentManager>& contentManager, const uint32_t minMipLod, MipSelectionInfo& rMipInfo)
    {
      Vulkan::VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      Texture texture = contentManager->ReadTexture("Textures/GPUSdk/SquareLogo512x512.jpg", PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::Undefined,
                                                    PixelChannelOrder::Undefined, true);

      const uint32_t numMipMapLevels = texture.GetLevels();
      const uint32_t finalMipLod = MathHelper::Clamp(minMipLod, 0u, numMipMapLevels);

      FSLLOG3_INFO("Texture {}, forcing mipmap level to {} in range 0-{}", texture.GetExtent2D(), finalMipLod,
                   (numMipMapLevels > 0 ? numMipMapLevels - 1 : 0));

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      // samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
      samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = static_cast<float>(finalMipLod);
      samplerCreateInfo.maxLod = static_cast<float>(finalMipLod);
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      rMipInfo = MipSelectionInfo(finalMipLod, numMipMapLevels);
      return imageCreator.CreateTexture(texture, samplerCreateInfo);
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
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

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count;

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


    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& uboBuffer,
                                        const VkDescriptorImageInfo& textureImageInfo)
    {
      assert(uboBuffer.IsValid());

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


    VkDescriptorSet CreateDescriptorSet(const RapidVulkan::DescriptorPool& descriptorPool,
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout, const Vulkan::VUBufferMemory& uboBuffer,
                                        const Vulkan::VUTexture& texture)
    {
      assert(descriptorPool.IsValid());
      assert(descriptorSetLayout.IsValid());
      assert(uboBuffer.IsValid());
      assert(texture.IsValid());

      // Allocate a new descriptor set from the global descriptor pool
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool.Get();
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
      RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets", __FILE__,
                              __LINE__);

      UpdateDescriptorSet(descriptorPool.GetDevice(), descriptorSet, uboBuffer, texture.GetDescriptorImageInfo());

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


    RapidVulkan::RenderPass CreateRenderPass(const VkDevice device, const VkFormat swapchainImageFormat)
    {
      VkAttachmentDescription attachmentDescription{};
      attachmentDescription.flags = 0;
      attachmentDescription.format = swapchainImageFormat;
      attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
      attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      VkAttachmentReference colorAttachmentReference{};
      colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      VkSubpassDependency subpassDependency{};
      subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency.dstSubpass = 0;
      subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency.srcAccessMask = 0;
      subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

      VkSubpassDescription subpassDescription{};
      subpassDescription.flags = 0;
      subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription.inputAttachmentCount = 0;
      subpassDescription.pInputAttachments = nullptr;
      subpassDescription.colorAttachmentCount = 1;
      subpassDescription.pColorAttachments = &colorAttachmentReference;
      subpassDescription.pResolveAttachments = nullptr;
      subpassDescription.pDepthStencilAttachment = nullptr;
      subpassDescription.preserveAttachmentCount = 0;
      subpassDescription.pPreserveAttachments = nullptr;

      return {device, 0, 1, &attachmentDescription, 1, &subpassDescription, 1, &subpassDependency};
    }
  }

  GenerateMipMaps::GenerateMipMaps(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
    , m_bufferManager(
        std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))

  {
    auto options = config.GetOptions<OptionParser>();

    const std::shared_ptr<IContentManager> content = GetContentManager();

    m_resources.MainProgramInfo = CreateProgram(m_device, content);
    MipSelectionInfo mipSelectionInfo{};
    m_resources.Texture = CreateTexture(m_device, m_deviceQueue, content, options->GetMipMapLevel(), mipSelectionInfo);
    m_resources.Samplers = CreateSamplers(m_device.Get(), mipSelectionInfo.MaxMipMapLevel);
    m_resources.MainVertexBufferInfo = CreateVertexBuffer(m_bufferManager);
    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight);
    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.UboBuffer = CreateVertexShaderUBO(m_device, sizeof(VertexUBOData));
      rFrame.DescriptorSet =
        CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout, rFrame.UboBuffer, m_resources.Texture);
      rFrame.mipMapLod = mipSelectionInfo.FinalMipLod;
    }

    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);

    const PxSize2D currentSizePx = GetWindowSizePx();
    const float aspectRatio = static_cast<float>(currentSizePx.RawWidth()) / static_cast<float>(currentSizePx.RawHeight());
    m_matProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(75.0f), aspectRatio, 1.0f, 1000.0f);
    m_matTranslate = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);

    m_ui = CreateUI(mipSelectionInfo.FinalMipLod, mipSelectionInfo.MaxMipMapLevel);
    m_defaultMipMapLevel = mipSelectionInfo.FinalMipLod;
  }


  void GenerateMipMaps::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsHandled() && event.IsPressed())
    {
      switch (event.GetKey())
      {
      case VirtualKey::Space:
        SetDefaultSettings();
        break;
      case VirtualKey::LeftArrow:
        m_ui.Slider->SubValue(1);
        break;
      case VirtualKey::RightArrow:
        m_ui.Slider->AddValue(1);
        break;
      default:
        break;
      }
    }
  }

  // void GenerateMipMaps::OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final
  //{
  //  if (theEvent->GetSource() == m_ui.Slider)
  //  {

  //  }
  //}

  void GenerateMipMaps::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_ui.BtnDefault)
    {
      SetDefaultSettings();
    }
  }


  void GenerateMipMaps::Update(const DemoTime& demoTime)
  {
    m_angle.X -= 0.30f * demoTime.DeltaTime;
    m_angle.Y -= 0.25f * demoTime.DeltaTime;
    m_angle.Z -= 0.20f * demoTime.DeltaTime;

    m_angle = Clamp(m_angle);

    // Rotate and translate the model view matrix
    m_matModel = Matrix::CreateRotationX(m_angle.X) * Matrix::CreateRotationY(m_angle.Y) * Matrix::CreateRotationZ(m_angle.Z) * m_matTranslate;
  }


  void GenerateMipMaps::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    FSL_PARAM_NOT_USED(demoTime);

    const auto currentFrameIndex = drawContext.CurrentFrameIndex;
    assert(currentFrameIndex < m_resources.MainFrameResources.size());

    // Upload the changes
    VertexUBOData buffer;
    buffer.MatModelView = m_matModel;
    buffer.MatProj = m_matProj;
    m_resources.MainFrameResources[currentFrameIndex].UboBuffer.Upload(0, &buffer, sizeof(VertexUBOData));

    // Update the descriptor set when the mip lod changes
    const uint32_t currentMipLod = m_ui.Slider->GetValue();
    if (m_resources.MainFrameResources[currentFrameIndex].mipMapLod != currentMipLod)
    {
      m_resources.MainFrameResources[currentFrameIndex].mipMapLod = currentMipLod;
      VkDescriptorImageInfo textureImageInfo = m_resources.Texture.GetDescriptorImageInfo();
      textureImageInfo.sampler = m_resources.Samplers[currentMipLod].Get();
      UpdateDescriptorSet(m_device.Get(), m_resources.MainFrameResources[currentFrameIndex].DescriptorSet,
                          m_resources.MainFrameResources[currentFrameIndex].UboBuffer, textureImageInfo);
    }

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
        DrawToCommandBuffer(m_resources.MainFrameResources[currentFrameIndex], hCmdBuffer);

        // Calling this last allows the UI to draw on top of everything.
        m_uiExtension->Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass GenerateMipMaps::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateRenderPass(m_device.Get(), context.SwapchainImageFormat);
    m_dependentResources.Pipeline = CreatePipeline(
      m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.MainProgramInfo.VertexShaderModule.Get(),
      m_resources.MainProgramInfo.FragmentShaderModule.Get(), m_resources.MainVertexBufferInfo, m_dependentResources.MainRenderPass.Get());
    return m_dependentResources.MainRenderPass.Get();
  }


  void GenerateMipMaps::OnFreeResources()
  {
    m_dependentResources.Reset();
  }

  void GenerateMipMaps::SetDefaultSettings()
  {
    m_ui.Slider->SetValue(m_defaultMipMapLevel);
  }


  GenerateMipMaps::UIRecord GenerateMipMaps::CreateUI(const uint32_t currentMipMapLevel, const uint32_t numMipMapLevels)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    auto& uiFactory = *uiControlFactory;

    auto labelSlider = uiFactory.CreateLabel("MipMap level: ");
    labelSlider->SetAlignmentY(UI::ItemAlignment::Center);

    auto slider = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal,
                                                 ConstrainedValue<uint32_t>(currentMipMapLevel, 0, (numMipMapLevels > 0 ? numMipMapLevels - 1 : 0)));
    slider->SetAlignmentY(UI::ItemAlignment::Center);

    auto btnDefault = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Set defaults");

    auto bottomGrid = std::make_shared<UI::GridLayout>(context);
    bottomGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
    bottomGrid->SetAlignmentY(UI::ItemAlignment::Stretch);
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 10));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 10));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddChild(labelSlider, 1, 0);
    bottomGrid->AddChild(slider, 2, 0);
    bottomGrid->AddChild(btnDefault, 3, 0);

    auto bottomBar = uiFactory.CreateBottomBar(bottomGrid);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(bottomBar);
    return {slider, btnDefault};
  }

  void GenerateMipMaps::DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    // auto sizePx = GetWindowSizePx();

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &frame.DescriptorSet, 0,
                            nullptr);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.Pipeline.Get());

    DrawCube(frame, commandBuffer, m_resources.MainProgramInfo, m_matModel);
  }


  void GenerateMipMaps::DrawCube(const FrameResources& frame, const VkCommandBuffer commandBuffer, const ProgramInfo& programInfo,
                                 const Matrix& matModel)
  {
    FSL_PARAM_NOT_USED(frame);
    FSL_PARAM_NOT_USED(programInfo);
    FSL_PARAM_NOT_USED(matModel);

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.MainVertexBufferInfo.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(commandBuffer, m_resources.MainVertexBufferInfo.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  GenerateMipMaps::ProgramInfo GenerateMipMaps::CreateProgram(const Vulkan::VUDevice& device, const std::shared_ptr<IContentManager>& contentManager)
  {
    ProgramInfo info;
    info.VertexShaderModule.Reset(device.Get(), 0, contentManager->ReadBytes("Shader.vert.spv"));
    info.FragmentShaderModule.Reset(device.Get(), 0, contentManager->ReadBytes("Shader.frag.spv"));
    return info;
  }


  GenerateMipMaps::VertexBufferInfo GenerateMipMaps::CreateVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager)
  {
    VertexBufferInfo info;
    info.VertexBuffer.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(g_vertices), Vulkan::VMBufferUsage::STATIC);

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

  RapidVulkan::GraphicsPipeline GenerateMipMaps::CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                                const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                                const VertexBufferInfo& vertexBufferInfo, const VkRenderPass renderPass)
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

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.flags = 0;
    pipelineDynamicStateCreateInfo.dynamicStateCount = 0;
    pipelineDynamicStateCreateInfo.pDynamicStates = nullptr;

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
    graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = 0;

    return {pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo};
  }
}
