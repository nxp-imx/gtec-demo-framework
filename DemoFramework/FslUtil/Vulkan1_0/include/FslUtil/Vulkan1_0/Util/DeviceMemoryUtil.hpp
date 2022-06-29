#ifndef FSLUTIL_VULKAN1_0_UTIL_DEVICEMEMORYUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_DEVICEMEMORYUTIL_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/SpanRange.hpp>
#include <FslUtil/Vulkan1_0/VUScopedMapMemory.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <cstring>
#include <stdexcept>

namespace Fsl::Vulkan::DeviceMemoryUtil
{
  struct MemorySpan
  {
    //! The span of the touched 'pages' (this span will be aligned to full 'pages')
    SpanRange<VkDeviceSize> Touched;
    //! The start index relative to the start of the 'page'
    VkDeviceSize RelativeStartOffset{};

    MemorySpan() = default;

    MemorySpan(SpanRange<VkDeviceSize> touched, VkDeviceSize relativeStartOffset)
      : Touched(touched)
      , RelativeStartOffset(relativeStartOffset)
    {
    }
  };

  //! @brief Determine all touched pages and a page relative start index
  //! @param length the length of the data (VK_WHOLE_SIZE is not handled here)
  //! @details This is intended for mapping non-coherent device memory in a way that allows us to write a buffer to any offset.
  //!          as this figures out the touched pages that need to be mapped and the page relative start index.
  inline MemorySpan CalcMemorySpan([[maybe_unused]] const VkDeviceSize capacity, const VkDeviceSize pageSize, const VkDeviceSize offset,
                                   const VkDeviceSize length)
  {
    assert(pageSize > 0);
    assert(pageSize <= capacity);
    assert(offset <= capacity);
    assert(length <= (capacity - offset));
    const auto finalPageSize = pageSize;
    const auto dstEndOffset = offset + length;
    const auto endPageIndex = dstEndOffset % finalPageSize;
    const auto endPage = (dstEndOffset / finalPageSize) + (endPageIndex != 0 ? 1 : 0);
    const auto startPage = offset / finalPageSize;

    // Calculate the 'pages' that need to be mapped for this update to be possible
    const VkDeviceSize touchedPageStart = finalPageSize * startPage;
    const VkDeviceSize touchedLength = finalPageSize * (endPage - startPage);
    // Calculate the start index inside the page.
    const VkDeviceSize pageRelativeStartOffset = offset % finalPageSize;

    // Validate the output
    assert(length <= touchedLength);
    assert(touchedPageStart <= capacity);
    assert(touchedLength <= (capacity - touchedPageStart));
    assert(pageRelativeStartOffset <= touchedLength);
    assert(length <= (touchedLength - pageRelativeStartOffset));

    return {SpanRange<VkDeviceSize>(touchedPageStart, touchedLength), pageRelativeStartOffset};
  }

