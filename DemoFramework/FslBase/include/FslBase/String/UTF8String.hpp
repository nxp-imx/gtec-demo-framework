#ifndef FSLBASE_STRING_UTF8STRING_HPP
#define FSLBASE_STRING_UTF8STRING_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <string>
#include <utility>

namespace Fsl
{
  class UTF8String
  {
    std::string m_content;

  public:
    UTF8String() noexcept = default;

    // Request that the compiler generates a copy constructor and assignment operator
    UTF8String(const UTF8String&) = default;
    UTF8String& operator=(const UTF8String&) = default;

    // move assignment operator
    UTF8String& operator=(UTF8String&& other) noexcept
    {
      if (this != &other)
      {
        m_content = std::move(other.m_content);
      }
      return *this;
    }

    // move constructor
    UTF8String(UTF8String&& other) noexcept
      : m_content(std::move(other.m_content))
    {
    }


    explicit UTF8String(std::string str);

    UTF8String(const char* const psz);        // NOLINT(google-explicit-constructor)
    UTF8String(const StringViewLite& str);    // NOLINT(google-explicit-constructor)
    UTF8String(const std::string& str, const std::size_t startIndex, const std::size_t length);
    UTF8String(const UTF8String& str, const std::size_t startIndex, const std::size_t length);
    ~UTF8String() noexcept = default;

    void Clear();

    // @brief append count copies of char 'ch' to the end of the string
    void Append(const std::size_t count, const char ch);

    //! @brief append the string at the end of the string
    void Append(const char* const psz);

    //! @brief append the string at the end of the string
    void Append(const StringViewLite& str);

    //! @brief append the string at the end of the string
    void Append(const UTF8String& str)
    {
      m_content.append(str.m_content.data(), str.m_content.size());
    }

    // @brief Insert count copies of char 'ch' at the beginning at the current string
    void Prepend(const std::size_t count, const char ch);

    // @brief Insert the string at the beginning at the current string
    void Prepend(const char* const psz);

    // @brief Insert the string at the beginning at the current string
    void Prepend(const StringViewLite& str);

    // @brief Insert the string at the beginning at the current string
    void Prepend(const UTF8String& str)
    {
      m_content.insert(0u, str.m_content.data(), str.m_content.size());
    }

    //! @brief Check if the string is empty
    bool IsEmpty() const
    {
      return m_content.empty();
    }

    int32_t GetByteSize() const
    {
      return static_cast<int32_t>(m_content.size());
    }

    bool Contains(const char ch) const;
    // bool Contains(const StringViewLite& str) const;
    bool Contains(const UTF8String& str) const;

    bool StartsWith(const char ch) const;

    bool StartsWith(const char* const psz) const
    {
      return AsStringViewLite().starts_with(psz);
    }

    bool StartsWith(const StringViewLite& str) const
    {
      return AsStringViewLite().starts_with(str);
    }

    bool StartsWith(const UTF8String& str) const;

    bool EndsWith(const char ch) const;

    bool EndsWith(const char* const psz) const
    {
      return AsStringViewLite().ends_with(psz);
    }

    bool EndsWith(const StringViewLite& str) const
    {
      return AsStringViewLite().ends_with(str);
    }

    bool EndsWith(const UTF8String& path) const;

    void Replace(const char from, const char to);

    int32_t IndexOf(const char ch, const std::size_t fromIndex = 0) const;

    int32_t LastIndexOf(const char ch) const;

    const std::string& AsString() const
    {
      return m_content;
    }

    const std::string& ToUTF8String() const
    {
      return m_content;
    }

    std::string ToAsciiString() const;

    UTF8String& operator=(const StringViewLite& str);

    UTF8String& operator=(const char* const psz)
    {
      *this = StringViewLite(psz);
      return *this;
    }

    // NOTE: there is no operator+ or += that takes a char as that can lead subtle bugs

    UTF8String& operator+=(const char* const pszRhs)
    {
      Append(StringViewLite(pszRhs));
      return *this;
    }

    UTF8String& operator+=(const StringViewLite& rhs)
    {
      Append(rhs);
      return *this;
    }

    UTF8String& operator+=(const UTF8String& rhs)
    {
      Append(rhs);
      return *this;
    }

    inline friend UTF8String operator+(UTF8String lhs, const char* const pszRhs)
    {
      lhs.Append(StringViewLite(pszRhs));
      return lhs;
    }

    inline friend UTF8String operator+(UTF8String lhs, const StringViewLite& rhs)
    {
      lhs.Append(rhs);
      return lhs;
    }

    inline friend UTF8String operator+(UTF8String lhs, const UTF8String& rhs)
    {
      lhs.Append(rhs);
      return lhs;
    }

    inline friend UTF8String operator+(const char* const pszLhs, UTF8String rhs)
    {
      rhs.Prepend(StringViewLite(pszLhs));
      return rhs;
    }

    inline friend UTF8String operator+(const StringViewLite& lhs, UTF8String rhs)
    {
      rhs.Prepend(lhs);
      return rhs;
    }

