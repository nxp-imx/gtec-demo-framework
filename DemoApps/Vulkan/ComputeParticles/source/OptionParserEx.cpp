/****************************************************************************************************************************************************
 * Copyright 2017, 2022, 2024 NXP
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
        ParticleCount = DEMO_APP_OPTION_BASE,
        Preset,
      };
    };

    namespace
    {
      constexpr int32_t ParticleCountUltra = 16 * 16384;
      constexpr int32_t ParticleCountHigher = 16 * 8192;
      constexpr int32_t ParticleCountHigh = 16 * 4096;
      constexpr int32_t ParticleCountMedium = 16 * 2048;
      constexpr int32_t ParticleCountLow = 16 * 1024;
    }
  }

  OptionParserEx::OptionParserEx()
    : m_particleCount(ParticleCountLow)
  {
  }

  void OptionParserEx::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    ADemoOptionParser::OnArgumentSetup(rOptions);

    rOptions.emplace_back("ParticleCount", OptionArgument::OptionRequired, CommandId::ParticleCount, "Set the particle count directly.");
    rOptions.emplace_back("Preset", OptionArgument::OptionRequired, CommandId::Preset,
                          "Set the particle count preset (low,medium,high,higher,ultra)");
  }


  OptionParseResult OptionParserEx::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    switch (cmdId)
    {
    case CommandId::ParticleCount:
      StringParseUtil::Parse(m_particleCount, strOptArg);
      return OptionParseResult::Parsed;
    case CommandId::Preset:
      {
        if (strOptArg == nullptr)
        {
          return OptionParseResult::Failed;
        }
        if (strOptArg == "low")
        {
          m_particleCount = ParticleCountLow;
        }
        else if (strOptArg == "medium")
        {
          m_particleCount = ParticleCountMedium;
        }
        else if (strOptArg == "high")
        {
          m_particleCount = ParticleCountHigh;
        }
        else if (strOptArg == "higher")
        {
          m_particleCount = ParticleCountHigher;
        }
        else if (strOptArg == "ultra")
        {
          m_particleCount = ParticleCountUltra;
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
    if (m_particleCount < 1)
    {
      const int32_t newParticleCount = 16;
      FSLLOG3_WARNING("ParticleCount was forced to {} instead of the invalid value: {}", newParticleCount, m_particleCount);
      m_particleCount = newParticleCount;
    }
    if ((m_particleCount % 16) != 0)
    {
      const int32_t newParticleCount = ((m_particleCount / 16) + 1) * 16;
      FSLLOG3_WARNING("ParticleCount was forced to {} instead of: {}", newParticleCount, m_particleCount);
      m_particleCount = newParticleCount;
    }

    // If you return false, the app exits.
    return ADemoOptionParser::OnParsingComplete();
  }
}
