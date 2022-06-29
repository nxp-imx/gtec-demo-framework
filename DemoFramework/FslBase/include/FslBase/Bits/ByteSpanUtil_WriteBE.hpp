#ifndef FSLBASE_BITS_BYTESPANUTIL_WRITEBE_HPP
#define FSLBASE_BITS_BYTESPANUTIL_WRITEBE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/Span/Span.hpp>
#include <cassert>

namespace Fsl::ByteSpanUtil
{
  //! @brief Write a uint8_t to the given span in big endian format
  //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
  //!       it does some basic validation on the input params like the rest of the methods
  constexpr inline Span<uint8_t>::size_type WriteUInt8BE(Span<uint8_t> dst, const uint8_t value)
  {
    assert(dst.length() >= 1);

    dst[0] = value;
    return 1;
  }

  //! @brief Write a int8_t to the given span in big endian format
  //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
  //!       it does some basic validation on the input params like the rest of the methods
  constexpr inline Span<uint8_t>::size_type WriteInt8BE(Span<uint8_t> dst, const int8_t value)
  {
    assert(dst.length() >= 1);

    dst[0] = static_cast<uint8_t>(value);
    return 1;
  }

  //! @brief Write a uint16_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteUInt16BE(Span<uint8_t> dst, const uint16_t value)
  {
    assert(dst.length() >= 2);


    dst[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[1] = static_cast<uint8_t>((value)&0xFF);
    return 2;
  }

  //! @brief Write a int16_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteInt16BE(Span<uint8_t> dst, const int16_t value)
  {
    assert(dst.length() >= 2);

    dst[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[1] = static_cast<uint8_t>((value)&0xFF);
    return 2;
  }

  //! @brief Write a uint32_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteUInt32BE(Span<uint8_t> dst, const uint32_t value)
  {
    assert(dst.length() >= 4);

    dst[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    dst[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    dst[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[3] = static_cast<uint8_t>((value)&0xFF);
    return 4;
  }

  //! @brief Write a int32_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteInt32BE(Span<uint8_t> dst, const int32_t value)
  {
    assert(dst.length() >= 4);

    dst[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    dst[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    dst[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[3] = static_cast<uint8_t>((value)&0xFF);
    return 4;
  }

  //! @brief Write a uint64_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteUInt64BE(Span<uint8_t> dst, const uint64_t value)
  {
    assert(dst.length() >= 8);

    dst[0] = static_cast<uint8_t>((value >> 56) & 0xFF);
    dst[1] = static_cast<uint8_t>((value >> 48) & 0xFF);
    dst[2] = static_cast<uint8_t>((value >> 40) & 0xFF);
    dst[3] = static_cast<uint8_t>((value >> 32) & 0xFF);
    dst[4] = static_cast<uint8_t>((value >> 24) & 0xFF);
    dst[5] = static_cast<uint8_t>((value >> 16) & 0xFF);
    dst[6] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[7] = static_cast<uint8_t>((value)&0xFF);
    return 8;
  }

  //! @brief Read a int64_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteInt64BE(Span<uint8_t> dst, const int64_t value)
  {
    assert(dst.length() >= 8);

    dst[0] = static_cast<uint8_t>((value >> 56) & 0xFF);
    dst[1] = static_cast<uint8_t>((value >> 48) & 0xFF);
    dst[2] = static_cast<uint8_t>((value >> 40) & 0xFF);
    dst[3] = static_cast<uint8_t>((value >> 32) & 0xFF);
    dst[4] = static_cast<uint8_t>((value >> 24) & 0xFF);
    dst[5] = static_cast<uint8_t>((value >> 16) & 0xFF);
    dst[6] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[7] = static_cast<uint8_t>((value)&0xFF);
    return 8;
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief Write a uint8_t to the given dstIndex in big endian format
  //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
  //!       it does some basic validation on the input params like the rest of the methods
  constexpr inline Span<uint8_t>::size_type WriteUInt8BE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const uint8_t value)
  {
    assert(dstIndex < dst.length());
    return WriteUInt8BE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a int8_t to the given dstIndex in big endian format
  //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
  //!       it does some basic validation on the input params like the rest of the methods
  constexpr inline Span<uint8_t>::size_type WriteInt8BE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const int8_t value)
  {
    assert(dstIndex < dst.length());
    return WriteInt8BE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a uint16_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteUInt16BE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const uint16_t value)
  {
    assert(dstIndex < dst.length());
    return WriteUInt16BE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a int16_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteInt16BE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const int16_t value)
  {
    assert(dstIndex < dst.length());
    return WriteInt16BE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a uint32_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteUInt32BE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const uint32_t value)
  {
    assert(dstIndex < dst.length());
    return WriteUInt32BE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a int32_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteInt32BE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const int32_t value)
  {
    assert(dstIndex < dst.length());
    return WriteInt32BE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a uint64_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteUInt64BE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const uint64_t value)
  {
    assert(dstIndex < dst.length());
    return WriteUInt64BE(dst.subspan(dstIndex), value);
  }

  //! @brief Read a int64_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteInt64BE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const int64_t value)
  {
    assert(dstIndex < dst.length());
    return WriteInt64BE(dst.subspan(dstIndex), value);
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // WriteBE
  // -----------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief Write a uint8_t to the given span in big endian format
  //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
  //!       it does some basic validation on the input params like the rest of the methods
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const uint8_t value)
  {
    assert(dst.length() >= 1);

    dst[0] = value;
    return 1;
  }

  //! @brief Write a int8_t to the given span in big endian format
  //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
  //!       it does some basic validation on the input params like the rest of the methods
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const int8_t value)
  {
    assert(dst.length() >= 1);

    dst[0] = static_cast<uint8_t>(value);
    return 1;
  }

  //! @brief Write a uint16_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const uint16_t value)
  {
    assert(dst.length() >= 2);

    dst[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[1] = static_cast<uint8_t>((value)&0xFF);
    return 2;
  }

  //! @brief Write a int16_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const int16_t value)
  {
    assert(dst.length() >= 2);

    dst[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[1] = static_cast<uint8_t>((value)&0xFF);
    return 2;
  }

  //! @brief Write a uint32_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const uint32_t value)
  {
    assert(dst.length() >= 4);

    dst[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    dst[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    dst[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[3] = static_cast<uint8_t>((value)&0xFF);
    return 4;
  }

  //! @brief Write a int32_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const int32_t value)
  {
    assert(dst.length() >= 4);

    dst[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    dst[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    dst[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[3] = static_cast<uint8_t>((value)&0xFF);
    return 4;
  }

  //! @brief Write a uint64_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const uint64_t value)
  {
    assert(dst.length() >= 8);

    dst[0] = static_cast<uint8_t>((value >> 56) & 0xFF);
    dst[1] = static_cast<uint8_t>((value >> 48) & 0xFF);
    dst[2] = static_cast<uint8_t>((value >> 40) & 0xFF);
    dst[3] = static_cast<uint8_t>((value >> 32) & 0xFF);
    dst[4] = static_cast<uint8_t>((value >> 24) & 0xFF);
    dst[5] = static_cast<uint8_t>((value >> 16) & 0xFF);
    dst[6] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[7] = static_cast<uint8_t>((value)&0xFF);
    return 8;
  }

  //! @brief Read a int64_t to the given span in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const int64_t value)
  {
    assert(dst.length() >= 8);

    dst[0] = static_cast<uint8_t>((value >> 56) & 0xFF);
    dst[1] = static_cast<uint8_t>((value >> 48) & 0xFF);
    dst[2] = static_cast<uint8_t>((value >> 40) & 0xFF);
    dst[3] = static_cast<uint8_t>((value >> 32) & 0xFF);
    dst[4] = static_cast<uint8_t>((value >> 24) & 0xFF);
    dst[5] = static_cast<uint8_t>((value >> 16) & 0xFF);
    dst[6] = static_cast<uint8_t>((value >> 8) & 0xFF);
    dst[7] = static_cast<uint8_t>((value)&0xFF);
    return 8;
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief Write a uint8_t to the given dstIndex in big endian format
  //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
  //!       it does some basic validation on the input params like the rest of the methods
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const uint8_t value)
  {
    assert(dstIndex < dst.length());
    return WriteBE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a int8_t to the given dstIndex in big endian format
  //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
  //!       it does some basic validation on the input params like the rest of the methods
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const int8_t value)
  {
    assert(dstIndex < dst.length());
    return WriteBE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a uint16_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const uint16_t value)
  {
    assert(dstIndex < dst.length());
    return WriteBE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a int16_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const int16_t value)
  {
    assert(dstIndex < dst.length());
    return WriteBE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a uint32_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const uint32_t value)
  {
    assert(dstIndex < dst.length());
    return WriteBE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a int32_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const int32_t value)
  {
    assert(dstIndex < dst.length());
    return WriteBE(dst.subspan(dstIndex), value);
  }

  //! @brief Write a uint64_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const uint64_t value)
  {
    assert(dstIndex < dst.length());
    return WriteBE(dst.subspan(dstIndex), value);
  }

  //! @brief Read a int64_t to the given dstIndex in big endian format
  constexpr inline Span<uint8_t>::size_type WriteBE(Span<uint8_t> dst, const Span<uint8_t>::size_type dstIndex, const int64_t value)
  {
    assert(dstIndex < dst.length());
    return WriteBE(dst.subspan(dstIndex), value);
  }
}

#endif
