/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>

namespace Fsl
{
  namespace Arguments
  {
    namespace
    {
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

        ParseContext(std::deque<EncodedCommand>&& encodedCommands)
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
        ParseResult Result = ParseResult::InternalError;
        //! The last argument index that was touched by the parser when the result occurred
        //! If this is negative no argument was touched
        int LastSourceArgumentIndex{0};
        //! If this is negative no command was found
        int32_t CommandIndex{0};

        ParseArgumentResult() = default;
        constexpr ParseArgumentResult(const ParseResult result, const int lastSourceArgumentIndex, const int32_t commandIndex) noexcept
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

      LookupResult TryLookupShortArgument(const char* const pszArg, const std::deque<Command>& commands, ParseContext& rParseContext)
      {
        assert(pszArg != nullptr);
        assert(std::strlen(pszArg) > 1);
        assert(pszArg[0] == '-');
        assert(pszArg[1] != '-');

        const char* pszSrc = pszArg + 1;    // +1 to skip the initial "-"

        while (*pszSrc != 0)
        {
          bool found = false;
          int32_t commandIndex = 0;
          for (const auto& cmd : commands)
          {
            if (cmd.ShortName.size() == 1 && *pszSrc == cmd.ShortName[0])
            {
              found = true;
              auto result = TryAddArgument(cmd, EncodedCommand(cmd.Type, cmd.Id, nullptr), rParseContext);
              if (result != ParseResult::Completed)
              {
                return LookupResult(result, CommandType::Undefined, commandIndex);
              }
              // ok we were able to add the argument, so lets see if its a value argument
              if ((static_cast<uint32_t>(cmd.Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) != 0u)
              {
                // this argument is followed by a value, so it should be the last entry in a sequence
                return LookupResult((*(pszSrc + 1) == 0 ? ParseResult::Completed : ParseResult::CombinedValueArgumentMustBeLastError), cmd.Type,
                                    commandIndex);
              }
            }
            ++commandIndex;
          }
          if (!found)
          {
            return LookupResult(ParseResult::UnknownArgumentError, CommandType::Undefined, -1);
          }
          ++pszSrc;
        }
        return LookupResult(ParseResult::Completed, CommandType::Undefined, -1);
      }


      LookupResult TryLookupLongArgument(const char* const pszArg, const std::deque<Command>& commands, ParseContext& rParseContext)
      {
        assert(pszArg != nullptr);
        assert(std::strlen(pszArg) > 2);
        assert(pszArg[0] == '-');
        assert(pszArg[1] == '-');
        assert(pszArg[2] != '-');

        const char* pszSrc = pszArg + 2;    // +2 to skip the initial "--"

        int32_t commandIndex = 0;
        for (const auto& cmd : commands)
        {
          if (!cmd.Name.empty() && cmd.Name == pszSrc)
          {
            auto result = TryAddArgument(cmd, EncodedCommand(cmd.Type, cmd.Id, nullptr), rParseContext);
            if (result != ParseResult::Completed)
            {
              return LookupResult(result, CommandType::Undefined, commandIndex);
            }
            // ok we were able to add the argument, so lets see if its a value argument
            if ((static_cast<uint32_t>(cmd.Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) != 0u)
            {
              // this argument is followed by a value, so it should be the last entry in a sequence
              return LookupResult(ParseResult::Completed, cmd.Type, commandIndex);
            }
            return LookupResult(ParseResult::Completed, cmd.Type, commandIndex);
          }
          ++commandIndex;
        }
        return LookupResult(ParseResult::UnknownArgumentError, CommandType::Undefined, -1);
      }


      LookupResult TryHandlePositionalArgument(const char* const pszArg, const std::deque<Command>& commands, ParseContext& rParseContext)
      {
        assert(pszArg != nullptr);
        assert(std::strlen(pszArg) > 0);
        assert(pszArg[0] != '-');

        int32_t commandIndex = 0;
        uint32_t positionalArgumentCount = 0;
        for (const auto& cmd : commands)
        {
          assert(cmd.IsValid());
          if ((static_cast<uint32_t>(cmd.Type) & static_cast<uint32_t>(CommandTypeFlags::Positional)) != 0u)
          {
            if (positionalArgumentCount >= rParseContext.PositionalArgumentCount)
            {
              rParseContext.EncodedCommands.emplace_back(PositionalArgument(cmd.Type, cmd.Id, pszArg));
              ++rParseContext.PositionalArgumentCount;
              return LookupResult(ParseResult::Completed, cmd.Type, commandIndex);
            }
            ++positionalArgumentCount;
          }
          ++commandIndex;
        }
        return LookupResult(ParseResult::UnknownArgumentError, CommandType::PositionalValue, -1);
      }


      ArgParseResult TryDetermineArgumentType(const char* const pszArg)
      {
        auto* pszSrc = pszArg;
        ArgParseMode mode = ArgParseMode::Identify;
        while (*pszSrc != 0)
        {
          switch (mode)
          {
          case ArgParseMode::Identify:
            if (*pszSrc != '-')
            {
              return ArgParseResult::PositionalArgument;
            }
            mode = ArgParseMode::ShortArgument;
            break;
          case ArgParseMode::ShortArgument:
            if (*pszSrc != '-')
            {
              return ArgParseResult::ShortArgument;
            }
            mode = ArgParseMode::LongArgument;
            break;
          case ArgParseMode::LongArgument:
            return (*pszSrc == '-' ? ArgParseResult::ArgumentFormatError : ArgParseResult::LongArgument);
          default:
            return ArgParseResult::InternalParseError;
          }
          ++pszSrc;
        }

        switch (mode)
        {
        case ArgParseMode::Identify:         // If we hit this the argument list contained a 'empty' string
        case ArgParseMode::ShortArgument:    // The start of a short argument but without any 'argument to lookup'
          return ArgParseResult::ArgumentFormatError;
        case ArgParseMode::LongArgument:    // The start of a long argument but without any 'argument to lookup' so this is the unhandled arguments
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
          return ParseArgumentTypeResult(res.Result, ParserMode::ErrorOut, res.CommandIndex);
        }

        // The lookup completed and it was a 'value' type, so switch to parse argument value
        const auto newMode =
          ((static_cast<uint32_t>(res.Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) == 0u ? ParserMode::ParseArgumentType
                                                                                                    : ParserMode::ParseShortArgumentValue);
        return ParseArgumentTypeResult(ParseResult::Completed, newMode, res.CommandIndex);
      }


      ParseArgumentTypeResult TryParseArgumentType(const char* const pszArg, const std::deque<Command>& commands, ParseContext& rParseContext)
      {
        const auto result = TryDetermineArgumentType(pszArg);
        switch (result)
        {
        case ArgParseResult::ShortArgument:
          return HandleArgumentLookupResult(TryLookupShortArgument(pszArg, commands, rParseContext));
        case ArgParseResult::LongArgument:
          return HandleArgumentLookupResult(TryLookupLongArgument(pszArg, commands, rParseContext));
        case ArgParseResult::PositionalArgument:
          return HandleArgumentLookupResult(TryHandlePositionalArgument(pszArg, commands, rParseContext));
        case ArgParseResult::UnhandledArgumentsStart:
          return ParseArgumentTypeResult(ParseResult::Completed, ParserMode::AddUnhandledArguments, -1);
        case ArgParseResult::ArgumentFormatError:
          return ParseArgumentTypeResult(ParseResult::ArgumentFormatError, ParserMode::ErrorOut, -1);
        case ArgParseResult::ArgumentEmptyError:
          return ParseArgumentTypeResult(ParseResult::ArgumentEmptyError, ParserMode::ErrorOut, -1);
        case ArgParseResult::InternalParseError:
        default:
          return ParseArgumentTypeResult(ParseResult::InternalError, ParserMode::ErrorOut, -1);
        }
      }

      enum class UnhandledArgumentMode
      {
        NotKnown,
        Allowed,
        NotAllowed,
      };

      ParseArgumentResult TryParseArguments(const int argc, const char* const* const argv, const std::deque<Command>& commands,
                                            ParseContext& rParseContext)
      {
        assert(argc >= 1);
        assert(argv != nullptr);

        // Examine all arguments
        auto unhandledArgumentMode = UnhandledArgumentMode::NotKnown;
        int32_t commandIndex = -1;
        uint32_t unhandledArgumentCmdId = 0u;
        ParserMode mode = ParserMode::ParseArgumentType;
        for (int i = 0; i < argc; ++i)
        {
          const char* pszArg = argv[i];
          if (pszArg == nullptr)
          {
            return ParseArgumentResult(ParseResult::ArgumentListContainedNullError, i, 0);
          }

          switch (mode)
          {
          case ParserMode::ParseArgumentType:
          {
            auto res = TryParseArgumentType(pszArg, commands, rParseContext);
            if (res.Result != ParseResult::Completed)
            {
              return ParseArgumentResult(res.Result, i, res.CommandIndex);
            }
            mode = res.NewMode;
            commandIndex = res.CommandIndex;
            break;
          }
          case ParserMode::ParseShortArgumentValue:
          case ParserMode::ParseLongArgumentValue:
            assert(!rParseContext.EncodedCommands.empty());
            assert((static_cast<uint32_t>(rParseContext.EncodedCommands.back().Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) != 0u);
            assert(rParseContext.EncodedCommands.back().pszOptArg == nullptr);
            mode = ParserMode::ParseArgumentType;
            rParseContext.EncodedCommands.back().pszOptArg = pszArg;
            assert(rParseContext.EncodedCommands.back().pszOptArg != nullptr);
            break;
          case ParserMode::AddUnhandledArguments:
            if (unhandledArgumentMode == UnhandledArgumentMode::NotKnown)
            {
              assert(commandIndex < 0);
              auto itrFind =
                std::find_if(commands.begin(), commands.end(), [](const Command& entry) { return entry.Type == CommandType::Unhandled; });
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
              rParseContext.EncodedCommands.emplace_back(UnhandledArgument(unhandledArgumentCmdId, pszArg));
              ++rParseContext.UnhandledArgumentCount;
            }
            else
            {
              return ParseArgumentResult(ParseResult::UnknownArgumentError, i, 0);
            }
            break;
          case ParserMode::ErrorOut:
          default:
            return ParseArgumentResult(ParseResult::InternalError, i, 0);
          }
        }

        const auto lastArgumentIndex = (argc > 0 ? argc - 1 : 0);

        // Check the mode
        switch (mode)
        {
        case ParserMode::ParseArgumentType:
        case ParserMode::AddUnhandledArguments:
          return ParseArgumentResult(ParseResult::Completed, -1, -1);
        case ParserMode::ParseShortArgumentValue:
        case ParserMode::ParseLongArgumentValue:
          return ParseArgumentResult(ParseResult::ArgumentMissingValueError, lastArgumentIndex, commandIndex);
        case ParserMode::ErrorOut:
        default:
          // FIX: log
          return ParseArgumentResult(ParseResult::InternalError, lastArgumentIndex, 0);
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
              return ParseArgumentResult(ParseResult::RequiredArgumentNotFound, -1, commandIndex);
            }
          }
          ++commandIndex;
        }
        return ParseArgumentResult(ParseResult::Completed, -1, -1);
      }
    }

    namespace ArgumentParser
    {
      ParseResult TryParse(std::deque<EncodedCommand>& rEncodedCommands, const int argCount, const char* const* const ppszArgs,
                           const std::deque<Command>& commands, ParseErrorInfo* pErrorInfo)
      {
        rEncodedCommands.clear();
        if (pErrorInfo != nullptr)
        {
          *pErrorInfo = {};
        }

        if (argCount < 0 || ppszArgs == nullptr)
        {
          return ParseResult::InvalidArguments;
        }

        auto res = TryValidateArgumentCommands(commands);
        if (res != ParseResult::Completed)
        {
          return res;
        }

        if (argCount == 0)
        {
          return ParseResult::Completed;
        }

        ParseContext parseContext(std::move(rEncodedCommands));
        auto parseResult = TryParseArguments(argCount, ppszArgs, commands, parseContext);
        rEncodedCommands = std::move(parseContext.EncodedCommands);

        assert(parseResult.LastSourceArgumentIndex < argCount);

        if (parseResult.Result == ParseResult::Completed)
        {
          parseResult = CheckRequiredArgumentsArePresent(commands, rEncodedCommands);
        }

        assert(parseResult.LastSourceArgumentIndex < argCount);

        if (parseResult.Result != ParseResult::Completed && pErrorInfo != nullptr)
        {
          pErrorInfo->IsValid = true;
          pErrorInfo->SourceArgumentIndex = parseResult.LastSourceArgumentIndex;
          pErrorInfo->CommandIndex = parseResult.CommandIndex;
        }
        return parseResult.Result;
      }
    }
  }
}
