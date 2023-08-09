/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <Shared/ModelInstancing/OptionParser.hpp>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        MaxInstancesX = DEMO_APP_OPTION_BASE,
        MaxInstancesY,
        MaxInstancesZ
      };
    };
  }

  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("x", "MaxInstancesX", OptionArgument::OptionRequired, CommandId::MaxInstancesX,
                          "Select the maximum number of instances to render. WARNING a too large number can crash the app on some drivers");
    rOptions.emplace_back("y", "MaxInstancesY", OptionArgument::OptionRequired, CommandId::MaxInstancesY,
                          "Select the maximum number of instances to render. WARNING a too large number can crash the app on some drivers");
    rOptions.emplace_back("z", "MaxInstancesZ", OptionArgument::OptionRequired, CommandId::MaxInstancesZ,
                          "Select the maximum number of instances to render. WARNING a too large number can crash the app on some drivers");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    uint32_t uintValue = 0;

    switch (cmdId)
    {
    case CommandId::MaxInstancesX:
      if (StringParseUtil::Parse(uintValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_maxInstancesX = std::max(uintValue, 1u);
      return OptionParseResult::Parsed;
    case CommandId::MaxInstancesY:
      if (StringParseUtil::Parse(uintValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_maxInstancesY = std::max(uintValue, 1u);
      return OptionParseResult::Parsed;
    case CommandId::MaxInstancesZ:
      if (StringParseUtil::Parse(uintValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_maxInstancesZ = std::max(uintValue, 1u);
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
