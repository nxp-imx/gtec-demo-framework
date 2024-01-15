/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

// Since this is a basic example it will not utilize most of the utility code and instead do most of the work itself.

#include "Triangle.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <glm/ext/matrix_clip_space.hpp>    // glm::perspective
#include <glm/ext/matrix_transform.hpp>     // glm::translate, glm::rotate, glm::scale
#include <vulkan/vulkan.h>
#include <array>

namespace Fsl
{
  namespace
  {
    // Set to "true" to use staging buffers for uploading vertex and index data to device local memory
    // See "prepareVertices" for details on what's staging and on why to use it
    const bool g_configUseStaging = true;

    // Default fence timeout in nanoseconds
    const auto DEFAULT_FENCE_TIMEOUT = 100000000000;

    const auto VERTEX_BUFFER_BIND_ID = 0;

    struct Vertex
    {
      float position[3];    // NOLINT(modernize-avoid-c-arrays)
      float color[3];       // NOLINT(modernize-avoid-c-arrays)
    };

    struct BasicMesh
    {
      BufferAndMemory Vertices;
      BufferAndMemory Indices;
    };

    // Get a new command buffer from the command pool
    // If begin is true, the command buffer is also started so we can start adding commands
    RapidVulkan::CommandBuffer DoGetCommandBuffer(const VkDevice device, const VkCommandPool cmdPool, const bool begin)
    {
      VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
      cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufAllocateInfo.commandPool = cmdPool;
      cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      cmdBufAllocateInfo.commandBufferCount = 1;

      RapidVulkan::CommandBuffer cmdBuffer(device, cmdBufAllocateInfo);

      // If requested, also start the new command buffer
      if (begin)
      {
        VkCommandBufferBeginInfo cmdBufInfo{};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufInfo.pNext = nullptr;

        cmdBuffer.Begin(cmdBufInfo);
      }
      return cmdBuffer;
    }


    // End the command buffer and submit it to the queue
    // Uses a fence to ensure command buffer has finished executing before deleting it
    void DoFlushCommandBuffer(RapidVulkan::CommandBuffer& rCommandBuffer, Vulkan::VUDeviceQueueRecord& rQueue)
    {
      assert(rCommandBuffer.IsValid());

      rCommandBuffer.End();

      // Create fence to ensure that the command buffer has finished executing
      VkFenceCreateInfo fenceCreateInfo{};
      fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceCreateInfo.flags = 0;
      RapidVulkan::Fence fence(rCommandBuffer.GetDevice(), fenceCreateInfo);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = rCommandBuffer.GetPointer();

      // Submit to the queue
      rQueue.Submit(1, &submitInfo, fence.Get());

      // Wait for the fence to signal that command buffer has finished executing
      fence.WaitForFence(DEFAULT_FENCE_TIMEOUT);

      rCommandBuffer.Reset();
    }

