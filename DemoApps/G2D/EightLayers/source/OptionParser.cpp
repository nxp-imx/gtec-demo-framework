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

#include "OptionParser.hpp"
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log.hpp>
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
        VSync = DEMO_APP_OPTION_BASE,
        Cache,
        High,
        Animate,
        Manual,
        Align,
        Offscreen,
        OffscreenFormat,
        Dummy,
        Test,
        Test2
      };

    };
  }

  OptionParser::OptionParser()
    : m_useForceVSync(false)
    , m_useCache(false)
    , m_use32BPP(false)
    , m_useAnimation(true)
    , m_useManual(false)
    , m_alignFB(true)
    , m_useOffscreen(false)
    , m_useDummyMode(false)
    , m_offscreenFormat(-1)
    , m_test(0)
    , m_test2(0)
  {
  }


  OptionParser::~OptionParser()
  {

  }


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.push_back(Option("s", "VSync", OptionArgument::OptionRequired, CommandId::VSync, "Enable/disable forced vsync (0 to 1)"));
    rOptions.push_back(Option("c", "Cache", OptionArgument::OptionRequired, CommandId::Cache, "Enable/disable the cache flag on surface creation (0 to 1)"));
    rOptions.push_back(Option("t", "Test", OptionArgument::OptionRequired, CommandId::Test, "magic"));
    rOptions.push_back(Option("r", "Test2", OptionArgument::OptionRequired, CommandId::Test2, "magic"));
    rOptions.push_back(Option("d", "Depth", OptionArgument::OptionRequired, CommandId::High, "Enable/disable 32bpp mode (0 to 1)"));
    rOptions.push_back(Option("a", "Anim", OptionArgument::OptionRequired, CommandId::Animate, "Enable/disable animation (0 to 1)"));
    rOptions.push_back(Option("Align", OptionArgument::OptionRequired, CommandId::Align, "Enable/disable framebuffer forced align (0 to 1)"));
    rOptions.push_back(Option("m", "Manual", OptionArgument::OptionRequired, CommandId::Manual, "Enable/disable manual blitting (0 to 1)"));
    rOptions.push_back(Option("o", "Offscreen", OptionArgument::OptionRequired, CommandId::Offscreen, "Enable/disable offscreen blitting (0 to 1)"));
    rOptions.push_back(Option("f", "Format", OptionArgument::OptionRequired, CommandId::OffscreenFormat, "Set the g2d offscreen format if less than zero it uses the fb format"));
    rOptions.push_back(Option("Dummy", OptionArgument::OptionRequired, CommandId::Dummy, "Enable/disable dummy mode (0 to 1)"));
  }


  OptionParseResult::Enum OptionParser::OnParse(const int32_t cmdId, const char*const pszOptArg)
  {
    bool boolValue;
    int intValue;

    switch (cmdId)
    {
    case CommandId::VSync:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_useForceVSync = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Cache:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_useCache = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Animate:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_useAnimation = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Align:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_alignFB = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::High:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_use32BPP = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Manual:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_useManual = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Dummy:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_useDummyMode = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Offscreen:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_useOffscreen = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::OffscreenFormat:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_offscreenFormat = intValue;
      return OptionParseResult::Parsed;
    case CommandId::Test:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_test = intValue;
      return OptionParseResult::Parsed;
    case CommandId::Test2:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_test2 = intValue;
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
