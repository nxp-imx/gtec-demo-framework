/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES2/DynamicNativeTexture2D.hpp>
#include <utility>
#include "GausianHelper.hpp"
#include "BlurredScene.hpp"
#include "ReferenceOnePassBlurredDraw.hpp"
#include "ReferenceTwoPassBlurredDraw.hpp"
#include "ReferenceTwoPassLinearBlurredDraw.hpp"
#include "TwoPassLinearScaledBlurredDraw.hpp"
#include "OptionParser.hpp"
#include "VBHelper.hpp"

namespace Fsl
{
  using namespace GLES2;


  // FIX: the way the setup is done currently we cant change the framebuffer size (since the 'scene' thinks is using the fullscreen)

  BlurredScene::BlurredScene(const DemoAppConfig& config, std::shared_ptr<AScene> scene)
    : m_config(config.GetOptions<OptionParser>()->GetConfig())
    , m_scene(std::move(scene))
    , m_graphicsService(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_batch2D(m_graphicsService->GetNativeBatch2D())
    , m_screenResolution(config.ScreenResolution)
    , m_currentIndex(0)
    , m_hTexAtlas(0)
  {
    // length 17
    // sigma 4


    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();

    m_program.Reset(contentManager->ReadAllText("BasicShader.vert"), contentManager->ReadAllText("BasicShader.frag"));

    if (m_config.CompareEnabled)
    {
      if ((m_config.CRMFlags & (0x01)) != 0)
      {
        m_blurImplementations.push_back(std::shared_ptr<ABlurredDraw>(new ReferenceTwoPassBlurredDraw(config, m_config)));
      }
      if ((m_config.CRMFlags & 0x02) != 0)
      {
        m_blurImplementations.push_back(std::shared_ptr<ABlurredDraw>(new ReferenceTwoPassLinearBlurredDraw(config, m_config)));
      }
      if ((m_config.CRMFlags & 0x04) != 0)
      {
        m_blurImplementations.push_back(std::shared_ptr<ABlurredDraw>(new TwoPassLinearScaledBlurredDraw(config, m_config)));
      }
      if ((m_config.CRMFlags & 0x08) != 0)
      {
        m_blurImplementations.push_back(std::shared_ptr<ABlurredDraw>(new ReferenceOnePassBlurredDraw(config, m_config)));
      }
    }
    else
    {
      switch (m_config.TheRenderMode)
      {
      case RenderMode::ReferenceOnePass:
        m_blurImplementations.push_back(std::shared_ptr<ABlurredDraw>(new ReferenceOnePassBlurredDraw(config, m_config)));
        break;
      case RenderMode::ReferenceTwoPass:
        m_blurImplementations.push_back(std::shared_ptr<ABlurredDraw>(new ReferenceTwoPassBlurredDraw(config, m_config)));
        break;
      case RenderMode::TwoPassLinarScaled:
        m_blurImplementations.push_back(std::shared_ptr<ABlurredDraw>(new TwoPassLinearScaledBlurredDraw(config, m_config)));
        break;
      case RenderMode::ReferenceTwoPassLinear:
      default:
        m_blurImplementations.push_back(std::shared_ptr<ABlurredDraw>(new ReferenceTwoPassLinearBlurredDraw(config, m_config)));
        break;
      }
    }


    m_blurredDraw = m_blurImplementations[m_currentIndex];

    {
      BasicTextureAtlas atlas;
      contentManager->Read(atlas, "MainAtlas.bta");

      BasicFontKerning fontKerning;
      contentManager->Read(fontKerning, "MainAtlasFont.fbk");

      m_font.Reset(atlas, fontKerning);

      Fsl::Bitmap bitmap;
      contentManager->Read(bitmap, "MainAtlas.png", PixelFormat::R8G8B8A8_UNORM);
      m_texFontAtlas.Reset(m_graphicsService->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);
      m_texDescription.Reset(m_texFontAtlas, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Banners"));

      m_hTexAtlas = std::dynamic_pointer_cast<DynamicNativeTexture2D>(m_texFontAtlas.GetNative())->Get();
    }

    // Build the description VB
    {
      const Vector2 res(config.ScreenResolution.X / 2, config.ScreenResolution.Y / 2);
      const Vector2 atlasSize(TypeConverter::UncheckedTo<Vector2>(m_texDescription.GetAtlasSize()));
      const AtlasTextureInfo atlasInfo = m_texDescription.GetInfo();

      // texSize.X / tex
      float x1 = -1.0f - (atlasInfo.OffsetPx.X / res.X);
      float x2 = x1 + (atlasInfo.TrimmedRectPx.Width / res.X);
      float y1 = -1.0f - (atlasInfo.OffsetPx.Y / res.Y);
      float y2 = y1 + (atlasInfo.TrimmedRectPx.Height / res.Y);

      float u1 = atlasInfo.TrimmedRectPx.Left() / atlasSize.X;
      float v1 = 1.0f - (atlasInfo.TrimmedRectPx.Top() / atlasSize.Y);
      float u2 = atlasInfo.TrimmedRectPx.Right() / atlasSize.X;
      float v2 = 1.0f - (atlasInfo.TrimmedRectPx.Bottom() / atlasSize.Y);

      VBHelper::BuildVB(m_vbDescription, BoxF(x1, -y2, x2, -y1), BoxF(u1, v2, u2, v1));
    }
  }


  void BlurredScene::OnKeyEvent(const KeyEvent& event)
  {
    switch (event.GetKey())
    {
    case VirtualKey::Space:
      event.Handled();
      if (event.IsPressed())
      {
        m_blurredDraw = m_blurImplementations.front();
        ++m_currentIndex;
        if (std::size_t(m_currentIndex) >= m_blurImplementations.size())
        {
          m_currentIndex = 0;
        }
        m_blurredDraw = m_blurImplementations[m_currentIndex];
      }
      break;
    default:
      break;
    }
  }


  void BlurredScene::Update(const DemoTime& demoTime)
  {
    m_scene->Update(demoTime);
  }


  void BlurredScene::Draw()
  {
    m_blurredDraw->Draw(m_scene.get());

    switch (m_config.TheCaptionType)
    {
    case CaptionType::Algorithm:
      m_batch2D->Begin();
      m_batch2D->DrawString(m_texFontAtlas, m_font, m_blurredDraw->GetName(), Vector2(), Color::White());
      m_batch2D->End();
      break;
    case CaptionType::Description:
      // m_batch2D->Begin();
      // m_batch2D->Draw(m_texDescription, Vector2(), Color::White());
      // m_batch2D->End();

      // Draw the description using the static vertex buffer
      glEnable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_hTexAtlas);
      glUseProgram(m_program.Get());
      glBindBuffer(m_vbDescription.GetTarget(), m_vbDescription.Get());
      m_vbDescription.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      break;
    default:
      break;
    }
  }
}
