#ifndef FSLDEMOPLATFORM_SERVICE_MMDCSTATS_MMDCSTATSSERVICE_HPP
#define FSLDEMOPLATFORM_SERVICE_MMDCSTATS_MMDCSTATSSERVICE_HPP
//#ifdef __linux__
/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslDemoApp/Service/ServiceProvider.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalService.hpp>
#include <FslDemoPlatform/Service/MMDCStats/IMMDCStatsService.hpp>
#include <FslDemoApp/Service/Profiler/IProfilerService.hpp>
#include <FslDemoApp/Service/Profiler/ScopedProfilerCustomCounterHandle.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <memory>

namespace Fsl
{
  class IHostInfo;
  class MmdcData;
  class MMDCStatsServiceOptionParser;

  class MMDCStatsService
    : public ThreadLocalService
    , public IMMDCStatsService
  {
    HighResolutionTimer m_timer;
    std::shared_ptr<IProfilerService> m_profiler;
    std::shared_ptr<IHostInfo> m_hostInfo;
    std::shared_ptr<MmdcData> m_mmdcData;
    //ScopedProfilerCustomCounterHandle m_handleGPU3D;
    //ScopedProfilerCustomCounterHandle m_handleGPU2D;
    //ScopedProfilerCustomCounterHandle m_handleGPUVG;
    ScopedProfilerCustomCounterHandle m_handleMMDC;
    ScopedProfilerCustomCounterHandle m_handleReads;
    ScopedProfilerCustomCounterHandle m_handleWrites;
    bool m_enabled;
    bool m_crashed;
    uint64_t m_sampleStartTime;
    // We rely on the pointer from c_str being valid so this object needs to be const
    const std::string m_mmdcUsage;
    const char* m_pszMmdcUsage;
    uint32_t m_mmdcMillisecondDuration;
    bool m_mmdcLog;
  public:
    MMDCStatsService(const ServiceProvider& serviceProvider, const std::shared_ptr<MMDCStatsServiceOptionParser>& optionParser);
    ~MMDCStatsService();
    virtual void Update();

    // From IMMDCStatsService
    virtual void SetEnabled(const bool enabled);
  private:
  };
}

//#endif
#endif
