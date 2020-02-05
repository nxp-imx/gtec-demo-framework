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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslDemoPlatform/Service/MMDCStats/MMDCStatsService.hpp>
#include <FslDemoPlatform/Service/MMDCStats/MMDCStatsServiceOptionParser.hpp>
#include <FslDemoApp/Base/Service/Host/IHostInfo.hpp>
#include <cassert>

#ifdef FSL_PLATFORM_YOCTO
#include <mmdc.h>
#endif


namespace Fsl
{
  struct MmdcStats
  {
    uint32_t TotalCycles{0};
    uint32_t BusyCycles{0};
    uint32_t ReadAccesses{0};
    uint32_t WriteAccesses{0};
    uint32_t ReadBytes{0};
    uint32_t WriteBytes{0};
    uint32_t DataLoad{0};
    uint32_t Utilization{0};
    uint32_t AccessUtilization{0};
    uint32_t AvgWriteBburstsize{0};
    uint32_t AvgReadBurstsize{0};
    //! The profile delta time (in microseconds)
    uint64_t MicrosecondDeltaTime{0};

    MmdcStats() = default;

    MmdcStats(const uint32_t totalCycles, const uint32_t busyCycles, const uint32_t readAccesses, const uint32_t writeAccesses,
              const uint32_t readBytes, const uint32_t writeBytes, const uint32_t dataLoad, const uint32_t utilization,
              const uint32_t accessUtilization, const uint32_t avgWriteBburstsize, const uint32_t avgReadBurstsize,
              const uint64_t microsecondDeltaTime)
      : TotalCycles(totalCycles)
      , BusyCycles(busyCycles)
      , ReadAccesses(readAccesses)
      , WriteAccesses(writeAccesses)
      , ReadBytes(readBytes)
      , WriteBytes(writeBytes)
      , DataLoad(dataLoad)
      , Utilization(utilization)
      , AccessUtilization(accessUtilization)
      , AvgWriteBburstsize(avgWriteBburstsize)
      , AvgReadBurstsize(avgReadBurstsize)
      , MicrosecondDeltaTime(microsecondDeltaTime)
    {
    }

    double CalcReadAccessesPerSecond() const
    {
      return (ReadAccesses * 1000.0) / (MicrosecondDeltaTime / 1000.0);
    }


    double CalcWriteAccessesPerSecond() const
    {
      return (WriteAccesses * 1000.0) / (MicrosecondDeltaTime / 1000.0);
    }


    double CalcReadBytesPerSecond() const
    {
      return (ReadBytes * 1000.0) / (MicrosecondDeltaTime / 1000.0);
    }


    double CalcWriteBytesPerSecond() const
    {
      return (WriteBytes * 1000.0) / (MicrosecondDeltaTime / 1000.0);
    }


    double CalcTotalBytesPerSecond() const
    {
      const double totalBytes = static_cast<double>(ReadBytes) + static_cast<double>(WriteBytes);
      return (totalBytes * 1000.0) / (MicrosecondDeltaTime / 1000.0);
    }
  };


  const char* AutoSelectAPI(const DemoHostFeature& api)
  {
    switch (api.Name)
    {
    case DemoHostFeatureName::G2D:
      return "GPU2D";
    case DemoHostFeatureName::OpenGL:
    case DemoHostFeatureName::OpenGLES:
      return "GPU3D";
    case DemoHostFeatureName::OpenVG:
      return "GPUVG";
    default:
      return "SUM";
    }
  }

#ifndef FSL_PLATFORM_YOCTO
  class MmdcData
  {
  public:
    MmdcData(const char* const pszType, const bool useNativeLog)
    {
      FSL_PARAM_NOT_USED(pszType);
      FSL_PARAM_NOT_USED(useNativeLog);
    }
    bool BeginProfiling(const HighResolutionTimer& timer)
    {
      FSL_PARAM_NOT_USED(timer);
      return false;
    }
    MmdcStats EndProfiling(const HighResolutionTimer& timer)
    {
      FSL_PARAM_NOT_USED(timer);
      return {};
    }
  };
#else
  class MmdcData
  {
    char* m_pszType;
    bool m_useMMDCLog;
    bool m_isProfiling;
    uint64_t m_sampleStartTime{};
    MMDC_CONTEXT_t m_context;

