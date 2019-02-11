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

#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <climits>
#include <limits>

namespace Fsl
{
  namespace
  {
    uint32_t ParseUInt32(const char* const psz, const std::size_t startIndex, const std::size_t length)
    {
      if (psz == nullptr)
      {
        throw std::invalid_argument("psz can not be null");
      }

      const auto actualLength = strlen(psz);
      const auto endIndex = startIndex + length;
      if (endIndex > actualLength)
      {
        throw std::invalid_argument("startIndex or length out of bounds");
      }
      if (length < 1)
      {
        throw FormatException("array not in the correct format");
      }

      // Check the string contains a digit as expected
      const char* pszSrc = psz + startIndex;
      if ((pszSrc[0] >= '0' && pszSrc[0] <= '9') || (length >= 2 && pszSrc[0] == '+' && (pszSrc[1] >= '0' && pszSrc[1] <= '9')))
      {
        char* pEnd = nullptr;
        errno = 0;
        auto value = strtoul(pszSrc, &pEnd, 0);
        if ((value == ULONG_MAX && errno == ERANGE) || value > std::numeric_limits<uint32_t>::max())
        {
          throw OverflowException("The number is outside than the expected value range");
        }

        if (pEnd != (pszSrc + length))
        {
          throw FormatException("number not in the correct format");
        }

        return static_cast<uint32_t>(value);
      }
      throw FormatException("number not in the correct format");
    }


    int32_t ParseInt32(const char* const psz, const std::size_t startIndex, const std::size_t length)
    {
      if (psz == nullptr)
      {
        throw std::invalid_argument("psz can not be null");
      }

      const auto actualLength = strlen(psz);
      const auto endIndex = startIndex + length;
      if (endIndex > actualLength)
      {
        throw std::invalid_argument("startIndex or length out of bounds");
      }
      if (length < 1)
      {
        throw FormatException("array not in the correct format");
      }

      // Check the string contains a digit as expected
      const char* pszSrc = psz + startIndex;
      if ((pszSrc[0] >= '0' && pszSrc[0] <= '9') || (length >= 2 && (pszSrc[0] == '+' || pszSrc[0] == '-') && (pszSrc[1] >= '0' && pszSrc[1] <= '9')))
      {
        char* pEnd = nullptr;
        errno = 0;

        if (length >= 32)
        {
          throw std::invalid_argument("string is too long");
        }
        char tmpBuffer[32];
        for (std::size_t i = 0; i < length; ++i)
        {
          tmpBuffer[i] = psz[startIndex + i];
        }
        tmpBuffer[length] = 0;

        const auto value = strtol(tmpBuffer, &pEnd, 0);
        if (((value == LONG_MIN || value == LONG_MAX) && errno == ERANGE) || value < std::numeric_limits<int32_t>::min() ||
            value > std::numeric_limits<int32_t>::max())
        {
          throw OverflowException("The number is outside than the expected value range");
        }

        if (pEnd != (tmpBuffer + length))
        {
          throw FormatException("number not in the correct format");
        }

        return static_cast<int32_t>(value);
      }
      throw FormatException("number not in the correct format");
    }


    double ParseDouble(const char* const psz, const std::size_t startIndex, const std::size_t length)
    {
      if (psz == nullptr)
      {
        throw std::invalid_argument("psz can not be null");
      }

      const auto actualLength = strlen(psz);
      const auto endIndex = startIndex + length;
      if (endIndex > actualLength)
      {
        throw std::invalid_argument("startIndex or length out of bounds");
      }
      if (length < 1)
      {
        throw FormatException("array not in the correct format");
      }

      // Check the string doesn't start with a white space
      const char* pszSrc = psz + startIndex;
      if (pszSrc[0] != ' ')
      {
        char* pEnd = nullptr;
        errno = 0;
        const double value = strtod(pszSrc, &pEnd);
        if (value == HUGE_VAL && errno == ERANGE)
        {
          throw OverflowException("The number is outside than the expected value range");
        }

        if (pEnd != (pszSrc + length))
        {
          throw FormatException("number not in the correct format");
        }

        return value;
      }
      throw FormatException("number not in the correct format");
    }


