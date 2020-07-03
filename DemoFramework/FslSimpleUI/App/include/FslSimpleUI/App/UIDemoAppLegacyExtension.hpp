#ifndef FSLSIMPLEUI_APP_UIDEMOAPPLEGACYEXTENSION_HPP
#define FSLSIMPLEUI_APP_UIDEMOAPPLEGACYEXTENSION_HPP
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

#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/CompatibilityTextureAtlasMap.hpp>
#include <FslSimpleUI/App/UITestPatternMode.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>
#include <FslSimpleUI/App/UIAppTextureHandle.hpp>
#include <FslSimpleUI/App/UIDemoAppExtensionBase.hpp>


namespace Fsl
{
  namespace IO
  {
    class Path;
  }
  namespace UI
  {
    class WindowContext;
  }
  class AtlasFont;
  class IContentManager;
  class ISpriteResourceManager;
  class SpriteFont;
  class UIAppResourceManager;


  class UIDemoAppLegacyExtension final : public UIDemoAppExtensionBase
  {
    struct Resources
    {
      UIAppTextureHandle UITextureHandle;
      std::shared_ptr<SpriteFont> DefaultFont;
      Texture2D LegacyTexture;
      CompatibilityTextureAtlasMap LegacyTextureAtlasMap;
    };

    std::unique_ptr<UIAppResourceManager> m_resourceManager;
    Resources m_resources;
    std::shared_ptr<UI::WindowContext> m_context;

  public:
    UIDemoAppLegacyExtension(const UIDemoAppLegacyExtension&) = delete;
    UIDemoAppLegacyExtension& operator=(const UIDemoAppLegacyExtension&) = delete;

    UIDemoAppLegacyExtension(const DemoAppConfig& demoAppConfig, const std::shared_ptr<UI::IEventListener>& eventListener, const IO::Path& fontName,
                             const UITestPatternMode testPatternMode = UITestPatternMode::Disabled);
    ~UIDemoAppLegacyExtension() override;


    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;

    void Draw();

    //! @brief Get access to the window context
    std::shared_ptr<UI::WindowContext> GetContext() const
    {
      return m_context;
    }

    const TextureAtlasMap& GetAtlasTextureMap() const;

    //! @brief Retrieve a atlas texture from the default texture atlas
    AtlasTexture2D GetAtlasTexture2D(const IO::PathView& filename) const;

    //! @brief Retrieve a atlas texture from the default texture atlas
    AtlasTexture2D GetAtlasTexture2D(const IO::Path& filename) const
    {
      return GetAtlasTexture2D(filename.AsPathView());
    }

    //! @brief Retrieve the atlas texture used for the default texture atlas
    const BaseTexture2D& GetAtlasTexture() const;

    const ITextureAtlas& GetDefaultTextureAtlas() const;

    //! @brief Enable/disable the test pattern
    //! @note This requires that the object was constructed with UITestPatternMode::DisabledAllowSwitching or UITestPatternMode::EnableAllowSwitching.
    //        If it wasn't this will just log a warning.
    bool SetTestPattern(const bool enabled);


    std::shared_ptr<SpriteFont> CreateLegacySpriteFont(const IO::PathView& font);
    std::shared_ptr<SpriteFont> CreateLegacySpriteFont(const IO::Path& font)
    {
      return CreateLegacySpriteFont(font.AsPathView());
    }
    std::shared_ptr<SpriteFont> CreateSpriteFont(const IO::PathView& font);
    std::shared_ptr<SpriteFont> CreateSpriteFont(const IO::Path& font)
    {
      return CreateSpriteFont(font.AsPathView());
    }

    // Get access to the sprite resource manager
    const ISpriteResourceManager& GetSpriteResourceManager() const;
    // Get access to the sprite resource manager
    ISpriteResourceManager& GetSpriteResourceManager();

    SpriteMaterialId GetDefaultMaterialId() const;


  private:
    static Resources PrepareDefaultResources(UIAppResourceManager& rResourceManager, IContentManager& contentManager, const IO::Path& fontName);
  };
}

#endif
