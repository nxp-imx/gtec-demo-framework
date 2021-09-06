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
#include <FslBase/Math/Point2U.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/String/StringParseArrayResult.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <chrono>

namespace Fsl
{
  //! @brief This is a work in progress class
  class StringParseUtil
  {
  public:
    //! @brief Parse the input string into the correct type
    //! @return the number of characters that was consumed from psz
    static std::size_t Parse(bool& rResult, const StringViewLite strView);
    static std::size_t Parse(uint8_t& rResult, const StringViewLite strView);
    static std::size_t Parse(int8_t& rResult, const StringViewLite strView);
    static std::size_t Parse(uint16_t& rResult, const StringViewLite strView);
    static std::size_t Parse(int16_t& rResult, const StringViewLite strView);
    static std::size_t Parse(uint32_t& rResult, const StringViewLite strView);
    static std::size_t Parse(int32_t& rResult, const StringViewLite strView);
    // static std::size_t Parse(uint64_t& rResult, const StringViewLite strView);
    // static std::size_t Parse(int64_t& rResult, const StringViewLite strView);
    static std::size_t Parse(float& rResult, const StringViewLite strView);
    static std::size_t Parse(double& rResult, const StringViewLite strView);
    static std::size_t Parse(Point2& rResult, const StringViewLite strView);
    static std::size_t Parse(Point2U& rResult, const StringViewLite strView);
    static std::size_t Parse(PxSize2D& rResult, const StringViewLite strView);
    static std::size_t Parse(Rectangle& rResult, const StringViewLite strView);

    //! @brief Parse the input string into the correct type
    //! @param startIndex the start index in psz to start scanning from.
    //! @param length the number of characters that should be considered part of the psz string (from startIndex)
    //! @return the number of characters that was consumed from psz
    inline static std::size_t Parse(bool& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);

    static std::size_t Parse(uint8_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(int8_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(uint16_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(int16_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(uint32_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(int32_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    // static std::size_t Parse(uint64_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    // static std::size_t Parse(int64_t& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(float& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(double& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(Point2& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(Point2U& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static std::size_t Parse(Rectangle& rResult, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);

    static StringParseArrayResult ParseArray(Span<bool> dst, const StringViewLite strView);
    static StringParseArrayResult ParseArray(Span<uint8_t> dst, const StringViewLite strView);
    static StringParseArrayResult ParseArray(Span<int8_t> dst, const StringViewLite strView);
    static StringParseArrayResult ParseArray(Span<uint16_t> dst, const StringViewLite strView);
    static StringParseArrayResult ParseArray(Span<int16_t> dst, const StringViewLite strView);
    static StringParseArrayResult ParseArray(Span<uint32_t> dst, const StringViewLite strView);
    static StringParseArrayResult ParseArray(Span<int32_t> dst, const StringViewLite strView);
    // static StringParseArrayResult ParseArray(Span<uint64_t> dst, const StringViewLite strView);
    // static StringParseArrayResult ParseArray(Span<int64_t> dst, const StringViewLite strView);
    static StringParseArrayResult ParseArray(Span<float> dst, const StringViewLite strView);
    static StringParseArrayResult ParseArray(Span<double> dst, const StringViewLite strView);


    static StringParseArrayResult ParseArray(Span<bool> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static StringParseArrayResult ParseArray(Span<uint8_t> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static StringParseArrayResult ParseArray(Span<int8_t> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static StringParseArrayResult ParseArray(Span<uint16_t> dst, const StringViewLite strView, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(Span<int16_t> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static StringParseArrayResult ParseArray(Span<uint32_t> dst, const StringViewLite strView, const std::size_t startIndex,
                                             const std::size_t length);
    static StringParseArrayResult ParseArray(Span<int32_t> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    // static StringParseArrayResult ParseArray(Span<uint64_t> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t
    // length); static StringParseArrayResult ParseArray(Span<int64_t> dst, const StringViewLite strView, const std::size_t startIndex, const
    // std::size_t length);
    static StringParseArrayResult ParseArray(Span<float> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);
    static StringParseArrayResult ParseArray(Span<double> dst, const StringViewLite strView, const std::size_t startIndex, const std::size_t length);


    static bool TryParse(int32_t& rValue, const StringViewLite strView, const int32_t radix = 10);
    static bool TryParse(int32_t& rValue, const StringViewLite strView, const std::size_t startIndex, const std::size_t length,
                         const int32_t radix = 10);

    //! @brief Parse a time string in the format HH:MM:SS and return the result in seconds
    static std::size_t ParseTime(std::chrono::seconds& rResult, const StringViewLite strView);

    // Replaced
    // static StringParseArrayResult ParseArray(<type>* pDst, const std::size_t dstLength, ...) with a Span<type> variant instead
  };
}

#endif
