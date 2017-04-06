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

#include <FslDemoPlatform/Service/MMDCStats/MMDCStatsServiceOptionParser.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <cassert>
#include <cstring>
#include <sstream>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        MMDC,
        MMDCUsage,
        MMDCDuration,
        MMDCLog,
      };
    };


    const char* g_pszUsage[] =
    {
      "DEFAULT",
      "CUSTOM",
      "ARM",
      "DSP1",
      "DSP2",
      "GPU2D",
      "GPU2D1",
      "GPU2D2",
      "GPU3D",
      "GPUVG",
      "VPU",
      "M4",
      "PXP",
      "USB",
      "SUM"
    };

    std::string BuildUsageString()
    {
      std::stringstream str;
      str << g_pszUsage[0];
      for (size_t i = 1; i < (sizeof(g_pszUsage) / sizeof(char*)); ++i)
      {
        str << "|";
        str << g_pszUsage[i];
      }
      return str.str();
    }


    bool IsValidUsageString(const char*const psz)
    {
      for (size_t i = 0; i < (sizeof(g_pszUsage) / sizeof(char*)); ++i)
      {
        if (strcmp(g_pszUsage[i], psz) == 0)
          return true;
      }
      return false;
    }

  }


  MMDCStatsServiceOptionParser::MMDCStatsServiceOptionParser()
    : m_enabled(false)
    , m_logEnabled(false)
    , m_duration(0)
    , m_usage("DEFAULT")
  {
  }

  void MMDCStatsServiceOptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    std::string strDescUsage = std::string("Select what to profile (") + BuildUsageString() + ") DEFAULT will make its choice based on the sample type. CUSTOM is like running mmdc2 without a parameter.";

    rOptions.push_back(Option("mmdc", OptionArgument::OptionNone, CommandId::MMDC, "Enable basic MMDC profiling using the most appropriate profiler for the app type (if mmdcUsage is specified it will determine what we profile)"));
    rOptions.push_back(Option("mmdcUsage", OptionArgument::OptionRequired, CommandId::MMDCUsage, strDescUsage.c_str()));
    rOptions.push_back(Option("mmdcDuration", OptionArgument::OptionRequired, CommandId::MMDCDuration, "Select the profile duration in milliseconds (can not be lower than the rendering time of one frame)"));
    rOptions.push_back(Option("mmdcLog", OptionArgument::OptionNone, CommandId::MMDCLog, "Print the mmdc profiling stats to the log"));
  }


  OptionParseResult::Enum MMDCStatsServiceOptionParser::OnParse(const int32_t cmdId, const char*const pszOptArg)
  {
    switch (cmdId)
    {
    case CommandId::MMDC:
      m_enabled = true;
      return OptionParseResult::Parsed;
    case CommandId::MMDCUsage:
      m_enabled = true;
      if (!IsValidUsageString(pszOptArg))
        throw NotSupportedException(std::string("Unknown mmdc usage string: ") + pszOptArg);
      m_usage = pszOptArg;
      return OptionParseResult::Parsed;
    case CommandId::MMDCDuration:
      m_enabled = true;
      StringParseUtil::Parse(m_duration, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::MMDCLog:
      m_enabled = true;
      m_logEnabled = true;
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
    return OptionParseResult::NotHandled;
  }


  bool MMDCStatsServiceOptionParser::OnParsingComplete()
  {
    return true;
  }
}
