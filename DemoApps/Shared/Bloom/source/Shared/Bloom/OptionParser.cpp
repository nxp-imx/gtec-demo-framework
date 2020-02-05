/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <Shared/Bloom/OptionParser.hpp>
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
        Rotate,
        BlurPass,
        BrightPass,
        FinalScene,
        FinalBloom,
        ShowBuffers,
        ScaleInputSequentially
      };
    };
  }

  OptionParser::OptionParser()
    : m_sceneId(0)
    , m_rotateEnabled(true)
    , m_blurPassEnabled(true)
    , m_brightPassEnabled(true)
    , m_renderFinalScene(true)
    , m_renderFinalBloom(true)
    , m_showBuffers(false)
    , m_scaleInputSequentially(true)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("s", "Scene", OptionArgument::OptionRequired, CommandId::Scene, "Select the scene number");
    rOptions.emplace_back("Rotate", OptionArgument::OptionRequired, CommandId::Rotate, "Rotate on/off");
    rOptions.emplace_back("BlurPass", OptionArgument::OptionRequired, CommandId::BlurPass, "Blue pass on/off");
    rOptions.emplace_back("BrightPass", OptionArgument::OptionRequired, CommandId::BrightPass, "Bright pass on/off");
    rOptions.emplace_back("FinalScene", OptionArgument::OptionRequired, CommandId::FinalScene, "Render final scene on/off");
    rOptions.emplace_back("FinalBloom", OptionArgument::OptionRequired, CommandId::FinalBloom, "Render final bloom on/off");
    rOptions.emplace_back("ShowBuffers", OptionArgument::OptionRequired, CommandId::ShowBuffers, "Show buffers on/off");
    rOptions.emplace_back("ScaleSeq", OptionArgument::OptionRequired, CommandId::ScaleInputSequentially, "Scale input sequentially on/off");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    int32_t intValue;
    bool boolValue;

    switch (cmdId)
    {
    case CommandId::Scene:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_sceneId = intValue;
      return OptionParseResult::Parsed;
    case CommandId::Rotate:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_rotateEnabled = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::BlurPass:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_blurPassEnabled = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::BrightPass:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_brightPassEnabled = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::FinalScene:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_renderFinalScene = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::FinalBloom:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_renderFinalBloom = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::ShowBuffers:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_showBuffers = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::ScaleInputSequentially:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_scaleInputSequentially = boolValue;
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
