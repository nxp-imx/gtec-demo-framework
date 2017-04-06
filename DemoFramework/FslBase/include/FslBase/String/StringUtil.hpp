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

namespace Fsl
{

  //! @brief This is a work in progress class
  class StringUtil
  {
  public:

    //! @brief Check if the 'source' string starts with the 'value'
    static bool StartsWith(const std::string& source, const std::string& value);

    //! @brief Check if the 'source' string ends with the 'value'
    static bool EndsWith(const std::string& source, const std::string& value);


    //! @brief Index of the first occurrence of the char 'ch' in 'source'
    //! @return the index or less than zero if not found
    static int32_t IndexOf(const std::string& source, const char ch);


    //! @brief Index of the first occurrence of the char 'ch' in 'source'
    //! @return the index or less than zero if not found
    static int32_t IndexOf(const std::string& source, const char ch, const int32_t fromIndex);


    //! @brief Replace all instances of the old value in str with the new value
    static void Replace(std::string& str, const std::string& oldValue, const std::string& newValue);

    //! @brief Replace all instances of the old value in str with the new value
    //static std::string Replace(const std::string& str, const std::string& oldValue, const std::string& newValue);
  };
}

#endif
