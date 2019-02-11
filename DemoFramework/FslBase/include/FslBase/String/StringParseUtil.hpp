#ifndef FSLBASE_STRING_STRINGPARSEUTIL_HPP
#define FSLBASE_STRING_STRINGPARSEUTIL_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/String/StringParseArrayResult.hpp>
#include <chrono>

namespace Fsl
{
  //! @brief This is a work in progress class
  class StringParseUtil
  {
  public:
    //! @brief Parse the input string into the correct type
    //! @return the number of characters that was consumed from psz
    static std::size_t Parse(bool& rResult, const char* const psz);
    static std::size_t Parse(uint8_t& rResult, const char* const psz);
    static std::size_t Parse(int8_t& rResult, const char* const psz);
    static std::size_t Parse(uint16_t& rResult, const char* const psz);
    static std::size_t Parse(int16_t& rResult, const char* const psz);
    static std::size_t Parse(uint32_t& rResult, const char* const psz);
    static std::size_t Parse(int32_t& rResult, const char* const psz);
    // static std::size_t Parse(uint64_t& rResult, const char*const psz);
    // static std::size_t Parse(int64_t& rResult, const char*const psz);
    static std::size_t Parse(float& rResult, const char* const psz);
    static std::size_t Parse(double& rResult, const char* const psz);
    static std::size_t Parse(Point2& rResult, const char* const psz);
    static std::size_t Parse(Rectangle& rResult, const char* const psz);

    //! @brief Parse the input string into the correct type
    //! @param startIndex the start index in psz to start scanning from.
    //! @param length the number of characters that should be considered part of the psz string (from startIndex)
    //! @return the number of characters that was consumed from psz
    static std::size_t Parse(bool& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(uint8_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(int8_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(uint16_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(int16_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(uint32_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(int32_t& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    // static std::size_t Parse(uint64_t& rResult, const char*const psz, const std::size_t startIndex, const std::size_t length);
    // static std::size_t Parse(int64_t& rResult, const char*const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(float& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(double& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(Point2& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(Rectangle& rResult, const char* const psz, const std::size_t startIndex, const std::size_t length);

    //! @brief Parse the input string into the correct type
    //! @param pDst the destination array.
    //! @param dstLength the length of the pDst array.
    //! @param psz the input string that should be parsed
    //! @return the number of characters that was consumed from psz
    static const StringParseArrayResult ParseArray(bool* pDst, const std::size_t dstLength, const char* const psz);
    static const StringParseArrayResult ParseArray(uint8_t* pDst, const std::size_t dstLength, const char* const psz);
    static const StringParseArrayResult ParseArray(int8_t* pDst, const std::size_t dstLength, const char* const psz);
    static const StringParseArrayResult ParseArray(uint16_t* pDst, const std::size_t dstLength, const char* const psz);
    static const StringParseArrayResult ParseArray(int16_t* pDst, const std::size_t dstLength, const char* const psz);
    static const StringParseArrayResult ParseArray(uint32_t* pDst, const std::size_t dstLength, const char* const psz);
    static const StringParseArrayResult ParseArray(int32_t* pDst, const std::size_t dstLength, const char* const psz);
    // static const StringParseArrayResult ParseArray(uint64_t* pDst, const std::size_t dstLength, const char*const psz);
    // static const StringParseArrayResult ParseArray(int64_t* pDst, const std::size_t dstLength, const char*const psz);
    static const StringParseArrayResult ParseArray(float* pDst, const std::size_t dstLength, const char* const psz);
    static const StringParseArrayResult ParseArray(double* pDst, const std::size_t dstLength, const char* const psz);

    //! @brief Parse the input string into the correct type
    //! @param pDst the destination array.
    //! @param dstLength the length of the pDst array.
    //! @param psz the input string that should be parsed
    //! @param startIndex the start index in psz to start scanning from.
    //! @param length the number of characters that should be considered part of the psz string (from startIndex)
    //! @return the result of the parse
    static StringParseArrayResult ParseArray(bool* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(uint8_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(int8_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(uint16_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(int16_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(uint32_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(int32_t* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);
    // static StringParseArrayResult ParseArray(uint64_t* pDst, const std::size_t dstLength, const char*const psz, const std::size_t startIndex, const
    // std::size_t length);  static StringParseArrayResult ParseArray(int64_t* pDst, const std::size_t dstLength, const char*const psz, const
    // std::size_t startIndex, const std::size_t length);
    static StringParseArrayResult ParseArray(float* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(double* pDst, const std::size_t dstLength, const char* const psz, const std::size_t startIndex,
                                             const std::size_t length);

    static bool TryParse(int32_t& rValue, const char* const psz, const std::size_t startIndex, const std::size_t length, const int32_t radix = 10);

    //! @brief Parse a time string in the format HH:MM:SS and return the result in seconds
    static std::size_t ParseTime(std::chrono::seconds& rResult, const char* const psz);
  };
}

#endif
