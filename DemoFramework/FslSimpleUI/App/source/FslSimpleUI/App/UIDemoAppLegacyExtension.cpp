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

#include <FslSimpleUI/App/UIDemoAppLegacyExtension.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Transition/TransitionCache.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontConfig.hpp>
#include <FslSimpleUI/App/UIAppConfig.hpp>
#include <FslSimpleUI/App/UIAppResourceManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <utility>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView UIDefaultPath("UI");

      constexpr IO::PathView FontNbf("_Font.nbf");
      constexpr IO::PathView FontAtlasExtension(".bta");
      constexpr IO::PathView FontKerningExtension(".fbk");
      constexpr IO::PathView FontTextureExtension(".png");
    }
  }


  UIDemoAppLegacyExtension::UIDemoAppLegacyExtension(const DemoAppConfig& demoAppConfig, const std::shared_ptr<UI::IEventListener>& eventListener,
                                                     const IO::Path& fontName, const UITestPatternMode testPatternMode)
    : UIDemoAppExtensionBase(demoAppConfig, eventListener)
  {
    auto contentManager = demoAppConfig.DemoServiceProvider.Get<IContentManager>();
    auto graphicsService = demoAppConfig.DemoServiceProvider.Get<IGraphicsService>();
    m_resourceManager =
      std::make_unique<UIAppResourceManager>(contentManager, graphicsService->GetNativeGraphics(), demoAppConfig.WindowMetrics, testPatternMode);

    // Load the default texture atlas and font stuff
    m_resources = PrepareDefaultResources(*m_resourceManager, *contentManager, fontName);

    // Prepare the window context
    m_context = std::make_shared<UI::WindowContext>(GetUIContext(), graphicsService->GetNativeBatch2D(), m_resources.DefaultFont,
                                                    demoAppConfig.WindowMetrics.DensityDpi);
  }


  UIDemoAppLegacyExtension::~UIDemoAppLegacyExtension() = default;


  void UIDemoAppLegacyExtension::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    if (m_context)
    {
      m_context->UnitConverter.SetDensityDpi(windowMetrics.DensityDpi);
    }
    if (m_resourceManager)
    {
      m_resourceManager->ConfigurationChanged(windowMetrics);
    }
    UIDemoAppExtensionBase::ConfigurationChanged(windowMetrics);
  }

  void UIDemoAppLegacyExtension::Draw()
  {
    if (m_context)
    {
      const auto batch = m_context->Batch2D;
      batch->Begin();
      DoDraw();
      batch->End();
    }
  }

  const TextureAtlasMap& UIDemoAppLegacyExtension::GetAtlasTextureMap() const
  {
    return m_resources.LegacyTextureAtlasMap;
  }

  AtlasTexture2D UIDemoAppLegacyExtension::GetAtlasTexture2D(const IO::PathView& filename) const
  {
    return AtlasTexture2D(m_resources.LegacyTexture, m_resources.LegacyTextureAtlasMap.GetAtlasTextureInfo(filename));
  }


  const BaseTexture2D& UIDemoAppLegacyExtension::GetAtlasTexture() const
  {
    return m_resources.LegacyTexture;
  }

  const ITextureAtlas& UIDemoAppLegacyExtension::GetDefaultTextureAtlas() const
  {
    return m_resources.LegacyTextureAtlasMap;
  }


  bool UIDemoAppLegacyExtension::SetTestPattern(const bool enabled)
  {
    bool result = false;
    if (m_resourceManager)
    {
      result = m_resourceManager->SetTestPattern(enabled);
    }
    return result;
  }


  std::shared_ptr<SpriteFont> UIDemoAppLegacyExtension::CreateSpriteFont(const IO::PathView& font)
  {
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    SpriteFontConfig spriteFontConfig(true);
    return m_resourceManager->CreateSpriteFont(UIAppConfig::DefaultUIAlphaBlendMaterialId, font, spriteFontConfig);
  }

  std::shared_ptr<SpriteFont> UIDemoAppLegacyExtension::CreateLegacySpriteFont(const IO::PathView& font)
  {
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    SpriteFontConfig spriteFontConfig(true);
    return m_resourceManager->CreateLegacySpriteFont(UIAppConfig::DefaultUIAlphaBlendMaterialId, font, spriteFontConfig);
  }

  const ISpriteResourceManager& UIDemoAppLegacyExtension::GetSpriteResourceManager() const
  {
    ISpriteResourceManager* pResourceManager = m_resourceManager.get();
    if (pResourceManager == nullptr)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return *pResourceManager;
  }


  ISpriteResourceManager& UIDemoAppLegacyExtension::GetSpriteResourceManager()
  {
    ISpriteResourceManager* pResourceManager = m_resourceManager.get();
    if (pResourceManager == nullptr)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return *pResourceManager;
  }

  SpriteMaterialId UIDemoAppLegacyExtension::GetDefaultMaterialId() const
  {
    return UIAppConfig::DefaultUIAlphaBlendMaterialId;
  }


  UIDemoAppLegacyExtension::Resources UIDemoAppLegacyExtension::PrepareDefaultResources(UIAppResourceManager& rResourceManager,
                                                                                        IContentManager& contentManager, const IO::Path& fontName)
  {
    IO::Path pathFontnameAtlas(fontName + LocalConfig::FontAtlasExtension);
    IO::Path pathFontnameKerning(fontName + LocalConfig::FontKerningExtension);
    IO::Path pathFontnameTexture(fontName + LocalConfig::FontTextureExtension);
    IO::Path pathNbfFont(fontName + LocalConfig::FontNbf);

    IO::Path tempPath = IO::Path::Combine(LocalConfig::UIDefaultPath, pathFontnameAtlas);
    if (contentManager.Exists(tempPath))
    {
      pathFontnameAtlas = std::move(tempPath);
      pathFontnameKerning = IO::Path::Combine(LocalConfig::UIDefaultPath, pathFontnameKerning);
      pathFontnameTexture = IO::Path::Combine(LocalConfig::UIDefaultPath, pathFontnameTexture);
      pathNbfFont = IO::Path::Combine(LocalConfig::UIDefaultPath, pathNbfFont);
    }

    UIAppTextureResourceCreationInfo createInfo(PixelFormat::R8G8B8A8_UNORM, Texture2DFilterHint::Smooth);

    auto textureResult = rResourceManager.CreateTexture(pathFontnameTexture, createInfo,
                                                        UIAppResourceFlag::UIGroup | UIAppResourceFlag::Atlas | UIAppResourceFlag::NotDpAware);

    rResourceManager.AddSpriteMaterial(UIAppConfig::DefaultUIAlphaBlendMaterialId, textureResult.Handle, BlendState::AlphaBlend);

    SpriteFontConfig spriteFontConfig(true);
    const auto& finalFontName = contentManager.Exists(pathNbfFont) ? pathNbfFont : pathFontnameKerning;
    auto defaultFont = rResourceManager.CreateLegacySpriteFont(UIAppConfig::DefaultUIAlphaBlendMaterialId, finalFontName, spriteFontConfig);

    return {textureResult.Handle, defaultFont, Texture2D(textureResult.Texture, textureResult.ExtentPx, textureResult.TexturePixelFormat),
            rResourceManager.GetLegacyTextureAtlasMap(textureResult.Handle)};
  }
}
