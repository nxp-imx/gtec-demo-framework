#ifndef FSLBASE_STRING_STRINGVIEWUTIL_HPP
#define FSLBASE_STRING_STRINGVIEWUTIL_HPP
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

#include <FslBase/String/CStringUtil_Length.hpp>
#include <stdexcept>
#include <string_view>

namespace Fsl::StringViewUtil
{
  inline constexpr static std::string_view UncheckedCreate(const char* const pStr, const std::size_t count) noexcept
  {
    assert(pStr != nullptr);
    // Done this was as the 'sv' operator is defined as noexcept and the constructor std::string_view(psz, count) is not.
    return std::literals::string_view_literals::operator""sv(pStr, count);
  }

  //! @brief A safe way to create a std::string_view from a zero terminated 'c style' string.
  //!        Unfortunately this is not the standard behavior for std::string_view(const char*) overload.
  inline constexpr static std::string_view Create(const char* const pStr, const std::size_t count)
  {
    if (pStr == nullptr)
    {
      if (count == 0u)
      {
        return {};
      }
      throw std::invalid_argument("pStr can not be null if count > 0");
    }
    return UncheckedCreate(pStr, count);
  }

  //! @brief A safe way to create a std::string_view from a zero terminated 'c style' string.
  //!        Unfortunately this is not the standard behavior for std::string_view(const char*) overload.
  inline constexpr static std::string_view Create(const char* const psz) noexcept
  {
    return psz != nullptr ? UncheckedCreate(psz, CStringUtil::UncheckedLength(psz)) : std::string_view();
  }


  //! @brief A safe way to create a std::string_view from a zero terminated 'c style' string.
  //!        Unfortunately this is not the standard behavior for std::string_view(const char*) overload.
  inline constexpr static std::string_view UncheckedCreate(const char* const psz) noexcept
  {
    assert(psz != nullptr);
    return UncheckedCreate(psz, CStringUtil::UncheckedLength(psz));
  }
}

#endif
