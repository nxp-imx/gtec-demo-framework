/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include <FslBase/Arguments/ArgumentParser.hpp>
#include <FslBase/Arguments/EncodedCommands.hpp>
#include <FslBase/Arguments/ParseErrorInfo.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>

namespace Fsl::Arguments
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t EmptyIndex = 0;
    }

    enum class ParserMode
    {
      ErrorOut,
      ParseArgumentType,
      ParseShortArgumentValue,
      ParseLongArgumentValue,
      // All the remaining arguments should be classified as 'unhandled'
      AddUnhandledArguments,
    };

    enum class ArgParseMode
    {
      Identify,
      ShortArgument,
      LongArgument,
    };

    enum class ArgParseResult
    {
      //! A short argument
      ShortArgument,
      //! A long argument
      LongArgument,
      //! A positional argument
      PositionalArgument,
      //! Unhandled arguments start
      UnhandledArgumentsStart,

      // Possible user input errors
      //! '---' Three dashes or more encountered in argument
      ArgumentFormatError,

      //! This should never happen, the argument list contained a empty string. For normal C++ main arguments this will not happen,
      //! but from data generated elsewhere it might.
      ArgumentEmptyError,

      //! This should never happen, the parser failed.
      InternalParseError
    };


    struct ParseContext
    {
      std::deque<EncodedCommand> EncodedCommands;
      uint32_t PositionalArgumentCount{0};
      uint32_t UnhandledArgumentCount{0};

      ParseContext() = default;

      explicit ParseContext(std::deque<EncodedCommand>&& encodedCommands)
        : EncodedCommands(std::move(encodedCommands))
      {
      }
    };


    struct LookupResult
    {
      ParseResult Result = ParseResult::InternalError;
      CommandType Type = CommandType::Undefined;
      //! If this is negative no command was found
      int32_t CommandIndex = -1;

      LookupResult() = default;
      constexpr LookupResult(const ParseResult result, const CommandType type, const int32_t commandIndex) noexcept
        : Result(result)
        , Type(type)
        , CommandIndex(commandIndex)
      {
      }
    };


    struct ParseArgumentTypeResult
    {
      ParseResult Result = ParseResult::InternalError;
      // This is only valid if result is ParseResult::Completed
      ParserMode NewMode = ParserMode::ErrorOut;
      //! If this is negative no command was found
      int32_t CommandIndex = -1;

      ParseArgumentTypeResult() = default;
      constexpr ParseArgumentTypeResult(const ParseResult result, const ParserMode newMode, const int32_t commandIndex) noexcept
        : Result(result)
        , NewMode(newMode)
        , CommandIndex(commandIndex)
      {
      }
    };

    struct ParseArgumentResult
    {
      ParseResult Result{ParseResult::InternalError};
      //! The last argument index that was touched by the parser when the result occurred
      //! If this is negative no argument was touched
      uint32_t LastSourceArgumentIndex{0};
      //! If this is negative no command was found
      int32_t CommandIndex{0};

      constexpr ParseArgumentResult() = default;
      constexpr ParseArgumentResult(const ParseResult result, const uint32_t lastSourceArgumentIndex, const int32_t commandIndex) noexcept
        : Result(result)
        , LastSourceArgumentIndex(lastSourceArgumentIndex)
        , CommandIndex(commandIndex)
      {
      }
    };


    ParseResult TryAddArgument(const Command& argument, const EncodedCommand& encodedArgument, ParseContext& rParseContext)
    {
      auto itrFind = std::find_if(rParseContext.EncodedCommands.begin(), rParseContext.EncodedCommands.end(),
                                  [argument](const EncodedCommand& entry) { return (entry.Type == argument.Type && entry.Id == argument.Id); });

      if (itrFind != rParseContext.EncodedCommands.end())
      {
        switch (argument.Type)
        {
        case CommandType::Switch:
        case CommandType::PositionalValue:
          return ParseResult::DuplicatedSwitchArgumentError;
        case CommandType::Value:
          return ParseResult::DuplicatedValueArgumentError;
        case CommandType::MultiSwitch:
          ++itrFind->Count;
          return ParseResult::Completed;
        case CommandType::Unhandled:
        default:
          return ParseResult::InternalError;
        }
      }
      rParseContext.EncodedCommands.emplace_back(encodedArgument);
      return ParseResult::Completed;
    }

    LookupResult TryLookupShortArgument(const StringViewLite strArg, const std::deque<Command>& commands, ParseContext& rParseContext)
    {
      assert(strArg.size() > 1);
      assert(strArg[0] == '-');
      assert(strArg[1] != '-');

      auto strSrc = strArg.substr(1u);    // +1 to skip the initial "-"
      while (!strSrc.empty())
      {
        bool found = false;
        int32_t commandIndex = 0;
        for (const auto& cmd : commands)
        {
          if (cmd.ShortName.size() == 1 && strSrc.front() == cmd.ShortName[0])
          {
            found = true;
            const auto result = TryAddArgument(cmd, EncodedCommand(cmd.Type, cmd.Id, nullptr), rParseContext);
            if (result != ParseResult::Completed)
            {
              return {result, CommandType::Undefined, commandIndex};
            }
            // ok we were able to add the argument, so lets see if its a value argument
            if ((static_cast<uint32_t>(cmd.Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) != 0u)
            {
              // this argument is followed by a value, so it should be the last entry in a sequence
              return {(strSrc.size() == 1 ? ParseResult::Completed : ParseResult::CombinedValueArgumentMustBeLastError), cmd.Type, commandIndex};
            }
          }
          ++commandIndex;
        }
        if (!found)
        {
          return {ParseResult::UnknownArgumentError, CommandType::Undefined, -1};
        }
        strSrc.remove_prefix(1u);
      }
      return {ParseResult::Completed, CommandType::Undefined, -1};
    }


    LookupResult TryLookupLongArgument(const StringViewLite strArg, const std::deque<Command>& commands, ParseContext& rParseContext)
    {
      assert(strArg.size() > 2);
      assert(strArg[0] == '-');
      assert(strArg[1] == '-');
      assert(strArg[2] != '-');

      const auto strSrc = strArg.substr(2u);    // +2 to skip the initial "--"

      int32_t commandIndex = 0;
      for (const auto& cmd : commands)
      {
        if (!cmd.Name.empty() && cmd.Name == strSrc)
        {
          const auto result = TryAddArgument(cmd, EncodedCommand(cmd.Type, cmd.Id, StringViewLite()), rParseContext);
          if (result != ParseResult::Completed)
          {
            return {result, CommandType::Undefined, commandIndex};
          }
          // ok we were able to add the argument, so lets see if its a value argument
          if ((static_cast<uint32_t>(cmd.Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) != 0u)
          {
            // this argument is followed by a value, so it should be the last entry in a sequence
            return {ParseResult::Completed, cmd.Type, commandIndex};
          }
          return {ParseResult::Completed, cmd.Type, commandIndex};
        }
        ++commandIndex;
      }
      return {ParseResult::UnknownArgumentError, CommandType::Undefined, -1};
    }


    LookupResult TryHandlePositionalArgument(const StringViewLite strArg, const std::deque<Command>& commands, ParseContext& rParseContext)
    {
      assert(!strArg.empty());
      assert(strArg[0] != '-');

      int32_t commandIndex = 0;
      uint32_t positionalArgumentCount = 0;
      for (const auto& cmd : commands)
      {
        assert(cmd.IsValid());
        if ((static_cast<uint32_t>(cmd.Type) & static_cast<uint32_t>(CommandTypeFlags::Positional)) != 0u)
        {
          if (positionalArgumentCount >= rParseContext.PositionalArgumentCount)
          {
            rParseContext.EncodedCommands.emplace_back(PositionalArgument(cmd.Type, cmd.Id, strArg));
            ++rParseContext.PositionalArgumentCount;
            return {ParseResult::Completed, cmd.Type, commandIndex};
          }
          ++positionalArgumentCount;
        }
        ++commandIndex;
      }
      return {ParseResult::UnknownArgumentError, CommandType::PositionalValue, -1};
    }


    ArgParseResult TryDetermineArgumentType(const StringViewLite strArg)
    {
      StringViewLite strSrc(strArg);
      ArgParseMode mode = ArgParseMode::Identify;
      while (!strSrc.empty())
      {
        switch (mode)
        {
        case ArgParseMode::Identify:
          if (strSrc.front() != '-')
          {
            return ArgParseResult::PositionalArgument;
          }
          mode = ArgParseMode::ShortArgument;
          break;
        case ArgParseMode::ShortArgument:
          if (strSrc.front() != '-')
          {
            return ArgParseResult::ShortArgument;
          }
          mode = ArgParseMode::LongArgument;
          break;
        case ArgParseMode::LongArgument:
          return (strSrc.front() == '-' ? ArgParseResult::ArgumentFormatError : ArgParseResult::LongArgument);
        default:
          return ArgParseResult::InternalParseError;
        }
        strSrc.remove_prefix(1u);
      }

      switch (mode)
      {
      case ArgParseMode::Identify:         // If we hit this the argument list contained a 'empty' string
      case ArgParseMode::ShortArgument:    // The start of a short argument but without any 'argument to lookup'
        return ArgParseResult::ArgumentFormatError;
      case ArgParseMode::LongArgument:     // The start of a long argument but without any 'argument to lookup' so this is the unhandled arguments
                                           // start indicator
        return ArgParseResult::UnhandledArgumentsStart;
      default:
        return ArgParseResult::InternalParseError;
      }
    }

    ParseArgumentTypeResult HandleArgumentLookupResult(const LookupResult res)
    {
      if (res.Result != ParseResult::Completed)
      {
        return {res.Result, ParserMode::ErrorOut, res.CommandIndex};
      }

      // The lookup completed and it was a 'value' type, so switch to parse argument value
      const auto newMode =
        ((static_cast<uint32_t>(res.Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) == 0u ? ParserMode::ParseArgumentType
                                                                                                  : ParserMode::ParseShortArgumentValue);
      return {ParseResult::Completed, newMode, res.CommandIndex};
    }


    ParseArgumentTypeResult TryParseArgumentType(const StringViewLite strArg, const std::deque<Command>& commands, ParseContext& rParseContext)
    {
      const auto result = TryDetermineArgumentType(strArg);
      switch (result)
      {
      case ArgParseResult::ShortArgument:
        return HandleArgumentLookupResult(TryLookupShortArgument(strArg, commands, rParseContext));
      case ArgParseResult::LongArgument:
        return HandleArgumentLookupResult(TryLookupLongArgument(strArg, commands, rParseContext));
      case ArgParseResult::PositionalArgument:
        return HandleArgumentLookupResult(TryHandlePositionalArgument(strArg, commands, rParseContext));
      case ArgParseResult::UnhandledArgumentsStart:
        return {ParseResult::Completed, ParserMode::AddUnhandledArguments, -1};
      case ArgParseResult::ArgumentFormatError:
        return {ParseResult::ArgumentFormatError, ParserMode::ErrorOut, -1};
      case ArgParseResult::ArgumentEmptyError:
        return {ParseResult::ArgumentEmptyError, ParserMode::ErrorOut, -1};
      case ArgParseResult::InternalParseError:
      default:
        return {ParseResult::InternalError, ParserMode::ErrorOut, -1};
      }
    }

    enum class UnhandledArgumentMode
    {
      NotKnown,
      Allowed,
      NotAllowed,
    };

    ParseArgumentResult TryParseArguments(const ReadOnlySpan<StringViewLite> args, const std::deque<Command>& commands, ParseContext& rParseContext)
    {
      assert(!args.empty());
      if (args.size() > std::numeric_limits<uint32_t>::max())
      {
        return {ParseResult::UnsupportedNumberOfArguments, LocalConfig::EmptyIndex, -1};
      }

      // Examine all arguments
      auto unhandledArgumentMode = UnhandledArgumentMode::NotKnown;
      int32_t commandIndex = -1;
      uint32_t unhandledArgumentCmdId = 0u;
      ParserMode mode = ParserMode::ParseArgumentType;
      for (uint32_t i = 0; i < args.size(); ++i)
      {
        const auto& strArg = args[i];

        switch (mode)
        {
        case ParserMode::ParseArgumentType:
          {
            const auto res = TryParseArgumentType(strArg, commands, rParseContext);
            if (res.Result != ParseResult::Completed)
            {
              return {res.Result, i, res.CommandIndex};
            }
            mode = res.NewMode;
            commandIndex = res.CommandIndex;
            break;
          }
        case ParserMode::ParseShortArgumentValue:
        case ParserMode::ParseLongArgumentValue:
          assert(!rParseContext.EncodedCommands.empty());
          assert((static_cast<uint32_t>(rParseContext.EncodedCommands.back().Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) != 0u);
          assert(rParseContext.EncodedCommands.back().StrOptArg.empty());
          mode = ParserMode::ParseArgumentType;
          rParseContext.EncodedCommands.back().StrOptArg = strArg;
          break;
        case ParserMode::AddUnhandledArguments:
          if (unhandledArgumentMode == UnhandledArgumentMode::NotKnown)
          {
            assert(commandIndex < 0);
            auto itrFind = std::find_if(commands.begin(), commands.end(), [](const Command& entry) { return entry.Type == CommandType::Unhandled; });
            if (itrFind == commands.end())
            {
              unhandledArgumentMode = UnhandledArgumentMode::NotAllowed;
            }
            else
            {
              unhandledArgumentMode = UnhandledArgumentMode::Allowed;
              unhandledArgumentCmdId = itrFind->Id;
              // lookup the command index
              assert((itrFind - commands.begin()) <= std::numeric_limits<int32_t>::max());
              commandIndex = static_cast<int32_t>(itrFind - commands.begin());
            }
          }
          if (unhandledArgumentMode == UnhandledArgumentMode::Allowed)
          {
            rParseContext.EncodedCommands.emplace_back(UnhandledArgument(unhandledArgumentCmdId, strArg));
            ++rParseContext.UnhandledArgumentCount;
          }
          else
          {
            return {ParseResult::UnknownArgumentError, i, 0};
          }
          break;
        case ParserMode::ErrorOut:
        default:
          return {ParseResult::InternalError, i, 0};
        }
      }

      const uint32_t lastArgumentIndex = (!args.empty() ? UncheckedNumericCast<uint32_t>(args.size() - 1) : 0);

      // Check the mode
      switch (mode)
      {
      case ParserMode::ParseArgumentType:
      case ParserMode::AddUnhandledArguments:
        return {ParseResult::Completed, LocalConfig::EmptyIndex, -1};
      case ParserMode::ParseShortArgumentValue:
      case ParserMode::ParseLongArgumentValue:
        return {ParseResult::ArgumentMissingValueError, lastArgumentIndex, commandIndex};
      case ParserMode::ErrorOut:
      default:
        // FIX: log
        return {ParseResult::InternalError, lastArgumentIndex, 0};
      }
    }


    ParseResult TryValidateArgumentCommands(const std::deque<Command>& commands)
    {
      if (!commands.empty())
      {
        uint32_t unhandledArgumentCount = 0u;
        for (std::size_t i = 0; i < commands.size(); ++i)
        {
          const auto& arg = commands[i];
          if (!arg.IsValid())
          {
            // invalid argument
            return ParseResult::CommandListIsInvalidError;
          }
          if (arg.Type == CommandType::Unhandled)
          {
            ++unhandledArgumentCount;
          }

          for (std::size_t j = 1; j < commands.size(); ++j)
          {
            if (i != j)
            {
              if (arg.Id == commands[j].Id)
              {
                // A command id can not be reused by multiple commands
                return ParseResult::CommandListIsInvalidError;
              }

              if ((!arg.ShortName.empty() && arg.ShortName == commands[j].ShortName) || (!arg.Name.empty() && arg.Name == commands[j].Name))
              {
                //  a short name or name can not be reused by multiple commands
                return ParseResult::CommandListIsInvalidError;
              }
            }
          }
        }
        if (unhandledArgumentCount > 1u)
        {
          // Multiple unhandled arguments are not supported
          return ParseResult::CommandListIsInvalidError;
        }
      }
      return ParseResult::Completed;
    }

    ParseArgumentResult CheckRequiredArgumentsArePresent(const std::deque<Command>& commands, const std::deque<EncodedCommand>& encodedCommands)
    {
      int32_t commandIndex = 0;
      for (const auto& command : commands)
      {
        if (command.Required)
        {
          auto commandId = command.Id;
          // Check if the required command was found
          auto itrFind = std::find_if(encodedCommands.begin(), encodedCommands.end(),
                                      [commandId](const EncodedCommand& entry) { return (entry.Id == commandId); });
          if (itrFind == encodedCommands.end())
          {
            // it was not, error
            return {ParseResult::RequiredArgumentNotFound, LocalConfig::EmptyIndex, commandIndex};
          }
        }
        ++commandIndex;
      }
      return {ParseResult::Completed, LocalConfig::EmptyIndex, -1};
    }
  }

  namespace ArgumentParser
  {
    ParseResult TryParse(std::deque<EncodedCommand>& rEncodedCommands, const ReadOnlySpan<StringViewLite> args, const std::deque<Command>& commands,
                         ParseErrorInfo* pErrorInfo)
    {
      rEncodedCommands.clear();
      if (pErrorInfo != nullptr)
      {
        *pErrorInfo = {};
      }

      const auto res = TryValidateArgumentCommands(commands);
      if (res != ParseResult::Completed)
      {
        return res;
      }

      if (args.empty())
      {
        return ParseResult::Completed;
      }

      ParseContext parseContext(std::move(rEncodedCommands));
      auto parseResult = TryParseArguments(args, commands, parseContext);
      rEncodedCommands = std::move(parseContext.EncodedCommands);

      assert(parseResult.LastSourceArgumentIndex < args.size());

      if (parseResult.Result == ParseResult::Completed)
      {
        parseResult = CheckRequiredArgumentsArePresent(commands, rEncodedCommands);
      }

      assert(parseResult.LastSourceArgumentIndex < args.size());

      if (parseResult.Result != ParseResult::Completed && pErrorInfo != nullptr)
      {
        pErrorInfo->IsValid = true;
        pErrorInfo->SourceArgumentIndex = UncheckedNumericCast<int32_t>(parseResult.LastSourceArgumentIndex);
        pErrorInfo->CommandIndex = parseResult.CommandIndex;
      }
      return parseResult.Result;
    }
  }
}
