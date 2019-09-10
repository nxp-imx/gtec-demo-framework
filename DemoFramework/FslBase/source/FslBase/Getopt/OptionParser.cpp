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

#include <FslBase/Getopt/OptionParser.hpp>
#include <FslBase/Arguments/ArgumentParser.hpp>
#include <FslBase/Arguments/ArgumentParserErrorStrings.hpp>
#include <FslBase/Arguments/ParseErrorInfo.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/BasicLog.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/IO/File.hpp>
#include "OptionRecord.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
//#include <fmt/core.h>

namespace Fsl
{
  namespace
  {
    bool IsUniqueOption(const std::deque<OptionRecord>& combinedOptions, const Option& option)
    {
      for (const auto& combinedOption : combinedOptions)
      {
        const Option& existingOption = combinedOption.SourceOption;
        if (!option.ShortName.empty() && option.ShortName == existingOption.ShortName)
        {
          return false;
        }
        if (!option.Name.empty() && option.Name == existingOption.Name)
        {
          return false;
        }
      }
      return true;
    }


    void AppendArguments(std::deque<OptionRecord>& rCombinedOptions, const std::shared_ptr<OptionSourceRecord>& source,
                         const std::deque<Option>& sourceOptions)
    {
      for (auto option : sourceOptions)
      {
        if (IsUniqueOption(rCombinedOptions, option))
        {
          rCombinedOptions.emplace_back(source, option);
        }
        else
        {
          std::stringstream stream("Option already defined: ");
          if (!option.ShortName.empty())
          {
            stream << "'" << option.ShortName << "'";
          }
          if (!option.Name.empty())
          {
            if (!option.ShortName.empty())
            {
              stream << "' ";
            }
            else
            {
              stream << "'";
            }
            stream << option.Name << "'";
          }
          throw UsageErrorException(stream.str());
        }
      }
    }


    std::deque<OptionRecord> ArgumentSetup(const std::deque<OptionParser::ParserRecord>& inputArgs)
    {
      std::deque<OptionRecord> combinedOptions;
      const auto source = std::make_shared<OptionSourceRecord>("base");

      // Configure the standard options
      combinedOptions.emplace_back(source, Option("h", "help", OptionArgument::OptionNone, 'h', "Display options"));
      combinedOptions.emplace_back(source, Option("ghelp", OptionArgument::OptionRequired, 1, "Display option groups: all, demo or host"));
      combinedOptions.emplace_back(source, Option("v", "verbose", OptionArgument::OptionNone, 'v', "Enable verbose output"));
      // rCombinedOptions.push_back(OptionRecord(source, Option("VerbosityLevel", OptionArgument::OptionRequired, 3, "Set a specific verbosity
      // level")));
      combinedOptions.emplace_back(source, Option("System.Arguments.Save", OptionArgument::OptionRequired, 2,
                                                  "Save all command line arguments to the given file name.", OptionGroup::Hidden));
      // rCombinedOptions.push_back(Option("Test1", OptionArgument::Required, nullptr, 10, "test required arg"));
      // rCombinedOptions.push_back(Option("Test2", OptionArgument::Optional, nullptr, 11, "test required arg"));

      // Add the options from the supplied parsers
      std::deque<Option> options;
      for (const auto& inputEntry : inputArgs)
      {
        IOptionParser* pDemoInputArgs = inputEntry.Parser;

        if (pDemoInputArgs == nullptr)
        {
          throw std::invalid_argument("inputArgs deque can not contain null");
        }

        options.clear();
        pDemoInputArgs->ArgumentSetup(options);

        // Shift the command id's with the requested offset
        for (auto& rOption : options)
        {
          rOption.CmdId += inputEntry.CmdIdOffset;
        }

        const auto source2 = std::make_shared<OptionSourceRecord>(inputEntry.Parser->GetName());
        AppendArguments(combinedOptions, source2, options);
      }
      return combinedOptions;
    }

