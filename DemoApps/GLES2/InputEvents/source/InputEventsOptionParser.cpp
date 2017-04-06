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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include "InputEventsOptionParser.hpp"

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        TestA = DEMO_APP_OPTION_BASE,
        TestB,
        TestC,
        TestD,
        TestE,
        Fail,
        TestG
      };

    };
  }

  InputEventsOptionParser::InputEventsOptionParser()
    : m_bTestASet(false)
    , m_testBValue(0)
    , m_testCValue()
    , m_bContinueRunning(true)
  {

  }


  InputEventsOptionParser::~InputEventsOptionParser()
  {

  }


  void InputEventsOptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.push_back(Option("a", "testA", OptionArgument::OptionNone, CommandId::TestA, "test setting a flag parameter"));
    rOptions.push_back(Option("b", "testB", OptionArgument::OptionRequired, CommandId::TestB, "test setting a parameter that requires a argument"));
    rOptions.push_back(Option("c", "testC", OptionArgument::OptionRequired, CommandId::TestC, "test setting a parameter that requires a argument"));
    rOptions.push_back(Option("d", "testD", OptionArgument::OptionRequired, CommandId::TestD, "test setting a parameter that requires a argument, but validation always fails"));
    rOptions.push_back(Option("testE", OptionArgument::OptionNone, CommandId::TestE, "Test parameter"));
    rOptions.push_back(Option("f", "fail", OptionArgument::OptionNone, CommandId::Fail, "If this is set the app fails to launch :)"));
    rOptions.push_back(Option("g", OptionArgument::OptionRequired, CommandId::TestD, "test setting a parameter that requires a argument, but validation always fails"));
  }


  OptionParseResult::Enum InputEventsOptionParser::OnParse(const int32_t cmdId, const char*const pszOptArg)
  {
    switch (cmdId)
    {
    case CommandId::TestA:
      m_bTestASet = true;
      return OptionParseResult::Parsed;
    case CommandId::TestB:
      StringParseUtil::Parse(m_testBValue, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::TestC:
      StringParseUtil::Parse(m_testCValue, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::TestD:
      FSLLOG("This option always fails parsing :)")
      return OptionParseResult::Failed;
    case CommandId::TestE:
      return OptionParseResult::Parsed;
    case CommandId::Fail:
      m_bContinueRunning = false;
      return OptionParseResult::Parsed;
    case CommandId::TestG:
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool InputEventsOptionParser::OnParsingComplete()
  {
    // This allows you to do some precessing or force a exit after all the parsing has been done.

    if (!m_bContinueRunning)
    {
      FSLLOG("Forced exit")
    }

    // If you return false, the app exits.
    return m_bContinueRunning;
  }

}
