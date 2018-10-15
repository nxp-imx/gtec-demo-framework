#ifndef FSLBASE_STRING_STRINGCOMPAT_HPP
#define FSLBASE_STRING_STRINGCOMPAT_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <cstdio>
#include <utility>

namespace Fsl
{
  //! Provides platform independent of standard string functionality that is missing on some platforms
  //! Try to not include this from any header file
  namespace StringCompat
  {
    //! @brief
    //! @note While snprintf is standard it is also more complex to simulate so we 'emulate' sprintf_S behavior instead
    //!       Use a variadic template to allow us to forward the argument list
    template <class... Args>
    int sprintf_s(char* s, const std::size_t n, const char* const pszFormat, Args&&... args)
    {
#ifdef _WIN32
      return ::sprintf_s(s, n, pszFormat, std::forward<Args>(args)...);
#else
      // emulate sprintf_s behavior
      const auto res = ::snprintf(s, n, pszFormat, std::forward<Args>(args)...);
      return (res >= 0 && static_cast<std::size_t>(res) < n) ? res : -1;
#endif
    }
  }
}

#endif
