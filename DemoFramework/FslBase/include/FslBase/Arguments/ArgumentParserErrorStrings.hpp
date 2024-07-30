#ifndef FSLBASE_ARGUMENTS_ARGUMENTPARSERERRORSTRINGS_HPP
#define FSLBASE_ARGUMENTS_ARGUMENTPARSERERRORSTRINGS_HPP
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

#include <FslBase/Arguments/Command.hpp>
#include <FslBase/Arguments/ParseResult.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <fmt/core.h>
#include <cstdint>
#include <deque>
#include <string>

namespace Fsl::Arguments
{
  struct ParseErrorInfo;

  namespace ArgumentParser
  {
    class ErrorFormatter
    {
    public:
      virtual ~ErrorFormatter() = default;

      virtual std::string Format(fmt::format_string<std::string_view> formatString, const std::string_view str) = 0;
      virtual std::string Format(fmt::format_string<uint32_t> formatString, const uint32_t arg0) = 0;
    };

    std::string GetErrorString(const ParseResult result, const ParseErrorInfo& parseErrorInfo, const ReadOnlySpan<StringViewLite> args,
                               const std::deque<Command>& commands, ErrorFormatter& formatter);
  };
}

#endif
