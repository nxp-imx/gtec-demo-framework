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

#include <FslBase/Getopt/Option.hpp>
#include <FslBase/Exceptions.hpp>
#include <cstring>
#include <utility>

namespace Fsl
{
  Option::Option(const std::string& smartName, const OptionArgument::Enum hasArg, const int32_t cmdId, std::string description)
    : HasArg(hasArg)
    , CmdId(cmdId)
    , Description(std::move(description))
  {
    if (smartName.empty())
    {
      throw std::invalid_argument("smartName can not be empty");
    }
    if (smartName.size() == 1)
    {
      ShortName = smartName;
    }
    else
    {
      Name = smartName;
    }
  }


  Option::Option(const std::string& smartName, const OptionArgument::Enum hasArg, const int32_t cmdId, std::string description,
                 const OptionGroup::Enum group)
    : HasArg(hasArg)
    , CmdId(cmdId)
    , Description(std::move(description))
    , Group(group)
  {
    if (smartName.empty())
    {
      throw std::invalid_argument("smartName can not be empty");
    }
    if (smartName.size() == 1)
    {
      ShortName = smartName;
    }
    else
    {
      Name = smartName;
    }
  }


  Option::Option(const std::string& shortName, const std::string& name, const OptionArgument::Enum hasArg, const int32_t cmdId,
                 std::string description)
    : ShortName(shortName)
    , Name(name)
    , HasArg(hasArg)
    , CmdId(cmdId)
    , Description(std::move(description))
  {
    if (shortName.size() != 1)
    {
      throw std::invalid_argument("A short name is expected to have a length of 1");
    }

    if (name.size() <= 1)
    {
      throw std::invalid_argument("A name is expected to have a length that is greater than one");
    }
  }


  Option::Option(const std::string& shortName, const std::string& name, const OptionArgument::Enum hasArg, const int32_t cmdId,
                 std::string description, const OptionGroup::Enum group)
    : ShortName(shortName)
    , Name(name)
    , HasArg(hasArg)
    , CmdId(cmdId)
    , Description(std::move(description))
    , Group(group)
  {
    if (shortName.size() != 1)
    {
      throw std::invalid_argument("A short name is expected to have a length of 1");
    }

    if (name.size() <= 1)
    {
      throw std::invalid_argument("A name is expected to have a length that is greater than one");
    }
  }


  PositionalOption::PositionalOption(const std::string& name, const OptionArgument::Enum hasArg, const int32_t cmdId, const std::string& description)
    : Option(name, hasArg, cmdId, description)
  {
    IsPositional = true;
  }


  PositionalOption::PositionalOption(const std::string& name, const OptionArgument::Enum hasArg, const int32_t cmdId, const std::string& description,
                                     const OptionGroup::Enum group)
    : Option(name, hasArg, cmdId, description, group)
  {
    IsPositional = true;
  }
}