  //! @brief Upload a memory buffer to host visible coherent device memory.
  //! @details The code will try to assert check most of the requirements, but its up to the caller to ensure all rules are followed.
  //! @param device the Vulkan device (!= VK_NULL_HANDLE)
  //! @param deviceMemory the device memory (!= VK_NULL_HANDLE)
  //! @param dstMemoryCapacity the total byte capacity of the device memory (used for sanity checking)
  //! @param deviceMemoryPropertyFlags the memory property flags (used for sanity checking)
  //! @param dstOffset The offset inside the device memory.
  //! @param flags the memory map flags
  //! @param pSrcData the that to upload (!= nullptr)
  //! @param srcDataSize the size of the srcData
  //! @warning This is a low level operation that expects that all parameters obey the Vulkan limits.
  //! @warning This only checks parameters using asserts.
  //! @warning This maps and unmaps the memory which could be a expensive operation, if the memory needs to be updated often consider leaving it
  //!          mapped while the frequent update is necessary.
  inline void FastUploadCoherent(const VkDevice device, const VkDeviceMemory deviceMemory, const VkDeviceSize dstMemoryCapacity,
                                 const VkMemoryPropertyFlags deviceMemoryPropertyFlags, const VkDeviceSize dstOffset, const VkMemoryMapFlags flags,
                                 const void* const pSrcData, const VkDeviceSize srcDataSize)
  {
    FSL_PARAM_NOT_USED(dstMemoryCapacity);
    FSL_PARAM_NOT_USED(deviceMemoryPropertyFlags);

    assert(device != VK_NULL_HANDLE);
    assert(deviceMemory != VK_NULL_HANDLE);
    assert((deviceMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0u);
    assert((deviceMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0u);
    assert(pSrcData != nullptr);
    assert(dstOffset <= dstMemoryCapacity);
    assert(srcDataSize <= (dstMemoryCapacity - dstOffset));

    // 1. Map the memory so we can access it
    // 2. Copy the src data to dst
    // 3. Unmap the memory
    {
      void* pDstMemory = nullptr;
      RapidVulkan::CheckError(vkMapMemory(device, deviceMemory, dstOffset, srcDataSize, flags, &pDstMemory), "vkMapMemory", __FILE__, __LINE__);

      std::memcpy(pDstMemory, pSrcData, srcDataSize);

      vkUnmapMemory(device, deviceMemory);
    }
  }

  //! @brief Upload a memory buffer to host visible non-coherent device memory.
  //! @details The code will try to assert check most of the requirements, but its up to the caller to ensure all rules are followed.
  //! @param device the Vulkan device (!= VK_NULL_HANDLE)
  //! @param physicalDeviceLimitNonCoherentAtomSize the device limits for NonCoherentAtomSize (> 0, used for sanity checking)
  //! @param deviceMemory the device memory (!= VK_NULL_HANDLE)
  //! @param dstMemoryCapacity the total byte capacity of the device memory (used for sanity checking)
  //! @param deviceMemoryPropertyFlags the memory property flags (used for sanity checking)
  //! @param dstDeviceMemory the device memory to write to.
  //! @param flags the memory map flags
  //! @param pSrcData the that to upload (!= nullptr)
  //! @param srcDataSize the size of the srcData
  //! @warning This is a low level operation that expects that all parameters obey the Vulkan limits.
  //! @warning This only checks parameters using asserts.
  //! @warning This maps and unmaps the memory which could be a expensive operation, if the memory needs to be updated often consider leaving it
  //!          mapped while the frequent update is necessary.
  inline void FastUploadNonCoherent(const VkDevice device, const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize,
                                    const VkDeviceMemory deviceMemory, const VkDeviceSize dstMemoryCapacity,
                                    const VkMemoryPropertyFlags deviceMemoryPropertyFlags, MemorySpan dstDeviceMemory, const VkMemoryMapFlags flags,
                                    const void* const pSrcData, const VkDeviceSize srcDataSize)
  {
    FSL_PARAM_NOT_USED(physicalDeviceLimitNonCoherentAtomSize);
    FSL_PARAM_NOT_USED(dstMemoryCapacity);
    FSL_PARAM_NOT_USED(deviceMemoryPropertyFlags);

    assert(device != VK_NULL_HANDLE);
    assert(physicalDeviceLimitNonCoherentAtomSize != 0);
    assert(deviceMemory != VK_NULL_HANDLE);
    assert((deviceMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0u);
    assert((deviceMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u);
    assert((dstDeviceMemory.Touched.Start % physicalDeviceLimitNonCoherentAtomSize) == 0);
    assert((dstDeviceMemory.Touched.Length % physicalDeviceLimitNonCoherentAtomSize) == 0 || dstDeviceMemory.Touched.Length == VK_WHOLE_SIZE);
    assert(dstDeviceMemory.Touched.Start <= dstMemoryCapacity);
    assert(dstDeviceMemory.Touched.Length <= (dstMemoryCapacity - dstDeviceMemory.Touched.Start));
    assert(pSrcData != nullptr);
    // Ensure that the src data can fit inside the requested 'mapped' memory area
    assert(dstDeviceMemory.RelativeStartOffset <= dstDeviceMemory.Touched.Length);
    assert(srcDataSize <= (dstDeviceMemory.Touched.Length - dstDeviceMemory.RelativeStartOffset));

    // 1. Map the memory so we can access it
    // 2. Copy the src data to dst
    // 3. Flush the changes
    // 4. Unmap the memory
    VkResult result = VK_SUCCESS;
    {
      void* pDstMemory = nullptr;
      RapidVulkan::CheckError(vkMapMemory(device, deviceMemory, dstDeviceMemory.Touched.Start, dstDeviceMemory.Touched.Length, flags, &pDstMemory),
                              "vkMapMemory", __FILE__, __LINE__);

      std::memcpy(static_cast<uint8_t*>(pDstMemory) + dstDeviceMemory.RelativeStartOffset, pSrcData, srcDataSize);

      {    // Since the memory is non-coherent we need to flush the memory
        VkMappedMemoryRange mappedMemoryRange{};
        mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedMemoryRange.memory = deviceMemory;
        mappedMemoryRange.offset = dstDeviceMemory.Touched.Start;
        mappedMemoryRange.size = dstDeviceMemory.Touched.Length;
        result = vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);
      }

      vkUnmapMemory(device, deviceMemory);
    }

    // We check the result of the flush after the unmap since we don't have to deal with a try catch block then. So be careful if this code is
    // extended.
    RapidVulkan::CheckError(result, "vkFlushMappedMemoryRanges", __FILE__, __LINE__);
  }

  //! @brief Upload the a memory buffer to device memory.
  //! @details This will automatically select the right method based on the deviceMemoryPropertyFlags. All parameter validations are done using
  //!          asserts. The mapping rules for NonCohorent memory is handled automatically and the 'updated' memory will be flushed as needed
  //!          before we unmap.
  //! @param device the Vulkan device (!= VK_NULL_HANDLE)
  //! @param physicalDeviceLimitNonCoherentAtomSize the device limits for NonCoherentAtomSize (> 0, used for sanity checking)
  //! @param deviceMemory the device memory (!= VK_NULL_HANDLE)
  //! @param dstMemoryCapacity the total byte capacity of the device memory (used for sanity checking)
  //! @param deviceMemoryPropertyFlags the memory property flags, this will be used to select a optimal upload for either Coherent or NonCoherent.
  //! @param dstOffset The offset inside the mapped span.
  //! @param flags the memory map flags
  //! @param pSrcData the that to upload (!= nullptr)
  //! @param srcDataSize the size of the srcData
  //! @warning This only checks parameters using asserts.
  //! @warning This maps and unmaps the memory which could be a expensive operation, if the memory needs to be updated often consider leaving it
  //!          mapped while the frequent update is necessary.
  inline void FastUpload(const VkDevice device, const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize, const VkDeviceMemory deviceMemory,
                         const VkDeviceSize dstMemoryCapacity, const VkMemoryPropertyFlags deviceMemoryPropertyFlags, const VkDeviceSize dstOffset,
                         const VkMemoryMapFlags flags, const void* const pSrcData, const VkDeviceSize srcDataSize)
  {
    assert(physicalDeviceLimitNonCoherentAtomSize != 0);
    assert((deviceMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0u);

    if ((deviceMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0u)
    {
      FastUploadCoherent(device, deviceMemory, dstMemoryCapacity, deviceMemoryPropertyFlags, dstOffset, flags, pSrcData, srcDataSize);
    }
    else
    {
      auto memorySpan = CalcMemorySpan(dstMemoryCapacity, physicalDeviceLimitNonCoherentAtomSize, dstOffset, srcDataSize);
      FastUploadNonCoherent(device, physicalDeviceLimitNonCoherentAtomSize, deviceMemory, dstMemoryCapacity, deviceMemoryPropertyFlags, memorySpan,
                            flags, pSrcData, srcDataSize);
    }
  }


  //! @brief Copy a memory buffer to host visible non-coherent device memory then vkFlushMappedMemoryRanges.
  //! @details The code will try to assert check most of the requirements, but its up to the caller to ensure all rules are followed.
  //! @param device the Vulkan device (!= VK_NULL_HANDLE)
  //! @param physicalDeviceLimitNonCoherentAtomSize the device limits for NonCoherentAtomSize (> 0, used for sanity checking)
  //! @param deviceMemory the device memory (!= VK_NULL_HANDLE)
  //! @param dstMemoryCapacity the total byte capacity of the device memory (used for sanity checking)
  //! @param deviceMemoryPropertyFlags the memory property flags (used for sanity checking)
  //! @param dstDeviceMemory the device memory chunks to write to.
  //! @param pSrcData the that to upload (!= nullptr)
  //! @param srcDataSize the size of the srcData
  //! @warning This is a low level operation that expects that all parameters obey the Vulkan limits.
  //! @warning This only checks parameters using asserts.
  //! @warning This maps and unmaps the memory which could be a expensive operation, if the memory needs to be updated often consider leaving it
  //!          mapped while the frequent update is necessary.
  inline VkResult TryFastCopyDataNonCoherent(const VkDevice device, const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize,
                                             const VkDeviceMemory deviceMemory, const VkDeviceSize dstMemoryCapacity,
                                             const VkMemoryPropertyFlags deviceMemoryPropertyFlags, MemorySpan dstDeviceMemory, void* pDstMemory,
                                             const void* const pSrcData, const VkDeviceSize srcDataSize)
  {
    FSL_PARAM_NOT_USED(physicalDeviceLimitNonCoherentAtomSize);
    FSL_PARAM_NOT_USED(dstMemoryCapacity);
    FSL_PARAM_NOT_USED(deviceMemoryPropertyFlags);

    assert(device != VK_NULL_HANDLE);
    assert(physicalDeviceLimitNonCoherentAtomSize != 0);
    assert(deviceMemory != VK_NULL_HANDLE);
    assert((deviceMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0u);
    assert((deviceMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u);
    assert((dstDeviceMemory.Touched.Start % physicalDeviceLimitNonCoherentAtomSize) == 0);
    assert((dstDeviceMemory.Touched.Length % physicalDeviceLimitNonCoherentAtomSize) == 0 || dstDeviceMemory.Touched.Length == VK_WHOLE_SIZE);
    assert(dstDeviceMemory.Touched.Start <= dstMemoryCapacity);
    assert(dstDeviceMemory.Touched.Length <= (dstMemoryCapacity - dstDeviceMemory.Touched.Start));
    assert(pSrcData != nullptr);
    // Ensure that the src data can fit inside the requested 'mapped' memory area
    assert(dstDeviceMemory.RelativeStartOffset <= dstDeviceMemory.Touched.Length);
    assert(srcDataSize <= (dstDeviceMemory.Touched.Length - dstDeviceMemory.RelativeStartOffset));

    // 1. Copy the src data to dst
    // 2. Flush the changes
    std::memcpy(static_cast<uint8_t*>(pDstMemory) + dstDeviceMemory.RelativeStartOffset, pSrcData, srcDataSize);

    // Since the memory is non-coherent we need to flush the memory
    VkMappedMemoryRange mappedMemoryRange{};
    mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedMemoryRange.memory = deviceMemory;
    mappedMemoryRange.offset = dstDeviceMemory.Touched.Start;
    mappedMemoryRange.size = dstDeviceMemory.Touched.Length;
    return vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);
  }
}

#endif
