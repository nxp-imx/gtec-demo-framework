#ifndef FSLSIMPLEUI_APP_DEMOPERFORMANCECAPTURE_HPP
#define FSLSIMPLEUI_APP_DEMOPERFORMANCECAPTURE_HPP
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

#include <FslBase/System/BasicPerformanceCapture.hpp>
#include <FslSimpleUI/Render/Base/RenderPerformanceCapture.hpp>
#include <array>
#include <cassert>
#include <cmath>

namespace Fsl
{
  enum class DemoPerformanceCaptureId
  {
    Roundtrip = 0,
    UIProcessEvents = 1,
    UIUpdate = 2,
    UIDraw = 3,
    UIDrawPreprocessDrawCommands = 4,
    UIDrawGenerateMeshes = 5,
    UIDrawUpdateBuffers = 6,
    UIDrawScheduleDraw = 7
  };

  /// <summary>
  /// Very basic class for timing some KPI's
  /// </summary>
  class DemoPerformanceCapture
  {
    BasicPerformanceCapture<DemoPerformanceCaptureId, 8> m_entries;
    uint64_t m_lastClearCounter{0};
    bool m_isFirst{true};
    double m_frequency{0};

  public:
    static constexpr double CalcFrequency(const uint64_t frequency) noexcept
    {
      return frequency != 0u ? 1000000.0 / frequency : 0u;
    }

    DemoPerformanceCapture()
      : m_frequency(CalcFrequency(m_entries.GetFrequency()))
    {
    }

    void CalcRoundTripTime()
    {
      uint64_t currentCounter = m_entries.GetCounter();
      if (m_isFirst)
      {
        m_isFirst = false;
        m_lastClearCounter = currentCounter;
      }

      m_entries.Set(DemoPerformanceCaptureId::Roundtrip, BasicPerformanceCaptureRecord(m_lastClearCounter, currentCounter));
      m_lastClearCounter = currentCounter;
    }

    void Clear()
    {
      m_entries = {};
    }

    void Clear(const DemoPerformanceCaptureId id)
    {
      m_entries.Set(id, BasicPerformanceCaptureRecord());
    }

    void BeginProfile(const DemoPerformanceCaptureId id)
    {
      m_entries.Begin(id);
    }

    void EndProfile(const DemoPerformanceCaptureId id)
    {
      m_entries.End(id);
    }

    void SetRenderPerformanceCapture(const UI::RenderPerformanceCapture& capture)
    {
      m_entries.Set(DemoPerformanceCaptureId::UIDrawPreprocessDrawCommands, capture.Get(UI::RenderPerformanceCaptureId::PreprocessDrawCommands));
      m_entries.Set(DemoPerformanceCaptureId::UIDrawGenerateMeshes, capture.Get(UI::RenderPerformanceCaptureId::GenerateMeshes));
      m_entries.Set(DemoPerformanceCaptureId::UIDrawUpdateBuffers, capture.Get(UI::RenderPerformanceCaptureId::UpdateBuffers));
      m_entries.Set(DemoPerformanceCaptureId::UIDrawScheduleDraw, capture.Get(UI::RenderPerformanceCaptureId::ScheduleDraw));
    }

    int64_t GetResult(const DemoPerformanceCaptureId entry) const
    {
      const BasicPerformanceCaptureRecord& rRecord = m_entries.Get(entry);
      return static_cast<int64_t>(std::round(double(rRecord.End - rRecord.Begin) * m_frequency));
    }
  };
}

#endif
