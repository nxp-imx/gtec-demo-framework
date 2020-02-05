/*
 * Vulkan Example - Animated gears using multiple uniform buffers
 *
 * See readme.md for details
 *
 * Copyright (C) 2015 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called 'Gears' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include "VulkanGear.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <RapidVulkan/Check.hpp>
#include <cmath>
#include <cstring>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace RapidVulkan;

namespace Fsl
{
  VulkanGear::VulkanGear(Willems::VulkanDevice* pVulkanDevice)
    : m_pVulkanDevice(pVulkanDevice)
    , DescriptorSet(VK_NULL_HANDLE)
  {
  }


  VulkanGear::~VulkanGear() = default;


  void VulkanGear::Generate(const GearInfo& gearinfo, const VkQueue queue)
  {
    m_color = gearinfo.Color;
    m_pos = gearinfo.Pos;
    m_rotOffset = gearinfo.RotOffset;
    m_rotSpeed = gearinfo.RotSpeed;

    std::vector<Vertex> vBuffer;
    std::vector<uint32_t> iBuffer;

    // int i, j;
    // float r0, r1, r2;
    // float ta, da;
    // float u1, v1, u2, v2, len;
    // float cos_ta, cos_ta_1da, cos_ta_2da, cos_ta_3da, cos_ta_4da;
    // float sin_ta, sin_ta_1da, sin_ta_2da, sin_ta_3da, sin_ta_4da;
    // int32_t ix0, ix1, ix2, ix3, ix4, ix5;

    const float r0 = gearinfo.InnerRadius;
    const float r1 = gearinfo.OuterRadius - gearinfo.ToothDepth / 2.0f;
    const float r2 = gearinfo.OuterRadius + gearinfo.ToothDepth / 2.0f;
    const float da = 2.0f * MathHelper::PI / gearinfo.NumTeeth / 4.0f;
    const float halfGearWidth = gearinfo.Width * 0.5f;

    for (int i = 0; i < gearinfo.NumTeeth; ++i)
    {
      const float ta = i * 2.0f * MathHelper::PI / gearinfo.NumTeeth;
      const float cos_ta = std::cos(ta);
      const float cos_ta_1da = std::cos(ta + da);
      const float cos_ta_2da = std::cos(ta + 2 * da);
      const float cos_ta_3da = std::cos(ta + 3 * da);
      const float cos_ta_4da = std::cos(ta + 4 * da);
      const float sin_ta = std::sin(ta);
      const float sin_ta_1da = std::sin(ta + da);
      const float sin_ta_2da = std::sin(ta + 2 * da);
      const float sin_ta_3da = std::sin(ta + 3 * da);
      const float sin_ta_4da = std::sin(ta + 4 * da);

      float u1 = r2 * cos_ta_1da - r1 * cos_ta;
      float v1 = r2 * sin_ta_1da - r1 * sin_ta;
      const float len = std::sqrt(u1 * u1 + v1 * v1);
      u1 /= len;
      v1 /= len;
      const float u2 = r1 * cos_ta_3da - r2 * cos_ta_2da;
      const float v2 = r1 * sin_ta_3da - r2 * sin_ta_2da;


      // front face
      glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
      auto ix0 = NewVertex(vBuffer, r0 * cos_ta, r0 * sin_ta, halfGearWidth, normal);
      auto ix1 = NewVertex(vBuffer, r1 * cos_ta, r1 * sin_ta, halfGearWidth, normal);
      auto ix2 = NewVertex(vBuffer, r0 * cos_ta, r0 * sin_ta, halfGearWidth, normal);
      auto ix3 = NewVertex(vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, halfGearWidth, normal);
      auto ix4 = NewVertex(vBuffer, r0 * cos_ta_4da, r0 * sin_ta_4da, halfGearWidth, normal);
      auto ix5 = NewVertex(vBuffer, r1 * cos_ta_4da, r1 * sin_ta_4da, halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);
      NewFace(iBuffer, ix2, ix3, ix4);
      NewFace(iBuffer, ix3, ix5, ix4);

      // front sides of teeth
      normal = glm::vec3(0.0f, 0.0f, 1.0f);
      ix0 = NewVertex(vBuffer, r1 * cos_ta, r1 * sin_ta, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      // back face
      normal = glm::vec3(0.0f, 0.0f, -1.0f);
      ix0 = NewVertex(vBuffer, r1 * cos_ta, r1 * sin_ta, -halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r0 * cos_ta, r0 * sin_ta, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, -halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r0 * cos_ta, r0 * sin_ta, -halfGearWidth, normal);
      ix4 = NewVertex(vBuffer, r1 * cos_ta_4da, r1 * sin_ta_4da, -halfGearWidth, normal);
      ix5 = NewVertex(vBuffer, r0 * cos_ta_4da, r0 * sin_ta_4da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);
      NewFace(iBuffer, ix2, ix3, ix4);
      NewFace(iBuffer, ix3, ix5, ix4);

      // back sides of teeth
      normal = glm::vec3(0.0, 0.0, -1.0f);
      ix0 = NewVertex(vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, -halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cos_ta, r1 * sin_ta, -halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      // draw outward faces of teeth
      normal = glm::vec3(v1, -u1, 0.0f);
      ix0 = NewVertex(vBuffer, r1 * cos_ta, r1 * sin_ta, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r1 * cos_ta, r1 * sin_ta, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      normal = glm::vec3(cos_ta, sin_ta, 0.0f);
      ix0 = NewVertex(vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      normal = glm::vec3(v2, -u2, 0.0f);
      ix0 = NewVertex(vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      normal = glm::vec3(cos_ta, sin_ta, 0.0f);
      ix0 = NewVertex(vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cos_ta_4da, r1 * sin_ta_4da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r1 * cos_ta_4da, r1 * sin_ta_4da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      // draw inside radius cylinder
      ix0 = NewVertex(vBuffer, r0 * cos_ta, r0 * sin_ta, -halfGearWidth, glm::vec3(-cos_ta, -sin_ta, 0.0f));
      ix1 = NewVertex(vBuffer, r0 * cos_ta, r0 * sin_ta, halfGearWidth, glm::vec3(-cos_ta, -sin_ta, 0.0f));
      ix2 = NewVertex(vBuffer, r0 * cos_ta_4da, r0 * sin_ta_4da, -halfGearWidth, glm::vec3(-cos_ta_4da, -sin_ta_4da, 0.0f));
      ix3 = NewVertex(vBuffer, r0 * cos_ta_4da, r0 * sin_ta_4da, halfGearWidth, glm::vec3(-cos_ta_4da, -sin_ta_4da, 0.0f));
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);
    }

    const auto vertexBufferSize = static_cast<uint32_t>(vBuffer.size() * sizeof(Vertex));
    const auto indexBufferSize = static_cast<uint32_t>(iBuffer.size() * sizeof(uint32_t));

    const bool useStaging = true;

    if (useStaging)
    {
      // Create staging buffers
      // Vertex data
      auto vertexStaging =
        m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertexBufferSize, vBuffer.data());

      // Index data
      auto indexStaging =
        m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, indexBufferSize, iBuffer.data());

      // Create device local buffers
      // Vertex buffer
      m_vertexBuffer = m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBufferSize);
      // Index buffer

      m_indexBuffer = m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBufferSize);

      {    // Copy from staging buffers
        CommandBuffer copyCmd = m_pVulkanDevice->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

        VkBufferCopy copyRegion{};
        copyRegion.size = vertexBufferSize;

        vkCmdCopyBuffer(copyCmd.Get(), vertexStaging.GetBuffer(), m_vertexBuffer.GetBuffer(), 1, &copyRegion);

        copyRegion.size = indexBufferSize;
        vkCmdCopyBuffer(copyCmd.Get(), indexStaging.GetBuffer(), m_indexBuffer.GetBuffer(), 1, &copyRegion);

        m_pVulkanDevice->FlushCommandBuffer(copyCmd, queue, true);
      }
    }
    else
    {
      // Vertex buffer
      m_vertexBuffer =
        m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertexBufferSize, vBuffer.data());

      // Index buffer
      m_indexBuffer =
        m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, indexBufferSize, iBuffer.data());
    }

    m_indexCount = static_cast<uint32_t>(iBuffer.size());
    PrepareUniformBuffer();
  }


  void VulkanGear::UpdateUniformBuffer(const glm::mat4& perspective, const glm::vec3& rotation, const float zoom, const float timer)
  {
    m_ubo.Projection = perspective;

    m_ubo.View = glm::lookAt(glm::vec3(0, 0, -zoom), glm::vec3(-1.0, -1.5, 0), glm::vec3(0, 1, 0));
    m_ubo.View = glm::rotate(m_ubo.View, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_ubo.View = glm::rotate(m_ubo.View, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    m_ubo.Model = glm::mat4(1.0f);
    m_ubo.Model = glm::translate(m_ubo.Model, m_pos);

    const float moddedRotationZ = (m_rotSpeed * timer) + m_rotOffset;
    m_ubo.Model = glm::rotate(m_ubo.Model, glm::radians(moddedRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

    m_ubo.Normal = glm::inverseTranspose(m_ubo.View * m_ubo.Model);

    m_ubo.LightPos = glm::vec3(0.0f, 0.0f, 2.5f);
    m_ubo.LightPos.x = sin(glm::radians(timer)) * 8.0f;
    m_ubo.LightPos.z = cos(glm::radians(timer)) * 8.0f;

    {    // Transfer
      void* pData = nullptr;
      m_uniformData.Memory.MapMemory(0, sizeof(m_ubo), 0, &pData);
      std::memcpy(pData, &m_ubo, sizeof(m_ubo));
      m_uniformData.Memory.UnmapMemory();
    }
  }


  void VulkanGear::SetupDescriptorSet(const VkDescriptorPool pool, const VkDescriptorSetLayout descriptorSetLayout)
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_pVulkanDevice->GetDevice(), &allocInfo, &DescriptorSet));

    // Binding 0 : Vertex shader uniform buffer
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext = nullptr;
    writeDescriptorSet.dstSet = DescriptorSet;
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pBufferInfo = &m_uniformData.Descriptor;

    vkUpdateDescriptorSets(m_pVulkanDevice->GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
  }


  void VulkanGear::Draw(const VkCommandBuffer cmdbuffer, const VkPipelineLayout pipelineLayout)
  {
    VkDeviceSize offsets[1] = {0};

    vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &DescriptorSet, 0, nullptr);
    vkCmdBindVertexBuffers(cmdbuffer, 0, 1, m_vertexBuffer.GetBufferPointer(), offsets);
    vkCmdBindIndexBuffer(cmdbuffer, m_indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdbuffer, m_indexCount, 1, 0, 0, 1);
  }


  int32_t VulkanGear::NewVertex(std::vector<Vertex>& rVBuffer, const float x, const float y, const float z, const glm::vec3& normal)
  {
    Vertex v(glm::vec3(x, y, z), normal, m_color);
    rVBuffer.push_back(v);
    return static_cast<int32_t>(rVBuffer.size()) - 1;
  }


  void VulkanGear::NewFace(std::vector<uint32_t>& rIBuffer, const int a, const int b, const int c)
  {
    rIBuffer.push_back(a);
    rIBuffer.push_back(b);
    rIBuffer.push_back(c);
  }


  void VulkanGear::PrepareUniformBuffer()
  {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = sizeof(m_ubo);
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    m_uniformData.Buffer.Reset(m_pVulkanDevice->GetDevice(), bufferInfo);

    const VkMemoryRequirements memReqs = m_uniformData.Buffer.GetBufferMemoryRequirements();

    // Vertex shader uniform buffer block
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex =
      m_pVulkanDevice->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_uniformData.Memory.Reset(m_pVulkanDevice->GetDevice(), allocInfo);

    RAPIDVULKAN_CHECK(vkBindBufferMemory(m_pVulkanDevice->GetDevice(), m_uniformData.GetBuffer(), m_uniformData.GetMemory(), 0));

    m_uniformData.Descriptor = VkDescriptorBufferInfo{};
    m_uniformData.Descriptor.buffer = m_uniformData.GetBuffer();
    m_uniformData.Descriptor.offset = 0;
    m_uniformData.Descriptor.range = static_cast<VkDeviceSize>(sizeof(m_ubo));
    m_uniformData.AllocSize = static_cast<uint32_t>(allocInfo.allocationSize);
  }
}
