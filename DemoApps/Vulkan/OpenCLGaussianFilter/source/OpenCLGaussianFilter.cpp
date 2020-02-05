/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include "OpenCLGaussianFilter.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Service/BitmapConverter/IBitmapConverter.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenCL1_1/ContextEx.hpp>
#include <FslUtil/OpenCL1_1/OpenCLHelper.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <RapidOpenCL1/Buffer.hpp>
#include <RapidOpenCL1/Check.hpp>
#include <RapidOpenCL1/CommandQueue.hpp>
#include <RapidOpenCL1/DebugStrings.hpp>
#include <RapidOpenCL1/Kernel.hpp>
#include <RapidOpenCL1/Program.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <vector>

using namespace RapidOpenCL1;

namespace Fsl
{
  using namespace Vulkan;
  using namespace OpenCL;

  namespace
  {
    const auto VERTEX_BUFFER_BIND_ID = 0;


    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      return setup;
    }

    void PrintInfo(const cl_platform_id rplatformId, const cl_device_id deviceId)
    {
      const int arrSize = 4;
      const char* attributeNames[arrSize] = {"Name", "Vendor", "Version", "Profile"};
      const cl_platform_info attributeTypes[arrSize] = {CL_PLATFORM_NAME, CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE};
      const cl_device_info deviceAttributeTypes[arrSize] = {CL_DEVICE_NAME, CL_DEVICE_VENDOR, CL_DEVICE_VERSION, CL_DEVICE_PROFILE};
      const int attributeCount = sizeof(attributeNames) / sizeof(char*);

      FSLLOG3_INFO("\n-=-=-=- Platform and Device information -=-=-=-\n\n");

      for (int count = 0; count < attributeCount; ++count)
      {
        const std::string info = OpenCLHelper::GetPlatformInfo<std::string>(rplatformId, attributeTypes[count]);
        const std::string deviceInfo = OpenCLHelper::GetDeviceInfo<std::string>(deviceId, deviceAttributeTypes[count]);

        FSLLOG3_INFO("Platform Attributes {}: {}", attributeNames[count], info);
        FSLLOG3_INFO("Device Attributes {}: {}", attributeNames[count], deviceInfo);
      }

      const auto deviceItems = OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
      FSLLOG3_INFO("Device Max Work Item Dimensions: {}-D", deviceItems);

      const auto deviceSize = OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE);
      FSLLOG3_INFO("Device Max Work Group Size: {}", deviceSize);

