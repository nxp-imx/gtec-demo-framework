#ifndef FSLBASE_BITS_BITSUTIL_HPP
#define FSLBASE_BITS_BITSUTIL_HPP
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

namespace Fsl
{
  class BitsUtil
  {
  public:
    //! @brief Count the number of 'on' bits in a UInt32.
    //! @note http://www.necessaryandsufficient.net/2009/04/optimising-bit-counting-using-iterative-data-driven-development/
    static int Count(uint32_t value)
    {
      // Modified HAKMEM bit counting
      uint32_t n = (value >> 1) & 0x77777777;
      value = value - n;
      n = (n >> 1) & 0x77777777;
      value = value - n;
      n = (n >> 1) & 0x77777777;
      value = value - n;
      value = ((value + (value >> 4)) & 0xF0F0F0F);
      value = value * 0x01010101;
      return static_cast<int>(value >> 24);
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Locate the first 'on' bit starting from bit0 in a UInt32
    //! @return the bit index or -1 if none found.
    static int IndexOf(uint32_t value)
    {
      int index = 0;
      while ((value & 1) == 0 && index < 32)
      {
        value >>= 1;
        ++index;
      }
      return (index < 32 ? index : -1);
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Locate the first 'on' bit starting from bit0 in a UInt32
    //! @return the bit index or -1 if none found.
    static int IndexOf(int32_t value)
    {
      int index = 0;
      while ((value & 1) == 0 && index < 32)
      {
        value >>= 1;
        ++index;
      }
      return (index < 32 ? index : -1);
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Find the power of two value that is equal or higher than the supplied value.
    static uint32_t NextPowerOfTwo(uint32_t value)
    {
      if (value > 0)
      {
        --value;
        value |= (value >> 1);
        value |= (value >> 2);
        value |= (value >> 4);
        value |= (value >> 8);
        value |= (value >> 16);
        ++value;    // Val is now the next highest power of 2.
      }
      else
      {
        value = 1;
      }
      return value;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Find the power of two value that is equal or higher than the supplied value.
    static int32_t NextPowerOfTwo(const int32_t value)
    {
      int32_t absValue = (value >= 0 ? value : -value);
      if (absValue > 0)
      {
        --absValue;
        absValue |= (absValue >> 1);
        absValue |= (absValue >> 2);
        absValue |= (absValue >> 4);
        absValue |= (absValue >> 8);
        absValue |= (absValue >> 16);
        ++absValue;    // Val is now the next highest power of 2.
        return value >= 0 ? absValue : -absValue;
      }
      return 1;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Check if the value is a pow 2 value
    static bool IsPowerOfTwo(int value)
    {
      return value > 0 && (NextPowerOfTwo(value) == value);
    }
  };
}

#endif
