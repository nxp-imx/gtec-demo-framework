#ifndef FSLBASE_IO_PATHVIEW_HPP
#define FSLBASE_IO_PATHVIEW_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/String/StringViewLite.hpp>

namespace Fsl::IO
{
  struct PathView : public StringViewLite
  {
    constexpr PathView() noexcept = default;
    constexpr PathView(const PathView& other) noexcept = default;

    //! @brief overload that allows you to create a PathView from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid and contains no backslashes
    explicit constexpr PathView(const StringViewLite str, const OptimizationCheckFlag reserved) noexcept
      : StringViewLite(str)
    {
      assert(find('\\') == StringViewLite::npos);
    }


    //! @brief overload that allows you to create a PathView from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid and contains no backslashes
    explicit constexpr PathView(const const_pointer pStr, size_type count, const OptimizationCheckFlag reserved) noexcept
      : StringViewLite(StringViewLite::UncheckedCreate(pStr, count))
    {
      assert(find('\\') == StringViewLite::npos);
    }

    explicit constexpr PathView(const StringViewLite strView)
      : StringViewLite(strView.data(), strView.size())
    {
      if (find('\\') != StringViewLite::npos)
      {
        throw PathFormatErrorException("PathView can not contain '\\'");
      }
    }

    explicit constexpr PathView(const const_pointer pStr, size_type count)
      : StringViewLite(pStr, count)
    {
      if (find('\\') != StringViewLite::npos)
      {
        throw PathFormatErrorException("PathView can not contain '\\'");
      }
    }

    explicit constexpr PathView(const const_pointer psz)
      : StringViewLite(psz)
    {
      if (find('\\') != StringViewLite::npos)
      {
        throw PathFormatErrorException("PathView can not contain '\\'");
      }
    }

    //! @brief Returns a view of the substring [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr PathView subpath(size_type pos = 0, size_type count = npos) const
    {
      const auto currentLength = length();
      if (pos > currentLength)
      {
        throw std::out_of_range("pos out of range");
      }
      auto maxLength = (currentLength - pos);
      return PathView(data() + pos, (count <= maxLength ? count : maxLength), OptimizationCheckFlag::NoCheck);
    }
  };
}

#endif
