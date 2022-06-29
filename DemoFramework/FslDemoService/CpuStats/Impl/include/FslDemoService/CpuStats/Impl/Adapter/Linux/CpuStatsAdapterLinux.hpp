#ifndef FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_LINUX_CPUSTATSADAPTERLINUX_HPP
#define FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_LINUX_CPUSTATSADAPTERLINUX_HPP
#ifdef __linux__
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

#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslDemoService/CpuStats/Impl/Adapter/ICpuStatsAdapter.hpp>
#include <FslDemoService/CpuStats/Impl/Adapter/Linux/BufferedFileParser.hpp>
#include <FslDemoService/CpuStats/Impl/Adapter/Linux/ProcFileUtil.hpp>
#include <sys/times.h>
#include <array>
#include <string>

namespace Fsl
{
  class CpuStatsAdapterLinux final : public ICpuStatsAdapter
  {
    struct ProcessTimes
    {
      uint64_t KernelTime{0};
      uint64_t UserTime{0};

      ProcessTimes() = default;
      ProcessTimes(const uint64_t kernelTime, const uint64_t userTime)
        : KernelTime(kernelTime)
        , UserTime(userTime)
      {
      }
    };

    struct CpuStats
    {
      ProcFileUtil::CPUInfo Current;
      ProcFileUtil::CPUInfo Last;
    };

    uint32_t m_cpuCount{0};
    mutable ProcessTimes m_appProcessLast;

    mutable TimeSpan m_lastTryGetCpuUsage{0};
    mutable std::string m_scratchpad;

    mutable TimeSpan m_lastTryGetApplicationCpuUsageTime{0};
    mutable float m_appCpuUsagePercentage{0.0f};

    mutable clock_t m_lastAppTime;

    mutable std::array<CpuStats, 256> m_cpuStats;

    mutable BasicFileReader m_fileReader;
    mutable BufferedFileParser<4096> m_fileParser;
    mutable bool m_coreParserEnabled;
    mutable bool m_ramParserEnabled{true};
    mutable std::vector<ProcFileUtil::CPUInfo> m_cpuInfo;

    HighResolutionTimer m_timer;

  public:
    // NOLINTNEXTLINE(google-explicit-constructor)
    CpuStatsAdapterLinux(const bool coreParserEnabled = true);
    void Process() final{};
    uint32_t GetCpuCount() const final;
    bool TryGetCpuUsage(float& rUsagePercentage, const uint32_t cpuIndex) const final;
    bool TryGetApplicationCpuUsage(float& rUsagePercentage) const final;
    bool TryGetApplicationRamUsage(uint64_t& rRamUsage) const final;

  private:
    bool TryParseCpuLoadNow() const;
    bool TryGetProcessTimes(ProcessTimes& rTimes, clock_t* pCurrentTime = nullptr) const;
  };
}

#endif
#endif
