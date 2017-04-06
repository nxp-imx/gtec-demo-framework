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
#include <FslGraphicsVulkan1_0/Extend/SemaphoreEx.hpp>
#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <cassert>
#include <cstring>

namespace Fsl
{
  namespace Vulkan
  {
    // For now we implement the code here, if this turns out to be a performance problem we can move the code
    // to the header file to ensure its inlined.
    SemaphoreEx::SemaphoreEx()
      : m_semaphore()
      , m_createInfo{}
    {
    }


    SemaphoreEx::SemaphoreEx(VkDevice device, const VkSemaphoreCreateFlags flags)
      : SemaphoreEx() // delegating constructor C++11
    {
      Reset(device, { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, flags });
    }


    SemaphoreEx::SemaphoreEx(VkDevice device, const VkSemaphoreCreateInfo& createInfo)
      : SemaphoreEx() // delegating constructor C++11
    {
      Reset(device, createInfo);
    }


    SemaphoreEx::~SemaphoreEx()
    {
      Reset();
    }


    void SemaphoreEx::Reset()
    {
      if (! m_semaphore.IsValid())
        return;

      m_semaphore.Reset();
      m_createInfo = VkSemaphoreCreateInfo{};
    }


    void SemaphoreEx::Reset(VkDevice device, const VkSemaphoreCreateInfo& createInfo)
    {
      if (m_semaphore.IsValid())
        Reset();

      m_semaphore.Reset(device, createInfo);
      m_createInfo = createInfo;
    }


    void SemaphoreEx::Recreate()
    {
      if (! m_semaphore.IsValid())
        throw UsageErrorException("Object does not contain a semaphore, so it can not be recreated");

      // duplicate the member data since the reset modifies the variables
      const auto createInfo = m_createInfo;
      const auto device = m_semaphore.GetDevice();
      Reset(device, createInfo);
    }
  }
}