    BasicMesh PrepareVerticesUsingStagingBuffers(const Vulkan::VUDevice& device, Vulkan::VUDeviceQueueRecord& rDeviceQueue,
                                                 const VkCommandPool cmdPool, const ReadOnlySpan<Vertex> vertices,
                                                 const ReadOnlySpan<uint32_t> indices)
    {
      // Static data like vertex and index buffer should be stored on the device memory
      // for optimal (and fastest) access by the GPU
      //
      // To achieve this we use so-called "staging buffers" :
      // - Create a buffer that's visible to the host (and can be mapped)
      // - Copy the data to this buffer
      // - Create another buffer that's local on the device (VRAM) with the same size
      // - Copy the data from the host to the device using a command buffer
      // - Delete the host visible (staging) buffer
      // - Use the device local buffers for rendering

      VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = device.GetPhysicalDevice().MemoryProperties;

      BasicMesh stagingBuffers;

      const auto vertexBufferSize = UncheckedNumericCast<uint32_t>(vertices.size() * sizeof(Vertex));

      // Vertex buffer
      VkBufferCreateInfo vertexBufferInfo{};
      vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      vertexBufferInfo.size = vertexBufferSize;
      // Buffer is used as the copy source
      vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      // Create a host-visible buffer to copy the vertex data to (staging buffer)

      stagingBuffers.Vertices.Buffer.Reset(device.Get(), vertexBufferInfo);

      VkMemoryRequirements memReqs = stagingBuffers.Vertices.Buffer.GetBufferMemoryRequirements();

      VkMemoryAllocateInfo memAlloc{};
      memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memAlloc.allocationSize = memReqs.size;
      // Request a host visible memory type that can be used to copy our data do
      // Also request it to be coherent, so that writes are visible to the GPU right after unmapping the buffer
      memAlloc.memoryTypeIndex = Vulkan::MemoryTypeUtil::GetMemoryTypeIndex(
        physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      stagingBuffers.Vertices.Memory.Reset(device.Get(), memAlloc);

      // Map and copy
      {
        void* data = nullptr;
        RAPIDVULKAN_CHECK(vkMapMemory(device.Get(), stagingBuffers.Vertices.Memory.Get(), 0, memAlloc.allocationSize, 0, &data));
        {
          std::memcpy(data, vertices.data(), vertexBufferSize);
        }
        vkUnmapMemory(device.Get(), stagingBuffers.Vertices.Memory.Get());
      }
      RAPIDVULKAN_CHECK(vkBindBufferMemory(device.Get(), stagingBuffers.Vertices.Buffer.Get(), stagingBuffers.Vertices.Memory.Get(), 0));

      // Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
      vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

      BufferAndMemory basicVertexBuffer;

      basicVertexBuffer.Buffer.Reset(device.Get(), vertexBufferInfo);
      vkGetBufferMemoryRequirements(device.Get(), basicVertexBuffer.Buffer.Get(), &memReqs);
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex =
        Vulkan::MemoryTypeUtil::GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

      basicVertexBuffer.Memory.Reset(device.Get(), memAlloc);
      RAPIDVULKAN_CHECK(vkBindBufferMemory(device.Get(), basicVertexBuffer.Buffer.Get(), basicVertexBuffer.Memory.Get(), 0));

      // Index buffer
      const auto indexBufferSize = UncheckedNumericCast<uint32_t>(indices.size() * sizeof(uint32_t));

      VkBufferCreateInfo indexbufferInfo{};
      indexbufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      indexbufferInfo.size = indexBufferSize;
      indexbufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      // Copy index data to a buffer visible to the host (staging buffer)
      stagingBuffers.Indices.Buffer.Reset(device.Get(), indexbufferInfo);
      memReqs = stagingBuffers.Indices.Buffer.GetBufferMemoryRequirements();
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex = Vulkan::MemoryTypeUtil::GetMemoryTypeIndex(
        physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      stagingBuffers.Indices.Memory.Reset(device.Get(), memAlloc);

      {
        void* data = nullptr;
        RAPIDVULKAN_CHECK(vkMapMemory(device.Get(), stagingBuffers.Indices.Memory.Get(), 0, indexBufferSize, 0, &data));
        {
          std::memcpy(data, indices.data(), indexBufferSize);
        }
        vkUnmapMemory(device.Get(), stagingBuffers.Indices.Memory.Get());
        RAPIDVULKAN_CHECK(vkBindBufferMemory(device.Get(), stagingBuffers.Indices.Buffer.Get(), stagingBuffers.Indices.Memory.Get(), 0));
      }

      BufferAndMemory basicIndexBuffer;
      // Create destination buffer with device only visibility
      indexbufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      basicIndexBuffer.Buffer.Reset(device.Get(), indexbufferInfo);
      memReqs = basicIndexBuffer.Buffer.GetBufferMemoryRequirements();
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex =
        Vulkan::MemoryTypeUtil::GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      basicIndexBuffer.Memory.Reset(device.Get(), memAlloc);
      RAPIDVULKAN_CHECK(vkBindBufferMemory(device.Get(), basicIndexBuffer.Buffer.Get(), basicIndexBuffer.Memory.Get(), 0));

      // VkCommandBufferBeginInfo cmdBufferBeginInfo{};
      // cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      // cmdBufferBeginInfo.pNext = nullptr;

      // Buffer copies have to be submitted to a queue, so we need a command buffer for them
      // Note: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies
      RapidVulkan::CommandBuffer copyCmd = DoGetCommandBuffer(device.Get(), cmdPool, true);

      // Put buffer region copies into command buffer
      VkBufferCopy copyRegion{};

      // Vertex buffer
      copyRegion.size = vertexBufferSize;
      vkCmdCopyBuffer(copyCmd.Get(), stagingBuffers.Vertices.Buffer.Get(), basicVertexBuffer.Buffer.Get(), 1, &copyRegion);
      // Index buffer
      copyRegion.size = indexBufferSize;
      vkCmdCopyBuffer(copyCmd.Get(), stagingBuffers.Indices.Buffer.Get(), basicIndexBuffer.Buffer.Get(), 1, &copyRegion);

      // Flushing the command buffer will also submit it to the queue and uses a fence to ensure that all commands have been executed before
      // returning
      DoFlushCommandBuffer(copyCmd, rDeviceQueue);

      // Destroy staging buffers (by having the go out of scope)
      // Note: Staging buffer must not be deleted before the copies have been submitted and executed
      return {std::move(basicVertexBuffer), std::move(basicIndexBuffer)};
    }

    BasicMesh PrepareVerticesBasic(const Vulkan::VUDevice& device, const ReadOnlySpan<Vertex> vertexSpan, const ReadOnlySpan<uint32_t> indexSpan)
    {
      // Don't use staging
      // Create host-visible buffers only and use these for rendering. This is not advised and will usually result in lower rendering performance
      const VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = device.GetPhysicalDevice().MemoryProperties;

      BasicMesh mesh;
      {
        // Vertex buffer
        VkBufferCreateInfo vertexBufferInfo{};
        vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexBufferInfo.size = NumericCast<VkDeviceSize>(sizeof(Vertex) * vertexSpan.size());
        vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        // Copy vertex data to a buffer visible to the host
        mesh.Vertices.Buffer.Reset(device.Get(), vertexBufferInfo);
        VkMemoryRequirements memReqs{};
        vkGetBufferMemoryRequirements(device.Get(), mesh.Vertices.Buffer.Get(), &memReqs);

        VkMemoryAllocateInfo memAlloc{};
        memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex =
          Vulkan::MemoryTypeUtil::GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        mesh.Vertices.Memory.Reset(device.Get(), memAlloc);
        {
          void* pBufferData{};
          RAPIDVULKAN_CHECK(vkMapMemory(device.Get(), mesh.Vertices.Memory.Get(), 0, memAlloc.allocationSize, 0, &pBufferData));
          std::memcpy(pBufferData, vertexSpan.data(), vertexBufferInfo.size);
          vkUnmapMemory(device.Get(), mesh.Vertices.Memory.Get());
        }
        RAPIDVULKAN_CHECK(vkBindBufferMemory(device.Get(), mesh.Vertices.Buffer.Get(), mesh.Vertices.Memory.Get(), 0));
      }

      // Index buffer
      {
        VkBufferCreateInfo indexbufferInfo{};
        indexbufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexbufferInfo.size = NumericCast<VkDeviceSize>(sizeof(uint32_t) * indexSpan.size());
        indexbufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        // Copy index data to a buffer visible to the host
        mesh.Indices.Buffer.Reset(device.Get(), indexbufferInfo);

        VkMemoryRequirements memReqs{};
        vkGetBufferMemoryRequirements(device.Get(), mesh.Indices.Buffer.Get(), &memReqs);

        VkMemoryAllocateInfo memAlloc{};
        memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex =
          Vulkan::MemoryTypeUtil::GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        mesh.Indices.Memory.Reset(device.Get(), memAlloc);
        {
          void* pBufferData{};
          RAPIDVULKAN_CHECK(vkMapMemory(device.Get(), mesh.Indices.Memory.Get(), 0, memAlloc.allocationSize, 0, &pBufferData));
          std::memcpy(pBufferData, indexSpan.data(), indexbufferInfo.size);
          vkUnmapMemory(device.Get(), mesh.Indices.Memory.Get());
        }
        RAPIDVULKAN_CHECK(vkBindBufferMemory(device.Get(), mesh.Indices.Buffer.Get(), mesh.Indices.Memory.Get(), 0));
      }
      return mesh;
    }

    BasicMesh PrepareMesh(const Vulkan::VUDevice& device, Vulkan::VUDeviceQueueRecord& rDeviceQueue, const VkCommandPool cmdPool,
                          const ReadOnlySpan<Vertex> vertices, const ReadOnlySpan<uint32_t> indices, const bool useStagingBuffers)
    {
      if (useStagingBuffers)
      {
        return PrepareVerticesUsingStagingBuffers(device, rDeviceQueue, cmdPool, vertices, indices);
      }


      return PrepareVerticesBasic(device, vertices, indices);
    }

    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      // Setup layout of descriptors used in this example
      // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
      // So every shader binding should map to one descriptor set layout binding

      // Binding 0: Uniform buffer (Vertex shader)
      VkDescriptorSetLayoutBinding layoutBinding{};
      layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      layoutBinding.descriptorCount = 1;
      layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
      layoutBinding.pImmutableSamplers = nullptr;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.pNext = nullptr;
      descriptorLayout.bindingCount = 1;
      descriptorLayout.pBindings = &layoutBinding;

      return {device.Get(), descriptorLayout};
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const Vulkan::VUDevice& device, const RapidVulkan::DescriptorSetLayout& descriptorSetLayout)
    {
      // Create the pipeline layout that is used to generate the rendering pipelines that are based on this descriptor set layout
      // In a more complex scenario you would have different pipeline layouts for different descriptor set layouts that could be reused
      VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo{};
      pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pPipelineLayoutCreateInfo.pNext = nullptr;
      pPipelineLayoutCreateInfo.setLayoutCount = 1;
      pPipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      return {device.Get(), pPipelineLayoutCreateInfo};
    }

    RapidVulkan::ShaderModule LoadShader(const IContentManager& contentManager, const IO::Path& fileName, const VkDevice device)
    {
      const auto shaderBinary = contentManager.ReadBytes(IO::Path(fileName));

      VkShaderModuleCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      createInfo.codeSize = shaderBinary.size();
      createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBinary.data());

      return {device, createInfo};
    }


