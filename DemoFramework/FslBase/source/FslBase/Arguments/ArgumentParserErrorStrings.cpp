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

#include <FslBase/Arguments/ArgumentParserErrorStrings.hpp>
#include <FslBase/Arguments/ParseErrorInfo.hpp>
#include <cassert>

namespace Fsl
{
  namespace Arguments
  {
    namespace ArgumentParser
    {
      namespace
      {
        const char* const SafeGetArgumentString(const int argCount, const char* const* const ppszArgs, const int32_t index)
        {
          if (index < 0)
          {
            // No argument
            return "";
          }
          if (argCount < 0 || ppszArgs == nullptr)
          {
            return "(invalid arguments)";
          }
          if (index >= argCount)
          {
            return "(index out of bounds)";
          }
          return ppszArgs[index] != nullptr ? ppszArgs[index] : "nullptr";
        }

        std::string GetCombinedArgumentErrorString(const ParseResult result, const ParseErrorInfo& parseErrorInfo, const int argCount,
                                                   const char* const* const ppszArgs, const std::deque<Command>& commands, ErrorFormatter& formatter,
                                                   const char* const pszFormat1, const char* const pszFormat2)
        {
          auto pszArg = SafeGetArgumentString(argCount, ppszArgs, parseErrorInfo.SourceArgumentIndex);
          assert(pszArg != nullptr);
          if (pszArg[0] == '-')
          {
            if (pszArg[1] != '-' && pszArg[1] != 0 && pszArg[2] != 0)
            {
              // short name combined switch  (since its length was larger than 2)
              if (parseErrorInfo.CommandIndex >= 0)
              {
                assert(static_cast<uint32_t>(parseErrorInfo.CommandIndex) < commands.size());
                if (static_cast<uint32_t>(parseErrorInfo.CommandIndex) < commands.size())
                {
                  assert(!commands[parseErrorInfo.CommandIndex].ShortName.empty());
                  // and we have a matching command and its a combined switch
                  return formatter.Format(pszFormat2, commands[parseErrorInfo.CommandIndex].ShortName);
                }
              }
            }
            // long name switch
          }
          return formatter.Format(pszFormat1, pszArg);
        }

        std::string SafeGetCommandName(const Command& command)
        {
          return (!command.Name.empty() ? command.Name : command.ShortName);
        }

        std::string GetRequiredArgumentNotFoundErrorString(const std::deque<Command>& commands, const int32_t commandIndex, ErrorFormatter& formatter)
        {
          if (commandIndex < 0 || static_cast<uint32_t>(commandIndex) >= commands.size())
          {
            return "Required argument not found";
          }
          const auto& cmd = commands[commandIndex];
          if ((static_cast<uint32_t>(cmd.Type) & static_cast<uint32_t>(CommandTypeFlags::Positional)) != 0u)
          {
            // Positional argument
            return formatter.Format("Required positional argument '{0}' is missing", SafeGetCommandName(cmd));
          }
          // else if ((static_cast<uint32_t>(cmd.Type) & static_cast<uint32_t>(CommandTypeFlags::Value)) != 0u)
          //{
          //  // value argument
          //  if (!cmd.Name.empty())
          //  {
          //    return formatter.Format("Required value argument '--{0}' is missing", cmd.Name);
          //  }
          //  return formatter.Format("Required value argument '-{0}' is missing", cmd.ShortName);
          //}

          if (!cmd.Name.empty())
          {
            return formatter.Format("Required argument '--{0}' is missing", cmd.Name);
          }
          return formatter.Format("Required argument '-{0}' is missing", cmd.ShortName);
        }


      }

      std::string GetErrorString(const ParseResult result, const ParseErrorInfo& parseErrorInfo, const int argCount,
                                 const char* const* const ppszArgs, const std::deque<Command>& commands, ErrorFormatter& formatter)
      {
        switch (result)
        {
        case ParseResult::InternalError:
          return "Internal error";
        case ParseResult::Completed:
          return "Completed";
        case ParseResult::InvalidArguments:
          return "Method called with invalid arguments";
        case ParseResult::DuplicatedSwitchArgumentError:
          return GetCombinedArgumentErrorString(result, parseErrorInfo, argCount, ppszArgs, commands, formatter, "Duplicated switch '{}'",
                                                "Duplicated switch '-{}'");
        case ParseResult::DuplicatedValueArgumentError:
          return GetCombinedArgumentErrorString(result, parseErrorInfo, argCount, ppszArgs, commands, formatter, "Duplicated value '{}'",
                                                "Duplicated value '-{}'");
        case ParseResult::UnknownArgumentError:
          return formatter.Format("Unknown argument '{}'", SafeGetArgumentString(argCount, ppszArgs, parseErrorInfo.SourceArgumentIndex));
        case ParseResult::ArgumentMissingValueError:
          return GetCombinedArgumentErrorString(result, parseErrorInfo, argCount, ppszArgs, commands, formatter, "Missing value for argument '{}'",
                                                "Missing value for argument '-{}'");
        case ParseResult::CombinedValueArgumentMustBeLastError:
          return GetCombinedArgumentErrorString(result, parseErrorInfo, argCount, ppszArgs, commands, formatter,
                                                "Combined short value argument '{}' must be last in sequence",
                                                "Combined short value argument '-{}' must be last in sequence");
        case ParseResult::ArgumentFormatError:
          return formatter.Format("Argument '{}' has a invalid format",
                                  SafeGetArgumentString(argCount, ppszArgs, parseErrorInfo.SourceArgumentIndex));
        case ParseResult::RequiredArgumentNotFound:
          return GetRequiredArgumentNotFoundErrorString(commands, parseErrorInfo.CommandIndex, formatter);
        case ParseResult::ArgumentEmptyError:
          return formatter.Format("Empty argument string found at argument index '{}'",
                                  SafeGetArgumentString(argCount, ppszArgs, parseErrorInfo.SourceArgumentIndex));
        case ParseResult::ArgumentListContainedNullError:
          return formatter.Format("nullptr found at argument index '{}'", parseErrorInfo.SourceArgumentIndex);
        case ParseResult::CommandListIsInvalidError:
          return "Command list is invalid";
        default:
          return formatter.Format("Unknown error {0}", static_cast<uint32_t>(result));
        }
      }
    }
  }
}
