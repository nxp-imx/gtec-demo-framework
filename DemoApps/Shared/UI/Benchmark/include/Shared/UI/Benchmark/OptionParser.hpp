#ifndef SHARED_UI_BENCHMARK_OPTIONPARSER_HPP
#define SHARED_UI_BENCHMARK_OPTIONPARSER_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/IO/Path.hpp>
#include <FslDemoApp/Base/ADemoOptionParser.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkScene.hpp>
#include <Shared/UI/Benchmark/SceneId.hpp>
#include <optional>

namespace Fsl
{
  class OptionParser : public ADemoOptionParser
  {
    std::optional<bool> m_disableChart;
    std::optional<bool> m_showIdle;
    std::optional<IO::Path> m_compareReportFile;
    std::optional<IO::Path> m_viewReportFile;
    SceneId m_sceneId{SceneId::Playground};
    AppBenchmarkScene m_benchmarkScene{AppBenchmarkScene::Scene0};
    bool m_runDefaultBench{false};

  public:
    OptionParser() = default;
    ~OptionParser() override = default;

    std::optional<bool> TryGetChartDisabled() const
    {
      return m_disableChart;
    }

    std::optional<bool> TryGetShowIdleEnabled() const
    {
      return m_showIdle;
    }

    SceneId GetSceneId() const
    {
      if (m_runDefaultBench)
      {
        return SceneId::Benchmark;
      }
      return !m_viewReportFile.has_value() ? m_sceneId : SceneId::Result;
    }

    AppBenchmarkScene GetBenchmarkScene() const
    {
      if (m_runDefaultBench)
      {
        return AppBenchmarkScene::Scene0;
      }
      return m_benchmarkScene;
    }

    bool GetRunDefaultBench() const
    {
      return m_runDefaultBench;
    }

    std::optional<IO::Path> TryGetCompareReportFile() const
    {
      return m_compareReportFile;
    }

    std::optional<IO::Path> TryGetViewReportFile() const
    {
      return m_viewReportFile;
    }

  protected:
    void OnArgumentSetup(std::deque<Option>& rOptions) override;
    OptionParseResult OnParse(const int32_t cmdId, const StringViewLite& strOptArg) override;
    bool OnParsingComplete() override;
  };
}

#endif
