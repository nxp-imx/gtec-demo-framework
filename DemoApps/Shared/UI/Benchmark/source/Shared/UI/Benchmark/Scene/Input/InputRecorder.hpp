#ifndef SHARED_UI_BENCHMARK_SCENE_INPUT_INPUTRECORDER_HPP
#define SHARED_UI_BENCHMARK_SCENE_INPUT_INPUTRECORDER_HPP
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

#include <Shared/UI/Benchmark/Persistence/Window/CustomWindowId.hpp>
#include <memory>
#include "InputCommandList.hpp"

namespace Fsl
{
  class ICustomWindowInfoModule;
  class MouseButtonEvent;
  class MouseMoveEvent;
  class MouseWheelEvent;

  class InputRecorder
  {
    struct Record
    {
      bool IsDown{false};
      CustomWindowId WindowId;

      Record() = default;
      explicit Record(const CustomWindowId windowId)
        : IsDown(true)
        , WindowId(windowId)
      {
      }
    };

    struct RecordingRecord
    {
      bool IsRecording{false};
      Record MouseButton;
      Record MouseMove;
    };

    std::shared_ptr<ICustomWindowInfoModule> m_info;
    RecordingRecord m_recording;
    InputCommandList m_commandList;

  public:
    explicit InputRecorder(std::shared_ptr<ICustomWindowInfoModule> info);

    bool IsRecording() const
    {
      return m_recording.IsRecording;
    }

    void SetRecording(ReadOnlySpan<InputCommandRecord> span, const uint32_t frameCount);
    ReadOnlySpan<InputCommandRecord> GetCommandSpan() const;
    ReadOnlySpan<InputCommandRecord> GetCommandSpanForFrame(const uint32_t frameIndex) const;


    uint32_t FrameCount() const;

    void RecordBegin();
    void RecordEnd();
    void RecordNextFrameIndex();

    void OnMouseButtonEvent(const MouseButtonEvent& event);
    void OnMouseMoveEvent(const MouseMoveEvent& event);
    void OnMouseWheelEvent(const MouseWheelEvent& event);
  };
}

#endif
