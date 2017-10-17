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
#include <FslBase/String/ToString.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/Getopt/OptionParser.hpp>
#include "OptionParserTCLAP.hpp"
#include "OptionRecord.hpp"

namespace Fsl
{
  namespace
  {
    bool IsUniqueOption(const std::deque<OptionRecord>& combinedOptions, const Option& option)
    {
      for (std::size_t i = 0; i < combinedOptions.size(); ++i)
      {
        const Option& existingOption = combinedOptions[i].SourceOption;
        if (option.ShortName.size() > 0 && option.ShortName == existingOption.ShortName)
          return false;
        if (option.Name.size() > 0 && option.Name == existingOption.Name)
          return false;
      }
      return true;
    }


    void AppendArguments(std::deque<OptionRecord>& rCombinedOptions, const std::shared_ptr<OptionSourceRecord> source, const std::deque<Option>& sourceOptions)
    {
      for (std::size_t i = 0; i < sourceOptions.size(); ++i)
      {
        const Option option = sourceOptions[i];

        if (IsUniqueOption(rCombinedOptions, option))
          rCombinedOptions.push_back(OptionRecord(source, option));
        else
        {
          std::stringstream stream("Option already defined: ");
          if (option.ShortName.size() > 0)
            stream << "'" << option.ShortName << "'";
          if (option.Name.size() > 0)
          {
            if (option.ShortName.size() > 0)
              stream << "' ";
            else
              stream << "'";
            stream << option.Name << "'";
          }
          throw UsageErrorException(stream.str());
        }
      }
    }


    void ArgumentSetup(const std::deque<OptionParser::ParserRecord>& inputArgs, std::deque<OptionRecord>& rCombinedOptions)
    {
      const auto source = std::make_shared<OptionSourceRecord>("base");

      // Configure the standard options
      rCombinedOptions.push_back(OptionRecord(source, Option("h", "help", OptionArgument::OptionNone, 'h', "Display options")));
      rCombinedOptions.push_back(OptionRecord(source, Option("ghelp", OptionArgument::OptionRequired, 1, "Display option groups: all, demo or host")));
      rCombinedOptions.push_back(OptionRecord(source, Option("v", "verbose", OptionArgument::OptionNone, 'v', "Enable verbose output")));
      rCombinedOptions.push_back(OptionRecord(source, Option("System.Arguments.Save", OptionArgument::OptionRequired, 2, "Save all command line arguments to the given file name.", OptionGroup::Hidden)));
      //rCombinedOptions.push_back(Option("Test1", OptionArgument::Required, nullptr, 10, "test required arg"));
      //rCombinedOptions.push_back(Option("Test2", OptionArgument::Optional, nullptr, 11, "test required arg"));

      // Add the options from the supplied parsers
      std::deque<Option> options;
      for(const auto& inputEntry : inputArgs)
      {
        IOptionParser* pDemoInputArgs = inputEntry.Parser;

        if (pDemoInputArgs == nullptr)
          throw std::invalid_argument("inputArgs deque can not contain null");

        options.clear();
        pDemoInputArgs->ArgumentSetup(options);

        // Shift the command id's with the requested offset
        for(auto& rOption : options)
        {
          rOption.CmdId += inputEntry.CmdIdOffset;
        }

        const auto source = std::make_shared<OptionSourceRecord>(inputEntry.Parser->GetName());
        AppendArguments(rCombinedOptions, source, options);
      }
    }

