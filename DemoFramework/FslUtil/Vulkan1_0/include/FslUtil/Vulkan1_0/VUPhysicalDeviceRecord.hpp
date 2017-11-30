#ifndef FSLUTIL_VULKAN1_0_VUPHYSICALDEVICERECORD_HPP
#define FSLUTIL_VULKAN1_0_VUPHYSICALDEVICERECORD_HPP
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

#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/Extend/PhysicalDeviceRecord.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    //! This is contains information about a vulkan PhysicalDevice.
    //! This is not really a RAII object as it doesn't manage lifetime, hence the postfix Record.
    //! While you do have direct access to the members of this record, it is best to utilize the Constructor or Reset method
    //! to change the physical device.
    struct VUPhysicalDeviceRecord
    {
      PhysicalDeviceRecord Device;
      VkPhysicalDeviceProperties Properties;
      VkPhysicalDeviceFeatures Features;
      VkPhysicalDeviceMemoryProperties MemoryProperties;

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VUPhysicalDeviceRecord();
      VUPhysicalDeviceRecord(const VkPhysicalDevice physicalDevice);

      void Reset();
      void Reset(const VkPhysicalDevice physicalDevice);

      bool IsValid() const
      {
        return Device.IsValid();
      }

      //! @brief
      uint32_t GetMemoryTypeIndex(const uint32_t typeBits, const VkMemoryPropertyFlags properties) const;
    };
  }
}

#endif
