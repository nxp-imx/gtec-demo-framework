#if defined(_WIN32) && defined(FSL_PLATFORM_WINDOWS)
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslDemoService/CpuStats/Impl/Adapter/Win32/CpuStatsAdapterWin32.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <cassert>
#include <exception>
#include <pdhmsg.h>
#include <psapi.h>

namespace Fsl
{
  namespace
  {
    // microseconds
    const uint64_t MIN_INTERVAL_CORE_CPU_USAGE = 250 * 1000;
    // microseconds
    const uint64_t MIN_INTERVAL_APPLICATION_CPU_USAGE = 250 * 1000;

    uint32_t DoGetCpuCount()
    {
      SYSTEM_INFO systemInfo{};
      GetSystemInfo(&systemInfo);
      return static_cast<uint32_t>(systemInfo.dwNumberOfProcessors);
    }

    uint64_t ToUInt64(const FILETIME& time)
    {
      ULARGE_INTEGER theTime;
      theTime.LowPart = time.dwLowDateTime;
      theTime.HighPart = time.dwHighDateTime;
      return theTime.QuadPart;
    }
  }


  CpuStatsAdapterWin32::CpuStatsAdapterWin32()
  {
    auto result = PdhOpenQuery(nullptr, 0, &m_hQuery);
    if (result != ERROR_SUCCESS)
    {
      throw std::runtime_error(fmt::format("PdhOpenQuery with: {}", result));
    }

    try
    {
      m_cpuCount = std::min(DoGetCpuCount(), UncheckedNumericCast<uint32_t>(m_cpuStats.size()));

      fmt::memory_buffer buf;
      for (uint32_t cpuIndex = 0; cpuIndex < m_cpuCount; ++cpuIndex)
      {
        buf.clear();
        fmt::format_to(buf, "\\Processor({})\\% Processor Time", cpuIndex);
        buf.push_back(0);

        result = PdhAddEnglishCounterA(m_hQuery, buf.data(), 0, &m_cpuStats[cpuIndex].Counter);
        if (result != ERROR_SUCCESS)
        {
          throw std::runtime_error(fmt::format("PdhAddEnglishCounter failed with: {}", result));
        }

        m_cpuStats[cpuIndex].Initialized = true;
        m_cpuStats[cpuIndex].Value = 0.0f;
      }

      // Do a query right away just to verify that it works
      result = PdhCollectQueryData(m_hQuery);
      if (result != ERROR_SUCCESS)
      {
        throw std::runtime_error(fmt::format("PdhCollectQueryData failed with: {}", result));
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

      m_lastTryGetCpuUsage = m_timer.GetTime() - MIN_INTERVAL_CORE_CPU_USAGE;
      m_lastTryGetApplicationCpuUsageTime = m_timer.GetTime() - MIN_INTERVAL_APPLICATION_CPU_USAGE;
    }
    catch (const std::exception&)
    {
      RemoveCounters();
      PdhCloseQuery(&m_hQuery);
      throw;
    }
  }


  CpuStatsAdapterWin32::~CpuStatsAdapterWin32()
  {
    RemoveCounters();
    PdhCloseQuery(&m_hQuery);
  }


  uint32_t CpuStatsAdapterWin32::GetCpuCount() const
  {
    return m_cpuCount;
  }


  bool CpuStatsAdapterWin32::TryGetCpuUsage(float& rUsagePercentage, const uint32_t cpuIndex) const
  {
    // We rely on the service to do the actual validation
    assert(m_cpuCount <= m_cpuStats.size());
    if (cpuIndex >= m_cpuCount || !m_cpuStats[cpuIndex].Initialized)
    {
      return false;
    }

    // Ensure that we only cache the counters after the desired time has passed
    auto currentTime = m_timer.GetTime();
    auto deltaTime = currentTime - m_lastTryGetCpuUsage;
    if (deltaTime >= MIN_INTERVAL_CORE_CPU_USAGE)
    {
      m_lastTryGetCpuUsage = currentTime;
      if (!TryQueryCountersNow())
      {
        return false;
      }
    }

    rUsagePercentage = m_cpuStats[cpuIndex].Value;
    return true;
  }


  // Inspired by: http://www.philosophicalgeek.com/2009/01/03/determine-cpu-usage-of-current-process-c-and-c/
  bool CpuStatsAdapterWin32::TryGetApplicationCpuUsage(float& rUsagePercentage) const
  {
    rUsagePercentage = 0.0f;

    auto currentTime = m_timer.GetTime();
    auto deltaTime = currentTime - m_lastTryGetApplicationCpuUsageTime;
    if (deltaTime < MIN_INTERVAL_APPLICATION_CPU_USAGE)
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
      m_appCpuUsagePercentage = static_cast<float>((100.0 * appTime) / systemTime);
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


  void CpuStatsAdapterWin32::RemoveCounters() noexcept
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
  }


  bool CpuStatsAdapterWin32::TryQueryCountersNow() const
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
