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

#include "OpenCVMatToNativeBatch.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <opencv/cv.hpp>
#include <algorithm>
#include <cassert>

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
    : DemoAppGLES3(config)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(m_graphics->GetNativeBatch2D())    // We just acquire the completely API independent version (see DFNativeBatch2D if you want
                                                       // access to the OpenGLES3 specific methods)
  {
    auto contentManager = GetContentManager();

    CreateFont(contentManager);
    CreateReferenceTextures(contentManager);
    CreateOpenCVTextures(contentManager);
  }


  OpenCVMatToNativeBatch::~OpenCVMatToNativeBatch() = default;


  void OpenCVMatToNativeBatch::Update(const DemoTime& demoTime)
  {
  }


  void OpenCVMatToNativeBatch::Draw(const DemoTime& demoTime)
  {
    // Clear the screen using the API (we dont currently have a API independent way to do it)
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_nativeBatch->Begin(BlendState::Opaque);

    const auto sizeTex1 = m_texTest.GetSize();

    Vector2 dst1;
    Vector2 dst2(0, sizeTex1.Y);

    DrawImages(dst1, m_texTest, m_texTestR, m_texTestG, m_texTestB, "Reference");
    DrawImages(dst2, m_texTestMat, m_texTestMatR, m_texTestMatG, m_texTestMatB, "OpenCV");

    m_nativeBatch->End();
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
    const int32_t heightCaptionRGB = sizeTexR.Y + sizeTexG.Y + sizeTexB.Y + sizeTextCaption.Y;
    const int32_t yAdjust = (sizeTex1.Y - heightCaptionRGB) / 2;

    dstPosR.X += sizeTex1.X;
    dstPosG.X += sizeTex1.X;
    dstPosB.X += sizeTex1.X;

    dstPosR.Y += yAdjust + sizeTextCaption.Y;
    dstPosG.Y += yAdjust + sizeTextCaption.Y + sizeTexR.Y;
    dstPosB.Y += yAdjust + sizeTextCaption.Y + sizeTexR.Y + sizeTexG.Y;

    m_nativeBatch->Draw(tex1, dstPos1, Color::White());
    m_nativeBatch->Draw(texR, dstPosR, Color::White());
    m_nativeBatch->Draw(texG, dstPosG, Color::White());
    m_nativeBatch->Draw(texB, dstPosB, Color::White());

    Vector2 dstPosText1(sizeTex1.X, dstPosition.Y + yAdjust);
    Vector2 dstPosTextR = dstPosR;
    Vector2 dstPosTextG = dstPosG;
    Vector2 dstPosTextB = dstPosB;

    dstPosTextR.X += sizeTexR.X;
    dstPosTextG.X += sizeTexG.X;
    dstPosTextB.X += sizeTexB.X;

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

    m_font.Reset(atlas, fontBasicKerning);

    const auto nativeGraphics = m_graphics->GetNativeGraphics();
    m_texFont.Reset(nativeGraphics, bitmap, Texture2DFilterHint::Smooth);
  }


  void OpenCVMatToNativeBatch::CreateReferenceTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    auto bitmapTest = contentManager->ReadBitmap(TEST_IMAGE1, PixelFormat::R8G8B8_UNORM);
    auto bitmapTestR = contentManager->ReadBitmap(TEST_IMAGE_R, PixelFormat::R8G8B8_UNORM);
    auto bitmapTestG = contentManager->ReadBitmap(TEST_IMAGE_G, PixelFormat::R8G8B8_UNORM);
    auto bitmapTestB = contentManager->ReadBitmap(TEST_IMAGE_B, PixelFormat::R8G8B8_UNORM);

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

    Convert(m_texTestMat, m_convert, nativeGraphics, mat1, PixelFormat::R8G8B8_UNORM, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatR, m_convert, nativeGraphics, matR, PixelFormat::R8G8B8_UNORM, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatG, m_convert, nativeGraphics, matG, PixelFormat::R8G8B8_UNORM, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
    Convert(m_texTestMatB, m_convert, nativeGraphics, matB, PixelFormat::R8G8B8_UNORM, apiPreferredBitmapOrigin, Texture2DFilterHint::Smooth);
  }
}
