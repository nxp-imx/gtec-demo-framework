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

#include "SceneDemoAppExtensionProxy.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxRectangle.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <Shared/UI/Benchmark/UIModule/ICustomWindowInfoModule.hpp>
#include <utility>
#include "Input/InputRecorder.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr TimeSpan RecordDeltaTime(TimeInfo::TicksPerSecond / 60);
    }
  }

  SceneDemoAppExtensionProxy::SceneDemoAppExtensionProxy(std::shared_ptr<ICustomWindowInfoModule> info, std::shared_ptr<IDemoAppExtension> extension)
    : m_proxy(std::move(extension))
    , m_info(std::move(info))
    , m_recorder(std::make_shared<InputRecorder>(m_info))
  {
  }


  SceneDemoAppExtensionProxy::~SceneDemoAppExtensionProxy() = default;


  void SceneDemoAppExtensionProxy::SetExtension(std::shared_ptr<IDemoAppExtension> extension)
  {
    m_proxy.SetProxy(std::move(extension));
  }


  void SceneDemoAppExtensionProxy::OnKeyEvent(const KeyEvent& event)
  {
    if (m_proxy.IsReady() && AllowInputForwarding())
    {
      m_proxy.OnKeyEvent(event);
    }
  }

  void SceneDemoAppExtensionProxy::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (m_proxy.IsReady() && AllowInputForwarding())
    {
      m_proxy.OnMouseButtonEvent(event);
      if (m_recorder)
      {
        m_recorder->OnMouseButtonEvent(event);
      }
    }
  }

  void SceneDemoAppExtensionProxy::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (m_proxy.IsReady() && AllowInputForwarding())
    {
      m_proxy.OnMouseMoveEvent(event);
      if (m_recorder)
      {
        m_recorder->OnMouseMoveEvent(event);
      }
    }
  }

  void SceneDemoAppExtensionProxy::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    if (m_proxy.IsReady() && AllowInputForwarding())
    {
      m_proxy.OnMouseWheelEvent(event);
      if (m_recorder)
      {
        m_recorder->OnMouseWheelEvent(event);
      }
    }
  }

  void SceneDemoAppExtensionProxy::OnRawMouseMoveEvent(const RawMouseMoveEvent& event)
  {
    if (m_proxy.IsReady() && AllowInputForwarding())
    {
      m_proxy.OnRawMouseMoveEvent(event);
    }
  }

  void SceneDemoAppExtensionProxy::OnTimeStateEvent(const TimeStateEvent& event)
  {
    m_proxy.OnTimeStateEvent(event);
  }

  void SceneDemoAppExtensionProxy::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_proxy.ConfigurationChanged(windowMetrics);
  }

  void SceneDemoAppExtensionProxy::Begin(const DemoAppExtensionCallOrder callOrder)
  {
    m_proxy.Begin(callOrder);
  }

  void SceneDemoAppExtensionProxy::PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    m_proxy.PreUpdate(callOrder, GetDemoTime(demoTime));
  }

  void SceneDemoAppExtensionProxy::FixedUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    if (callOrder == DemoAppExtensionCallOrder::PreApp)
    {
      m_fixedUpdateCalled = true;
    }
    m_proxy.FixedUpdate(callOrder, GetDemoTime(demoTime));
  }


  void SceneDemoAppExtensionProxy::Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    if (callOrder == DemoAppExtensionCallOrder::PreApp && m_proxy.IsReady() && m_playRecord.IsPlaying)
    {
      // Generate the fake input
      GenerateFakeInput(m_recorder->GetCommandSpanForFrame(m_playRecord.FrameIndex));
    }
    m_proxy.Update(callOrder, GetDemoTime(demoTime));
  }


  void SceneDemoAppExtensionProxy::PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    m_proxy.PostUpdate(callOrder, GetDemoTime(demoTime));

    if (callOrder == DemoAppExtensionCallOrder::PostApp)
    {
      if (m_fixedUpdateCalled)
      {
        m_fixedUpdateCalled = false;

        if (UseCustomTime())
        {
          constexpr auto deltaTime = LocalConfig::RecordDeltaTime;
          const auto totalTime = m_controlledDemoTime.TotalTime + deltaTime;
          m_controlledDemoTime = DemoTime(totalTime, deltaTime);
        }


        // We use the fixed update check to ensure that our fake recording speed doesn't go out of hand on very fast systems
        // The limiter might mean that videos recorded >60fps displays 'group' input updates that occurred over multiple frames into one.
        if (m_recorder->IsRecording())
        {
          m_recorder->RecordNextFrameIndex();
        }
      }
      if (m_playRecord.IsPlaying)
      {
        ++m_playRecord.FrameIndex;
        if (m_playRecord.FrameIndex >= m_recorder->FrameCount())
        {
          PlayEnd();
        }
      }
    }
  }

  void SceneDemoAppExtensionProxy::Resolve(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    m_proxy.Resolve(callOrder, GetDemoTime(demoTime));
  }


  void SceneDemoAppExtensionProxy::OnDrawSkipped(const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo)
  {
    m_proxy.OnDrawSkipped(callOrder, frameInfo);
  }

  void SceneDemoAppExtensionProxy::End(const DemoAppExtensionCallOrder callOrder)
  {
    m_proxy.End(callOrder);
  }

  ReadOnlySpan<InputCommandRecord> SceneDemoAppExtensionProxy::GetRecordingAsSpan() const
  {
    return m_recorder->GetCommandSpan();
  }


  void SceneDemoAppExtensionProxy::SetRecording(ReadOnlySpan<InputCommandRecord> span, const uint32_t frameCount)
  {
    return m_recorder->SetRecording(span, frameCount);
  }


  uint32_t SceneDemoAppExtensionProxy::FrameCount() const
  {
    return m_recorder->FrameCount();
  }

  bool SceneDemoAppExtensionProxy::IsRecording() const
  {
    return m_recorder->IsRecording();
  }

  void SceneDemoAppExtensionProxy::RecordBegin()
  {
    StopAll();
    m_controlledDemoTime = {};
    return m_recorder->RecordBegin();
  }

  void SceneDemoAppExtensionProxy::RecordEnd()
  {
    if (IsRecording())
    {
      m_recorder->RecordEnd();
    }
  }


  bool SceneDemoAppExtensionProxy::IsPlaying() const
  {
    return m_playRecord.IsPlaying;
  }


  uint32_t SceneDemoAppExtensionProxy::PlayFrameIndex() const
  {
    return m_playRecord.FrameIndex;
  }

  void SceneDemoAppExtensionProxy::PlayBegin()
  {
    StopAll();
    m_controlledDemoTime = {};
    m_playRecord = {};
    m_playRecord.IsPlaying = true;
  }

  void SceneDemoAppExtensionProxy::PlayEnd()
  {
    if (IsPlaying())
    {
      m_playRecord = {};
    }
  }


  std::optional<DemoTime> SceneDemoAppExtensionProxy::TryGetDemoTime() const
  {
    if (UseCustomTime())
    {
      return {m_controlledDemoTime};
    }
    return {};
  }


  void SceneDemoAppExtensionProxy::StopAll()
  {
    PlayEnd();
    RecordEnd();
  }

  bool SceneDemoAppExtensionProxy::AllowInputForwarding() const
  {
    return m_recorder->IsRecording();
  }

  bool SceneDemoAppExtensionProxy::UseCustomTime() const
  {
    return m_recorder->IsRecording() || m_playRecord.IsPlaying;
  }


  DemoTime SceneDemoAppExtensionProxy::GetDemoTime(const DemoTime& demoTime) const
  {
    return UseCustomTime() ? m_controlledDemoTime : demoTime;
  }

  void SceneDemoAppExtensionProxy::GenerateFakeInput(ReadOnlySpan<InputCommandRecord> span)
  {
    if (span.empty())
    {
      return;
    }

    for (std::size_t i = 0; i < span.size(); ++i)
    {
      switch (span[i].CommandId)
      {
      case InputCommandId::MouseDown:
        GenerateFakeMouseDown(span[i]);
        break;
      case InputCommandId::MouseUp:
        GenerateFakeMouseUp(span[i]);
        break;
      case InputCommandId::MouseDownMove:
        GenerateFakeMouseDownMove(span[i]);
        break;
      case InputCommandId::MouseMove:
        GenerateFakeMouseMove(span[i]);
        break;
      case InputCommandId::MouseMoveClear:
        GenerateFakeMouseClear(span[i]);
        break;
      case InputCommandId::Invalid:
        FSLLOG3_ERROR("InputCommandId::Invaid should never be recorded");
        break;
      default:
        FSLLOG3_ERROR("Unsupported command id found: '{}'", static_cast<uint32_t>(span[i].CommandId));
        break;
      }
    }
  }

  void SceneDemoAppExtensionProxy::GenerateFakeMouseDown(const InputCommandRecord& entry)
  {
    const std::optional<PxPoint2> positionPx = TryRewriteScreenCoordinate(entry);
    if (positionPx.has_value())
    {
      assert(m_proxy.IsReady());
      m_proxy.OnMouseButtonEvent(MouseButtonEvent(VirtualMouseButton::Left, true, positionPx.value()));
    }
  }

  void SceneDemoAppExtensionProxy::GenerateFakeMouseUp(const InputCommandRecord& entry)
  {
    const std::optional<PxPoint2> positionPx = TryRewriteScreenCoordinate(entry);
    if (positionPx.has_value())
    {
      assert(m_proxy.IsReady());
      m_proxy.OnMouseButtonEvent(MouseButtonEvent(VirtualMouseButton::Left, false, positionPx.value()));
    }
  }

  void SceneDemoAppExtensionProxy::GenerateFakeMouseDownMove(const InputCommandRecord& entry)
  {
    const std::optional<PxPoint2> positionPx = TryRewriteScreenCoordinate(entry);
    if (positionPx.has_value())
    {
      assert(m_proxy.IsReady());
      m_proxy.OnMouseMoveEvent(MouseMoveEvent(positionPx.value(), VirtualMouseButtonFlags(VirtualMouseButton::Left)));
    }
  }

  void SceneDemoAppExtensionProxy::GenerateFakeMouseMove(const InputCommandRecord& entry)
  {
    const std::optional<PxPoint2> positionPx = TryRewriteScreenCoordinate(entry);
    if (positionPx.has_value())
    {
      assert(m_proxy.IsReady());
      m_proxy.OnMouseMoveEvent(MouseMoveEvent(positionPx.value(), VirtualMouseButtonFlags()));
    }
  }

  void SceneDemoAppExtensionProxy::GenerateFakeMouseClear(const InputCommandRecord& entry)
  {
    FSL_PARAM_NOT_USED(entry);
    const PxPoint2 positionPx(-25000, -25000);
    assert(m_proxy.IsReady());
    m_proxy.OnMouseMoveEvent(MouseMoveEvent(positionPx, VirtualMouseButtonFlags()));
  }


  std::optional<PxPoint2> SceneDemoAppExtensionProxy::TryRewriteScreenCoordinate(const InputCommandRecord& entry) const
  {
    std::optional<PxRectangle> currentRectanglePx = m_info->TryGetWindowRectanglePx(entry.WindowId);
    if (currentRectanglePx.has_value())
    {
      const PxRectangle windowRectanglePx = currentRectanglePx.value();

      PxPoint2 newPositionPx = entry.MousePositionPx - entry.WindowRectPx.TopLeft();

      if (entry.WindowRectPx.Width() > 0 && windowRectanglePx.Width() != entry.WindowRectPx.Width())
      {
        // The window dimensions were changed, so lets rescale the coordinate
        const float scaleFactor = static_cast<float>(windowRectanglePx.Width()) / static_cast<float>(entry.WindowRectPx.Width());
        newPositionPx.X = static_cast<int32_t>(std::round(static_cast<float>(newPositionPx.X) * scaleFactor));
      }
      if (entry.WindowRectPx.Height() > 0 && windowRectanglePx.Height() != entry.WindowRectPx.Height())
      {
        const float scaleFactor = static_cast<float>(windowRectanglePx.Height()) / static_cast<float>(entry.WindowRectPx.Height());
        newPositionPx.Y = static_cast<int32_t>(std::round(static_cast<float>(newPositionPx.Y) * scaleFactor));
      }

      newPositionPx += windowRectanglePx.TopLeft();

      return {newPositionPx};
    }
    FSLLOG3_ERROR("Failed to locate the window for id {}, this input recording does not match the current UI setup", entry.WindowId.Value);
    return {};
  }

}
