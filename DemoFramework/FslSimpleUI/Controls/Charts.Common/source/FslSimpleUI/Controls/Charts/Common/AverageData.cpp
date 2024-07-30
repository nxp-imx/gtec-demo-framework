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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslSimpleUI/Controls/Charts/Common/AverageData.hpp>
#include <algorithm>

// #define LOCAL_SANITY_CHECK
#ifdef LOCAL_SANITY_CHECK
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SANITY_CHECK() SanityCheck()
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SANITY_CHECK() \
  {                    \
  }
#endif

namespace Fsl::UI
{
  AverageData::AverageData(const uint32_t entries)
    : m_buffer(entries)
  {
  }


  void AverageData::Clear()
  {
    m_buffer.clear();
    m_currentSum = {};
    SANITY_CHECK();
  }


  void AverageData::Append(const ChartDataEntry& value)
  {
    if (m_buffer.size() == m_buffer.capacity() && !m_buffer.empty())
    {
      Sub(m_currentSum, m_buffer.front());
      m_buffer.pop_front();
    }

    m_buffer.push_back(value);
    Add(m_currentSum, value);
    SANITY_CHECK();
  }

  void AverageData::SetWindowMaxSize(const uint32_t desiredNewCapacity)
  {
    const uint32_t newCapacity = std::max(desiredNewCapacity, static_cast<uint32_t>(1));
    if (newCapacity != m_buffer.capacity())
    {
      m_buffer.resize_pop_front(newCapacity);
      m_currentSum = CalcSum();
      SANITY_CHECK();
    }
  }


  ReadOnlySpan<double> AverageData::CalcAverage()
  {
    if (!m_buffer.empty())
    {
      const std::size_t entries = m_buffer.size();
      for (std::size_t i = 0; i < m_currentSum.Values.size(); ++i)
      {
        m_averageScratchpad[i] = static_cast<double>(m_currentSum.Values[i]) / static_cast<double>(entries);
      }
    }
    else
    {
      for (std::size_t i = 0; i < m_currentSum.Values.size(); ++i)
      {
        m_averageScratchpad[i] = 0.0f;
      }
    }
    return SpanUtil::AsReadOnlySpan(m_averageScratchpad);
  }


  void AverageData::Sub(ChartDataEntry& rDst, const ChartDataEntry& value)
  {
    for (std::size_t i = 0; i < rDst.Values.size(); ++i)
    {
      assert(rDst.Values[i] >= value.Values[i]);
      rDst.Values[i] -= value.Values[i];
    }
  }

  void AverageData::Add(ChartDataEntry& rDst, const ChartDataEntry& value)
  {
    for (std::size_t i = 0; i < rDst.Values.size(); ++i)
    {
      rDst.Values[i] += value.Values[i];
    }
  }

  ChartDataEntry AverageData::CalcSum() const
  {
    ChartDataEntry currentSum;
    for (std::size_t j = 0; j < m_buffer.size(); ++j)
    {
      const auto& entry = m_buffer[j];
      for (std::size_t i = 0; i < entry.Values.size(); ++i)
      {
        currentSum.Values[i] += entry.Values[i];
      }
    }
    return currentSum;
  }

  void AverageData::SanityCheck()
  {
    auto currentSum = CalcSum();
    for (std::size_t i = 0; i < currentSum.Values.size(); ++i)
    {
      if (currentSum.Values[i] != m_currentSum.Values[i])
      {
        throw InternalErrorException("current sum is incorrect");
      }
    }
  }

}
