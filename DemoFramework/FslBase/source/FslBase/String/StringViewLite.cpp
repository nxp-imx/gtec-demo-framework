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
  // FIX-LATER: Remove once we move to C++17
  // NOLINTNEXTLINE(readability-redundant-declaration)
  const StringViewLite::size_type StringViewLite::npos;

  int StringViewLite::compare(StringViewLite value) const noexcept
  {
    // min of m_length and value.m_length
    const auto countCompare = m_length <= value.m_length ? m_length : value.m_length;
    const auto result = (countCompare == 0 ? 0 : strncmp(m_pStr, value.m_pStr, countCompare));

    return (result == 0 ? (static_cast<int>(m_length > value.m_length) - static_cast<int>(m_length < value.m_length)) : (result < 0 ? -1 : 1));
  }

  bool StringViewLite::starts_with(StringViewLite value) const noexcept
  {
    return m_length >= value.m_length ? (strncmp(m_pStr, value.m_pStr, value.m_length) == 0) : false;
  }

  bool StringViewLite::ends_with(StringViewLite value) const noexcept
  {
    return m_length >= value.m_length ? (strncmp(m_pStr + (m_length - value.m_length), value.m_pStr, value.m_length) == 0) : false;
  }


  // StringViewLite::size_type StringViewLite::find(StringViewLite value, const size_type pos = 0) const noexcept

}
