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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/String/StringViewLiteArrayUtil.hpp>
#include <fmt/format.h>
#include <array>
#include <cassert>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstring>
#include <limits>

namespace Fsl
{
  namespace
  {
    namespace LocalStrings
    {
      constexpr const StringViewLite StrTrue("true");
      constexpr const StringViewLite StrFalse("false");
      constexpr const StringViewLite Str1("1");
      constexpr const StringViewLite Str0("0");
    }

    inline void CheckInput(const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
    {
      if ((startIndex + length) > strView.size())
      {
        throw std::invalid_argument("startIndex or length out of bounds");
      }
    }


    uint32_t ParseUInt32(const StringViewLite strView)
    {
      if (!strView.empty())
      {
        // Check the string contains a digit as expected
        if ((strView[0] >= '0' && strView[0] <= '9') || (strView.size() >= 2u && strView[0] == '+' && (strView[1] >= '0' && strView[1] <= '9')))
        {
          //// 01234567890
          //// 4294967295
          if (strView.size() >= 32)
          {
            throw std::invalid_argument("string is too long");
          }
          const auto tmpBuffer = StringViewLiteArrayUtil::ToArray<32>(strView);

          char* pEnd = nullptr;
          errno = 0;
          auto value = strtoul(tmpBuffer.data(), &pEnd, 10);
          if ((value == ULONG_MAX && errno == ERANGE) || value > std::numeric_limits<uint32_t>::max())
          {
            throw OverflowException("The number is outside than the expected value range");
          }

          if (pEnd != (tmpBuffer.data() + strView.size()))
          {
            throw FormatException("number not in the correct format");
          }
          return static_cast<uint32_t>(value);
        }
      }

      throw FormatException("number not in the correct format");
    }


    int32_t ParseInt32(const StringViewLite strView)
    {
      if (!strView.empty())
      {
        // Check the string contains a digit as expected
        if ((strView[0] >= '0' && strView[0] <= '9') ||
            (strView.size() >= 2 && (strView[0] == '+' || strView[0] == '-') && (strView[1] >= '0' && strView[1] <= '9')))
        {
          char* pEnd = nullptr;
          errno = 0;

          if (strView.size() >= 32)
          {
            throw std::invalid_argument("string is too long");
          }
          const auto tmpBuffer = StringViewLiteArrayUtil::ToArray<32>(strView);

          const auto value = strtol(tmpBuffer.data(), &pEnd, 10);
          if (((value == LONG_MIN || value == LONG_MAX) && errno == ERANGE) || value < std::numeric_limits<int32_t>::min() ||
              value > std::numeric_limits<int32_t>::max())
          {
            throw OverflowException("The number is outside than the expected value range");
          }

          if (pEnd != (tmpBuffer.data() + strView.size()))
          {
            throw FormatException("number not in the correct format");
          }
          return NumericCast<int32_t>(value);
        }
      }
      throw FormatException("number not in the correct format");
    }

    double ParseDouble(const StringViewLite strView)
    {
      if (!strView.empty())
      {
        // Check the string doesn't start with a white space
        if (strView[0] != ' ')
        {
          if (strView.size() >= 32)
          {
            throw std::invalid_argument("string is too long");
          }
          const auto tmpBuffer = StringViewLiteArrayUtil::ToArray<32>(strView);

          char* pEnd = nullptr;
          errno = 0;
          const double value = strtod(tmpBuffer.data(), &pEnd);
          if (value == HUGE_VAL && errno == ERANGE)
          {
            throw OverflowException("The number is outside than the expected value range");
          }

          if (pEnd != (tmpBuffer.data() + strView.size()))
          {
            throw FormatException("number not in the correct format");
          }
          return value;
        }
      }
      throw FormatException("number not in the correct format");
    }

    template <typename T>
    std::size_t DoParseArray(Span<T> dst, const StringViewLite strView)
    {
      if (strView.empty())
      {
        throw FormatException("array not in the correct format, empty string");
      }

      const char* pszCurrent = strView.data();
      const char* const pszEnd = pszCurrent + strView.size();
      if (strView.size() < 3 || *pszCurrent != '[' || *(pszEnd - 1) != ']')
      {
        throw FormatException(fmt::format("array not in the correct format '{}'", strView.AsStringView()));
      }

      ++pszCurrent;
      std::size_t index = 0;
      while (pszCurrent < pszEnd && index < dst.size())
      {
        const auto count = strcspn(pszCurrent, ",]");
        if (index >= dst.size())
        {
          throw FormatException(fmt::format("array not in the correct format '{}'", strView.AsStringView()));
        }
        const auto charactersConsumed = StringParseUtil::Parse(dst[index], StringViewLite(pszCurrent, count));
        assert(charactersConsumed == count);
        pszCurrent += charactersConsumed + 1;
        ++index;
      }

      if (pszCurrent != pszEnd)
      {
        throw FormatException(fmt::format("array not in the correct format '{}'", strView.AsStringView()));
      }
      return index;
    }
  }

