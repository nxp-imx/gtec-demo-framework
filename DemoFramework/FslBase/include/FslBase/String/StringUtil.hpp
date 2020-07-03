#ifndef FSLBASE_STRING_STRINGUTIL_HPP
#define FSLBASE_STRING_STRINGUTIL_HPP
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
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <string>
#include <vector>

namespace Fsl
{
  //! @brief This is a work in progress class
  namespace StringUtil
  {
    //! @brief Check if the string contains the character
    constexpr inline bool Contains(const StringViewLite& src, const char ch)
    {
      return src.find(ch) != StringViewLite::npos;
    }

    //! @brief Check if the string contains the character
    constexpr inline bool Contains(const char* const pszSrc, const char ch)
    {
      return StringViewLite(pszSrc).find(ch) != StringViewLite::npos;
    }

    //! @brief Check if the string contains the character
    inline bool Contains(const std::string& src, const char ch)
    {
      return Contains(StringViewLiteUtil::AsStringViewLite(src), ch);
    }


    //! @brief Check if the src-string contains the string
    bool Contains(const std::string& src, const std::string& str);


    //! @brief Check if the 'source' string starts with the character
    constexpr inline bool StartsWith(const StringViewLite& src, const char ch)
    {
      return src.starts_with(ch);
    }

    //! @brief Check if the 'source' string starts with the character
    constexpr inline bool StartsWith(const char* const pszSrc, const char ch)
    {
      return pszSrc != nullptr && pszSrc[0] == ch && ch != 0;
    }

