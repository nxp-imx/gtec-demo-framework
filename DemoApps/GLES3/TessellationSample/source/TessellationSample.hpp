#ifndef GLES3_TESSELLATIONSAMPLE_TESSELLATIONSAMPLE_HPP
#define GLES3_TESSELLATIONSAMPLE_TESSELLATIONSAMPLE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslUtil/OpenGLES3/NativeBatch2D.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <deque>
#include <memory>
#include <vector>
#include "AScene.hpp"
#include "RenderDrawConfig.hpp"
#include "RenderMaterial.hpp"
#include "RenderTessellationConfig.hpp"

namespace Fsl
{
  class IGraphicsService;
  class OptionParser;
  class SliderControl;

  namespace UI
  {
    class Label;
    class LabelNineSliceButton;
    class RadioButton;
    class StackLayout;
    class Switch;
    class WindowContext;

    namespace Theme
    {
      class BasicThemeFactory;
    }
  }


  struct SceneRecord
  {
    std::shared_ptr<AScene> Scene;
    std::shared_ptr<UI::RadioButton> Button;
  };

  class TessellationSample
    : public DemoAppGLES3
    , public UI::EventListener
  {
    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<GLES3::NativeBatch2D> m_nativeBatch;

    TextureAtlasBitmapFont m_font;

    UI::CallbackEventListenerScope m_uiEventListener;
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<UI::WindowContext> m_context;

    // std::shared_ptr<UI::Label> m_caption;
    std::shared_ptr<UI::Switch> m_checkDisplacement;
    std::shared_ptr<UI::Switch> m_checkWireframe;
    std::shared_ptr<UI::Switch> m_checkTexture;
    std::shared_ptr<UI::Switch> m_checkRotate;
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> m_sliderTInner;
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> m_sliderTOuter;
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> m_sliderTDispFactor;
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> m_sliderTDispMod;
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> m_sliderShininess;
    std::shared_ptr<UI::StackLayout> m_buttonStack;

    std::deque<SceneRecord> m_scenes;
    std::shared_ptr<AScene> m_scene;
    RenderDrawConfig m_drawConfig;
    RenderTessellationConfig m_tessellationConfig;
    RenderMaterial m_material;

  public:
    explicit TessellationSample(const DemoAppConfig& config);
    ~TessellationSample() override;

    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void OnMouseWheelEvent(const MouseWheelEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

    void BuildUI(UI::Theme::BasicThemeFactory& factory);
    void UpdateCaption();
    void UpdateUIWithSceneSettings();
  };
}

#endif
