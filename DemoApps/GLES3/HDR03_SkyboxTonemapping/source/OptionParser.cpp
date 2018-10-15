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
#include <FslBase/Log/Log.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Exceptions.hpp>
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
        Mask,
      };
    };
  }

  OptionParser::OptionParser()
    : m_sceneMask(static_cast<uint32_t>(SceneFlags::Split))
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("s", "Scene", OptionArgument::OptionRequired, CommandId::Scene, "Select the scene to run (0 to 5)");
    rOptions.emplace_back("Mask", OptionArgument::OptionRequired, CommandId::Mask, "Select the tonemappers using a mask");
  }


  OptionParseResult::Enum OptionParser::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    uint32_t uintValue;

    switch (cmdId)
    {
    case CommandId::Scene:
      if (StringParseUtil::Parse(uintValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      switch (uintValue)
      {
      case 0:
        SetSceneMask(SceneFlags::Split);
        break;
      case 1:
        SetSceneMask(SceneFlags::Scene1);
        break;
      case 2:
        SetSceneMask(SceneFlags::Scene2);
        break;
      case 3:
        SetSceneMask(SceneFlags::Scene3);
        break;
      case 4:
        SetSceneMask(SceneFlags::Scene4);
        break;
      case 5:
        SetSceneMask(SceneFlags::Scene5);
        break;
      default:
        FSLLOG_ERROR("Scene is out of range: " << uintValue);
        return OptionParseResult::Failed;
      }
      return OptionParseResult::Parsed;
    case CommandId::Mask:
      if (StringParseUtil::Parse(uintValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      SetSceneMask(uintValue);
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool OptionParser::OnParsingComplete()
  {
    return true;
  }


  void OptionParser::SetSceneMask(const SceneFlags flags)
  {
    SetSceneMask(static_cast<uint32_t>(flags));
  }


  void OptionParser::SetSceneMask(const uint32_t flags)
  {
    m_sceneMask = (flags & static_cast<uint32_t>(SceneFlags::Split));
  }
}