    //! @brief Check if the 'source' string starts with the character
    inline bool StartsWith(const std::string& src, const char ch)
    {
      return (!src.empty() ? src.front() == ch : false);
    }

    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const StringViewLite& source, const StringViewLite& value)
    {
      return source.starts_with(value);
    }

    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const StringViewLite& source, const char* const pszValue)
    {
      return source.starts_with(pszValue);
    }

    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const StringViewLite& source, const std::string& value)
    {
      return source.starts_with(StringViewLiteUtil::AsStringViewLite(value));
    }


    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const char* const pszSource, const StringViewLite& value)
    {
      return StringViewLite(pszSource).starts_with(value);
    }

    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const char* const pszSource, const char* const pzValue)
    {
      return StringViewLite(pszSource).starts_with(pzValue);
    }

    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const char* const pszSource, const std::string& value)
    {
      return StringViewLite(pszSource).starts_with(StringViewLiteUtil::AsStringViewLite(value));
    }

    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const std::string& source, const StringViewLite& value)
    {
      return StringViewLiteUtil::AsStringViewLite(source).starts_with(StringViewLite(value));
    }

    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const std::string& source, const char* const pszValue)
    {
      return StringViewLiteUtil::AsStringViewLite(source).starts_with(pszValue);
    }

    //! @brief Check if the 'source' string starts with the 'value'
    inline bool StartsWith(const std::string& source, const std::string& value)
    {
      return StringViewLiteUtil::AsStringViewLite(source).starts_with(StringViewLiteUtil::AsStringViewLite(value));
    }

    //! @brief Check if the 'source' string ends with the character
    constexpr inline bool EndsWith(const StringViewLite& src, const char ch)
    {
      return src.ends_with(ch);
    }

    //! @brief Check if the 'source' string ends with the character
    constexpr inline bool EndsWith(const char* const pszSrc, const char ch)
    {
      return pszSrc != nullptr && StringViewLite(pszSrc).ends_with(ch);
    }

    //! @brief Check if the 'source' string ends with the character
    inline bool EndsWith(const std::string& src, const char ch)
    {
      return StringViewLiteUtil::AsStringViewLite(src).ends_with(ch);
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const StringViewLite& source, const StringViewLite& value)
    {
      return source.ends_with(value);
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const StringViewLite& source, const char* const pszValue)
    {
      return source.ends_with(pszValue);
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const StringViewLite& source, const std::string& value)
    {
      return source.ends_with(StringViewLiteUtil::AsStringViewLite(value));
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const char* const pszSource, const StringViewLite& value)
    {
      return StringViewLite(pszSource).ends_with(value);
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const char* const pszSource, const char* const pszValue)
    {
      return StringViewLite(pszSource).ends_with(pszValue);
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const char* const pszSource, const std::string& value)
    {
      return StringViewLite(pszSource).ends_with(StringViewLiteUtil::AsStringViewLite(value));
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const std::string& source, const StringViewLite& value)
    {
      return StringViewLiteUtil::AsStringViewLite(source).ends_with(value);
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const std::string& source, const char* const pszValue)
    {
      return StringViewLiteUtil::AsStringViewLite(source).ends_with(pszValue);
    }

    //! @brief Check if the 'source' string ends with the 'value'
    inline bool EndsWith(const std::string& source, const std::string& value)
    {
      return StringViewLiteUtil::AsStringViewLite(source).ends_with(StringViewLiteUtil::AsStringViewLite(value));
    }

    //! @brief Index of the first occurrence of the char 'ch' in 'source'
    //! @param fromIndex the index to start at. Beware that if fromIndex is out of bounds this returns a negative value.
    //! @return the index or less than zero if not found
    extern int32_t IndexOf(const StringViewLite& source, const char ch, const int32_t fromIndex = 0);

    //! @brief Index of the first occurrence of the char 'ch' in 'source'
    //! @param fromIndex the index to start at. Beware that if fromIndex is out of bounds this returns a negative value.
    //! @return the index or less than zero if not found
    inline int32_t IndexOf(const char* const pszSource, const char ch, const int32_t fromIndex = 0)
    {
      return IndexOf(StringViewLite(pszSource), ch, fromIndex);
    }

    //! @brief Index of the first occurrence of the char 'ch' in 'source'
    //! @param fromIndex the index to start at. Beware that if fromIndex is out of bounds this returns a negative value.
    //! @return the index or less than zero if not found
    inline int32_t IndexOf(const std::string& source, const char ch, const int32_t fromIndex = 0)
    {
      return IndexOf(StringViewLiteUtil::AsStringViewLite(source), ch, fromIndex);
    }

    //! @brief Index of the last occurrence of the char 'ch' in 'source' (searching backwards from the end to the beginning of the string).
    extern int32_t LastIndexOf(const StringViewLite& source, const char ch);

    //! @brief Index of the last occurrence of the char 'ch' in 'source' (searching backwards from the end to the beginning of the string).
    inline int32_t LastIndexOf(const char* const pszSource, const char ch)
    {
      return LastIndexOf(StringViewLite(pszSource), ch);
    }

    //! @brief Index of the last occurrence of the char 'ch' in 'source' (searching backwards from the end to the beginning of the string).
    inline int32_t LastIndexOf(const std::string& source, const char ch)
    {
      return LastIndexOf(StringViewLiteUtil::AsStringViewLite(source), ch);
    }

    //! @brief Index of the last occurrence of the char 'ch' in 'source' (searching backwards from the end to the beginning of the string).
    //! @param fromIndex the index to start at. Beware that if fromIndex is out of bounds this returns a negative value.
    //! @return the index or less than zero if not found
    extern int32_t LastIndexOf(const StringViewLite& source, const char ch, const int32_t fromIndex);

    //! @brief Index of the last occurrence of the char 'ch' in 'source' (searching backwards from the end to the beginning of the string).
    //! @param fromIndex the index to start at. Beware that if fromIndex is out of bounds this returns a negative value.
    //! @return the index or less than zero if not found
    inline int32_t LastIndexOf(const char* const pszSource, const char ch, const int32_t fromIndex)
    {
      return LastIndexOf(StringViewLite(pszSource), ch, fromIndex);
    }

    //! @brief Index of the last occurrence of the char 'ch' in 'source' (searching backwards from the end to the beginning of the string).
    //! @param fromIndex the index to start at. Beware that if fromIndex is out of bounds this returns a negative value.
    //! @return the index or less than zero if not found
    inline int32_t LastIndexOf(const std::string& source, const char ch, const int32_t fromIndex)
    {
      return LastIndexOf(StringViewLiteUtil::AsStringViewLite(source), ch, fromIndex);
    }


    //! @brief Replace all instances of the 'from' character with the 'to' character
    extern void Replace(std::string& rSrc, const char from, const char to);

    //! @brief Replace all instances of the old value in str with the new value
    extern void Replace(std::string& rStr, const std::string& oldValue, const std::string& newValue);

    //! @brief Replace all instances of the old value in str with the new value
    // static std::string Replace(const std::string& str, const std::string& oldValue, const std::string& newValue);

    extern std::vector<StringViewLite> Split(const StringViewLite& str, const char delimiterChar, const bool removeEmpty);

    //! @brief Split the 'str' at delimiterChar
    //! @param str = source string
    //! @param delimeterChar = the char to split at
    //! @param allowEmpty = if this is true then empty strings are allowed
    inline std::vector<StringViewLite> Split(const char* const psz, const char delimiterChar, const bool removeEmpty)
    {
      return Split(StringViewLite(psz), delimiterChar, removeEmpty);
    }

    //! @brief Split the 'str' at delimiterChar
    //! @param str = source string
    //! @param delimeterChar = the char to split at
    //! @param allowEmpty = if this is true then empty strings are allowed
    inline std::vector<StringViewLite> Split(const std::string& str, const char delimiterChar, const bool removeEmpty)
    {
      return Split(StringViewLiteUtil::AsStringViewLite(str), delimiterChar, removeEmpty);
    }

    extern std::vector<std::string> StringSplit(const StringViewLite& str, const char delimiterChar, const bool removeEmpty);

    //! @brief Split the 'str' at delimiterChar
    //! @param str = source string
    //! @param delimeterChar = the char to split at
    //! @param allowEmpty = if this is true then empty strings are allowed
    inline std::vector<std::string> StringSplit(const char* const psz, const char delimiterChar, const bool removeEmpty)
    {
      return StringSplit(StringViewLite(psz), delimiterChar, removeEmpty);
    }

    //! @brief Split the 'str' at delimiterChar
    //! @param str = source string
    //! @param delimeterChar = the char to split at
    //! @param allowEmpty = if this is true then empty strings are allowed
    inline std::vector<std::string> StringSplit(const std::string& str, const char delimiterChar, const bool removeEmpty)
    {
      return StringSplit(StringViewLiteUtil::AsStringViewLite(str), delimiterChar, removeEmpty);
    }
  };
}

#endif
