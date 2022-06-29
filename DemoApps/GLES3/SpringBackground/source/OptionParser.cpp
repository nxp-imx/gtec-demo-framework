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

#include "OptionParser.hpp"
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
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
        RenderId = DEMO_APP_OPTION_BASE,
        Grid,
        Bloom,
        ShowBuffers,
      };
    };
  }

  OptionParser::OptionParser()
    : m_renderId(-1)
    , m_bloomEnabled(false)
    , m_showBuffers(false)
    , m_gridResolution(60, 30)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("r", "Render", OptionArgument::OptionRequired, CommandId::RenderId, "Select the render type to use for the grid");
    rOptions.emplace_back("g", "Grid", OptionArgument::OptionRequired, CommandId::Grid, "Grid resolution [x,y]");
    rOptions.emplace_back("b", "Bloom", OptionArgument::OptionRequired, CommandId::Bloom, "Bloom on/off");
    rOptions.emplace_back("ShowBuffers", OptionArgument::OptionRequired, CommandId::ShowBuffers, "Show buffers on/off");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    int32_t intValue = 0;
    bool boolValue = false;

    switch (cmdId)
    {
    case CommandId::RenderId:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_renderId = intValue;
      return OptionParseResult::Parsed;
    case CommandId::Grid:
      if (StringParseUtil::Parse(m_gridResolution, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      return OptionParseResult::Parsed;
    case CommandId::Bloom:
      if (StringParseUtil::Parse(boolValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_bloomEnabled = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::ShowBuffers:
      if (StringParseUtil::Parse(boolValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_showBuffers = boolValue;
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
