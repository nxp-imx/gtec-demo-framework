#ifndef FSLSIMPLEUI_APP_UIDEMOAPPEXTENSION_HPP
#define FSLSIMPLEUI_APP_UIDEMOAPPEXTENSION_HPP
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

#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasMap.hpp>
#include <FslSimpleUI/App/UIAppTextureHandle.hpp>
#include <FslSimpleUI/App/UIDemoAppExtensionBase.hpp>
#include <FslSimpleUI/App/UITestPatternMode.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>

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
  class BaseTexture2D;
  class IContentManager;
  class ISpriteResourceManager;
  class SpriteFont;
  class ImageSprite;
  class UIAppResourceManager;
  struct UIAppTextureResourceCreationInfo;


  //! @brief This takes advantage of the new Sprite material infrastructure to provide a fully DP aware UI that can switch its native resources
  //         based on the currently active screen density at runtime.
  class UIDemoAppExtension final : public UIDemoAppExtensionBase
  {
    struct TestRecord
    {
      Texture Original;
      Texture TestPattern;
      std::shared_ptr<IDynamicNativeTexture2D> AtlasTexture;
      bool TestPatternEnabled{false};
    };

    struct Resources
    {
      std::shared_ptr<SpriteFont> DefaultFont;
    };

    std::unique_ptr<UIAppResourceManager> m_resourceManager;
    std::shared_ptr<UI::WindowContext> m_context;

    Resources m_resources;
    TestRecord m_testRecord;

  public:
    UIDemoAppExtension(const UIDemoAppExtension&) = delete;
    UIDemoAppExtension& operator=(const UIDemoAppExtension&) = delete;

    UIDemoAppExtension(const DemoAppConfig& demoAppConfig, const std::shared_ptr<UI::IEventListener>& eventListener, const IO::Path& atlasName,
                       const UITestPatternMode testPatternMode = UITestPatternMode::Disabled);
    ~UIDemoAppExtension() override;

    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;


    void Draw();

    //! @brief Get access to the window context
    std::shared_ptr<UI::WindowContext> GetContext() const
    {
      return m_context;
    }

    SpriteMaterialId GetDefaultMaterialId() const;

    //! @brief Retrieve the atlas texture used for the default texture atlas

    //! @brief Enable/disable the test pattern
    //! @note This requires that the object was constructed with UITestPatternMode::DisabledAllowSwitching or UITestPatternMode::EnableAllowSwitching.
    //        If it wasn't this will just log a warning.
    bool SetTestPattern(const bool enabled);

    UIAppTextureHandle CreateTexture(const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                     const bool isUITexture);
    UIAppTextureHandle CreateAtlasTexture(const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                          const bool isUITexture);
    void AddSpriteMaterial(const SpriteMaterialId& spriteMaterialId, const UIAppTextureHandle& hTexture, const BlendState blendState);

    // Get access to the sprite resource manager
    const ISpriteResourceManager& GetSpriteResourceManager() const;
    // Get access to the sprite resource manager
    ISpriteResourceManager& GetSpriteResourceManager();

  private:
    static Resources PrepareResources(UIAppResourceManager& rResourceManager, const IO::Path& atlasName);
  };
}

#endif
