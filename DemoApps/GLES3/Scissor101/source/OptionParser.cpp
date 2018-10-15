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
        NoScissorX = DEMO_APP_OPTION_BASE,
        NoScissorY,
        InvalidWidth,
        InvalidHeight
      };
    };
  }

  OptionParser::OptionParser()
    : ScissorX(true)
    , ScissorY(true)
    , ForceInvalidWidth(false)
    , ForceInvalidHeight(false)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("x", "DisableX", OptionArgument::OptionNone, CommandId::NoScissorX, "Disable scissoring of X");
    rOptions.emplace_back("y", "DisableY", OptionArgument::OptionNone, CommandId::NoScissorY, "Disable scissoring of Y");
    rOptions.emplace_back("ForceInvalidWidth", OptionArgument::OptionNone, CommandId::InvalidWidth, "Force a invalid width");
    rOptions.emplace_back("ForceInvalidHeight", OptionArgument::OptionNone, CommandId::InvalidHeight, "Force a invalid height");
  }


  OptionParseResult::Enum OptionParser::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    switch (cmdId)
    {
    case CommandId::NoScissorX:
      ScissorX = false;
      return OptionParseResult::Parsed;
    case CommandId::NoScissorY:
      ScissorY = false;
      return OptionParseResult::Parsed;
    case CommandId::InvalidWidth:
      ForceInvalidWidth = true;
      FSLLOG_WARNING("Forcing a invalid glScissor width, this might crash the program");
      return OptionParseResult::Parsed;
    case CommandId::InvalidHeight:
      ForceInvalidHeight = true;
      FSLLOG_WARNING("Forcing a invalid glScissor height, this might crash the program");
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
