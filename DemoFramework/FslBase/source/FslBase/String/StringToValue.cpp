/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/String/StringToValue.hpp>

namespace Fsl::StringToValue
{
  bool TryParse(uint64_t& rResult, const StringViewLite& strView)
  {
    if (strView.data() == nullptr || strView.empty())
    {
      FSLLOG3_DEBUG_WARNING_IF(strView.data() == nullptr && strView.size() != 0u, "strView.data() can not be null");
      FSLLOG3_DEBUG_WARNING_IF(strView.size() == 0u, "strView.size() == 0");
      rResult = 0;
      return false;
    }

    uint64_t result = 0;
    const char* pSrc = strView.data();
    const char* const pSrcEnd = pSrc + strView.size();
    while (pSrc < pSrcEnd)
    {
      if (*pSrc < '0' || *pSrc > '9')
      {
        rResult = 0u;
        return false;
      }

      result = (result << 1) + (result << 3) + (*pSrc - '0');
      ++pSrc;
    }
    rResult = result;
    return true;
  }
}
