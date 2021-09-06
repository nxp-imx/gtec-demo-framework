#ifndef FSLBASE_SPAN_READONLYFLEXSPANUTIL_HPP
#define FSLBASE_SPAN_READONLYFLEXSPANUTIL_HPP
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
#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <cassert>
#include <exception>

namespace Fsl
{
  namespace ReadOnlyFlexSpanUtil
  {
    template <typename T>
    constexpr inline ReadOnlyFlexSpan AsSpan(const T* const pData, const std::size_t dataEntries)
    {
      if (pData == nullptr && dataEntries > 0)
      {
        throw std::invalid_argument("a nullptr can not have any entries");
      }
      return ReadOnlyFlexSpan(pData, dataEntries, sizeof(T));
    }

    template <typename T>
    constexpr inline ReadOnlyFlexSpan AsSpan(const T* pData, const std::size_t dataEntries, const OptimizationCheckFlag flag) noexcept
    {
      assert(pData != nullptr || dataEntries == 0u);
      return ReadOnlyFlexSpan(pData, dataEntries, sizeof(T), flag);
    }

    template <typename T>
    constexpr inline ReadOnlyFlexSpan AsSpan(ReadOnlySpan<T> value) noexcept
    {
      return ReadOnlyFlexSpan(value.data(), value.size(), sizeof(T), OptimizationCheckFlag::NoCheck);
    }

    template <typename T>
    constexpr inline ReadOnlyFlexSpan AsSpan(Span<T> value) noexcept
    {
      return ReadOnlyFlexSpan(value.data(), value.size(), sizeof(T), OptimizationCheckFlag::NoCheck);
    }

    constexpr inline ReadOnlyFlexSpan AsSpan(const void* const pData, const std::size_t dataEntries, const std::size_t stride)
    {
      if (pData == nullptr && dataEntries > 0)
      {
        throw std::invalid_argument("a nullptr can not have any entries");
      }
      if (dataEntries > 0 && stride == 0)
      {
        throw std::invalid_argument("a non empty span can not have a stride of zero");
      }
      return ReadOnlyFlexSpan(pData, dataEntries, stride);
    }

  }
}

#endif
