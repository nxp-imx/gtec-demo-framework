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

#include <VulkanWillemsDemoAppExperimental/MeshLoader/MeshBufferInfo.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphicsVulkan1_0/Check.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace Willems
  {
    namespace MeshLoader
    {
      MeshBufferInfo& MeshBufferInfo::operator = (MeshBufferInfo&& other)
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
            Reset();

          // Claim ownership here
          Buffer = std::move(other.Buffer);
          Memory = std::move(other.Memory);
          Size = other.Size;

          // Remove the data from other
          other.Size = 0;
        }
        return *this;
      }


      MeshBufferInfo::MeshBufferInfo(MeshBufferInfo&& other)
        : Buffer(std::move(other.Buffer))
        , Memory(std::move(other.Memory))
        , Size(other.Size)
      {
        // Remove the data from other
        other.Size = 0;
      }


      MeshBufferInfo::MeshBufferInfo(Vulkan::Buffer&& buffer, Vulkan::Memory&& memory, const std::size_t size)
        : Buffer(std::move(buffer))
        , Memory(std::move(memory))
        , Size(size)
      {
        const bool hasOneValid = Buffer.IsValid() || Memory.IsValid();
        if (Buffer.IsValid() != hasOneValid || Memory.IsValid() != hasOneValid)
          throw std::invalid_argument("Either all objects has to be valid or none must be");

        if (Buffer.GetDevice() != Memory.GetDevice())
          throw std::invalid_argument("All objects must belong to the same device");
      }


      MeshBufferInfo::MeshBufferInfo()
        : Buffer()
        , Memory()
        , Size(0)
      {
      }


      void MeshBufferInfo::Reset()
      {
        if (!IsValid())
          return;

        assert(Buffer.IsValid());
        assert(Memory.IsValid());
        Buffer.Reset();
        Memory.Reset();
        Size = 0;
      }
    }
  }
}
