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

#include <Shared/DFGraphicsBasic2D/Shared.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <sstream>
#include <cassert>
#include <cmath>

namespace Fsl
{
  Shared::Shared(const DemoAppConfig& config)
    : m_profilerService(config.DemoServiceProvider.Get<IProfilerService>())
    , m_basic2D(config.DemoServiceProvider.Get<IGraphicsService>()->GetBasic2D())
    , m_controlPoints(4 * 3)
    , m_points(512)
    , m_x1Ang(0.0f)
    , m_x2Ang(0.0f)
    , m_y1Ang(0.0f)
    , m_y2Ang(0.0f)
    , m_hCounter1(m_profilerService, m_profilerService->CreateCustomCounter("c1", 0, 100, Color::Blue()))
    , m_hCounter2(m_profilerService, m_profilerService->CreateCustomCounter("c2", 0, 1000, Color(0xFF, 0x00, 0xFF, 0xFF)))
  {
  }


  Shared::~Shared() = default;


  void Shared::Update(const DemoTime& demoTime, const PxSize2D& windowSizePx)
  {
    const auto sx = static_cast<float>(windowSizePx.Width());
    const auto sy = static_cast<float>(windowSizePx.Height());

    // Red
    m_controlPoints[0] = Vector2(0, 0);
    m_controlPoints[1] = Vector2(sx - 3, 0);
    m_controlPoints[2] = Vector2(2, sy - 1);
    m_controlPoints[3] = Vector2(sx - 2, sy - 1);
    // Green
    m_controlPoints[4] = Vector2(1, 0);
    m_controlPoints[5] = Vector2(sx - 2, 0);
    m_controlPoints[6] = Vector2(1, sy - 1);
    m_controlPoints[7] = Vector2(sx - 1, sy - 1);
    // Blue
    m_controlPoints[8] = Vector2(2, 0);
    m_controlPoints[9] = Vector2(sx - 1, 0);
    m_controlPoints[10] = Vector2(0, sy - 1);
    m_controlPoints[11] = Vector2(sx - 3, sy - 1);


    const float xAdd1 = (180 / 512.0f) * MathHelper::TO_RADS;
    const float xAdd2 = (180 / 512.0f) * MathHelper::TO_RADS * 5;
    const float yAdd1 = (360 / 512.0f) * MathHelper::TO_RADS;
    const float yAdd2 = (180 / 512.0f) * MathHelper::TO_RADS * 3;

    const float sx2 = sx * 0.5f;
    const float sy2 = sy * 0.5f;
    const float xdist1 = (sx2 / 5) * 2.0f;
    const float xdist2 = sx2 - xdist1;
    const float ydist1 = (sy2 / 10) * 6.0f;
    const float ydist2 = sy2 - ydist1;
    float x = 0.0f;
    float y = 0.0f;
    float x1Ang = m_x1Ang;
    float x2Ang = m_x2Ang;
    float y1Ang = m_y1Ang;
    float y2Ang = m_y2Ang;
    for (std::size_t i = 0; i < m_points.size(); ++i)
    {
      x = sx2 + ((std::sin(x1Ang) * xdist1) + (std::cos(x2Ang) * xdist2));
      y = sy2 + ((std::cos(y1Ang) * ydist1) + (std::sin(y2Ang) * ydist2));
      m_points[i] = Vector2(x, y);
      x1Ang += xAdd1;
      x2Ang += xAdd2;
      y1Ang += yAdd1;
      y2Ang += yAdd2;
    }

    m_x1Ang += 0.9f * demoTime.DeltaTime;
    m_x2Ang -= 1.5f * demoTime.DeltaTime;
    m_y1Ang -= 0.6f * demoTime.DeltaTime;
    m_y2Ang += 1.4f * demoTime.DeltaTime;

    // Fill the counter with a simple value
    auto funValue1 = static_cast<int32_t>(std::round(std::sin(m_x1Ang) * 500.0f + 500.0f));
    auto funValue2 = static_cast<int32_t>(std::round(std::cos(m_x2Ang) * 500.0f + 500.0f));
    funValue1 = std::min(1000, std::max(funValue1, 0));
    funValue2 = std::min(1000, std::max(funValue2, 0));
    m_profilerService->Set(m_hCounter1, funValue1 / 10);
    m_profilerService->Set(m_hCounter2, funValue2);
  }


  void Shared::Draw()
  {
    const PxSize2D fontSize = m_basic2D->FontSize();
    const auto fontHeight = static_cast<float>(fontSize.Height());

    m_basic2D->Begin();

    const auto size4 = static_cast<int32_t>(m_points.size() / 4);
    m_basic2D->DrawPoints(m_points.data(), size4, Color::White());
    m_basic2D->DrawPoints(m_points.data() + size4, size4, Color::Red());
    m_basic2D->DrawPoints(m_points.data() + size4 * 2, size4, Color::Green());
    m_basic2D->DrawPoints(m_points.data() + size4 * 3, size4, Color::Blue());


    m_basic2D->DrawString("Hello world!", Vector2(0, 1));
    m_basic2D->DrawString("  This is a longer line 01234", Vector2(0, 1 + fontHeight));
    m_basic2D->DrawString("Stuff!!", Vector2(0, 1 + (fontHeight * 2)));

    const ProfilerFrameTime lastFrame = m_profilerService->GetLastFrameTime();
    const ProfilerFrameTime averageFrames = m_profilerService->GetAverageFrameTime();
    std::stringstream tmpLast;
    tmpLast << "Total: " << lastFrame.TotalTime << " Update: " << lastFrame.UpdateTime << " Draw: " << lastFrame.DrawTime
            << " FPS: " << lastFrame.GetFramePerSecond();
    std::stringstream tmpAverage;
    tmpAverage << "Total: " << averageFrames.TotalTime << " Update: " << averageFrames.UpdateTime << " Draw: " << averageFrames.DrawTime
               << " FPS: " << averageFrames.GetFramePerSecond();

    m_basic2D->DrawString(tmpLast.str(), Vector2(0, 1 + (fontHeight * 3)));
    m_basic2D->DrawString(tmpAverage.str(), Vector2(0, 1 + (fontHeight * 4)));

    m_basic2D->DrawPoints(m_controlPoints.data(), 4, Color::Red());
    m_basic2D->DrawPoints(m_controlPoints.data() + 4, 4, Color::Green());
    m_basic2D->DrawPoints(m_controlPoints.data() + 8, 4, Color::Blue());
    m_basic2D->End();
  }
}