    inline StringViewLite AsStringViewLite() const noexcept
    {
      // we call the special noexcept constructor here as we trust that std::string wont have a nullptr and a size > 0
      return StringViewLite(m_content.data(), m_content.size(), OptimizationCheckFlag::NoCheck);
    }
  };

  // Operator ==

  inline bool operator==(const UTF8String& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsStringViewLite() == rhs.AsStringViewLite();
  }

  inline bool operator==(const UTF8String& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() == pszRhs;
  }

  inline bool operator==(const UTF8String& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.AsStringViewLite() == rhs;
  }

  inline bool operator==(const UTF8String& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsStringViewLite() == StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator==(const char* const pszLhs, const UTF8String& rhs) noexcept
  {
    return pszLhs == rhs.AsStringViewLite();
  }

  inline bool operator==(const StringViewLite& lhs, const UTF8String& rhs) noexcept
  {
    return lhs == rhs.AsStringViewLite();
  }

  inline bool operator==(const std::string& lhs, const UTF8String& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) == rhs.AsStringViewLite();
  }

  // Operator !=

  inline bool operator!=(const UTF8String& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsStringViewLite() != rhs.AsStringViewLite();
  }


  inline bool operator!=(const UTF8String& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() != pszRhs;
  }

  inline bool operator!=(const UTF8String& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.AsStringViewLite() != rhs;
  }

  inline bool operator!=(const UTF8String& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsStringViewLite() != StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator!=(const char* const pszLhs, const UTF8String& rhs) noexcept
  {
    return pszLhs != rhs.AsStringViewLite();
  }

  inline bool operator!=(const StringViewLite& lhs, const UTF8String& rhs) noexcept
  {
    return lhs != rhs.AsStringViewLite();
  }

  inline bool operator!=(const std::string& lhs, const UTF8String& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) != rhs.AsStringViewLite();
  }

  // Operator <

  inline bool operator<(const UTF8String& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsStringViewLite() < rhs.AsStringViewLite();
  }

  inline bool operator<(const UTF8String& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() < pszRhs;
  }

  inline bool operator<(const UTF8String& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.AsStringViewLite() < rhs;
  }

  inline bool operator<(const UTF8String& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsStringViewLite() < StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator<(const char* const pszLhs, const UTF8String& rhs) noexcept
  {
    return pszLhs < rhs.AsStringViewLite();
  }

  inline bool operator<(const StringViewLite& lhs, const UTF8String& rhs) noexcept
  {
    return lhs < rhs.AsStringViewLite();
  }

  inline bool operator<(const std::string& lhs, const UTF8String& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) < rhs.AsStringViewLite();
  }

  // Operator <=

  inline bool operator<=(const UTF8String& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsStringViewLite() <= rhs.AsStringViewLite();
  }

  inline bool operator<=(const UTF8String& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() <= pszRhs;
  }

  inline bool operator<=(const UTF8String& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.AsStringViewLite() <= rhs;
  }

  inline bool operator<=(const UTF8String& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsStringViewLite() <= StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator<=(const char* const pszLhs, const UTF8String& rhs) noexcept
  {
    return pszLhs <= rhs.AsStringViewLite();
  }

  inline bool operator<=(const StringViewLite& lhs, const UTF8String& rhs) noexcept
  {
    return lhs <= rhs.AsStringViewLite();
  }

  inline bool operator<=(const std::string& lhs, const UTF8String& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) <= rhs.AsStringViewLite();
  }

  // Operator >

  inline bool operator>(const UTF8String& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsStringViewLite() > rhs.AsStringViewLite();
  }

  inline bool operator>(const UTF8String& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() > pszRhs;
  }

  inline bool operator>(const UTF8String& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.AsStringViewLite() > rhs;
  }

  inline bool operator>(const UTF8String& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsStringViewLite() > StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator>(const char* const pszLhs, const UTF8String& rhs) noexcept
  {
    return pszLhs > rhs.AsStringViewLite();
  }

  inline bool operator>(const StringViewLite& lhs, const UTF8String& rhs) noexcept
  {
    return lhs > rhs.AsStringViewLite();
  }

  inline bool operator>(const std::string& lhs, const UTF8String& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) > rhs.AsStringViewLite();
  }

  // Operator >=

  inline bool operator>=(const UTF8String& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsStringViewLite() >= rhs.AsStringViewLite();
  }

  inline bool operator>=(const UTF8String& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() >= pszRhs;
  }

  inline bool operator>=(const UTF8String& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.AsStringViewLite() >= rhs;
  }

  inline bool operator>=(const UTF8String& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsStringViewLite() >= StringViewLiteUtil::AsStringViewLite(rhs);
  }

  inline bool operator>=(const char* const pszLhs, const UTF8String& rhs) noexcept
  {
    return pszLhs >= rhs.AsStringViewLite();
  }

  inline bool operator>=(const StringViewLite& lhs, const UTF8String& rhs) noexcept
  {
    return lhs >= rhs.AsStringViewLite();
  }

  inline bool operator>=(const std::string& lhs, const UTF8String& rhs) noexcept
  {
    return StringViewLiteUtil::AsStringViewLite(lhs) >= rhs.AsStringViewLite();
  }
}

#endif
