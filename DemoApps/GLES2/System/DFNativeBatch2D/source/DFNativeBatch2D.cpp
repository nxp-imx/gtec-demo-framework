/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include "DFNativeBatch2D.hpp"
#include <FslBase/Math/Rectangle.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>
#include <iostream>

namespace Fsl
{
  DFNativeBatch2D::DFNativeBatch2D(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_scene1(config.DemoServiceProvider, SpriteNativeAreaCalc(true), 160)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(std::dynamic_pointer_cast<GLES2::NativeBatch2D>(m_graphics->GetNativeBatch2D()))
  {
    if (!m_nativeBatch)
    {
      throw NotSupportedException("INativeBatch not backed by the expected type");
    }

    const std::shared_ptr<IContentManager> contentManager = GetContentManager();
    {
      Fsl::Bitmap bitmap;
      contentManager->Read(bitmap, "Test/Test.png", PixelFormat::R8G8B8A8_UNORM);
      m_texture2D.Reset(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Nearest);
    }
    {
      GLES2::GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      Fsl::Bitmap bitmap;
      contentManager->Read(bitmap, "Test/Test2.png", PixelFormat::R8G8B8_UNORM);
      m_nativeTexture.SetData(bitmap, params);
    }
  }


  DFNativeBatch2D::~DFNativeBatch2D() = default;


  void DFNativeBatch2D::OnKeyEvent(const KeyEvent& event)
  {
    m_scene1.OnKeyEvent(event);
  }


  void DFNativeBatch2D::Update(const DemoTime& demoTime)
  {
    m_scene1.Update(demoTime);
  }


  void DFNativeBatch2D::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    // Clear the screen
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    const std::shared_ptr<INativeGraphics> native = m_graphics->GetNativeGraphics();
    const PxSize2D windowSizePx = GetWindowSizePx();
    const PxValue offsetY = m_scene1.GetGridOffsetY().Value();

    constexpr auto Size2Px = PxSize1D::Create(2);

    m_nativeBatch->Begin(BlendState::Opaque);

    const PxSize2D nativeTexSize = m_nativeTexture.GetSize();

    // GLES2 native texture handle
    m_nativeBatch->Draw(GLES2::GLTextureInfo(m_nativeTexture.Get(), m_nativeTexture.GetSize()),
                        PxRectangle(windowSizePx.Width() - PxSize1D::UncheckedCreate(256), offsetY + PxSize1D::UncheckedCreate(256),
                                    PxSize1D::UncheckedCreate(256), PxSize1D::UncheckedCreate(256)),
                        Colors::White());

    // GLES2 native texture
    PxRectangle dstRect(windowSizePx.Width() - nativeTexSize.Width() - PxSize1D::UncheckedCreate(128 * 2),
                        offsetY + PxSize1D::UncheckedCreate(256 * 0 + 128), nativeTexSize.Width() / Size2Px, nativeTexSize.Height() / Size2Px);
    m_nativeBatch->Draw(m_nativeTexture, Vector2(windowSizePx.RawWidth() - nativeTexSize.RawWidth() - (128 * 2), offsetY.Value), Colors::White());
    m_nativeBatch->Draw(m_nativeTexture, dstRect, Colors::White());

    // API independent texture
    const PxSize2D texSize = m_texture2D.GetSize();
    m_nativeBatch->Draw(m_texture2D, Vector2(windowSizePx.RawWidth() - texSize.RawWidth(), offsetY.Value), Colors::White());
    dstRect = PxRectangle(windowSizePx.Width() - texSize.Width() - (texSize.Width() / Size2Px), offsetY + texSize.Height(), texSize.Width() / Size2Px,
                          texSize.Height() / Size2Px);
    m_nativeBatch->Draw(m_texture2D, dstRect, Colors::White());

    m_nativeBatch->End();


    // Draw native API independent graphics
    m_scene1.Draw(windowSizePx);
  }
}
