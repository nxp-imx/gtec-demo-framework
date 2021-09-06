/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/String/StringDateUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <cstdlib>
#include <ctime>
#include <string>

#ifdef _WIN32
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define timegm _mkgmtime
#endif

namespace Fsl
{
  namespace StringDateUtil
  {
    namespace
    {
      namespace LocalConfig
      {
        //                                                               1
        //                                                     01234567890123456789
        constexpr std::size_t ExpectedLength = StringViewLite("1970-01-01T00:00:00Z").size();
      }

      void ThrowFormatException(const StringViewLite strView)
      {
        throw FormatException(fmt::format(FMT_STRING("the string '{}' did not match the expected format"), strView));
      }
    }

    std::string ToString(const std::time_t& currentTime)
    {
      return fmt::format("{0:%F}T{0:%T}Z", fmt::gmtime(currentTime));
    }

    std::time_t Parse(const StringViewLite strView)
    {
      if (strView.size() != LocalConfig::ExpectedLength)
      {
        throw FormatException(
          fmt::format(FMT_STRING("the string '{}' is of length {} and not {} as required"), strView, strView.size(), LocalConfig::ExpectedLength));
      }
      if (strView[4] != '-' || strView[7] != '-' || strView[10] != 'T' || strView[13] != ':' || strView[16] != ':' || strView[19] != 'Z')
      {
        ThrowFormatException(strView);
      }

      uint16_t year{0};
      uint16_t month{0};
      uint16_t day{0};
      uint16_t hour{0};
      uint16_t minute{0};
      uint16_t second{0};
      if (StringParseUtil::Parse(year, strView.substr(0, 4)) != 4)
      {
        ThrowFormatException(strView);
      }
      if (StringParseUtil::Parse(month, strView.substr(5, 2)) != 2)
      {
        ThrowFormatException(strView);
      }
      if (StringParseUtil::Parse(day, strView.substr(8, 2)) != 2)
      {
        ThrowFormatException(strView);
      }
      if (StringParseUtil::Parse(hour, strView.substr(11, 2)) != 2)
      {
        ThrowFormatException(strView);
      }
      if (StringParseUtil::Parse(minute, strView.substr(14, 2)) != 2)
      {
        ThrowFormatException(strView);
      }
      if (StringParseUtil::Parse(second, strView.substr(17, 2)) != 2)
      {
        ThrowFormatException(strView);
      }

      if (year > 9999 || month < 1 || month > 12 || day < 1 || day > 31 || hour > 23 || minute > 59 || second > 60)
      {
        ThrowFormatException(strView);
      }

      std::tm time{};
      time.tm_year = UncheckedNumericCast<int>(year) - 1900;
      time.tm_mon = UncheckedNumericCast<int>(month) - 1;
      time.tm_mday = UncheckedNumericCast<int>(day);
      time.tm_hour = UncheckedNumericCast<int>(hour);
      time.tm_min = UncheckedNumericCast<int>(minute);
      time.tm_sec = UncheckedNumericCast<int>(second);
      time.tm_isdst = 0;
      std::time_t res = timegm(&time);
      if (res == -1)
      {
        ThrowFormatException(strView);
      }
      return res;
    }
  }
}
