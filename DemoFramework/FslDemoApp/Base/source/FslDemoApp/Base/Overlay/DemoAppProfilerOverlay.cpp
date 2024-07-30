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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/String/StringCompat.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/Overlay/DemoAppProfilerOverlay.hpp>
#include <FslDemoService/CpuStats/ICpuStatsService.hpp>
#include <FslDemoService/Graphics/IBasic2D.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/Profiler/DefaultProfilerColors.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <utility>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr int32_t DefaultGraphWidth = 120;
      constexpr int32_t DefaultGraphHeight = 200;
    }

    int32_t MinimumDigits(const int32_t value)
    {
      return value != 0 ? static_cast<int32_t>(std::floor(std::log10(std::abs(value))) + 1) : 1;
    }
  }


  DemoAppProfilerOverlay::CustomRecord::CustomRecord(const ProfilerCustomCounterHandle& handle, const ProfilerCustomCounterDesc& desc,
                                                     std::shared_ptr<DemoAppProfilerGraph> graph)
    : Handle(handle)
    , Name(desc.Name)
    , TheColor(desc.ColorHint)
    , Graph(std::move(graph))
  {
    int32_t digits1 = MinimumDigits(desc.MinValue);
    const int32_t digits2 = MinimumDigits(desc.MaxValue);
    if (desc.MinValue < 0)
    {
      ++digits1;
    }

    const int32_t maxDigits = std::max(digits1, digits2);

    StringCompat::sprintf_s(FormatString.data(), FormatString.size(), "{:%d}", maxDigits);
  }


  DemoAppProfilerOverlay::DemoAppProfilerOverlay(const ServiceProvider& serviceProvider, const DemoAppStatsFlags& logStatsFlags)
    : m_logStatsFlags(logStatsFlags)
    , m_graphTotal(0, 2000, Point2(LocalConfig::DefaultGraphWidth, LocalConfig::DefaultGraphHeight))
    , m_graphUpdate(0, 2000, Point2(LocalConfig::DefaultGraphWidth, LocalConfig::DefaultGraphHeight))
    , m_graphDraw(0, 2000, Point2(LocalConfig::DefaultGraphWidth, LocalConfig::DefaultGraphHeight))
    , m_graphCPU(0, 1000, Point2(LocalConfig::DefaultGraphWidth, LocalConfig::DefaultGraphHeight))
    , m_customConfigurationRevision(0)    // the profiler service never returns zero for its revision

  {
    m_profilerService = serviceProvider.Get<IProfilerService>();
    m_graphicsService = serviceProvider.TryGet<IGraphicsService>();
    m_cpuStatsService = serviceProvider.TryGet<ICpuStatsService>();
  }


  DemoAppProfilerOverlay::~DemoAppProfilerOverlay() = default;


  void DemoAppProfilerOverlay::Draw(const DemoWindowMetrics& windowMetrics)
  {
    MaintainCachedCustomEntries();

    std::shared_ptr<IBasic2D> basic2D = m_basic2D.lock();
    if (!basic2D && m_graphicsService)
    {
      m_basic2D = m_graphicsService->GetBasic2D();
      basic2D = m_basic2D.lock();
    }

    if (basic2D)
    {
      const ProfilerFrameTime frameTime = m_profilerService->GetLastFrameTime();
      m_graphTotal.Add(frameTime.TotalTime / 100);
      m_graphUpdate.Add(frameTime.UpdateTime / 100);
      m_graphDraw.Add(frameTime.DrawTime / 100);

      basic2D->Begin();

      bool showFPS = true;
      bool showMilliseconds = true;

      if (showFPS || showMilliseconds)
      {
        m_scracthpad.clear();
        if (m_logStatsFlags.IsFlagged(DemoAppStatsFlags::Frame))
        {
          if (showFPS && showMilliseconds)
          {
            fmt::format_to(std::back_inserter(m_scracthpad), "{:5.1f}FPS {:4.1f}ms", frameTime.GetFramePerSecond(),
                           static_cast<double>(frameTime.TotalTime) / 1000.0);
          }
          else if (showFPS)
          {
            fmt::format_to(std::back_inserter(m_scracthpad), "{:5.1f}FPS", frameTime.GetFramePerSecond());
          }
          else if (showMilliseconds)
          {
            fmt::format_to(std::back_inserter(m_scracthpad), "{:4.1f}ms", static_cast<double>(frameTime.TotalTime) / 1000.0);
          }
        }
        if (m_logStatsFlags.IsFlagged(DemoAppStatsFlags::CPU) && m_cpuStatsService)
        {
          float cpuUsage = 0.0f;
          if (m_cpuStatsService->TryGetApplicationCpuUsage(cpuUsage))
          {
            m_graphCPU.Add(static_cast<int32_t>(std::round(cpuUsage * 10.0f)));
            fmt::format_to(std::back_inserter(m_scracthpad), "{}{:5.1f}cpu", m_scracthpad.size() > 0 ? " " : "", cpuUsage);
          }
        }

        const PxSize2D fontSize = basic2D->FontSize();
        Vector2 dstPos(0.0f, static_cast<float>(windowMetrics.ExtentPx.Height.Value - fontSize.RawHeight()));
        if (m_scracthpad.size() > 0)
        {
          basic2D->DrawString(StringViewLite(m_scracthpad.data(), m_scracthpad.size()), dstPos);
        }

        {    // Render text for the custom counters
          dstPos.X += static_cast<float>((m_scracthpad.size() + 1) * fontSize.RawWidth());
          auto itr = m_customCounters.begin();
          while (itr != m_customCounters.end())
          {
            m_scracthpad.clear();
            std::string_view formatString(itr->FormatString.data());
            const auto stuff = m_profilerService->Get(itr->Handle);
            fmt::vformat_to(std::back_inserter(m_scracthpad), formatString, fmt::make_format_args(stuff));
            if (m_scracthpad.size() > 0u)
            {
              basic2D->DrawString(StringViewLite(m_scracthpad.data(), m_scracthpad.size()), dstPos);
              dstPos.X += static_cast<float>(m_scracthpad.size() * fontSize.RawWidth());
              basic2D->DrawString(itr->Name, dstPos);
              dstPos.X += static_cast<float>((itr->Name.size() + 1) * fontSize.RawWidth());
            }
            ++itr;
          }
        }
      }

      const Point2 graphSize = m_graphTotal.GetSize();
      const Vector2 dstPosGraph(static_cast<float>(windowMetrics.ExtentPx.Width.Value - graphSize.X),
                                static_cast<float>(windowMetrics.ExtentPx.Height.Value - graphSize.Y));

      UpdateAndDrawCustomCounters(basic2D, dstPosGraph);

      if (m_logStatsFlags.IsFlagged(DemoAppStatsFlags::CPU))
      {
        m_graphCPU.Draw(basic2D, dstPosGraph, DefaultProfilerColors::CpuLoad);
      }

      if (m_logStatsFlags.IsFlagged(DemoAppStatsFlags::Frame))
      {
        m_graphTotal.Draw(basic2D, dstPosGraph, DefaultProfilerColors::Total);
        m_graphUpdate.Draw(basic2D, dstPosGraph, DefaultProfilerColors::Update);
        m_graphDraw.Draw(basic2D, dstPosGraph, DefaultProfilerColors::Draw);
      }

      basic2D->End();
    }
  }


  void DemoAppProfilerOverlay::MaintainCachedCustomEntries()
  {
    uint32_t currentRevision = m_profilerService->GetCustomConfigurationRevision();
    if (currentRevision == m_customConfigurationRevision)
    {
      return;
    }

    // Something changed so lets update our cached entries
    m_customConfigurationRevision = currentRevision;

    const int32_t count = m_profilerService->GetCustomCounterCount();
    if (count <= 0)
    {
      m_customCounters.clear();
      return;
    }

    // Remove all outdated entries
    {
      auto itr = m_customCounters.begin();
      while (itr != m_customCounters.end())
      {
        if (m_profilerService->IsValidHandle(itr->Handle))
        {
          ++itr;
        }
        else
        {
          itr = m_customCounters.erase(itr);
        }
      }
    }

    // Add all new entries
    for (int32_t i = 0; i < count; ++i)
    {
      const ProfilerCustomCounterHandle handle = m_profilerService->GetCustomCounterHandle(i);
      if (IsNewHandle(handle))
      {
        const ProfilerCustomCounterDesc desc = m_profilerService->GetDescription(handle);
        auto ptr = std::make_shared<DemoAppProfilerGraph>(desc.MinValue, desc.MaxValue,
                                                          Point2(LocalConfig::DefaultGraphWidth, LocalConfig::DefaultGraphHeight));
        m_customCounters.emplace_back(handle, desc, ptr);
      }
    }
  }


  bool DemoAppProfilerOverlay::IsNewHandle(const ProfilerCustomCounterHandle& handle) const
  {
    auto itr = m_customCounters.begin();
    while (itr != m_customCounters.end())
    {
      if (itr->Handle == handle)
      {
        return false;
      }
      ++itr;
    }
    return true;
  }


  void DemoAppProfilerOverlay::UpdateAndDrawCustomCounters(const std::shared_ptr<IBasic2D>& basic2D, const Vector2& dstPos)
  {
    auto itr = m_customCounters.begin();
    while (itr != m_customCounters.end())
    {
      itr->Graph->Add(m_profilerService->Get(itr->Handle));
      itr->Graph->Draw(basic2D, dstPos, itr->TheColor);
      ++itr;
    }
  }
}
