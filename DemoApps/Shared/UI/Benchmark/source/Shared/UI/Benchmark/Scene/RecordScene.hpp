#ifndef SHARED_UI_BENCHMARK_SCENE_RECORDSCENE_HPP
#define SHARED_UI_BENCHMARK_SCENE_RECORDSCENE_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
#include <FslGraphics/Transition/TransitionColor.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include "BasicTestScene.hpp"

namespace Fsl
{
  class InputRecordingManager;

  namespace UI
  {
    class BaseWindow;
    class ButtonBase;
  }

  class RecordScene final : public BasicTestScene
  {
    enum class State
    {
      Ready,
      Recording,
      Playing,
      Closing,
    };

    struct TransitionRecord
    {
      TransitionColor ColorButtonBack;
      TransitionColor ColorButtonRecord;

      TransitionRecord(TransitionCache& rTransitionCache, const TransitionTimeSpan& time)
        : ColorButtonBack(rTransitionCache, time, TransitionType::Smooth)
        , ColorButtonRecord(rTransitionCache, time, TransitionType::Smooth)
      {
      }

      void Update(const TransitionTimeSpan& deltaTime)
      {
        ColorButtonBack.Update(deltaTime);
        ColorButtonRecord.Update(deltaTime);
      }
    };

    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;
      std::shared_ptr<UI::ButtonBase> ButtonRecord;
      std::shared_ptr<UI::ButtonBase> ButtonStop;
      std::shared_ptr<UI::ButtonBase> ButtonPlay;
      std::shared_ptr<UI::ButtonBase> ButtonBack;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> FrameCount;
    };

    std::shared_ptr<InputRecordingManager> m_inputRecordingManager;
    UIRecord m_ui;
    TransitionRecord m_anim;
    State m_state{State::Ready};

  public:
    explicit RecordScene(const SceneCreateInfo& createInfo, std::shared_ptr<InputRecordingManager> inputRecordingManager);
    ~RecordScene() final;

    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;
    void OnKeyEvent(const KeyEvent& event) final;
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    bool Resolve(const DemoTime& demoTime) final;
    void Draw(const DemoTime& demoTime) final;
    void OnFrameSequenceEnd() final;

  private:
    void RestartTestApp();

    void OnBeginClose() override;

    static UIRecord CreateUI(UI::Theme::IThemeControlFactory& controlFactory);

    void StartPlay();
    void StopPlay();
    void StartRecord();
    void StopRecord();
    void SetState(const State state);
    bool TryLoad();
    bool TrySave();
  };
}

#endif