  public:
    MmdcData(const char* const pszType, const bool useNativeLog)
      : m_pszType(const_cast<char*>(pszType))
      , m_useMMDCLog(useNativeLog)
      , m_isProfiling(false)
    {
      MMDC_INFO_t info;
      if (get_mmdc_info(&info) == 0)
      {
        throw InitFailedException("Failed to retrieve mmdc info");
      }
      if (begin_mmdc(&m_context) == 0)
      {
        throw InitFailedException("Failed to init mmdc code");
      }

      FSLLOG3_INFO("MMDC profiling enabled");
    }

    ~MmdcData()
    {
      if (m_isProfiling)
      {
        stop_mmdc_profiling(m_context.mmdc);
      }

      end_mmdc(&m_context);
    }

    bool BeginProfiling(const HighResolutionTimer& timer)
    {
      if (auto_configuration_mmdc_profiling(&m_context, m_pszType, m_useMMDCLog ? 1 : 0) == 0)
      {
        FSLLOG3_INFO("MMDC configure failed");
        return false;
      }

      m_isProfiling = true;
      clear_mmdc_results(m_context.mmdc);
      m_sampleStartTime = timer.GetTime();
      start_mmdc_profiling(m_context.mmdc);
      return true;
    }

    MmdcStats EndProfiling(const HighResolutionTimer& timer)
    {
      assert(m_isProfiling);
      m_isProfiling = false;

      MMDC_PROFILE_RES_t results;
      load_mmdc_results(m_context.mmdc);
      get_mmdc_profiling_results(m_context.mmdc, &results);

      const auto sampleEndTime = timer.GetTime();
      const MmdcStats result(results.total_cycles, results.busy_cycles, results.read_accesses, results.write_accesses, results.read_bytes,
                             results.write_bytes, results.data_load, results.utilization, results.access_utilization, results.avg_write_burstsize,
                             results.avg_read_burstsize, sampleEndTime - m_sampleStartTime);

      if (m_useMMDCLog)
      {
        print_mmdc_profiling_results(results, RES_FULL, result.MicrosecondDeltaTime / 1000);
      }

      stop_mmdc_profiling(m_context.mmdc);
      return result;
    }

  private:
  };
#endif


  MMDCStatsService::MMDCStatsService(const ServiceProvider& serviceProvider, const std::shared_ptr<MMDCStatsServiceOptionParser>& optionParser)
    : ThreadLocalService(serviceProvider)
    , m_profiler(serviceProvider.Get<IProfilerService>())
    , m_hostInfo(serviceProvider.Get<IHostInfo>())
    //, m_handleGPU3D(m_profiler, m_profiler->CreateCustomCounter("%_3d", 0, 1000, Color(0xFF70DB93)))
    //, m_handleGPU2D(m_profiler, m_profiler->CreateCustomCounter("%_2d", 0, 1000, Color(0xFF70DB93)))
    //, m_handleGPUVG(m_profiler, m_profiler->CreateCustomCounter("%_vg", 0, 1000, Color(0xFF70DB93)))
    , m_enabled(false)
    , m_crashed(false)
    , m_mmdcUsage(optionParser->GetUsageString())
    , m_pszMmdcUsage(nullptr)
    , m_mmdcMillisecondDuration(optionParser->GetMillisecondDuration())
    , m_mmdcLog(optionParser->IsLogEnabled())
  {
  }


  MMDCStatsService::~MMDCStatsService() = default;


