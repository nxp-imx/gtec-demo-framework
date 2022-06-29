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
#include <FslBase/Exceptions.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    void ThrowInvalidFormatNotEnoughBytes()
    {
      throw InvalidFormatException("did not contain the expected number of bytes");
    }

    void ThrowIndexOutOfRangeException()
    {
      throw IndexOutOfRangeException("Not enough room in destination buffer");
    }
  }


  int32_t ValueCompression::ReadSimpleInt32(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    const uint32_t value = ReadSimpleUInt32(pSrc, srcLength, index);
    return static_cast<int32_t>((value >> 1) ^ ((~0) + ((~value) & 1)));
  }


  uint32_t ValueCompression::ReadSimpleUInt32(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    if (index >= srcLength)
    {
      ThrowInvalidFormatNotEnoughBytes();
    }
    const uint32_t value = pSrc[index];
    if ((value & 0x80) == 0)
    {
      return value;
    }
    if ((value & 0x40) == 0)
    {
      if ((index + 1) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x3F) | (static_cast<uint32_t>(pSrc[index + 1]) << 6);
    }
    if ((value & 0x20) == 0)
    {
      if ((index + 2) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x1F) | (static_cast<uint32_t>(pSrc[index + 1]) << 5) | (static_cast<uint32_t>(pSrc[index + 2]) << 13);
    }
    if ((value & 0x10) == 0)
    {
      if ((index + 3) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x0F) | (static_cast<uint32_t>(pSrc[index + 1]) << 4) | (static_cast<uint32_t>(pSrc[index + 2]) << 12) |
             (static_cast<uint32_t>(pSrc[index + 3]) << 20);
    }
    if ((index + 4) >= srcLength)
    {
      ThrowInvalidFormatNotEnoughBytes();
    }
    return (value & 0x07) | (static_cast<uint32_t>(pSrc[index + 1]) << 3) | (static_cast<uint32_t>(pSrc[index + 2]) << 11) |
           (static_cast<uint32_t>(pSrc[index + 3]) << 19) | (static_cast<uint32_t>(pSrc[index + 4]) << 27);
  }

  int64_t ValueCompression::ReadSimpleInt64(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    const uint64_t value = ReadSimpleUInt64(pSrc, srcLength, index);
    return static_cast<int64_t>((value >> 1) ^ ((~0) + ((~value) & 1)));
  }


  uint64_t ValueCompression::ReadSimpleUInt64(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    if (index >= srcLength)
    {
      ThrowInvalidFormatNotEnoughBytes();
    }
    const uint64_t value = pSrc[index];
    if ((value & 0x80) == 0)
    {
      return value;
    }
    if ((value & 0x40) == 0)
    {
      if ((index + 1) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x3F) | (static_cast<uint64_t>(pSrc[index + 1]) << 6);
    }
    if ((value & 0x20) == 0)
    {
      if ((index + 2) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x1F) | (static_cast<uint64_t>(pSrc[index + 1]) << 5) | (static_cast<uint64_t>(pSrc[index + 2]) << 13);
    }
    if ((value & 0x10) == 0)
    {
      if ((index + 3) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x0F) | (static_cast<uint64_t>(pSrc[index + 1]) << 4) | (static_cast<uint64_t>(pSrc[index + 2]) << 12) |
             (static_cast<uint64_t>(pSrc[index + 3]) << 20);
    }
    if ((value & 0x8) == 0)
    {
      if ((index + 4) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x07) | (static_cast<uint64_t>(pSrc[index + 1]) << 3) | (static_cast<uint64_t>(pSrc[index + 2]) << 11) |
             (static_cast<uint64_t>(pSrc[index + 3]) << 19) | (static_cast<uint64_t>(pSrc[index + 4]) << 27);
    }
    if ((value & 0x4) == 0)
    {
      if ((index + 5) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x03) | (static_cast<uint64_t>(pSrc[index + 1]) << 2) | (static_cast<uint64_t>(pSrc[index + 2]) << 10) |
             (static_cast<uint64_t>(pSrc[index + 3]) << 18) | (static_cast<uint64_t>(pSrc[index + 4]) << 26) |
             (static_cast<uint64_t>(pSrc[index + 5]) << 34);
    }
    if ((value & 0x2) == 0)
    {
      if ((index + 6) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return (value & 0x01) | (static_cast<uint64_t>(pSrc[index + 1]) << 1) | (static_cast<uint64_t>(pSrc[index + 2]) << 9) |
             (static_cast<uint64_t>(pSrc[index + 3]) << 17) | (static_cast<uint64_t>(pSrc[index + 4]) << 25) |
             (static_cast<uint64_t>(pSrc[index + 5]) << 33) | (static_cast<uint64_t>(pSrc[index + 6]) << 41);
    }
    if ((value & 0x1) == 0)
    {
      if ((index + 7) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      return static_cast<uint64_t>(pSrc[index + 1]) | (static_cast<uint64_t>(pSrc[index + 2]) << 8) | (static_cast<uint64_t>(pSrc[index + 3]) << 16) |
             (static_cast<uint64_t>(pSrc[index + 4]) << 24) | (static_cast<uint64_t>(pSrc[index + 5]) << 32) |
             (static_cast<uint64_t>(pSrc[index + 6]) << 40) | (static_cast<uint64_t>(pSrc[index + 7]) << 48);
    }

    if ((index + 8) >= srcLength)
    {
      ThrowInvalidFormatNotEnoughBytes();
    }
    return static_cast<uint64_t>(pSrc[index + 1]) | (static_cast<uint64_t>(pSrc[index + 2]) << 8) | (static_cast<uint64_t>(pSrc[index + 3]) << 16) |
           (static_cast<uint64_t>(pSrc[index + 4]) << 24) | (static_cast<uint64_t>(pSrc[index + 5]) << 32) |
           (static_cast<uint64_t>(pSrc[index + 6]) << 40) | (static_cast<uint64_t>(pSrc[index + 7]) << 48) |
           (static_cast<uint64_t>(pSrc[index + 8]) << 56);
  }


  std::size_t ValueCompression::ReadSimple(int32_t& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    uint32_t value = 0;
    const auto bytesRead = ReadSimple(value, pSrc, srcLength, index);

    rResult = static_cast<int32_t>((value >> 1) ^ ((~0) + ((~value) & 1)));
    assert(rResult == static_cast<int32_t>((value & 1) == 0 ? (value >> 1) : ((value >> 1) ^ (~0))));
    return bytesRead;
  }


  std::size_t ValueCompression::ReadSimple(uint32_t& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    if (index >= srcLength)
    {
      ThrowInvalidFormatNotEnoughBytes();
    }
    auto currentIndex = index;
    uint32_t result = pSrc[currentIndex];
    if ((result & 0x80) == 0)
    {
      ++currentIndex;
    }
    else if ((result & 0x40) == 0)
    {
      if ((index + 1) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x3F) | (static_cast<uint32_t>(pSrc[currentIndex + 1]) << 6);
      currentIndex += 2;
    }
    else if ((result & 0x20) == 0)
    {
      if ((index + 2) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x1F) | (static_cast<uint32_t>(pSrc[currentIndex + 1]) << 5) | (static_cast<uint32_t>(pSrc[currentIndex + 2]) << 13);
      currentIndex += 3;
    }
    else if ((result & 0x10) == 0)
    {
      if ((index + 3) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x0F) | (static_cast<uint32_t>(pSrc[currentIndex + 1]) << 4) | (static_cast<uint32_t>(pSrc[currentIndex + 2]) << 12) |
               (static_cast<uint32_t>(pSrc[currentIndex + 3]) << 20);
      currentIndex += 4;
    }
    else
    {
      if ((index + 4) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x07) | (static_cast<uint32_t>(pSrc[currentIndex + 1]) << 3) | (static_cast<uint32_t>(pSrc[currentIndex + 2]) << 11) |
               (static_cast<uint32_t>(pSrc[currentIndex + 3]) << 19) | (static_cast<uint32_t>(pSrc[currentIndex + 4]) << 27);
      currentIndex += 5;
    }
    rResult = result;
    return currentIndex - index;
  }


  std::size_t ValueCompression::ReadSimple(int64_t& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    uint64_t value = 0;
    const auto bytesRead = ReadSimple(value, pSrc, srcLength, index);

    rResult = static_cast<int64_t>((value >> 1) ^ ((~0) + ((~value) & 1)));
    assert(rResult == static_cast<int64_t>((value & 1) == 0 ? (value >> 1) : ((value >> 1) ^ (~0))));
    return bytesRead;
  }


  std::size_t ValueCompression::ReadSimple(uint64_t& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index)
  {
    assert(pSrc != nullptr);
    if (index >= srcLength)
    {
      ThrowInvalidFormatNotEnoughBytes();
    }
    auto currentIndex = index;
    uint64_t result = pSrc[currentIndex];
    if ((result & 0x80) == 0)    // 0AAA AAAA
    {
      ++currentIndex;
    }
    else if ((result & 0x40) == 0)    // 10AA AAAA - BBBB BBAA
    {
      if ((index + 1) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x3F) | (static_cast<uint64_t>(pSrc[currentIndex + 1]) << 6);
      currentIndex += 2;
    }
    else if ((result & 0x20) == 0)    // 110A AAAA - BBBB BAAA - CCCC CBBB
    {
      if ((index + 2) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x1F) | (static_cast<uint64_t>(pSrc[currentIndex + 1]) << 5) | (static_cast<uint64_t>(pSrc[currentIndex + 2]) << 13);
      currentIndex += 3;
    }
    else if ((result & 0x10) == 0)    // 1110 AAAA - BBBB AAAA - CCCC BBBB - DDDD CCCC
    {
      if ((index + 3) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x0F) | (static_cast<uint64_t>(pSrc[currentIndex + 1]) << 4) | (static_cast<uint64_t>(pSrc[currentIndex + 2]) << 12) |
               (static_cast<uint64_t>(pSrc[currentIndex + 3]) << 20);
      currentIndex += 4;
    }
    else if ((result & 0x8) == 0)    // 1111 0AAA - BBBA AAAA - CCCB BBBB - DDDC CCCC - EEED DDDD
    {
      if ((index + 4) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x07) | (static_cast<uint64_t>(pSrc[currentIndex + 1]) << 3) | (static_cast<uint64_t>(pSrc[currentIndex + 2]) << 11) |
               (static_cast<uint64_t>(pSrc[currentIndex + 3]) << 19) | (static_cast<uint64_t>(pSrc[currentIndex + 4]) << 27);
      currentIndex += 5;
    }
    else if ((result & 0x4) == 0)    // 1111 10AA - BBAA AAAA - CCVB BBBB - DDCC CCCC - EEDD DDDD - FFEE EEEE
    {
      if ((index + 5) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x03) | (static_cast<uint64_t>(pSrc[currentIndex + 1]) << 2) | (static_cast<uint64_t>(pSrc[currentIndex + 2]) << 10) |
               (static_cast<uint64_t>(pSrc[currentIndex + 3]) << 18) | (static_cast<uint64_t>(pSrc[currentIndex + 4]) << 26) |
               (static_cast<uint64_t>(pSrc[currentIndex + 5]) << 34);

      currentIndex += 6;
    }
    else if ((result & 0x2) == 0)    // 1111 110A - BAAA AAAA - CCBB BBBB - DCCC CCCC - EDDD DDDD - FEEE EEEE - GFFF FFFF
    {
      if ((index + 6) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (result & 0x01) | (static_cast<uint64_t>(pSrc[currentIndex + 1]) << 1) | (static_cast<uint64_t>(pSrc[currentIndex + 2]) << 9) |
               (static_cast<uint64_t>(pSrc[currentIndex + 3]) << 17) | (static_cast<uint64_t>(pSrc[currentIndex + 4]) << 25) |
               (static_cast<uint64_t>(pSrc[currentIndex + 5]) << 33) | (static_cast<uint64_t>(pSrc[currentIndex + 6]) << 41);

      currentIndex += 7;
    }
    else if ((result & 0x1) == 0)    // 1111 1110 - AAAA AAAA - BBBB BBBB - CCCC CCCC - DDDD DDDD - EEEE EEEE - FFFF FFFF - GGGG GGGG
    {
      if ((index + 7) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      result = (static_cast<uint64_t>(pSrc[currentIndex + 1])) | (static_cast<uint64_t>(pSrc[currentIndex + 2]) << (8 * 1)) |
               (static_cast<uint64_t>(pSrc[currentIndex + 3]) << (8 * 2)) | (static_cast<uint64_t>(pSrc[currentIndex + 4]) << (8 * 3)) |
               (static_cast<uint64_t>(pSrc[currentIndex + 5]) << (8 * 4)) | (static_cast<uint64_t>(pSrc[currentIndex + 6]) << (8 * 5)) |
               (static_cast<uint64_t>(pSrc[currentIndex + 7]) << (8 * 6));
      currentIndex += 8;
    }
    else    // 1111 1111 - AAAA AAAA - BBBB BBBB - CCCC CCCC - DDDD DDDD - EEEE EEEE - FFFF FFFF - GGGG GGGG - HHHH HHHH
    {
      if ((index + 8) >= srcLength)
      {
        ThrowInvalidFormatNotEnoughBytes();
      }
      assert(result == 0xFF);
      result = (static_cast<uint64_t>(pSrc[currentIndex + 1])) | (static_cast<uint64_t>(pSrc[currentIndex + 2]) << (8 * 1)) |
               (static_cast<uint64_t>(pSrc[currentIndex + 3]) << (8 * 2)) | (static_cast<uint64_t>(pSrc[currentIndex + 4]) << (8 * 3)) |
               (static_cast<uint64_t>(pSrc[currentIndex + 5]) << (8 * 4)) | (static_cast<uint64_t>(pSrc[currentIndex + 6]) << (8 * 5)) |
               (static_cast<uint64_t>(pSrc[currentIndex + 7]) << (8 * 6)) | (static_cast<uint64_t>(pSrc[currentIndex + 8]) << (8 * 7));
      currentIndex += 9;
    }
    rResult = result;
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
      if (index >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=7 bits value
      pDst[index] = static_cast<uint8_t>(value);
      return 1;
    }
    if (value <= 0x3FFF)
    {
      if ((index + 1) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=14 bits value
      pDst[index + 0] = static_cast<uint8_t>(0x80 | (value & 0x3F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x3FC0) >> 6);
      return 2;
    }
    if (value <= 0x1FFFFF)
    {
      if ((index + 2) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=21 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xC0 | (value & 0x1F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x001FE0) >> 5);
      pDst[index + 2] = static_cast<uint8_t>((value & 0x1FE000) >> 13);
      return 3;
    }
    if (value <= 0xFFFFFFF)
    {
      if ((index + 3) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=28 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xE0 | (value & 0x0F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x00000FF0) >> 4);
      pDst[index + 2] = static_cast<uint8_t>((value & 0x000FF000) >> 12);
      pDst[index + 3] = static_cast<uint8_t>((value & 0x0FF00000) >> 20);
      return 4;
    }

    if ((index + 4) >= dstLength)
    {
      ThrowIndexOutOfRangeException();
    }
    // >28 bits value
    pDst[index + 0] = static_cast<uint8_t>(0xF0 | (value & 0x07));
    pDst[index + 1] = static_cast<uint8_t>((value & 0x000007F8) >> 3);
    pDst[index + 2] = static_cast<uint8_t>((value & 0x0007F800) >> 11);
    pDst[index + 3] = static_cast<uint8_t>((value & 0x07F80000) >> 19);
    pDst[index + 4] = static_cast<uint8_t>((value & 0xF8000000) >> 27);
    return 5;
  }


  std::size_t ValueCompression::WriteSimple(uint8_t* const pDst, const std::size_t dstLength, const std::size_t index, const int64_t value)
  {
    // ZigZag encode signed numbers
    // While this "(value << 1) ^ (value >> 31)" might be more optimal its very dependent on how the language handles sign extension when downshifting
    // so a 'safe' implementation was chosen
    return WriteSimple(pDst, dstLength, index, static_cast<uint64_t>(value >= 0 ? (value << 1) : ((value << 1) ^ (~0))));
  }

  std::size_t ValueCompression::WriteSimple(uint8_t* const pDst, const std::size_t dstLength, const std::size_t index, const uint64_t value)
  {
    assert(pDst != nullptr);
    if (value <= 0x7F)    // 0AAA AAAA
    {
      if (index >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=7 bits value
      pDst[index] = static_cast<uint8_t>(value);
      return 1;
    }
    if (value <= 0x3FFF)    // 10AA AAAA - BBBB BBAA
    {
      if ((index + 1) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=14 bits value
      pDst[index + 0] = static_cast<uint8_t>(0x80 | (value & 0x3F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x3FC0) >> 6);
      return 2;
    }
    if (value <= 0x1FFFFF)    // 110A AAAA - BBBB BAAA - CCCC CBBB
    {
      if ((index + 2) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=21 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xC0 | (value & 0x1F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x001FE0) >> 5);
      pDst[index + 2] = static_cast<uint8_t>((value & 0x1FE000) >> 13);
      return 3;
    }
    if (value <= 0xFFFFFFF)    // 1110 AAAA - BBBB AAAA - CCCC BBBB - DDDD CCCC
    {
      if ((index + 3) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=28 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xE0 | (value & 0x0F));
      pDst[index + 1] = static_cast<uint8_t>((value & 0x00000FF0) >> 4);
      pDst[index + 2] = static_cast<uint8_t>((value & 0x000FF000) >> 12);
      pDst[index + 3] = static_cast<uint8_t>((value & 0x0FF00000) >> 20);
      return 4;
    }
    if (value <= 0x7FFFFFFFF)    // 1111 0AAA - BBBA AAAA - CCCB BBBB - DDDC CCCC - EEED DDDD
    {
      if ((index + 4) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=35 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xF0 | (value & 0x07));
      pDst[index + 1] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 3)) >> 3);
      pDst[index + 2] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 11)) >> 11);
      pDst[index + 3] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 19)) >> 19);
      pDst[index + 4] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 27)) >> 27);
      return 5;
    }
    if (value <= 0x3FFFFFFFFFF)    // 1111 10AA - BBAA AAAA - CCVB BBBB - DDCC CCCC - EEDD DDDD - FFEE EEEE
    {
      if ((index + 5) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=42 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xF8 | (value & 0x03));
      pDst[index + 1] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 2)) >> 2);
      pDst[index + 2] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 10)) >> 10);
      pDst[index + 3] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 18)) >> 18);
      pDst[index + 4] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 26)) >> 26);
      pDst[index + 5] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 34)) >> 34);
      return 6;
    }
    if (value <= 0x1FFFFFFFFFFFF)    // 1111 110A - BAAA AAAA - CCBB BBBB - DCCC CCCC - EDDD DDDD - FEEE EEEE - GFFF FFFF
    {
      if ((index + 6) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=49 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xFC | (value & 0x01));
      pDst[index + 1] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 1)) >> 1);
      pDst[index + 2] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 9)) >> 9);
      pDst[index + 3] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 17)) >> 17);
      pDst[index + 4] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 25)) >> 25);
      pDst[index + 5] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 33)) >> 33);
      pDst[index + 6] = static_cast<uint8_t>((value & (static_cast<uint64_t>(0xFF) << 41)) >> 41);
      return 7;
    }
    if (value <= 0xFFFFFFFFFFFFFF)    // 1111 1110 - AAAA AAAA - BBBB BBBB - CCCC CCCC - DDDD DDDD - EEEE EEEE - FFFF FFFF - GGGG GGGG
    {
      if ((index + 7) >= dstLength)
      {
        ThrowIndexOutOfRangeException();
      }
      // <=56 bits value
      pDst[index + 0] = static_cast<uint8_t>(0xFE);
      pDst[index + 1] = static_cast<uint8_t>((value & 0x00000000000000FF));
      pDst[index + 2] = static_cast<uint8_t>((value & 0x000000000000FF00) >> (8 * 1));
      pDst[index + 3] = static_cast<uint8_t>((value & 0x0000000000FF0000) >> (8 * 2));
      pDst[index + 4] = static_cast<uint8_t>((value & 0x00000000FF000000) >> (8 * 3));
      pDst[index + 5] = static_cast<uint8_t>((value & 0x000000FF00000000) >> (8 * 4));
      pDst[index + 6] = static_cast<uint8_t>((value & 0x0000FF0000000000) >> (8 * 5));
      pDst[index + 7] = static_cast<uint8_t>((value & 0x00FF000000000000) >> (8 * 6));
      return 8;
    }

    // 1111 1111 - AAAA AAAA - BBBB BBBB - CCCC CCCC - DDDD DDDD - EEEE EEEE - FFFF FFFF - GGGG GGGG - HHHH HHHH
    if ((index + 8) >= dstLength)
    {
      ThrowIndexOutOfRangeException();
    }

    // >56 bits value (aka 64bit)
    pDst[index + 0] = static_cast<uint8_t>(0xFF);
    pDst[index + 1] = static_cast<uint8_t>((value & 0x00000000000000FF));
    pDst[index + 2] = static_cast<uint8_t>((value & 0x000000000000FF00) >> (8 * 1));
    pDst[index + 3] = static_cast<uint8_t>((value & 0x0000000000FF0000) >> (8 * 2));
    pDst[index + 4] = static_cast<uint8_t>((value & 0x00000000FF000000) >> (8 * 3));
    pDst[index + 5] = static_cast<uint8_t>((value & 0x000000FF00000000) >> (8 * 4));
    pDst[index + 6] = static_cast<uint8_t>((value & 0x0000FF0000000000) >> (8 * 5));
    pDst[index + 7] = static_cast<uint8_t>((value & 0x00FF000000000000) >> (8 * 6));
    pDst[index + 8] = static_cast<uint8_t>((value & 0xFF00000000000000) >> (8 * 7));
    return 9;
  }
}
