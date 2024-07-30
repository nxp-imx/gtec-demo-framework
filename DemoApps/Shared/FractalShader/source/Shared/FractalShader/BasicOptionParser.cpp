/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <Shared/FractalShader/BasicOptionParser.hpp>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        Quality = DEMO_APP_OPTION_BASE,
        RenderMode,
        Iterations,
        ForceUnroll,
        Show
      };
    };
  }

  BasicOptionParser::BasicOptionParser()

  {
    m_config.SetQuality(Quality::Medium);
  }


  BasicOptionParser::~BasicOptionParser() = default;


  void BasicOptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("q", "Quality", OptionArgument::OptionRequired, CommandId::Quality,
                          "Select the rendering quality (low,medium,high,vhigh).");
    rOptions.emplace_back("r", "RenderMode", OptionArgument::OptionRequired, CommandId::RenderMode,
                          "Select the render mode (0=gray, 1=col, 2=smooth).");
    rOptions.emplace_back("i", "Iterations", OptionArgument::OptionRequired, CommandId::Iterations, "The number of iterations to perform (>=1).");
    rOptions.emplace_back("f", "ForceUnroll", OptionArgument::OptionNone, CommandId::ForceUnroll, "Force unroll the shader loops.");
    rOptions.emplace_back("s", "Show", OptionArgument::OptionNone, CommandId::Show, "Show coordinate in julia animation.");
  }


  OptionParseResult BasicOptionParser::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    int intValue = 0;

    switch (cmdId)
    {
    case CommandId::Quality:
      if (strOptArg == "low")
      {
        m_config.SetQuality(Quality::Low);
      }
      else if (strOptArg == "medium")
      {
        m_config.SetQuality(Quality::Medium);
      }
      else if (strOptArg == "high")
      {
        m_config.SetQuality(Quality::High);
      }
      else if (strOptArg == "vhigh")
      {
        m_config.SetQuality(Quality::VeryHigh);
      }
      else
      {
        return OptionParseResult::Failed;
      }
      return OptionParseResult::Parsed;
    case CommandId::RenderMode:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_modified.Flag(ModifiedFlags::RenderMode);
      m_config.SetRenderMode(static_cast<RenderMode>(intValue));
      return OptionParseResult::Parsed;
    case CommandId::Iterations:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_modified.Flag(ModifiedFlags::Iterations);
      m_config.SetIterations(intValue);
      return OptionParseResult::Parsed;
    case CommandId::ForceUnroll:
      m_modified.Flag(ModifiedFlags::ForceUnroll);
      m_config.SetForceUnroll(true);
      return OptionParseResult::Parsed;
    case CommandId::Show:
      m_modified.Flag(ModifiedFlags::Show);
      m_config.SetShow(true);
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool BasicOptionParser::OnParsingComplete()
  {
    switch (m_config.TheQuality)
    {
    case Quality::Low:
      if (!m_modified.IsFlagged(ModifiedFlags::RenderMode))
      {
        m_config.SetRenderMode(RenderMode::Gray);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::Iterations))
      {
        m_config.SetIterationsJ(10);
        m_config.SetIterationsM(20);
      }
      break;
    case Quality::Medium:
      if (!m_modified.IsFlagged(ModifiedFlags::RenderMode))
      {
        m_config.SetRenderMode(RenderMode::Col);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::Iterations))
      {
        m_config.SetIterationsJ(DefaultValues::JuliaDefaultIterations);
        m_config.SetIterationsM(DefaultValues::MandelbrotDefaultIterations);
      }
      break;
    case Quality::High:
      if (!m_modified.IsFlagged(ModifiedFlags::RenderMode))
      {
        m_config.SetRenderMode(RenderMode::Smooth);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::Iterations))
      {
        m_config.SetIterationsJ(30);
        m_config.SetIterationsM(50);
      }
      break;
    case Quality::VeryHigh:
      if (!m_modified.IsFlagged(ModifiedFlags::RenderMode))
      {
        m_config.SetRenderMode(RenderMode::Smooth);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::Iterations))
      {
        m_config.SetIterationsJ(60);
        m_config.SetIterationsM(100);
      }
      break;
    default:
      break;
    }
    // If you return false, the app exits.
    return true;
  }
}
