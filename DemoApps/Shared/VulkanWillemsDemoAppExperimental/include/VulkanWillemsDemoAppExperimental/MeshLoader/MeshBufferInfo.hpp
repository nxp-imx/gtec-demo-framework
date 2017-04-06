#ifndef VULKANWILLEMSDEMOAPPEXPERIMENTAL_MESHLOADER_MESHBUFFERINFO_HPP
#define VULKANWILLEMSDEMOAPPEXPERIMENTAL_MESHLOADER_MESHBUFFERINFO_HPP
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

#include <FslGraphicsVulkan1_0/Common.hpp>
#include <FslGraphicsVulkan1_0/Buffer.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <vulkan/vulkan.h>
#include <cstdlib>

namespace Fsl
{
  namespace Willems
  {
    namespace MeshLoader
    {
      //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
      struct MeshBufferInfo
      {
        Vulkan::Buffer Buffer;
        Vulkan::Memory Memory;
        std::size_t Size;

        MeshBufferInfo(const MeshBufferInfo&) = delete;
        MeshBufferInfo& operator=(const MeshBufferInfo&) = delete;

        //! @brief Move assignment operator
        MeshBufferInfo& operator=(MeshBufferInfo&& other);

        //! @brief Move constructor
        //! Transfer ownership from other to this
        MeshBufferInfo(MeshBufferInfo&& other);

        //! @brief Move objects into this object
        MeshBufferInfo(Vulkan::Buffer&& buffer, Vulkan::Memory&& memory, const std::size_t size);

        //! @brief Create a 'invalid' instance (use Reset to populate it)
        MeshBufferInfo();

        ~MeshBufferInfo()
        {
          Reset();
        }

        //! @brief Destroys any owned resources and resets the object to its default state.
        void Reset();


        VkDevice GetDevice() const
        {
          return Buffer.GetDevice();
        }

        VkBuffer GetBuffer() const
        {
          return Buffer.Get();
        }

        const VkBuffer* GetBufferPointer() const
        {
          return Buffer.GetPointer();
        }

        VkDeviceMemory GetMemory() const
        {
          return Memory.Get();
        }

        const VkDeviceMemory* GetMemoryPointer() const
        {
          return Memory.GetPointer();
        }

        //! @brief Check if this object contains a valid resource
        inline bool IsValid() const
        {
          return Buffer.IsValid();
        }
      };
    }
  }
}

#endif
