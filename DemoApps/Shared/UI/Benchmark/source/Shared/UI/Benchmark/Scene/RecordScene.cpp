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

#include "RecordScene.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/SimpleImageButton.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformStackLayout.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Benchmark/App/TestAppFactory.hpp>
#include <Shared/UI/Benchmark/App/TestAppHost.hpp>
#include "ISceneTestDemoControl.hpp"
#include "Input/InputRecorder.hpp"
#include "Input/InputRecordingManager.hpp"
#include "SceneCreateInfo.hpp"
#include "TestHostForwarderTweak.hpp"

namespace Fsl
{
  RecordScene::RecordScene(const SceneCreateInfo& createInfo, std::shared_ptr<InputRecordingManager> inputRecordingManager)
    : BasicTestScene(createInfo)
    , m_inputRecordingManager(std::move(inputRecordingManager))
    , m_anim(UI::DefaultAnim::ColorChangeTime)
  {
    TryLoad();

    m_ui = CreateUI(*m_uiControlFactory);
    m_rootLayout->AddChild(m_ui.Main);
    RestartTestApp();
  }


  RecordScene::~RecordScene() = default;


  void RecordScene::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    assert(!theEvent->IsHandled());

    if (m_state == State::Ready)
    {
      if (theEvent->GetSource() == m_ui.ButtonRecord)
      {
        theEvent->Handled();
        StartRecord();
      }
      else if (theEvent->GetSource() == m_ui.ButtonPlay)
      {
        theEvent->Handled();
        StartPlay();
      }
    }
    else if (m_state == State::Recording)
    {
      if (theEvent->GetSource() == m_ui.ButtonRecord || theEvent->GetSource() == m_ui.ButtonStop)
      {
        theEvent->Handled();
        StopRecord();
      }
    }
    else if (m_state == State::Playing)
    {
      if (theEvent->GetSource() == m_ui.ButtonPlay || theEvent->GetSource() == m_ui.ButtonStop)
      {
        theEvent->Handled();
        StopPlay();
      }
    }

