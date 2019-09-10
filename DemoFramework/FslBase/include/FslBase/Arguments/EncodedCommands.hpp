#ifndef FSLBASE_ARGUMENTS_ENCODEDCOMMANDS_HPP
#define FSLBASE_ARGUMENTS_ENCODEDCOMMANDS_HPP
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

#include <FslBase/Arguments/EncodedCommand.hpp>
#include <cassert>

namespace Fsl
{
  namespace Arguments
  {
    // The various argument types (these are basically just here for type safety)
    class SwitchArgument : public EncodedCommand
    {
    public:
      constexpr SwitchArgument() noexcept = default;

      constexpr SwitchArgument(const uint32_t commandId) noexcept
        : EncodedCommand(CommandType::Switch, commandId, nullptr)
      {
      }

      explicit SwitchArgument(const EncodedCommand& argument)
        : EncodedCommand(argument)
      {
        assert(argument.Type == CommandType::Switch);
      }
    };


    class MultiSwitchArgument : public EncodedCommand
    {
    public:
      constexpr MultiSwitchArgument() noexcept = default;

      constexpr MultiSwitchArgument(const uint32_t commandId, const uint32_t count) noexcept
        : EncodedCommand(CommandType::MultiSwitch, commandId, nullptr, count)
      {
      }

      explicit MultiSwitchArgument(const EncodedCommand& argument)
        : EncodedCommand(argument)
      {
        assert(argument.Type == CommandType::MultiSwitch);
      }
    };


    class ValueArgument : public EncodedCommand
    {
    public:
      constexpr ValueArgument() noexcept = default;

      constexpr ValueArgument(const uint32_t commandId) noexcept
        : EncodedCommand(CommandType::Value, commandId, nullptr)
      {
      }

      explicit ValueArgument(const EncodedCommand& argument)
        : EncodedCommand(argument)
      {
        assert(argument.Type == CommandType::Value);
      }
    };


    class MultiValueArgument : public EncodedCommand
    {
    public:
      constexpr MultiValueArgument() noexcept = default;

      constexpr MultiValueArgument(const uint32_t commandId) noexcept
        : EncodedCommand(CommandType::MultiValue, commandId, nullptr)
      {
      }

      explicit MultiValueArgument(const EncodedCommand& argument)
        : EncodedCommand(argument)
      {
        assert(argument.Type == CommandType::MultiValue);
      }
    };


    class PositionalArgument : public EncodedCommand
    {
    public:
      constexpr PositionalArgument() noexcept = default;

      constexpr PositionalArgument(const CommandType commandType, const uint32_t commandId, const char* const pszOptArg) noexcept
        : EncodedCommand(commandType, commandId, pszOptArg)
      {
        assert((static_cast<uint32_t>(commandType) & static_cast<uint32_t>(CommandTypeFlags::Positional)) != 0u);
      }

      explicit PositionalArgument(const EncodedCommand& argument)
        : EncodedCommand(argument)
      {
        assert((static_cast<uint32_t>(argument.Id) & static_cast<uint32_t>(CommandTypeFlags::Positional)) != 0u);
      }
    };

    class PositionalValueArgument : public PositionalArgument
    {
    public:
      constexpr PositionalValueArgument() noexcept = default;

      constexpr PositionalValueArgument(const uint32_t commandId, const char* const pszOptArg) noexcept
        : PositionalArgument(CommandType::PositionalValue, commandId, pszOptArg)
      {
      }

      explicit PositionalValueArgument(const EncodedCommand& argument)
        : PositionalArgument(argument)
      {
        assert(argument.Type == CommandType::PositionalValue);
      }
    };


    class UnhandledArgument : public EncodedCommand
    {
    public:
      constexpr UnhandledArgument() noexcept = default;

      constexpr UnhandledArgument(const uint32_t commandId, const char* const psz) noexcept
        : EncodedCommand(CommandType::Unhandled, commandId, psz)
      {
      }

      explicit UnhandledArgument(const EncodedCommand& argument)
        : EncodedCommand(argument)
      {
        assert(argument.Type == CommandType::Unhandled);
      }
    };
  }
}

#endif
