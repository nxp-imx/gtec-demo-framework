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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cmath>
#include "OptionParser.hpp"

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        Scene = DEMO_APP_OPTION_BASE,
        Model,
        Texture
      };
    };
  }

  OptionParser::OptionParser()
    : m_scene(DemoScene::Default)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("s", "Scene", OptionArgument::OptionRequired, CommandId::Scene, "Select the scene to run (0 to 0 or basic)");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    int intValue;

    switch (cmdId)
    {
    case CommandId::Scene:
      if (std::string("basic") == pszOptArg)
      {
        m_scene = DemoScene::Basic;
      }
      else
      {
        if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        {
          return OptionParseResult::Failed;
        }
        m_scene = static_cast<DemoScene>(intValue);
      }
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool OptionParser::OnParsingComplete()
  {
    return true;
  }
}
