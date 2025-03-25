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

#include <FslGraphics/Vertices/BasicVertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslGraphics/Vertices/VertexElement.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMIndexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <glm/glm.hpp>
#include <cstddef>
#include <vector>

namespace Fsl
{
  struct Vertex
  {
    float Pos[3]{};       // NOLINT(modernize-avoid-c-arrays)
    float Normal[3]{};    // NOLINT(modernize-avoid-c-arrays)
    float Color[3]{};     // NOLINT(modernize-avoid-c-arrays)

    Vertex() = default;

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

    constexpr static VertexDeclarationArray<3> GetVertexDeclarationArray()
    {
      constexpr BasicVertexDeclarationArray<3> Elements = {
        VertexElement(offsetof(Vertex, Pos), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
        VertexElement(offsetof(Vertex, Normal), VertexElementFormat::Vector3, VertexElementUsage::Color, 0),
        VertexElement(offsetof(Vertex, Color), VertexElementFormat::Vector3, VertexElementUsage::Normal, 0),
      };
      return {Elements, sizeof(Vertex)};
    }


    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<3> Decl = GetVertexDeclarationArray();
      return Decl.AsReadOnlySpan();
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

    struct Resources
    {
      Vulkan::VUBufferMemory UniformData;
      VkDescriptorSet DescriptorSet{VK_NULL_HANDLE};
    };

    Vulkan::VUPhysicalDeviceRecord m_physicalDevice;
    VkDevice m_device;
    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;

    glm::vec3 m_color{};
    glm::vec3 m_pos{};
    float m_rotSpeed = 0.0f;
    float m_rotOffset = 0.0f;

    Vulkan::VMVertexBuffer m_vertexBuffer;
    Vulkan::VMIndexBuffer m_indexBuffer;
    uint32_t m_indexCount = 0;

    UBO m_ubo{};
    std::vector<Resources> m_resources;

  public:
    explicit VulkanGear(Vulkan::VUPhysicalDeviceRecord physicalDevice, const VkDevice device, std::shared_ptr<Vulkan::VMBufferManager> bufferManager);
    ~VulkanGear();

    void Generate(const GearInfo& gearinfo, const VkQueue queue);

    void UpdateUniformBuffer(const uint32_t frameIndex, const glm::mat4& perspective, const glm::mat4& view, const float timer);
    void SetupDescriptorSet(const uint32_t maxFramesInFlight, const VkDescriptorPool pool, const VkDescriptorSetLayout descriptorSetLayout);
    void Draw(const uint32_t frameIndex, const VkCommandBuffer cmdbuffer, const VkPipelineLayout pipelineLayout);

  private:
    int32_t NewVertex(std::vector<Vertex>& rVBuffer, const float x, const float y, const float z, const glm::vec3& normal);
  };
}

#endif
