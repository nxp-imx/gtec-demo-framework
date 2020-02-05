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

#include <Shared/FractalShader/OptionParser.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Exceptions.hpp>
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
        Scene = DEMO_APP_OPTION_BASE,
        Quality,
        RenderMode,
        Iterations,
        ForceUnroll,
        Show,
        AnimationMode,
        DemoMode
      };
    };
  }

  OptionParser::OptionParser()
    : m_demoMode(false)
  {
    m_config.SetQuality(Quality::Medium);
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("Scene", OptionArgument::OptionRequired, CommandId::Scene, "Select the scene to run (0 to 1)");
    rOptions.emplace_back("q", "Quality", OptionArgument::OptionRequired, CommandId::Quality, "Select the rendering quality (low,medium,high).");
    rOptions.emplace_back("r", "RenderMode", OptionArgument::OptionRequired, CommandId::RenderMode,
                          "Select the render mode (0=gray, 1=col, 2=smooth, 3=tex).");
    rOptions.emplace_back("i", "Iterations", OptionArgument::OptionRequired, CommandId::Iterations, "The number of iterations to perform (>=1).");
    rOptions.emplace_back("f", "ForceUnroll", OptionArgument::OptionNone, CommandId::ForceUnroll, "Force unroll the shader loops.");
    rOptions.emplace_back("s", "Show", OptionArgument::OptionNone, CommandId::Show, "Show coordinate in julia animation.");
    rOptions.emplace_back("a", "AnimMode", OptionArgument::OptionRequired, CommandId::AnimationMode, "Chose the julia animation mode.");
    rOptions.emplace_back("d", "Demo", OptionArgument::OptionNone, CommandId::DemoMode, "Lock into demo mode (nothing can be overridden).");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    //    bool boolValue;
    int intValue;
    //    float floatValue;

    switch (cmdId)
    {
    case CommandId::Scene:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetScene(intValue);
      return OptionParseResult::Parsed;
    case CommandId::Quality:
      if (strcmp(pszOptArg, "low") == 0)
      {
        m_config.SetQuality(Quality::Low);
      }
      else if (strcmp(pszOptArg, "medium") == 0)
      {
        m_config.SetQuality(Quality::Medium);
      }
      else if (strcmp(pszOptArg, "high") == 0)
      {
        m_config.SetQuality(Quality::High);
      }
      else
      {
        return OptionParseResult::Failed;
      }
      return OptionParseResult::Parsed;
    case CommandId::RenderMode:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_modified.Flag(ModifiedFlags::RenderMode);
      m_config.SetRenderMode(static_cast<RenderMode>(intValue));
      return OptionParseResult::Parsed;
    case CommandId::Iterations:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
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
    case CommandId::AnimationMode:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_modified.Flag(ModifiedFlags::AnimationMode);
      m_config.SetAnimationMode(static_cast<AnimationMode>(intValue));
      return OptionParseResult::Parsed;
    case CommandId::DemoMode:
      m_demoMode = true;
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool OptionParser::OnParsingComplete()
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
        m_config.SetIterations(m_config.Scene == 0 ? 10 : 20);
      }
      break;
    case Quality::Medium:
      if (!m_modified.IsFlagged(ModifiedFlags::RenderMode))
      {
        m_config.SetRenderMode(RenderMode::Col);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::Iterations))
      {
        m_config.SetIterations(m_config.Scene == 0 ? 20 : 35);
      }
      break;
    case Quality::High:
      if (!m_modified.IsFlagged(ModifiedFlags::RenderMode))
      {
        m_config.SetRenderMode(RenderMode::Smooth);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::Iterations))
      {
        m_config.SetIterations(m_config.Scene == 0 ? 30 : 50);
      }
      break;
    default:
      break;
    }

    if (m_demoMode)
    {
      FSLLOG3_INFO("DemoMode enabled, all other arguments ignored!");
      m_config = Config();
      m_config.SetRenderMode(RenderMode::Col);
      m_config.SetIterations(m_config.Scene == 0 ? 30 : 35);
      m_config.SetForceUnroll(true);
      m_config.SetScene(0);
      m_config.SetAnimationMode(AnimationMode::ConstantLoad);
    }

    // If you return false, the app exits.
    return true;
  }
}
