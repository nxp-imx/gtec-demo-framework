/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/String/UTF8String.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>

namespace Fsl
{
  namespace
  {
    const char UTF8_CHAR_MIN = 0;
    const char UTF8_CHAR_MAX = 127;


    constexpr inline bool IsValidUTF8(const char* const psz, const std::size_t startIndex, const std::size_t length)
    {
      FSL_PARAM_NOT_USED(startIndex);
      FSL_PARAM_NOT_USED(length);
      // FIX: add UTF8 validation check
      return psz != nullptr;
    }


    constexpr inline bool IsValidChar(const int ch)
    {
      return (ch >= static_cast<int>(UTF8_CHAR_MIN) && ch <= static_cast<int>(UTF8_CHAR_MAX));
    }
  }


  UTF8String::UTF8String(std::string str)
    : m_content(std::move(str))
  {
    if (!IsValidUTF8(m_content.c_str(), 0, m_content.size()))
    {
      throw InvalidUTF8StringException("The supplied UTF8 string is not valid");
    }
  }


  UTF8String::UTF8String(const char* const psz)
    : m_content(psz != nullptr ? psz : "")
  {
    FSLLOG3_WARNING_IF(psz == nullptr, "UTF8String was supplied a null pointer, using a empty string");
    if (psz != nullptr && !IsValidUTF8(psz, 0, std::strlen(psz)))
    {
      throw InvalidUTF8StringException("The supplied UTF8 string is not valid");
    }
  }


  UTF8String::UTF8String(const StringViewLite& str)
  {
    if (!str.empty())
    {
      if (!IsValidUTF8(str.data(), 0u, str.size()))
      {
        throw InvalidUTF8StringException("The supplied UTF8 string is not valid");
      }
      m_content.assign(str.data(), 0u, str.size());
    }
  }


  UTF8String::UTF8String(const std::string& str, const std::size_t startIndex, const std::size_t length)
  {
    if (startIndex >= str.size())
    {
      throw std::invalid_argument("Start index was out of bounds");
    }
    if ((startIndex + length) > str.size())
    {
      throw std::invalid_argument("startIndex + length was out of bounds");
    }

    if (!IsValidUTF8(str.c_str(), startIndex, length))
    {
      throw InvalidUTF8StringException("The supplied UTF8 string is not valid");
    }
    m_content.assign(str, startIndex, length);
  }


  UTF8String::UTF8String(const UTF8String& str, const std::size_t startIndex, const std::size_t length)
  {
    if (startIndex >= str.m_content.size())
    {
      throw std::invalid_argument("Start index was out of bounds");
    }
    if ((startIndex + length) > str.m_content.size())
    {
      throw std::invalid_argument("startIndex + length was out of bounds");
    }
    if (!IsValidUTF8(m_content.c_str(), startIndex, length))
    {
      throw InvalidUTF8StringException("The substring is not valid utf8");
    }
    m_content.assign(str.m_content, startIndex, length);
  }


  void UTF8String::Clear()
  {
    m_content.clear();
  }


  void UTF8String::Append(const std::size_t count, const char ch)
  {
    if (!IsValidChar(int(ch)))
    {
      throw std::invalid_argument("ch should be in the range 0 to 127");
    }

    m_content.append(count, ch);
  }


  void UTF8String::Append(const char* const psz)
  {
    FSLLOG3_DEBUG_WARNING_IF(psz == nullptr, "UTF8String was supplied a null pointer, using a empty string");
    Append(StringViewLite(psz));
  }


  //! @brief append the string at the end of the string
  void UTF8String::Append(const StringViewLite& str)
  {
    if (!str.empty())
    {
      if (!IsValidUTF8(str.data(), 0u, str.size()))
      {
        throw InvalidUTF8StringException("The supplied UTF8 string is not valid");
      }

      m_content.append(str.data(), str.size());
    }
  }

  void UTF8String::Prepend(const std::size_t count, const char ch)
  {
    if (!IsValidChar(int(ch)))
    {
      throw std::invalid_argument("ch should be in the range 0 to 127");
    }

    m_content.insert(0u, count, ch);
  }

  void UTF8String::Prepend(const char* const psz)
  {
    FSLLOG3_DEBUG_WARNING_IF(psz == nullptr, "UTF8String was supplied a null pointer, using a empty string");
    Prepend(StringViewLite(psz));
  }

