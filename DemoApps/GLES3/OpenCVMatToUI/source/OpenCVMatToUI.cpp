/****************************************************************************************************************************************************
* Copyright 2017 NXP
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

#include "OpenCVMatToUI.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Extended/Texture2DImage.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <GLES3/gl3.h>
#include <opencv/cv.hpp>

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
      const cv::Mat srcImage = cv::imread(path.ToUTF8String());
      if (!srcImage.data)
        throw GraphicsException("Failed to load image");
      return srcImage;
    }


    // WARNING: Use the very limited example converter to convert the source image.
    //          We should add a proper cv::Mat -> Bitmap converter in the future.
    void Convert(Texture2D& rDstTexture, BasicImageConvert& rConvert, const std::shared_ptr<INativeGraphics>& nativeGraphics,
                 const cv::Mat& srcImage, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredBitmapOrigin,
                 const Texture2DFilterHint textureFilterHint)
    {
      const Bitmap srcBitmap = rConvert.Convert(srcImage, desiredPixelFormat, desiredBitmapOrigin);
      // Convert the bitmap to a API independent Texture2D
      rDstTexture.Reset(nativeGraphics, srcBitmap, textureFilterHint);
    }


    std::shared_ptr<UI::BaseWindow> CreateTextureAndText(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& tex, const std::string& strCaption)
    {
      const auto image = std::make_shared<UI::Texture2DImage>(context);
      image->SetContent(tex);

      const auto label = std::make_shared<UI::Label>(context);
      label->SetContent(strCaption);

      auto stack = std::make_shared<UI::StackLayout>(context);
      stack->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
      stack->AddChild(image);
      stack->AddChild(label);
      return stack;
    }

    std::shared_ptr<UI::BaseWindow> CreateImageGroup(const std::shared_ptr<UI::WindowContext>& context,
                                                     const Texture2D& tex1, const Texture2D& texR, const Texture2D& texG, const Texture2D& texB,
                                                     const std::string& strCaption)
    {
      const auto image1 = std::make_shared<UI::Texture2DImage>(context);
      image1->SetContent(tex1);

      const auto label = std::make_shared<UI::Label>(context);
      label->SetContent(strCaption);

      const auto controlRed = CreateTextureAndText(context, texR, "Red");
      const auto controlGreen = CreateTextureAndText(context, texG, "Green");
      const auto controlBlue = CreateTextureAndText(context, texB, "Blue");

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
    : DemoAppGLES3(config)
    , m_uiEventListener(this)                                                                                     // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))   // Prepare the extension
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(m_graphics->GetNativeBatch2D())  // We just acquire the completely API independent version (see DFNativeBatch2D if you want access to the OpenGLES3 specific methods)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);


    auto contentManager = GetContentManager();

    CreateReferenceTextures(contentManager);
    CreateOpenCVTextures(contentManager);

    CreateUI();
  }


  OpenCVMatToUI::~OpenCVMatToUI()
  {

  }


  void OpenCVMatToUI::Update(const DemoTime& demoTime)
  {
  }


  void OpenCVMatToUI::Draw(const DemoTime& demoTime)
  {
    // Clear the screen using the API (we dont currently have a API independent way to do it)
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }


  void OpenCVMatToUI::CreateReferenceTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    auto bitmapTest = contentManager->ReadBitmap(TEST_IMAGE1, PixelFormat::R8G8B8_UINT);
    auto bitmapTestR = contentManager->ReadBitmap(TEST_IMAGE_R, PixelFormat::R8G8B8_UINT);
    auto bitmapTestG = contentManager->ReadBitmap(TEST_IMAGE_G, PixelFormat::R8G8B8_UINT);
    auto bitmapTestB = contentManager->ReadBitmap(TEST_IMAGE_B, PixelFormat::R8G8B8_UINT);

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

    Convert(m_texTestMat, m_convert, nativeGraphics, mat1, PixelFormat::R8G8B8_UINT, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatR, m_convert, nativeGraphics, matR, PixelFormat::R8G8B8_UINT, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatG, m_convert, nativeGraphics, matG, PixelFormat::R8G8B8_UINT, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatB, m_convert, nativeGraphics, matB, PixelFormat::R8G8B8_UINT, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
  }


  void OpenCVMatToUI::CreateUI()
  {
    auto context = m_uiExtension->GetContext();

    const auto group1 = CreateImageGroup(context, m_texTest, m_texTestR, m_texTestG, m_texTestB, "Reference");
    const auto group2 = CreateImageGroup(context, m_texTestMat, m_texTestMatR, m_texTestMatG, m_texTestMatB, "OpenCV");

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
    context->WindowManager->Add(fillLayout);
  }
}
