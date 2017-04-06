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

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Getopt/OptionParser.hpp>
#include "OptionParserTCLAP.hpp"

namespace Fsl
{
  namespace
  {
    bool IsUniqueOption(const std::deque<Option>& combinedOptions, const Option& option)
    {
      for (std::size_t i = 0; i < combinedOptions.size(); ++i)
      {
        const Option existingOption = combinedOptions[i];
        if (option.ShortName != nullptr && existingOption.ShortName != nullptr && strcmp(option.ShortName, existingOption.ShortName) == 0)
          return false;
        if (option.Name != nullptr && existingOption.Name != nullptr && strcmp(option.Name, existingOption.Name) == 0)
          return false;
      }
      return true;
    }


    void AppendArguments(std::deque<Option>& rCombinedOptions, const std::deque<Option>& options)
    {
      for (std::size_t i = 0; i < options.size(); ++i)
      {
        const Option option = options[i];

        if (IsUniqueOption(rCombinedOptions, option))
          rCombinedOptions.push_back(option);
        else
        {
          std::stringstream stream("Option already defined: ");
          if (option.ShortName != nullptr)
            stream << "'" << option.ShortName << "'";
          if (option.Name != nullptr)
          {
            if (option.ShortName != nullptr)
              stream << "' ";
            else
              stream << "'";
            stream << option.Name << "'";
          }
          throw UsageErrorException(stream.str());
        }
      }
    }


    void ArgumentSetup(const std::deque<OptionParser::ParserRecord>& inputArgs, std::deque<Option>& rCombinedOptions)
    {
      // Configure the standard options
      rCombinedOptions.push_back(Option("h", "help", OptionArgument::OptionNone, 'h', "Display options"));
      rCombinedOptions.push_back(Option("ghelp", OptionArgument::OptionRequired, 1, "Display option groups: all, demo or host"));
      rCombinedOptions.push_back(Option("v", "verbose", OptionArgument::OptionNone, 'v', "Enable verbose output"));
      //rCombinedOptions.push_back(Option("Test1", OptionArgument::Required, nullptr, 10, "test required arg"));
      //rCombinedOptions.push_back(Option("Test2", OptionArgument::Optional, nullptr, 11, "test required arg"));

      // Add the options from the supplied parsers
      std::deque<Option> options;
      for (auto itrRecords = inputArgs.begin(); itrRecords != inputArgs.end(); ++itrRecords)
      {
        IOptionParser* pDemoInputArgs = itrRecords->Parser;

        if (pDemoInputArgs == nullptr)
          throw std::invalid_argument("inputArgs deque can not contain null");

        options.clear();
        pDemoInputArgs->ArgumentSetup(options);

        // Shift the command id's with the requested offset
        for (auto itrOptions = options.begin(); itrOptions != options.end(); ++itrOptions)
        {
          itrOptions->CmdId += itrRecords->CmdIdOffset;
        }

        AppendArguments(rCombinedOptions, options);
      }
    }

    std::size_t FindMaxNameLength(const std::deque<Option>& args, const int32_t optionGroupFlags)
    {
      std::size_t maxLength = 0;
      std::deque<Option>::const_iterator itr = args.begin();
      const std::deque<Option>::const_iterator itrEnd = args.end();

      while (itr != itrEnd)
      {
        if ((optionGroupFlags & int32_t(itr->Group)) != 0)
        {
          auto len = itr->Name != nullptr ? strlen(itr->Name) + 2 : 0;
          len += itr->ShortName != nullptr ? 4 : 0;

          switch (itr->HasArg)
          {
          case OptionArgument::OptionRequired:
            len += 6;  // " <arg>"
            break;
          //case OptionArgument::OptionOptional:
          //  len += 6;  // " [arg]"
          //  break;
          case OptionArgument::OptionNone:
          default:
            break;
          }

          if (len > maxLength)
            maxLength = len;
        }
        ++itr;
      }
      return maxLength;
    }


    const char* SafeString(const char*const psz)
    {
      return psz != nullptr ? psz : "";
    }


    const std::string GetFormattedDescription(const char*const psz, const std::size_t indentation)
    {
      std::string str(psz);
      std::stringstream stream;

      std::string::const_iterator itrFrom = str.begin();
      std::string::const_iterator itrTo = str.end();
      std::string::const_iterator itrFound;
      const std::streamsize defaultWidth = stream.width();
      do
      {
        itrFound = std::find(itrFrom, itrTo, '\n');

        while (itrFrom != itrFound)
        {
          stream << *itrFrom;
          ++itrFrom;
        }
        if (itrFound != itrTo)
        {
          stream << "\n";
          stream.width(indentation);
          stream << " ";
          stream.width(defaultWidth);
          ++itrFound;
          itrFrom = itrFound;
        }
      } while (itrFound != itrTo);

      return stream.str();
    }

    const std::string GetFormattedName(const Option& option)
    {
      std::string str;

      if (option.ShortName != nullptr)
      {
        if ( ! option.IsPositional )
          str += "-";
        str += option.ShortName;
        if (option.Name != nullptr)
          str += ", ";
      }
      if (option.Name != nullptr)
      {
        if (!option.IsPositional)
          str += "--";
        str += option.Name;
      }

      switch (option.HasArg)
      {
      case OptionArgument::OptionRequired:
        str += " <arg>";
        break;
      //case OptionArgument::OptionOptional:
      //  str += " [arg]";
      //  break;
      case OptionArgument::OptionNone:
      default:
        break;
      }
      return str;
    }

