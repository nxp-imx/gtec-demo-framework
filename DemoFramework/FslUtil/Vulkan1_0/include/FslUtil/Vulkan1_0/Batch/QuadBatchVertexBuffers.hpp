#ifndef FSLUTIL_VULKAN1_0_BATCH_QUADBATCHVERTEXBUFFERS_HPP
#define FSLUTIL_VULKAN1_0_BATCH_QUADBATCHVERTEXBUFFERS_HPP
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

#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <deque>
#include <vector>

namespace Fsl
{
  struct VertexPositionColorTexture;

  namespace Vulkan
  {
    //! Simple vertex buffer allocator
    //! It uses a basic bucket allocation scheme.
    class QuadBatchVertexBuffers
    {
      VUPhysicalDeviceRecord m_physicalDevice;
      VkDevice m_device;
      uint32_t m_segmentVertexCount;
      std::deque<VUBuffer> m_buckets;

      uint32_t m_activeCount;
    public:
      struct VertexBufferEntry
      {
        VkBuffer VertexBuffer;
        VertexPositionColorTexture* pMapped;
        uint32_t VertexCapacity;
        //! This is the start index inside the buffer that was applied to pMapped
        uint32_t UsedStartIndex;

        VertexBufferEntry(const VkBuffer vertexBuffer, VertexPositionColorTexture* pMappedBuffer, const uint32_t vertexCapacity, const uint32_t usedStartIndex)
          : VertexBuffer(vertexBuffer)
          , pMapped(pMappedBuffer)
          , VertexCapacity(vertexCapacity)
          , UsedStartIndex(usedStartIndex)
        {
        }
      };

      QuadBatchVertexBuffers(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const uint32_t segmentVertexCount);
      ~QuadBatchVertexBuffers();

      void Clear();

      //! @brief Get the next free command buffer
      //! @param vertexCapacityHint is the desired capacity, but beware that the returned capacity can be less. However its
      //                            assumed that the entire returned capacity is claimed
      VertexBufferEntry NextFree(const uint32_t vertexCapacityHint);

    private:
    };
  }
}

#endif
