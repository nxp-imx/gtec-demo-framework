#ifndef VULKAN_VULKANCOMPUTEMANDELBROT_VULKANCOMPUTEMANDELBROT_HPP
#define VULKAN_VULKANCOMPUTEMANDELBROT_VULKANCOMPUTEMANDELBROT_HPP
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

// Based on a sample by Norbert Nopper from VKTS Examples
// Recreated as a DemoFramework freestyle console sample by Freescale (2016)

#include <FslDemoApp/Console/DemoAppConsole.hpp>
#include <FslUtil/Vulkan1_0/Extend/PhysicalDeviceRecord.hpp>
#include <FslUtil/Vulkan1_0/VUDeviceQueueRecord.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/ComputePipeline.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/DescriptorSets.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/Device.hpp>
#include <RapidVulkan/Fence.hpp>
#include <RapidVulkan/Instance.hpp>
#include <RapidVulkan/Image.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include "DeviceBuffer.hpp"
#include "DeviceTexture.hpp"
#include "ImageData.hpp"

namespace Fsl
{
  class VulkanComputeMandelbrot : public DemoAppConsole
  {
    RapidVulkan::Instance m_instance;
    Vulkan::PhysicalDeviceRecord m_physicalDevice;
    RapidVulkan::Device m_device;
    Vulkan::VUDeviceQueueRecord m_deviceQueue;
    RapidVulkan::Fence m_fence;
    RapidVulkan::ShaderModule m_computeShaderModule;
    RapidVulkan::CommandPool m_commandPool;
    DeviceTexture m_currentTexture;
    RapidVulkan::ImageView m_imageView;
    RapidVulkan::DescriptorSetLayout m_descriptorSetLayout;
    RapidVulkan::DescriptorPool m_descriptorPool;
    RapidVulkan::DescriptorSets m_descriptorSets;
    RapidVulkan::PipelineLayout m_pipelineLayout;
    RapidVulkan::ComputePipeline m_pipeline;
    RapidVulkan::CommandBuffer m_commandBuffer;

    struct UpdateState
    {
      bool Executed;
      bool Informed;

      UpdateState()
        : Executed(false)
        , Informed(false)
      {
      }
    };
  public:
    VulkanComputeMandelbrot(const DemoAppConfig& config);
    ~VulkanComputeMandelbrot();
  protected:
    virtual void Run() override;
  private:
    bool Update(UpdateState& rState);

    ImageData GatherImageData();

    void Destroy();

    void BuildShader();
    void BuildCmdPool();
    void BuildTexture();
    void BuildDescriptorSetLayout();
    void BuildDescriptorSetPool();
    void BuildDescriptorSets();
    void BuildPipelineLayout();
    void BuildPipeline();
    void BuildCmdBuffer();

    DeviceBuffer CreateBuffer(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags) const;
    DeviceTexture CreateTexture(const VkDevice device, const VkImageTiling imageTiling, const VkImageUsageFlags usage, const VkImageLayout initialLayout, const VkMemoryPropertyFlags memoryPropertyFlagBits, const VkAccessFlags accessMask);

  };
}

#endif
