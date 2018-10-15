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

#include <FslGraphics/Vertices/IndexConverter.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>
#include <cstring>

namespace Fsl
{
  namespace
  {
  }


  void IndexConverter::Convert(uint8_t* const pDst, const std::size_t dstIndexCapacity, const uint8_t* const pSrc, const std::size_t srcIndexCount)
  {
    if (pDst == nullptr || pSrc == nullptr || srcIndexCount > dstIndexCapacity)
    {
      throw std::invalid_argument("invalid arguments");
    }

    std::memcpy(pDst, pSrc, srcIndexCount);
  }


  void IndexConverter::Convert(uint16_t* const pDst, const std::size_t dstIndexCapacity, const uint16_t* const pSrc, const std::size_t srcIndexCount)
  {
    if (pDst == nullptr || pSrc == nullptr || srcIndexCount > dstIndexCapacity)
    {
      throw std::invalid_argument("invalid arguments");
    }

    std::memcpy(pDst, pSrc, srcIndexCount * sizeof(uint16_t));
  }


  void IndexConverter::Convert(uint8_t* const pDst, const std::size_t dstIndexCapacity, const uint16_t* const pSrc, const std::size_t srcIndexCount)
  {
    if (pDst == nullptr || pSrc == nullptr || srcIndexCount > dstIndexCapacity)
    {
      throw std::invalid_argument("invalid arguments");
    }

    for (uint32_t i = 0; i < srcIndexCount; ++i)
    {
      const uint16_t value = pSrc[i];
      if (value > 0xFF)
      {
        throw NotSupportedException("the src index is too large to fit in a uint8_t");
      }
      pDst[i] = static_cast<uint8_t>(value & 0xFF);
    }
  }


  void IndexConverter::Convert(uint16_t* const pDst, const std::size_t dstIndexCapacity, const uint8_t* const pSrc, const std::size_t srcIndexCount)
  {
    if (pDst == nullptr || pSrc == nullptr || srcIndexCount > dstIndexCapacity)
    {
      throw std::invalid_argument("invalid arguments");
    }

    for (uint32_t i = 0; i < srcIndexCount; ++i)
    {
      pDst[i] = pSrc[i];
    }
  }


  void IndexConverter::GenericConvert(void* const pDst, const std::size_t cbDst, const std::size_t dstIndexByteSize, const void* const pSrc,
                                      const std::size_t cbSrc, const std::size_t srcIndexByteSize, const std::size_t srcIndexCount)
  {
    if (dstIndexByteSize == 2)
    {
      switch (srcIndexByteSize)
      {
      case 1:
        Convert(static_cast<uint16_t*>(pDst), cbDst / dstIndexByteSize, static_cast<const uint8_t*>(pSrc), srcIndexCount);
        break;
      case 2:
        Convert(static_cast<uint16_t*>(pDst), cbDst / dstIndexByteSize, static_cast<const uint16_t*>(pSrc), srcIndexCount);
        break;
      default:
        throw NotSupportedException("source index size not supported");
      }
    }
    else if (dstIndexByteSize == 1)
    {
      switch (srcIndexByteSize)
      {
      case 1:
        Convert(static_cast<uint8_t*>(pDst), cbDst / dstIndexByteSize, static_cast<const uint8_t*>(pSrc), srcIndexCount);
        break;
      case 2:
        Convert(static_cast<uint8_t*>(pDst), cbDst / dstIndexByteSize, static_cast<const uint16_t*>(pSrc), srcIndexCount);
        break;
      default:
        throw NotSupportedException("source index size not supported");
      }
    }
  }
}
