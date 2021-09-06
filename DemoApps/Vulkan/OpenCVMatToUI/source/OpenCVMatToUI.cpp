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

// Included first since OpenCV currently has a conflict with the X11 header, so this is a workaround for that
#include <opencv2/opencv.hpp>
// Normal includes that dont conflict
#include "OpenCVMatToUI.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/ICustomSpriteResourceManager.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <vulkan/vulkan.h>

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


    std::shared_ptr<UI::BaseWindow> CreateTextureAndText(const std::shared_ptr<UI::WindowContext>& context,
                                                         ICustomSpriteResourceManager& customSpriteResourceManager, const Texture2D& tex,
                                                         const std::string& strCaption)
    {
      auto sprite = customSpriteResourceManager.CreateCustomTextureSprite(tex.TryGetNative(), BlendState::AlphaBlend);

      const auto image = std::make_shared<UI::Image>(context);
      image->SetContent(sprite);

      const auto label = std::make_shared<UI::Label>(context);
      label->SetContent(strCaption);
      label->SetAlignmentY(UI::ItemAlignment::Center);

      auto stack = std::make_shared<UI::StackLayout>(context);
      stack->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
      stack->AddChild(image);
      stack->AddChild(label);
      return stack;
    }

    std::shared_ptr<UI::BaseWindow> CreateImageGroup(const std::shared_ptr<UI::WindowContext>& context,
                                                     ICustomSpriteResourceManager& customSpriteResourceManager, const Texture2D& tex1,
                                                     const Texture2D& texR, const Texture2D& texG, const Texture2D& texB,
                                                     const std::string& strCaption)
    {
      auto sprite1 = customSpriteResourceManager.CreateCustomTextureSprite(tex1.TryGetNative(), BlendState::AlphaBlend);

      const auto image1 = std::make_shared<UI::Image>(context);
      image1->SetContent(sprite1);

      const auto label = std::make_shared<UI::Label>(context);
      label->SetContent(strCaption);

      const auto controlRed = CreateTextureAndText(context, customSpriteResourceManager, texR, "Red");
      const auto controlGreen = CreateTextureAndText(context, customSpriteResourceManager, texG, "Green");
      const auto controlBlue = CreateTextureAndText(context, customSpriteResourceManager, texB, "Blue");

      auto colorsStackLayout = std::make_shared<UI::StackLayout>(context);
      colorsStackLayout->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
      colorsStackLayout->SetAlignmentY(UI::ItemAlignment::Center);
      colorsStackLayout->AddChild(label);
      colorsStackLayout->AddChild(controlRed);
      colorsStackLayout->AddChild(controlGreen);
      colorsStackLayout->AddChild(controlBlue);

      auto rowStackLayout = std::make_shared<UI::StackLayout>(context);
      rowStackLayout->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
      rowStackLayout->AddChild(image1);
      rowStackLayout->AddChild(colorsStackLayout);
      return rowStackLayout;
    }
  }


  OpenCVMatToUI::OpenCVMatToUI(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(m_graphics->GetNativeBatch2D())    // We just acquire the completely API independent version (see DFNativeBatch2D if you want
                                                       // access to the Vulkan specific methods)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);


    auto contentManager = GetContentManager();

    CreateReferenceTextures(contentManager);
    CreateOpenCVTextures(contentManager);

    CreateUI();
  }


  OpenCVMatToUI::~OpenCVMatToUI() = default;


  void OpenCVMatToUI::Update(const DemoTime& /*demoTime*/)
  {
  }


  void OpenCVMatToUI::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
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
        // Render the UI
        m_uiExtension->Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass OpenCVMatToUI::OnBuildResources(const VulkanBasic::BuildResourcesContext& /*context*/)
  {
    // Since we only draw using the NativeBatch we just create the most basic render pass that is compatible
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void OpenCVMatToUI::OnFreeResources()
  {
    m_dependentResources = {};
  }


  void OpenCVMatToUI::CreateReferenceTextures(const std::shared_ptr<IContentManager>& contentManager)
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


  void OpenCVMatToUI::CreateOpenCVTextures(const std::shared_ptr<IContentManager>& contentManager)
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


  void OpenCVMatToUI::CreateUI()
  {
    ICustomSpriteResourceManager& customSpriteResourceManager = m_uiExtension->GetCustomSpriteResourceManager();
    auto context = m_uiExtension->GetContext();

    const auto group1 = CreateImageGroup(context, customSpriteResourceManager, m_texTest, m_texTestR, m_texTestG, m_texTestB, "Reference");
    const auto group2 = CreateImageGroup(context, customSpriteResourceManager, m_texTestMat, m_texTestMatR, m_texTestMatG, m_texTestMatB, "OpenCV");

    auto stackLayout = std::make_shared<UI::StackLayout>(context);
    stackLayout->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
    stackLayout->SetAlignmentX(UI::ItemAlignment::Center);
    stackLayout->SetAlignmentY(UI::ItemAlignment::Center);
    stackLayout->AddChild(group1);
    stackLayout->AddChild(group2);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<UI::FillLayout>(context);
    fillLayout->AddChild(stackLayout);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(fillLayout);
  }
}