    template <typename T>
    std::size_t DoParseArray(T* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex, const std::size_t length)
    {
      if (psz == nullptr)
      {
        throw std::invalid_argument("psz can not be null");
      }

      const auto actualLength = strlen(psz);
      const auto endIndex = startIndex + length;
      if (endIndex > actualLength)
      {
        throw std::invalid_argument("startIndex or length out of bounds");
      }
      if (length < 1)
      {
        throw FormatException("array not in the correct format");
      }

      const char* pszCurrent = psz + startIndex;
      const char* const pszEnd = pszCurrent + length;
      if (length < 3 || *pszCurrent != '[' || *(pszEnd - 1) != ']')
      {
        throw FormatException("array not in the correct format");
      }

      ++pszCurrent;
      std::size_t index = 0;
      while (pszCurrent < pszEnd && index < dstLength)
      {
        const auto count = strcspn(pszCurrent, ",]");
        if (index >= dstLength)
        {
          throw FormatException("array not in the correct format");
        }
        const auto charactersConsumed = StringParseUtil::Parse(*(pDst + index), pszCurrent, 0, count);
        assert(charactersConsumed == count);
        pszCurrent += charactersConsumed + 1;
        ++index;
      }

      if (pszCurrent != pszEnd)
      {
        throw FormatException("array not in the correct format");
      }
      return index;
    }
  }

