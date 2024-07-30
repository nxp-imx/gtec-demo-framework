#ifndef SHARED_HDR_BASICSCENE_MENUUI_HPP
#define SHARED_HDR_BASICSCENE_MENUUI_HPP
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

#include <FslBase/Transition/TransitionValue.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/Service/Keyboard/KeyboardState.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/CanvasLayout.hpp>
#include <FslSimpleUI/Base/UIColorSpace.hpp>
#include <Shared/HDR/BasicScene/OptionParser.hpp>
#include <string>

namespace Fsl
{
  class OptionParser;

  class MenuUI : public UI::EventListener
  {
    struct LocalGamepadState
    {
      bool B1 = false;
      bool B2 = false;
      bool B3 = false;
    };

    UI::UIColorSpace m_uiColorSpace;
    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<UI::CanvasLayout> m_rootCanvas;
    std::shared_ptr<UI::Label> m_labelTopLeft;
    std::shared_ptr<UI::Label> m_labelTopRight;
    std::shared_ptr<UI::Label> m_labelNote;
    std::shared_ptr<UI::BaseWindow> m_configWindow;
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> m_exposureSlider;
    std::shared_ptr<UI::Switch> m_checkboxLDR;
    std::shared_ptr<UI::Switch> m_checkboxHDR;

    SceneState m_state;

    TransitionValue m_scene1LabelAlpha;
    TransitionValue m_scene2LabelAlpha;

    LocalGamepadState m_gamepadState;

    std::string m_menuTextLDR;
    std::string m_menuTextHDR;
    std::string m_captionTopLeft;
    std::string m_captionTopRight;

  public:
    TransitionValue SplitX;

    explicit MenuUI(const DemoAppConfig& config, const UI::UIColorSpace uiColorSpace);
    ~MenuUI() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    void SetNoteLabel(const std::string& content);
    void SetCaptionLeft(const std::string& content);
    void SetCaptionRight(const std::string& content);
    void SetMenuTextLeft(const std::string& content);
    void SetMenuTextRight(const std::string& content);

    void OnKeyEvent(const KeyEvent& event);

    // virtual void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;

    void ToggleMenu();
    void ShowMenu(const bool enabled);

    void Update(const DemoTime& demoTime);
    void Draw();

    void UpdateExposureInput(const DemoTime& demoTime, const KeyboardState& keyboardState);

    float GetExposure() const;

    void SetExposure(const float value);

    SceneState GetState() const
    {
      return m_state;
    }

    void SetState(const SceneState newState);

  private:
    void UpdateUIState();
    void PrepareTransition();
    void BuildUI();
    void CreateMenuUI();
    void DestroyMenuUI();
    void UpdateStateBasedOnCheckboxes(const std::shared_ptr<UI::Switch>& source);
    std::shared_ptr<UI::CanvasLayout> CreateUI(const std::shared_ptr<UI::WindowContext>& context);
    std::shared_ptr<UI::BaseWindow> CreateConfigDialog(const std::shared_ptr<UI::WindowContext>& context, const UI::UIColorSpace colorSpace);
  };
}

#endif
