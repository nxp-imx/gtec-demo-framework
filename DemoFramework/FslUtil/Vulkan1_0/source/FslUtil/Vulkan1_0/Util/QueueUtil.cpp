/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslUtil/Vulkan1_0/Util/QueueUtil.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    namespace QueueUtil
    {
      bool TryGetQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& allQueueFamilyProperties, const VkQueueFlags queueFlags,
                                  const uint32_t queueIndex, const std::vector<bool>* pFilter, uint32_t& rQueueFamilyIndex)
      {
        if ((pFilter != nullptr) && pFilter->size() != allQueueFamilyProperties.size())
        {
          throw std::invalid_argument("The filter size must match the allQueueFamilyProperties size");
        }

        for (uint32_t queueFamilyIndexWalker = 0; queueFamilyIndexWalker < allQueueFamilyProperties.size(); ++queueFamilyIndexWalker)
        {
          if (((allQueueFamilyProperties[queueFamilyIndexWalker].queueFlags & queueFlags) != 0u) &&
              allQueueFamilyProperties[queueFamilyIndexWalker].queueCount > queueIndex)
          {
            if ((pFilter == nullptr) || ((pFilter != nullptr) && (*pFilter)[queueFamilyIndexWalker]))
            {
              rQueueFamilyIndex = queueFamilyIndexWalker;
              return true;
            }
          }
        }
        return false;
      }


      uint32_t GetQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& allQueueFamilyProperties, const VkQueueFlags queueFlags,
                                   const uint32_t queueIndex, const std::vector<bool>* pFilter)
      {
        uint32_t queueFamilyIndex;
        if (TryGetQueueFamilyIndex(allQueueFamilyProperties, queueFlags, queueIndex, pFilter, queueFamilyIndex))
        {
          return queueFamilyIndex;
        }
        throw NotSupportedException("Could not find matching physical device queue properties");
      }
    }
  }
}
