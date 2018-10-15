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
#include <string>
#include <vector>

namespace Fsl
{
  //! @brief This is a work in progress class
  namespace StringUtil
  {
    bool Contains(const std::string& src, const char ch);
    bool Contains(const std::string& src, const std::string& str);

    //! @brief Check if the 'source' string starts with the character
    bool StartsWith(const std::string& src, const char ch);

    //! @brief Check if the 'source' string starts with the 'value'
    bool StartsWith(const std::string& source, const std::string& value);

    //! @brief Check if the 'source' string ends with the character
    bool EndsWith(const std::string& src, const char ch);

    //! @brief Check if the 'source' string ends with the 'value'
    bool EndsWith(const std::string& source, const std::string& value);


    //! @brief Index of the first occurrence of the char 'ch' in 'source'
    //! @return the index or less than zero if not found
    int32_t IndexOf(const std::string& source, const char ch);


    //! @brief Index of the first occurrence of the char 'ch' in 'source'
    //! @return the index or less than zero if not found
    int32_t IndexOf(const std::string& source, const char ch, const int32_t fromIndex);

    int32_t LastIndexOf(const std::string& src, const char ch);

    //! @brief Replace all instances of the 'from' character with the 'to' character
    void Replace(std::string& rSrc, const char from, const char to);

    //! @brief Replace all instances of the old value in str with the new value
    void Replace(std::string& rStr, const std::string& oldValue, const std::string& newValue);

    //! @brief Replace all instances of the old value in str with the new value
    // static std::string Replace(const std::string& str, const std::string& oldValue, const std::string& newValue);

    //! @brief Split the 'str' at delimiterChar
    //! @param str = source string
    //! @param delimeterChar = the char to split at
    //! @param allowEmpty = if this is true then empty strings are allowed
    std::vector<std::string> Split(const std::string& str, const char delimiterChar, const bool removeEmpty);
  };
}

#endif
