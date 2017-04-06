#ifndef DFGRAPHICSBASIC2DSHARED_SHARED_HPP
#define DFGRAPHICSBASIC2DSHARED_SHARED_HPP
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

#include <FslDemoApp/DemoAppConfig.hpp>
#include <FslDemoApp/DemoTime.hpp>
#include <FslDemoApp/Service/Graphics/Basic2D.hpp>
#include <FslDemoApp/Service/Profiler/IProfilerService.hpp>
#include <FslDemoApp/Service/Profiler/ScopedProfilerCustomCounterHandle.hpp>
#include <vector>

namespace Fsl
{
  class Shared
  {
    std::shared_ptr<IProfilerService> m_profilerService;
    std::shared_ptr<Basic2D> m_basic2D;
    std::vector<Vector2> m_controlPoints;
    std::vector<Vector2> m_points;
    float m_x1Ang;
    float m_x2Ang;
    float m_y1Ang;
    float m_y2Ang;
    ScopedProfilerCustomCounterHandle m_hCounter1;
    ScopedProfilerCustomCounterHandle m_hCounter2;
  public:
    Shared(const DemoAppConfig& config);
    ~Shared();
    void Update(const DemoTime& demoTime, const Point2& currentScreenResolution);
    void Draw();
  };
}

#endif
