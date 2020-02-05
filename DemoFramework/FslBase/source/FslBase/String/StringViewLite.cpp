/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
#include <cstring>

namespace Fsl
{
  bool operator==(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.size() == rhs.size() && strncmp(lhs.data(), rhs.data(), lhs.size()) == 0;
  }

  bool operator!=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.size() != rhs.size() || strncmp(lhs.data(), rhs.data(), lhs.size()) != 0;
  }

  bool operator<(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    if (lhs.size() <= rhs.size())
    {
      auto cmp = strncmp(lhs.data(), rhs.data(), lhs.size());
      return cmp < 0 || (cmp == 0 && lhs.size() < rhs.size());
    }
    auto cmp = strncmp(rhs.data(), lhs.data(), rhs.size());
    return cmp < 0 || (cmp == 0 && lhs.size() < rhs.size());
  }

  bool operator<=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    if (lhs.size() <= rhs.size())
    {
      return strncmp(lhs.data(), rhs.data(), lhs.size()) <= 0;
    }
    return strncmp(rhs.data(), lhs.data(), rhs.size()) < 0;
  }

  bool operator>(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return rhs < lhs;
  }

  bool operator>=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return rhs <= lhs;
  }

  bool operator==(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    if (pszRhs == nullptr)
    {
      return false;
    }

    const char* pLhs = lhs.data();
    const char* const pLhsEnd = lhs.data() + lhs.size();
    const char* psz = pszRhs;
    while (pLhs < pLhsEnd && (*psz) != 0 && (*pLhs) == (*psz))
    {
      ++pLhs;
      ++psz;
    }
    return pLhs == pLhsEnd && (*psz) == 0u;
  }

  bool operator!=(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return !(lhs == pszRhs);
  }

  bool operator==(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return rhs == pszLhs;
  }

  bool operator!=(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return rhs != pszLhs;
  }
}