    void ShowHelp(const char*const pszHelpCaption, const std::deque<Option>& options, const int32_t optionGroupFlags)
    {
      const auto maxNameLength = FindMaxNameLength(options, optionGroupFlags);
      std::stringstream stream;

      // Add the caption if supplied
      if (pszHelpCaption != nullptr)
        stream << pszHelpCaption << "\n";

      std::deque<Option>::const_iterator itr = options.begin();
      const std::deque<Option>::const_iterator itrEnd = options.end();

      const std::streamsize defaultWidth = stream.width();
      while (itr != itrEnd)
      {
        if ((optionGroupFlags & int32_t(itr->Group)) != 0)
        {
          // 4 due to the "  --"
          // 3 due to the " = "
          std::string strDesc(GetFormattedDescription(SafeString(itr->Description), 4 + 3 + maxNameLength));
          stream << "  ";
          stream.width(maxNameLength);
          stream << std::left << GetFormattedName(*itr);
          stream.width(defaultWidth);
          stream << " = " << strDesc << std::endl;
        }
        ++itr;
      }

      FSLLOG(stream.str());
    }


    bool ProcessHelpOption(const char*const pszOptionParam, int32_t& rShowHelpOptionGroupFlags)
    {
      bool bSuccess = true;
      if (pszOptionParam == nullptr)
        rShowHelpOptionGroupFlags |= (0x7FFFFFFF & (~OptionGroup::Hidden));
      else
      {
        if (strcmp(pszOptionParam, "all") == 0)
          rShowHelpOptionGroupFlags |= (0x7FFFFFFF & (~OptionGroup::Hidden));
        else if (strcmp(pszOptionParam, "demo") == 0)
          rShowHelpOptionGroupFlags |= OptionGroup::Demo;
        else if (strcmp(pszOptionParam, "host") == 0)
          rShowHelpOptionGroupFlags |= OptionGroup::Host;
        else if (strcmp(pszOptionParam, "hidden") == 0)
          rShowHelpOptionGroupFlags |= OptionGroup::Hidden;
        else
        {
          bSuccess = false;
          FSLLOG("ERROR: Unknown help argument '" << pszOptionParam << "'");
        }
      }
      return bSuccess;
    }
  }


  bool OptionParser::Parse(int argc, char** argv, const char*const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsers;
    return Parse(argc, argv, inputOptionParsers, pszHelpCaption);
  }


  bool OptionParser::Parse(int argc, char** argv, IOptionParser& inputOptionParser, const char*const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsers;
    inputOptionParsers.push_back(ParserRecord(&inputOptionParser, 0));
    return Parse(argc, argv, inputOptionParsers, pszHelpCaption);
  }


  bool OptionParser::Parse(int argc, char** argv, const std::deque<IOptionParser*>& inputOptionParsers, const char*const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsersEx;
    for (auto itr = inputOptionParsers.begin(); itr != inputOptionParsers.end(); ++itr)
      inputOptionParsersEx.push_back(ParserRecord(*itr, 0));
    return Parse(argc, argv, inputOptionParsersEx, pszHelpCaption);
  }


  bool OptionParser::Parse(int argc, char** argv, const std::deque<ParserRecord>& inputOptionParsers, const char*const pszHelpCaption)
  {
    std::deque<Option> combinedOptions;
    ArgumentSetup(inputOptionParsers, combinedOptions);

    bool bForceExit = false;
    int32_t showHelpOptionGroupFlags = 0;
    int optionErrors = 0;
    { // Parse input arguments
      int value;
      std::string strOptArg;
      OptionParserTCLAP parser(argc, argv, combinedOptions);
      while (parser.Next(value, strOptArg))
      {
        switch (value)
        {
        case 'v': // verbose
          // FIX: enable verbose mode
          break;
        case 'h':
          showHelpOptionGroupFlags |= (0x7FFFFFFF & (~OptionGroup::Hidden));
          break;
        case 1: // ghelp
          if (!ProcessHelpOption(strOptArg.c_str(), showHelpOptionGroupFlags))
            ++optionErrors;
          break;
        default:
          {
            // Parse the unknown option off to the supplied parsers
            OptionParseResult::Enum result = OptionParseResult::NotHandled;
            auto itr = inputOptionParsers.begin();
            while (itr != inputOptionParsers.end() && result == OptionParseResult::NotHandled)
            {
              try
              {
                // Remove the offset before we call the parser
                const int32_t cmdId = value - itr->CmdIdOffset;
                result = itr->Parser->Parse(cmdId, strOptArg.c_str());
              }
              catch (const std::exception& ex)
              {
                FSLLOG("ERROR: Parse option failed with: " << ex.what());
                result = OptionParseResult::Failed;
              }
              ++itr;
            }
            if (result != OptionParseResult::Parsed)
              ++optionErrors;
            break;
          }
        }
      }

      if (optionErrors == 0)
      {
        // Give the parsers a chance to finalize their configuration.
        auto itr = inputOptionParsers.begin();
        while (itr != inputOptionParsers.end() && !bForceExit)
        {
          bForceExit = !(itr->Parser->ParsingComplete());
          ++itr;
        }
      }
    }

    if (showHelpOptionGroupFlags != 0)
      ShowHelp(pszHelpCaption, combinedOptions, showHelpOptionGroupFlags);

    return !bForceExit && showHelpOptionGroupFlags == 0 && optionErrors == 0;
  }



}
