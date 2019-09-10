/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Transition/TransitionCache.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <utility>

namespace Fsl
{
  using namespace UI;

  namespace
  {
    std::shared_ptr<AtlasFont> PrepareDefaultFont(const std::shared_ptr<IContentManager>& contentManager,
                                                  const std::shared_ptr<INativeGraphics>& nativeGraphics, const UTF8String& fontName,
                                                  BasicTextureAtlas& rFontAtlas)
    {
      const auto defaultPath = "UI";
      const auto& strFontname = fontName.ToUTF8String();
      IO::Path pathFontnameAtlas(strFontname + ".bta");
      IO::Path pathFontnameKerning(strFontname + ".fbk");
      IO::Path pathFontnameTexture(strFontname + ".png");

      IO::Path tempPath = IO::Path::Combine(defaultPath, pathFontnameAtlas);
      if (contentManager->Exists(tempPath))
      {
        pathFontnameAtlas = std::move(tempPath);
        pathFontnameKerning = IO::Path::Combine(defaultPath, pathFontnameKerning);
        pathFontnameTexture = IO::Path::Combine(defaultPath, pathFontnameTexture);
      }

      BasicFontKerning fontKerning;
      Bitmap fontBitmap;
      contentManager->Read(rFontAtlas, pathFontnameAtlas);
      contentManager->Read(fontKerning, pathFontnameKerning);
      contentManager->Read(fontBitmap, pathFontnameTexture, PixelFormat::R8G8B8A8_UNORM);

      // Prepare the texture
      Texture2D atlasTexture(nativeGraphics, fontBitmap, Texture2DFilterHint::Smooth);

      // Prepare the bitmap font
      TextureAtlasBitmapFont bitmapFont(rFontAtlas, fontKerning);

      // Create the font instance
      return std::make_shared<AtlasFont>(atlasTexture, bitmapFont);
    }
  }


  UIDemoAppExtension::UIDemoAppExtension(const DemoAppConfig& demoAppConfig, const std::shared_ptr<UI::IEventListener>& eventListener,
                                         const UTF8String& fontName)
    : m_uiManager(demoAppConfig.ScreenResolution)
    , m_transitionCache(std::make_shared<TransitionCache>())
  {
    m_uiManager.RegisterEventListener(eventListener);

    auto contentManager = demoAppConfig.DemoServiceProvider.Get<IContentManager>();
    auto graphicsService = demoAppConfig.DemoServiceProvider.Get<IGraphicsService>();

    // Load the default texture atlas and font stuff
    m_defaultFont = PrepareDefaultFont(contentManager, graphicsService->GetNativeGraphics(), fontName, m_defaultTextureAtlas);

    // Prepare a quick look up map for textures
    m_textureAtlasMap = TextureAtlasMap(m_defaultTextureAtlas);

    // Prepare the window context
    m_context = std::make_shared<WindowContext>(m_uiManager.GetUIContext(), graphicsService->GetNativeBatch2D(), m_defaultFont);
  }


  UIDemoAppExtension::~UIDemoAppExtension() = default;

  void UIDemoAppExtension::RegisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener)
  {
    m_uiManager.RegisterEventListener(eventListener);
  }

  void UIDemoAppExtension::UnregisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener)
  {
    m_uiManager.UnregisterEventListener(eventListener);
  }


  void UIDemoAppExtension::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    m_uiManager.SendMouseButtonEvent(event);
  }


  void UIDemoAppExtension::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    m_uiManager.SendMouseMoveEvent(event);
  }

  void UIDemoAppExtension::Resized(const Point2& size)
  {
    m_uiManager.Resized(size);
  }

  void UIDemoAppExtension::PreUpdate(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    m_uiManager.ProcessEvents();
  }

  void UIDemoAppExtension::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    m_uiManager.ProcessEvents();
  }


  void UIDemoAppExtension::PostUpdate(const DemoTime& demoTime)
  {
    // We call the UIManager in post update to allow the app to modify the UI in its update method
    m_uiManager.Update(demoTime);
  }


  void UIDemoAppExtension::Draw()
  {
    if (m_context)
    {
      const auto batch = m_context->Batch2D;
      batch->Begin();
      m_uiManager.Draw();
      batch->End();
    }
  }


  AtlasTexture2D UIDemoAppExtension::GetAtlasTexture2D(const UTF8String& filename) const
  {
    return AtlasTexture2D(m_defaultFont->GetAtlasTexture(), m_textureAtlasMap.GetAtlasTextureInfo(filename));
  }


  Texture2D UIDemoAppExtension::GetAtlasTexture() const
  {
    return m_defaultFont->GetAtlasTexture();
  }


  std::shared_ptr<TransitionCache> UIDemoAppExtension::GetTransitionCache() const
  {
    return m_transitionCache;
  }
}