    std::size_t FindMaxNameLength(const std::deque<OptionRecord>& args, const int32_t optionGroupFlags)
    {
      std::size_t maxLength = 0;
      std::deque<OptionRecord>::const_iterator itr = args.begin();
      const std::deque<OptionRecord>::const_iterator itrEnd = args.end();

      while (itr != itrEnd)
      {
        if ((optionGroupFlags & int32_t(itr->SourceOption.Group)) != 0)
        {
          auto len = itr->SourceOption.Name.size() > 0 ? itr->SourceOption.Name.size() + 2 : 0;
          len += itr->SourceOption.ShortName.size() > 0 ? 4 : 0;

          switch (itr->SourceOption.HasArg)
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



    const std::string GetFormattedDescription(const std::string& str, const std::size_t indentation)
    {
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

      if (option.ShortName.size() > 0)
      {
        if ( ! option.IsPositional )
          str += "-";
        str += option.ShortName;
        if (option.Name.size() > 0)
          str += ", ";
      }
      if (option.Name.size() > 0)
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

    void ShowHelp(const char*const pszHelpCaption, const std::deque<OptionRecord>& options, const int32_t optionGroupFlags)
    {
      const auto maxNameLength = FindMaxNameLength(options, optionGroupFlags);
      std::stringstream stream;

      // Add the caption if supplied
      if (pszHelpCaption != nullptr)
        stream << pszHelpCaption << "\n";

      std::deque<OptionRecord>::const_iterator itr = options.begin();
      const std::deque<OptionRecord>::const_iterator itrEnd = options.end();

      const std::streamsize defaultWidth = stream.width();
      while (itr != itrEnd)
      {
        if ((optionGroupFlags & int32_t(itr->SourceOption.Group)) != 0)
        {
          // 4 due to the "  --"
          // 3 due to the " = "
          std::string strDesc(GetFormattedDescription(itr->SourceOption.Description, 4 + 3 + maxNameLength));
          stream << "  ";
          stream.width(maxNameLength);
          stream << std::left << GetFormattedName(itr->SourceOption);
          stream.width(defaultWidth);
          stream << " = " << strDesc << std::endl;
        }
        ++itr;
      }

      FSLLOG(stream.str());
    }


    bool ProcessHelpOption(const std::string& strOptionParam, int32_t& rShowHelpOptionGroupFlags)
    {
      bool bSuccess = true;
      if (strOptionParam.size() == 0)
        rShowHelpOptionGroupFlags |= (0x7FFFFFFF & (~OptionGroup::Hidden));
      else
      {
        if (strOptionParam == "all")
          rShowHelpOptionGroupFlags |= (0x7FFFFFFF & (~OptionGroup::Hidden));
        else if (strOptionParam == "demo")
          rShowHelpOptionGroupFlags |= OptionGroup::Demo;
        else if (strOptionParam == "host")
          rShowHelpOptionGroupFlags |= OptionGroup::Host;
        else if (strOptionParam == "hidden")
          rShowHelpOptionGroupFlags |= OptionGroup::Hidden;
        else
        {
          bSuccess = false;
          FSLLOG("ERROR: Unknown help argument '" << strOptionParam << "'");
        }
      }
      return bSuccess;
    }


    std::string SafeJsonString(const std::string& str)
    {
      // Extremely basic escaping, ought to work for our use case
      std::string newStr(str);
      StringUtil::Replace(newStr, "\n", "\\n");
      StringUtil::Replace(newStr, "\"", "\\\"");
      return newStr;
    }

    template<typename T>
    std::string SafeJsonString(const T& value)
    {
      return SafeJsonString(ToString(value));
    }


    bool TrySaveArgumentsToJsonFile(const std::string& strOptionParam, const std::deque<OptionRecord>& combinedOptions)
    {
      if (strOptionParam.size() <= 0)
      {
        FSLLOG_ERROR("No filename specified");
        return false;
      }

      try
      {
        FSLLOG("Saving command line arguments to the json file: '" << strOptionParam << "'");

        std::stringstream stream;
        stream << "{\n";
        stream << "  \"arguments\":\n";
        stream << "  [\n";

        std::size_t index = 0;
        const auto entries = combinedOptions.size();

        for (auto& entry : combinedOptions)
        {
          ++index;
          stream << "    {\n";
          stream << "      \"CmdId\": \"" << SafeJsonString(entry.SourceOption.CmdId) << "\",\n";
          stream << "      \"Description\": \"" << SafeJsonString(entry.SourceOption.Description) << "\",\n";
          stream << "      \"Group\": \"" << SafeJsonString(entry.SourceOption.Group) << "\",\n";
          stream << "      \"HasArg\": \"" << SafeJsonString(entry.SourceOption.HasArg) << "\",\n";
          stream << "      \"IsPositional\": \"" << SafeJsonString(entry.SourceOption.IsPositional) << "\",\n";
          stream << "      \"Name\": \"" << SafeJsonString(entry.SourceOption.Name) << "\",\n";
          stream << "      \"ShortName\": \"" << SafeJsonString(entry.SourceOption.ShortName) << "\",\n";
          stream << "      \"Type\": \"" << SafeJsonString(entry.SourceOption.Type) << "\",\n";
          stream << "      \"Help_FormattedName\": \"" << SafeJsonString(GetFormattedName(entry.SourceOption)) << "\",\n";
          stream << "      \"SourceName\": \"" << SafeJsonString(entry.Source->Name) << "\"\n";
          if (index < entries)
            stream << "    },\n";
          else
            stream << "    }\n";
        }
        stream << "  ]\n";
        stream << "}\n";

        IO::File::WriteAllText(strOptionParam, stream.str());
        return true;
      }
      catch (const std::exception& ex)
      {
        FSLLOG_ERROR("Failed to save arguments to file due to exception: " << ex.what());
      }
      return false;
    }
  }


  OptionParser::Result OptionParser::Parse(int argc, char** argv, const char*const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsers;
    return Parse(argc, argv, inputOptionParsers, pszHelpCaption);
  }


  OptionParser::Result OptionParser::Parse(int argc, char** argv, IOptionParser& inputOptionParser, const char*const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsers;
    inputOptionParsers.push_back(ParserRecord(&inputOptionParser, 0));
    return Parse(argc, argv, inputOptionParsers, pszHelpCaption);
  }


  OptionParser::Result OptionParser::Parse(int argc, char** argv, const std::deque<IOptionParser*>& inputOptionParsers, const char*const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsersEx;
    for (auto itr = inputOptionParsers.begin(); itr != inputOptionParsers.end(); ++itr)
      inputOptionParsersEx.push_back(ParserRecord(*itr, 0));
    return Parse(argc, argv, inputOptionParsersEx, pszHelpCaption);
  }


  OptionParser::Result OptionParser::Parse(int argc, char** argv, const std::deque<ParserRecord>& inputOptionParsers, const char*const pszHelpCaption)
  {
    std::deque<OptionRecord> combinedOptions;
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
          if (!ProcessHelpOption(strOptArg, showHelpOptionGroupFlags))
            ++optionErrors;
          break;
        case 2: // System.Arguments.Save
          if (!TrySaveArgumentsToJsonFile(strOptArg, combinedOptions))
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

    if (optionErrors != 0)
      return Result::Failed;

    return !bForceExit && showHelpOptionGroupFlags == 0 ? Result::OK : Result::Exit;
  }



}