    std::size_t FindMaxNameLength(const std::deque<OptionRecord>& args, const int32_t optionGroupFlags)
    {
      std::size_t maxLength = 0;
      auto itr = args.begin();
      const std::deque<OptionRecord>::const_iterator itrEnd = args.end();

      while (itr != itrEnd)
      {
        if ((optionGroupFlags & int32_t(itr->SourceOption.Group)) != 0)
        {
          auto len = !itr->SourceOption.Name.empty() ? itr->SourceOption.Name.size() + 2 : 0;
          len += !itr->SourceOption.ShortName.empty() ? 4 : 0;

          switch (itr->SourceOption.HasArg)
          {
          case OptionArgument::OptionRequired:
            len += 6;    // " <arg>"
            break;
            // case OptionArgument::OptionOptional:
            //  len += 6;  // " [arg]"
            //  break;
          case OptionArgument::OptionNone:
          default:
            break;
          }

          if (len > maxLength)
          {
            maxLength = len;
          }
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
      const std::streamsize defaultWidth = 0;
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

      if (!option.ShortName.empty())
      {
        if (!option.IsPositional)
        {
          str += "-";
        }
        str += option.ShortName;
        if (!option.Name.empty())
        {
          str += ", ";
        }
      }
      if (!option.Name.empty())
      {
        if (!option.IsPositional)
        {
          str += "--";
        }
        str += option.Name;
      }

      switch (option.HasArg)
      {
      case OptionArgument::OptionRequired:
        str += " <arg>";
        break;
        // case OptionArgument::OptionOptional:
        //  str += " [arg]";
        //  break;
      case OptionArgument::OptionNone:
      default:
        break;
      }
      return str;
    }

    void ShowHelp(const char* const pszHelpCaption, const std::deque<OptionRecord>& options, const int32_t optionGroupFlags)
    {
      const auto maxNameLength = FindMaxNameLength(options, optionGroupFlags);
      std::stringstream stream;

      // Add the caption if supplied
      if (pszHelpCaption != nullptr)
      {
        stream << pszHelpCaption << "\n";
      }

      auto itr = options.begin();
      const std::deque<OptionRecord>::const_iterator itrEnd = options.end();

      const std::streamsize defaultWidth = 0;
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

      FSLBASICLOG(stream.str());
    }


    bool ProcessHelpOption(const std::string& strOptionParam, int32_t& rShowHelpOptionGroupFlags)
    {
      bool bSuccess = true;
      if (strOptionParam.empty())
      {
        rShowHelpOptionGroupFlags |= (0x7FFFFFFF & (~OptionGroup::Hidden));
      }
      else
      {
        if (strOptionParam == "all")
        {
          rShowHelpOptionGroupFlags |= (0x7FFFFFFF & (~OptionGroup::Hidden));
        }
        else if (strOptionParam == "demo")
        {
          rShowHelpOptionGroupFlags |= OptionGroup::Demo;
        }
        else if (strOptionParam == "host")
        {
          rShowHelpOptionGroupFlags |= OptionGroup::Host;
        }
        else if (strOptionParam == "hidden")
        {
          rShowHelpOptionGroupFlags |= OptionGroup::Hidden;
        }
        else
        {
          bSuccess = false;
          FSLLOG_ERROR("Unknown help argument '" << strOptionParam << "'");
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

    template <typename T>
    std::string SafeJsonString(const T& value)
    {
      return SafeJsonString(ToString(value));
    }


    bool TrySaveArgumentsToJsonFile(const std::string& strOptionParam, const std::deque<OptionRecord>& combinedOptions)
    {
      if (strOptionParam.empty())
      {
        FSLBASICLOG_ERROR("No filename specified");
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
          // stream << fmt::format(
          //  "    {{\n"
          //  "      \"CmdId\": \"{0}\",\n"
          //  "      \"Description\": \"{1}\",\n"
          //  "      \"Group\": \"{2}\",\n"
          //  "      \"HasArg\": \"{3}\",\n"
          //  "      \"IsPositional\": \"{4}\",\n"
          //  "      \"Name\": \"{5}\",\n"
          //  "      \"ShortName\": \"{6}\",\n"
          //  "      \"Type\": \"{7}\",\n"
          //  "      \"Help_FormattedName\": \"{8}\",\n"
          //  "      \"SourceName\": \"{9}\"\n",
          //  SafeJsonString(entry.SourceOption.CmdId), SafeJsonString(entry.SourceOption.Description), SafeJsonString(entry.SourceOption.Group),
          //  SafeJsonString(entry.SourceOption.HasArg), SafeJsonString(entry.SourceOption.IsPositional), SafeJsonString(entry.SourceOption.Name),
          //  SafeJsonString(entry.SourceOption.ShortName), SafeJsonString(entry.SourceOption.Type),
          //  SafeJsonString(GetFormattedName(entry.SourceOption)), SafeJsonString(entry.Source->Name));
          stream << "    {\n";
          stream << R"(      "CmdId": ")" << SafeJsonString(entry.SourceOption.CmdId) << "\",\n";
          stream << R"(      "Description": ")" << SafeJsonString(entry.SourceOption.Description) << "\",\n";
          stream << R"(      "Group": ")" << SafeJsonString(entry.SourceOption.Group) << "\",\n";
          stream << R"(      "HasArg": ")" << SafeJsonString(entry.SourceOption.HasArg) << "\",\n";
          stream << R"(      "IsPositional": ")" << SafeJsonString(entry.SourceOption.IsPositional) << "\",\n";
          stream << R"(      "Name": ")" << SafeJsonString(entry.SourceOption.Name) << "\",\n";
          stream << R"(      "ShortName": ")" << SafeJsonString(entry.SourceOption.ShortName) << "\",\n";
          stream << R"(      "Type": ")" << SafeJsonString(entry.SourceOption.Type) << "\",\n";
          stream << R"(      "Help_FormattedName": ")" << SafeJsonString(GetFormattedName(entry.SourceOption)) << "\",\n";
          stream << R"(      "SourceName": ")" << SafeJsonString(entry.Source->Name) << "\"\n";
          if (index < entries)
          {
            stream << "    },\n";
          }
          else
          {
            stream << "    }\n";
          }
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

    class ParseErrorFormatter : public Arguments::ArgumentParser::ErrorFormatter
    {
    public:
      std::string Format(const char* const pszFormat, const char* const pszArg0) override
      {
        // FIX: replace with the 'fmt' library
        std::string strResult(pszFormat);
        StringUtil::Replace(strResult, "{}", pszArg0);
        return strResult;
      }
      std::string Format(const char* const pszFormat, const std::string& str) override
      {
        // FIX: replace with the 'fmt' library
        std::string strResult(pszFormat);
        StringUtil::Replace(strResult, "{}", str);
        return strResult;
      }
      std::string Format(const char* const pszFormat, const uint32_t arg0) override
      {
        // FIX: replace with the 'fmt' library
        std::string strResult(pszFormat);
        StringUtil::Replace(strResult, "{}", ToString(arg0));
        return strResult;
      }
    };

    inline const char* SafeCString(const char* const psz)
    {
      return psz != nullptr ? psz : "";
    }

    std::deque<Arguments::Command> ToCommands(const std::deque<OptionRecord>& options)
    {
      using namespace Arguments;
      std::deque<Command> commands;

      for (const auto& option : options)
      {
        switch (option.SourceOption.HasArg)
        {
        case OptionArgument::OptionNone:
          commands.emplace_back(Command(option.SourceOption.ShortName, option.SourceOption.Name, option.SourceOption.CmdId, CommandType::Switch));
          break;
        case OptionArgument::OptionRequired:
          if (!option.SourceOption.IsPositional)
          {
            commands.emplace_back(Command(option.SourceOption.ShortName, option.SourceOption.Name, option.SourceOption.CmdId, CommandType::Value));
          }
          else
          {
            const bool required = (option.SourceOption.HasArg == OptionArgument::OptionRequired);
            commands.emplace_back(
              Command(option.SourceOption.ShortName, option.SourceOption.Name, option.SourceOption.CmdId, CommandType::PositionalValue, required));
          }
          break;
        default:
          throw NotSupportedException("OptionArgument not supported");
        }
      }
      return commands;
    }
  }


  OptionParser::ParseResult OptionParser::Parse(int argc, char** argv, const char* const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsers;
    return Parse(argc, argv, inputOptionParsers, pszHelpCaption);
  }


  OptionParser::ParseResult OptionParser::Parse(int argc, char** argv, IOptionParser& inputOptionParser, const char* const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsers;
    inputOptionParsers.emplace_back(&inputOptionParser, 0);
    return Parse(argc, argv, inputOptionParsers, pszHelpCaption);
  }


  OptionParser::ParseResult OptionParser::Parse(int argc, char** argv, const std::deque<IOptionParser*>& inputOptionParsers,
                                                const char* const pszHelpCaption)
  {
    std::deque<ParserRecord> inputOptionParsersEx;
    for (auto itr = inputOptionParsers.begin(); itr != inputOptionParsers.end(); ++itr)
    {
      inputOptionParsersEx.emplace_back(*itr, 0);
    }
    return Parse(argc, argv, inputOptionParsersEx, pszHelpCaption);
  }


  OptionParser::ParseResult OptionParser::Parse(int argc, char** argv, const std::deque<ParserRecord>& inputOptionParsers,
                                                const char* const pszHelpCaption)
  {
    assert(argc > 0);
    assert(argv != nullptr);
    std::deque<OptionRecord> combinedOptions = ArgumentSetup(inputOptionParsers);

    uint32_t verbosityLevel = 0;
    bool bForceExit = false;
    int32_t showHelpOptionGroupFlags = 0;
    int optionErrors = 0;
    {    // Parse input arguments
      auto commands = ToCommands(combinedOptions);
      Arguments::ParseErrorInfo parseErrorInfo;
      std::deque<Arguments::EncodedCommand> encodedCommands;
      auto parseResult = Arguments::ArgumentParser::TryParse(encodedCommands, argc - 1, argv + 1, commands, &parseErrorInfo);
      if (parseResult != Arguments::ParseResult::Completed)
      {
        ParseErrorFormatter formatter;
        auto strParseError = Arguments::ArgumentParser::GetErrorString(parseResult, parseErrorInfo, argc - 1, argv + 1, commands, formatter);
        throw std::runtime_error(strParseError);
      }

      for (const auto& encodedCommand : encodedCommands)
      {
        auto pszOptArg = SafeCString(encodedCommand.pszOptArg);
        switch (encodedCommand.Id)
        {
        case 'v':    // verbose
          ++verbosityLevel;
          break;
        case 'h':
          showHelpOptionGroupFlags |= (0x7FFFFFFF & (~OptionGroup::Hidden));
          break;
        case 1:    // ghelp
          assert(encodedCommand.Type == Arguments::CommandType::Value);
          if (!ProcessHelpOption(pszOptArg, showHelpOptionGroupFlags))
          {
            ++optionErrors;
          }
          break;
        case 2:    // System.Arguments.Save
          assert(encodedCommand.Type == Arguments::CommandType::Value);
          if (!TrySaveArgumentsToJsonFile(pszOptArg, combinedOptions))
          {
            ++optionErrors;
          }
          break;
          // case 3:
          //  StringParseUtil::Parse(verbosityLevel, strOptArg.c_str());
          //  break;
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
              const int32_t cmdId = encodedCommand.Id - itr->CmdIdOffset;
              result = itr->Parser->Parse(cmdId, pszOptArg);
            }
            catch (const std::exception& ex)
            {
              FSLLOG_ERROR("Parse option failed with: " << ex.what());
              result = OptionParseResult::Failed;
            }
            ++itr;
          }
          if (result != OptionParseResult::Parsed)
          {
            ++optionErrors;
          }
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
    {
      ShowHelp(pszHelpCaption, combinedOptions, showHelpOptionGroupFlags);
    }

    if (optionErrors != 0)
    {
      return ParseResult(Result::Failed, verbosityLevel);
    }

    return ParseResult(!bForceExit && showHelpOptionGroupFlags == 0 ? Result::OK : Result::Exit, verbosityLevel);
  }
}
