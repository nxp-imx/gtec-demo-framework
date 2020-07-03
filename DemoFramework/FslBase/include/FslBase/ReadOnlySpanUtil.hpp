#ifndef FSLBASE_READONLYSPANUTIL_HPP
#define FSLBASE_READONLYSPANUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/ReadOnlySpan.hpp>
#include <array>
#include <cassert>
#include <exception>
#include <vector>

namespace Fsl
{
  namespace ReadOnlySpanUtil
  {
    template <typename T>
    constexpr inline ReadOnlySpan<T> AsSpan(const T* const pData, const std::size_t cbData)
    {
      return ReadOnlySpan<T>(pData, cbData);
    }

    template <typename T, std::size_t TSize>
    constexpr inline ReadOnlySpan<T> AsSpan(const T* pData, const std::size_t cbData, const OptimizationCheckFlag /*unused*/) noexcept
    {
      assert(pData != nullptr || cbData == 0u);
      return ReadOnlySpan<T>(pData, cbData);
    }


    template <typename T, std::size_t TSize>
    constexpr inline ReadOnlySpan<T> AsSpan(const std::array<T, TSize>& value) noexcept
    {
      return ReadOnlySpan<T>(value.data(), value.size());
    }

    template <typename T>
    constexpr inline ReadOnlySpan<T> AsSpan(const std::vector<T>& value) noexcept
    {
      return ReadOnlySpan<T>(value.data(), value.size());
    }

    template <typename T>
    inline std::vector<T> ToVector(const ReadOnlySpan<T> span)
    {
      return std::vector<T>(span.data(), span.data() + span.size());
    }

  }
}

#endif