  std::size_t StringParseUtil::Parse(bool& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(uint8_t& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(int8_t& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(uint16_t& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(int16_t& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(uint32_t& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(int32_t& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  // int StringParseUtil::Parse(uint64_t& rResult, const char*const psz)
  //{
  // if (psz == nullptr)
  //  throw std::invalid_argument("psz can not be null");
  // return Parse(rResult, psz, 0, strlen(psz));
  //}


  // int StringParseUtil::Parse(int64_t& rResult, const char*const psz)
  //{
  // if (psz == nullptr)
  //  throw std::invalid_argument("psz can not be null");
  // return Parse(rResult, psz, 0, strlen(psz));
  //}


  std::size_t StringParseUtil::Parse(float& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(double& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(Point2& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(Rectangle& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return Parse(rResult, psz, 0, strlen(psz));
  }


  std::size_t StringParseUtil::Parse(bool& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    rResult = false;
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }

    const auto actualLength = strlen(psz);
    const char* pszSrc = psz + startIndex;
    int charactersConsumed = 0;

    if ((startIndex + length) > actualLength)
    {
      throw std::invalid_argument("startIndex or length out of bounds");
    }
    if (length < 1)
    {
      throw FormatException("length too small to contain a bool");
    }

    if (length == 4 && strncmp(pszSrc, "true", 4) == 0)
    {
      rResult = true;
      charactersConsumed = 4;
    }
    else if (length == 1 && strncmp(pszSrc, "1", 1) == 0)
    {
      rResult = true;
      charactersConsumed = 1;
    }
    else if (length == 1 && strncmp(pszSrc, "0", 1) == 0)
    {
      rResult = false;
      charactersConsumed = 1;
    }
    else if (length == 5 && strncmp(pszSrc, "false", 5) == 0)
    {
      rResult = false;
      charactersConsumed = 5;
    }
    else
    {
      throw FormatException("not a valid 'bool' string");
    }
    return charactersConsumed;
  }


  std::size_t StringParseUtil::Parse(uint8_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    const uint32_t result = ParseUInt32(psz, startIndex, length);
    if (result > std::numeric_limits<uint8_t>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = static_cast<uint8_t>(result);
    return length;
  }


  std::size_t StringParseUtil::Parse(int8_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    const int32_t result = ParseInt32(psz, startIndex, length);
    if (result < std::numeric_limits<int8_t>::min() || result > std::numeric_limits<int8_t>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = static_cast<int8_t>(result);
    return length;
  }


  std::size_t StringParseUtil::Parse(uint16_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    const uint32_t result = ParseUInt32(psz, startIndex, length);
    if (result > std::numeric_limits<uint16_t>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = static_cast<uint16_t>(result);
    return length;
  }


  std::size_t StringParseUtil::Parse(int16_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    const int32_t result = ParseInt32(psz, startIndex, length);
    if (result < std::numeric_limits<int16_t>::min() || result > std::numeric_limits<int16_t>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = static_cast<int16_t>(result);
    return length;
  }


  std::size_t StringParseUtil::Parse(uint32_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    rResult = ParseUInt32(psz, startIndex, length);
    return length;
  }


  std::size_t StringParseUtil::Parse(int32_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    rResult = ParseInt32(psz, startIndex, length);
    return length;
  }


  // int StringParseUtil::Parse(uint64_t& rResult, const char*const psz, const std::size_t startIndex, const std::size_t length)
  //{
  //  throw NotImplementedException();
  //}


  // int StringParseUtil::Parse(int64_t& rResult, const char*const psz, const std::size_t startIndex, const std::size_t length)
  //{
  //  throw NotImplementedException();
  //}


  std::size_t StringParseUtil::Parse(float& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    const double result = ParseDouble(psz, startIndex, length);
    if (result < std::numeric_limits<float>::lowest() || result > std::numeric_limits<float>::max())
    {
      throw OverflowException("overflow");
    }
    rResult = static_cast<float>(result);
    return length;
  }


  std::size_t StringParseUtil::Parse(double& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    rResult = ParseDouble(psz, startIndex, length);
    return length;
  }


  std::size_t StringParseUtil::Parse(Point2& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    int32_t values[2]{};
    StringParseArrayResult res = ParseArray(values, 2, psz, startIndex, length);
    if (res.ArrayEntries != 2)
    {
      throw FormatException("Point2 not in the correct format");
    }
    rResult = Point2(values[0], values[1]);
    return res.CharactersConsumed;
  }


  std::size_t StringParseUtil::Parse(Rectangle& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length)
  {
    int32_t values[4]{};
    StringParseArrayResult res = ParseArray(values, 4, psz, startIndex, length);
    if (res.ArrayEntries != 4)
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


  const StringParseArrayResult StringParseUtil::ParseArray(bool* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  const StringParseArrayResult StringParseUtil::ParseArray(uint8_t* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  const StringParseArrayResult StringParseUtil::ParseArray(int8_t* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  const StringParseArrayResult StringParseUtil::ParseArray(uint16_t* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  const StringParseArrayResult StringParseUtil::ParseArray(int16_t* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  const StringParseArrayResult StringParseUtil::ParseArray(uint32_t* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  const StringParseArrayResult StringParseUtil::ParseArray(int32_t* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  // const StringParseArrayResult StringParseUtil::ParseArray(uint64_t* pDst, const std::size_t dstLength, const char*const psz)
  //{
  // if (pDst == nullptr)
  //  throw std::invalid_argument("pDst can not be null");
  // if (psz == nullptr)
  //  throw std::invalid_argument("psz can not be null");
  // return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  //}


  // const StringParseArrayResult StringParseUtil::ParseArray(int64_t* pDst, const std::size_t dstLength, const char*const psz)
  //{
  // if (pDst == nullptr)
  //  throw std::invalid_argument("pDst can not be null");
  // if (psz == nullptr)
  //  throw std::invalid_argument("psz can not be null");
  // return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  //}


  const StringParseArrayResult StringParseUtil::ParseArray(float* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  const StringParseArrayResult StringParseUtil::ParseArray(double* pDst, const std::size_t dstLength, const char* const psz)
  {
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return ParseArray(pDst, dstLength, psz, 0, strlen(psz));
  }


  StringParseArrayResult StringParseUtil::ParseArray(bool* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  StringParseArrayResult StringParseUtil::ParseArray(uint8_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  StringParseArrayResult StringParseUtil::ParseArray(int8_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  StringParseArrayResult StringParseUtil::ParseArray(uint16_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  StringParseArrayResult StringParseUtil::ParseArray(int16_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  StringParseArrayResult StringParseUtil::ParseArray(uint32_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  StringParseArrayResult StringParseUtil::ParseArray(int32_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  // StringParseArrayResult StringParseUtil::ParseArray(uint64_t* pDst, const std::size_t dstLength, const char*const psz, const std::size_t
  // startIndex, const std::size_t length)
  //{
  // const int arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
  // return StringParseArrayResult(length, arrayEntries);
  //}


  // StringParseArrayResult StringParseUtil::ParseArray(int64_t* pDst, const std::size_t dstLength, const char*const psz, const std::size_t
  // startIndex, const std::size_t length)
  //{
  // const int arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
  // return StringParseArrayResult(length, arrayEntries);
  //}


  StringParseArrayResult StringParseUtil::ParseArray(float* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  StringParseArrayResult StringParseUtil::ParseArray(double* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                                     const std::size_t length)
  {
    const auto arrayEntries = DoParseArray(pDst, dstLength, psz, startIndex, length);
    return StringParseArrayResult(length, arrayEntries);
  }


  bool StringParseUtil::TryParse(int32_t& rValue, const char* const psz, const std::size_t startIndex, const std::size_t length, const int32_t radix)
  {
    rValue = 0;
    if (psz == nullptr)
    {
      FSLLOG_DEBUG_WARNING("psz can not be null");
      return false;
    }

    const auto actualLength = strlen(psz);
    const auto endIndex = startIndex + length;
    if (endIndex > actualLength)
    {
      FSLLOG_DEBUG_WARNING("startIndex or length out of bounds");
      return false;
    }
    if (length < 1)
    {
      FSLLOG_DEBUG_WARNING("length too small to contain a int32");
      return false;
    }

    // Check the string contains a digit as expected
    const char* pszSrc = psz + startIndex;

    bool ch1Extended = false;
    bool ch2Extended = false;
    if (radix > 10)
    {
      const int32_t radixAbove10 = radix - 10;
      ch1Extended = (pszSrc[0] >= 'a' && (pszSrc[0] - 'a') < radixAbove10) || (pszSrc[0] >= 'A' && (pszSrc[0] - 'A') < radixAbove10);
      ch2Extended = length >= 2 && ((pszSrc[1] >= 'a' && (pszSrc[1] - 'a') < radixAbove10) || (pszSrc[1] >= 'A' && (pszSrc[1] - 'A') < radixAbove10));
    }

    if ((pszSrc[0] >= '0' && (pszSrc[0] <= '9' || ch1Extended)) ||
        (length >= 2 && (pszSrc[0] == '+' || pszSrc[0] == '-') && (pszSrc[1] >= '0' && (pszSrc[1] <= '9' || ch2Extended))))
    {
      char* pEnd = nullptr;
      errno = 0;

      if (length >= 32)
      {
        FSLLOG_DEBUG_WARNING("string is too long");
        return false;
      }
      char tmpBuffer[32];
      for (std::size_t i = 0; i < length; ++i)
      {
        tmpBuffer[i] = psz[startIndex + i];
      }
      tmpBuffer[length] = 0;

      const auto value = strtol(tmpBuffer, &pEnd, radix);
      if (((value == LONG_MIN || value == LONG_MAX) && errno == ERANGE) || value < std::numeric_limits<int32_t>::min() ||
          value > std::numeric_limits<int32_t>::max())
      {
        return false;
      }

      if (pEnd != (tmpBuffer + length))
      {
        return false;
      }

      rValue = static_cast<int32_t>(value);
      return true;
    }
    return false;
  }

  std::size_t StringParseUtil::ParseTime(std::chrono::seconds& rResult, const char* const psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }

    // 012345678
    // HH:MM:SS
    const std::size_t expectedLength = 8;
    if (strlen(psz) != expectedLength || psz[2] != ':' || psz[5] != ':')
    {
      throw FormatException("time not in the expected HH:MM:SS format");
    }

    const uint32_t hh = ParseUInt32(psz, 0, 2);
    const uint32_t mm = ParseUInt32(psz, 3, 2);
    const uint32_t ss = ParseUInt32(psz, 6, 2);

    if (ss >= 60 || mm >= 60 || hh > 24)
    {
      throw FormatException("number not in the correct format");
    }

    rResult = std::chrono::seconds((hh * 60 * 60) + (mm * 60) + ss);
    return expectedLength;
  }
}
