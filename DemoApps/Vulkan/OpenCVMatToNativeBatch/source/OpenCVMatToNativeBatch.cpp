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

// Included first since OpenCV currently has a conflict with the X11 header, so this is a workaround for that
#include <opencv2/opencv.hpp>
// Normal includes that dont conflict
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <cassert>
#include "OpenCVMatToNativeBatch.hpp"

namespace Fsl
{
  namespace
  {
    const auto TEST_IMAGE1 = "Image1.png";
    const auto TEST_IMAGE_R = "TestRed.png";
    const auto TEST_IMAGE_G = "TestGreen.png";
    const auto TEST_IMAGE_B = "TestBlue.png";

    cv::Mat SafeImread(const IO::Path& path)
    {
      cv::Mat srcImage = cv::imread(path.ToUTF8String());
      if (srcImage.data == nullptr)
      {
        throw GraphicsException("Failed to load image");
      }
      return srcImage;
    }


    // WARNING: Use the very limited example converter to convert the source image.
    //          We should add a proper cv::Mat -> Bitmap converter in the future.
    void Convert(Texture2D& rDstTexture, BasicImageConvert& rConvert, const std::shared_ptr<INativeGraphics>& nativeGraphics, const cv::Mat& srcImage,
                 const PixelFormat desiredPixelFormat, const BitmapOrigin desiredBitmapOrigin, const Texture2DFilterHint textureFilterHint)
    {
      const Bitmap srcBitmap = rConvert.Convert(srcImage, desiredPixelFormat, desiredBitmapOrigin);
      // Convert the bitmap to a API independent Texture2D
      rDstTexture.Reset(nativeGraphics, srcBitmap, textureFilterHint);
    }
  }


