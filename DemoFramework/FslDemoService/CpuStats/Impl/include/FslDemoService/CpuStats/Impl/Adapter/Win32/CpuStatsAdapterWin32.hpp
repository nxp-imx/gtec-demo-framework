#ifndef FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_WIN32_CPUSTATSADAPTERWIN32_HPP
#define FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_WIN32_CPUSTATSADAPTERWIN32_HPP
#ifdef _WIN32
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
#include <FslDemoService/CpuStats/Impl/Adapter/ICpuStatsAdapter.hpp>
#include <pdh.h>
#include <array>

namespace Fsl
{
  class CpuStatsAdapterWin32 : public ICpuStatsAdapter
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
      bool Initialized{false};
      PDH_HCOUNTER Counter{};
      float Value{0.0f};
    };

    PDH_HQUERY m_hQuery{};
    //! the number of cpus
    uint32_t m_cpuCount{0};
    mutable ProcessTimes m_appSystemLast;
    mutable ProcessTimes m_appProcessLast;

    mutable uint64_t m_lastTryGetCpuUsage{0};
    mutable uint64_t m_lastTryGetApplicationCpuUsageTime{0};
    mutable float m_appCpuUsagePercentage{0.0f};

    // We allow up to 256 cpus
    mutable std::array<CpuStats, 256> m_cpuStats;

    HighResolutionTimer m_timer;

  public:
    CpuStatsAdapterWin32();
    ~CpuStatsAdapterWin32() override;

    void Process() override{};
    uint32_t GetCpuCount() const override;
    bool TryGetCpuUsage(float& rUsagePercentage, const uint32_t cpuIndex) const override;
    bool TryGetApplicationCpuUsage(float& rUsagePercentage) const override;
    bool TryGetApplicationRamUsage(uint64_t& rRamUsage) const override;

  private:
    void RemoveCounters() noexcept;
    bool TryQueryCountersNow() const;
    bool TryGetProcessTimes(ProcessTimes& rTimes) const;
    bool TryGetSystemTimes(ProcessTimes& rTimes) const;
  };
}

#endif
#endif
