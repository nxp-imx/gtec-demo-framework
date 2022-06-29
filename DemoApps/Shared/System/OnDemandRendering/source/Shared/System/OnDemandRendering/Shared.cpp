/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/Service/DemoAppControl/IDemoAppControl.hpp>
#include <FslDemoService/CpuStats/ICpuStatsService.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/App/UISpriteToTextureUtil.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformStackLayout.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Controls/Charts/Common/ChartGridLinesFps.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/System/OnDemandRendering/Chart/CustomChartGridLines.hpp>
#include <Shared/System/OnDemandRendering/Shared.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");

      constexpr bool SwitchOnDemandDefault = true;

      constexpr uint16_t DefaultFrameInterval = 30;

      constexpr uint16_t MinFrameInterval = 1;
      constexpr uint16_t MaxFrameInterval = 1000;

      constexpr uint16_t MinFixedUpdatesPerSecond = 1;
      constexpr uint16_t MaxFixedUpdatesPerSecond = 1000;

      constexpr float GraphSizeDp = 100.0f;
      constexpr float GraphSpacingDp = 4.0;

      constexpr uint32_t AverageEntries = 20;
      constexpr uint32_t MaxProfileDataEntries = 1;

      constexpr uint32_t FastSpeedFixedUpdatesPerSecond = 10;
      constexpr uint32_t SlowSpeedFixedUpdatesPerSecond = 1;
      constexpr uint32_t FastSpeedIdleFrameInterval = 10;
      constexpr uint32_t SlowSpeedIdleFrameInterval = 1;

      constexpr uint32_t FastAnimFramesPerSecond = 10;
      constexpr uint32_t SlowAnimFramesPerSecond = 1;

      constexpr Color ChartColor(0xFF3488A7);    // light blue

      constexpr bool DefaultValueAnimate = false;
      constexpr ConstrainedValue<uint32_t> AnimationExpectedFps(60, 1, 1000);
    }


    namespace IdleColor
    {
      constexpr Color Idle = Color::Green();
      constexpr Color Busy = Color::Red();
    }

    void SetContent(UI::FmtValueLabel<float>* pTarget, const double value)
    {
      if (pTarget != nullptr)
      {
        pTarget->SetContent(static_cast<float>(value));
      }
    }
  }

  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_profilerService(config.DemoServiceProvider.Get<IProfilerService>())
    , m_cpuStatsService(config.DemoServiceProvider.TryGet<ICpuStatsService>())
    , m_nativeBatch(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D())
    , m_renderSystem(config.DemoServiceProvider.Get<IGraphicsService>()->GetBasicRenderSystem())
    , m_defaults(m_demoAppControl->GetFixedUpdatesPerSecond(), m_demoAppControl->GetOnDemandFrameInterval())
    , m_dataUpdate(std::make_shared<UI::ChartData>(m_uiExtension->GetDataBinding(), config.WindowMetrics.ExtentPx.Width,
                                                   LocalConfig::MaxProfileDataEntries, UI::ChartData::Constraints(0, {})))
    , m_dataUpdateAverage(LocalConfig::AverageEntries)
    , m_dataDraw(std::make_shared<UI::ChartData>(m_uiExtension->GetDataBinding(), config.WindowMetrics.ExtentPx.Width,
                                                 LocalConfig::MaxProfileDataEntries, UI::ChartData::Constraints(0, {})))
    , m_dataDrawAverage(LocalConfig::AverageEntries)
    , m_dataFixedUpdate(std::make_shared<UI::ChartData>(m_uiExtension->GetDataBinding(), config.WindowMetrics.ExtentPx.Width,
                                                        LocalConfig::MaxProfileDataEntries, UI::ChartData::Constraints(0, 2)))
    , m_dataFixedUpdateAverage(LocalConfig::AverageEntries)
    , m_onDemandRendering(LocalConfig::SwitchOnDemandDefault, LocalConfig::DefaultFrameInterval, m_defaults.OnDemandFrameInterval)
  {
    m_dataUpdate->SetChannelMetaData(0, LocalConfig::ChartColor);
    m_dataDraw->SetChannelMetaData(0, LocalConfig::ChartColor);
    m_dataFixedUpdate->SetChannelMetaData(0, LocalConfig::ChartColor);


    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    const auto hasCpuUsage = static_cast<bool>(m_cpuStatsService);

    m_ui =
      CreateUI(*uiControlFactory, m_dataUpdate, m_dataDraw, m_dataFixedUpdate, m_defaults.FixedUpdatesPerSecond, m_onDemandRendering, hasCpuUsage);
    m_uiExtension->SetMainWindow(m_ui.MainWindow);

    m_fillSprite = uiControlFactory->GetResources().GetBasicFillSprite();

    OnConfigurationChanged(config.WindowMetrics);
  }


  Shared::~Shared() = default;


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::C:
      ClearCharts();
      break;
      // case VirtualKey::T:
      //  m_ui.TestPatternCheckBox->Toggle();
      //  break;
      // case VirtualKey::I:
      //  m_ui.DrawImageCheckBox->Toggle();
      //  break;
    case VirtualKey::O:
      ToggleOnDemandRendering();
      break;
    case VirtualKey::UpArrow:
      AddSpeedIdleFrameInterval(LocalConfig::FastSpeedIdleFrameInterval);
      break;
    case VirtualKey::DownArrow:
      SubSpeedIdleFrameInterval(LocalConfig::FastSpeedIdleFrameInterval);
      break;
    case VirtualKey::LeftArrow:
      SubSpeedIdleFrameInterval(LocalConfig::SlowSpeedIdleFrameInterval);
      break;
    case VirtualKey::RightArrow:
      AddSpeedIdleFrameInterval(LocalConfig::SlowSpeedIdleFrameInterval);
      break;
    case VirtualKey::W:
      AddFixedUpdatesPerSecond(LocalConfig::FastSpeedFixedUpdatesPerSecond);
      break;
    case VirtualKey::S:
      SubFixedUpdatesPerSecond(LocalConfig::FastSpeedFixedUpdatesPerSecond);
      break;
    case VirtualKey::A:
      SubFixedUpdatesPerSecond(LocalConfig::SlowSpeedFixedUpdatesPerSecond);
      break;
    case VirtualKey::D:
      AddFixedUpdatesPerSecond(LocalConfig::SlowSpeedFixedUpdatesPerSecond);
      break;

    case VirtualKey::R:
      ResetAnim();
      break;
    case VirtualKey::Y:
      ToggleAnimation();
      break;
    case VirtualKey::T:
      AddAnimationFramesPerSecond(LocalConfig::FastAnimFramesPerSecond);
      break;
    case VirtualKey::G:
      SubAnimationFramesPerSecond(LocalConfig::FastAnimFramesPerSecond);
      break;
    case VirtualKey::F:
      SubAnimationFramesPerSecond(LocalConfig::SlowAnimFramesPerSecond);
      break;
    case VirtualKey::H:
      AddAnimationFramesPerSecond(LocalConfig::SlowAnimFramesPerSecond);
      break;
    case VirtualKey::Space:
      SetDefaultValues();
      break;
    default:
      break;
    }
  }


  void Shared::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_ui.BtnDefault)
    {
      SetDefaultValues();
    }
    else if (theEvent->GetSource() == m_ui.BtnClearChart)
    {
      ClearCharts();
    }
    else if (theEvent->GetSource() == m_ui.Time.BtnReset)
    {
      ResetAnim();
    }
  }


  void Shared::OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);
    if (theEvent->GetSource() == m_ui.SliderIdleFrameInterval)
    {
      m_onDemandRendering.IdleFrameInterval = m_ui.SliderIdleFrameInterval->GetValue();
    }
    else if (theEvent->GetSource() == m_ui.SliderFixedUpdatesPerSecond)
    {
      const auto fixedUpdatesPerSecond = m_ui.SliderFixedUpdatesPerSecond->GetValue();
      m_demoAppControl->SetFixedUpdatesPerSecond(NumericCast<uint16_t>(fixedUpdatesPerSecond));
      ResetAnim();
    }
    else if (theEvent->GetSource() == m_ui.SwitchOnDemand)
    {
      SetOnDemandRendering(m_ui.SwitchOnDemand->IsChecked());
    }
    else if (theEvent->GetSource() == m_ui.Time.SwitchAnimate)
    {
      if (m_ui.Time.SwitchAnimate->IsChecked())
      {
        ResetAnim();
      }
    }
    else if (theEvent->GetSource() == m_ui.Time.SliderRefreshRate)
    {
      ResetAnim();
    }
  }


  void Shared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_cachedWindowMetrics = windowMetrics;
    m_fillTexture = UISpriteToTextureUtil::ExtractFillTexture(*m_renderSystem, m_fillSprite);
    m_jankDetector.OnConfigurationChanged(windowMetrics);
  }


  void Shared::FixedUpdate(const DemoTime& demoTime)
  {
    m_jankDetector.FixedUpdate(demoTime, m_ui.Time.SwitchAnimate->IsChecked(), m_ui.Time.SliderRefreshRate->GetValue());

    ++m_fixedUpdateCount;
  }

  void Shared::Update(const DemoTime& demoTime)
  {
    {
      // Fixed update call count
      UI::ChartDataEntry entry;
      entry.Values[0] = m_fixedUpdateCount;
      m_dataFixedUpdate->Append(entry);
      m_dataFixedUpdateAverage.Append(entry);
      m_fixedUpdateCount = 0;
    }
    {    // Update
      UI::ChartDataEntry entry;
      auto updateTime = TimeSpanUtil::ToClampedMicrosecondsUInt64(demoTime.ElapsedTime);
      entry.Values[0] = UncheckedNumericCast<uint32_t>(MathHelper::Clamp(updateTime, static_cast<uint64_t>(0), static_cast<uint64_t>(0xFFFFFFFF)));
      m_dataUpdate->Append(entry);
      m_dataUpdateAverage.Append(entry);
      {
        auto res = m_dataUpdateAverage.CalcAverage();
        if (!res.empty())
        {
          const auto average = static_cast<float>(res[0]);
          SetContent(m_ui.UpdateFmtLabel.get(), average);
        }
      }
    }
    {    // Draw average one frame more delayed than the rest
      auto res = m_dataDrawAverage.CalcAverage();
      if (!res.empty())
      {
        const auto average = static_cast<float>(res[0]);
        SetContent(m_ui.DrawFmtLabel.get(), average);
      }
    }

    {
      auto res = m_dataFixedUpdateAverage.CalcAverage();
      if (!res.empty())
      {
        const auto average = static_cast<float>(res[0]);
        SetContent(m_ui.FixedUpdateFmtLabel.get(), average);
      }
    }
    if (m_cpuStatsService && m_ui.Stats.OptionalCpuUsage)
    {
      float usagePercentage = 0.0f;
      if (m_cpuStatsService->TryGetApplicationCpuUsage(usagePercentage))
      {
        SetContent(m_ui.Stats.OptionalCpuUsage.get(), usagePercentage);
      }
    }


    // If we dont want to show the system UI idle/busy state we force the systemUIIdle to true
    const Color idleColor = m_cachedState.WasUIIdle ? IdleColor::Idle : IdleColor::Busy;
    m_ui.Stats.ImageIdle->SetContentColor(idleColor);
    m_ui.Stats.LabelFramerate->SetContent(m_profilerService->GetLastFrameTime().GetFramePerSecond());
    m_ui.Stats.LabelAverageFramerate->SetContent(m_profilerService->GetAverageFrameTime().GetFramePerSecond());
    m_ui.Stats.LabelFrameInterval->SetContent(m_demoAppControl->GetOnDemandFrameInterval());

    m_jankDetector.OnUpdate(demoTime);
  }

  void Shared::Resolve(const DemoTime& /*demoTime*/)
  {
    // Adjust on-demand frame interval
    const auto idleFrameInterval = m_onDemandRendering.CurrentInterval();
    const bool isIdle = m_uiExtension->IsIdle() && !m_ui.Time.SwitchAnimate->IsChecked();
    const auto desiredFrameInterval = isIdle ? idleFrameInterval : 1;
    m_demoAppControl->SetOnDemandFrameInterval(desiredFrameInterval);
  }


  void Shared::Draw(const DemoTime& demoTime)
  {
    m_jankDetector.OnDraw(demoTime);

    DrawTest(*m_nativeBatch, m_fillTexture, m_cachedWindowMetrics, m_jankDetector.GetRecord0(), m_jankDetector.GetRecord1(),
             m_jankDetector.Record1AsTimingSpan(), m_jankDetector.GetRecord2());


    {    // Draw
      UI::ChartDataEntry entry;
      auto drawTime = TimeSpanUtil::ToClampedMicrosecondsUInt64(demoTime.ElapsedTime);
      entry.Values[0] = UncheckedNumericCast<uint32_t>(MathHelper::Clamp(drawTime, static_cast<uint64_t>(0), static_cast<uint64_t>(0xFFFFFFFF)));
      m_dataDraw->Append(entry);
      m_dataDrawAverage.Append(entry);
    }

    m_uiExtension->Draw();
  }


  void Shared::OnFrameSequenceEnd()
  {
    m_cachedState.WasUIIdle = m_uiExtension->IsIdle();
  }


  void Shared::DrawTest(INativeBatch2D& batch, const AtlasTexture2D& fillTexture, const DemoWindowMetrics& windowMetrics,
                        const JankDetector::AnimationRecord0& record0, const JankDetector::AnimationRecord1& record1,
                        const ReadOnlySpan<JankDetector::TimingRecords> timingRecords, const JankDetector::AnimationRecord1& record2)
  {
    constexpr auto demoBlockSize = 40;
    constexpr auto demoBlockSpace = demoBlockSize / 4;
    constexpr auto smallDemoBlockSize = 32;
    constexpr auto smallDemoBlockSpace = smallDemoBlockSize / 4;
    const auto sizePx = windowMetrics.GetSizePx();
    const int32_t offsetPx = (sizePx.Width() - demoBlockSize) / 2;
    const int32_t lineOffset0Px = offsetPx - 1;
    const int32_t lineOffset1Px = offsetPx + demoBlockSize;
    const int32_t lineWidthPx = sizePx.Width();
    const int32_t lineHeightPx = (demoBlockSize * 4) + (demoBlockSpace * 5);

    const int32_t boxYAdd = demoBlockSize + demoBlockSpace;
    const int32_t smallBoxYAdd = smallDemoBlockSize + smallDemoBlockSpace;

    const int32_t smallStartPos = lineHeightPx + smallDemoBlockSpace;
    const int32_t smallStartPos2 = smallStartPos + smallBoxYAdd * 4;
    int32_t yPos = demoBlockSpace;

    constexpr Color color(Color::Black());
    constexpr Color lineColor(Color::White());

    const int32_t secondLineYPx = smallStartPos2 - (smallDemoBlockSpace / 2);

    batch.Begin();
    {
      {
        const int32_t box0Position = offsetPx + record0.Box0Position - record0.CameraPosition;
        const int32_t box1Position = offsetPx + record0.Box1Position - record0.CameraPosition;
        const int32_t box2Position = offsetPx + record0.Box2Position - record0.CameraPosition;
        const int32_t box3Position = offsetPx + record0.Box3Position - record0.CameraPosition;

        batch.DebugDrawLine(fillTexture, PxPoint2(lineOffset0Px, 0), PxPoint2(lineOffset0Px, lineHeightPx), lineColor);
        batch.DebugDrawLine(fillTexture, PxPoint2(lineOffset1Px, 0), PxPoint2(lineOffset1Px, lineHeightPx), lineColor);
        batch.DebugDrawLine(fillTexture, PxPoint2(0, lineHeightPx), PxPoint2(lineWidthPx, lineHeightPx), lineColor);
        batch.DebugDrawLine(fillTexture, PxPoint2(0, secondLineYPx), PxPoint2(lineWidthPx, secondLineYPx), lineColor);
        batch.Draw(fillTexture, PxRectangle(box0Position, yPos, demoBlockSize, demoBlockSize), color);
        yPos += boxYAdd;
        batch.Draw(fillTexture, PxRectangle(box1Position, yPos, demoBlockSize, demoBlockSize), color);
        yPos += boxYAdd;
        batch.Draw(fillTexture, PxRectangle(box2Position, yPos, demoBlockSize, demoBlockSize), color);
        yPos += boxYAdd;
        batch.Draw(fillTexture, PxRectangle(box3Position, yPos, demoBlockSize, demoBlockSize), color);
      }

      constexpr Color lineColor0(Color::Red());
      constexpr Color lineColor1(Color::Green());
      constexpr Color lineColor2(Color::Blue());
      constexpr Color lineColor3(Color::Cyan());
      {
        const int32_t smallBox0Position = record1.Box0Position;
        const int32_t smallBox1Position = record1.Box1Position;
        const int32_t smallBox2Position = record1.Box2Position;
        const int32_t smallBox3Position = record1.Box3Position;

        const int32_t lineStart = smallStartPos;
        const int32_t lineEnd = lineStart + (smallBoxYAdd * 3) + smallDemoBlockSize;
        for (std::size_t i = 0; i < timingRecords.size(); ++i)
        {
          const auto& entry = timingRecords[i];
          yPos = lineStart;
          batch.DebugDrawLine(fillTexture, PxPoint2(entry.Box0Position, yPos), PxPoint2(entry.Box0Position, lineEnd), lineColor0);
          yPos += smallBoxYAdd;
          batch.DebugDrawLine(fillTexture, PxPoint2(entry.Box1Position, yPos), PxPoint2(entry.Box1Position, lineEnd), lineColor1);
          yPos += smallBoxYAdd;
          batch.DebugDrawLine(fillTexture, PxPoint2(entry.Box2Position, yPos), PxPoint2(entry.Box2Position, lineEnd), lineColor2);
          yPos += smallBoxYAdd;
          batch.DebugDrawLine(fillTexture, PxPoint2(entry.Box3Position, yPos), PxPoint2(entry.Box3Position, lineEnd), lineColor3);
        }

        yPos = smallStartPos;
        batch.Draw(fillTexture, PxRectangle(smallBox0Position, yPos, smallDemoBlockSize, smallDemoBlockSize), color);
        yPos += smallBoxYAdd;
        batch.Draw(fillTexture, PxRectangle(smallBox1Position, yPos, smallDemoBlockSize, smallDemoBlockSize), color);
        yPos += smallBoxYAdd;
        batch.Draw(fillTexture, PxRectangle(smallBox2Position, yPos, smallDemoBlockSize, smallDemoBlockSize), color);
        yPos += smallBoxYAdd;
        batch.Draw(fillTexture, PxRectangle(smallBox3Position, yPos, smallDemoBlockSize, smallDemoBlockSize), color);
      }
      {
        const int32_t slowBox0Position = record2.Box0Position;
        const int32_t slowBox1Position = record2.Box1Position;
        const int32_t slowBox2Position = record2.Box2Position;
        const int32_t slowBox3Position = record2.Box3Position;

        yPos = smallStartPos2;
        int32_t lineStart = yPos;
        const int32_t lineEnd = lineStart + (smallBoxYAdd * 3) + smallDemoBlockSize;
        {
          const int32_t lineX1 = slowBox0Position - 1;
          const int32_t lineX2 = slowBox0Position + smallDemoBlockSize;
          batch.DebugDrawLine(fillTexture, PxPoint2(lineX1, lineStart - 2), PxPoint2(lineX1, lineEnd + 2), lineColor);
          batch.DebugDrawLine(fillTexture, PxPoint2(lineX2, lineStart - 2), PxPoint2(lineX2, lineEnd + 2), lineColor);
          batch.DebugDrawLine(fillTexture, PxPoint2(slowBox0Position - 6, lineStart), PxPoint2(slowBox0Position - 6, lineEnd), lineColor0);
          lineStart += smallBoxYAdd;
          batch.DebugDrawLine(fillTexture, PxPoint2(slowBox1Position - 6, lineStart), PxPoint2(slowBox1Position - 6, lineEnd), lineColor1);
          lineStart += smallBoxYAdd;
          batch.DebugDrawLine(fillTexture, PxPoint2(slowBox2Position - 6, lineStart), PxPoint2(slowBox2Position - 6, lineEnd), lineColor2);
          lineStart += smallBoxYAdd;
          batch.DebugDrawLine(fillTexture, PxPoint2(slowBox3Position - 6, lineStart), PxPoint2(slowBox3Position - 6, lineEnd), lineColor3);
        }

        batch.Draw(fillTexture, PxRectangle(slowBox0Position, yPos, smallDemoBlockSize, smallDemoBlockSize), color);
        yPos += smallBoxYAdd;
        batch.Draw(fillTexture, PxRectangle(slowBox1Position, yPos, smallDemoBlockSize, smallDemoBlockSize), color);
        yPos += smallBoxYAdd;
        batch.Draw(fillTexture, PxRectangle(slowBox2Position, yPos, smallDemoBlockSize, smallDemoBlockSize), color);
        yPos += smallBoxYAdd;
        batch.Draw(fillTexture, PxRectangle(slowBox3Position, yPos, smallDemoBlockSize, smallDemoBlockSize), color);
      }
    }
    batch.End();
  }


  void Shared::SetDefaultValues()
  {
    SetAnimDefaultValues();

    m_ui.SliderFixedUpdatesPerSecond->SetValue(m_defaults.FixedUpdatesPerSecond);
    m_ui.SliderIdleFrameInterval->SetValue(LocalConfig::SwitchOnDemandDefault ? LocalConfig::DefaultFrameInterval : m_defaults.OnDemandFrameInterval);
    SetOnDemandRendering(LocalConfig::SwitchOnDemandDefault);
  }

  void Shared::SetAnimDefaultValues()
  {
    m_ui.Time.SwitchAnimate->SetIsChecked(LocalConfig::DefaultValueAnimate);
    m_ui.Time.SliderRefreshRate->SetValue(LocalConfig::AnimationExpectedFps.Get());
    ResetAnim();
  }

  void Shared::ResetAnim()
  {
    m_jankDetector.ResetAnim();
  }

  void Shared::AddSpeedIdleFrameInterval(const uint32_t value)
  {
    m_ui.SliderIdleFrameInterval->AddValue(value);
  }


  void Shared::SubSpeedIdleFrameInterval(const uint32_t value)
  {
    m_ui.SliderIdleFrameInterval->SubValue(value);
  }


  void Shared::AddFixedUpdatesPerSecond(const uint32_t value)
  {
    m_ui.SliderFixedUpdatesPerSecond->AddValue(value);
  }


  void Shared::AddAnimationFramesPerSecond(const uint32_t value)
  {
    m_ui.Time.SliderRefreshRate->AddValue(value);
  }


  void Shared::SubAnimationFramesPerSecond(const uint32_t value)
  {
    m_ui.Time.SliderRefreshRate->SubValue(value);
  }


  void Shared::SubFixedUpdatesPerSecond(const uint32_t value)
  {
    m_ui.SliderFixedUpdatesPerSecond->SubValue(value);
  }


  void Shared::ToggleAnimation()
  {
    m_ui.Time.SwitchAnimate->Toggle();
  }

  void Shared::ToggleOnDemandRendering()
  {
    SetOnDemandRendering(!m_ui.SwitchOnDemand->IsChecked());
  }


  void Shared::SetOnDemandRendering(const bool enabled)
  {
    if (enabled != m_onDemandRendering.Enabled)
    {
      m_onDemandRendering.Enabled = enabled;
      m_ui.SwitchOnDemand->SetIsChecked(enabled);
      m_ui.SliderIdleFrameInterval->SetEnabled(enabled);
    }
  }


  void Shared::ClearCharts()
  {
    m_dataFixedUpdate->Clear();
    m_dataFixedUpdateAverage.Clear();
    m_dataUpdate->Clear();
    m_dataUpdateAverage.Clear();
    m_dataDraw->Clear();
    m_dataDrawAverage.Clear();
  }


  Shared::UIRecord Shared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::AChartData>& dataUpdate,
                                    const std::shared_ptr<UI::AChartData>& dataDraw, const std::shared_ptr<UI::AChartData>& dataFixedUpdate,
                                    const uint16_t fixedUpdatesPerSecond, const OnDemandState& onDemandRendering, const bool hasCpuStats)
  {
    auto context = uiFactory.GetContext();

    auto onDemandFrameInterval = onDemandRendering.IdleFrameInterval;

    auto labelIdleFrameInterval = uiFactory.CreateLabel("Idle framerate interval:");
    labelIdleFrameInterval->SetAlignmentY(UI::ItemAlignment::Center);
    auto sliderIdleFrameInterval =
      uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal,
                                     ConstrainedValue<uint32_t>(onDemandFrameInterval, LocalConfig::MinFrameInterval, LocalConfig::MaxFrameInterval));
    sliderIdleFrameInterval->SetAlignmentY(UI::ItemAlignment::Center);
    sliderIdleFrameInterval->SetEnabled(onDemandRendering.Enabled);
    sliderIdleFrameInterval->FinishAnimation();

    auto switchOnDemand = uiFactory.CreateSwitch("On demand", onDemandRendering.Enabled);
    switchOnDemand->SetAlignmentX(UI::ItemAlignment::Stretch);
    // switchOnDemand->SetAlignmentY(UI::ItemAlignment::Near);

    auto labelFixedUpdatesPerSecound = uiFactory.CreateLabel("Fixed updates per second:");
    labelFixedUpdatesPerSecound->SetAlignmentY(UI::ItemAlignment::Center);
    auto sliderFixedUpdatesPerSecound = uiFactory.CreateSliderFmtValue(
      UI::LayoutOrientation::Horizontal,
      ConstrainedValue<uint32_t>(fixedUpdatesPerSecond, LocalConfig::MinFixedUpdatesPerSecond, LocalConfig::MaxFixedUpdatesPerSecond));
    sliderFixedUpdatesPerSecound->SetAlignmentY(UI::ItemAlignment::Center);


    auto btnDefault = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Set defaults");
    btnDefault->SetAlignmentX(UI::ItemAlignment::Stretch);
    btnDefault->SetAlignmentY(UI::ItemAlignment::Far);
    auto btnClearChart = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Clear charts");
    btnClearChart->SetAlignmentX(UI::ItemAlignment::Stretch);
    btnClearChart->SetAlignmentY(UI::ItemAlignment::Near);

    auto updateLabel = uiFactory.CreateLabel("Update delta-time");
    auto updateFmtLabel = uiFactory.CreateFmtValueLabel(0.0f, u8"{:.0f}\u03BCs");
    updateFmtLabel->SetAlignmentX(UI::ItemAlignment::Far);
    updateFmtLabel->SetAlignmentY(UI::ItemAlignment::Far);

    auto gridLines = std::make_shared<UI::ChartGridLinesFps>();
    auto updateChart = std::make_shared<UI::AreaChart>(context);
    {
      updateChart->SetAlignmentX(UI::ItemAlignment::Stretch);
      updateChart->SetAlignmentY(UI::ItemAlignment::Stretch);
      updateChart->SetOpaqueFillSprite(uiFactory.GetResources().GetBasicFillSprite(true));
      updateChart->SetTransparentFillSprite(uiFactory.GetResources().GetBasicFillSprite(false));
      updateChart->SetGridLines(gridLines);
      updateChart->SetDataView(dataUpdate);
      updateChart->SetFont(uiFactory.GetResources().GetDefaultSpriteFont());
      updateChart->SetLabelBackground(uiFactory.GetResources().GetToolTipNineSliceSprite());
      updateChart->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
    }

    auto drawLabel = uiFactory.CreateLabel("Draw delta-time");
    auto drawFmtLabel = uiFactory.CreateFmtValueLabel(0.0f, u8"{:.0f}\u03BCs");
    drawFmtLabel->SetAlignmentX(UI::ItemAlignment::Far);
    drawFmtLabel->SetAlignmentY(UI::ItemAlignment::Far);
    auto drawChart = std::make_shared<UI::AreaChart>(context);
    {
      drawChart->SetAlignmentX(UI::ItemAlignment::Stretch);
      drawChart->SetAlignmentY(UI::ItemAlignment::Stretch);
      drawChart->SetOpaqueFillSprite(uiFactory.GetResources().GetBasicFillSprite(true));
      drawChart->SetTransparentFillSprite(uiFactory.GetResources().GetBasicFillSprite(false));
      drawChart->SetGridLines(gridLines);
      drawChart->SetDataView(dataDraw);
      drawChart->SetFont(uiFactory.GetResources().GetDefaultSpriteFont());
      drawChart->SetLabelBackground(uiFactory.GetResources().GetToolTipNineSliceSprite());
      drawChart->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
    }

    auto fixedUpdateLabel = uiFactory.CreateLabel("Fixed update count");
    auto fixedUpdateFmtLabel = uiFactory.CreateFmtValueLabel(0.0f, "{:.2f}");
    fixedUpdateFmtLabel->SetAlignmentX(UI::ItemAlignment::Far);
    fixedUpdateFmtLabel->SetAlignmentY(UI::ItemAlignment::Far);
    auto fixedUpdateChart = std::make_shared<UI::AreaChart>(context);
    {
      fixedUpdateChart->SetAlignmentX(UI::ItemAlignment::Stretch);
      fixedUpdateChart->SetAlignmentY(UI::ItemAlignment::Stretch);
      fixedUpdateChart->SetOpaqueFillSprite(uiFactory.GetResources().GetBasicFillSprite(true));
      fixedUpdateChart->SetTransparentFillSprite(uiFactory.GetResources().GetBasicFillSprite(false));
      fixedUpdateChart->SetGridLines(gridLines);
      fixedUpdateChart->SetDataView(dataFixedUpdate);
      fixedUpdateChart->SetFont(uiFactory.GetResources().GetDefaultSpriteFont());
      fixedUpdateChart->SetLabelBackground(uiFactory.GetResources().GetToolTipNineSliceSprite());
      fixedUpdateChart->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
    }

    auto play = CreatePlayUI(uiFactory, context);
    auto time = CreateDeltaTimeUI(uiFactory, context);
    auto stats = CreateStatsOverlayUI(uiFactory, context, hasCpuStats);

    auto bottomGrid = std::make_shared<UI::GridLayout>(context);
    bottomGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
    bottomGrid->SetAlignmentY(UI::ItemAlignment::Stretch);
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 8));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 8));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, LocalConfig::GraphSizeDp));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, LocalConfig::GraphSpacingDp));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, LocalConfig::GraphSizeDp));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, LocalConfig::GraphSpacingDp));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, LocalConfig::GraphSizeDp));
    bottomGrid->AddChild(labelIdleFrameInterval, 0, 0);
    bottomGrid->AddChild(sliderIdleFrameInterval, 2, 0);
    bottomGrid->AddChild(switchOnDemand, 4, 0);
    bottomGrid->AddChild(labelFixedUpdatesPerSecound, 0, 1);
    bottomGrid->AddChild(sliderFixedUpdatesPerSecound, 2, 1);
    bottomGrid->AddChild(fixedUpdateLabel, 0, 2);
    bottomGrid->AddChild(fixedUpdateFmtLabel, 0, 2);
    bottomGrid->AddChild(fixedUpdateChart, 2, 2);
    bottomGrid->AddChild(updateLabel, 0, 4);
    bottomGrid->AddChild(updateFmtLabel, 0, 4);
    bottomGrid->AddChild(updateChart, 2, 4);
    bottomGrid->AddChild(drawLabel, 0, 6);
    bottomGrid->AddChild(drawFmtLabel, 0, 6);
    bottomGrid->AddChild(drawChart, 2, 6);
    bottomGrid->AddChild(btnClearChart, 4, 6);
    bottomGrid->AddChild(btnDefault, 4, 6);

    auto bottomBar = uiFactory.CreateBottomBar(bottomGrid);

    auto playLayout = std::make_shared<UI::GridLayout>(context);
    playLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    playLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    playLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
    playLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    playLayout->AddChild(play.MainLayout, 0, 0);
    playLayout->AddChild(stats.MainOverlay, 0, 0);
    playLayout->AddChild(time.MainLayout, 0, 1);
    playLayout->AddChild(bottomBar, 0, 2);

    // auto fillLayout = std::make_shared<UI::FillLayout>(context);
    // fillLayout->AddChild(playLayout);
    // fillLayout->AddChild(stats.MainOverlay);
    // fillLayout->AddChild(bottomBar);

    return {playLayout,
            sliderIdleFrameInterval,
            sliderFixedUpdatesPerSecound,
            btnDefault,
            updateFmtLabel,
            drawFmtLabel,
            fixedUpdateFmtLabel,
            btnClearChart,
            switchOnDemand,
            play,
            time,
            stats};
  }


  Shared::PlayUI Shared::CreatePlayUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context)
  {
    auto grid = std::make_shared<UI::GridLayout>(context);
    grid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 200));
    grid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    grid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    grid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    auto switchDummy = uiFactory.CreateSwitch("Dummy", true);
    switchDummy->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto slider = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<uint32_t>(0, 0, 100));
    auto button = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Dummy");
    button->SetAlignmentX(UI::ItemAlignment::Stretch);

    grid->AddChild(switchDummy, 0, 0);
    grid->AddChild(slider, 0, 1);
    grid->AddChild(button, 0, 2);

    std::shared_ptr<UI::Background> mainLayout = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::DialogNormal, grid);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Near);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Near);
    return {mainLayout};
  }


  Shared::DeltaTimeUI Shared::CreateDeltaTimeUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context)
  {
    auto grid = std::make_shared<UI::GridLayout>(context);
    grid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    grid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1));
    grid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    grid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    auto switchAnimate = uiFactory.CreateSwitch("Animate", LocalConfig::DefaultValueAnimate);
    switchAnimate->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto sliderExpectedFps = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::AnimationExpectedFps);

    auto btnReset = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Reset anim");

    grid->AddChild(switchAnimate, 0, 0);
    grid->AddChild(sliderExpectedFps, 1, 0);
    grid->AddChild(btnReset, 2, 0);

    std::shared_ptr<UI::Background> mainLayout = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::DialogNormal, grid);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Far);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Far);
    return {mainLayout, switchAnimate, sliderExpectedFps, btnReset};
  }

  Shared::StatsOverlayUI Shared::CreateStatsOverlayUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context,
                                                      const bool hasCpuStats)
  {
    auto labelIdle = uiFactory.CreateLabel("UI idle");
    auto imageIdle = uiFactory.CreateImage(uiFactory.GetResources().GetColorMarkerNineSliceSprite());
    imageIdle->SetAlignmentX(UI::ItemAlignment::Far);
    imageIdle->SetAlignmentY(UI::ItemAlignment::Center);
    imageIdle->SetContentColor(IdleColor::Busy);
    imageIdle->FinishAnimation();


    StatsOverlayUI overlay;


    auto lblDesc0 = uiFactory.CreateLabel("Framerate:");
    auto lblDesc1 = uiFactory.CreateLabel("Average framerate:");
    auto lblDesc2 = uiFactory.CreateLabel("On demand frame interval:");

    auto lbl0 = uiFactory.CreateFmtValueLabel(0.0f, "{:.1f}");
    auto lbl1 = uiFactory.CreateFmtValueLabel(0.0f, "{:.1f}");
    auto lbl2 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(0));
    lbl0->SetAlignmentX(UI::ItemAlignment::Far);
    lbl1->SetAlignmentX(UI::ItemAlignment::Far);
    lbl2->SetAlignmentX(UI::ItemAlignment::Far);


    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 70.0f));

    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    layout->AddChild(labelIdle, 0, 0);
    layout->AddChild(lblDesc0, 0, 1);
    layout->AddChild(lblDesc1, 0, 2);
    layout->AddChild(lblDesc2, 0, 3);

    layout->AddChild(imageIdle, 1, 0);
    layout->AddChild(lbl0, 1, 1);
    layout->AddChild(lbl1, 1, 2);
    layout->AddChild(lbl2, 1, 3);

    std::shared_ptr<UI::FmtValueLabel<float>> optionalCpuUsage;
    if (hasCpuStats)
    {
      auto cpuUsage = uiFactory.CreateLabel("CPU usage:");
      optionalCpuUsage = uiFactory.CreateFmtValueLabel(0.0f, "{:.1f}");
      optionalCpuUsage->SetAlignmentX(UI::ItemAlignment::Far);
      layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
      layout->AddChild(cpuUsage, 0, 4);
      layout->AddChild(optionalCpuUsage, 1, 4);
    }

    std::shared_ptr<UI::Background> mainLayout = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Transparent, layout);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Far);
    return {mainLayout, imageIdle, lbl0, lbl1, lbl2, optionalCpuUsage};
  }
}
