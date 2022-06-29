#ifndef SHARED_UI_DPISCALE_SHARED_HPP
#define SHARED_UI_DPISCALE_SHARED_HPP
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

#include <FslBase/Math/Pixel/PxRectangle2D.hpp>
#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/Transition/TransitionValue.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasMap.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
//#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
//#include <FslSimpleUI/Base/System/EventListener.hpp>
//#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <fmt/format.h>
#include <memory>

namespace Fsl
{
  namespace UI::Theme
  {
    class IThemeControlFactory;
  }

  struct Resources
  {
    Texture2D AtlasTexture;
    Texture2D AtlasTestTexture;
    TextureAtlasMap AtlasMap;
    std::shared_ptr<AtlasFont> Font;
    uint32_t Density{0};

    //! The density scale used for the current resolution
    float ResolutionDensityScale{1.0f};

    BitmapFontConfig FontConfig;

    void UpdateDensity(const float newDensity)
    {
      ResolutionDensityScale = newDensity / static_cast<float>(Density);
      FontConfig.Scale = ResolutionDensityScale;
    }
  };

  class Shared : public UI::EventListener
  {
    struct UIRecord
    {
      std::shared_ptr<UI::Label> LabelResPx;
      std::shared_ptr<UI::Label> LabelResDp;
      std::shared_ptr<UI::Label> LabelDpi;
      std::shared_ptr<UI::Label> LabelDensityDpi;
      std::shared_ptr<UI::Label> LabelDensityScale;
      std::shared_ptr<UI::Switch> CheckBoxPixelPerfect;
      std::shared_ptr<UI::Switch> CheckBoxTestPattern;
      std::shared_ptr<UI::Switch> CheckBoxUITestPattern;
      std::shared_ptr<UI::Switch> CheckBoxEmulateDpi;
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> SliderDpi;
      std::shared_ptr<UI::Switch> CheckForceTexDpi;
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> SliderTextureDpi;
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> SliderDownscalePreference;
      std::shared_ptr<UI::Switch> CheckBoxShowScaleExample;
      std::shared_ptr<UI::BackgroundLabelButton> BtnSetDefaultValues;
      std::shared_ptr<UI::Layout> MainLayout;
    };


    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<IBasicRenderSystem> m_renderSystem;
    std::shared_ptr<INativeBatch2D> m_nativeBatch;

    DemoWindowMetrics m_displayMetrics;

    std::shared_ptr<UI::BaseWindow> m_dummyDp;
    std::shared_ptr<UI::BaseWindow> m_dummyPx;
    std::shared_ptr<UI::BaseWindow> m_dummyText;
    std::shared_ptr<UI::GridLayout> m_mainLayout;
    UIRecord m_uiRecord;

    uint32_t m_activeDensity{0};
    Resources m_res160;
    Resources m_res320;
    Resources m_res480;
    Resources m_res640;

    fmt::memory_buffer m_fmtScratchpad;
    std::vector<SpriteFontGlyphPosition> m_glyphScratchpad;
    std::shared_ptr<ImageSprite> m_fillSprite;
    AtlasTexture2D m_texFill;

    TransitionCache m_transitionCache;
    TransitionValue m_exampleYPosition;

  public:
    explicit Shared(const DemoAppConfig& config);
    ~Shared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    // From EventListener
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;


    void OnKeyEvent(const KeyEvent& event);
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& demoTime);
    void Draw();

  private:
    void SetDefaultValues();
    void DrawExamplePx(INativeBatch2D& rNativeBatch, const PxRectangle2D& dstRect, const Resources& resources, const StringViewLite& caption,
                       const bool useTestAtlas);
    void DrawExampleDp(INativeBatch2D& rNativeBatch, const PxRectangle2D& dstRect, const Resources& resources, const StringViewLite& caption,
                       const bool useTestAtlas);
    void DrawExampleDpFloat(INativeBatch2D& rNativeBatch, const PxRectangle2D& dstRect, const Resources& resources, const StringViewLite& caption,
                            const bool useTestAtlas);

    int32_t CalcPxAreaHeightPx(const Resources& resources) const;
    int32_t CalcTextAreaHeightPx(const Resources& resources) const;
    void DrawTextArea(INativeBatch2D& rNativeBatch, const PxRectangle2D& dstRect, const Resources& resources, const bool useTestAtlas);
    void DrawText(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont, const StringViewLite& text,
                  const PxPoint2& dstPositionPx, const Color& fontColor, const PxClipRectangle& clipRectPx);
    void DrawTextNaiveScaling(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                              const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx, const Color& fontColor,
                              const PxClipRectangle& clipRectPx);
    void DrawTextDstRoundedToFullPixels(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                        const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                        const Color& fontColor, const PxClipRectangle& clipRectPx);
    void DrawTextBaseLineAware(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                               const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx, const Color& fontColor,
                               const PxClipRectangle& clipRectPx);
    void DrawTextAlmostPixelPerfect(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                    const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                    const Color& fontColor, const PxClipRectangle& clipRectPx);
    void DrawTextAlmostPixelPerfect2(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                     const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                     const Color& fontColor, const PxClipRectangle& clipRectPx);

    void DrawTextPixelPerfect(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                              const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx, const Color& fontColor,
                              const PxClipRectangle& clipRectPx);

    void DrawTextRenderRules(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                             const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx, const Color& fontColor,
                             const PxClipRectangle& clipRectPx);


    UIRecord CreateUI(const std::shared_ptr<UI::WindowContext>& context, UI::Theme::IThemeControlFactory& rUIFactory, const uint32_t densityDpi,
                      const bool enableTestPattern, const bool enableUITestPattern);
    void UpdateResourceScale();
    const Resources& SelectResource(const uint32_t density, const float preferenceWeight);
  };
}

#endif
