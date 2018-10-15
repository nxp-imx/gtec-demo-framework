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

#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslBase/Log/Log.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    VUPhysicalDeviceRecord::VUPhysicalDeviceRecord()
      : Properties{}
      , Features{}
      , MemoryProperties{}
    {
    }


    VUPhysicalDeviceRecord::VUPhysicalDeviceRecord(const VkPhysicalDevice physicalDevice)
      : VUPhysicalDeviceRecord()
    {
      Reset(physicalDevice);
    }


    void VUPhysicalDeviceRecord::Reset()
    {
      Device = PhysicalDeviceRecord();
      Properties = VkPhysicalDeviceProperties{};
      Features = VkPhysicalDeviceFeatures{};
      MemoryProperties = VkPhysicalDeviceMemoryProperties{};
    }


    void VUPhysicalDeviceRecord::Reset(const VkPhysicalDevice physicalDevice)
    {
      // Free any currently allocated resource
      if (IsValid())
      {
        Reset();
      }

      try
      {
        Device = PhysicalDeviceRecord(physicalDevice);
        Properties = Device.GetPhysicalDeviceProperties();
        Features = Device.GetPhysicalDeviceFeatures();
        MemoryProperties = Device.GetPhysicalDeviceMemoryProperties();
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }


    uint32_t VUPhysicalDeviceRecord::GetMemoryTypeIndex(const uint32_t typeBits, const VkMemoryPropertyFlags properties) const
    {
      return MemoryTypeUtil::GetMemoryTypeIndex(MemoryProperties, typeBits, properties);
    }
  }
}
