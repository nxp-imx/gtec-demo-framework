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

#include <FslDemoService/CpuStats/Impl/CpuStatsService.hpp>
#include <FslDemoService/CpuStats/Impl/Adapter/ICpuStatsAdapter.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <algorithm>

namespace Fsl
{
  CpuStatsService::CpuStatsService(const ServiceProvider& serviceProvider,
                                   const std::function<std::unique_ptr<ICpuStatsAdapter>()>& fnAllocateAdapter)
    : ThreadLocalService(serviceProvider)
  {
    if (!fnAllocateAdapter)
    {
      throw std::invalid_argument("allocate adapter function must be valid");
    }
    try
    {
      m_adapter = fnAllocateAdapter();
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_WARNING("Failed to allocate adapter {}", ex.what());
      m_adapter.reset();
    }

    if (m_adapter)
    {
      // Cache the CPU count
      m_cpuCount = m_adapter->GetCpuCount();
    }
    else
    {
      m_cpuCount = 0;
    }
  }


  CpuStatsService::~CpuStatsService() = default;


  void CpuStatsService::Update()
  {
    if (m_adapter)
    {
      m_adapter->Process();
    }
  }


  uint32_t CpuStatsService::GetCpuCount() const
  {
    return m_cpuCount;
  }


  bool CpuStatsService::TryGetCpuUsage(float& rUsagePercentage, const uint32_t cpuIndex) const
  {
    rUsagePercentage = 0.0f;
    if (!m_adapter)
    {
      FSLLOG3_DEBUG_INFO("not available");
      return false;
    }
    if (cpuIndex >= m_cpuCount)
    {
      FSLLOG3_DEBUG_INFO("cpuIndex out of bounds");
      return false;
    }
    return m_adapter->TryGetCpuUsage(rUsagePercentage, cpuIndex);
    //{
    //  return false;
    //}
    //// rUsagePercentage = std::max(std::min(rUsagePercentage, 100.0f), 0.0f);
    // return true;
  }


  bool CpuStatsService::TryGetApplicationCpuUsage(float& rUsagePercentage) const
  {
    rUsagePercentage = 0.0f;
    if (!m_adapter)
    {
      FSLLOG3_DEBUG_INFO("not available");
      return false;
    }
    return m_adapter->TryGetApplicationCpuUsage(rUsagePercentage);
    //{
    //  return false;
    //}
    //// rUsagePercentage = std::max(std::min(rUsagePercentage, 100.0f), 0.0f);
    // return true;
  }


  bool CpuStatsService::TryGetApplicationRamUsage(uint64_t& rRamUsage) const
  {
    rRamUsage = 0u;
    if (!m_adapter)
    {
      FSLLOG3_DEBUG_INFO("not available");
      return false;
    }
    return m_adapter->TryGetApplicationRamUsage(rRamUsage);
  }
}
