#ifndef FSLBASE_SPAN_SPANUTIL_CREATE_HPP
#define FSLBASE_SPAN_SPANUTIL_CREATE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020-2024 NXP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Span/Span.hpp>
#include <cassert>
#include <exception>

namespace Fsl::SpanUtil
{
  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Convert from pointer, size -> span
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr ReadOnlySpan<T> CheckedCreateReadOnly(const T* const pData, const std::size_t count)
  {
    if (pData == nullptr)
    {
      throw std::invalid_argument("passing a nullptr is not valid");
    }
    return ReadOnlySpan<T>(pData, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! A safe way to create a ReadOnlySpan<T> which can actually be used to create a span from a span.data(), span.size() unlike the normal std::span
  //! which fails if you do that as it does not allow pData to be a nullptr.
  template <typename T>
  inline constexpr ReadOnlySpan<T> CreateReadOnly(const T* const pData, const std::size_t count) noexcept
  {
    FSLLOG3_DEBUG_INFO_IF(pData == nullptr && count != 0, "forcing count to zero");
    return pData != nullptr ? std::span<const T>(pData, count) : std::span<const T>();
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! For when you need the additional performance. This assumes that pData is not a nullptr just like the std::span constructor.
  //! Only use this when you are 100% sure that is the case.
  //! Beware that std::span.data() returns a nullptr from span.data() if its empty and a size() of zero and its not valid to pass that to this method!
  template <typename T>
  inline constexpr ReadOnlySpan<T> UncheckedCreateReadOnly(const T* const pData, const std::size_t count) noexcept
  {
    assert(pData != nullptr);
    return std::span<const T>(pData, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Convert from pointer, size -> span
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr Span<T> CheckedCreate(T* const pData, const std::size_t count)
  {
    if (pData == nullptr)
    {
      throw std::invalid_argument("passing a nullptr is not valid");
    }
    return Span<T>(pData, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! A safe way to create a Span<T> which can actually be used to create a span from a span.data(), span.size() unlike the normal std::span
  //! which fails if you do that as it does not allow pData to be a nullptr.
  template <typename T>
  inline constexpr Span<T> Create(T* pData, const std::size_t count) noexcept
  {
    FSLLOG3_DEBUG_INFO_IF(pData == nullptr && count != 0, "forcing count to zero");
    return pData != nullptr ? std::span<T>(pData, count) : std::span<T>();
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! For when you need the additional performance. This assumes that pData is not a nullptr just like the std::span constructor.
  //! Only use this when you are 100% sure that is the case.
  //! Beware that std::span.data() returns a nullptr from span.data() if its empty and a size() of zero and its not valid to pass that to this method!
  template <typename T>
  inline constexpr Span<T> UncheckedCreate(T* pData, const std::size_t count) noexcept
  {
    assert(pData != nullptr);
    return std::span<T>(pData, count);
  }
}

#endif
