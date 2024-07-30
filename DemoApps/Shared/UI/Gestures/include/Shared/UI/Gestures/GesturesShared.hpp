#ifndef SHARED_UI_GESTURES_GESTURESSHARED_HPP
#define SHARED_UI_GESTURES_GESTURESSHARED_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/UI/Gestures/MoveableRectangles.hpp>
#include <fmt/format.h>
#include <memory>
#include <utility>

namespace Fsl
{
  class DemoAppConfig;
  struct DemoTime;
  class IGraphicsService;
  class KeyEvent;
  class MouseButtonEvent;
  class MouseMoveEvent;
  class SpriteNativeAreaCalc;

  class GesturesShared final : public UI::EventListener
  {
    struct UIInfoRecord
    {
      std::shared_ptr<UI::BaseWindow> MainLayout;
      std::shared_ptr<UI::Label> LblWinPx;
      std::shared_ptr<UI::Label> LblWinDp;
      std::shared_ptr<UI::Label> LblWinDpi;
      std::shared_ptr<UI::Label> LblFps;
      std::shared_ptr<UI::Label> LblFpsTime;
      std::shared_ptr<UI::Image> ImageIdle;

      UIInfoRecord() = default;

      UIInfoRecord(std::shared_ptr<UI::BaseWindow> mainLayout, std::shared_ptr<UI::Label> lblWinPx, std::shared_ptr<UI::Label> lblWinDp,
                   std::shared_ptr<UI::Label> lblWinDpi, std::shared_ptr<UI::Label> lblFps, std::shared_ptr<UI::Label> lblFpsTime,
                   std::shared_ptr<UI::Image> imageIdle)
        : MainLayout(std::move(mainLayout))
        , LblWinPx(std::move(lblWinPx))
        , LblWinDp(std::move(lblWinDp))
        , LblWinDpi(std::move(lblWinDpi))
        , LblFps(std::move(lblFps))
        , LblFpsTime(std::move(lblFpsTime))
        , ImageIdle(std::move(imageIdle))
      {
      }
    };

    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> MainLayout;
      std::shared_ptr<UI::Slider<float>> SliderBounceSpringStiffness;
      std::shared_ptr<UI::Slider<float>> SliderBounceAnimationTime;
      std::shared_ptr<UI::Slider<float>> SliderFlickDecelerationDpPerSecond;
      std::shared_ptr<UI::Slider<float>> SliderFlickAnimationTime;
      std::shared_ptr<UI::CheckBox> CheckBoxShowFramePacingX;
      std::shared_ptr<UI::CheckBox> CheckBoxShowFramePacingY;
      std::shared_ptr<UI::BackgroundLabelButton> BtnSetDefaults;
      std::shared_ptr<UI::MoveableRectangles> MoveableRects;
      UIInfoRecord Stats;

      UIRecord() = default;

      UIRecord(std::shared_ptr<UI::BaseWindow> mainLayout, std::shared_ptr<UI::Slider<float>> sliderBounceSpringStiffness,
               std::shared_ptr<UI::Slider<float>> sliderBounceAnimationTime, std::shared_ptr<UI::Slider<float>> sliderFlickDecelerationDpPerSecond,
               std::shared_ptr<UI::Slider<float>> sliderFlickAnimationTime, std::shared_ptr<UI::CheckBox> checkBoxShowFramePacingX,
               std::shared_ptr<UI::CheckBox> checkBoxShowFramePacingY, std::shared_ptr<UI::BackgroundLabelButton> btnSetDefaults,
               std::shared_ptr<UI::MoveableRectangles> moveableRects, UIInfoRecord stats)
        : MainLayout(std::move(mainLayout))
        , SliderBounceSpringStiffness(std::move(sliderBounceSpringStiffness))
        , SliderBounceAnimationTime(std::move(sliderBounceAnimationTime))
        , SliderFlickDecelerationDpPerSecond(std::move(sliderFlickDecelerationDpPerSecond))
        , SliderFlickAnimationTime(std::move(sliderFlickAnimationTime))
        , CheckBoxShowFramePacingX(std::move(checkBoxShowFramePacingX))
        , CheckBoxShowFramePacingY(std::move(checkBoxShowFramePacingY))
        , BtnSetDefaults(std::move(btnSetDefaults))
        , MoveableRects(std::move(moveableRects))
        , Stats(std::move(stats))
      {
      }
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<IProfilerService> m_profiler;

    DemoWindowMetrics m_cachedWindowMetrics;
    SpriteUnitConverter m_spriteUnitConverter;

    bool m_wasUIIdle{false};

    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<IBasicRenderSystem> m_renderSystem;
    std::shared_ptr<INativeBatch2D> m_batch;

    UIRecord m_ui;

    std::shared_ptr<ImageSprite> m_fillSprite;
    AtlasTexture2D m_texFill;


    fmt::memory_buffer m_resolutionBuffer1;
    fmt::memory_buffer m_resolutionBuffer2;
    fmt::memory_buffer m_resolutionBuffer3;
    fmt::memory_buffer m_fpsBuffer1;
    fmt::memory_buffer m_fpsBuffer2;

    PxPoint2 m_position0Px;
    PxPoint2 m_position1Px;
    PxValue m_add0Px;
    PxValue m_add1Px;

  public:
    explicit GesturesShared(const DemoAppConfig& config);
    ~GesturesShared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);

    void OnMouseButtonEvent(const MouseButtonEvent& event);
    void OnMouseMoveEvent(const MouseMoveEvent& event);
    void OnKeyEvent(const KeyEvent& event);
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;

    void Update(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);
    void OnFrameSequenceEnd();

  private:
    void UpdateDuringDrawFramePacingAnimation(const PxRectangle collisionRectanglePx);
    void RebuildStatsCache();
    void SetToDefaults();

    UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory);
    UIInfoRecord CreateUIInfo(UI::Theme::IThemeControlFactory& uiFactory);
  };
}


#endif
