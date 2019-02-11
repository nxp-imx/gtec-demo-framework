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

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <FslBase/Log/Log.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace Willems
  {
    VulkanUniformData& VulkanUniformData::operator=(VulkanUniformData&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        Buffer = std::move(other.Buffer);
        Memory = std::move(other.Memory);
        Descriptor = other.Descriptor;
        AllocSize = other.AllocSize;
        pMapped = other.pMapped;

        // Remove the data from other
        other.Descriptor = VkDescriptorBufferInfo{};
        other.AllocSize = 0;
        other.pMapped = nullptr;
      }
      return *this;
    }


    VulkanUniformData::VulkanUniformData(VulkanUniformData&& other) noexcept
      : Buffer(std::move(other.Buffer))
      , Memory(std::move(other.Memory))
      , Descriptor(other.Descriptor)
      , AllocSize(other.AllocSize)
      , pMapped(other.pMapped)
    {
      // Remove the data from other
      other.Descriptor = VkDescriptorBufferInfo{};
      other.AllocSize = 0;
      other.pMapped = nullptr;
    }


    VulkanUniformData::VulkanUniformData(RapidVulkan::Buffer&& buffer, RapidVulkan::Memory&& deviceMemory, const VkDescriptorBufferInfo& descriptor,
                                         const uint32_t allocSize)
      : Buffer(std::move(buffer))
      , Memory(std::move(deviceMemory))
      , Descriptor(descriptor)
      , AllocSize(allocSize)
      , pMapped(nullptr)
    {
      const bool hasOneValid = Buffer.IsValid() || Memory.IsValid();
      if (Buffer.IsValid() != hasOneValid || Memory.IsValid() != hasOneValid)
      {
        throw std::invalid_argument("Either all objects has to be valid or none must be");
      }

      if (Buffer.GetDevice() != Memory.GetDevice())
      {
        throw std::invalid_argument("All objects must belong to the same device");
      }
    }


    VulkanUniformData::VulkanUniformData()
      : Descriptor{}
      , AllocSize(0)
      , pMapped(nullptr)
    {
    }


    void VulkanUniformData::Reset() noexcept
    {
      if (!IsValid())
      {
        return;
      }

      assert(Buffer.IsValid());
      assert(Memory.IsValid());
      //! If this goes of a mapped buffer is being reset, unmap it!
      if (pMapped != nullptr)
      {
        Unmap();
      }

      Buffer.Reset();
      Memory.Reset();
      Descriptor = VkDescriptorBufferInfo{};
      AllocSize = 0;
      pMapped = nullptr;
    }


    void VulkanUniformData::Map(const VkDeviceSize offset, const VkDeviceSize size)
    {
      assert(IsValid());
      RAPIDVULKAN_CHECK(vkMapMemory(Buffer.GetDevice(), Memory.Get(), offset, size, 0, &pMapped));
    }


    void VulkanUniformData::Unmap()
    {
      FSLLOG_DEBUG_WARNING_IF(!IsValid(), "Unmap called on a invalid object, request ignored.");
      FSLLOG_DEBUG_WARNING_IF(pMapped == nullptr, "Unmap called on a unmapped object, request ignored.");
      if (pMapped != nullptr)
      {
        vkUnmapMemory(Buffer.GetDevice(), Memory.Get());
        pMapped = nullptr;
      }
    }


    void VulkanUniformData::Bind(const VkDeviceSize offset)
    {
      assert(IsValid());
      RAPIDVULKAN_CHECK(vkBindBufferMemory(Buffer.GetDevice(), Buffer.Get(), Memory.Get(), offset));
    }
  }
}
