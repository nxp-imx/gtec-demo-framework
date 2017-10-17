#ifndef FSLDEMOHOST_BASE_DEMOAPPPROFILEROVERLAY_HPP
#define FSLDEMOHOST_BASE_DEMOAPPPROFILEROVERLAY_HPP
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

#include <FslDemoHost/Base/DemoAppProfilerGraph.hpp>
#include <FslDemoApp/Base/Service/Profiler/ProfilerCustomCounterHandle.hpp>
#include <FslGraphics/Color.hpp>
#include <list>
#include <memory>
#include <string>

namespace Fsl
{
  class Basic2D;
  struct DemoAppSetup;
  class DemoAppConfig;
  class IGraphicsService;
  class IProfilerService;
  struct Point2;
  struct ProfilerCustomCounterDesc;

  class DemoAppProfilerOverlay
  {
    struct CustomRecord
    {
      ProfilerCustomCounterHandle Handle;
      std::string Name;
      Color TheColor;
      std::shared_ptr<DemoAppProfilerGraph> Graph;
      char FormatString[10+3];  // 10 so we can hold the largest positive int32_t and +3 for '%','d' and terminating zero

      CustomRecord()
        : Handle()
        , Name()
        , TheColor()
        , Graph()
      {
        FormatString[0] = '%';
        FormatString[1] = 'd';
        FormatString[2] = 0;
      }

      CustomRecord(const ProfilerCustomCounterHandle& handle, const ProfilerCustomCounterDesc& desc, const std::shared_ptr<DemoAppProfilerGraph>& graph);
    };

    std::shared_ptr<IProfilerService> m_profilerService;
    std::shared_ptr<IGraphicsService> m_graphicsService;
    std::weak_ptr<Basic2D> m_basic2D;

    DemoAppProfilerGraph m_graphTotal;
    DemoAppProfilerGraph m_graphUpdate;
    DemoAppProfilerGraph m_graphDraw;

    uint32_t m_customConfigurationRevision;
    std::list<CustomRecord> m_customCounters;
  public:
    DemoAppProfilerOverlay(const DemoAppSetup& demoAppSetup, const DemoAppConfig& demoAppConfig);
    ~DemoAppProfilerOverlay();

    void Draw(const Point2& screenResolution);
  private:
    void MaintainCachedCustomEntries();
    bool IsNewHandle(const ProfilerCustomCounterHandle& handle) const;
    void UpdateAndDrawCustomCounters(const std::shared_ptr<Basic2D>& basic2D, const Vector2& dstPosGraph);
  };
}

#endif
