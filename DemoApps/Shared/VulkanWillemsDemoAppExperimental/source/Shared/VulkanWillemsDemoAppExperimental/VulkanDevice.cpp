/*
* Vulkan device class
*
* Encapsulates a physical Vulkan device and it's logical representation
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in VulkanDevice to make it easier
// to port samples.

#include <Shared/VulkanWillemsDemoAppExperimental/Config.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanDevice.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Fence.hpp>
#include <cassert>
#include <cstring>
#include <utility>

using namespace RapidVulkan;

namespace Fsl
{
  using namespace Vulkan;

  namespace Willems
  {
    namespace
    {
    }


    VulkanDevice::VulkanDevice(const VkPhysicalDevice physicalDevice, const VkDevice device)
      : m_physicalDevice(physicalDevice)
      , m_device(device)
      , m_properties{}
      , m_features{}
      , m_memoryProperties{}
      , m_queueFamilyProperties()
      , m_commandPool()
      , m_enableDebugMarkers(false)
      , m_queueFamilyIndices()
    {
      if (m_physicalDevice == VK_NULL_HANDLE || m_device == VK_NULL_HANDLE)
        throw std::invalid_argument("arguments can not be null");

      // Store Properties features, limits and properties of the physical device for later use
      // Device properties also contain limits and sparse properties
      vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
      // Features should be checked by the examples before using them
      vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);
      // Memory properties are used regularly for creating all kinds of buffer
      vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);

      // Queue family properties, used for setting up requested queues upon device creation
      m_queueFamilyProperties = PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(m_physicalDevice);
      if (m_queueFamilyProperties.size() == 0)
        throw NotSupportedException("Family count of zero is not supported");

      m_commandPool = CreateCommandPool(m_queueFamilyIndices.Graphics);
    }


    VulkanDevice::~VulkanDevice()
    {
    }


    uint32_t VulkanDevice::GetMemoryType(const uint32_t typeBits, const VkMemoryPropertyFlags properties) const
    {
      return MemoryTypeUtil::GetMemoryTypeIndex(m_memoryProperties, typeBits, properties);
    }


    uint32_t VulkanDevice::GetQueueFamiliyIndex(const VkQueueFlagBits queueFlags) const
    {
      // If a compute queue is requested, try to find a separate compute queue family from graphics first
      if (queueFlags & VK_QUEUE_COMPUTE_BIT)
      {
        for (uint32_t i = 0; i < static_cast<uint32_t>(m_queueFamilyProperties.size()); i++)
        {
          if ((m_queueFamilyProperties[i].queueFlags & queueFlags) && ((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
            return i;
        }
      }

      // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
      for (uint32_t i = 0; i < static_cast<uint32_t>(m_queueFamilyProperties.size()); i++)
      {
        if (m_queueFamilyProperties[i].queueFlags & queueFlags)
          return i;
      }
      throw std::runtime_error("Could not find a matching queue family index");
    }


    void VulkanDevice::CreateBuffer(RapidVulkan::Buffer& rBuffer, Memory& rMemory, const VkBufferUsageFlags usageFlags,
                                    const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size, const void* pData)
    {
      try
      {
        // Create the buffer handle
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.usage = usageFlags;
        bufferCreateInfo.size = size;
        bufferCreateInfo.flags = 0;

        rBuffer.Reset(m_device, bufferCreateInfo);

        // Create the memory backing up the buffer handle
        const VkMemoryRequirements memReqs = rBuffer.GetBufferMemoryRequirements();

        VkMemoryAllocateInfo memAllocInfo = {};
        memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAllocInfo.pNext = nullptr;
        memAllocInfo.allocationSize = 0;
        memAllocInfo.allocationSize = memReqs.size;
        memAllocInfo.memoryTypeIndex = 0;
        // Find a memory type index that fits the properties of the buffer
        memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);

        rMemory.Reset(m_device, memAllocInfo);

        // If a pointer to the buffer data has been passed, map the buffer and copy over the data
        if (pData != nullptr)
        {
          void* pMapped;
          rMemory.MapMemory(0, size, 0, &pMapped);
          {
            std::memcpy(pMapped, pData, size);
          }
          rMemory.UnmapMemory();
        }

        // Attach the memory to the buffer object
        RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device, rBuffer.Get(), rMemory.Get(), 0));
      }
      catch (const std::exception&)
      {
        rBuffer.Reset();
        rMemory.Reset();
        throw;
      }
    }


    VulkanBuffer VulkanDevice::CreateBuffer(const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size, const void* pData)
    {
      // Create the buffer handle
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.pNext = nullptr;
      bufferCreateInfo.flags = 0;
      bufferCreateInfo.size = size;
      bufferCreateInfo.usage = usageFlags;

      Buffer newBuffer(m_device, bufferCreateInfo);

      const VkMemoryRequirements memReqs = newBuffer.GetBufferMemoryRequirements();

      // Create the memory backing up the buffer handle
      VkMemoryAllocateInfo memAlloc{};
      memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memAlloc.pNext = nullptr;
      memAlloc.allocationSize = memReqs.size;
      // Find a memory type index that fits the properties of the buffer
      memAlloc.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);

      Memory newMemory(m_device, memAlloc);

      // If a pointer to the buffer data has been passed, map the buffer and copy over the data
      if (pData != nullptr)
      {
        void* pDst;
        newMemory.MapMemory(0, VK_WHOLE_SIZE, 0, &pDst);
        {
          std::memcpy(pDst, pData, size);
        }
        newMemory.UnmapMemory();
      }

      // Initialize a default descriptor that covers the whole buffer size
      VkDescriptorBufferInfo descriptor{};
      descriptor.buffer = newBuffer.Get();
      descriptor.offset = 0;
      descriptor.range = VK_WHOLE_SIZE;

      VulkanBuffer buffer(std::move(newBuffer), std::move(newMemory), descriptor, memAlloc.allocationSize, memReqs.alignment, usageFlags, memoryPropertyFlags);
      // Attach the memory to the buffer object
      buffer.Bind();
      return std::move(buffer);
    }


    void VulkanDevice::CopyBuffer(const VulkanBuffer& src, VulkanBuffer& rDst, const VkQueue queue, const VkBufferCopy* pCopyRegion)
    {
      assert(rDst.GetSize() <= src.GetSize());
      assert(src.IsValid() && rDst.IsValid());

      CommandBuffer copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
      VkBufferCopy bufferCopy{};
      if (pCopyRegion == nullptr)
      {
        bufferCopy.size = src.GetSize();
      }
      else
      {
        bufferCopy = *pCopyRegion;
      }

      vkCmdCopyBuffer(copyCmd.Get(), src.GetBuffer(), rDst.GetBuffer(), 1, &bufferCopy);

      FlushCommandBuffer(copyCmd, queue);
    }


    CommandPool VulkanDevice::CreateCommandPool(const uint32_t queueFamilyIndex, const VkCommandPoolCreateFlags createFlags)
    {
      VkCommandPoolCreateInfo cmdPoolInfo{};
      cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
      cmdPoolInfo.flags = createFlags;
      return CommandPool(m_device, cmdPoolInfo);
    }


    CommandBuffer VulkanDevice::CreateCommandBuffer(const VkCommandBufferLevel level, const bool begin)
    {
      VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
      cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufAllocateInfo.commandPool = m_commandPool.Get();
      cmdBufAllocateInfo.level = level;
      cmdBufAllocateInfo.commandBufferCount = 1;

      CommandBuffer cmdBuffer(m_device, cmdBufAllocateInfo);

      // If requested, also start recording for the new command buffer
      if (begin)
      {
        VkCommandBufferBeginInfo cmdBufInfo{};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufInfo.pNext = nullptr;
        cmdBuffer.Begin(cmdBufInfo);
      }
      return cmdBuffer;
    }


    void VulkanDevice::FlushCommandBuffer(RapidVulkan::CommandBuffer& rCommandBuffer, const VkQueue queue, const bool free)
    {
      if (! rCommandBuffer.IsValid())
      {
        FSLLOG_DEBUG_WARNING("Can not flush a invalid command buffer");
        return;
      }
      rCommandBuffer.End();

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.pNext = nullptr;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = rCommandBuffer.GetPointer();

      // Create fence to ensure that the command buffer has finished executing
      VkFenceCreateInfo fenceInfo{};
      fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceInfo.flags = 0;

      RapidVulkan::Fence fence(m_device, fenceInfo);

      // Submit to the queue
      RAPIDVULKAN_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence.Get()));
      // Wait for the fence to signal that command buffer has finished executing
      RAPIDVULKAN_CHECK(vkWaitForFences(m_device, 1, fence.GetPointer(), VK_TRUE, Config::DEFAULT_FENCE_TIMEOUT));

      if (free)
        rCommandBuffer.Reset();
    }
  }
}
