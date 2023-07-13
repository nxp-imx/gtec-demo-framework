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

#include "InputRecorder.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxRectangle.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseWheelEvent.hpp>
#include <Shared/UI/Benchmark/UIModule/ICustomWindowInfoModule.hpp>
#include <utility>

namespace Fsl
{
  InputRecorder::InputRecorder(std::shared_ptr<ICustomWindowInfoModule> info)
    : m_info(std::move(info))
  {
  }


  void InputRecorder::RecordBegin()
  {
    m_commandList.Clear();
    m_recording = {};
    m_recording.IsRecording = true;
  }


  void InputRecorder::SetRecording(ReadOnlySpan<InputCommandRecord> span, const uint32_t frameCount)
  {
    if (m_recording.IsRecording)
    {
      throw UsageErrorException("Can not SetRecording during record");
    }

    m_commandList.SetRecording(span, frameCount);
  }


  ReadOnlySpan<InputCommandRecord> InputRecorder::GetCommandSpan() const
  {
    return m_commandList.GetCommandSpan();
  }


  ReadOnlySpan<InputCommandRecord> InputRecorder::GetCommandSpanForFrame(const uint32_t frameIndex) const
  {
    return m_commandList.GetCommandSpanForFrame(frameIndex);
  }


  uint32_t InputRecorder::FrameCount() const
  {
    return m_commandList.FrameCount();
  }

  void InputRecorder::RecordNextFrameIndex()
  {
    if (m_recording.IsRecording)
    {
      m_commandList.NextFrameIndex();
    }
    else
    {
      FSLLOG3_WARNING("No recording, request ignored");
    }
  }

  void InputRecorder::RecordEnd()
  {
    m_recording = {};
  }


  void InputRecorder::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (m_recording.IsRecording)
    {
      if (m_recording.MouseButton.IsDown)
      {
        if (event.IsPressed())
        {
          FSLLOG3_WARNING("Unexpected event state, force cancelling old button down");
        }
        assert(m_recording.MouseButton.WindowId.IsValid());

        const PxPoint2 eventPositionPx = event.GetPosition();
        const auto windowRectanglePx = m_info->GetWindowRectanglePx(m_recording.MouseButton.WindowId);

        // FSLLOG3_INFO("MouseButtonEvent up at window #{}, RectPx: {}, EventPosPx: {}", m_recording.MouseButton.WindowId.Value, windowRectanglePx,
        //             eventPositionPx);
        m_commandList.AddMouseButtonUp(m_recording.MouseButton.WindowId, windowRectanglePx, eventPositionPx, event.IsTouch());
        m_recording.MouseButton = {};
      }

      if (event.IsHandled() && event.IsPressed())
      {
        const PxPoint2 eventPositionPx = event.GetPosition();
        const auto customWindowId = m_info->TryGetClickInputWindow(eventPositionPx);
        if (customWindowId.IsValid())
        {
          m_recording.MouseButton = Record(customWindowId);
          // We found a window that accepted the event, so lookup its current location
          const auto windowRectanglePx = m_info->GetWindowRectanglePx(customWindowId);
          // FSLLOG3_INFO("MouseButtonEvent down at window #{}, RectPx: {}, EventPosPx: {}", customWindowId.Value, windowRectanglePx,
          // eventPositionPx);
          m_commandList.AddMouseButtonDown(m_recording.MouseButton.WindowId, windowRectanglePx, eventPositionPx, event.IsTouch());
        }
      }
    }
  }


  void InputRecorder::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (m_recording.IsRecording)
    {
      const PxPoint2 eventPositionPx = event.GetPosition();
      bool clearMove = true;
      if (m_recording.MouseButton.IsDown)
      {
        assert(m_recording.MouseButton.WindowId.IsValid());
        const auto windowRectanglePx = m_info->GetWindowRectanglePx(m_recording.MouseButton.WindowId);
        // FSLLOG3_INFO("MouseButtonMove at window #{}, RectPx: {}, EventPosPx: {}", m_recording.MouseButton.WindowId.Value, windowRectanglePx,
        //             eventPositionPx);
        m_commandList.AddMouseMoveWhileDown(m_recording.MouseButton.WindowId, windowRectanglePx, eventPositionPx, event.IsTouch());
        clearMove = false;
      }
      else if (event.IsHandled())
      {
        auto customWindowId = m_info->TryGetMouseOverWindow(eventPositionPx);
        if (customWindowId.IsValid())
        {
          const auto windowRectanglePx = m_info->GetWindowRectanglePx(customWindowId);
          // FSLLOG3_INFO("MouseMove at window #{}, RectPx: {}, EventPosPx: {}", customWindowId.Value, windowRectanglePx, eventPositionPx);
          m_recording.MouseMove = Record(customWindowId);
          m_commandList.AddMouseMove(customWindowId, windowRectanglePx, eventPositionPx, event.IsTouch());
          clearMove = false;
        }
      }
      if (m_recording.MouseMove.IsDown && clearMove)
      {
        // FSLLOG3_INFO("MouseMove cleared");
        m_commandList.AddMouseMoveDone();
        m_recording.MouseMove = {};
      }
    }
  }

  void InputRecorder::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    FSL_PARAM_NOT_USED(event);
  }
}
