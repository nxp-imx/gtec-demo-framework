#ifndef FSLBASE_STRING_STRINGVIEWLITEARRAYUTIL_HPP
#define FSLBASE_STRING_STRINGVIEWLITEARRAYUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <array>

namespace Fsl
{
  // Adds comparision operators for std::string
  // Added in a external file to ensure that not all StringViewLite users need to depend on std::string (and its header)

  namespace StringViewLiteArrayUtil
  {
    template <std::size_t T>
    void CopyToAndZeroTerminate(std::array<char, T>& rDst, StringViewLite strView)
    {
      static_assert(T > 0, "Array size must be larger than zero");

      const std::size_t maxCharsToCopy = std::min(rDst.size() - 1u, strView.size());
      for (std::size_t i = 0; i < maxCharsToCopy; ++i)
      {
        rDst[i] = strView[i];
      }
      rDst[maxCharsToCopy] = 0;
    }

    template <std::size_t T>
    std::array<char, T> ToArray(StringViewLite strView)
    {
      static_assert(T > 0, "Array size must be larger than zero");

      std::array<char, T> dst;
      const std::size_t maxCharsToCopy = std::min(dst.size() - 1u, strView.size());
      for (std::size_t i = 0; i < maxCharsToCopy; ++i)
      {
        dst[i] = strView[i];
      }
      dst[maxCharsToCopy] = 0;
      return dst;
    }
  }
}

#endif
