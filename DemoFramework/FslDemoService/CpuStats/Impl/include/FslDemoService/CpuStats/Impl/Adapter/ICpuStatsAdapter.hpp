#ifndef FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_ICPUSTATSADAPTER_HPP
#define FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_ICPUSTATSADAPTER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDemoService/CpuStats/CpuUsageRecord.hpp>

namespace Fsl
{
  //! @note Be very careful with what is used here as its the bottom layer.
  class ICpuStatsAdapter
  {
  public:
    virtual ~ICpuStatsAdapter() = default;

    //! @brief Allow the adapter to do some processing
    virtual void Process() = 0;

    //! @brief Get the number of CPU's available
    virtual uint32_t GetCpuCount() const = 0;

    //! @brief Get the current usage for the given cpuIndex.
    virtual bool TryGetCpuUsage(CpuUsageRecord& rUsageRecord, const uint32_t cpuIndex) const = 0;

    //! @brief Get the total application CPU usage.
    virtual bool TryGetApplicationCpuUsage(CpuUsageRecord& rUsageRecord) const = 0;

    //! @brief Get the total application total ram usage.
    virtual bool TryGetApplicationRamUsage(uint64_t& rRamUsage) const = 0;
  };
}

#endif
