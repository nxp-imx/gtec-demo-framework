#ifndef FSLBASE_GETOPT_UTIL_OPTIONARGUTIL_HPP
#define FSLBASE_GETOPT_UTIL_OPTIONARGUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Getopt/OptionParseResult.hpp>
#include <FslBase/Getopt/Util/OptionArg.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <limits>
#include <string>

namespace Fsl::OptionArgUtil
{
  template <typename T>
  constexpr int32_t IndexOf(const ReadOnlySpan<OptionArg<T>> arguments, const StringViewLite argument) noexcept
  {
    if (argument.size() > UncheckedNumericCast<std::size_t>(std::numeric_limits<int32_t>::max()))
    {
      return -1;
    }

    for (std::size_t i = 0; i < arguments.size(); ++i)
    {
      if (arguments[i].Name == argument)
      {
        return UncheckedNumericCast<int32_t>(i);
      }
    }
    return -1;
  }

  template <typename T>
  constexpr T Parse(const ReadOnlySpan<OptionArg<T>> arguments, const StringViewLite argument, const T defaultValue) noexcept
  {
    int32_t index = IndexOf(arguments, argument);
    return index >= 0 ? arguments[index].Value : defaultValue;
  }


  template <typename T>
  constexpr bool TryParse(const ReadOnlySpan<OptionArg<T>> arguments, const StringViewLite argument, OptionArg<T>& rResult) noexcept
  {
    int32_t index = IndexOf(arguments, argument);
    if (index >= 0)
    {
      rResult = arguments[index];
      return true;
    }
    rResult = {};
    return false;
  }


  template <typename T>
  std::string BuildArgumentString(const ReadOnlySpan<OptionArg<T>> arguments, const bool includeDescription)
  {
    std::string res;
    if (!arguments.empty())
    {
      auto firstSpan = arguments.subspan(0, arguments.size() - 1u);
      for (const auto& entry : firstSpan)
      {
        res += entry.Name;
        if (includeDescription && !entry.Desc.empty())
        {
          res += " (";
          res += entry.Desc;
          res += "), ";
        }
        else
        {
          res += ", ";
        }
      }
      res += arguments.back().Name;
      if (includeDescription && !arguments.back().Desc.empty())
      {
        res += " (";
        res += arguments.back().Desc;
        res += ")";
      }
    }
    return res;
  }

  template <typename T>
  OptionParseResult TryParseOptionArg(const StringViewLite optionName, const ReadOnlySpan<OptionArg<T>> arguments, const StringViewLite argument,
                                      T& rResult)
  {
    OptionArg<T> argMatch;
    if (OptionArgUtil::TryParse(arguments, argument, argMatch))
    {
      rResult = argMatch.Value;
      return OptionParseResult::Parsed;
    }

    FSLLOG3_ERROR("Unsupported {} '{}' expected {}.", optionName, argument, OptionArgUtil::BuildArgumentString(arguments, false));
    return OptionParseResult::Failed;
  }
}

#endif