    RapidVulkan::GraphicsPipeline CreatePipeline(const Vulkan::VUDevice& device, const VkPipelineLayout pipelineLayout, const VkRenderPass renderPass,
                                                 const VkShaderModule vertexShaderModule, const VkShaderModule fragShaderModule,
                                                 const VertexBuffer& vertexBuffer)
    {
      // Create the graphics pipeline used in this example
      // Vulkan uses the concept of rendering pipelines to encapsulate fixed states, replacing OpenGL's complex state machine
      // A pipeline is then stored and hashed on the GPU making pipeline changes very fast
      // Note: There are still a few dynamic states that are not directly part of the pipeline (but the info that they are used is)

      // Construct the different states making up the pipeline

      // Input assembly state describes how primitives are assembled
      // This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
      VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
      inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

      // Rasterization state
      VkPipelineRasterizationStateCreateInfo rasterizationState{};
      rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
      rasterizationState.cullMode = VK_CULL_MODE_NONE;
      rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      rasterizationState.depthClampEnable = VK_FALSE;
      rasterizationState.rasterizerDiscardEnable = VK_FALSE;
      rasterizationState.depthBiasEnable = VK_FALSE;
      rasterizationState.lineWidth = 1.0f;

      // Color blend state describes how blend factors are calculated (if used)
      // We need one blend attachment state per color attachment (even if blending is not used
      VkPipelineColorBlendAttachmentState blendAttachmentState{};
      blendAttachmentState.colorWriteMask = 0xf;
      blendAttachmentState.blendEnable = VK_FALSE;
      VkPipelineColorBlendStateCreateInfo colorBlendState{};
      colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlendState.attachmentCount = 1;
      colorBlendState.pAttachments = &blendAttachmentState;

      // Viewport state sets the number of viewports and scissor used in this pipeline
      // Note: This is actually overridden by the dynamic states (see below)
      VkPipelineViewportStateCreateInfo viewportState{};
      viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.scissorCount = 1;

      // Enable dynamic states
      // Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
      // To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on
      // in the command buffer. For this example we will set the viewport and scissor using dynamic states
      std::vector<VkDynamicState> dynamicStateEnables;
      dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
      dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
      VkPipelineDynamicStateCreateInfo dynamicState{};
      dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState.pDynamicStates = dynamicStateEnables.data();
      dynamicState.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicStateEnables.size());

