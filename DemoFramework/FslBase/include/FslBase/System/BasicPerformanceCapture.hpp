#ifndef FSLBASE_SYSTEM_BASICPERFORMANCECAPTURE_HPP
#define FSLBASE_SYSTEM_BASICPERFORMANCECAPTURE_HPP
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

#include <FslBase/System/PerformanceCounter.hpp>
#include <array>
#include <cassert>

namespace Fsl
{
  struct BasicPerformanceCaptureRecord
  {
    uint64_t Begin{0};
    uint64_t End{0};

    constexpr BasicPerformanceCaptureRecord() noexcept = default;
    constexpr BasicPerformanceCaptureRecord(const uint64_t begin, const uint64_t end) noexcept
      : Begin(begin)
      , End(end)
    {
    }
  };

  // @brief Very basic class for timing some KPI's
  //        TKeyType must be unsigned and its range must be between 0 and 'TEntries - 1'
  template <typename TKeyType, std::size_t TEntries>
  class BasicPerformanceCapture
  {
  private:
    std::array<BasicPerformanceCaptureRecord, TEntries> m_results;

  public:
    using key_type = TKeyType;

    inline constexpr std::size_t Length() const noexcept
    {
      return TEntries;
    }

    inline void Begin(const key_type key)
    {
      assert(static_cast<uint32_t>(key) <= m_results.size());
      m_results[static_cast<uint32_t>(key)].Begin = PerformanceCounter::GetPerformanceCounter();
    }

    inline void End(const key_type key)
    {
      assert(static_cast<uint32_t>(key) <= m_results.size());
      m_results[static_cast<uint32_t>(key)].End = PerformanceCounter::GetPerformanceCounter();
    }

    inline void EndThenBegin(const key_type keyEnd, const key_type keyBegin)
    {
      assert(static_cast<uint32_t>(keyEnd) <= m_results.size());
      assert(static_cast<uint32_t>(keyBegin) <= m_results.size());
      const auto counter = PerformanceCounter::GetPerformanceCounter();
      m_results[static_cast<uint32_t>(keyEnd)].End = counter;
      m_results[static_cast<uint32_t>(keyBegin)].Begin = counter;
    }

    inline const BasicPerformanceCaptureRecord& Get(const key_type key) const
    {
      assert(static_cast<uint32_t>(key) <= m_results.size());
      return m_results[static_cast<uint32_t>(key)];
    }

    inline uint64_t GetCounter() const
    {
      return PerformanceCounter::GetPerformanceCounter();
    }

    inline uint64_t GetFrequency() const
    {
      return PerformanceCounter::GetPerformanceFrequency();
    }

    inline void Set(const key_type key, const BasicPerformanceCaptureRecord& entry)
    {
      assert(static_cast<uint32_t>(key) <= m_results.size());
      m_results[static_cast<uint32_t>(key)] = entry;
    }
  };
}

#endif
