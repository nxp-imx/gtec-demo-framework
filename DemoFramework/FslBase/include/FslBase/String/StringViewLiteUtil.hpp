#ifndef FSLBASE_STRING_STRINGVIEWLITEUTIL_HPP
#define FSLBASE_STRING_STRINGVIEWLITEUTIL_HPP
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

#include <FslBase/String/StringViewLite.hpp>
#include <string>

namespace Fsl
{
  // Adds comparision operators for std::string
  // Added in a external file to ensure that not all StringViewLite users need to depend on std::string (and its header)

  namespace StringViewLiteUtil
  {
    constexpr inline const char* NotNull(const char* const psz) noexcept
    {
      return psz != nullptr ? psz : "";
    }

    inline StringViewLite AsStringViewLite(const std::string& str) noexcept
    {
      return StringViewLite(NotNull(str.c_str()), str.size());
    }

    inline std::string ToString(const StringViewLite& strView)
    {
      return std::string(strView.data(), strView.size());
    }

    inline void Set(std::string& rStr, const StringViewLite& strView)
    {
      if (!strView.empty())
      {
        rStr.assign(strView.data(), strView.size());
      }
      else
      {
        rStr.clear();
      }
      // rStr.assign(strView.data(), strView.size());
    }
  }

  inline bool operator==(const StringViewLite& lhs, const std::string& rhs) noexcept
  {
    return lhs == StringViewLiteUtil::AsStringViewLite(rhs);
  }
  inline bool operator==(const std::string& lhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) == rhs;
  }


  inline bool operator!=(const StringViewLite& lhs, const std::string& rhs) noexcept
  {
    return lhs != StringViewLiteUtil::AsStringViewLite(rhs);
  }
  inline bool operator!=(const std::string& lhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) != rhs;
  }


  inline bool operator<(const StringViewLite& lhs, const std::string& rhs) noexcept
  {
    return lhs < StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator<(const std::string& lhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) < rhs;
  }


  inline bool operator<=(const StringViewLite& lhs, const std::string& rhs) noexcept
  {
    return lhs <= StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator<=(const std::string& lhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) <= rhs;
  }


  inline bool operator>(const StringViewLite& lhs, const std::string& rhs) noexcept
  {
    return lhs > StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator>(const std::string& lhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) > rhs;
  }


  inline bool operator>=(const StringViewLite& lhs, const std::string& rhs) noexcept
  {
    return lhs >= StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator>=(const std::string& lhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) >= rhs;
  }

}

#endif