  void MMDCStatsService::Update()
  {
    ThreadLocalService::Update();

    if (m_crashed)
    {
      return;
    }

    if (!m_enabled)
    {
      // for now we poll the host to figure out the api
      const auto activeAPI = m_hostInfo->GetActiveAPI();
      if (activeAPI.Name == DemoHostFeatureName::Stub)
      {
        return;
      }

      if (m_mmdcUsage == "DEFAULT")
      {
        m_pszMmdcUsage = AutoSelectAPI(activeAPI);
      }

      // We default to enabled
      SetEnabled(true);
    }

    auto time = m_timer.GetTime();
    auto delta = time - m_sampleStartTime;
    const uint64_t sampleDuration = m_mmdcMillisecondDuration * static_cast<uint64_t>(1000);
    if (m_mmdcData && delta >= sampleDuration)
    {
      const auto stats = m_mmdcData->EndProfiling(m_timer);

      // we got new stats

      // FSLLOG3_INFO("TotalCycles: " << stats.TotalCycles
      //        << " TotalCycles: " << stats.TotalCycles
      //        << " BusyCycles: " << stats.BusyCycles
      //        << " ReadAccesses: " << stats.ReadAccesses
      //        << " WriteAccesses: " << stats.WriteAccesses
      //        << " ReadBytes: " << stats.ReadBytes
      //        << " WriteBytes: " << stats.WriteBytes
      //        << " DataLoad: " << stats.DataLoad
      //        << " Utilization: " << stats.Utilization
      //        << " AccessUtilization: " << stats.AccessUtilization
      //        << " AvgWriteBburstsize: " << stats.AvgWriteBburstsize
      //        << " AvgReadBurstsize: " << stats.AvgReadBurstsize
      //        << " MicrosecondDeltaTime: " << stats.MicrosecondDeltaTime
      //        );

      const auto dataLoadPercentage = static_cast<int32_t>(stats.DataLoad);
      // const int32_t readAccessesPerSec = static_cast<int32_t>(stats.CalcReadAccessesPerSecond() / 1000);
      // const int32_t writeAccessesPerSec = static_cast<int32_t>(stats.CalcWriteAccessesPerSecond() / 1000);
      const auto readMBPerSec = static_cast<int32_t>(stats.CalcReadBytesPerSecond() / (1024 * 1024));
      const auto writeMBPerSec = static_cast<int32_t>(stats.CalcWriteBytesPerSecond() / (1024 * 1024));

      m_profiler->Set(m_handleMMDC, dataLoadPercentage);
      m_profiler->Set(m_handleReads, readMBPerSec);
      m_profiler->Set(m_handleWrites, writeMBPerSec);
      if (!m_mmdcData->BeginProfiling(m_timer))
      {
        m_crashed = true;
      }
      m_sampleStartTime = m_timer.GetTime();
    }
    else if (!m_mmdcData)
    {
      m_mmdcData = std::make_shared<MmdcData>(m_pszMmdcUsage, m_mmdcLog);
      if (!m_mmdcData->BeginProfiling(m_timer))
      {
        m_crashed = true;
      }
      m_sampleStartTime = time;
    }
  }


  void MMDCStatsService::SetEnabled(const bool enabled)
  {
    if (enabled != m_enabled)
    {
      m_enabled = enabled;
      if (!enabled)
      {
        m_handleMMDC.Reset();
        m_handleReads.Reset();
        m_handleWrites.Reset();
      }
      else
      {
        m_handleMMDC.Reset(m_profiler, m_profiler->CreateCustomCounter("%_mmdc", 0, 100, Color(0xFF70DB93)));
        m_handleReads.Reset(m_profiler, m_profiler->CreateCustomCounter("r", 0, 5000, Color(0xFFFFAE00)));
        m_handleWrites.Reset(m_profiler, m_profiler->CreateCustomCounter("w", 0, 5000, Color(0xFF00FF48)));

        if (m_mmdcUsage == "DEFAULT")
        {
          // a profile has been selected
        }
        else if (m_mmdcUsage == "CUSTOM")
        {
          m_pszMmdcUsage = nullptr;
        }
        else
        {
          m_pszMmdcUsage = m_mmdcUsage.c_str();
        }
      }
    }
  }
}
