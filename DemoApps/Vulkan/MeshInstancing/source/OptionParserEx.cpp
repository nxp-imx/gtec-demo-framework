/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include "OptionParserEx.hpp"
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
        InstanceCount = DEMO_APP_OPTION_BASE,
        Preset,
      };
    };

    namespace
    {
      const uint32_t INSTANCE_COUNT_ULTRA = 4096;    // Original sample was 2048
      const uint32_t INSTANCE_COUNT_HIGH = 2048;
      const uint32_t INSTANCE_COUNT_MEDIUM = 1024;
      const uint32_t INSTANCE_COUNT_LOW = 512;
    }
  }

  OptionParserEx::OptionParserEx()
    : m_instanceCount(INSTANCE_COUNT_MEDIUM)
  {
  }

  void OptionParserEx::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    OptionParser::OnArgumentSetup(rOptions);

    rOptions.emplace_back("InstanceCount", OptionArgument::OptionRequired, CommandId::InstanceCount, "Set the instance count directly.");
    rOptions.emplace_back("Preset", OptionArgument::OptionRequired, CommandId::Preset, "Set the instance count preset (low,medium,high,ultra)");
  }


  OptionParseResult::Enum OptionParserEx::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    switch (cmdId)
    {
    case CommandId::InstanceCount:
      StringParseUtil::Parse(m_instanceCount, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::Preset:
    {
      if (pszOptArg == nullptr)
      {
        return OptionParseResult::Failed;
      }
      std::string input(pszOptArg);
      if (input == "low")
      {
        m_instanceCount = INSTANCE_COUNT_LOW;
      }
      else if (input == "medium")
      {
        m_instanceCount = INSTANCE_COUNT_MEDIUM;
      }
      else if (input == "high")
      {
        m_instanceCount = INSTANCE_COUNT_HIGH;
      }
      else if (input == "ultra")
      {
        m_instanceCount = INSTANCE_COUNT_ULTRA;
      }
      else
      {
        return OptionParseResult::Failed;
      }
      return OptionParseResult::Parsed;
    }
    default:
      return OptionParser::OnParse(cmdId, pszOptArg);
    }
  }


  bool OptionParserEx::OnParsingComplete()
  {
    if (m_instanceCount < 1)
    {
      const int32_t newCount = 16;
      FSLLOG_WARNING("InstanceCount was forced to " << newCount << " instead of the invalid value: " << m_instanceCount);
      m_instanceCount = newCount;
    }
    if ((m_instanceCount % 16) != 0)
    {
      const int32_t newCount = ((m_instanceCount / 16) + 1) * 16;
      FSLLOG_WARNING("InstanceCount was forced to " << newCount << " instead of: " << m_instanceCount);
      m_instanceCount = newCount;
    }

    // If you return false, the app exits.
    return OptionParser::OnParsingComplete();
  }
}
