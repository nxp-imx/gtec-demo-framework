/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <Shared/System/OnDemandRendering/JankDetector.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <algorithm>
#include <cmath>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr double CameraResetPos = 10000.0;
      constexpr int32_t SmallBlockResolution1 = 50;
      constexpr int32_t SmallBlockResolution2 = 1;
    }
  }

  JankDetector::JankDetector() = default;


  JankDetector::~JankDetector() = default;

  void JankDetector::ResetAnim()
  {
    m_animationRecords = {};
    m_timing.Clear();
  }


  void JankDetector::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_cachedWindowMetrics = windowMetrics;
    // Ensure that we have enough timing entries
    const std::size_t desiredCapacity = windowMetrics.GetSizePx().Width() * std::size_t(2);
    m_timing.EnsureCapacity(desiredCapacity);
    ResetAnim();
  }

  void JankDetector::FixedUpdate(const DemoTime& demoTime, const bool animationEnabled, const uint32_t refreshRate)
  {
    FSLLOG3_WARNING_IF(refreshRate < 1, "RefreshRate must be > 0, forcing it to 1");
    const uint32_t cappedRefreshRate = std::max(refreshRate, uint32_t(1));
    if (animationEnabled)
    {
      if (!m_animationRecords.IsEnabled)
      {
        // Its important that we start the animation here to keep everything in sync
        m_animationRecords = {};
        m_animationRecords.IsEnabled = true;
        m_animationRecords.Anim0.SpeedPerSecond = static_cast<float>(m_cachedWindowMetrics.GetSizePx().Width() * 2);
        m_animationRecords.Anim0.FixedSpeedPerFrame = m_animationRecords.Anim0.SpeedPerSecond / cappedRefreshRate;

        m_animationRecords.Anim1.SpeedPerSecond = static_cast<float>(cappedRefreshRate) * LocalConfig::SmallBlockResolution1;
        m_animationRecords.Anim1.FixedSpeedPerFrame = LocalConfig::SmallBlockResolution1;
        m_timing.Clear();

        m_animationRecords.Anim2.SpeedPerSecond = static_cast<float>(cappedRefreshRate) * LocalConfig::SmallBlockResolution2;
        m_animationRecords.Anim2.FixedSpeedPerFrame = LocalConfig::SmallBlockResolution2;
      }
    }
    else
    {
      m_animationRecords.IsEnabled = false;
    }

    if (m_animationRecords.IsEnabled)
    {
      if (m_animationRecords.Anim0.AnimCamera >= LocalConfig::CameraResetPos)
      {
        FSLLOG3_VERBOSE("Resetting camera position");
        // Reset the camera position to keep the precision high
        m_animationRecords.Anim0.Clear();
      }
      if (m_animationRecords.Anim1.AnimBox0 >= m_cachedWindowMetrics.GetSizePx().Width() &&
          m_animationRecords.Anim1.AnimBox1 >= m_cachedWindowMetrics.GetSizePx().Width() &&
          m_animationRecords.Anim1.AnimBox2 >= m_cachedWindowMetrics.GetSizePx().Width())
      {
        // Reset animation
        m_animationRecords.Anim1.Clear();
        m_timing.ClearCount();
      }
      if (m_animationRecords.Anim2.AnimBox0 >= m_cachedWindowMetrics.GetSizePx().Width() &&
          m_animationRecords.Anim2.AnimBox1 >= m_cachedWindowMetrics.GetSizePx().Width() &&
          m_animationRecords.Anim2.AnimBox2 >= m_cachedWindowMetrics.GetSizePx().Width())
      {
        // Reset animation
        m_animationRecords.Anim2.Clear();
      }


      m_animationRecords.Anim0.AnimBox1 += static_cast<double>(m_animationRecords.Anim0.SpeedPerSecond) * demoTime.DeltaTime;
      m_animationRecords.Anim1.AnimBox1 += static_cast<double>(m_animationRecords.Anim1.SpeedPerSecond) * demoTime.DeltaTime;
      m_animationRecords.Anim2.AnimBox1 += static_cast<double>(m_animationRecords.Anim2.SpeedPerSecond) * demoTime.DeltaTime;
    }
  }

  void JankDetector::OnUpdate(const DemoTime& demoTime)
  {
    // Handle animation
    if (m_animationRecords.IsEnabled)
    {
      m_animationRecords.Anim0.AnimCamera += m_animationRecords.Anim0.FixedSpeedPerFrame;
      m_animationRecords.Anim0.AnimBox2 += static_cast<double>(m_animationRecords.Anim0.SpeedPerSecond) * demoTime.DeltaTime;

      m_animationRecords.Anim1.AnimBox0 += m_animationRecords.Anim1.FixedSpeedPerFrame;
      m_animationRecords.Anim1.AnimBox2 += static_cast<double>(m_animationRecords.Anim1.SpeedPerSecond) * demoTime.DeltaTime;

      m_animationRecords.Anim2.AnimBox0 += m_animationRecords.Anim2.FixedSpeedPerFrame;
      m_animationRecords.Anim2.AnimBox2 += static_cast<double>(m_animationRecords.Anim2.SpeedPerSecond) * demoTime.DeltaTime;
    }
  }

  void JankDetector::OnDraw(const DemoTime& demoTime)
  {
    // Handle animation
    if (m_animationRecords.IsEnabled)
    {
      ++m_animationRecords.FrameCount;

      m_animationRecords.Anim0.AnimBox3 += static_cast<double>(m_animationRecords.Anim0.SpeedPerSecond) * demoTime.DeltaTime;
      m_animationRecords.Anim1.AnimBox3 += static_cast<double>(m_animationRecords.Anim1.SpeedPerSecond) * demoTime.DeltaTime;
      m_animationRecords.Anim2.AnimBox3 += static_cast<double>(m_animationRecords.Anim2.SpeedPerSecond) * demoTime.DeltaTime;

      m_animationRecords.Anim0.CameraPosition = static_cast<int32_t>(std::round(m_animationRecords.Anim0.AnimCamera));
      m_animationRecords.Anim0.Box0Position = m_animationRecords.Anim0.CameraPosition;
      m_animationRecords.Anim0.Box1Position = static_cast<int32_t>(std::round(m_animationRecords.Anim0.AnimBox1));
      m_animationRecords.Anim0.Box2Position = static_cast<int32_t>(std::round(m_animationRecords.Anim0.AnimBox2));
      m_animationRecords.Anim0.Box3Position = static_cast<int32_t>(std::round(m_animationRecords.Anim0.AnimBox3));

      m_animationRecords.Anim1.Box0Position = static_cast<int32_t>(std::round(m_animationRecords.Anim1.AnimBox0));
      m_animationRecords.Anim1.Box1Position = static_cast<int32_t>(std::round(m_animationRecords.Anim1.AnimBox1));
      m_animationRecords.Anim1.Box2Position = static_cast<int32_t>(std::round(m_animationRecords.Anim1.AnimBox2));
      m_animationRecords.Anim1.Box3Position = static_cast<int32_t>(std::round(m_animationRecords.Anim1.AnimBox3));

      m_timing.Add(TimingRecords(m_animationRecords.Anim1.Box0Position, m_animationRecords.Anim1.Box1Position, m_animationRecords.Anim1.Box2Position,
                                 m_animationRecords.Anim1.Box3Position));

      m_animationRecords.Anim2.Box0Position = static_cast<int32_t>(std::round(m_animationRecords.Anim2.AnimBox0));
      m_animationRecords.Anim2.Box1Position = static_cast<int32_t>(std::round(m_animationRecords.Anim2.AnimBox1));
      m_animationRecords.Anim2.Box2Position = static_cast<int32_t>(std::round(m_animationRecords.Anim2.AnimBox2));
      m_animationRecords.Anim2.Box3Position = static_cast<int32_t>(std::round(m_animationRecords.Anim2.AnimBox3));
    }
  }

  ReadOnlySpan<JankDetector::TimingRecords> JankDetector::Record1AsTimingSpan() const
  {
    return ReadOnlySpanUtil::AsSpan(m_timing.Entries, 0, m_timing.ValidEntries);
  }
}
