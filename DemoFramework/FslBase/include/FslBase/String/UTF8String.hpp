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
#include <string>

namespace Fsl
{
  class UTF8String
  {
    std::string m_content;

  public:
    // move assignment operator
    UTF8String& operator=(UTF8String&& other) noexcept;
    // move constructor
    UTF8String(UTF8String&& other) noexcept;
    // Request that the compiler generates a copy constructor and assignment operator
    UTF8String(const UTF8String&) = default;
    UTF8String& operator=(const UTF8String&) = default;

    UTF8String();
    explicit UTF8String(std::string str);
    UTF8String(const char* const psz);
    UTF8String(const std::string& str, const std::size_t startIndex, const std::size_t length);
    UTF8String(const UTF8String& str, const std::size_t startIndex, const std::size_t length);
    ~UTF8String();

    void Clear();

    bool IsEmpty() const;

    int32_t GetByteSize() const;
    bool Contains(const char ch) const;
    bool Contains(const UTF8String& str) const;
    bool StartsWith(const char ch) const;
    bool StartsWith(const UTF8String& str) const;
    bool EndsWith(const char ch) const;
    bool EndsWith(const UTF8String& path) const;
    void Replace(const char from, const char to);
    int32_t IndexOf(const char ch, const std::size_t fromIndex = 0) const;
    int32_t LastIndexOf(const char ch) const;

    const std::string& ToUTF8String() const
    {
      return m_content;
    }

    std::string ToAsciiString() const;

    void Reset(const char* const psz, const std::size_t startIndex, const std::size_t length);

    bool operator==(const UTF8String& rhs) const
    {
      return m_content == rhs.m_content;
    }

    bool operator!=(const UTF8String& rhs) const
    {
      return m_content != rhs.m_content;
    }

    bool operator<(const UTF8String& rhs) const
    {
      return m_content < rhs.m_content;
    }

    bool operator>(const UTF8String& rhs) const
    {
      return m_content > rhs.m_content;
    }

    bool operator<=(const UTF8String& rhs) const
    {
      return m_content <= rhs.m_content;
    }

    bool operator>=(const UTF8String& rhs) const
    {
      return m_content >= rhs.m_content;
    }
  };
}

#endif