  template <typename T>
  std::size_t DoParseArray(Span<T> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return DoParseArray(dst, strView.substr(startIndex, length));
  }

  std::size_t StringParseUtil::Parse(bool& rResult, const StringViewLite strView)
  {
    std::size_t charactersConsumed = 0;
    if (strView == LocalStrings::StrTrue)
    {
      rResult = true;
      charactersConsumed = LocalStrings::StrTrue.size();
    }
    else if (strView == LocalStrings::StrFalse)
    {
      rResult = false;
      charactersConsumed = LocalStrings::StrFalse.size();
    }
    else if (strView.size() == 1)
    {
      if (strView == LocalStrings::Str1)
      {
        rResult = true;
        charactersConsumed = LocalStrings::Str1.size();
      }
      else if (strView == LocalStrings::Str0)
      {
        rResult = false;
        charactersConsumed = LocalStrings::Str0.size();
      }
    }

    if (charactersConsumed == 0u)
    {
      throw FormatException(fmt::format("'{}' is not a valid bool string. Valid values are: (true, false, 1, 0)", strView));
    }
    return charactersConsumed;
  }


  std::size_t StringParseUtil::Parse(uint8_t& rResult, const StringViewLite strView)
  {
    const uint32_t result = ParseUInt32(strView);
    if (result > std::numeric_limits<uint8_t>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = NumericCast<uint8_t>(result);
    return strView.size();
  }


  std::size_t StringParseUtil::Parse(int8_t& rResult, const StringViewLite strView)
  {
    const int32_t result = ParseInt32(strView);
    if (result < std::numeric_limits<int8_t>::min() || result > std::numeric_limits<int8_t>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = NumericCast<int8_t>(result);
    return strView.size();
  }


  std::size_t StringParseUtil::Parse(uint16_t& rResult, const StringViewLite strView)
  {
    const uint32_t result = ParseUInt32(strView);
    if (result > std::numeric_limits<uint16_t>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = NumericCast<uint16_t>(result);
    return strView.size();
  }


  std::size_t StringParseUtil::Parse(int16_t& rResult, const StringViewLite strView)
  {
    const int32_t result = ParseInt32(strView);
    if (result < std::numeric_limits<int16_t>::min() || result > std::numeric_limits<int16_t>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = NumericCast<int16_t>(result);
    return strView.size();
  }


  std::size_t StringParseUtil::Parse(uint32_t& rResult, const StringViewLite strView)
  {
    rResult = ParseUInt32(strView);
    return strView.size();
  }


  std::size_t StringParseUtil::Parse(int32_t& rResult, const StringViewLite strView)
  {
    rResult = ParseInt32(strView);
    return strView.size();
  }


  // int StringParseUtil::Parse(uint64_t& rResult, const char*const psz)
  //{
  // if (psz == nullptr)
  //  throw std::invalid_argument("psz can not be null");
  // return Parse(rResult, psz, 0, std::strlen(psz));
  //}


  // int StringParseUtil::Parse(int64_t& rResult, const char*const psz)
  //{
  // if (psz == nullptr)
  //  throw std::invalid_argument("psz can not be null");
  // return Parse(rResult, psz, 0, std::strlen(psz));
  //}


  std::size_t StringParseUtil::Parse(float& rResult, const StringViewLite strView)
  {
    const double result = ParseDouble(strView);
    if (result < std::numeric_limits<float>::lowest() || result > std::numeric_limits<float>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = static_cast<float>(result);
    return strView.size();
  }


  std::size_t StringParseUtil::Parse(double& rResult, const StringViewLite strView)
  {
    rResult = ParseDouble(strView);
    return strView.size();
  }


  std::size_t StringParseUtil::Parse(Point2& rResult, const StringViewLite strView)
  {
    std::array<int32_t, 2> values{};
    StringParseArrayResult res = ParseArray(SpanUtil::AsSpan(values), strView);
    if (res.ArrayEntries != values.size())
    {
      throw FormatException("Point2 not in the correct format");
    }
    rResult = Point2(values[0], values[1]);
    return res.CharactersConsumed;
  }


  std::size_t StringParseUtil::Parse(Point2U& rResult, const StringViewLite strView)
  {
    std::array<uint32_t, 2> values{};
    StringParseArrayResult res = ParseArray(SpanUtil::AsSpan(values), strView);
    if (res.ArrayEntries != values.size())
    {
      throw FormatException("Point2U not in the correct format");
    }
    rResult = Point2U(values[0], values[1]);
    return res.CharactersConsumed;
  }

  std::size_t StringParseUtil::Parse(PxSize2D& rResult, const StringViewLite strView)
  {
    std::array<int32_t, 2> values{};
    StringParseArrayResult res = ParseArray(SpanUtil::AsSpan(values), strView);
    if (res.ArrayEntries != values.size())
    {
      throw FormatException("Point2 not in the correct format");
    }
    rResult = PxSize2D::Create(values[0], values[1]);
    return res.CharactersConsumed;
  }


  std::size_t StringParseUtil::Parse(Rectangle& rResult, const StringViewLite strView)
  {
    std::array<int32_t, 4> values{};
    StringParseArrayResult res = ParseArray(SpanUtil::AsSpan(values), strView);
    if (res.ArrayEntries != values.size())
    {
      throw FormatException("Rectangle not in the correct format");
    }
    if (values[2] < 0 || values[3] < 0)
    {
      throw FormatException("Rectangle not in the correct format, width and height can not be negative");
    }
    rResult = Rectangle(values[0], values[1], values[2], values[3]);
    return res.CharactersConsumed;
  }


  std::size_t StringParseUtil::Parse(bool& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(uint8_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(int8_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(uint16_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(int16_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(uint32_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(int32_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  // int StringParseUtil::Parse(uint64_t& rResult, const char*const psz, const std::size_t startIndex, const std::size_t length)
  //{
  //  throw NotImplementedException();
  //}


  // int StringParseUtil::Parse(int64_t& rResult, const char*const psz, const std::size_t startIndex, const std::size_t length)
  //{
  //  throw NotImplementedException();
  //}


  std::size_t StringParseUtil::Parse(float& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(double& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(Point2& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(Point2U& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  std::size_t StringParseUtil::Parse(Rectangle& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return Parse(rResult, strView.substr(startIndex, length));
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<bool> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<uint8_t> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<int8_t> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<uint16_t> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<int16_t> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<uint32_t> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<int32_t> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  // const StringParseArrayResult StringParseUtil::ParseArray(Span<uint64_t> dst, const char*const psz)
  //{
  // if (pDst == nullptr)
  //  throw std::invalid_argument("pDst can not be null");
  // if (psz == nullptr)
  //  throw std::invalid_argument("psz can not be null");
  // return ParseArray(pDst, dstLength, psz, 0, std::strlen(psz));
  //}


  // const StringParseArrayResult StringParseUtil::ParseArray(Span<int64_t> dst, const char*const psz)
  //{
  // if (pDst == nullptr)
  //  throw std::invalid_argument("pDst can not be null");
  // if (psz == nullptr)
  //  throw std::invalid_argument("psz can not be null");
  // return ParseArray(pDst, dstLength, psz, 0, std::strlen(psz));
  //}


  StringParseArrayResult StringParseUtil::ParseArray(Span<float> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<double> dst, const StringViewLite strView)
  {
    const auto arrayEntries = DoParseArray(dst, strView);
    return {strView.size(), arrayEntries};
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<bool> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<uint8_t> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<int8_t> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<uint16_t> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<int16_t> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<uint32_t> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<int32_t> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  // StringParseArrayResult StringParseUtil::ParseArray(Span<uint64_t> dst, const StringViewLite strView, const std::size_t startIndex, const
  // std::size_t length)
  //{
  // CheckInput(strView, startIndex, length);
  // return ParseArray(dst, strView.substr(startIndex, length));
  //}


  // StringParseArrayResult StringParseUtil::ParseArray(Span<int64_t> dst, const StringViewLite strView, const std::size_t startIndex, const
  // std::size_t length)
  //{
  // CheckInput(strView, startIndex, length);
  // return ParseArray(dst, strView.substr(startIndex, length));
  //}


  StringParseArrayResult StringParseUtil::ParseArray(Span<float> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  StringParseArrayResult StringParseUtil::ParseArray(Span<double> dst, const StringViewLite strView, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    CheckInput(strView, startIndex, length);
    return ParseArray(dst, strView.substr(startIndex, length));
  }


  bool StringParseUtil::TryParse(int32_t& rValue, const StringViewLite strView, const int32_t radix)
  {
    rValue = 0;
    if (strView.empty())
    {
      FSLLOG3_DEBUG_WARNING("length too small to contain a int32");
      return false;
    }

    // Check the string contains a digit as expected
    bool ch1Extended = false;
    bool ch2Extended = false;
    if (radix > 10)
    {
      const int32_t radixAbove10 = radix - 10;
      ch1Extended = (strView[0] >= 'a' && (strView[0] - 'a') < radixAbove10) || (strView[0] >= 'A' && (strView[0] - 'A') < radixAbove10);
      ch2Extended =
        strView.size() >= 2 && ((strView[1] >= 'a' && (strView[1] - 'a') < radixAbove10) || (strView[1] >= 'A' && (strView[1] - 'A') < radixAbove10));
    }

    if ((strView[0] >= '0' && (strView[0] <= '9' || ch1Extended)) ||
        (strView.size() >= 2 && (strView[0] == '+' || strView[0] == '-') && (strView[1] >= '0' && (strView[1] <= '9' || ch2Extended))))
    {
      char* pEnd = nullptr;
      errno = 0;

      if (strView.size() >= 32)
      {
        FSLLOG3_DEBUG_WARNING("string is too long");
        return false;
      }
      std::array<char, 32> tmpBuffer = StringViewLiteArrayUtil::ToArray<32>(strView);

      const auto value = strtol(tmpBuffer.data(), &pEnd, radix);
      if (((value == LONG_MIN || value == LONG_MAX) && errno == ERANGE) || value < std::numeric_limits<int32_t>::min() ||
          value > std::numeric_limits<int32_t>::max())
      {
        return false;
      }

      if (pEnd != (tmpBuffer.data() + strView.size()))
      {
        return false;
      }

      rValue = NumericCast<int32_t>(value);
      return true;
    }
    return false;
  }

  bool StringParseUtil::TryParse(int32_t& rValue, const StringViewLite strView, const std::size_t startIndex, const std::size_t length,
                                 const int32_t radix)
  {
    if ((startIndex + length) > strView.size())
    {
      rValue = 0;
      FSLLOG3_DEBUG_WARNING("startIndex or length out of bounds");
      return false;
    }
    return TryParse(rValue, strView.substr(startIndex, length), radix);
  }


  std::size_t StringParseUtil::ParseTime(std::chrono::seconds& rResult, const StringViewLite strView)
  {
    // 012345678
    // HH:MM:SS
    const std::size_t expectedLength = 8;
    if (strView.size() != expectedLength || strView[2] != ':' || strView[5] != ':')
    {
      throw FormatException("time not in the expected HH:MM:SS format");
    }

    const uint32_t hh = ParseUInt32(strView.substr(0, 2));
    const uint32_t mm = ParseUInt32(strView.substr(3, 2));
    const uint32_t ss = ParseUInt32(strView.substr(6, 2));

    if (ss >= 60 || mm >= 60 || hh > 24)
    {
      throw FormatException("number not in the correct format");
    }

    rResult = std::chrono::seconds((hh * 60 * 60) + (mm * 60) + ss);
    return expectedLength;
  }
}
