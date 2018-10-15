#ifndef FSLBASE_BITS_BYTEARRAYUTIL_HPP
#define FSLBASE_BITS_BYTEARRAYUTIL_HPP
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
#include <cstddef>

namespace Fsl
{
  class ByteArrayUtil
  {
  public:
    //! @brief Read a uint8_t from the given index in little endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static uint8_t ReadUInt8LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a int8_t from the given index in little endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static int8_t ReadInt8LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a uint16_t from the given index in little endian format
    static uint16_t ReadUInt16LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a int16_t from the given index in little endian format
    static int16_t ReadInt16LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a uint32_t from the given index in little endian format
    static uint32_t ReadUInt32LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a int32_t from the given index in little endian format
    static int32_t ReadInt32LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a uint64_t from the given index in little endian format
    static uint64_t ReadUInt64LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a int64_t from the given index in little endian format
    static int64_t ReadInt64LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);

    //! @brief Read a uint8_t from the given index in big endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static uint8_t ReadUInt8BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a int8_t from the given index in big endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static int8_t ReadInt8BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a uint16_t from the given index in big endian format
    static uint16_t ReadUInt16BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a int16_t from the given index in big endian format
    static int16_t ReadInt16BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a uint32_t from the given index in big endian format
    static uint32_t ReadUInt32BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a int32_t from the given index in big endian format
    static int32_t ReadInt32BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a uint64_t from the given index in big endian format
    static uint64_t ReadUInt64BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);
    //! @brief Read a int64_t from the given index in big endian format
    static int64_t ReadInt64BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);


    //! @brief Write a uint8_t to the given dstIndex in little endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static std::size_t WriteUInt8LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t value);
    //! @brief Write a int8_t to the given dstIndex in little endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static std::size_t WriteInt8LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int8_t value);
    //! @brief Write a uint16_t to the given dstIndex in little endian format
    static std::size_t WriteUInt16LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint16_t value);
    //! @brief Write a int16_t to the given dstIndex in little endian format
    static std::size_t WriteInt16LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int16_t value);
    //! @brief Write a uint32_t to the given dstIndex in little endian format
    static std::size_t WriteUInt32LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint32_t value);
    //! @brief Write a int32_t to the given dstIndex in little endian format
    static std::size_t WriteInt32LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int32_t value);
    //! @brief Write a uint64_t to the given dstIndex in little endian format
    static std::size_t WriteUInt64LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint64_t value);
    //! @brief Write a int64_t v the given dstIndex in little endian format
    static std::size_t WriteInt64LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int64_t value);

    //! @brief Write a uint8_t to the given dstIndex in big endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static std::size_t WriteUInt8BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t value);
    //! @brief Write a int8_t to the given dstIndex in big endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static std::size_t WriteInt8BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int8_t value);
    //! @brief Write a uint16_t to the given dstIndex in big endian format
    static std::size_t WriteUInt16BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint16_t value);
    //! @brief Write a int16_t to the given dstIndex in big endian format
    static std::size_t WriteInt16BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int16_t value);
    //! @brief Write a uint32_t to the given dstIndex in big endian format
    static std::size_t WriteUInt32BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint32_t value);
    //! @brief Write a int32_t to the given dstIndex in big endian format
    static std::size_t WriteInt32BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int32_t value);
    //! @brief Write a uint64_t to the given dstIndex in big endian format
    static std::size_t WriteUInt64BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint64_t value);
    //! @brief Read a int64_t to the given dstIndex in big endian format
    static std::size_t WriteInt64BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int64_t value);


    //! @brief Write a uint8_t to the given dstIndex in little endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static std::size_t WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t value);
    //! @brief Write a int8_t to the given dstIndex in little endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static std::size_t WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int8_t value);
    //! @brief Write a uint16_t to the given dstIndex in little endian format
    static std::size_t WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint16_t value);
    //! @brief Write a int16_t to the given dstIndex in little endian format
    static std::size_t WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int16_t value);
    //! @brief Write a uint32_t to the given dstIndex in little endian format
    static std::size_t WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint32_t value);
    //! @brief Write a int32_t to the given dstIndex in little endian format
    static std::size_t WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int32_t value);
    //! @brief Write a uint64_t to the given dstIndex in little endian format
    static std::size_t WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint64_t value);
    //! @brief Write a int64_t v the given dstIndex in little endian format
    static std::size_t WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int64_t value);

    //! @brief Write a uint8_t to the given dstIndex in big endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static std::size_t WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t value);
    //! @brief Write a int8_t to the given dstIndex in big endian format
    //! @note Yes this method does not make much sense, but its here to follow the style of the other methods and
    //!       it does some basic validation on the input params like the rest of the methods
    static std::size_t WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int8_t value);
    //! @brief Write a uint16_t to the given dstIndex in big endian format
    static std::size_t WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint16_t value);
    //! @brief Write a int16_t to the given dstIndex in big endian format
    static std::size_t WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int16_t value);
    //! @brief Write a uint32_t to the given dstIndex in big endian format
    static std::size_t WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint32_t value);
    //! @brief Write a int32_t to the given dstIndex in big endian format
    static std::size_t WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int32_t value);
    //! @brief Write a uint64_t to the given dstIndex in big endian format
    static std::size_t WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint64_t value);
    //! @brief Read a int64_t to the given dstIndex in big endian format
    static std::size_t WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int64_t value);

    //! @brief Write a the byte sequence into the given array
    static std::size_t WriteBytes(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t* pSrc,
                                  const std::size_t srcLength);
  };
}

#endif
