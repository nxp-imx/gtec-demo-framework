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

#include <FslBase/Bits/ByteArrayUtil.hpp>
#include <cassert>
#include <cstring>

namespace Fsl
{
  uint8_t ByteArrayUtil::ReadUInt8LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 1);
    assert(index < srcLength);
    return pSrc[index];
  }


  int8_t ByteArrayUtil::ReadInt8LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 1);
    assert(index < srcLength);
    return static_cast<int8_t>(pSrc[index]);
  }


  uint16_t ByteArrayUtil::ReadUInt16LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 2);
    assert((index + 1) < srcLength);

    return static_cast<uint16_t>(static_cast<uint16_t>(pSrc[index]) | (static_cast<uint16_t>(pSrc[index + 1]) << 8));
  }


  int16_t ByteArrayUtil::ReadInt16LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 2);
    assert((index + 1) < srcLength);

    return static_cast<int16_t>(static_cast<uint16_t>(pSrc[index]) | (static_cast<uint16_t>(pSrc[index + 1]) << 8));
  }


  uint32_t ByteArrayUtil::ReadUInt32LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 4);
    assert((index + 3) < srcLength);

    return static_cast<uint32_t>(static_cast<uint32_t>(pSrc[index]) | (static_cast<uint32_t>(pSrc[index + 1]) << 8) |
                                 (static_cast<uint32_t>(pSrc[index + 2]) << 16) | (static_cast<uint32_t>(pSrc[index + 3]) << 24));
  }


  int32_t ByteArrayUtil::ReadInt32LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 4);
    assert((index + 3) < srcLength);

    return static_cast<int32_t>(static_cast<uint32_t>(pSrc[index]) | (static_cast<uint32_t>(pSrc[index + 1]) << 8) |
                                (static_cast<uint32_t>(pSrc[index + 2]) << 16) | (static_cast<uint32_t>(pSrc[index + 3]) << 24));
  }


  uint64_t ByteArrayUtil::ReadUInt64LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 8);
    assert((index + 7) < srcLength);

    return static_cast<uint64_t>(static_cast<uint64_t>(pSrc[index]) | (static_cast<uint64_t>(pSrc[index + 1]) << 8) |
                                 (static_cast<uint64_t>(pSrc[index + 2]) << 16) | (static_cast<uint64_t>(pSrc[index + 3]) << 24) |
                                 (static_cast<uint64_t>(pSrc[index + 4]) << 32) | (static_cast<uint64_t>(pSrc[index + 5]) << 40) |
                                 (static_cast<uint64_t>(pSrc[index + 6]) << 48) | (static_cast<uint64_t>(pSrc[index + 7]) << 56));
  }

  int64_t ByteArrayUtil::ReadInt64LE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 8);
    assert((index + 7) < srcLength);

    return static_cast<int64_t>(static_cast<int64_t>(pSrc[index]) | (static_cast<int64_t>(pSrc[index + 1]) << 8) |
                                (static_cast<int64_t>(pSrc[index + 2]) << 16) | (static_cast<int64_t>(pSrc[index + 3]) << 24) |
                                (static_cast<int64_t>(pSrc[index + 4]) << 32) | (static_cast<int64_t>(pSrc[index + 5]) << 40) |
                                (static_cast<int64_t>(pSrc[index + 6]) << 48) | (static_cast<int64_t>(pSrc[index + 7]) << 56));
  }


  uint8_t ByteArrayUtil::ReadUInt8BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 1);
    assert(index < srcLength);

    return pSrc[index];
  }


  int8_t ByteArrayUtil::ReadInt8BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 1);
    assert(index < srcLength);

    return static_cast<int8_t>(pSrc[index]);
  }


  uint16_t ByteArrayUtil::ReadUInt16BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 2);
    assert((index + 1) < srcLength);


    return static_cast<uint16_t>(static_cast<uint16_t>(pSrc[index + 1]) | (static_cast<uint16_t>(pSrc[index]) << 8));
  }


  int16_t ByteArrayUtil::ReadInt16BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 2);
    assert((index + 1) < srcLength);

    return static_cast<int16_t>(static_cast<uint16_t>(pSrc[index + 1]) | (static_cast<uint16_t>(pSrc[index]) << 8));
  }


  uint32_t ByteArrayUtil::ReadUInt32BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 4);
    assert((index + 3) < srcLength);


    return static_cast<uint32_t>(static_cast<uint32_t>(pSrc[index + 3]) | (static_cast<uint32_t>(pSrc[index + 2]) << 8) |
                                 (static_cast<uint32_t>(pSrc[index + 1]) << 16) | (static_cast<uint32_t>(pSrc[index]) << 24));
  }


  int32_t ByteArrayUtil::ReadInt32BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 4);
    assert((index + 3) < srcLength);


    return static_cast<int32_t>(static_cast<uint32_t>(pSrc[index + 3]) | (static_cast<uint32_t>(pSrc[index + 2]) << 8) |
                                (static_cast<uint32_t>(pSrc[index + 1]) << 16) | (static_cast<uint32_t>(pSrc[index]) << 24));
  }


  uint64_t ByteArrayUtil::ReadUInt64BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 8);
    assert((index + 7) < srcLength);

    return static_cast<uint64_t>(static_cast<uint64_t>(pSrc[index + 7]) | (static_cast<uint64_t>(pSrc[index + 6]) << 8) |
                                 (static_cast<uint64_t>(pSrc[index + 5]) << 16) | (static_cast<uint64_t>(pSrc[index + 4]) << 24) |
                                 (static_cast<uint64_t>(pSrc[index + 3]) << 32) | (static_cast<uint64_t>(pSrc[index + 2]) << 40) |
                                 (static_cast<uint64_t>(pSrc[index + 1]) << 48) | (static_cast<uint64_t>(pSrc[index + 0]) << 56));
  }


  int64_t ByteArrayUtil::ReadInt64BE(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(srcLength >= 8);
    assert((index + 7) < srcLength);

    return static_cast<int64_t>(static_cast<int64_t>(pSrc[index + 7]) | (static_cast<int64_t>(pSrc[index + 6]) << 8) |
                                (static_cast<int64_t>(pSrc[index + 5]) << 16) | (static_cast<int64_t>(pSrc[index + 4]) << 24) |
                                (static_cast<int64_t>(pSrc[index + 3]) << 32) | (static_cast<int64_t>(pSrc[index + 2]) << 40) |
                                (static_cast<int64_t>(pSrc[index + 1]) << 48) | (static_cast<int64_t>(pSrc[index + 0]) << 56));
  }


  std::size_t ByteArrayUtil::WriteUInt8LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 1);
    assert(dstIndex < dstLength);

    pDst[dstIndex] = value;
    return 1;
  }


  std::size_t ByteArrayUtil::WriteInt8LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int8_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 1);
    assert(dstIndex < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value);
    return 1;
  }


  std::size_t ByteArrayUtil::WriteUInt16LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint16_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 2);
    assert((dstIndex + 1) < dstLength);


    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    return 2;
  }


  std::size_t ByteArrayUtil::WriteInt16LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int16_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 2);
    assert((dstIndex + 1) < dstLength);


    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    return 2;
  }


  std::size_t ByteArrayUtil::WriteUInt32LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint32_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 4);
    assert((dstIndex + 3) < dstLength);


    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    return 4;
  }


  std::size_t ByteArrayUtil::WriteInt32LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int32_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 4);
    assert((dstIndex + 3) < dstLength);


    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    return 4;
  }


  std::size_t ByteArrayUtil::WriteUInt64LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint64_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 8);
    assert((dstIndex + 7) < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 4] = static_cast<uint8_t>((value >> 32) & 0xFF);
    pDst[dstIndex + 5] = static_cast<uint8_t>((value >> 40) & 0xFF);
    pDst[dstIndex + 6] = static_cast<uint8_t>((value >> 48) & 0xFF);
    pDst[dstIndex + 7] = static_cast<uint8_t>((value >> 56) & 0xFF);
    return 8;
  }


  std::size_t ByteArrayUtil::WriteInt64LE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int64_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 8);
    assert((dstIndex + 7) < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 4] = static_cast<uint8_t>((value >> 32) & 0xFF);
    pDst[dstIndex + 5] = static_cast<uint8_t>((value >> 40) & 0xFF);
    pDst[dstIndex + 6] = static_cast<uint8_t>((value >> 48) & 0xFF);
    pDst[dstIndex + 7] = static_cast<uint8_t>((value >> 56) & 0xFF);
    return 8;
  }


  std::size_t ByteArrayUtil::WriteUInt8BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 1);
    assert(dstIndex < dstLength);

    pDst[dstIndex] = value;
    return 1;
  }


  std::size_t ByteArrayUtil::WriteInt8BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int8_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 1);
    assert(dstIndex < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value);
    return 1;
  }


  std::size_t ByteArrayUtil::WriteUInt16BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint16_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 2);
    assert((dstIndex + 1) < dstLength);


    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value)&0xFF);
    return 2;
  }


  std::size_t ByteArrayUtil::WriteInt16BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int16_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 2);
    assert((dstIndex + 1) < dstLength);


    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value)&0xFF);
    return 2;
  }


  std::size_t ByteArrayUtil::WriteUInt32BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint32_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 4);
    assert((dstIndex + 3) < dstLength);


    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value)&0xFF);
    return 4;
  }


  std::size_t ByteArrayUtil::WriteInt32BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int32_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 4);
    assert((dstIndex + 3) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value)&0xFF);
    return 4;
  }


  std::size_t ByteArrayUtil::WriteUInt64BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint64_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 8);
    assert((dstIndex + 7) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 56) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 48) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 40) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 32) & 0xFF);
    pDst[dstIndex + 4] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 5] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 6] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 7] = static_cast<uint8_t>((value)&0xFF);
    return 8;
  }


  std::size_t ByteArrayUtil::WriteInt64BE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int64_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 8);
    assert((dstIndex + 7) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 56) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 48) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 40) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 32) & 0xFF);
    pDst[dstIndex + 4] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 5] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 6] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 7] = static_cast<uint8_t>((value)&0xFF);
    return 8;
  }


  std::size_t ByteArrayUtil::WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 1);
    assert(dstIndex < dstLength);

    pDst[dstIndex] = value;
    return 1;
  }


  std::size_t ByteArrayUtil::WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int8_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 1);
    assert(dstIndex < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value);
    return 1;
  }


  std::size_t ByteArrayUtil::WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint16_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 2);
    assert((dstIndex + 1) < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    return 2;
  }


  std::size_t ByteArrayUtil::WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int16_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 2);
    assert((dstIndex + 1) < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    return 2;
  }


  std::size_t ByteArrayUtil::WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint32_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 4);
    assert((dstIndex + 3) < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    return 4;
  }


  std::size_t ByteArrayUtil::WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int32_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 4);
    assert((dstIndex + 3) < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    return 4;
  }


  std::size_t ByteArrayUtil::WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint64_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 8);
    assert((dstIndex + 7) < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 4] = static_cast<uint8_t>((value >> 32) & 0xFF);
    pDst[dstIndex + 5] = static_cast<uint8_t>((value >> 40) & 0xFF);
    pDst[dstIndex + 6] = static_cast<uint8_t>((value >> 48) & 0xFF);
    pDst[dstIndex + 7] = static_cast<uint8_t>((value >> 56) & 0xFF);
    return 8;
  }


  std::size_t ByteArrayUtil::WriteLE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int64_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 8);
    assert((dstIndex + 7) < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 4] = static_cast<uint8_t>((value >> 32) & 0xFF);
    pDst[dstIndex + 5] = static_cast<uint8_t>((value >> 40) & 0xFF);
    pDst[dstIndex + 6] = static_cast<uint8_t>((value >> 48) & 0xFF);
    pDst[dstIndex + 7] = static_cast<uint8_t>((value >> 56) & 0xFF);
    return 8;
  }


  std::size_t ByteArrayUtil::WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 1);
    assert(dstIndex < dstLength);

    pDst[dstIndex] = value;
    return 1;
  }


  std::size_t ByteArrayUtil::WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int8_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 1);
    assert(dstIndex < dstLength);

    pDst[dstIndex] = static_cast<uint8_t>(value);
    return 1;
  }


  std::size_t ByteArrayUtil::WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint16_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 2);
    assert((dstIndex + 1) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value)&0xFF);
    return 2;
  }


  std::size_t ByteArrayUtil::WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int16_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 2);
    assert((dstIndex + 1) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value)&0xFF);
    return 2;
  }


  std::size_t ByteArrayUtil::WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint32_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 4);
    assert((dstIndex + 3) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value)&0xFF);
    return 4;
  }


  std::size_t ByteArrayUtil::WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int32_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 4);
    assert((dstIndex + 3) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value)&0xFF);
    return 4;
  }


  std::size_t ByteArrayUtil::WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint64_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 8);
    assert((dstIndex + 7) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 56) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 48) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 40) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 32) & 0xFF);
    pDst[dstIndex + 4] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 5] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 6] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 7] = static_cast<uint8_t>((value)&0xFF);
    return 8;
  }


  std::size_t ByteArrayUtil::WriteBE(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const int64_t value)
  {
    assert(pDst != nullptr);
    assert(dstLength >= 8);
    assert((dstIndex + 7) < dstLength);

    pDst[dstIndex + 0] = static_cast<uint8_t>((value >> 56) & 0xFF);
    pDst[dstIndex + 1] = static_cast<uint8_t>((value >> 48) & 0xFF);
    pDst[dstIndex + 2] = static_cast<uint8_t>((value >> 40) & 0xFF);
    pDst[dstIndex + 3] = static_cast<uint8_t>((value >> 32) & 0xFF);
    pDst[dstIndex + 4] = static_cast<uint8_t>((value >> 24) & 0xFF);
    pDst[dstIndex + 5] = static_cast<uint8_t>((value >> 16) & 0xFF);
    pDst[dstIndex + 6] = static_cast<uint8_t>((value >> 8) & 0xFF);
    pDst[dstIndex + 7] = static_cast<uint8_t>((value)&0xFF);
    return 8;
  }

  std::size_t ByteArrayUtil::WriteBytes(uint8_t* pDst, const std::size_t dstLength, const std::size_t dstIndex, const uint8_t* pSrc,
                                        const std::size_t srcLength)
  {
    assert(pDst != nullptr);
    assert(pSrc != nullptr);
    assert(dstLength >= srcLength);
    assert((dstIndex + srcLength) <= dstLength);
    std::memcpy(pDst + dstIndex, pSrc, srcLength);
    return srcLength;
  }
}
