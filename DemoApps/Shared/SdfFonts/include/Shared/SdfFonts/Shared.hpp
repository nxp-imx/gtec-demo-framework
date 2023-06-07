#ifndef SHARED_SDFFONTS_SHARED_HPP
#define SHARED_SDFFONTS_SHARED_HPP
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
#include <FslGraphics/TextureAtlas/TextureAtlasMap.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
// #include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
// #include <FslSimpleUI/Base/System/EventListener.hpp>
// #include <FslSimpleUI/Base/System/UIManager.hpp>
#include <fmt/format.h>
#include <memory>

namespace Fsl
{
  namespace UI::Theme
  {
    class IThemeControlFactory;
  }

  enum class SdfFontMode
  {
    Normal = 0,
    Outline = 1,
    Shadow = 2,
    ShadowAndOutline = Outline | Shadow
  };

  constexpr inline SdfFontMode& operator|=(SdfFontMode& rLhs, const SdfFontMode rhs)
  {
    rLhs = static_cast<SdfFontMode>(static_cast<uint32_t>(rLhs) | static_cast<uint32_t>(rhs));
    return rLhs;
  }

  struct FontDrawConfig
  {
    int32_t FontSdfSpread{};
    float FontScale{};

    float OutlineDistance{};
    Vector2 ShadowOffset{};
    float ShadowSmoothing{};
  };

  class BasicImageSprite;
  class IBasicRenderSystem;
  class IGraphicsService;

  class Shared : public UI::EventListener
  {
    struct UIRecord
    {
      std::shared_ptr<UI::Switch> KerningCheckBox;
      std::shared_ptr<UI::Switch> DrawBoundingBoxesCheckBox;
      std::shared_ptr<UI::Switch> DrawOutlineCheckBox;
      std::shared_ptr<UI::Switch> DrawShadowCheckBox;
      std::shared_ptr<UI::SliderAndFmtValueLabel<float>> FontScaleSlider;
      std::shared_ptr<UI::Label> OutlineSliderLabel;
      std::shared_ptr<UI::SliderAndFmtValueLabel<float>> OutlineDistanceSlider;
      std::shared_ptr<UI::Label> ShadowSmoothingLabel;
      std::shared_ptr<UI::SliderAndFmtValueLabel<float>> ShadowSmoothingSlider;
      std::shared_ptr<UI::Label> ShadowOffsetXLabel;
      std::shared_ptr<UI::SliderAndFmtValueLabel<float>> ShadowOffsetXSlider;
      std::shared_ptr<UI::Label> ShadowOffsetYLabel;
      std::shared_ptr<UI::SliderAndFmtValueLabel<float>> ShadowOffsetYSlider;
      std::shared_ptr<UI::BackgroundLabelButton> ButtonDefault;
      std::shared_ptr<UI::BaseWindow> MainContent;
      std::shared_ptr<UI::Layout> MainLayout;
    };


    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<IGraphicsService> m_graphicsService;
    std::shared_ptr<IBasicRenderSystem> m_renderSystem;

    std::shared_ptr<BasicImageSprite> m_fillSprite;
    AtlasTexture2D m_fillTexture;
    UIRecord m_uiRecord;

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

    AtlasTexture2D GetFillTexture() const
    {
      return m_fillTexture;
    }

    PxPoint2 GetContentOffset() const;
    SdfFontMode GetSdfMode() const;
    bool GetKerningEnabled() const;
    bool GetBoundingBoxesEnabled() const;
    FontDrawConfig GetFontDrawConfig() const;

    void DrawBoundingBoxes(INativeBatch2D& nativeBatch, const PxPoint2& dstPositionPx, const StringViewLite& strView,
                           const TextureAtlasSpriteFont& font, const BitmapFontConfig& fontConfig,
                           std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad);

  private:
    void SetDefaultValues();
    void UpdateLinkedUIState();
    UIRecord CreateUI(const std::shared_ptr<UI::WindowContext>& context, UI::Theme::IThemeControlFactory& rUIFactory, const uint32_t densityDpi);
  };
}

#endif
