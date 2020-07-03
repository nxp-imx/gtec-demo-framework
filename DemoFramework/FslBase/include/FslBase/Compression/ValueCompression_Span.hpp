#ifndef FSLBASE_COMPRESSION_VALUECOMPRESSION_SPAN_HPP
#define FSLBASE_COMPRESSION_VALUECOMPRESSION_SPAN_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/Compression/ValueCompression.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span.hpp>
#include <cassert>
#include <cstdlib>

namespace Fsl
{
  namespace ValueCompression
  {
    //! @brief Read a int32 from pSrc at starting at index
    inline int32_t ReadSimpleInt32(ReadOnlySpan<uint8_t>& rSrc)
    {
      int32_t result = 0;
      auto count = ValueCompression::ReadSimple(result, rSrc.data(), rSrc.size(), 0);
      rSrc = rSrc.subspan(count);
      return result;
    }

    //! @brief Read a uint32 from pSrc at starting at index
    inline uint32_t ReadSimpleUInt32(ReadOnlySpan<uint8_t>& rSrc)
    {
      uint32_t result = 0;
      auto count = ValueCompression::ReadSimple(result, rSrc.data(), rSrc.size(), 0);
      rSrc = rSrc.subspan(count);
      return result;
    }

    //! @brief Read a int16 from pSrc at starting at index
    inline int16_t ReadSimpleInt16(ReadOnlySpan<uint8_t>& rSrc)
    {
      auto result = ReadSimpleInt32(rSrc);
      return NumericCast<int16_t>(result);
    }

    //! @brief Read a uint16 from pSrc at starting at index
    inline uint16_t ReadSimpleUInt16(ReadOnlySpan<uint8_t>& rSrc)
    {
      auto result = ReadSimpleUInt32(rSrc);
      return NumericCast<uint16_t>(result);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Read a int32 from pSrc at starting at index
    //! @return the number of bytes that was read
    inline ReadOnlySpan<uint8_t>::size_type ReadSimple(int32_t& rResult, const ReadOnlySpan<uint8_t>& src)
    {
      return ValueCompression::ReadSimple(rResult, src.data(), src.size(), 0);
    }

    //! @brief Read a uint32 from pSrc at starting at index
    //! @return the number of bytes that was read
    inline ReadOnlySpan<uint8_t>::size_type ReadSimple(uint32_t& rResult, const ReadOnlySpan<uint8_t>& src)
    {
      return ValueCompression::ReadSimple(rResult, src.data(), src.size(), 0);
    }


    //! @brief Read a int32 from pSrc at starting at index
    //! @return the number of bytes that was read
    inline ReadOnlySpan<uint8_t>::size_type ReadSimple(int32_t& rResult, const ReadOnlySpan<uint8_t> src, const std::size_t index)
    {
      return ReadSimple(rResult, src.subspan(index));
    }

    //! @brief Read a uint32 from pSrc at starting at index
    //! @return the number of bytes that was read
    inline ReadOnlySpan<uint8_t>::size_type ReadSimple(uint32_t& rResult, const ReadOnlySpan<uint8_t> src, const std::size_t index)
    {
      return ReadSimple(rResult, src.subspan(index));
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Encodes a integer into a variable length encoding where the length can be determined from the first byte.
    //         The encoding favors small values.
    /// @return the number of bytes written
    inline ReadOnlySpan<uint8_t>::size_type WriteSimple(Span<uint8_t> dst, const std::size_t index, const int32_t value)
    {
      return ValueCompression::WriteSimple(dst.data(), dst.size(), index, value);
    }

    //! @brief Encodes a integer into a variable length encoding where the length can be determined from the first byte.
    //         The encoding favors small values.
    /// @return the number of bytes written
    inline ReadOnlySpan<uint8_t>::size_type WriteSimple(Span<uint8_t> dst, const std::size_t index, const uint32_t value)
    {
      return ValueCompression::WriteSimple(dst.data(), dst.size(), index, value);
    }

  };
}

#endif
