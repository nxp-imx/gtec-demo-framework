/****************************************************************************************************************************************************
 * Copyright 2021, 2023 NXP
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

#include "InputCommandList.hpp"
#include <FslBase/Span/SpanUtil_Vector.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t DefaultCapacity = 2048;
      constexpr uint32_t GrowBy = 2048;
    }
  }

  InputCommandList::InputCommandList()
    : m_input(LocalConfig::DefaultCapacity)
  {
  }

  void InputCommandList::SetRecording(ReadOnlySpan<InputCommandRecord> span, const uint32_t frameCount)
  {
    // Ensure capacity
    if (span.size() >= m_input.size())
    {
      m_input.resize(span.size() + LocalConfig::GrowBy);
    }
    Clear();
    // Copy the content
    m_inputEntries = span.size();
    for (std::size_t i = 0; i < span.size(); ++i)
    {
      m_input[i] = span[i];
    }
    m_frameIndex = frameCount;
  }

  ReadOnlySpan<InputCommandRecord> InputCommandList::GetCommandSpan() const
  {
    return SpanUtil::AsReadOnlySpan(m_input, 0, m_inputEntries);
  }

  ReadOnlySpan<InputCommandRecord> InputCommandList::GetCommandSpanForFrame(const uint32_t frameIndex) const
  {
    // Locate span
    // Slow, but should be ok for this
    std::size_t index = 0;
    while (index < m_inputEntries && m_input[index].FrameIndex < frameIndex)
    {
      ++index;
    }
    if (index == m_inputEntries || m_input[index].FrameIndex > frameIndex)
    {
      return {};
    }
    assert(m_input[index].FrameIndex == frameIndex);
    const auto firstIndex = index;
    while (index < m_inputEntries && m_input[index].FrameIndex == frameIndex)
    {
      ++index;
    }
    assert(index >= firstIndex);
    assert(index <= m_inputEntries);
    return SpanUtil::AsReadOnlySpan(m_input, firstIndex, index - firstIndex);
  }


  uint32_t InputCommandList::FrameCount() const
  {
    return m_frameIndex;
  }

  void InputCommandList::Clear()
  {
    m_inputEntries = 0;
    m_frameIndex = 0;
  }

  void InputCommandList::NextFrameIndex()
  {
    ++m_frameIndex;
  }


  void InputCommandList::AddMouseButtonUp(const MillisecondTickCount32 timestamp, const CustomWindowId windowId, const PxRectangle windowRectPx,
                                          const PxPoint2 mousePosition, const bool isTouch)
  {
    Enqueue(InputCommandRecord(m_frameIndex, InputCommandId::MouseUp, timestamp, windowId, windowRectPx, mousePosition, isTouch));
  }

  void InputCommandList::AddMouseButtonDown(const MillisecondTickCount32 timestamp, const CustomWindowId windowId, const PxRectangle windowRectPx,
                                            const PxPoint2 mousePosition, const bool isTouch)
  {
    Enqueue(InputCommandRecord(m_frameIndex, InputCommandId::MouseDown, timestamp, windowId, windowRectPx, mousePosition, isTouch));
  }

  void InputCommandList::AddMouseMoveWhileDown(const MillisecondTickCount32 timestamp, const CustomWindowId windowId, const PxRectangle windowRectPx,
                                               const PxPoint2 mousePosition, const bool isTouch)
  {
    Enqueue(InputCommandRecord(m_frameIndex, InputCommandId::MouseDownMove, timestamp, windowId, windowRectPx, mousePosition, isTouch));
  }

  void InputCommandList::AddMouseMove(const MillisecondTickCount32 timestamp, const CustomWindowId windowId, const PxRectangle windowRectPx,
                                      const PxPoint2 mousePosition, const bool isTouch)
  {
    Enqueue(InputCommandRecord(m_frameIndex, InputCommandId::MouseMove, timestamp, windowId, windowRectPx, mousePosition, isTouch));
  }

  void InputCommandList::AddMouseMoveDone(const MillisecondTickCount32 timestamp)
  {
    Enqueue(InputCommandRecord(m_frameIndex, InputCommandId::MouseMoveClear, timestamp));
  }


  void InputCommandList::Enqueue(const InputCommandRecord record)
  {
    if (m_inputEntries >= m_input.size())
    {
      m_input.resize(m_inputEntries + LocalConfig::GrowBy);
    }
    m_input[m_inputEntries] = record;
    ++m_inputEntries;
  }
}
