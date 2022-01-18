#ifndef FSLBASE_SPAN_TYPEDFLEXSPANUTIL_HPP
#define FSLBASE_SPAN_TYPEDFLEXSPANUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/Span/TypedFlexSpan.hpp>
#include <FslBase/TypeAlignmentUtil.hpp>
#include <type_traits>
#include <cassert>
#include <exception>


namespace Fsl
{
  namespace TypedFlexSpanUtil
  {
    template <typename T>
    constexpr inline TypedFlexSpan<T> AsSpan(const T* const pData, const std::size_t dataEntries)
    {
      if (pData == nullptr && dataEntries > 0)
      {
        throw std::invalid_argument("a nullptr can not have any entries");
      }
      return TypedFlexSpan<T>(pData, dataEntries, sizeof(T));
    }

    template <typename T>
    constexpr inline TypedFlexSpan<T> UnsafeAsSpan(uint8_t* const pData, const std::size_t dataEntries, const std::size_t dataByteOffset,
                                                   const std::size_t stride)
    {
      if (pData == nullptr && dataEntries > 0)
      {
        throw std::invalid_argument("a nullptr can not have any entries");
      }
      if (sizeof(T) > stride)
      {
        throw std::invalid_argument("stride can not contain a element of the given type");
      }
      if (dataByteOffset > (stride - sizeof(T)))
      {
        throw std::invalid_argument("offset and type does not fit within the given stride");
      }
      auto* pDst = pData + dataByteOffset;
      if (!TypeAlignmentUtil::UnsafeIsPointerAligned(pDst, std::alignment_of<T>::value))
      {
        throw std::invalid_argument("type alignment restrictions not met");
      }
      if (!TypeAlignmentUtil::IsAligned(stride, std::alignment_of<T>::value))
      {
        throw std::invalid_argument("type alignment restrictions not met by stride");
      }
      return TypedFlexSpan<T>(reinterpret_cast<T*>(pDst), dataEntries, stride);
    }

    template <typename T>
    constexpr inline TypedFlexSpan<T> UnsafeVoidAsSpan(void* const pData, const std::size_t dataEntries, const std::size_t dataByteOffset,
                                                       const std::size_t stride)
    {
      return UnsafeAsSpan<T>(static_cast<uint8_t*>(pData), dataEntries, dataByteOffset, stride);
    }
  }
}

#endif
