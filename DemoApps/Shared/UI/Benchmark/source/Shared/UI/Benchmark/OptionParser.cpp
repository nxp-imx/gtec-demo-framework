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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <Shared/UI/Benchmark/OptionParser.hpp>
#include <fmt/format.h>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        DisableChart = DEMO_APP_OPTION_BASE,
        ShowSystemIdle,
        Scene,
        RunDefaultBench,
        Compare,
        View,
      };
    };
  }

  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("NoChart", OptionArgument::OptionNone, CommandId::DisableChart, "Disable the chart UI");
    rOptions.emplace_back("ShowSystemIdle", OptionArgument::OptionNone, CommandId::ShowSystemIdle, "Indicate if the system UI is idle or not");
    rOptions.emplace_back("Scene", OptionArgument::OptionRequired, CommandId::Scene, "Select the scene to start: bench, play, record, result");
    rOptions.emplace_back("RunDefaultBench", OptionArgument::OptionNone, CommandId::RunDefaultBench,
                          "Run the default bench, this forces the use of the default input recording and forces the scene to bench");
    rOptions.emplace_back("Compare", OptionArgument::OptionRequired, CommandId::Compare, "Always compare a benchmark to the supplied result file");
    rOptions.emplace_back("View", OptionArgument::OptionRequired, CommandId::View,
                          "View the file as the current result, this will launch the app into the result screen.");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    switch (cmdId)
    {
    case CommandId::DisableChart:
      m_disableChart = true;
      return OptionParseResult::Parsed;
    case CommandId::ShowSystemIdle:
      m_showIdle = true;
      return OptionParseResult::Parsed;
    case CommandId::Scene:
      if (strOptArg == "bench")
      {
        m_sceneId = SceneId::Benchmark;
        return OptionParseResult::Parsed;
      }
      else if (strOptArg == "play")
      {
        m_sceneId = SceneId::Playground;
        return OptionParseResult::Parsed;
      }
      else if (strOptArg == "record")
      {
        m_sceneId = SceneId::Record;
        return OptionParseResult::Parsed;
      }
      else if (strOptArg == "result")
      {
        m_sceneId = SceneId::Result;
        return OptionParseResult::Parsed;
      }
      return OptionParseResult::Failed;
    case CommandId::RunDefaultBench:
      m_runDefaultBench = true;
      FSLLOG3_INFO("Running default benchmark as requested");
      return OptionParseResult::Parsed;
    case CommandId::Compare:
      if (!IO::File::Exists(strOptArg))
      {
        FSLLOG3_INFO("File not found: '{}'", strOptArg);
        return OptionParseResult::Failed;
      }
      m_compareReportFile = strOptArg;
      return OptionParseResult::Parsed;
    case CommandId::View:
      if (!IO::File::Exists(strOptArg))
      {
        FSLLOG3_INFO("File not found: '{}'", strOptArg);
        return OptionParseResult::Failed;
      }
      m_viewReportFile = strOptArg;
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool OptionParser::OnParsingComplete()
  {
    // If you return false, the app exits.
    return true;
  }
}
