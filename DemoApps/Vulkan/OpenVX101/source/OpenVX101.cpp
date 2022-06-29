/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include "OpenVX101.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidOpenVX/Check.hpp>
#include <RapidOpenVX/Context.hpp>
#include <RapidOpenVX/Exceptions.hpp>
#include <RapidOpenVX/Image.hpp>
#include <RapidVulkan/Check.hpp>
#include <VX/vx.h>
#include <VX/vxu.h>
#include <vulkan/vulkan.h>
#include <cmath>

namespace Fsl
{
  namespace
  {
    void CopyImageFromCPUToGPU(RapidOpenVX::Image& rImage, const Bitmap& srcBitmap)
    {
      assert(srcBitmap.GetPixelFormat() == PixelFormat::EX_ALPHA8_UNORM);

      const vx_uint32 imageWidth = srcBitmap.Width();
      const vx_uint32 imageHeight = srcBitmap.Height();

      vx_rectangle_t imageRect = {0, 0, imageWidth, imageHeight};
      vx_imagepatch_addressing_t imageInfo = VX_IMAGEPATCH_ADDR_INIT;
      vx_map_id mapId = 0;
      void* pImageAddress = nullptr;
      RAPIDOPENVX_CHECK(vxMapImagePatch(rImage.Get(), &imageRect, 0, &mapId, &imageInfo, &pImageAddress, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0));

      if ((1 != imageInfo.step_y) || (1 != imageInfo.step_x) || (imageHeight != imageInfo.dim_y) || (imageWidth != imageInfo.dim_x))
      {
        throw std::runtime_error("vx procedure error");
      }

      {
        RawBitmap rawSrcBitmap;
        Bitmap::ScopedDirectAccess scopedAccess(srcBitmap, rawSrcBitmap);
        const auto* pSrcBitmap = static_cast<const uint8_t*>(rawSrcBitmap.Content());
        const auto srcStride = rawSrcBitmap.Stride();

        for (uint32_t y = 0; y < imageHeight; ++y)
        {
          for (uint32_t x = 0; x < imageWidth; ++x)
          {
            auto* pImagePixels = static_cast<uint8_t*>(vxFormatImagePatchAddress2d(pImageAddress, x, y, &imageInfo));
            assert(pImagePixels != nullptr);
            *pImagePixels = pSrcBitmap[(y * srcStride) + x];
          }
        }
      }

      RAPIDOPENVX_CHECK(vxUnmapImagePatch(rImage.Get(), mapId));
    }


    void CopyImagesFromGPUToCPU(Bitmap& dstBitmap, const RapidOpenVX::Image& srcImage1, const RapidOpenVX::Image& srcImage2)
    {
      assert(dstBitmap.GetPixelFormat() == PixelFormat::EX_ALPHA8_UNORM);

      const vx_uint32 imageWidth = dstBitmap.Width();
      const vx_uint32 imageHeight = dstBitmap.Height();
      vx_rectangle_t imageRect = {0, 0, imageWidth, imageHeight};

      // transfer image from gpu to cpu
      vx_imagepatch_addressing_t imageInfo1 = VX_IMAGEPATCH_ADDR_INIT;
      vx_imagepatch_addressing_t imageInfo2 = VX_IMAGEPATCH_ADDR_INIT;
      vx_map_id mapId1 = 0;
      vx_map_id mapId2 = 0;
      void* pImageAddress1 = nullptr;
      void* pImageAddress2 = nullptr;
      RAPIDOPENVX_CHECK(vxMapImagePatch(srcImage1.Get(), &imageRect, 0, &mapId1, &imageInfo1, &pImageAddress1, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0));
      RAPIDOPENVX_CHECK(vxMapImagePatch(srcImage2.Get(), &imageRect, 0, &mapId2, &imageInfo2, &pImageAddress2, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0));

      if ((imageInfo2.dim_y != imageInfo1.dim_y) || (imageInfo2.dim_x != imageInfo1.dim_x) || (imageInfo2.step_y != imageInfo1.step_y) ||
          (imageInfo2.step_x != imageInfo1.step_x) || (imageInfo2.dim_y != imageHeight) || (imageInfo2.dim_x != imageWidth) ||
          (imageInfo2.step_y != 1) || (imageInfo2.step_x != 1))
      {
        throw std::runtime_error("vx procedure error");
      }

      RawBitmapEx rawDstBitmap;
      Bitmap::ScopedDirectAccess scopedAccess(dstBitmap, rawDstBitmap);
      auto* pDstBitmap = static_cast<uint8_t*>(rawDstBitmap.Content());
      const auto dstStride = rawDstBitmap.Stride();

      for (uint32_t y = 0; y < imageHeight; ++y)
      {
        for (uint32_t x = 0; x < imageWidth; ++x)
        {
          const auto* pImagePixel1 = static_cast<const int16_t*>(vxFormatImagePatchAddress2d(pImageAddress1, x, y, &imageInfo1));
          const auto* pImagePixel2 = static_cast<const int16_t*>(vxFormatImagePatchAddress2d(pImageAddress2, x, y, &imageInfo2));

          const float value = static_cast<float>(*pImagePixel1) * static_cast<float>(*pImagePixel1) +
                              static_cast<float>(*pImagePixel2) * static_cast<float>(*pImagePixel2);
          auto dstPixel = static_cast<uint16_t>(std::sqrt(value));
          if (dstPixel > 0xff)
          {
            dstPixel = 0xff;
          }

          pDstBitmap[(y * dstStride) + x] = static_cast<uint8_t>(dstPixel);
        }
      }

      RAPIDOPENVX_CHECK(vxUnmapImagePatch(srcImage1.Get(), mapId1));
      RAPIDOPENVX_CHECK(vxUnmapImagePatch(srcImage2.Get(), mapId2));
    }


