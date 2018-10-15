#ifndef FSLUTIL_VULKAN1_0_SAFETYPE_STRINGARRAYCOPY_HPP
#define FSLUTIL_VULKAN1_0_SAFETYPE_STRINGARRAYCOPY_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Fsl
{
  namespace Vulkan
  {
    class StringArrayCopy
    {
      std::vector<std::string> m_stringArray;
      std::vector<const char*> m_stringPointers;

    public:
      StringArrayCopy(const StringArrayCopy&) = delete;
      StringArrayCopy& operator=(const StringArrayCopy&) = delete;

      StringArrayCopy& operator=(StringArrayCopy&& other) noexcept;
      StringArrayCopy(StringArrayCopy&& other) noexcept;

      StringArrayCopy();
      StringArrayCopy(const char* const* ppStrings, const uint32_t entries);

      const char* const* data() const
      {
        return m_stringPointers.data();
      }

      std::size_t size() const
      {
        return m_stringPointers.size();
      }
    };
  }
}

#endif
