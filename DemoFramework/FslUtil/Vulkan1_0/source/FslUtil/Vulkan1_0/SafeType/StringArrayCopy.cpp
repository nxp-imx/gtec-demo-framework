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

#include <FslUtil/Vulkan1_0/SafeType/StringArrayCopy.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    //! @brief Move assignment operator
    StringArrayCopy& StringArrayCopy::operator=(StringArrayCopy&& other) noexcept
    {
      if (this != &other)
      {
        // Claim ownership here
        m_stringArray = std::move(other.m_stringArray);
        m_stringPointers = std::move(other.m_stringPointers);

        // Remove the data from other
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    StringArrayCopy::StringArrayCopy(StringArrayCopy&& other) noexcept
      : m_stringArray(std::move(other.m_stringArray))
      , m_stringPointers(std::move(other.m_stringPointers))
    {
      // Remove the data from other
    }

    StringArrayCopy::StringArrayCopy() = default;


    StringArrayCopy::StringArrayCopy(const char* const* ppStrings, const uint32_t entries)
      : m_stringArray(entries)
      , m_stringPointers(entries)
    {
      if (entries <= 0)
      {
        return;
      }

      if (ppStrings == nullptr)
      {
        throw std::invalid_argument("ppStrings can not be null");
      }

      for (uint32_t i = 0; i < entries; ++i)
      {
        if (ppStrings[i] != nullptr)
        {
          m_stringArray[i] = ppStrings[i];
          m_stringPointers[i] = m_stringArray[i].c_str();
        }
        else
        {
          m_stringArray[i].clear();
          m_stringPointers[i] = nullptr;
        }
      }
    }
  }
}
