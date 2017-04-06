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

namespace Fsl
{
  Option::Option(const char*const pszSmartName, const OptionArgument::Enum hasArg, const int32_t cmdId, const char*const pszDescription)
    : ShortName(nullptr)
    , Name(nullptr)
    , HasArg(hasArg)
    , CmdId(cmdId)
    , Description(pszDescription)
    , Group(OptionGroup::Default)
    , Type(OptionType::Default)
    , IsPositional(false)
  {
    if (pszSmartName == nullptr)
      throw std::invalid_argument("pszSmartName can not be null");
    const std::size_t len = strlen(pszSmartName);
    if (len == 0)
      throw std::invalid_argument("pszSmartName can not be empty");
    else if (len == 1)
      ShortName = pszSmartName;
    else
      Name = pszSmartName;
  }


  Option::Option(const char*const pszSmartName, const OptionArgument::Enum hasArg, const int32_t cmdId, const char*const pszDescription, const OptionGroup::Enum group)
    : ShortName(nullptr)
    , Name(nullptr)
    , HasArg(hasArg)
    , CmdId(cmdId)
    , Description(pszDescription)
    , Group(group)
    , Type(OptionType::Default)
    , IsPositional(false)
  {
    if (pszSmartName == nullptr)
      throw std::invalid_argument("pszSmartName can not be null");
    const std::size_t len = strlen(pszSmartName);
    if (len == 0)
      throw std::invalid_argument("pszSmartName can not be empty");
    else if (len == 1)
      ShortName = pszSmartName;
    else
      Name = pszSmartName;
  }


  Option::Option(const char*const pszShortName, const char*const pszName, const OptionArgument::Enum hasArg, const int32_t cmdId, const char*const pszDescription)
    : ShortName(pszShortName)
    , Name(pszName)
    , HasArg(hasArg)
    , CmdId(cmdId)
    , Description(pszDescription)
    , Group(OptionGroup::Default)
    , Type(OptionType::Default)
    , IsPositional(false)
  {
    if (pszShortName == nullptr && pszName == nullptr )
      throw std::invalid_argument("pszShortName and pszName can not both be null");

    if (pszShortName != nullptr && strlen(pszShortName) != 1)
      throw std::invalid_argument("A short name is expected to have a length of 1");

    if (pszName != nullptr && strlen(pszName) <= 1)
      throw std::invalid_argument("A name is expected to have a length that is greater than one");
  }


  Option::Option(const char*const pszShortName, const char*const pszName, const OptionArgument::Enum hasArg, const int32_t cmdId, const char*const pszDescription, const OptionGroup::Enum group)
    : ShortName(pszShortName)
    , Name(pszName)
    , HasArg(hasArg)
    , CmdId(cmdId)
    , Description(pszDescription)
    , Group(group)
    , Type(OptionType::Default)
    , IsPositional(false)
  {
    if (pszShortName == nullptr && pszName == nullptr)
      throw std::invalid_argument("pszShortName and pszName can not both be null");

    if (pszShortName != nullptr && strlen(pszShortName) != 1)
      throw std::invalid_argument("A short name is expected to have a length of 1");

    if (pszName != nullptr && strlen(pszName) <= 1)
      throw std::invalid_argument("A name is expected to have a length that is greater than one");
  }


  PositionalOption::PositionalOption(const char*const pszName, const OptionArgument::Enum hasArg, const int32_t cmdId, const char*const pszDescription)
    : Option(pszName, hasArg, cmdId, pszDescription)
  {
    IsPositional = true;
  }


  PositionalOption::PositionalOption(const char*const pszName, const OptionArgument::Enum hasArg, const int32_t cmdId, const char*const pszDescription, const OptionGroup::Enum group)
    : Option(pszName, hasArg, cmdId, pszDescription, group)
  {
    IsPositional = true;
  }

}
