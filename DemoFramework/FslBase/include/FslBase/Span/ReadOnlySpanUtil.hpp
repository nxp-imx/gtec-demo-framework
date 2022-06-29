#ifndef FSLBASE_SPAN_READONLYSPANUTIL_HPP
#define FSLBASE_SPAN_READONLYSPANUTIL_HPP
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
#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <array>
#include <cassert>
#include <exception>
#include <vector>

namespace Fsl::ReadOnlySpanUtil
{
  template <typename T>
  constexpr inline ReadOnlySpan<T> AsSpan(const T* const pData, const std::size_t dataEntries)
  {
    if (pData == nullptr && dataEntries > 0)
    {
      throw std::invalid_argument("a nullptr can not have any entries");
    }
    return ReadOnlySpan<T>(pData, dataEntries);
  }

  template <typename T>
  constexpr inline ReadOnlySpan<T> AsSpan(const T* pData, const std::size_t dataEntries, const OptimizationCheckFlag flag) noexcept
  {
    FSL_PARAM_NOT_USED(flag);
    assert(pData != nullptr || dataEntries == 0u);
    return ReadOnlySpan<T>(pData, dataEntries, OptimizationCheckFlag::NoCheck);
  }


  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlySpan<T> AsSpan(const std::array<T, TSize>& value) noexcept
  {
    return ReadOnlySpan<T>(value.data(), value.size(), OptimizationCheckFlag::NoCheck);
  }

  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlySpan<T> AsSpan(const std::array<T, TSize>& value, const std::size_t size, const OptimizationCheckFlag flag) noexcept
  {
    FSL_PARAM_NOT_USED(flag);
    assert(size <= value.size());
    return ReadOnlySpan<T>(value.data(), size, OptimizationCheckFlag::NoCheck);
  }

  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlySpan<T> AsSpan(const std::array<T, TSize>& value, const std::size_t size)
  {
    if (size > value.size())
    {
      throw std::invalid_argument("size is out of bounds");
    }
    return ReadOnlySpan<T>(value.data(), size, OptimizationCheckFlag::NoCheck);
  }


  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlySpan<T> AsSpan(const std::array<T, TSize>& value, const std::size_t offset, const std::size_t size)
  {
    if (offset >= value.size())
    {
      throw std::invalid_argument("offset is out of bounds");
    }
    if (size > (value.size() - offset))
    {
      throw std::invalid_argument("offset+size is out of bounds");
    }
    return ReadOnlySpan<T>(value.data() + offset, size, OptimizationCheckFlag::NoCheck);
  }

  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlySpan<T> AsSpan(const std::array<T, TSize>& value, const std::size_t offset, const std::size_t size,
                                          const OptimizationCheckFlag flag) noexcept
  {
    FSL_PARAM_NOT_USED(flag);
    assert(offset <= value.size());
    assert(size <= (value.size() - offset));
    return ReadOnlySpan<T>(value.data() + offset, size, OptimizationCheckFlag::NoCheck);
  }


  template <typename T>
  constexpr inline ReadOnlySpan<T> AsSpan(const std::vector<T>& value) noexcept
  {
    return ReadOnlySpan<T>(value.data(), value.size(), OptimizationCheckFlag::NoCheck);
  }

  template <typename T>
  constexpr inline ReadOnlySpan<T> AsSpan(const std::vector<T>& value, const std::size_t offset, const std::size_t size)
  {
    if (offset > value.size())
    {
      throw std::invalid_argument("offset is out of bounds");
    }
    if (size > (value.size() - offset))
    {
      throw std::invalid_argument("offset+size is out of bounds");
    }
    return ReadOnlySpan<T>(value.data() + offset, size, OptimizationCheckFlag::NoCheck);
  }

  template <typename T>
  constexpr inline ReadOnlySpan<T> AsSpan(const std::vector<T>& value, const std::size_t offset, const std::size_t size,
                                          const OptimizationCheckFlag flag) noexcept
  {
    FSL_PARAM_NOT_USED(flag);
    assert(offset <= value.size());
    assert(size <= (value.size() - offset));
    return ReadOnlySpan<T>(value.data() + offset, size, OptimizationCheckFlag::NoCheck);
  }

  template <typename T>
  inline std::vector<T> ToVector(const ReadOnlySpan<T> span)
  {
    return std::vector<T>(span.data(), span.data() + span.size());
  }

  // template <std::size_t TSize, typename T>
  // inline std::array<T, TSize> ToArray(const ReadOnlySpan<T> span)
  //{
  //  std::array<T, TSize> array;
  //  if (span.size() != TSize)
  //  {
  //    throw UsageErrorException("span size does not match the array size");
  //  }
  //  for (std::size_t i = 0; i < span.size(); ++i)
  //  {
  //    array[i] = span[i];
  //  }
  //  return array;
  //}
}

#endif
