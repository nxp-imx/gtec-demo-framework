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

#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Shared/Log/Host/FmtDemoWindowMetrics.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontConfig.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslGraphics/TextureAtlas/CompatibilityTextureAtlasMap.hpp>
#include <FslSimpleUI/App/UIAppConfig.hpp>
#include <FslSimpleUI/App/UIAppResourceManager.hpp>
#include <FslSimpleUI/App/UIDemoAppExtensionCreateInfo.hpp>
#include <FslSimpleUI/App/UIDemoAppExtensionLite.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <utility>

// #define LOCAL_TIME_DRAW
#ifdef LOCAL_TIME_DRAW
#include <FslBase/System/HighResolutionTimer.hpp>
#endif

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView FontNbf("Font.nbf");
      constexpr IO::PathView HeaderFontNbf("Header.nbf");
      constexpr IO::PathView SdfFontNbf("SdfFont.nbf");
      constexpr IO::PathView SdfHeaderFontNbf("SdfHeader.nbf");
      // constexpr IO::PathView FontAtlasExtension(".bta");
      constexpr IO::PathView FontTextureExtension(".png");

      constexpr Texture2DFilterHint UITextureFilterHint = Texture2DFilterHint::Smooth;

      constexpr SpriteFontConfig DefaultFontConfig(true);
    }
  }


  UIDemoAppExtensionLite::UIDemoAppExtensionLite(const UIDemoAppExtensionCreateInfo& createInfo,
                                                 const std::shared_ptr<UI::IEventListener>& eventListener, const IO::Path& atlasName,
                                                 const UITestPatternMode testPatternMode)
    : UIDemoAppExtensionBase(createInfo, eventListener)
    , m_config(createInfo.RenderCreateInfo.MaterialConfig)
  {
    auto contentManager = createInfo.DemoServiceProvider.Get<IContentManager>();
    auto graphicsService = createInfo.DemoServiceProvider.Get<IGraphicsService>();
    m_resourceManager = std::make_unique<UIAppResourceManager>(contentManager, graphicsService->GetBasicRenderSystem(), createInfo.WindowMetrics,
                                                               testPatternMode, createInfo.RenderCreateInfo.MaterialConfig.AllowDepthBuffer,
                                                               createInfo.RenderCreateInfo.MaterialCreateInfo.AllowDynamicCustomViewport,
                                                               SYS_GetUseYFlipTextureCoordinates());


    m_resources = PrepareResources(*m_resourceManager, atlasName, createInfo.RenderCreateInfo.MaterialCreateInfo, m_config.UseSdfFont);

    // Prepare the window context
    m_context = std::make_shared<UI::WindowContext>(GetUIContext(), m_resources.DefaultFont, createInfo.WindowMetrics.DensityDpi);

    if (testPatternMode == UITestPatternMode::EnabledAllowSwitching)
    {
      SetTestPattern(true);
    }
  }


  UIDemoAppExtensionLite::~UIDemoAppExtensionLite() = default;


  void UIDemoAppExtensionLite::SYS_SetRenderSystemViewport(const PxViewport& viewportPx)
  {
    m_resourceManager->SYS_SetRenderSystemViewport(viewportPx);
    UIDemoAppExtensionBase::SYS_SetRenderSystemViewport(viewportPx);
  }


  void UIDemoAppExtensionLite::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSLLOG3_VERBOSE("UIDemoAppExtensionLite::ConfigurationChanged: {}", windowMetrics)
    m_context->UnitConverter.SetDensityDpi(windowMetrics.DensityDpi);
    m_resourceManager->ConfigurationChanged(windowMetrics);

    UIDemoAppExtensionBase::ConfigurationChanged(windowMetrics);
  }


  void UIDemoAppExtensionLite::Draw()
  {
    if (m_context)
    {
#ifdef LOCAL_TIME_DRAW
      HighResolutionTimer timer;
      auto startTime = timer.GetTime();
#endif

      DoDraw();

#ifdef LOCAL_TIME_DRAW
      {
        auto endTime = timer.GetTime();
        FSLLOG3_INFO("{}microseconds", endTime - startTime);
      }
#endif
    }
  }


  std::shared_ptr<SpriteFont> UIDemoAppExtensionLite::GetDefaultFont() const
  {
    return m_resources.DefaultFont;
  }


  std::shared_ptr<SpriteFont> UIDemoAppExtensionLite::GetDefaultHeaderFont() const
  {
    return m_resources.DefaultHeaderFont;
  }


  SpriteMaterialId UIDemoAppExtensionLite::GetDefaultMaterialId(const bool opaque) const
  {
    return !opaque ? UIAppConfig::MaterialId::DefaultUI_AlphaBlend : UIAppConfig::MaterialId::DefaultUI_Opaque;
  }

  bool UIDemoAppExtensionLite::SetTestPattern(const bool enabled)
  {
    bool result = false;
    if (m_resourceManager)
    {
      result = m_resourceManager->SetTestPattern(enabled);
    }
    return result;
  }


  UIAppTextureHandle UIDemoAppExtensionLite::CreateTexture(const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
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

  UIAppTextureHandle UIDemoAppExtensionLite::CreateAtlasTexture(const IO::PathView& atlasPath,
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


  void UIDemoAppExtensionLite::AddSpriteMaterial(const SpriteMaterialId& spriteMaterialId, const UIAppTextureHandle& hTexture,
                                                 const BlendState blendState)
  {
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    m_resourceManager->AddSpriteMaterial(spriteMaterialId, hTexture, blendState);
  }


  const ISpriteResourceManager& UIDemoAppExtensionLite::GetSpriteResourceManager() const
  {
    ISpriteResourceManager* pResourceManager = m_resourceManager.get();
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return *pResourceManager;
  }

  ISpriteResourceManager& UIDemoAppExtensionLite::GetSpriteResourceManager()
  {
    ISpriteResourceManager* pResourceManager = m_resourceManager.get();
    if (pResourceManager == nullptr)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return *pResourceManager;
  }


  const ICustomSpriteResourceManager& UIDemoAppExtensionLite::GetCustomSpriteResourceManager() const
  {
    ICustomSpriteResourceManager* pResourceManager = m_resourceManager.get();
    if (!m_resourceManager)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return *pResourceManager;
  }

  ICustomSpriteResourceManager& UIDemoAppExtensionLite::GetCustomSpriteResourceManager()
  {
    ICustomSpriteResourceManager* pResourceManager = m_resourceManager.get();
    if (pResourceManager == nullptr)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return *pResourceManager;
  }

  const SpriteNativeAreaCalc& UIDemoAppExtensionLite::GetSpriteNativeAreaCalc() const
  {
    UIAppResourceManager* pResourceManager = m_resourceManager.get();
    if (pResourceManager == nullptr)
    {
      throw UsageErrorException("resource manager is not valid");
    }
    return pResourceManager->GetSpriteNativeAreaCalc();
  }


  uint16_t UIDemoAppExtensionLite::GetDefaultMaterialDpi() const
  {
    return m_resources.DefaultFont ? m_resources.DefaultFont->GetInfo().ImageDpi : 0;
  }


  void UIDemoAppExtensionLite::DoChangeDefaultMaterials(const UIDemoAppMaterialConfig& config)
  {
    UIAppResourceManager& rResourceManager = *m_resourceManager;

    if (config != m_config)
    {
      if (config.UseSdfFont != m_config.UseSdfFont)
      {
        if (config.UseSdfFont)
        {
          rResourceManager.PatchSpriteFont(m_resources.DefaultFont, UIAppConfig::MaterialId::DefaultUI_SdfFont, m_resources.Paths.SdfFontNbf);
          rResourceManager.PatchSpriteFont(m_resources.DefaultHeaderFont, UIAppConfig::MaterialId::DefaultUI_SdfFont,
                                           m_resources.Paths.SdfHeaderFontNbf);
        }
        else
        {
          rResourceManager.PatchSpriteFont(m_resources.DefaultFont, UIAppConfig::MaterialId::DefaultUI_AlphaBlend, m_resources.Paths.FontNbf);
          rResourceManager.PatchSpriteFont(m_resources.DefaultHeaderFont, UIAppConfig::MaterialId::DefaultUI_AlphaBlend,
                                           m_resources.Paths.HeaderFontNbf);
        }
      }

      rResourceManager.SetOptions(config.AllowDepthBuffer);

      m_config = config;
      ForceInvalidateLayout();
    }
  }


  UIDemoAppExtensionLite::Resources UIDemoAppExtensionLite::PrepareResources(UIAppResourceManager& rResourceManager, const IO::Path& atlasName,
                                                                             const UIDemoAppMaterialCreateInfo& materialCreateInfo,
                                                                             const bool useSdfFont)
  {
    IO::Path defaultAtlasTexturePath(atlasName + LocalConfig::FontTextureExtension);
    UIAppTextureResourceCreationInfo textureCreationInfo(materialCreateInfo.DefaultTexturePixelFormat, LocalConfig::UITextureFilterHint);

    // Texture, material and a default font
    auto textureResult =
      rResourceManager.CreateTexture(defaultAtlasTexturePath, textureCreationInfo, UIAppResourceFlag::Atlas | UIAppResourceFlag::UIGroup);


    const auto blendStateOpaque = !materialCreateInfo.DisableOpaqueMaterials ? BlendState::Opaque : BlendState::AlphaBlend;

    rResourceManager.AddSpriteMaterial(UIAppConfig::MaterialId::DefaultUI_Opaque, textureResult.Handle, blendStateOpaque);
    rResourceManager.AddSpriteMaterial(UIAppConfig::MaterialId::DefaultUI_AlphaBlend, textureResult.Handle, BlendState::AlphaBlend);
    rResourceManager.AddSpriteMaterial(UIAppConfig::MaterialId::DefaultUI_SdfFont, textureResult.Handle, BlendState::Sdf);

    IO::PathView headerFontNbf(rResourceManager.FontExists(textureResult.Handle, LocalConfig::HeaderFontNbf) ? LocalConfig::HeaderFontNbf
                                                                                                             : LocalConfig::FontNbf);
    IO::PathView sdfHeaderFontNbf(rResourceManager.FontExists(textureResult.Handle, LocalConfig::SdfHeaderFontNbf) ? LocalConfig::SdfHeaderFontNbf
                                                                                                                   : LocalConfig::SdfFontNbf);
    PathRecord paths(LocalConfig::FontNbf, headerFontNbf, LocalConfig::SdfFontNbf, sdfHeaderFontNbf);

    auto defaultFont = PrepareFont(rResourceManager, paths.FontNbf, paths.SdfFontNbf, useSdfFont);
    auto defaultHeaderFont = PrepareFont(rResourceManager, paths.HeaderFontNbf, paths.SdfHeaderFontNbf, useSdfFont);
    return {std::move(defaultFont), std::move(defaultHeaderFont), paths};
  }

  std::shared_ptr<SpriteFont> UIDemoAppExtensionLite::PrepareFont(UIAppResourceManager& rResourceManager, const IO::PathView normalFont,
                                                                  const IO::PathView sdfFont, const bool useSdfFont)
  {
    const auto materialId = !useSdfFont ? UIAppConfig::MaterialId::DefaultUI_AlphaBlend : UIAppConfig::MaterialId::DefaultUI_SdfFont;
    const auto fontFile = !useSdfFont ? normalFont : sdfFont;
    return rResourceManager.CreateSpriteFont(materialId, fontFile, LocalConfig::DefaultFontConfig);
  }

}
