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

#include "SimpleBench.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <VG/openvg.h>
#include <array>
#include <iostream>
#include "PointBench.hpp"

namespace Fsl
{
  namespace
  {
    const int32_t NUM_DRAWS_PER_BENCH = 10;
  }


  SimpleBench::SimpleBench(const DemoAppConfig& config)
    : DemoAppVG(config)
    , m_benchIndex(0)
    , m_taskTime(0)
    , m_benchDrawCount(0)
  {
    int pointCount = 200;
    for (int i = 1; i <= 5; ++i)
    {
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Clear)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Line)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::NewPath)));
      // m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i*pointCount, PointBench::Mode::ModifyPath)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Bitmap)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::BitmapFont)));
    }

    pointCount = 1000;
    for (int i = 1; i <= 5; ++i)
    {
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Clear)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Line)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::NewPath)));
      // m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i*pointCount, PointBench::Mode::ModifyPath)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Bitmap)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::BitmapFont)));
    }

    pointCount = 10000;
    for (int i = 1; i <= 5; ++i)
    {
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Clear)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Line)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::NewPath)));
      // m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i*pointCount, PointBench::Mode::ModifyPath)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::Bitmap)));
      m_benchmarks.push_back(std::shared_ptr<IBench>(new PointBench(config, i * pointCount, PointBench::Mode::BitmapFont)));
    }

    NextBenchmark();
  }


  SimpleBench::~SimpleBench() = default;


  void SimpleBench::Update(const DemoTime& demoTime)
  {
    if (!m_current)
    {
      return;
    }

    m_current->Update(demoTime);
  }


  void SimpleBench::Draw(const FrameInfo& frameInfo)
  {
    constexpr std::array<VGfloat, 4> color = {0.0f, 0.0f, 0.0f, 0.0f};
    vgSetfv(VG_CLEAR_COLOR, UncheckedNumericCast<VGint>(color.size()), color.data());

    const PxSize2D currentSizePx = GetWindowSizePx();
    vgClear(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());

    if (!m_current)
    {
      return;
    }

    const auto beginTime = m_timer.GetTimestamp();
    m_current->Draw(currentSizePx);
    const auto endTime = m_timer.GetTimestamp();
    m_taskTime += endTime - beginTime;

    ++m_benchDrawCount;
    if (m_benchDrawCount >= NUM_DRAWS_PER_BENCH)
    {
      FSLLOG3_INFO("Benchmark {} time: {} microseconds", m_current->GetName(), TimeSpanUtil::ToMicrosecondsInt64(m_taskTime));
      NextBenchmark();
    }
  }


  void SimpleBench::NextBenchmark()
  {
    if (m_benchmarks.empty())
    {
      throw UsageErrorException("m_benchmarks.size() can not be zero");
    }

    if (m_benchIndex >= m_benchmarks.size())
    {
      m_benchIndex = 0;
    }

    m_current = m_benchmarks[m_benchIndex];
    m_current->Restart();

    ++m_benchIndex;
    m_taskTime = {};
    m_benchDrawCount = 0;
  }
}
