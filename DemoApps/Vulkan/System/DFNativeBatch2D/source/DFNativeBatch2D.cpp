/****************************************************************************************************************************************************
 * Copyright 2018, 2023 NXP
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

#include "DFNativeBatch2D.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  DFNativeBatch2D::DFNativeBatch2D(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(std::dynamic_pointer_cast<Vulkan::NativeBatch2D>(m_graphics->GetNativeBatch2D()))
  {
    if (!m_nativeBatch)
    {
      throw NotSupportedException("INativeBatch not backed by the expected type");
    }

    m_scene1 = std::make_shared<TextureAtlasScene>(config.DemoServiceProvider, SpriteNativeAreaCalc(false), 160);

    const std::shared_ptr<IContentManager> contentManager = GetContentManager();
    {
      Fsl::Bitmap bitmap;
      contentManager->Read(bitmap, "Test/Test.png", PixelFormat::R8G8B8A8_UNORM);
      m_texture2D.Reset(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Nearest);
    }
    {
      Vulkan::VulkanImageCreator imageCreator(m_device, m_deviceQueue.Queue, m_deviceQueue.QueueIndex);

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      // samplerCreateInfo.flags;
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
      samplerCreateInfo.maxLod = 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      Fsl::Bitmap bitmap;
      contentManager->Read(bitmap, "Test/Test2.png", PixelFormat::R8G8B8A8_UNORM);
      m_nativeTexture = imageCreator.CreateTexture(bitmap, samplerCreateInfo);
    }
  }


  void DFNativeBatch2D::OnKeyEvent(const KeyEvent& event)
  {
    if (m_scene1)
    {
      m_scene1->OnKeyEvent(event);
    }
  }


  void DFNativeBatch2D::Update(const DemoTime& demoTime)
  {
    if (m_scene1)
    {
      m_scene1->Update(demoTime);
    }
  }


  void DFNativeBatch2D::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers,
                                   const VulkanBasic::DrawContext& drawContext)
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
        const std::shared_ptr<INativeGraphics> native = m_graphics->GetNativeGraphics();
        const PxSize2D windowSizePx = GetWindowSizePx();
        const PxValue offsetY = m_scene1->GetGridOffsetY().Value();

        m_nativeBatch->Begin(BlendState::Opaque);

        auto nativeTexExtent = m_nativeTexture.GetExtent();
        const Point2 nativeTexSize(UncheckedNumericCast<int32_t>(nativeTexExtent.width), UncheckedNumericCast<int32_t>(nativeTexExtent.height));

        constexpr PxSize1D size256Px = PxSize1D::Create(256);
        constexpr PxSize1D size2Px = PxSize1D::Create(2);

        // Vulkan native texture handle
        // While VUTextures support automatic conversion to Vulkan::VUTextureInfo, this shows how to fill out a VUTextureInfo manually
        auto textureInfo = Vulkan::VUTextureInfo(m_nativeTexture.Sampler().Get(), m_nativeTexture.ImageView().Get(),
                                                 m_nativeTexture.Image().GetImageLayout(), m_nativeTexture.Image().GetExtent());
        m_nativeBatch->Draw(textureInfo, PxRectangle(windowSizePx.Width() - size256Px, offsetY + size256Px, size256Px, size256Px), Color::White());

        // Vulkan native texture
        m_nativeBatch->Draw(m_nativeTexture, Vector2(windowSizePx.RawWidth() - nativeTexSize.X - 128 * 2, offsetY.Value), Color::White());
        m_nativeBatch->Draw(m_nativeTexture,
                            PxRectangle::Create(windowSizePx.RawWidth() - nativeTexSize.X - (128 * 2), offsetY.Value + (256 * 0) + 128,
                                                nativeTexSize.X / 2, nativeTexSize.Y / 2),
                            Color::White());

        // API independent texture
        const auto texSize = m_texture2D.GetSize();
        m_nativeBatch->Draw(m_texture2D, Vector2(windowSizePx.RawWidth() - texSize.RawWidth(), offsetY.Value), Color::White());
        m_nativeBatch->Draw(m_texture2D,
                            PxRectangle(windowSizePx.Width() - texSize.Width() - (texSize.Width() / size2Px), offsetY + texSize.Height(),
                                        texSize.Width() / size2Px, texSize.Height() / size2Px),
                            Color::White());

        m_nativeBatch->End();


        // Draw native API independent graphics
        if (m_scene1)
        {
          m_scene1->Draw(windowSizePx);
        }

        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);

      // m_swapchain.CmdPipelineBarrier(hCmdBuffer, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      //                               VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }

  VkRenderPass DFNativeBatch2D::OnBuildResources(const VulkanBasic::BuildResourcesContext& /*context*/)
  {
    // Since we only draw using the NativeBatch we just create the most basic render pass that is compatible
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }

  void DFNativeBatch2D::OnFreeResources()
  {
    m_dependentResources = {};
  }
}