      // Depth and stencil state containing depth and stencil compare and test operations
      // We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
      VkPipelineDepthStencilStateCreateInfo depthStencilState{};
      depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilState.depthTestEnable = VK_TRUE;
      depthStencilState.depthWriteEnable = VK_TRUE;
      depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
      depthStencilState.depthBoundsTestEnable = VK_FALSE;
      depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
      depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
      depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
      depthStencilState.stencilTestEnable = VK_FALSE;
      depthStencilState.front = depthStencilState.back;

      // Multi sampling state
      // This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
      VkPipelineMultisampleStateCreateInfo multisampleState{};
      multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      multisampleState.pSampleMask = nullptr;

      // Load shaders
      // Vulkan loads it's shaders from an immediate binary representation called SPIR-V
      // Shaders are compiled offline from e.g. GLSL using the reference glslang compiler
      std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

      shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      shaderStages[0].module = vertexShaderModule;
      shaderStages[0].pName = "main";

      shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      shaderStages[1].module = fragShaderModule;
      shaderStages[1].pName = "main";

      // Assign to the vertex input state used for pipeline creation
      VkPipelineVertexInputStateCreateInfo vertexInputState{};
      vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertexInputState.vertexBindingDescriptionCount = 1;
      vertexInputState.pVertexBindingDescriptions = &vertexBuffer.InputBinding;
      vertexInputState.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(vertexBuffer.InputAttributes.size());
      vertexInputState.pVertexAttributeDescriptions = vertexBuffer.InputAttributes.data();


      VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
      pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      // The layout used for this pipeline (can be shared among multiple pipelines using the same layout)
      pipelineCreateInfo.layout = pipelineLayout;
      // Render-pass this pipeline is attached to
      pipelineCreateInfo.renderPass = renderPass;
      // Assign the pipeline states to the pipeline creation info structure
      pipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(shaderStages.size());
      pipelineCreateInfo.pStages = shaderStages.data();
      pipelineCreateInfo.pVertexInputState = &vertexInputState;
      pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
      pipelineCreateInfo.pRasterizationState = &rasterizationState;
      pipelineCreateInfo.pColorBlendState = &colorBlendState;
      pipelineCreateInfo.pMultisampleState = &multisampleState;
      pipelineCreateInfo.pViewportState = &viewportState;
      pipelineCreateInfo.pDepthStencilState = &depthStencilState;
      pipelineCreateInfo.renderPass = renderPass;
      pipelineCreateInfo.pDynamicState = &dynamicState;

      // Create rendering pipeline using the specified states
      return {device.Get(), VK_NULL_HANDLE, pipelineCreateInfo};
    }

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device)
    {
      // We need to tell the API the number of max. requested descriptors per type
      std::array<VkDescriptorPoolSize, 1> typeCounts{};
      // This example only uses one descriptor type (uniform buffer) and only requests one descriptor of this type
      typeCounts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      typeCounts[0].descriptorCount = 1;
      // For additional types you need to add new entries in the type count list
      // E.g. for two combined image samplers :
      // typeCounts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      // typeCounts[1].descriptorCount = 2;

      // Create the global descriptor pool
      // All descriptors used in this example are allocated from this pool
      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.pNext = nullptr;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(typeCounts.size());
      descriptorPoolInfo.pPoolSizes = typeCounts.data();
      // Set the max. number of descriptor sets that can be requested from this pool (requesting beyond this limit will result in an error)
      descriptorPoolInfo.maxSets = 1;

      return {device.Get(), descriptorPoolInfo};
    }


    VkDescriptorSet CreateDescriptorSet(const Vulkan::VUDevice& device, const RapidVulkan::DescriptorPool& descriptorPool,
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout,
                                        const VkDescriptorBufferInfo descriptorBufferInfo)
    {
      VkDescriptorSet descriptorSet{VK_NULL_HANDLE};

      // Allocate a new descriptor set from the global descriptor pool
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool.Get();
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(device.Get(), &allocInfo, &descriptorSet));

      // Update the descriptor set determining the shader binding points
      // For every binding point used in a shader there needs to be one
      // descriptor set matching that binding point

      VkWriteDescriptorSet writeDescriptorSet{};

      // Binding 0 : Uniform buffer
      writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSet.dstSet = descriptorSet;
      writeDescriptorSet.descriptorCount = 1;
      writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
      // Binds this uniform buffer to binding point 0
      writeDescriptorSet.dstBinding = 0;

      vkUpdateDescriptorSets(device.Get(), 1, &writeDescriptorSet, 0, nullptr);
      return descriptorSet;
    }
  }


  Triangle::Triangle(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_cameraInfo{glm::vec3(), -2.5f}
  {
    auto contentManager = GetContentManager();

    m_resources.TriangleMesh = PrepareVertices(m_device, m_deviceQueue, GetCommandPool(), g_configUseStaging);
    m_resources.UniformData = PrepareUniformBuffers(m_device, m_cameraInfo, GetWindowAspectRatio());
    m_resources.DescriptorSetLayout = CreateDescriptorSetLayout(m_device);
    m_resources.PipelineLayout = CreatePipelineLayout(m_device, m_resources.DescriptorSetLayout);

    m_resources.VertShaderModule = LoadShader(*contentManager, "triangle.vert.spv", m_device.Get());
    m_resources.FragShaderModule = LoadShader(*contentManager, "triangle.frag.spv", m_device.Get());

    m_resources.DescriptorPool = CreateDescriptorPool(m_device);
    m_resources.DescriptorSet =
      CreateDescriptorSet(m_device, m_resources.DescriptorPool, m_resources.DescriptorSetLayout, m_resources.UniformData.Descriptor);
  }


  void Triangle::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
  }


  void Triangle::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    FSL_PARAM_NOT_USED(demoTime);

    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        DrawTriangle(hCmdBuffer, currentFrameIndex);
        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }

  void Triangle::DrawTriangle(VkCommandBuffer hCmdBuffer, const uint32_t cmdBufferIndex)
  {
    FSL_PARAM_NOT_USED(cmdBufferIndex);

    auto screenExtent = GetScreenExtent();

    const Mesh& mesh = m_resources.TriangleMesh;

    // Update dynamic viewport state
    VkViewport viewport{};
    viewport.width = static_cast<float>(screenExtent.Width.Value);
    viewport.height = static_cast<float>(screenExtent.Height.Value);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);

    // Update dynamic scissor state
    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = TypeConverter::UncheckedTo<VkExtent2D>(screenExtent);
    vkCmdSetScissor(hCmdBuffer, 0, 1, &scissor);

    // Bind descriptor sets describing shader binding points
    vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.PipelineLayout.Get(), 0, 1, &m_resources.DescriptorSet, 0,
                            nullptr);

    // Bind the rendering pipeline
    // The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation
    // time
    vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.Pipeline.Get());

    // Bind triangle vertex buffer (contains position and colors)
    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(hCmdBuffer, VERTEX_BUFFER_BIND_ID, 1, mesh.Vertices.Content.Buffer.GetPointer(), &offsets);

    // Bind triangle index buffer
    vkCmdBindIndexBuffer(hCmdBuffer, mesh.Indices.Content.Buffer.Get(), 0, VK_INDEX_TYPE_UINT32);

    // Draw indexed triangle
    vkCmdDrawIndexed(hCmdBuffer, mesh.Indices.Count, 1, 0, 0, 1);
  }


  VkRenderPass Triangle::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    FSL_PARAM_NOT_USED(context);

    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    m_dependentResources.Pipeline =
      CreatePipeline(m_device, m_resources.PipelineLayout.Get(), m_dependentResources.MainRenderPass.Get(), m_resources.VertShaderModule.Get(),
                     m_resources.FragShaderModule.Get(), m_resources.TriangleMesh.Vertices);

    return m_dependentResources.MainRenderPass.Get();
  }


  void Triangle::OnFreeResources()
  {
    m_dependentResources.Reset();
  }

  // Prepare vertex and index buffers for an indexed triangle
  // Also uploads them to device local memory using staging and initializes vertex input and attribute binding to match the vertex shader
  Triangle::Mesh Triangle::PrepareVertices(const Vulkan::VUDevice& device, Vulkan::VUDeviceQueueRecord& rDeviceQueue, const VkCommandPool cmdPool,
                                           const bool useStagingBuffers)
  {
    // A note on memory management in Vulkan in general:
    //  This is a very complex topic and while it's fine for an example application to do small individual memory allocations that is not
    //  what should be done a real-world application, where you should allocate large chunks of memory at once instead.

    // Setup vertices
    std::array<Vertex, 3> vertices = {Vertex{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, Vertex{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                                      Vertex{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

    // Setup indices
    constexpr std::array<uint32_t, 3> indices = {0, 1, 2};
    const auto indexCount = UncheckedNumericCast<uint32_t>(indices.size());

    BasicMesh mesh =
      PrepareMesh(device, rDeviceQueue, cmdPool, ReadOnlySpanUtil::AsSpan(vertices), ReadOnlySpanUtil::AsSpan(indices), useStagingBuffers);

    // Vertex input binding
    VertexBuffer vertexBuffer;
    {
      vertexBuffer.Content = std::move(mesh.Vertices);

      vertexBuffer.InputBinding.binding = VERTEX_BUFFER_BIND_ID;
      vertexBuffer.InputBinding.stride = sizeof(Vertex);
      vertexBuffer.InputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      // Input attribute binding describe shader attribute locations and memory layouts
      // These match the following shader layout (see triangle.vert):
      //  layout (location = 0) in vec3 inPos;
      //  layout (location = 1) in vec3 inColor;
      vertexBuffer.InputAttributes.resize(2);
      // Attribute location 0: Position
      vertexBuffer.InputAttributes[0].binding = VERTEX_BUFFER_BIND_ID;
      vertexBuffer.InputAttributes[0].location = 0;
      vertexBuffer.InputAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      vertexBuffer.InputAttributes[0].offset = offsetof(Vertex, position);
      // Attribute location 1: Color
      vertexBuffer.InputAttributes[1].binding = VERTEX_BUFFER_BIND_ID;
      vertexBuffer.InputAttributes[1].location = 1;
      vertexBuffer.InputAttributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      vertexBuffer.InputAttributes[1].offset = offsetof(Vertex, color);
    }
    IndexBuffer indexBuffer;
    {
      indexBuffer.Content = std::move(mesh.Indices);
      indexBuffer.Count = indexCount;
    }
    return {std::move(vertexBuffer), std::move(indexBuffer)};
  }


  Triangle::UniformDataVS Triangle::PrepareUniformBuffers(const Vulkan::VUDevice& device, const CameraInfo& cameraInfo, const float aspectRatio)
  {
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = device.GetPhysicalDevice().MemoryProperties;

    // Prepare and initialize a uniform buffer block containing shader uniforms
    // Single uniforms like in OpenGL are no longer present in Vulkan. All Shader uniforms are passed via uniform buffer blocks
    VkMemoryRequirements memReqs;

    // Vertex shader uniform buffer block
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = 0;
    allocInfo.memoryTypeIndex = 0;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(m_uboVS);
    // This buffer will be used as a uniform buffer
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    // Create a new buffer
    UniformDataVS uniformDataVS;
    uniformDataVS.Buffer.Reset(device.Get(), bufferInfo);
    // Get memory requirements including size, alignment and memory type
    memReqs = uniformDataVS.Buffer.GetBufferMemoryRequirements();
    allocInfo.allocationSize = memReqs.size;
    // Get the memory type index that supports host visible memory access
    // Most implementations offer multiple memory types and selecting the correct one to allocate memory from is crucial
    // We also want the buffer to be host coherent so we don't have to flush (or sync after every update.
    // Note: This may affect performance so you might not want to do this in a real world application that updates buffers on a regular base
    allocInfo.memoryTypeIndex = Vulkan::MemoryTypeUtil::GetMemoryTypeIndex(
      physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // Allocate memory for the uniform buffer
    uniformDataVS.Memory.Reset(device.Get(), allocInfo);
    // Bind memory to buffer
    RAPIDVULKAN_CHECK(vkBindBufferMemory(device.Get(), uniformDataVS.Buffer.Get(), uniformDataVS.Memory.Get(), 0));

    // Store information in the uniform's descriptor that is used by the descriptor set
    uniformDataVS.Descriptor.buffer = uniformDataVS.Buffer.Get();
    uniformDataVS.Descriptor.offset = 0;
    uniformDataVS.Descriptor.range = sizeof(m_uboVS);

    UpdateUniformBuffers(device, cameraInfo, aspectRatio, uniformDataVS);
    return uniformDataVS;
  }

  Triangle::UboVS Triangle::UpdateUniformBuffers(const Vulkan::VUDevice& device, const CameraInfo& cameraInfo, const float aspectRatio,
                                                 UniformDataVS& rUniformData)
  {
    // Update matrices
    UboVS uboVS;
    uboVS.ProjectionMatrix = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 256.0f);

    uboVS.ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, cameraInfo.Zoom));

    uboVS.ModelMatrix = glm::mat4(1.0f);
    uboVS.ModelMatrix = glm::rotate(uboVS.ModelMatrix, glm::radians(cameraInfo.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    uboVS.ModelMatrix = glm::rotate(uboVS.ModelMatrix, glm::radians(cameraInfo.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    uboVS.ModelMatrix = glm::rotate(uboVS.ModelMatrix, glm::radians(cameraInfo.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Map uniform buffer and update it
    uint8_t* pData = nullptr;
    RAPIDVULKAN_CHECK(vkMapMemory(device.Get(), rUniformData.Memory.Get(), 0, sizeof(m_uboVS), 0, reinterpret_cast<void**>(&pData)));
    {
      std::memcpy(pData, &uboVS, sizeof(m_uboVS));
      // Unmap after data has been copied
      // Note: Since we requested a host coherent memory type for the uniform buffer, the write is instantly visible to the GPU
    }
    vkUnmapMemory(device.Get(), rUniformData.Memory.Get());
    return uboVS;
  }
}