      FSLLOG3_INFO("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    }


    Program BuildProgram(const cl_context context, const cl_device_id deviceId, const std::string& kernelSrc)
    {
      const char* pszKernelSource = kernelSrc.c_str();
      const std::size_t kernelSize = kernelSrc.size();

      Program program(context, 1, &pszKernelSource, &kernelSize);

      const cl_int error = clBuildProgram(program.Get(), 1, &deviceId, "", nullptr, nullptr);
      if (error != CL_SUCCESS)
      {
        const auto buildInfo = OpenCLHelper::GetProgramBuildInfoString(program.Get(), deviceId, CL_PROGRAM_BUILD_LOG);
        FSLLOG3_INFO(buildInfo);
        RAPIDOPENCL_CHECK(error);
      }
      return program;
    }

    void ProcessBitmapUsingOpenCL(Bitmap& rBitmap, const std::string& strProgram)
    {
      ContextEx context;
      cl_device_id deviceId;
      context.Reset(CL_DEVICE_TYPE_GPU, &deviceId);

      CommandQueue commandQueue(context.Get(), deviceId, 0);

      PrintInfo(context.GetPlatformId(), deviceId);

      Program program = BuildProgram(context.Get(), deviceId, strProgram);
      Kernel kernel(program.Get(), "gaussian_filter");

      const std::size_t size2d = rBitmap.Width() * rBitmap.Height();
      Buffer gaussMemInput(context.Get(), CL_MEM_READ_ONLY, size2d, nullptr);
      Buffer gaussMemOutput(context.Get(), CL_MEM_WRITE_ONLY, size2d, nullptr);

      const auto width = static_cast<cl_int>(rBitmap.Width());
      const auto height = static_cast<cl_int>(rBitmap.Height());
      const cl_int channels = 1;
      clSetKernelArg(kernel.Get(), 0, sizeof(cl_mem), gaussMemInput.GetPointer());     // input
      clSetKernelArg(kernel.Get(), 1, sizeof(cl_mem), gaussMemOutput.GetPointer());    // output
      clSetKernelArg(kernel.Get(), 2, sizeof(cl_int), &width);                         // width
      clSetKernelArg(kernel.Get(), 3, sizeof(cl_int), &height);                        // height
      clSetKernelArg(kernel.Get(), 4, sizeof(cl_int), &channels);                      // channels

      {
        RawBitmapEx rawBitmap;
        Bitmap::ScopedDirectAccess scopedAccess(rBitmap, rawBitmap);
        auto* pImgData = static_cast<uint8_t*>(rawBitmap.Content());

        const std::size_t global[2] = {rawBitmap.Width(), rawBitmap.Height()};
        const int dimension = 2;
        std::size_t local[dimension] = {16, 8};
        RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), gaussMemInput.Get(), CL_TRUE, 0, size2d, pImgData, 0, nullptr, nullptr));
        RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernel.Get(), dimension, nullptr, global, local, 0, nullptr, nullptr));
        RAPIDOPENCL_CHECK(clEnqueueReadBuffer(commandQueue.Get(), gaussMemOutput.Get(), CL_TRUE, 0, size2d, pImgData, 0, nullptr, nullptr));
      }
    }


    // Vulkan

    std::array<VertexPositionTexture, 24> g_vertices = {
      // Draw A Quad

      // Top Right Of The Quad (Top)
      VertexPositionTexture(Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0f, 0.0f)),
      // Top Left Of The Quad (Top)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Right Of The Quad (Top)
      VertexPositionTexture(Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Left Of The Quad (Top)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f)),
      // Top Right Of The Quad (Bottom)
      VertexPositionTexture(Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0f, 1.0f)),
      // Top Left Of The Quad (Bottom)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Right Of The Quad (Bottom)
      VertexPositionTexture(Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Left Of The Quad (Bottom)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f)),
      // Top Right Of The Quad (Front)
      VertexPositionTexture(Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f)),
      // Top Left Of The Quad (Front)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Right Of The Quad (Front)
      VertexPositionTexture(Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Left Of The Quad (Front)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f)),

      // Top Right Of The Quad (Back)
      VertexPositionTexture(Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f)),
      // Top Left Of The Quad (Back)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Right Of The Quad (Back)
      VertexPositionTexture(Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Left Of The Quad (Back)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)),

      // Top Right Of The Quad (Left)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f)),
      // Top Left Of The Quad (Left)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Right Of The Quad (Left)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Left Of The Quad (Left)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f)),
      // Top Right Of The Quad (Right)
      VertexPositionTexture(Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)),
      // Top Left Of The Quad (Right)
      VertexPositionTexture(Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Right Of The Quad (Right)
      VertexPositionTexture(Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Left Of The Quad (Right)
      VertexPositionTexture(Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f))};


    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Bitmap& bitmap)
    {
      VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = 1.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(bitmap, samplerCreateInfo);
    }


    OpenCLGaussianFilter::SimpleMesh CreateMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                const std::array<VertexElementUsage, 2>& shaderBindOrder)
    {
      OpenCLGaussianFilter::SimpleMesh mesh;
      mesh.VertexBuffer.Reset(bufferManager, g_vertices, VMBufferUsage::STATIC);

      VMVertexBufferUtil::FillVertexInputAttributeDescription(mesh.VertexAttributeDescription, shaderBindOrder, mesh.VertexBuffer);
      mesh.VertexInputBindingDescription.binding = 0;
      mesh.VertexInputBindingDescription.stride = mesh.VertexBuffer.GetElementStride();
      mesh.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      return mesh;
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

      // Binding 1 : Fragment shader image sampler
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return RapidVulkan::DescriptorSetLayout(device.Get(), descriptorLayout);
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return RapidVulkan::DescriptorPool(device.Get(), descriptorPoolInfo);
    }


    Vulkan::VUBufferMemory CreateUBO(const Vulkan::VUDevice& device, const VkDeviceSize size)
    {
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = size;
      bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      Vulkan::VUBufferMemory buffer(device, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      // Keep the buffer mapped as we update it each frame
      buffer.Map();
      return buffer;
    }


    VkDescriptorSet CreateDescriptorSet(const RapidVulkan::DescriptorPool& descriptorPool,
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout)
    {
      assert(descriptorPool.IsValid());
      assert(descriptorSetLayout.IsValid());

      // Allocate a new descriptor set from the global descriptor pool
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool.Get();
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      VkDescriptorSet descriptorSet;
      RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets", __FILE__,
                              __LINE__);

      return descriptorSet;
    }


    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer,
                                        const Vulkan::VUTexture& texture)

    {
      assert(descriptorSet != nullptr);
      assert(vertUboBuffer.IsValid());
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
      // Binding 0 : Vertex shader uniform buffer
      auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      // Binding 1 : Fragment shader texture sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureImageInfo;

      vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

      return descriptorSet;
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

      return RapidVulkan::PipelineLayout(descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo);
    }


    RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                 const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                 const OpenCLGaussianFilter::SimpleMesh& mesh, const VkRenderPass renderPass, const uint32_t subpass)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);
      assert(!mesh.VertexAttributeDescription.empty());

      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};
      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
      pipelineShaderStageCreateInfo[1].pName = "main";
      pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

      VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
      pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &mesh.VertexInputBindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mesh.VertexAttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = mesh.VertexAttributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
      pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.width = static_cast<float>(extent.width);
      viewport.height = static_cast<float>(extent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{{0, 0}, extent};

      VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
      pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      pipelineViewportStateCreateInfo.viewportCount = 1;
      pipelineViewportStateCreateInfo.pViewports = &viewport;
      pipelineViewportStateCreateInfo.scissorCount = 1;
      pipelineViewportStateCreateInfo.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
      pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
      pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
      pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
      pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
      pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
      pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

      VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
      pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
      pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
      pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
      pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
      pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

      VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
      pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
      pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
      pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
      pipelineColorBlendAttachmentState.colorWriteMask = 0xf;

      VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
      pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
      pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
      pipelineColorBlendStateCreateInfo.attachmentCount = 1;
      pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
      pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
      pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

      VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
      depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
      depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
      depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
      depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.front = {};
      depthStencilStateCreateInfo.back = {};
      depthStencilStateCreateInfo.minDepthBounds = 0.0f;
      depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

      VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
      graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfo.size());
      graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
      graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
      graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
      graphicsPipelineCreateInfo.pTessellationState = nullptr;
      graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
      graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
      graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
      graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
      graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
      graphicsPipelineCreateInfo.pDynamicState = nullptr;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = subpass;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return RapidVulkan::GraphicsPipeline(pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    }
  }


  OpenCLGaussianFilter::OpenCLGaussianFilter(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_bufferManager(std::make_shared<VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();

    Bitmap bitmap;
    content->Read(bitmap, "Test.png", PixelFormat::EX_ALPHA8_UNORM, BitmapOrigin::LowerLeft);
    GetPersistentDataManager()->Write("Test_gray.bmp", bitmap);

    ProcessBitmapUsingOpenCL(bitmap, content->ReadAllText("gaussian_filter.cl"));

    // Write the result to a file so we can take a detailed look at it
    GetPersistentDataManager()->Write("Test_gauss.bmp", bitmap);

    // As Vulkan is picky about the pixel formats it supports we convert the image to a supported format
    auto converter = config.DemoServiceProvider.Get<IBitmapConverter>();
    // converter->Convert(bitmap, PixelFormat::R8_UNORM);
    converter->Convert(bitmap, PixelFormat::R8G8B8A8_UNORM);

    // We also use it as a texture
    m_resources.Texture = CreateTexture(m_device, m_deviceQueue, bitmap);

    // Vulkan setup
    m_resources.VertShaderModule.Reset(m_device.Get(), 0, content->ReadBytes("Shader.vert.spv"));
    m_resources.FragShaderModule.Reset(m_device.Get(), 0, content->ReadBytes("Shader.frag.spv"));

    std::array<VertexElementUsage, 2> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};
    m_resources.Mesh = CreateMesh(m_bufferManager, shaderBindOrder);

    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);
    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight);

    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.VertUboBuffer = CreateUBO(m_device, sizeof(VertexUBOData));
      rFrame.DescriptorSet = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
      UpdateDescriptorSet(m_device.Get(), rFrame.DescriptorSet, rFrame.VertUboBuffer, m_resources.Texture);
    }
    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);

    PrepareMatrices(config.ScreenResolution);
  }


  void OpenCLGaussianFilter::Update(const DemoTime& demoTime)
  {
    m_angle.X -= 0.60f * demoTime.DeltaTime;
    m_angle.Y -= 0.50f * demoTime.DeltaTime;
    m_angle.Z -= 0.40f * demoTime.DeltaTime;

    // Rotate and translate the model view matrix
    m_vertexUboData.MatModel =
      Matrix::CreateRotationX(m_angle.X) * Matrix::CreateRotationY(m_angle.Y) * Matrix::CreateRotationZ(m_angle.Z) * m_matTranslate;
  }


  void OpenCLGaussianFilter::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers,
                                        const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t frameIndex = drawContext.CurrentFrameIndex;
    const uint32_t currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;

    // Upload the changes
    m_resources.MainFrameResources[frameIndex].VertUboBuffer.Upload(0, &m_vertexUboData, sizeof(VertexUBOData));

    auto hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {0.0f, 0.0f, 0.5f, 1.0f};
      clearValues[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        DrawScene(m_resources.MainFrameResources[frameIndex], hCmdBuffer);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }


  VkRenderPass OpenCLGaussianFilter::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    m_dependentResources.Pipeline =
      CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.VertShaderModule.Get(),
                     m_resources.FragShaderModule.Get(), m_resources.Mesh, m_dependentResources.MainRenderPass.Get(), 0);

    return m_dependentResources.MainRenderPass.Get();
  }


  void OpenCLGaussianFilter::OnFreeResources()
  {
    m_dependentResources = {};
  }


  void OpenCLGaussianFilter::DrawScene(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.Pipeline.Get());
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &frame.DescriptorSet, 0,
                            nullptr);

    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.Mesh.VertexBuffer.GetBufferPointer(), offsets);

    // Not efficient to render this way, but this is the way the OpenGLES3 sample did it.
    vkCmdDraw(commandBuffer, 4, 1, 0, 0);
    vkCmdDraw(commandBuffer, 4, 1, 4, 0);
    vkCmdDraw(commandBuffer, 4, 1, 8, 0);
    vkCmdDraw(commandBuffer, 4, 1, 12, 0);
    vkCmdDraw(commandBuffer, 4, 1, 16, 0);
    vkCmdDraw(commandBuffer, 4, 1, 20, 0);
  }


  void OpenCLGaussianFilter::PrepareMatrices(const Point2& currentSize)
  {
    const float aspectRatio = currentSize.X / float(currentSize.Y);

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = MatrixUtil::GetClipMatrix();

    m_vertexUboData.MatProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(60.0f), aspectRatio, 1.0f, 1000.0f) * vulkanClipMatrix;
    m_matTranslate = Matrix::CreateTranslation(0.0f, 0.0f, -3.0f);
  }
}
