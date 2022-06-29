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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl::Vulkan::MemoryTypeUtil
{
  namespace
  {
    inline bool TryGetMemoryTypeIndexNow(uint32_t& rMemoryTypeIndex, const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
                                         const uint32_t memoryTypeBits, const VkMemoryPropertyFlags desiredPropertyFlags,
                                         const VkMemoryPropertyFlags notSetPropertyFlags)
    {
      if (physicalDeviceMemoryProperties.memoryTypeCount == 0 || physicalDeviceMemoryProperties.memoryTypeCount > VK_MAX_MEMORY_TYPES ||
          memoryTypeBits == 0)
      {
        throw std::invalid_argument("TryGetMemoryTypeIndex");
      }

      rMemoryTypeIndex = 0;

      const uint32_t maxMemoryTypes = physicalDeviceMemoryProperties.memoryTypeCount;
      uint32_t searchMemoryBits = memoryTypeBits;
      for (uint32_t currentMemoryTypeIndex = 0; currentMemoryTypeIndex < maxMemoryTypes; ++currentMemoryTypeIndex)
      {
        if ((searchMemoryBits & 1) != 0u)
        {
          if ((physicalDeviceMemoryProperties.memoryTypes[currentMemoryTypeIndex].propertyFlags & desiredPropertyFlags) == desiredPropertyFlags &&
              (physicalDeviceMemoryProperties.memoryTypes[currentMemoryTypeIndex].propertyFlags & notSetPropertyFlags) == 0u)
          {
            rMemoryTypeIndex = currentMemoryTypeIndex;
            return true;
          }
        }
        // Lower bits first.
        searchMemoryBits >>= 1;
      }
      return false;
    }
  }


  uint32_t GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties, const uint32_t memoryTypeBits,
                              const VkMemoryPropertyFlags desiredPropertyFlags, const VkMemoryPropertyFlags notSetPropertyFlags)
  {
    uint32_t memoryTypeIndex = 0;
    if (!TryGetMemoryTypeIndexNow(memoryTypeIndex, physicalDeviceMemoryProperties, memoryTypeBits, desiredPropertyFlags, notSetPropertyFlags))
    {
      throw NotSupportedException("Requested memory type not supported");
    }
    return memoryTypeIndex;
  }

  bool TryGetMemoryTypeIndex(uint32_t& rMemoryTypeIndex, const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
                             const uint32_t memoryTypeBits, const VkMemoryPropertyFlags desiredPropertyFlags,
                             const VkMemoryPropertyFlags notSetPropertyFlags)
  {
    return TryGetMemoryTypeIndexNow(rMemoryTypeIndex, physicalDeviceMemoryProperties, memoryTypeBits, desiredPropertyFlags, notSetPropertyFlags);
  }
}
