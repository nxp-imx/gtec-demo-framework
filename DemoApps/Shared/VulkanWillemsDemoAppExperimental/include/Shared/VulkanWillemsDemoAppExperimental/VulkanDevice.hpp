#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANDEVICE_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANDEVICE_HPP
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
// It is not a straight port, but it has instead been converted to follow the RAII principle
// used in this framework

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanBuffer.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/Memory.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Willems
  {
    class VulkanDevice
    {
    public:
      //! Contains queue family indices
      struct QueueFamilyIndices
      {
        uint32_t Graphics{0};
        uint32_t Compute{0};

        QueueFamilyIndices() = default;
      };

    private:
      VkPhysicalDevice m_physicalDevice;
      VkDevice m_device;

      //! Properties of the physical device including limits that the application can check against.
      VkPhysicalDeviceProperties m_properties;
      //! Features of the physical device that an application can use to check if a feature is supported.
      VkPhysicalDeviceFeatures m_features;
      //! Memory types and heaps of the physical device
      VkPhysicalDeviceMemoryProperties m_memoryProperties;
      //! Queue family properties of the physical device
      std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
      //! Default command pool for the graphics queue family index
      RapidVulkan::CommandPool m_commandPool;
      // Set to true when the debug marker extension is detected
      // bool m_enableDebugMarkers;

      QueueFamilyIndices m_queueFamilyIndices;

    public:
      VulkanDevice(const VkPhysicalDevice physicalDevice, const VkDevice device);
      ~VulkanDevice();

      QueueFamilyIndices GetQueueFamilyIndices() const
      {
        return m_queueFamilyIndices;
      }

      //! @brief Get the index of a memory type that has all the requested property bits set
      //! @param typeBits Bitmask with bits set for each memory type supported by the resource to request for (from VkMemoryRequirements)
      //! @param properties Bitmask of properties for the memory type to request
      //! @return Index of the requested memory type
      //! @throw Throws an exception if memTypeFound is null and no memory type could be found that supports the requested properties
      uint32_t GetMemoryType(const uint32_t typeBits, const VkMemoryPropertyFlags properties) const;

      //! @brief Get the index of a queue family that supports the requested queue flags
      //! @param queueFlags Queue flags to find a queue family index for
      //! @return Index of the queue family index that matches the flags
      //! @throw Throws an exception if no queue family index could be found that supports the requested flags
      uint32_t GetQueueFamiliyIndex(const VkQueueFlagBits queueFlags) const;

      //! @brief Create a buffer on the device
      //! @param rBuffer the buffer object that will be created
      //! @param rMemory the memory object that will be created
      //! @param usageFlags Usage flag bitmask for the buffer(i.e.index, vertex, uniform buffer)
      //! @param memoryPropertyFlags Memory properties for this buffer(i.e.device local, host visible, coherent)
      //! @param size Size of the buffer in byes
      //! @param data Pointer to the data that should be copied to the buffer after creation(optional, if not set, no data is copied over)
      void CreateBuffer(RapidVulkan::Buffer& rBuffer, RapidVulkan::Memory& rMemory, const VkBufferUsageFlags usageFlags,
                        const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size, const void* pData = nullptr);

      //! @brief Create a buffer on the device
      //! @param usageFlags Usage flag bitmask for the buffer (i.e. index, vertex, uniform buffer)
      //! @param memoryPropertyFlags Memory properties for this buffer (i.e. device local, host visible, coherent)
      //! @param buffer Pointer to a vk::Vulkan buffer object
      //! @param size Size of the buffer in byes
      //! @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over)
      VulkanBuffer CreateBuffer(const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size,
                                const void* pData = nullptr);

      //! @brief Copy buffer data from src to dst using VkCmdCopyBuffer
      //! @param src Pointer to the source buffer to copy from
      //! @param dst Pointer to the destination buffer to copy tp
      //! @param queue Pointer
      //! @param copyRegion (Optional) Pointer to a copy region, if NULL, the whole buffer is copied
      //! @note Source and destionation pointers must have the approriate transfer usage flags set (TRANSFER_SRC / TRANSFER_DST)
      void CopyBuffer(const VulkanBuffer& src, VulkanBuffer& rDst, const VkQueue queue, const VkBufferCopy* pCopyRegion = nullptr);

      //! @brief Create a command pool for allocation command buffers from
      //! @param queueFamilyIndex Family index of the queue to create the command pool for
      //! @param createFlags (Optional) Command pool creation flags (Defaults to VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
      //! @note Command buffers allocated from the created pool can only be submitted to a queue with the same family index
      //! @return A handle to the created command buffer
      RapidVulkan::CommandPool CreateCommandPool(const uint32_t queueFamilyIndex,
                                                 const VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

      //! @brief Allocate a command buffer from the command pool
      //! @param level Level of the new command buffer (primary or secondary)
      //! @param (Optional) begin If true, recording on the new command buffer will be started (vkBeginCommandBuffer) (Defaults to false)
      //! @return A handle to the allocated command buffer
      RapidVulkan::CommandBuffer CreateCommandBuffer(const VkCommandBufferLevel level, const bool begin = false);

      //! @brief Finish command buffer recording and submit it to a queue
      //! @param commandBuffer Command buffer to flush
      //! @param queue Queue to submit the command buffer to
      //! @param free (Optional) Free the command buffer once it has been submitted (Defaults to true)
      //! @note The queue that the command buffer is submitted to must be from the same family index as the pool it was allocated from
      //! @note Uses a fence to ensure command buffer has finished executing
      void FlushCommandBuffer(RapidVulkan::CommandBuffer& rCommandBuffer, const VkQueue queue, const bool free = true);


      VkPhysicalDevice GetPhysicalDevice() const
      {
        return m_physicalDevice;
      }


      VkDevice GetDevice() const
      {
        return m_device;
      }

      const VkPhysicalDeviceProperties& GetProperties() const
      {
        return m_properties;
      }

      const VkPhysicalDeviceFeatures& GetFeatures() const
      {
        return m_features;
      }
    };
  }
}

#endif
