/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Transition/TransitionCache.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontConfig.hpp>
#include <FslGraphics/TextureAtlas/CompatibilityTextureAtlasMap.hpp>
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
      constexpr IO::PathView FontNbf("Font.nbf");
      // constexpr IO::PathView FontAtlasExtension(".bta");
      constexpr IO::PathView FontTextureExtension(".png");

      constexpr PixelFormat UIPixelFormat = PixelFormat::R8G8B8A8_UNORM;
      constexpr Texture2DFilterHint UITextureFilterHint = Texture2DFilterHint::Smooth;

      constexpr SpriteFontConfig DefaultFontConfig(true);
    }
  }


  UIDemoAppExtension::UIDemoAppExtension(const DemoAppConfig& demoAppConfig, const std::shared_ptr<UI::IEventListener>& eventListener,
                                         const IO::Path& atlasName, const UITestPatternMode testPatternMode)
    : UIDemoAppExtensionBase(demoAppConfig, eventListener)
  {
    auto contentManager = demoAppConfig.DemoServiceProvider.Get<IContentManager>();
    auto graphicsService = demoAppConfig.DemoServiceProvider.Get<IGraphicsService>();
    m_resourceManager =
      std::make_unique<UIAppResourceManager>(contentManager, graphicsService->GetNativeGraphics(), demoAppConfig.WindowMetrics, testPatternMode);

    m_resources = PrepareResources(*m_resourceManager, atlasName);

    // Prepare the window context
    m_context = std::make_shared<UI::WindowContext>(GetUIContext(), graphicsService->GetNativeBatch2D(), m_resources.DefaultFont,
                                                    demoAppConfig.WindowMetrics.DensityDpi);

    if (testPatternMode == UITestPatternMode::EnabledAllowSwitching)
    {
      SetTestPattern(true);
    }
  }


  UIDemoAppExtension::~UIDemoAppExtension() = default;


  void UIDemoAppExtension::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_context->UnitConverter.SetDensityDpi(windowMetrics.DensityDpi);
    m_resourceManager->ConfigurationChanged(windowMetrics);

    UIDemoAppExtensionBase::ConfigurationChanged(windowMetrics);
  }

  void UIDemoAppExtension::Draw()
  {
    if (m_context)
    {
      const auto batch = m_context->Batch2D;
      batch->Begin();
      DoDraw();
      batch->End();
    }
  }

  SpriteMaterialId UIDemoAppExtension::GetDefaultMaterialId() const
  {
    return UIAppConfig::DefaultUIAlphaBlendMaterialId;
  }

  bool UIDemoAppExtension::SetTestPattern(const bool enabled)
  {
    bool result = false;
    if (m_resourceManager)
    {
      result = m_resourceManager->SetTestPattern(enabled);
    }
    return result;
  }


  UIAppTextureHandle UIDemoAppExtension::CreateTexture(const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                       const bool isUITexture)
  {
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    const auto flags = isUITexture ? UIAppResourceFlag::UIGroup : UIAppResourceFlag::Undefined;
    auto res = m_resourceManager->CreateTexture(atlasPath, textureCreationInfo, flags);
    return res.Handle;
  }

  UIAppTextureHandle UIDemoAppExtension::CreateAtlasTexture(const IO::PathView& atlasPath,
                                                            const UIAppTextureResourceCreationInfo& textureCreationInfo, const bool isUITexture)
  {
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    const auto flags = isUITexture ? UIAppResourceFlag::UIGroup | UIAppResourceFlag::Atlas : UIAppResourceFlag::Atlas;
    auto res = m_resourceManager->CreateTexture(atlasPath, textureCreationInfo, flags);
    return res.Handle;
  }


  void UIDemoAppExtension::AddSpriteMaterial(const SpriteMaterialId& spriteMaterialId, const UIAppTextureHandle& hTexture,
                                             const BlendState blendState)
  {
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    m_resourceManager->AddSpriteMaterial(spriteMaterialId, hTexture, blendState);
  }


  const ISpriteResourceManager& UIDemoAppExtension::GetSpriteResourceManager() const
  {
    ISpriteResourceManager* pResourceManager = m_resourceManager.get();
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return *pResourceManager;
  }

  ISpriteResourceManager& UIDemoAppExtension::GetSpriteResourceManager()
  {
    ISpriteResourceManager* pResourceManager = m_resourceManager.get();
    if (pResourceManager == nullptr)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return *pResourceManager;
  }


  UIDemoAppExtension::Resources UIDemoAppExtension::PrepareResources(UIAppResourceManager& rResourceManager, const IO::Path& atlasName)
  {
    IO::Path defaultAtlasTexturePath(atlasName + LocalConfig::FontTextureExtension);
    UIAppTextureResourceCreationInfo textureCreationInfo(LocalConfig::UIPixelFormat, LocalConfig::UITextureFilterHint);

    // Texture, material and a default font
    auto textureResult =
      rResourceManager.CreateTexture(defaultAtlasTexturePath, textureCreationInfo, UIAppResourceFlag::Atlas | UIAppResourceFlag::UIGroup);
    rResourceManager.AddSpriteMaterial(UIAppConfig::DefaultUIAlphaBlendMaterialId, textureResult.Handle, BlendState::AlphaBlend);
    std::shared_ptr<SpriteFont> defaultFont =
      rResourceManager.CreateSpriteFont(UIAppConfig::DefaultUIAlphaBlendMaterialId, LocalConfig::FontNbf, LocalConfig::DefaultFontConfig);

    return {defaultFont};
  }
}