  // @brief Insert the string at the beginning at the current string
  void UTF8String::Prepend(const StringViewLite& str)
  {
    if (!str.empty())
    {
      if (!IsValidUTF8(str.data(), 0u, str.size()))
      {
        throw InvalidUTF8StringException("The supplied UTF8 string is not valid");
      }

      m_content.insert(0u, str.data(), str.size());
    }
  }


  bool UTF8String::Contains(const char ch) const
  {
    if (!IsValidChar(int(ch)))
    {
      throw std::invalid_argument("ch should be in the range 0 to 127");
    }
    return std::find(m_content.begin(), m_content.end(), ch) != m_content.end();
  }


  bool UTF8String::Contains(const UTF8String& str) const
  {
    if (str.GetByteSize() != 0)
    {
      return std::search(m_content.begin(), m_content.end(), str.m_content.begin(), str.m_content.end()) != m_content.end();
    }
    return true;
  }


  bool UTF8String::StartsWith(const char ch) const
  {
    if (!IsValidChar(static_cast<unsigned char>(ch)))
    {
      throw std::invalid_argument("ch should be in the range 0 to 127");
    }
    return (!m_content.empty() ? m_content.front() == ch : false);
  }


  bool UTF8String::StartsWith(const UTF8String& str) const
  {
    return StringUtil::StartsWith(m_content, str.m_content);
  }


  bool UTF8String::EndsWith(const char ch) const
  {
    if (!IsValidChar(int(ch)))
    {
      throw std::invalid_argument("ch should be in the range 0 to 127");
    }
    return (!m_content.empty() ? m_content.back() == ch : false);
  }


  bool UTF8String::EndsWith(const UTF8String& path) const
  {
    if (m_content.size() < path.m_content.size())
    {
      return false;
    }
    return (m_content.compare(m_content.size() - path.m_content.size(), path.m_content.size(), path.m_content) == 0);
  }

  void UTF8String::Replace(const char from, const char to)
  {
    if (!IsValidChar(int(from)))
    {
      throw std::invalid_argument("from char should be in the range 0 to 127");
    }
    if (!IsValidChar(int(to)))
    {
      throw std::invalid_argument("from to should be in the range 0 to 127");
    }

    std::replace(m_content.begin(), m_content.end(), from, to);
  }


  int32_t UTF8String::IndexOf(const char ch, const std::size_t fromIndex) const
  {
    if (!IsValidChar(int(ch)))
    {
      throw std::invalid_argument("ch should be in the range 0 to 127");
    }
    return StringUtil::IndexOf(m_content, ch, static_cast<int32_t>(fromIndex));
  }


  int32_t UTF8String::LastIndexOf(const char ch) const
  {
    if (!IsValidChar(int(ch)))
    {
      throw std::invalid_argument("ch should be in the range 0 to 127");
    }
    const std::size_t index = m_content.rfind(ch);
    assert(index == std::string::npos || index <= static_cast<std::size_t>(std::numeric_limits<int32_t>::max()));
    return (index != std::string::npos ? static_cast<int32_t>(index) : -1);
  }


  std::string UTF8String::ToAsciiString() const
  {
    // Slow but it works
    fmt::memory_buffer buf;
    std::string::const_iterator itr = m_content.begin();
    const std::string::const_iterator itrEnd = m_content.end();
    bool bIsFirst = true;
    while (itr != itrEnd)
    {
      const uint32_t value = static_cast<unsigned char>(*itr);
      if (value <= static_cast<uint32_t>(UTF8_CHAR_MAX))
      {
        fmt::format_to(std::back_inserter(buf), "{}", static_cast<char>(value));
        bIsFirst = true;
      }
      else if (bIsFirst)
      {
        fmt::format_to(std::back_inserter(buf), "?");
        bIsFirst = false;
      }
      ++itr;
    }
    return fmt::to_string(buf);
  }


  UTF8String& UTF8String::operator=(const StringViewLite& str)
  {
    if (!str.empty())
    {
      if (!IsValidUTF8(str.data(), 0u, str.size()))
      {
        throw InvalidUTF8StringException("The supplied UTF8 string is not valid");
      }
      m_content.assign(str.data(), 0u, str.size());
    }
    else
    {
      m_content.clear();
    }
    return *this;
  }
}
