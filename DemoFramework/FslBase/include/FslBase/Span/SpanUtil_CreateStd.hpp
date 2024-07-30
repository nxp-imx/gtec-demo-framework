#ifndef FSLBASE_SPAN_SPANUTIL_CREATESTD_HPP
#define FSLBASE_SPAN_SPANUTIL_CREATESTD_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <cassert>
#include <span>

namespace Fsl::SpanUtil
{
  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Create
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief safer way to create a std::span. If pData is a nullptr then count will be assumed to be zero.
  //! @note Since passing a nullptr to std::span is undefined behavior this ensures that we always get well defined behavior per default!
  template <typename T>
  inline constexpr std::span<const T> CreateStd(const T* const pData, typename std::span<const T>::size_type count) noexcept
  {
    FSLLOG3_DEBUG_INFO_IF(pData == nullptr && count != 0, "forcing count to zero");
    return pData != nullptr ? std::span<const T>(pData, count) : std::span<const T>();
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief safer way to create a std::span. If pData is a nullptr then count will be assumed to be zero.
  //! @note Since passing a nullptr to std::span is undefined behavior this ensures that we always get well defined behavior per default!
  template <typename T>
  inline constexpr std::span<T> CreateStd(T* pData, typename std::span<T>::size_type count) noexcept
  {
    FSLLOG3_DEBUG_INFO_IF(pData == nullptr && count != 0, "forcing count to zero");
    return pData != nullptr ? std::span<T>(pData, count) : std::span<T>();
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // UncheckedCreate
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief This basically just forwards to the constructor of std::span but with a added assert for debug builds to verify that it's rules are
  //! enforced.
  template <typename T>
  inline constexpr std::span<const T> UncheckedCreateStd(const T* const pData, typename std::span<const T>::size_type count) noexcept
  {
    if (pData == nullptr)
    {
      FSLLOG3_DEBUG_INFO("");
    }
    assert(pData != nullptr);
    return std::span<const T>(pData, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief This basically just forwards to the constructor of std::span but with a added assert for debug builds to verify that it's rules are
  //! enforced.
  template <typename T>
  inline constexpr std::span<T> UncheckedCreateStd(T* pData, typename std::span<T>::size_type count) noexcept
  {
    assert(pData != nullptr);
    return std::span<T>(pData, count);
  }
}

#endif
