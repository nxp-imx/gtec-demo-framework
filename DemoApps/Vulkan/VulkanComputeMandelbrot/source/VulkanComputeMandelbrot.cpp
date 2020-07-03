/**
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Based on a sample by Norbert Nopper from VKTS Examples (VKTS_Sample08)
// Recreated as a DemoFramework freestyle console sample by Freescale (2016)

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslUtil/Vulkan1_0/Util/DeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/QueueUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include "OptionParser.hpp"
#include "VulkanComputeMandelbrot.hpp"
#include "SaveHelper.hpp"
#include <array>
#include <thread>

namespace Fsl
{
  namespace
  {
    // const auto IMAGE_NAME = "texture/mandelbrot.tga";
    const auto IMAGE_NAME = "mandelbrot.bmp";

    const auto COMPUTE_SHADER_NAME = "mandelbrot.comp.spv";

    // Has to be the same as in compute shader.
    const uint32_t IMAGE_LENGTH = 1024;
    const uint32_t LOCAL_SIZE = 16;
  }


  VulkanComputeMandelbrot::VulkanComputeMandelbrot(const DemoAppConfig& config)
    : DemoAppConsole(config)
  {
    try
    {
      {    // this was basically the setup done in main.cpp in the original example
        const auto options = config.GetOptions<OptionParser>();

        m_instance = Vulkan::InstanceUtil::CreateInstance("VulkanComputeMandelbrot", VK_MAKE_VERSION(1, 0, 0), VK_MAKE_VERSION(1, 0, 0), 0, 0,
                                                          nullptr, 0, nullptr);

        const uint32_t physicalDeviceIndex = options->GetPhysicalDeviceIndex();
        m_physicalDevice = Vulkan::VUPhysicalDeviceRecord(Vulkan::InstanceUtil::GetPhysicalDevice(m_instance.Get(), physicalDeviceIndex));

        const auto deviceQueueFamilyProperties = Vulkan::PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(m_physicalDevice.Device);
        const uint32_t queueFamilyIndex = Vulkan::QueueUtil::GetQueueFamilyIndex(deviceQueueFamilyProperties, VK_QUEUE_COMPUTE_BIT, 0, nullptr);

        std::array<float, 1> queuePriorities = {0.0f};
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.flags = 0;
        deviceQueueCreateInfo.queueFamilyIndex = 0;
        deviceQueueCreateInfo.queueCount = UncheckedNumericCast<uint32_t>(queuePriorities.size());
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();

        m_device.Reset(m_physicalDevice.Device, 0, 1, &deviceQueueCreateInfo, 0, nullptr, 0, nullptr, nullptr);

        m_deviceQueue = Vulkan::DeviceUtil::GetDeviceQueue(m_device.Get(), queueFamilyIndex, 0);
      }

      m_fence.Reset(m_device.Get(), 0);
      BuildShader();
      BuildCmdPool();
      BuildTexture();
      BuildDescriptorSetLayout();
      BuildDescriptorSetPool();
      BuildDescriptorSets();
      BuildPipelineLayout();
      BuildPipeline();
      BuildCmdBuffer();
    }
    catch (const std::exception&)
    {
      Destroy();
      throw;
    }
  }


  VulkanComputeMandelbrot::~VulkanComputeMandelbrot()
  {
    Destroy();
  }


  void VulkanComputeMandelbrot::Run()
  {
    // Dummy loop
    UpdateState state;
    while (Update(state))
    {
    }
  }


  bool VulkanComputeMandelbrot::Update(UpdateState& rState)
  {
    if (!rState.Executed)
    {
      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount = 0;
      submitInfo.pWaitSemaphores = nullptr;
      submitInfo.pWaitDstStageMask = nullptr;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = m_commandBuffer.GetPointer();
      submitInfo.signalSemaphoreCount = 0;
      submitInfo.pSignalSemaphores = nullptr;

      m_deviceQueue.Submit(1, &submitInfo, m_fence.Get());
      rState.Executed = VK_TRUE;
    }
    else
    {
      // Note: It is possible to wait (and block) for a fence.
      //      It is demonstrated, that the CPU can execute in parallel.
      if (m_fence.GetFenceStatus() == VK_SUCCESS)
      {
        FSLLOG3_INFO("Compute shader done.");
        //
        auto storedImage = GatherImageData();

        // SaveHelper::Save(IMAGE_NAME, storedImage);
        {    // FIX: quick fix to save for now
          RawBitmap rawBitmap;
          ImageData::ScopedRawBitmapAccess access(storedImage, rawBitmap);
          Bitmap bitmap(rawBitmap);
          GetPersistentDataManager()->Write(IMAGE_NAME, bitmap);
        }

        FSLLOG3_INFO("Saved image data as '{}'", IMAGE_NAME);
        // Returning false requests quitting the application.
        return false;
      }

      if (!rState.Informed)
      {
        FSLLOG3_INFO("Compute shader ongoing.");
        rState.Informed = VK_TRUE;
      }

      // Avoid "empty while" loop, as nothing is done.
      std::this_thread::yield();
    }
    return true;
  }


  ImageData VulkanComputeMandelbrot::GatherImageData()
  {
    RapidVulkan::Fence fence(m_device.Get(), 0);

    ImageData imageData(IMAGE_LENGTH, IMAGE_LENGTH, 1, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM);

    // Check, if we can use a linear tiled image for staging.
    if (m_physicalDevice.IsImageTilingAvailable(VK_IMAGE_TILING_LINEAR, imageData.GetPixelFormat(), imageData.GetImageType(), 0,
                                                imageData.GetExtent3D(), imageData.GetMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData.GetSize()))
    {
      FSLLOG3_INFO("Gather data using image tiling");

      DeviceTexture stageTexture = CreateTexture(m_device.Get(), VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 0);

      // Reset the command buffer
      RAPIDVULKAN_CHECK(vkResetCommandBuffer(m_commandBuffer.Get(), 0));

      m_commandBuffer.Begin(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
      {
        VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        // Prepare stage image for final layout etc.
        stageTexture.ImageCmdPipelineBarrier(m_commandBuffer.Get(), VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                             imageSubresourceRange);

        VkImageCopy imageCopy{};
        imageCopy.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        imageCopy.srcOffset = {0, 0, 0};
        imageCopy.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        imageCopy.dstOffset = {0, 0, 0};
        imageCopy.extent = {IMAGE_LENGTH, IMAGE_LENGTH, 1u};

        // Copy form device to host visible image / memory. This command also sets the needed barriers.
        m_currentTexture.CopyImage(m_commandBuffer.Get(), stageTexture.GetImage2(), imageCopy);

        stageTexture.ImageCmdPipelineBarrier(m_commandBuffer.Get(), VK_ACCESS_HOST_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, imageSubresourceRange);
      }
      m_commandBuffer.End();

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount = 0;
      submitInfo.pWaitSemaphores = nullptr;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = m_commandBuffer.GetPointer();
      submitInfo.signalSemaphoreCount = 0;
      submitInfo.pSignalSemaphores = nullptr;

      m_deviceQueue.Submit(1, &submitInfo, fence.Get());
      fence.WaitForFence(UINT64_MAX);

      // Copy pixel data from device memory into image data memory.

      VkImageSubresource imageSubresource{};
      imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      imageSubresource.mipLevel = 0;
      imageSubresource.arrayLayer = 0;

      VkSubresourceLayout subresourceLayout = stageTexture.GetImage().GetImageSubresourceLayout(imageSubresource);

      const auto allocationSize = stageTexture.GetAllocationSize();
      stageTexture.MapMemory(0, allocationSize, 0);
      {
        imageData.SetData(stageTexture.GetMappedMemoryPointer(), 0, 0, subresourceLayout);
        if ((stageTexture.GetMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u)
        {
          stageTexture.InvalidateMappedMemoryRanges(0, allocationSize);
        }
      }
      stageTexture.UnmapMemory();
      // Stage image and device memory are automatically destroyed.
    }
    else
    {
      // As an alternative, use the buffer.
      FSLLOG3_INFO("Gather data using buffer");

      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = imageData.GetSize();
      bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      bufferCreateInfo.flags = 0;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      bufferCreateInfo.queueFamilyIndexCount = 0;
      bufferCreateInfo.pQueueFamilyIndices = nullptr;

      auto stageBuffer = CreateBuffer(m_device.Get(), bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

      RAPIDVULKAN_CHECK(vkResetCommandBuffer(m_commandBuffer.Get(), 0));
      m_commandBuffer.Begin(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
      {
        VkBufferImageCopy bufferImageCopy{};
        bufferImageCopy.bufferOffset = 0;
        bufferImageCopy.bufferRowLength = IMAGE_LENGTH;
        bufferImageCopy.bufferImageHeight = IMAGE_LENGTH;
        bufferImageCopy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        bufferImageCopy.imageOffset = {0, 0, 0};
        bufferImageCopy.imageExtent = {IMAGE_LENGTH, IMAGE_LENGTH, 1};

        m_currentTexture.CopyImageToBuffer(m_commandBuffer.Get(), stageBuffer.GetBuffer2(), bufferImageCopy);
      }
      m_commandBuffer.End();


      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount = 0;
      submitInfo.pWaitSemaphores = nullptr;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = m_commandBuffer.GetPointer();
      submitInfo.signalSemaphoreCount = 0;
      submitInfo.pSignalSemaphores = nullptr;

      m_deviceQueue.Submit(1, &submitInfo, fence.Get());
      fence.WaitForFence(UINT64_MAX);

      // Copy pixel data from device memory into image data memory.

      VkSubresourceLayout subresourceLayout{};
      subresourceLayout.offset = 0;
      subresourceLayout.size = stageBuffer.GetBuffer().GetSize();
      subresourceLayout.rowPitch = IMAGE_LENGTH * 4 * sizeof(uint8_t);
      subresourceLayout.arrayPitch = IMAGE_LENGTH * IMAGE_LENGTH * 4 * sizeof(uint8_t);
      subresourceLayout.depthPitch = IMAGE_LENGTH * IMAGE_LENGTH * 4 * sizeof(uint8_t);

      stageBuffer.MapMemory(subresourceLayout.offset, subresourceLayout.size, 0);
      {
        imageData.SetData(stageBuffer.GetMappedMemoryPointer(), 0, 0, subresourceLayout);

        if ((stageBuffer.GetDeviceMemory().GetMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u)
        {
          stageBuffer.InvalidateMappedMemoryRanges(0, stageBuffer.GetDeviceMemory().GetAllocationSize());
        }
      }
      stageBuffer.UnmapMemory();
      // Stage image and device memory are automatically destroyed.
    }

    // Fence is automatically destroyed.
    return imageData;
  }


  void VulkanComputeMandelbrot::Destroy()
  {
    // Reverse allocation order
    // Note currently this sequence also fits with the reverse declaration order
    m_commandBuffer.Reset();
    m_pipeline.Reset();
    m_pipelineLayout.Reset();
    m_descriptorSets.Reset();
    m_descriptorPool.Reset();
    m_descriptorSetLayout.Reset();
    m_imageView.Reset();
    m_currentTexture.Reset();
    m_commandPool.Reset();
    m_computeShaderModule.Reset();
    m_fence.Reset();

    //    m_deviceQueue.Reset();
    m_device.Reset();

    // m_physicalDevice.Reset();
    m_instance.Reset();
  }


  void VulkanComputeMandelbrot::BuildShader()
  {
    const auto computeShaderBinary = GetContentManager()->ReadBytes(COMPUTE_SHADER_NAME);

    // VKTS_COMPUTE_SHADER_NAME
    m_computeShaderModule.Reset(m_device.Get(), 0, computeShaderBinary.size(), reinterpret_cast<const uint32_t*>(computeShaderBinary.data()));
  }


  void VulkanComputeMandelbrot::BuildCmdPool()
  {
    m_commandPool.Reset(m_device.Get(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_deviceQueue.QueueFamilyIndex);
  }


  void VulkanComputeMandelbrot::BuildTexture()
  {
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice.Device, VK_FORMAT_R8G8B8A8_UNORM, &formatProperties);

    // Check, if storage image is possible.
    if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) == 0u)
    {
      throw NotSupportedException("Format not supported");
    }

    m_currentTexture = CreateTexture(m_device.Get(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                                     VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);

    m_imageView.Reset(m_device.Get(), 0, m_currentTexture.GetImage().Get(), VK_IMAGE_VIEW_TYPE_2D, m_currentTexture.GetImageFormat(),
                      {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A},
                      {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
  }


  void VulkanComputeMandelbrot::BuildDescriptorSetLayout()
  {
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
    descriptorSetLayoutBinding.binding = 0;
    descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

    m_descriptorSetLayout.Reset(m_device.Get(), 0, 1, &descriptorSetLayoutBinding);
  }


  void VulkanComputeMandelbrot::BuildDescriptorSetPool()
  {
    VkDescriptorPoolSize descriptorTypeCount{};
    descriptorTypeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorTypeCount.descriptorCount = 1;

    m_descriptorPool.Reset(m_device.Get(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 1, &descriptorTypeCount);
  }


  void VulkanComputeMandelbrot::BuildDescriptorSets()
  {
    const VkDescriptorSetLayout currentDescriptorSetLayout = m_descriptorSetLayout.Get();
    m_descriptorSets.Reset(m_device.Get(), m_descriptorPool.Get(), 1, &currentDescriptorSetLayout);

    VkDescriptorImageInfo descriptorImageInfo{};
    descriptorImageInfo.sampler = VK_NULL_HANDLE;
    descriptorImageInfo.imageView = m_imageView.Get();
    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = m_descriptorSets[0];
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeDescriptorSet.pImageInfo = &descriptorImageInfo;
    writeDescriptorSet.pBufferInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    m_descriptorSets.UpdateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
  }


  void VulkanComputeMandelbrot::BuildPipelineLayout()
  {
    const VkDescriptorSetLayout currentDescriptorSet = m_descriptorSetLayout.Get();
    m_pipelineLayout.Reset(m_device.Get(), 0, 1, &currentDescriptorSet, 0, nullptr);
  }


  void VulkanComputeMandelbrot::BuildPipeline()
  {
    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{};
    pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo.flags = 0;
    pipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineShaderStageCreateInfo.module = m_computeShaderModule.Get();
    pipelineShaderStageCreateInfo.pName = "main";
    pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;

    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.stage = pipelineShaderStageCreateInfo;
    computePipelineCreateInfo.layout = m_pipelineLayout.Get();
    computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    computePipelineCreateInfo.basePipelineIndex = 0;

    m_pipeline.Reset(m_device.Get(), VK_NULL_HANDLE, computePipelineCreateInfo);
  }


  void VulkanComputeMandelbrot::BuildCmdBuffer()
  {
    m_commandBuffer.Reset(m_device.Get(), m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    m_commandBuffer.Begin(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      vkCmdBindPipeline(m_commandBuffer.Get(), VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline.Get());
      vkCmdBindDescriptorSets(m_commandBuffer.Get(), VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineLayout.Get(), 0, 1, m_descriptorSets.GetPointer(0), 0,
                              nullptr);

      VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

      m_currentTexture.ImageCmdPipelineBarrier(m_commandBuffer.Get(), VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, imageSubresourceRange);

      vkCmdDispatch(m_commandBuffer.Get(), IMAGE_LENGTH / LOCAL_SIZE, IMAGE_LENGTH / LOCAL_SIZE, 1);
    }
    m_commandBuffer.End();
  }


  DeviceBuffer VulkanComputeMandelbrot::CreateBuffer(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo,
                                                     const VkMemoryPropertyFlags memoryPropertyFlags) const
  {
    Vulkan::VUBuffer buffer(device, bufferCreateInfo.flags, bufferCreateInfo.size, bufferCreateInfo.usage, bufferCreateInfo.sharingMode,
                            bufferCreateInfo.queueFamilyIndexCount, bufferCreateInfo.pQueueFamilyIndices);

    // Get the memory requirements
    const auto memoryRequirements = buffer.GetBufferMemoryRequirements();
    const auto physicalDeviceMemoryProperties = m_physicalDevice.MemoryProperties;

    Vulkan::VUDeviceMemory deviceMemory(device, memoryRequirements, physicalDeviceMemoryProperties, memoryPropertyFlags,
                                        m_physicalDevice.Properties.limits.nonCoherentAtomSize);

    RAPIDVULKAN_CHECK(vkBindBufferMemory(device, buffer.Get(), deviceMemory.Get(), 0));
    return DeviceBuffer(std::move(buffer), std::move(deviceMemory));
  }


  DeviceTexture VulkanComputeMandelbrot::CreateTexture(const VkDevice device, const VkImageTiling imageTiling, const VkImageUsageFlags usage,
                                                       const VkImageLayout initialLayout, const VkMemoryPropertyFlags memoryPropertyFlagBits,
                                                       const VkAccessFlags accessMask)
  {
    VkExtent3D extent = {IMAGE_LENGTH, IMAGE_LENGTH, 1};

    const VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    ImageEx image(device, 0, VK_IMAGE_TYPE_2D, imageFormat, extent, 1, 1, VK_SAMPLE_COUNT_1_BIT, imageTiling, usage, VK_SHARING_MODE_EXCLUSIVE, 0,
                  nullptr, initialLayout, accessMask);

    // Get the memory requirements
    const auto memoryRequirements = image.GetImageMemoryRequirements();
    const auto physicalDeviceMemoryProperties = m_physicalDevice.MemoryProperties;

    Vulkan::VUDeviceMemory deviceMemory(device, memoryRequirements, physicalDeviceMemoryProperties, memoryPropertyFlagBits,
                                        m_physicalDevice.Properties.limits.nonCoherentAtomSize);

    // Bind image to memory.
    RAPIDVULKAN_CHECK(vkBindImageMemory(device, image.Get(), deviceMemory.Get(), 0));

    // move the local stack based objects into the Texture class
    return DeviceTexture(std::move(image), std::move(deviceMemory), imageFormat);
  }
}
