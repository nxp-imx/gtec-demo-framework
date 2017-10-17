#ifndef FSLUTIL_VULKAN1_0_VUDEVICEQUEUERECORD_HPP
#define FSLUTIL_VULKAN1_0_VUDEVICEQUEUERECORD_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Vulkan
  {
    //! This is contains information about a vulkan DeviceQueue.
    //! This is not really a RAII object as it doesn't manage lifetime, hence the postfix Record
    struct VUDeviceQueueRecord
    {
      VkDevice Device;
      uint32_t QueueFamilyIndex;
      uint32_t QueueIndex;
      VkQueue Queue;

      VUDeviceQueueRecord()
        : Device(VK_NULL_HANDLE)
        , QueueFamilyIndex(0)
        , QueueIndex(0)
        , Queue(VK_NULL_HANDLE)
      {
      }

      VUDeviceQueueRecord(const VkDevice device, const uint32_t queueFamilyIndex, const uint32_t queueIndex, const VkQueue queue)
        : Device(device)
        , QueueFamilyIndex(queueFamilyIndex)
        , QueueIndex(queueIndex)
        , Queue(queue)
      {
      }

      bool IsValid() const
      {
        return Queue != VK_NULL_HANDLE;
      }


      void Submit(const uint32_t submitCount, const VkSubmitInfo*const pSubmits, const VkFence fence)
      {
        RAPIDVULKAN_CHECK(vkQueueSubmit(Queue, submitCount, pSubmits, fence));
      }

      void WaitIdle()
      {
        RAPIDVULKAN_CHECK(vkQueueWaitIdle(Queue));
      }

    };
  }
}

#endif
