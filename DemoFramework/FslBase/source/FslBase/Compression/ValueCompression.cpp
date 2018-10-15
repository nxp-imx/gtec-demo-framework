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
#include <cassert>

namespace Fsl
{
  int32_t ValueCompression::ReadSimpleInt32(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    uint32_t value = ReadSimpleUInt32(pSrc, srcLength, index);
    return static_cast<int32_t>((value >> 1) ^ ((~0) + ((~value) & 1)));
  }


  uint32_t ValueCompression::ReadSimpleUInt32(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(index < srcLength);
    uint32_t value = pSrc[index];
    if ((value & 0x80) == 0)
    {
      return value;
    }
    if ((value & 0x40) == 0)
    {
      assert((index + 1) < srcLength);
      return (value & 0x3F) | (static_cast<uint32_t>(pSrc[index + 1]) << 6);
    }
    if ((value & 0x20) == 0)
    {
      assert((index + 2) < srcLength);
      return (value & 0x1F) | (static_cast<uint32_t>(pSrc[index + 1]) << 5) | (static_cast<uint32_t>(pSrc[index + 2]) << 13);
    }
    if ((value & 0x10) == 0)
    {
      assert((index + 3) < srcLength);
      return (value & 0x0F) | (static_cast<uint32_t>(pSrc[index + 1]) << 4) | (static_cast<uint32_t>(pSrc[index + 2]) << 12) |
             (static_cast<uint32_t>(pSrc[index + 3]) << 20);
    }

    assert((index + 4) < srcLength);
    return (value & 0x07) | (static_cast<uint32_t>(pSrc[index + 1]) << 3) | (static_cast<uint32_t>(pSrc[index + 2]) << 11) |
           (static_cast<uint32_t>(pSrc[index + 3]) << 19) | (static_cast<uint32_t>(pSrc[index + 4]) << 27);
  }


  std::size_t ValueCompression::ReadSimple(int32_t& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    uint32_t value;
    const auto result = ReadSimple(value, pSrc, srcLength, index);

    rResult = static_cast<int32_t>((value >> 1) ^ ((~0) + ((~value) & 1)));
    assert(rResult == static_cast<int32_t>((value & 1) == 0 ? (value >> 1) : ((value >> 1) ^ (~0))));
    return result;
  }


  std::size_t ValueCompression::ReadSimple(uint32_t& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    assert(index < srcLength);
    auto currentIndex = index;
    rResult = pSrc[currentIndex];
    if ((rResult & 0x80) == 0)
    {
      ++currentIndex;
    }
    else if ((rResult & 0x40) == 0)
    {
      assert((index + 1) < srcLength);
      rResult = (rResult & 0x3F) | (static_cast<uint32_t>(pSrc[currentIndex + 1]) << 6);
      currentIndex += 2;
    }
    else if ((rResult & 0x20) == 0)
    {
      assert((index + 2) < srcLength);
      rResult = (rResult & 0x1F) | (static_cast<uint32_t>(pSrc[currentIndex + 1]) << 5) | (static_cast<uint32_t>(pSrc[currentIndex + 2]) << 13);
      currentIndex += 3;
    }
    else if ((rResult & 0x10) == 0)
    {
      assert((index + 3) < srcLength);
      rResult = (rResult & 0x0F) | (static_cast<uint32_t>(pSrc[currentIndex + 1]) << 4) | (static_cast<uint32_t>(pSrc[currentIndex + 2]) << 12) |
                (static_cast<uint32_t>(pSrc[currentIndex + 3]) << 20);
      currentIndex += 4;
    }
    else
    {
      assert((index + 4) < srcLength);
      rResult = (rResult & 0x07) | (static_cast<uint32_t>(pSrc[currentIndex + 1]) << 3) | (static_cast<uint32_t>(pSrc[currentIndex + 2]) << 11) |
                (static_cast<uint32_t>(pSrc[currentIndex + 3]) << 19) | (static_cast<uint32_t>(pSrc[currentIndex + 4]) << 27);
      currentIndex += 5;
    }
    return currentIndex - index;
  }


  std::size_t ValueCompression::WriteSimple(uint8_t* const pDst, const std::size_t dstLength, const std::size_t index, const int32_t value)
  {
    // ZigZag encode signed numbers
    // While this "(value << 1) ^ (value >> 31)" might be more optimal its very dependent on how the language handles sign extension when downshifting
    // so a 'safe' implementation was chosen
    return WriteSimple(pDst, dstLength, index, static_cast<uint32_t>(value >= 0 ? (value << 1) : ((value << 1) ^ (~0))));
  }


  std::size_t ValueCompression::WriteSimple(uint8_t* const pDst, const std::size_t dstLength, const std::size_t index, const uint32_t value)
  {
    assert(pDst != nullptr);
    if (value <= 0x7F)
    {
      assert(index < dstLength);
      // <=7 bits value
      pDst[index] = static_cast<uint8_t>(value);
      return 1;
    }
    if (value <= 0x3FFF)
    {
      assert((index + 1) < dstLength);
      // <=14 bits value
      pDst[index + 0] = static_cast<uint8_t>(0x80 | (value & 0x3F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x3FC0) >> 6);
      return 2;
    }
    if (value <= 0x1FFFFF)
    {
      assert((index + 2) < dstLength);
      // <=21 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xC0 | (value & 0x1F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x001FE0) >> 5);
      pDst[index + 2] = static_cast<uint8_t>((value & 0x1FE000) >> 13);
      return 3;
    }
    if (value <= 0xFFFFFFF)
    {
      assert((index + 3) < dstLength);
      // <=28 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xE0 | (value & 0x0F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x00000FF0) >> 4);
      pDst[index + 2] = static_cast<uint8_t>((value & 0x000FF000) >> 12);
      pDst[index + 3] = static_cast<uint8_t>((value & 0x0FF00000) >> 20);
      return 4;
    }

    assert((index + 4) < dstLength);
    // >28 bits value
    pDst[index + 0] = static_cast<uint8_t>(0xF0 | (value & 0x07));
    pDst[index + 1] = static_cast<uint8_t>((value & 0x000007F8) >> 3);
    pDst[index + 2] = static_cast<uint8_t>((value & 0x0007F800) >> 11);
    pDst[index + 3] = static_cast<uint8_t>((value & 0x07F80000) >> 19);
    pDst[index + 4] = static_cast<uint8_t>((value & 0xF8000000) >> 27);
    return 5;
  }
}
