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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Vector.hpp>
#include <RapidVulkan/Check.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <cstring>
#include <vector>

namespace Fsl
{
  namespace
  {
    void NewFace(std::vector<uint32_t>& rIBuffer, const int a, const int b, const int c)
    {
      rIBuffer.push_back(a);
      rIBuffer.push_back(b);
      rIBuffer.push_back(c);
    }

    Vulkan::VUBufferMemory CreateUniformBuffer(const Vulkan::VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkDeviceSize size)
    {
      VkBufferCreateInfo bufferInfo{};
      bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferInfo.pNext = nullptr;
      bufferInfo.flags = 0;
      bufferInfo.size = size;
      bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

      return {physicalDevice, device, bufferInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
    }

  }

  VulkanGear::VulkanGear(Vulkan::VUPhysicalDeviceRecord physicalDevice, const VkDevice device, std::shared_ptr<Vulkan::VMBufferManager> bufferManager)
    : m_physicalDevice(physicalDevice)
    , m_device(device)
    , m_bufferManager(std::move(bufferManager))
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
    const float da = 2.0f * MathHelper::PI / static_cast<float>(gearinfo.NumTeeth) / 4.0f;
    const float halfGearWidth = gearinfo.Width * 0.5f;

    for (int i = 0; i < gearinfo.NumTeeth; ++i)
    {
      const float ta = static_cast<float>(i) * 2.0f * MathHelper::PI / static_cast<float>(gearinfo.NumTeeth);
      const float cosTa = std::cos(ta);
      const float cosTa1da = std::cos(ta + da);
      const float cosTa2da = std::cos(ta + 2 * da);
      const float cosTa3da = std::cos(ta + 3 * da);
      const float cosTa4da = std::cos(ta + 4 * da);
      const float sinTa = std::sin(ta);
      const float sinTa1da = std::sin(ta + da);
      const float sinTa2da = std::sin(ta + 2 * da);
      const float sinTa3da = std::sin(ta + 3 * da);
      const float sinTa4da = std::sin(ta + 4 * da);

      float u1 = r2 * cosTa1da - r1 * cosTa;
      float v1 = r2 * sinTa1da - r1 * sinTa;
      const float len = std::sqrt(u1 * u1 + v1 * v1);
      u1 /= len;
      v1 /= len;
      const float u2 = r1 * cosTa3da - r2 * cosTa2da;
      const float v2 = r1 * sinTa3da - r2 * sinTa2da;


      // front face
      glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
      auto ix0 = NewVertex(vBuffer, r0 * cosTa, r0 * sinTa, halfGearWidth, normal);
      auto ix1 = NewVertex(vBuffer, r1 * cosTa, r1 * sinTa, halfGearWidth, normal);
      auto ix2 = NewVertex(vBuffer, r0 * cosTa, r0 * sinTa, halfGearWidth, normal);
      auto ix3 = NewVertex(vBuffer, r1 * cosTa3da, r1 * sinTa3da, halfGearWidth, normal);
      auto ix4 = NewVertex(vBuffer, r0 * cosTa4da, r0 * sinTa4da, halfGearWidth, normal);
      auto ix5 = NewVertex(vBuffer, r1 * cosTa4da, r1 * sinTa4da, halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);
      NewFace(iBuffer, ix2, ix3, ix4);
      NewFace(iBuffer, ix3, ix5, ix4);

      // front sides of teeth
      normal = glm::vec3(0.0f, 0.0f, 1.0f);
      ix0 = NewVertex(vBuffer, r1 * cosTa, r1 * sinTa, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r2 * cosTa1da, r2 * sinTa1da, halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cosTa3da, r1 * sinTa3da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r2 * cosTa2da, r2 * sinTa2da, halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      // back face
      normal = glm::vec3(0.0f, 0.0f, -1.0f);
      ix0 = NewVertex(vBuffer, r1 * cosTa, r1 * sinTa, -halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r0 * cosTa, r0 * sinTa, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cosTa3da, r1 * sinTa3da, -halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r0 * cosTa, r0 * sinTa, -halfGearWidth, normal);
      ix4 = NewVertex(vBuffer, r1 * cosTa4da, r1 * sinTa4da, -halfGearWidth, normal);
      ix5 = NewVertex(vBuffer, r0 * cosTa4da, r0 * sinTa4da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);
      NewFace(iBuffer, ix2, ix3, ix4);
      NewFace(iBuffer, ix3, ix5, ix4);

      // back sides of teeth
      normal = glm::vec3(0.0, 0.0, -1.0f);
      ix0 = NewVertex(vBuffer, r1 * cosTa3da, r1 * sinTa3da, -halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r2 * cosTa2da, r2 * sinTa2da, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cosTa, r1 * sinTa, -halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r2 * cosTa1da, r2 * sinTa1da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      // draw outward faces of teeth
      normal = glm::vec3(v1, -u1, 0.0f);
      ix0 = NewVertex(vBuffer, r1 * cosTa, r1 * sinTa, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r1 * cosTa, r1 * sinTa, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r2 * cosTa1da, r2 * sinTa1da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r2 * cosTa1da, r2 * sinTa1da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      normal = glm::vec3(cosTa, sinTa, 0.0f);
      ix0 = NewVertex(vBuffer, r2 * cosTa1da, r2 * sinTa1da, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r2 * cosTa1da, r2 * sinTa1da, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r2 * cosTa2da, r2 * sinTa2da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r2 * cosTa2da, r2 * sinTa2da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      normal = glm::vec3(v2, -u2, 0.0f);
      ix0 = NewVertex(vBuffer, r2 * cosTa2da, r2 * sinTa2da, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r2 * cosTa2da, r2 * sinTa2da, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cosTa3da, r1 * sinTa3da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r1 * cosTa3da, r1 * sinTa3da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      normal = glm::vec3(cosTa, sinTa, 0.0f);
      ix0 = NewVertex(vBuffer, r1 * cosTa3da, r1 * sinTa3da, halfGearWidth, normal);
      ix1 = NewVertex(vBuffer, r1 * cosTa3da, r1 * sinTa3da, -halfGearWidth, normal);
      ix2 = NewVertex(vBuffer, r1 * cosTa4da, r1 * sinTa4da, halfGearWidth, normal);
      ix3 = NewVertex(vBuffer, r1 * cosTa4da, r1 * sinTa4da, -halfGearWidth, normal);
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);

      // draw inside radius cylinder
      ix0 = NewVertex(vBuffer, r0 * cosTa, r0 * sinTa, -halfGearWidth, glm::vec3(-cosTa, -sinTa, 0.0f));
      ix1 = NewVertex(vBuffer, r0 * cosTa, r0 * sinTa, halfGearWidth, glm::vec3(-cosTa, -sinTa, 0.0f));
      ix2 = NewVertex(vBuffer, r0 * cosTa4da, r0 * sinTa4da, -halfGearWidth, glm::vec3(-cosTa4da, -sinTa4da, 0.0f));
      ix3 = NewVertex(vBuffer, r0 * cosTa4da, r0 * sinTa4da, halfGearWidth, glm::vec3(-cosTa4da, -sinTa4da, 0.0f));
      NewFace(iBuffer, ix0, ix1, ix2);
      NewFace(iBuffer, ix1, ix3, ix2);
    }

    m_vertexBuffer.Reset(m_bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(vBuffer), Vulkan::VMBufferUsage::STATIC);
    m_indexBuffer.Reset(m_bufferManager, iBuffer, Vulkan::VMBufferUsage::STATIC);

    m_indexCount = UncheckedNumericCast<uint32_t>(iBuffer.size());
  }


  void VulkanGear::UpdateUniformBuffer(const uint32_t frameIndex, const glm::mat4& perspective, const glm::mat4& view, const float timer)
  {
    m_ubo.Projection = perspective;
    m_ubo.View = view;

    m_ubo.Model = glm::mat4(1.0f);
    m_ubo.Model = glm::translate(m_ubo.Model, m_pos);

    const float moddedRotationZ = (m_rotSpeed * timer) + m_rotOffset;
    m_ubo.Model = glm::rotate(m_ubo.Model, glm::radians(moddedRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

    m_ubo.Normal = glm::inverseTranspose(m_ubo.View * m_ubo.Model);

    m_ubo.LightPos = glm::vec3(0.0f, 0.0f, 2.5f);
    m_ubo.LightPos.x = std::sin(glm::radians(timer)) * 8.0f;
    m_ubo.LightPos.z = std::cos(glm::radians(timer)) * 8.0f;

    m_resources[frameIndex].UniformData.Upload(0, &m_ubo, sizeof(m_ubo));
  }


  void VulkanGear::SetupDescriptorSet(const uint32_t maxFramesInFlight, const VkDescriptorPool pool, const VkDescriptorSetLayout descriptorSetLayout)
  {
    m_resources.resize(maxFramesInFlight);

    for (auto& rFrame : m_resources)
    {
      rFrame.UniformData = CreateUniformBuffer(m_physicalDevice, m_device, sizeof(m_ubo));

      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.pNext = nullptr;
      allocInfo.descriptorPool = pool;
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = &descriptorSetLayout;

      RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_device, &allocInfo, &rFrame.DescriptorSet));

      // Binding 0 : Vertex shader uniform buffer
      auto vertUboBufferInfo = rFrame.UniformData.GetDescriptorBufferInfo();
      VkWriteDescriptorSet writeDescriptorSet{};
      writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSet.pNext = nullptr;
      writeDescriptorSet.dstSet = rFrame.DescriptorSet;
      writeDescriptorSet.dstBinding = 0;
      writeDescriptorSet.descriptorCount = 1;
      writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSet.pBufferInfo = &vertUboBufferInfo;

      vkUpdateDescriptorSets(m_device, 1, &writeDescriptorSet, 0, nullptr);
    }
  }


  void VulkanGear::Draw(const uint32_t frameIndex, const VkCommandBuffer cmdbuffer, const VkPipelineLayout pipelineLayout)
  {
    VkDeviceSize offsets = 0;

    vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &m_resources[frameIndex].DescriptorSet, 0, nullptr);
    vkCmdBindVertexBuffers(cmdbuffer, 0, 1, m_vertexBuffer.GetBufferPointer(), &offsets);
    vkCmdBindIndexBuffer(cmdbuffer, m_indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdbuffer, m_indexCount, 1, 0, 0, 1);
  }


  int32_t VulkanGear::NewVertex(std::vector<Vertex>& rVBuffer, const float x, const float y, const float z, const glm::vec3& normal)
  {
    Vertex v(glm::vec3(x, y, z), normal, m_color);
    rVBuffer.push_back(v);
    return static_cast<int32_t>(rVBuffer.size()) - 1;
  }
}
