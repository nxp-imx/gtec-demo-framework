#ifndef FSLUTIL_VULKAN1_0_BATCH_QUADBATCHRENDERRECORD_HPP
#define FSLUTIL_VULKAN1_0_BATCH_QUADBATCHRENDERRECORD_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslUtil/Vulkan1_0/Batch/QuadBatchDescriptorSets.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatchVertexBuffers.hpp>

namespace Fsl::Vulkan
{
  //! Contains things associated with a command buffer
  class QuadBatchRenderRecord final
  {
  public:
    QuadBatchRenderRecord(const QuadBatchRenderRecord&) = delete;
    QuadBatchRenderRecord& operator=(const QuadBatchRenderRecord&) = delete;

    QuadBatchDescriptorSets TextureDescriptorSets;
    QuadBatchVertexBuffers VertexBuffers;

    //! @brief Move assignment operator
    QuadBatchRenderRecord& operator=(QuadBatchRenderRecord&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        TextureDescriptorSets = std::move(other.TextureDescriptorSets);
        VertexBuffers = std::move(other.VertexBuffers);

        // Remove the data from other
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    QuadBatchRenderRecord(QuadBatchRenderRecord&& other) noexcept
      : TextureDescriptorSets(std::move(other.TextureDescriptorSets))
      , VertexBuffers(std::move(other.VertexBuffers))
    {
      // Remove the data from other
    }

    QuadBatchRenderRecord() = default;

    bool IsValid() const noexcept
    {
      return TextureDescriptorSets.IsValid() && VertexBuffers.IsValid();
    }

    void Reset() noexcept
    {
      // Use destruction order
      VertexBuffers.Reset();
      TextureDescriptorSets.Reset();
    }

    void Reset(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkDescriptorSetLayout descriptorSetLayout,
               const uint32_t minimumVertexCountRequest)
    {
      Reset();
      TextureDescriptorSets.Reset(device, descriptorSetLayout);
      VertexBuffers.Reset(physicalDevice, device, minimumVertexCountRequest);
    }

    void Clear() noexcept
    {
      // Use destruction order
      VertexBuffers.Clear();
      TextureDescriptorSets.Clear();
    }
  };
}

#endif
