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

#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/MemoryTypeHelper.hpp>
#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    namespace MemoryTypeHelper
    {
      uint32_t GetMemoryTypeIndex(const uint32_t memoryTypeCount, const VkMemoryType*const pMemoryType, const uint32_t memoryTypeBits, const VkMemoryPropertyFlags propertyFlags)
      {
        if (memoryTypeCount == 0 || memoryTypeCount > VK_MAX_MEMORY_TYPES || !pMemoryType || memoryTypeBits == 0)
          throw std::invalid_argument("GetMemoryTypeIndex");

        const uint32_t maxMemoryTypes = std::max(32u, static_cast<uint32_t>(VK_MAX_MEMORY_TYPES));
        uint32_t searchMemoryBits = memoryTypeBits;
        for (uint32_t currentMemoryTypeIndex = 0; currentMemoryTypeIndex < maxMemoryTypes; ++currentMemoryTypeIndex)
        {
          if (searchMemoryBits & 1)
          {
            if ((pMemoryType[currentMemoryTypeIndex].propertyFlags & propertyFlags) == propertyFlags)
              return currentMemoryTypeIndex;
          }
          // Lower bits first.
          searchMemoryBits >>= 1;
        }
        throw NotSupportedException("Requested memory type not supported");
      }
    }
  }
}