  OpenCVMatToNativeBatch::OpenCVMatToNativeBatch(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(m_graphics->GetNativeBatch2D())    // We just acquire the completely API independent version (see DFNativeBatch2D if you want
                                                       // access to the Vulkan specific methods)
  {
    auto contentManager = GetContentManager();

    CreateFont(contentManager);
    CreateReferenceTextures(contentManager);
    CreateOpenCVTextures(contentManager);
  }


  OpenCVMatToNativeBatch::~OpenCVMatToNativeBatch() = default;


  void OpenCVMatToNativeBatch::Update(const DemoTime& /*demoTime*/)
  {
  }


  void OpenCVMatToNativeBatch::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers,
                                          const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      VkClearColorValue clearColorValue{};
      clearColorValue.float32[0] = 0.5f;
      clearColorValue.float32[1] = 0.5f;
      clearColorValue.float32[2] = 0.5f;
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
        m_nativeBatch->Begin(BlendState::Opaque);

        const auto sizeTex1 = m_texTest.GetSize();

        Vector2 dst1;
        Vector2 dst2(0, sizeTex1.RawHeight());

        DrawImages(dst1, m_texTest, m_texTestR, m_texTestG, m_texTestB, "Reference");
        DrawImages(dst2, m_texTestMat, m_texTestMatR, m_texTestMatG, m_texTestMatB, "OpenCV");

        m_nativeBatch->End();


        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass OpenCVMatToNativeBatch::OnBuildResources(const VulkanBasic::BuildResourcesContext& /*context*/)
  {
    // Since we only draw using the NativeBatch we just create the most basic render pass that is compatible
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void OpenCVMatToNativeBatch::OnFreeResources()
  {
    m_dependentResources = {};
  }


  void OpenCVMatToNativeBatch::DrawImages(const Vector2& dstPosition, const Texture2D& tex1, const Texture2D& texR, const Texture2D& texG,
                                          const Texture2D& texB, const char* const pszCaption)
  {
    assert(pszCaption != nullptr);

    Vector2 dstPos1 = dstPosition;
    Vector2 dstPosR = dstPosition;
    Vector2 dstPosG = dstPosition;
    Vector2 dstPosB = dstPosition;

    const auto sizeTextCaption = m_font.MeasureString(pszCaption);

    const auto sizeTex1 = tex1.GetSize();
    const auto sizeTexR = texR.GetSize();
    const auto sizeTexG = texG.GetSize();
    const auto sizeTexB = texB.GetSize();

    // Show the reference color images centered just after the image
    const PxSize1D heightCaptionRGB = sizeTexR.Height() + sizeTexG.Height() + sizeTexB.Height() + sizeTextCaption.Height();
    const PxValue yAdjust = (sizeTex1.Height() - heightCaptionRGB) / PxValue::Create(2);

    const auto sizeTex1Width = static_cast<float>(sizeTex1.RawWidth());
    dstPosR.X += sizeTex1Width;
    dstPosG.X += sizeTex1Width;
    dstPosB.X += sizeTex1Width;

    dstPosR.Y += static_cast<float>((yAdjust + sizeTextCaption.Height()).Value);
    dstPosG.Y += static_cast<float>((yAdjust + sizeTextCaption.Height() + sizeTexR.Height()).Value);
    dstPosB.Y += static_cast<float>((yAdjust + sizeTextCaption.Height() + sizeTexR.Height() + sizeTexG.Height()).Value);

    m_nativeBatch->Draw(tex1, dstPos1, Color::White());
    m_nativeBatch->Draw(texR, dstPosR, Color::White());
    m_nativeBatch->Draw(texG, dstPosG, Color::White());
    m_nativeBatch->Draw(texB, dstPosB, Color::White());

    Vector2 dstPosText1(sizeTex1.RawWidth(), dstPosition.Y + static_cast<float>(yAdjust.Value));
    Vector2 dstPosTextR = dstPosR;
    Vector2 dstPosTextG = dstPosG;
    Vector2 dstPosTextB = dstPosB;

    dstPosTextR.X += static_cast<float>(sizeTexR.RawWidth());
    dstPosTextG.X += static_cast<float>(sizeTexG.RawWidth());
    dstPosTextB.X += static_cast<float>(sizeTexB.RawWidth());

    // BEWARE its very bad for render performance to switch blend state before we finish rendering all the Opaque elements,
    // but its simpler in this example, so we reset the state back after we rendered the text

    m_nativeBatch->ChangeTo(BlendState::NonPremultiplied);

    m_nativeBatch->DrawString(m_texFont, m_font, pszCaption, dstPosText1, Color::White());
    m_nativeBatch->DrawString(m_texFont, m_font, "Red", dstPosTextR, Color::White());
    m_nativeBatch->DrawString(m_texFont, m_font, "Green", dstPosTextG, Color::White());
    m_nativeBatch->DrawString(m_texFont, m_font, "Blue", dstPosTextB, Color::White());

    m_nativeBatch->ChangeTo(BlendState::Opaque);
  }


  void OpenCVMatToNativeBatch::CreateFont(const std::shared_ptr<IContentManager>& contentManager)
  {
    Fsl::Bitmap bitmap = contentManager->ReadBitmap("MainAtlas.png", PixelFormat::R8G8B8A8_UNORM);

    BasicTextureAtlas atlas;
    contentManager->Read(atlas, "MainAtlas.bta");

    BasicFontKerning fontBasicKerning;
    contentManager->Read(fontBasicKerning, "MainAtlas.fbk");

    SpriteNativeAreaCalc spriteNativeAreaCalc(false);
    m_font.Reset(spriteNativeAreaCalc, bitmap.GetExtent(), atlas, fontBasicKerning, 160);

    const auto nativeGraphics = m_graphics->GetNativeGraphics();
    m_texFont.Reset(nativeGraphics, bitmap, Texture2DFilterHint::Smooth);
  }


  void OpenCVMatToNativeBatch::CreateReferenceTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    auto bitmapTest = contentManager->ReadBitmap(TEST_IMAGE1, PixelFormat::R8G8B8A8_UNORM);
    auto bitmapTestR = contentManager->ReadBitmap(TEST_IMAGE_R, PixelFormat::R8G8B8A8_UNORM);
    auto bitmapTestG = contentManager->ReadBitmap(TEST_IMAGE_G, PixelFormat::R8G8B8A8_UNORM);
    auto bitmapTestB = contentManager->ReadBitmap(TEST_IMAGE_B, PixelFormat::R8G8B8A8_UNORM);

    const auto nativeGraphics = m_graphics->GetNativeGraphics();

    // Convert the bitmap to a API independent Texture2D
    m_texTest.Reset(nativeGraphics, bitmapTest, Texture2DFilterHint::Smooth);
    m_texTestR.Reset(nativeGraphics, bitmapTestR, Texture2DFilterHint::Smooth);
    m_texTestG.Reset(nativeGraphics, bitmapTestG, Texture2DFilterHint::Smooth);
    m_texTestB.Reset(nativeGraphics, bitmapTestB, Texture2DFilterHint::Smooth);
  }


  void OpenCVMatToNativeBatch::CreateOpenCVTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    const IO::Path contentPath = contentManager->GetContentPath();

    IO::Path pathImg1 = IO::Path::Combine(contentPath, TEST_IMAGE1);
    IO::Path pathImgR = IO::Path::Combine(contentPath, TEST_IMAGE_R);
    IO::Path pathImgG = IO::Path::Combine(contentPath, TEST_IMAGE_G);
    IO::Path pathImgB = IO::Path::Combine(contentPath, TEST_IMAGE_B);

    // Read a image using OpenCV (we use imread since we want to demonstrate how to convert a OpenCV image to a texture for use by INativeBatch2D)
    const cv::Mat mat1 = SafeImread(pathImg1);
    const cv::Mat matR = SafeImread(pathImgR);
    const cv::Mat matG = SafeImread(pathImgG);
    const cv::Mat matB = SafeImread(pathImgB);

    // Ask the content manager what bitmap origin the current API prefers to skip unnecessary bitmap conversions when creating the texture
    const auto apiPreferredBitmapOrigin = contentManager->GetPreferredBitmapOrigin();
    const auto nativeGraphics = m_graphics->GetNativeGraphics();

    Convert(m_texTestMat, m_convert, nativeGraphics, mat1, PixelFormat::R8G8B8A8_UNORM, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatR, m_convert, nativeGraphics, matR, PixelFormat::R8G8B8A8_UNORM, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatG, m_convert, nativeGraphics, matG, PixelFormat::R8G8B8A8_UNORM, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatB, m_convert, nativeGraphics, matB, PixelFormat::R8G8B8A8_UNORM, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
  }
}
