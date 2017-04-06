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

#include <FslDemoHost/DemoAppProfilerOverlay.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslDemoApp/DemoAppConfig.hpp>
#include <FslDemoApp/Service/Graphics/Basic2D.hpp>
#include <FslDemoApp/Service/Graphics/IGraphicsService.hpp>
#include <FslDemoApp/Service/Profiler/IProfilerService.hpp>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>

#if defined(__QNXNTO__)
#include <stdio.h>
#endif

namespace Fsl
{
  namespace
  {
    const int32_t DEFAULT_GRAPH_WIDTH = 120;
    const int32_t DEFAULT_GRAPH_HEIGHT = 200;

    int32_t MinimumDigits(const int32_t value)
    {
      return value != 0 ? static_cast<int32_t>(std::floor(std::log10(std::abs(value))) + 1) : 1;
    }

  }


#ifdef _WIN32
#pragma warning( push )
// Disable the warning about unsafe method under windows (unfortunately visual studio does not support snprintf)
#pragma warning( disable : 4996)
#endif
  DemoAppProfilerOverlay::CustomRecord::CustomRecord(const ProfilerCustomCounterHandle& handle, const ProfilerCustomCounterDesc& desc, const std::shared_ptr<DemoAppProfilerGraph>& graph)
    : Handle(handle)
    , Name(desc.Name)
    , TheColor(desc.ColorHint)
    , Graph(graph)
  {
    int32_t digits1 = MinimumDigits(desc.MinValue);
    const int32_t digits2 = MinimumDigits(desc.MaxValue);
    if (desc.MinValue < 0)
      ++digits1;

    const int32_t maxDigits = std::max(digits1, digits2);

    std::sprintf(FormatString, "%%%dd", maxDigits);
  }
#ifdef _WIN32
#pragma warning( pop )
#endif


  DemoAppProfilerOverlay::DemoAppProfilerOverlay(const DemoAppSetup& demoAppSetup, const DemoAppConfig& demoAppConfig)
    : m_profilerService()
    , m_graphicsService()
    , m_basic2D()
    , m_graphTotal(0, 2000, Point2(DEFAULT_GRAPH_WIDTH, DEFAULT_GRAPH_HEIGHT))
    , m_graphUpdate(0, 2000, Point2(DEFAULT_GRAPH_WIDTH, DEFAULT_GRAPH_HEIGHT))
    , m_graphDraw(0, 2000, Point2(DEFAULT_GRAPH_WIDTH, DEFAULT_GRAPH_HEIGHT))
    , m_customConfigurationRevision(0) // the profiler service never returns zero for its revision
    , m_customCounters()
  {
    m_profilerService = demoAppConfig.DemoServiceProvider.Get<IProfilerService>();
    m_graphicsService = demoAppConfig.DemoServiceProvider.Get<IGraphicsService>();
  }


  DemoAppProfilerOverlay::~DemoAppProfilerOverlay()
  {

  }


#ifdef _WIN32
// Disable the warning about unsafe method under windows (unfortunately visual studio does not support snprintf)
#pragma warning( push )
#pragma warning( disable : 4996)
#endif

  void DemoAppProfilerOverlay::Draw(const Point2& screenResolution)
  {
    MaintainCachedCustomEntries();

    std::shared_ptr<Basic2D> basic2D = m_basic2D.lock();
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
      bool showMicro = true;

      if (showFPS || showMicro)
      {
        char tmp[256];
        int numChars = 0;
        if (showFPS && showMicro)
          numChars = sprintf(tmp, "%.1fFPS %.1fms", frameTime.GetFramePerSecond(), frameTime.TotalTime / 1000.0f);
        else if (showFPS)
          numChars = sprintf(tmp, "%.1fFPS", frameTime.GetFramePerSecond());
        else if (showMicro)
          numChars = sprintf(tmp, "%.1fms", frameTime.TotalTime / 1000.0f);

        const Point2 fontSize = basic2D->FontSize();
        Vector2 dstPos(0.0f, static_cast<float>(screenResolution.Y - fontSize.Y));
        if ( numChars > 0 )
          basic2D->DrawString(tmp, dstPos);

        { // Render text for the custom counters
          numChars = std::max(numChars, 0);
          dstPos.X += (numChars + 1) * fontSize.X;
          std::list<CustomRecord>::const_iterator itr = m_customCounters.begin();
          while (itr != m_customCounters.end())
          {
            numChars = sprintf(tmp, itr->FormatString, m_profilerService->Get(itr->Handle));
            if (numChars > 0)
            {
              basic2D->DrawString(tmp, dstPos);
              dstPos.X += numChars * fontSize.X;
              basic2D->DrawString(itr->Name, dstPos);
              dstPos.X += (itr->Name.size() + 1) * fontSize.X;
            }
            ++itr;
          }
        }
      }

      const Point2 graphSize = m_graphTotal.GetSize();
      const Vector2 dstPosGraph(static_cast<float>(screenResolution.X - graphSize.X), static_cast<float>(screenResolution.Y - graphSize.Y));

      UpdateAndDrawCustomCounters(basic2D, dstPosGraph);

      m_graphTotal.Draw(basic2D, dstPosGraph, Color::Green());
      m_graphUpdate.Draw(basic2D, dstPosGraph, Color::Cyan());
      m_graphDraw.Draw(basic2D, dstPosGraph, Color::Yellow());

      basic2D->End();
    }
  }

#ifdef _WIN32
#pragma warning( pop )
#endif

  void DemoAppProfilerOverlay::MaintainCachedCustomEntries()
  {
    uint32_t currentRevision = m_profilerService->GetCustomConfigurationRevision();
    if (currentRevision == m_customConfigurationRevision)
      return;

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
      std::list<CustomRecord>::iterator itr = m_customCounters.begin();
      while (itr != m_customCounters.end())
      {
        if (m_profilerService->IsValidHandle(itr->Handle))
          ++itr;
        else
          itr = m_customCounters.erase(itr);
      }
    }

    // Add all new entries
    for (int32_t i = 0; i < count; ++i)
    {
      const ProfilerCustomCounterHandle handle = m_profilerService->GetCustomCounterHandle(i);
      if (IsNewHandle(handle))
      {
        const ProfilerCustomCounterDesc desc = m_profilerService->GetDescription(handle);
        auto ptr = std::make_shared<DemoAppProfilerGraph>(desc.MinValue, desc.MaxValue, Point2(DEFAULT_GRAPH_WIDTH, DEFAULT_GRAPH_HEIGHT));
        m_customCounters.push_back(CustomRecord(handle, desc, ptr));
      }
    }

  }


  bool DemoAppProfilerOverlay::IsNewHandle(const ProfilerCustomCounterHandle& handle) const
  {
    std::list<CustomRecord>::const_iterator itr = m_customCounters.begin();
    while (itr != m_customCounters.end())
    {
      if (itr->Handle == handle)
        return false;
      ++itr;
    }
    return true;
  }


  void DemoAppProfilerOverlay::UpdateAndDrawCustomCounters(const std::shared_ptr<Basic2D>& basic2D, const Vector2& dstPos)
  {
    std::list<CustomRecord>::const_iterator itr = m_customCounters.begin();
    while (itr != m_customCounters.end())
    {
      itr->Graph->Add(m_profilerService->Get(itr->Handle));
      itr->Graph->Draw(basic2D, dstPos, itr->TheColor);
      ++itr;
    }
  }

}
