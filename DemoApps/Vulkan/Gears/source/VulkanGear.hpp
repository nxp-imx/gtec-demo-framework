#ifndef VULKAN_GEARS_VULKANGEAR_HPP
#define VULKAN_GEARS_VULKANGEAR_HPP
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

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanDevice.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanUniformData.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanBuffer.hpp>
#include <vector>
#include <glm/glm.hpp>

namespace Fsl
{
  struct Vertex
  {
    float Pos[3]{};       // NOLINT(modernize-avoid-c-arrays)
    float Normal[3]{};    // NOLINT(modernize-avoid-c-arrays)
    float Color[3]{};     // NOLINT(modernize-avoid-c-arrays)

    Vertex()
    //: Pos{} // bogus warning in VC2013
    //, Normal{}
    //, Color{}
    {
      // Bypass warning
      Pos[0] = 0;
      Pos[1] = 0;
      Pos[2] = 0;
      Color[0] = 0;
      Color[1] = 0;
      Color[2] = 0;
      Normal[0] = 0;
      Normal[1] = 0;
      Normal[2] = 0;
    }

    Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec3& c)
    {
      Pos[0] = p.x;
      Pos[1] = p.y;
      Pos[2] = p.z;
      Color[0] = c.x;
      Color[1] = c.y;
      Color[2] = c.z;
      Normal[0] = n.x;
      Normal[1] = n.y;
      Normal[2] = n.z;
    }
  };

  struct GearInfo
  {
    float InnerRadius = 0.0f;
    float OuterRadius = 0.0f;
    float Width = 0.0f;
    int NumTeeth = 0;
    float ToothDepth = 0.0f;
    glm::vec3 Color;
    glm::vec3 Pos;
    float RotSpeed = 0.0f;
    float RotOffset = 0.0f;
  };


  class VulkanGear
  {
    struct UBO
    {
      glm::mat4 Projection;
      glm::mat4 Model;
      glm::mat4 Normal;
      glm::mat4 View;
      glm::vec3 LightPos;
    };

    Willems::VulkanDevice* m_pVulkanDevice;

    glm::vec3 m_color;
    glm::vec3 m_pos;
    float m_rotSpeed = 0.0f;
    float m_rotOffset = 0.0f;

    Willems::VulkanBuffer m_vertexBuffer;
    Willems::VulkanBuffer m_indexBuffer;
    uint32_t m_indexCount = 0;

    UBO m_ubo;
    Willems::VulkanUniformData m_uniformData;

  public:
    VkDescriptorSet DescriptorSet;

    explicit VulkanGear(Willems::VulkanDevice* pVulkanDevice);
    ~VulkanGear();

    void Generate(const GearInfo& gearinfo, const VkQueue queue);

    void UpdateUniformBuffer(const glm::mat4& perspective, const glm::vec3& rotation, const float zoom, const float timer);
    void SetupDescriptorSet(const VkDescriptorPool pool, const VkDescriptorSetLayout descriptorSetLayout);
    void Draw(const VkCommandBuffer cmdbuffer, const VkPipelineLayout pipelineLayout);

  private:
    int32_t NewVertex(std::vector<Vertex>& rVBuffer, const float x, const float y, const float z, const glm::vec3& normal);
    void NewFace(std::vector<uint32_t>& rIBuffer, const int a, const int b, const int c);

    void PrepareUniformBuffer();
  };
}

#endif
