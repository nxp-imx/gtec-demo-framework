#ifndef FSLBASE_SPAN_SPANUTIL_HPP
#define FSLBASE_SPAN_SPANUTIL_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Span/Span.hpp>
#include <array>
#include <vector>

namespace Fsl::SpanUtil
{
  template <typename T>
  constexpr inline Span<T> AsSpan(T* const pData, const std::size_t dataEntries)
  {
    if (pData == nullptr && dataEntries > 0)
    {
      throw std::invalid_argument("a nullptr can not have any entries");
    }
    return Span<T>(pData, dataEntries);
  }

  template <typename T>
  constexpr inline Span<T> AsSpan(T* pData, const std::size_t dataEntries, const OptimizationCheckFlag flag) noexcept
  {
    FSL_PARAM_NOT_USED(flag);
    assert(pData != nullptr || dataEntries == 0u);
    return Span<T>(pData, dataEntries, OptimizationCheckFlag::NoCheck);
  }


  template <typename T, std::size_t TSize>
  constexpr inline Span<T> AsSpan(std::array<T, TSize>& rValue)
  {
    return Span<T>(rValue.data(), rValue.size(), OptimizationCheckFlag::NoCheck);
  }

  template <typename T>
  constexpr inline Span<T> AsSpan(std::vector<T>& rValue)
  {
    return Span<T>(rValue.data(), rValue.size(), OptimizationCheckFlag::NoCheck);
  }

  template <typename T, std::size_t TSize>
  constexpr inline Span<T> AsSubSpan(std::array<T, TSize>& rValue, typename Span<T>::size_type pos,
                                     typename Span<T>::size_type count = Span<T>::extent)
  {
    return Span<T>(rValue.data(), rValue.size(), OptimizationCheckFlag::NoCheck).subspan(pos, count);
  }

  template <typename T, std::size_t TSize>
  constexpr inline Span<T> AsSubSpan(std::array<T, TSize>& rValue, typename Span<T>::size_type pos, typename Span<T>::size_type count,
                                     const OptimizationCheckFlag flag) noexcept
  {
    FSL_PARAM_NOT_USED(flag);
    assert(pos <= rValue.size());
    assert(count <= (rValue.size() - pos));
    return Span<T>(rValue.data() + pos, count, OptimizationCheckFlag::NoCheck);
  }

  template <typename T>
  constexpr inline Span<T> AsSubSpan(std::vector<T>& rValue, typename Span<T>::size_type pos, typename Span<T>::size_type count = Span<T>::extent)
  {
    return Span<T>(rValue.data(), rValue.size(), OptimizationCheckFlag::NoCheck).subspan(pos, count);
  }

  template <typename T>
  constexpr inline Span<T> AsSubSpan(std::vector<T>& rValue, typename Span<T>::size_type pos, typename Span<T>::size_type count,
                                     const OptimizationCheckFlag flag) noexcept
  {
    FSL_PARAM_NOT_USED(flag);
    assert(pos <= rValue.size());
    assert(count <= (rValue.size() - pos));
    return Span<T>(rValue.data() + pos, count, OptimizationCheckFlag::NoCheck);
  }
}

#endif