    Vulkan::VUTexture ToTexture(Vulkan::VulkanImageCreator& rImageCreator, const Bitmap& srcBitmap, const PixelFormat pixelFormat)
    {
      Bitmap tmpBitmap(srcBitmap);
      BitmapUtil::Convert(tmpBitmap, pixelFormat);


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
      samplerCreateInfo.maxLod = 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return rImageCreator.CreateTexture(tmpBitmap, samplerCreateInfo);
    }
  }


  OpenVX101::OpenVX101(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(std::dynamic_pointer_cast<Vulkan::NativeBatch2D>(m_graphics->GetNativeBatch2D()))
  {
    RapidOpenVX::Context context(RapidOpenVX::ResetMode::Create);

    // Read the image data
    Bitmap bitmap;
    GetContentManager()->Read(bitmap, "Test_gray.png", PixelFormat::EX_ALPHA8_UNORM);

    Vulkan::VulkanImageCreator imageCreator(m_device, m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

    m_texSrc = ToTexture(imageCreator, bitmap, PixelFormat::R8G8B8A8_UNORM);

    const vx_uint32 imageWidth = bitmap.Width();
    const vx_uint32 imageHeight = bitmap.Height();

    RapidOpenVX::Image image0(context.Get(), imageWidth, imageHeight, VX_DF_IMAGE_U8);
    RapidOpenVX::Image image1(context.Get(), imageWidth, imageHeight, VX_DF_IMAGE_S16);
    RapidOpenVX::Image image2(context.Get(), imageWidth, imageHeight, VX_DF_IMAGE_S16);

    CopyImageFromCPUToGPU(image0, bitmap);

    // process image
    RAPIDOPENVX_CHECK(vxuSobel3x3(context.Get(), image0.Get(), image1.Get(), image2.Get()));

    CopyImagesFromGPUToCPU(bitmap, image1, image2);


    FSLLOG3_INFO("vx process success.");

    m_texDst = ToTexture(imageCreator, bitmap, PixelFormat::R8G8B8A8_UNORM);
  }


  OpenVX101::~OpenVX101() = default;


  void OpenVX101::Update(const DemoTime& /*demoTime*/)
  {
  }


  void OpenVX101::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      VkClearColorValue clearColorValue{};
      clearColorValue.float32[0] = 0.0f;
      clearColorValue.float32[1] = 0.0f;
      clearColorValue.float32[2] = 0.0f;
      clearColorValue.float32[3] = 1.0f;

      VkClearValue clearValues = {clearColorValue};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = 1;
      renderPassBeginInfo.pClearValues = &clearValues;

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        const auto resPx = GetWindowSizePx();
        const auto halfWidth = resPx.Width() / 2;
        PxRectangle leftRect(0, 0, halfWidth, resPx.Height());
        PxRectangle rightRect(halfWidth, 0, halfWidth, resPx.Height());

        m_nativeBatch->Begin();
        m_nativeBatch->Draw(m_texSrc, leftRect, Color::White());
        m_nativeBatch->Draw(m_texDst, rightRect, Color::White());
        m_nativeBatch->End();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass OpenVX101::OnBuildResources(const VulkanBasic::BuildResourcesContext& /*context*/)
  {
    // Since we only draw using the NativeBatch we just create the most basic render pass that is compatible
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void OpenVX101::OnFreeResources()
  {
    m_dependentResources = {};
  }
}
