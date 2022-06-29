#if defined(_WIN32) && defined(FSL_PLATFORM_WINDOWS)
/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/CpuStats/Impl/Adapter/Win32/CpuStatsAdapterWin32.hpp>
#include <fmt/format.h>
#include <pdh.h>
#include <psapi.h>
#include <algorithm>
#include <cassert>
#include <exception>
#include "PerformanceCounterQueryWin32.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr TimeSpan MinIntervalApplicationCpuUsage(TimeSpanUtil::FromMicroseconds(250 * 1000));
    }


    uint32_t DoGetCpuCount()
    {
      SYSTEM_INFO systemInfo{};
      GetSystemInfo(&systemInfo);
      return UncheckedNumericCast<uint32_t>(systemInfo.dwNumberOfProcessors);
    }

    uint64_t ToUInt64(const FILETIME& time)
    {
      ULARGE_INTEGER theTime;
      theTime.LowPart = time.dwLowDateTime;
      theTime.HighPart = time.dwHighDateTime;
      return theTime.QuadPart;
    }
  }

  // Counters can be listed with: "Get-Counter -ListSet *"
  // The performance counters can be lost but can sometimes be manually rebuild.
  // Please read more at https://docs.microsoft.com/en-us/troubleshoot/windows-server/performance/rebuild-performance-counter-library-values
  // If you read that and understand what it does its possible that running "lodctr /R" will fix any issues.
  CpuStatsAdapterWin32::CpuStatsAdapterWin32()
  {
    m_cpuCount = DoGetCpuCount();

    try
    {
      m_counters = std::make_unique<PerformanceCounterQueryWin32>(m_cpuCount, m_timer.GetTimestamp());
    }
    catch (const std::exception& ex)
    {
      m_counters.reset();
      FSLLOG3_WARNING("Failed to create performance counters, so that functionality is missing. Reported failure: {}", ex.what());
    }

    // Get the initial value for 'app' times
    if (!TryGetSystemTimes(m_appSystemLast))
    {
      throw std::runtime_error("TryGetSystemTimes failed");
    }
    if (!TryGetProcessTimes(m_appProcessLast))
    {
      throw std::runtime_error("TryGetProcessTimes failed");
    }

    m_lastTryGetApplicationCpuUsageTime = m_timer.GetTimestamp() - LocalConfig::MinIntervalApplicationCpuUsage;
  }


  CpuStatsAdapterWin32::~CpuStatsAdapterWin32() = default;


  uint32_t CpuStatsAdapterWin32::GetCpuCount() const
  {
    return m_cpuCount;
  }


  bool CpuStatsAdapterWin32::TryGetCpuUsage(float& rUsagePercentage, const uint32_t cpuIndex) const
  {
    if (m_counters)
    {
      return m_counters->TryGetCpuUsage(rUsagePercentage, cpuIndex, m_timer.GetTimestamp());
    }
    rUsagePercentage = 0.0f;
    return false;
  }


  // Inspired by: http://www.philosophicalgeek.com/2009/01/03/determine-cpu-usage-of-current-process-c-and-c/
  bool CpuStatsAdapterWin32::TryGetApplicationCpuUsage(float& rUsagePercentage) const
  {
    rUsagePercentage = 0.0f;

    auto currentTime = m_timer.GetTimestamp();
    auto deltaTime = currentTime - m_lastTryGetApplicationCpuUsageTime;
    if (deltaTime < LocalConfig::MinIntervalApplicationCpuUsage)
    {
      rUsagePercentage = m_appCpuUsagePercentage;
      return true;
    }
    m_lastTryGetApplicationCpuUsageTime = currentTime;

    ProcessTimes systemTimes;
    if (!TryGetSystemTimes(systemTimes))
    {
      FSLLOG3_DEBUG_WARNING("TryGetSystemTimes failed");
      return false;
    }

    ProcessTimes processTimes;
    if (!TryGetProcessTimes(processTimes))
    {
      FSLLOG3_DEBUG_WARNING("TryGetProcessTimes failed");
      return false;
    }

    auto systemTime = (systemTimes.KernelTime - m_appSystemLast.KernelTime) + (systemTimes.UserTime - m_appSystemLast.UserTime);
    auto appTime = (processTimes.KernelTime - m_appProcessLast.KernelTime) + (processTimes.UserTime - m_appProcessLast.UserTime);

    if (systemTime > 0)
    {
      m_appSystemLast = systemTimes;
      m_appProcessLast = processTimes;
      m_appCpuUsagePercentage = static_cast<float>((100.0 * static_cast<double>(appTime)) / static_cast<double>(systemTime));
    }
    else
    {
      FSLLOG3_DEBUG_VERBOSE2("not enough time passed");
    }

    rUsagePercentage = m_appCpuUsagePercentage;
    return true;
  }


  bool CpuStatsAdapterWin32::TryGetApplicationRamUsage(uint64_t& rRamUsage) const
  {
    HANDLE hProcess = GetCurrentProcess();

    PROCESS_MEMORY_COUNTERS_EX memoryCounters;
    if (!GetProcessMemoryInfo(hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memoryCounters), sizeof(memoryCounters)))
    {
      return false;
    }

    rRamUsage = memoryCounters.WorkingSetSize;
    return true;
  }


  bool CpuStatsAdapterWin32::TryQueryCountersNow() const
  {
    if (m_counters)
    {
      return m_counters->TryQueryCountersNow();
    }
    return false;
  }


  bool CpuStatsAdapterWin32::TryGetProcessTimes(ProcessTimes& rTimes) const    // NOLINT(readability-convert-member-functions-to-static)
  {
    FILETIME creationTime{};
    FILETIME exitTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};
    HANDLE hProcess = GetCurrentProcess();
    if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime) == 0)
    {
      rTimes = {};
      return false;
    }

    const auto appKernelTime = ToUInt64(kernelTime);
    const auto appUserTime = ToUInt64(userTime);
    rTimes = ProcessTimes(appKernelTime, appUserTime);
    return true;
  }


  bool CpuStatsAdapterWin32::TryGetSystemTimes(ProcessTimes& rTimes) const    // NOLINT(readability-convert-member-functions-to-static)
  {
    FILETIME idleTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime) == 0)
    {
      rTimes = {};
      return false;
    }

    const auto appKernelTime = ToUInt64(kernelTime);
    const auto appUserTime = ToUInt64(userTime);
    rTimes = ProcessTimes(appKernelTime, appUserTime);
    return true;
  }
}

#endif
