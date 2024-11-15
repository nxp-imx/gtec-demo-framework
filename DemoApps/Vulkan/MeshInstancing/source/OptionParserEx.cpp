/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/NumericCast.hpp>
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
        InstanceCount = DEMO_APP_OPTION_BASE,
        Preset,
      };
    };

    namespace
    {
      constexpr uint32_t InstanceCountUltra = 4096;    // Original sample was 2048
      constexpr uint32_t InstanceCountHigh = 2048;
      constexpr uint32_t InstanceCountMedium = 1024;
      constexpr uint32_t InstanceCountLow = 512;
    }
  }

  OptionParserEx::OptionParserEx()
    : m_instanceCount(InstanceCountMedium)
  {
  }

  void OptionParserEx::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    ADemoOptionParser::OnArgumentSetup(rOptions);

    rOptions.emplace_back("InstanceCount", OptionArgument::OptionRequired, CommandId::InstanceCount, "Set the instance count directly.");
    rOptions.emplace_back("Preset", OptionArgument::OptionRequired, CommandId::Preset, "Set the instance count preset (low,medium,high,ultra)");
  }


  OptionParseResult OptionParserEx::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    switch (cmdId)
    {
    case CommandId::InstanceCount:
      StringParseUtil::Parse(m_instanceCount, strOptArg);
      return OptionParseResult::Parsed;
    case CommandId::Preset:
      {
        if (strOptArg == nullptr)
        {
          return OptionParseResult::Failed;
        }
        if (strOptArg == "low")
        {
          m_instanceCount = InstanceCountLow;
        }
        else if (strOptArg == "medium")
        {
          m_instanceCount = InstanceCountMedium;
        }
        else if (strOptArg == "high")
        {
          m_instanceCount = InstanceCountHigh;
        }
        else if (strOptArg == "ultra")
        {
          m_instanceCount = InstanceCountUltra;
        }
        else
        {
          return OptionParseResult::Failed;
        }
        return OptionParseResult::Parsed;
      }
    default:
      return ADemoOptionParser::OnParse(cmdId, strOptArg);
    }
  }


  bool OptionParserEx::OnParsingComplete()
  {
    if (m_instanceCount < 1)
    {
      const int32_t newCount = 16;
      FSLLOG3_WARNING("InstanceCount was forced to {} instead of the invalid value: {}", newCount, m_instanceCount);
      m_instanceCount = newCount;
    }
    if ((m_instanceCount % 16) != 0)
    {
      const int32_t newCount = ((NumericCast<int32_t>(m_instanceCount) / 16) + 1) * 16;
      FSLLOG3_WARNING("InstanceCount was forced to {} instead of: {}", newCount, m_instanceCount);
      m_instanceCount = newCount;
    }

    // If you return false, the app exits.
    return ADemoOptionParser::OnParsingComplete();
  }
}
