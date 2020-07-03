#ifndef SHARED_HDR_SKYBOXTONEMAPPING_MENUUI_HPP
#define SHARED_HDR_SKYBOXTONEMAPPING_MENUUI_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/Transition/TransitionValue.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/Service/Keyboard/IKeyboard.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/CanvasLayout.hpp>
#include "OptionParser.hpp"
#include "RenderRecord.hpp"
#include "Tonemapper.hpp"
#include <vector>

namespace Fsl
{
  class MenuUI : public UI::EventListener
  {
    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<UI::CanvasLayout> m_rootCanvas;
    std::shared_ptr<UI::BaseWindow> m_configWindow;
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> m_exposureSlider;
    std::vector<std::shared_ptr<UI::Switch>> m_checkboxes;

    std::shared_ptr<INativeBatch2D> m_nativeBatch;
    std::shared_ptr<SpriteFont> m_defaultFont;

    DemoWindowMetrics m_windowMetrics;

    TransitionCache m_transitionCache;
    std::vector<RenderRecord> m_render;
    uint32_t m_sceneRenderFlags = 0;

  public:
    MenuUI(const DemoAppConfig& config, const std::size_t entries);

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    void OnKeyEvent(const KeyEvent& event);
    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;


    void Update(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);

    void UpdateExposureInput(const DemoTime& demoTime, const KeyboardState& keyboardState);

    float GetExposure() const;

    const std::vector<RenderRecord>& GetRenderRecords() const
    {
      return m_render;
    }

    void SetWindowMetrics(const DemoWindowMetrics& windowMetrics)
    {
      m_windowMetrics = windowMetrics;
      SetStateViaFlags(m_sceneRenderFlags, true);
    }

  private:
    void DrawLabels();
    void UpdateStateBasedOnCheckboxes(const std::shared_ptr<UI::Switch>& source);
    void ToggleState(const SceneFlags newState);
    void SetState(const SceneFlags newState, const bool enabled);
    void SetStateViaFlags(const uint32_t flags, const bool forceInstant = false);
    void UpdateUIState();
    void UpdateSceneTransition(const DemoTime& demoTime);
    void PrepareTransition(std::vector<RenderRecord>& rRender, const std::size_t entries);
    void ForceCompleteTransitions(std::vector<RenderRecord>& rRender);
    std::shared_ptr<UI::CanvasLayout> CreateUI();
    std::shared_ptr<UI::BaseWindow> CreateConfigDialog(const std::vector<RenderRecord>& render);
  };
}

#endif
