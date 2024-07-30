#if defined(_WIN32) && defined(FSL_PLATFORM_WINDOWS)
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include "PerformanceCounterQueryWin32.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <fmt/format.h>
#include <pdhmsg.h>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr TimeSpan MinIntervalCoreCpuUsage(TimeSpanUtil::FromSeconds(1));
    }

    std::string ToErrorCodeString(const PDH_STATUS status)
    {
      const char* pszError = "Unknown";
      switch (status)
      {
      case static_cast<PDH_STATUS>(ERROR_SUCCESS):
        pszError = "No errors";
        break;
      case static_cast<PDH_STATUS>(PDH_CSTATUS_BAD_COUNTERNAME):
        pszError = "PDH_CSTATUS_BAD_COUNTERNAME";
        break;
      case static_cast<PDH_STATUS>(PDH_CSTATUS_NO_COUNTER):
        pszError = "PDH_CSTATUS_NO_COUNTER";
        break;
      case static_cast<PDH_STATUS>(PDH_CSTATUS_NO_COUNTERNAME):
        pszError = "PDH_CSTATUS_NO_COUNTERNAME";
        break;
      case static_cast<PDH_STATUS>(PDH_CSTATUS_NO_MACHINE):
        pszError = "PDH_CSTATUS_NO_MACHINE";
        break;
      case static_cast<PDH_STATUS>(PDH_CSTATUS_NO_OBJECT):
        pszError = "PDH_CSTATUS_NO_OBJECT";
        break;
      case static_cast<PDH_STATUS>(PDH_FUNCTION_NOT_FOUND):
        pszError = "PDH_FUNCTION_NOT_FOUND";
        break;
      case static_cast<PDH_STATUS>(PDH_INVALID_ARGUMENT):
        pszError = "PDH_INVALID_ARGUMENT";
        break;
      case static_cast<PDH_STATUS>(PDH_INVALID_HANDLE):
        pszError = "PDH_INVALID_HANDLE";
        break;
      case static_cast<PDH_STATUS>(PDH_MEMORY_ALLOCATION_FAILURE):
        pszError = "PDH_MEMORY_ALLOCATION_FAILURE";
        break;
      default:
        break;
      }
      return fmt::format("{} ({})", pszError, status);
    }
  }

  PerformanceCounterQueryWin32::PerformanceCounterQueryWin32(const uint32_t cpuCount, const TickCount currentTime)
    : m_cpuCount(std::min(cpuCount, UncheckedNumericCast<uint32_t>(m_cpuStats.size())))
  {
    PDH_STATUS result = PdhOpenQuery(nullptr, 0, &m_hQuery);
    if (result != ERROR_SUCCESS)
    {
      throw std::runtime_error(fmt::format("PdhOpenQuery with: {}", ToErrorCodeString(result)));
    }

    try
    {
      fmt::memory_buffer buf;
      for (uint32_t cpuIndex = 0; cpuIndex < m_cpuCount; ++cpuIndex)
      {
        buf.clear();
        fmt::format_to(std::back_inserter(buf), "\\Processor({})\\% Processor Time", cpuIndex);

        buf.push_back(0);

        result = PdhAddEnglishCounterA(m_hQuery, buf.data(), 0, &m_cpuStats[cpuIndex].Counter);
        if (result != ERROR_SUCCESS)
        {
          throw std::runtime_error(fmt::format("PdhAddEnglishCounter failed with: {} for counter '{}'", ToErrorCodeString(result), buf.data()));
        }

        m_cpuStats[cpuIndex].Initialized = true;
        m_cpuStats[cpuIndex].Value = 0.0f;
      }

      // Do a query right away just to verify that it works
      result = PdhCollectQueryData(m_hQuery);
      if (result != ERROR_SUCCESS)
      {
        throw std::runtime_error(fmt::format("PdhCollectQueryData failed with: {}", ToErrorCodeString(result)));
      }

      m_lastTryGetCpuUsage = currentTime - LocalConfig::MinIntervalCoreCpuUsage;
    }
    catch (const std::exception&)
    {
      DisposeNow();
      throw;
    }
  }

  PerformanceCounterQueryWin32::~PerformanceCounterQueryWin32()
  {
    DisposeNow();
  }

  bool PerformanceCounterQueryWin32::TryGetCpuUsage(CpuUsageRecord& rUsageRecord, const uint32_t cpuIndex, const TickCount currentTime) const
  {
    // We rely on the service to do the actual validation
    assert(m_cpuCount <= m_cpuStats.size());
    if (cpuIndex >= m_cpuCount || !m_cpuStats[cpuIndex].Initialized)
    {
      rUsageRecord = {};
      return false;
    }

    // Ensure that we only cache the counters after the desired time has passed
    auto deltaTime = currentTime - m_lastTryGetCpuUsage;
    if (deltaTime >= LocalConfig::MinIntervalCoreCpuUsage)
    {
      m_lastTryGetCpuUsage = currentTime;
      if (!TryQueryCountersNow())
      {
        return false;
      }
    }

    rUsageRecord = {m_lastTryGetCpuUsage, m_cpuStats[cpuIndex].Value};
    return true;
  }

  bool PerformanceCounterQueryWin32::TryQueryCountersNow() const
  {
    auto result = PdhCollectQueryData(m_hQuery);
    if (result != ERROR_SUCCESS)
    {
      FSLLOG3_DEBUG_WARNING("PdhCollectQueryData failed with: {}", result);
      return false;
    }

    for (uint32_t cpuIndex = 0; cpuIndex < m_cpuCount; ++cpuIndex)
    {
      PDH_FMT_COUNTERVALUE counterValue;
      result = PdhGetFormattedCounterValue(m_cpuStats[cpuIndex].Counter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, nullptr, &counterValue);
      if (result == ERROR_SUCCESS && (counterValue.CStatus == PDH_CSTATUS_NEW_DATA || counterValue.CStatus == PDH_CSTATUS_VALID_DATA))
      {
        m_cpuStats[cpuIndex].Value = static_cast<float>(counterValue.doubleValue);
      }
    }
    return true;
  }


  void PerformanceCounterQueryWin32::DisposeNow() noexcept
  {
    for (uint32_t cpuIndex = 0; cpuIndex < m_cpuCount; ++cpuIndex)
    {
      if (m_cpuStats[cpuIndex].Initialized)
      {
        m_cpuStats[cpuIndex].Initialized = false;
        auto result = PdhRemoveCounter(m_cpuStats[cpuIndex].Counter);
        FSLLOG3_ERROR_IF(result != ERROR_SUCCESS, "PdhRemoveCounter failed with: {:#x}", result);
      }
    }

    PdhCloseQuery(&m_hQuery);
  }
}
#endif
