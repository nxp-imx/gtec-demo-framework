#ifndef VULKAN_MODELINSTANCING_MODELINSTANCING_HPP
#define VULKAN_MODELINSTANCING_MODELINSTANCING_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMIndexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSet.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/ModelInstancing/ModelInstancingShared.hpp>
#include <array>
#include <vector>

namespace Fsl
{
  namespace Graphics3D
  {
    class SceneNode;
  }

  struct ModelMesh
  {
    Vulkan::VMVertexBuffer VertexBuffer;
    Vulkan::VMIndexBuffer IndexBuffer;
    std::array<VkVertexInputAttributeDescription, 4 + 4> VertexAttributeDescription;
    std::array<VkVertexInputBindingDescription, 2> VertexInputBindingDescription{};
  };

  class ModelInstancing final : public VulkanBasic::DemoAppVulkanBasic
  {
    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      Vulkan::VUBufferMemory UboBuffer;
      Vulkan::VUBufferMemory InstanceBuffer;
      VkDescriptorSet DescriptorSet;
    };

    struct Resources
    {
      Vulkan::VUTexture Texture;

      std::shared_ptr<Vulkan::VMBufferManager> BufferManager;
      ModelMesh Mesh;

      RapidVulkan::ShaderModule VertShader;
      RapidVulkan::ShaderModule FragShader;

      // std::array<GLES3::GLVertexAttribLink, 4> AttribLink{};
      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      std::vector<FrameResources> MainFrameResources;
      RapidVulkan::PipelineLayout MainPipelineLayout;

      Resources() = default;
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::GraphicsPipeline Pipeline;

      DependentResources() = default;
      DependentResources(const DependentResources&) = delete;
      DependentResources& operator=(const DependentResources&) = delete;
      DependentResources(DependentResources&& other) noexcept = delete;
      DependentResources& operator=(DependentResources&& other) noexcept = delete;

      void Reset() noexcept
      {
        // Reset in destruction order
        Pipeline.Reset();
        MainRenderPass.Reset();
      }
    };

    struct UBOData
    {
      Matrix MatView;
      Matrix MatProjection;
      Vector4 LightDirection;
      Vector4 LightColor;
      Vector4 AmbientColor;

      UBOData(const Vector4& lightDirection, const Vector4& lightColor, const Vector4& ambientColor)
        : LightDirection(lightDirection)
        , LightColor(lightColor)
        , AmbientColor(ambientColor)
      {
      }
    };

    Resources m_resources;

    DependentResources m_dependentResources;

    UBOData m_uboData;

    ModelInstancingShared m_shared;

  public:
    explicit ModelInstancing(const DemoAppConfig& config);

  protected:
    void OnKeyEvent(const KeyEvent& event) final;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) final;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) final;
    void OnFreeResources() final;

  private:
    void DrawMeshes(const FrameResources& frame, const VkCommandBuffer hCmdBuffer, const uint32_t instanceCount);
  };
}

#endif
