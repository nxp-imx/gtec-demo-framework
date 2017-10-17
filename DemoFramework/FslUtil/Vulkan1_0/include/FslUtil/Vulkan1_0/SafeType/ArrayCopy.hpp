#ifndef FSLUTIL_VULKAN1_0_SAFETYPE_ARRAYCOPY_HPP
#define FSLUTIL_VULKAN1_0_SAFETYPE_ARRAYCOPY_HPP
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
#include <FslBase/Exceptions.hpp>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    template <typename TSrc, typename TDst>
    class ArrayCopy
    {
      std::vector<TDst> m_arrayCopy;
      std::vector<TSrc> m_array;
    public:
      ArrayCopy(const ArrayCopy&) = delete;
      ArrayCopy& operator=(const ArrayCopy&) = delete;

      //! @brief Move assignment operator
      ArrayCopy& operator=(ArrayCopy&& other)
      {
        if (this != &other)
        {
          // Claim ownership here
          m_arrayCopy = std::move(other.m_arrayCopy);
          m_array = std::move(other.m_array);

          // Remove the data from other
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      ArrayCopy(ArrayCopy&& other)
        : m_arrayCopy(std::move(other.m_arrayCopy))
        , m_array(std::move(other.m_array))
      {
        // Remove the data from other
      }

      ArrayCopy()
        : m_arrayCopy()
        , m_array()
      {
      }

      ArrayCopy(const TSrc*const pSrc, const uint32_t entries)
        : m_arrayCopy(entries)
        , m_array(entries)
      {
        if (entries <= 0)
          return;
        if (pSrc == nullptr)
          throw std::invalid_argument("pSrc can not be null");

        for (uint32_t i = 0; i < entries; ++i)
        {
          m_arrayCopy[i] = std::move(TDst(pSrc[i]));
          m_array[i] = m_arrayCopy[i].Get();
        }
      }

      const TSrc* data() const
      {
        return m_array.data();
      }

      std::size_t size() const
      {
        return m_array.size();
      }
    };
  }
}

#endif