    // Finally check the back button if its allowed
    if (AllowBackKey() && theEvent->GetSource() == m_ui.ButtonBack)
    {
      theEvent->Handled();
      ScheduleClose();
    }
    BasicTestScene::OnSelect(theEvent);
  }

  void RecordScene::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    BasicTestScene::OnContentChanged(theEvent);
  }


  void RecordScene::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsPressed())
    {
      if (m_state == State::Ready)
      {
        switch (event.GetKey())
        {
        case VirtualKey::R:
          event.Handled();
          StartRecord();
          break;
        case VirtualKey::P:
          event.Handled();
          StartPlay();
          break;
        default:
          break;
        }
      }
      else if (m_state == State::Recording)
      {
        switch (event.GetKey())
        {
        case VirtualKey::Escape:
        case VirtualKey::R:
        case VirtualKey::S:
          event.Handled();
          StopRecord();
          break;
        default:
          break;
        }
      }
      else if (m_state == State::Playing)
      {
        switch (event.GetKey())
        {
        case VirtualKey::S:
        case VirtualKey::P:
          event.Handled();
          StopPlay();
          break;
        default:
          break;
        }
      }
    }
    BasicTestScene::OnKeyEvent(event);
  }


  void RecordScene::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    BasicTestScene::OnConfigurationChanged(windowMetrics);

    switch (m_state)
    {
    case State::Recording:
      // Force stop recording if the configuration changes
      StopRecord();
      break;
    default:
      break;
    }
  }


  void RecordScene::Update(const DemoTime& demoTime)
  {
    BasicTestScene::Update(demoTime);

    const auto& demoControl = GetDemoControl();
    if (m_state == State::Playing && !demoControl.IsPlaying())
    {
      StopPlay();
    }

    const auto frameCount = demoControl.FrameCount();
    const auto playFrameIndex = demoControl.PlayFrameIndex();
    const bool hasRecording = frameCount > 0;
    m_ui.ButtonPlay->SetEnabled((m_state == State::Ready || m_state == State::Playing) && hasRecording);
    m_ui.ButtonStop->SetEnabled(m_state == State::Recording || m_state == State::Playing);
    m_ui.ButtonRecord->SetEnabled(m_state == State::Ready || m_state == State::Recording);
    m_ui.ButtonBack->SetEnabled(AllowBackKey());

    m_anim.ColorButtonBack.SetValue(m_state == State::Ready || m_state == State::Playing ? UI::UIColors::White() : UI::UIColors::TransparentWhite());
    m_anim.ColorButtonRecord.SetValue(m_state == State::Recording ? UI::UIColors::Red() : UI::UIColors::White());
    m_anim.Update(demoTime.ElapsedTime);

    m_ui.ButtonRecord->SetBaseColor(m_anim.ColorButtonRecord.GetValue());
    m_ui.ButtonBack->SetBaseColor(m_anim.ColorButtonBack.GetValue());

    m_ui.FrameCount->SetContent(m_state == State::Playing ? playFrameIndex : frameCount);
  }


  bool RecordScene::Resolve(const DemoTime& demoTime)
  {
    return BasicTestScene::Resolve(demoTime);
  }


  void RecordScene::Draw(const DemoTime& demoTime)
  {
    BasicTestScene::Draw(demoTime);
  }

  void RecordScene::OnFrameSequenceEnd()
  {
    BasicTestScene::OnFrameSequenceEnd();
  }

  void RecordScene::RestartTestApp()
  {
    const bool depthBuffer = true;
    const bool useSdf = false;

    {    // restart the test app with the new render code
      m_testAppHost->StopTestApp();
      TestAppFactory appFactory(AppRenderMethod::FlexImBatch);

      UIDemoAppMaterialCreateInfo materialCreateInfo(false, true);
      UIDemoAppMaterialConfig materialConfig(useSdf, depthBuffer);
      m_testAppHost->StartTestApp(appFactory, materialCreateInfo, materialConfig);
    }
  }

  void RecordScene::OnBeginClose()
  {
    m_state = State::Closing;
  }

  RecordScene::UIRecord RecordScene::CreateUI(UI::Theme::IThemeControlFactory& controlFactory)
  {
    const UI::Theme::IThemeResources& resources = controlFactory.GetResources();
    auto context = controlFactory.GetContext();

    auto buttonBack = controlFactory.CreateFloatingButton(UI::Theme::FloatingButtonType::Back);

    auto buttonRecord = controlFactory.CreateImageButton(resources.GetIconSprite(UI::Theme::IconType::MediaRecord));
    buttonRecord->SetAlignmentX(UI::ItemAlignment::Stretch);
    buttonRecord->SetAlignmentY(UI::ItemAlignment::Stretch);
    auto buttonStop = controlFactory.CreateImageButton(resources.GetIconSprite(UI::Theme::IconType::MediaStop));
    buttonStop->SetAlignmentX(UI::ItemAlignment::Stretch);
    buttonStop->SetAlignmentY(UI::ItemAlignment::Stretch);
    auto buttonPlay = controlFactory.CreateImageButton(resources.GetIconSprite(UI::Theme::IconType::MediaPlay));
    buttonPlay->SetAlignmentX(UI::ItemAlignment::Stretch);
    buttonPlay->SetAlignmentY(UI::ItemAlignment::Stretch);
    buttonPlay->SetEnabled(false);


    auto frameLabel = controlFactory.CreateLabel("Frame:");
    auto frameCount = controlFactory.CreateFmtValueLabel(static_cast<uint32_t>(0), "{}");
    frameCount->SetAlignmentX(UI::ItemAlignment::Far);

    auto infoLayout = std::make_shared<UI::ComplexStackLayout>(context);
    infoLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    infoLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    infoLayout->AddChild(frameLabel, UI::LayoutLength(UI::LayoutUnitType::Auto));
    infoLayout->AddChild(frameCount, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

    auto layout = std::make_shared<UI::UniformStackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Horizontal);
    layout->AddChild(buttonRecord);
    layout->AddChild(buttonStop);
    layout->AddChild(buttonPlay);

    auto stackLayout = std::make_shared<UI::StackLayout>(context);
    stackLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    stackLayout->AddChild(infoLayout);
    stackLayout->AddChild(layout);

    auto background = controlFactory.CreateBackgroundWindow(UI::Theme::WindowType::DialogTransparent, stackLayout);
    background->SetAlignmentX(UI::ItemAlignment::Far);
    background->SetAlignmentY(UI::ItemAlignment::Far);

    auto mainLayout = std::make_shared<UI::FillLayout>(context);
    mainLayout->AddChild(buttonBack);
    mainLayout->AddChild(background);

    return {mainLayout, buttonRecord, buttonStop, buttonPlay, buttonBack, frameCount};
  }


  void RecordScene::StartPlay()
  {
    if (m_state == State::Ready)
    {
      const auto& demoControl = GetDemoControl();
      const bool hasRecording = demoControl.FrameCount() > 0;
      if (hasRecording)
      {
        RestartTestApp();

        auto& rDemoControl = GetDemoControl();
        assert(!rDemoControl.IsPlaying());
        rDemoControl.PlayBegin();

        SetState(State::Playing);
      }
    }
  }


  void RecordScene::StopPlay()
  {
    if (m_state == State::Playing)
    {
      auto& rDemoControl = GetDemoControl();
      rDemoControl.PlayEnd();

      SetState(State::Ready);
    }
  }


  void RecordScene::StartRecord()
  {
    if (m_state == State::Ready)
    {
      RestartTestApp();

      auto& rDemoControl = GetDemoControl();
      assert(!rDemoControl.IsRecording());
      rDemoControl.RecordBegin();

      SetState(State::Recording);
    }
  }


  void RecordScene::StopRecord()
  {
    if (m_state == State::Recording)
    {
      auto& rDemoControl = GetDemoControl();
      assert(rDemoControl.IsRecording());
      rDemoControl.RecordEnd();
      TrySave();
      SetState(State::Ready);
    }
  }


  void RecordScene::SetState(const State state)
  {
    if (m_state != state)
    {
      m_state = state;
      SetAllowBackKey(m_state == State::Ready || m_state == State::Playing);
    }
  }


  bool RecordScene::TryLoad()
  {
    const std::optional<AppInputCommandList> res = m_inputRecordingManager->TryGetRecording();
    if (res.has_value())
    {
      GetDemoControl().SetRecording(res.value().AsSpan(), res.value().GetFrameCount());
    }
    return res.has_value();
  }


  bool RecordScene::TrySave()
  {
    const auto& demoControl = GetDemoControl();
    const auto windowMetrics = GetwindowMetrics();
    AppInputCommandList commandList(windowMetrics.GetSizePx(), windowMetrics.DensityDpi, demoControl.GetRecordingAsSpan(), demoControl.FrameCount());
    return m_inputRecordingManager->TrySetRecording(commandList);
  }


}
