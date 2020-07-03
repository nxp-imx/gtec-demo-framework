#ifndef VULKAN_T3DSTRESSTEST_MESHRENDERVB_HPP
#define VULKAN_T3DSTRESSTEST_MESHRENDERVB_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics3D/Procedural/BasicMesh.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMIndexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include "MeshFurRender.hpp"
#include "VertexBufferInfo.hpp"

namespace Fsl
{
  // Render the mesh using vertex and index buffers
  class MeshRenderVB : public MeshFurRender
  {
    struct PODVector4
    {
      float X;
      float Y;
      float Z;
      float W;
      PODVector4() = default;
      PODVector4(const float x, const float y, const float z, const float w)
        : X(x)
        , Y(y)
        , Z(z)
        , W(w)
      {
      }
    };

    struct LightData10
    {
      PODVector4 LightDirection[10];    // NOLINT(modernize-avoid-c-arrays)
      PODVector4 LightColor[10];        // NOLINT(modernize-avoid-c-arrays)
    };
    struct LightData4
    {
      PODVector4 LightDirection1;
      PODVector4 LightColor1;
      PODVector4 LightDirection2;
      PODVector4 LightColor2;
      PODVector4 LightDirection3;
      PODVector4 LightColor3;
      PODVector4 LightDirection4;
      PODVector4 LightColor4;
    };

    union LightDataUnion
    {
      LightData10 L10;
      LightData4 L4;
    };

    struct VertexUBOData
    {
      Matrix World;
      Matrix View;
      Matrix Projection;
      Vector4 Displacement;
      Vector4 AmbientColor;
      float MaxHairLength{};
      float LightCount{};
      float Reserved1{};
      float Reserved2{};
      LightDataUnion Light{};
    };


    struct FrameResourceRecord
    {
      Vulkan::VUBufferMemory UboBuffer;
      VkDescriptorSet DescriptorSet{};
    };

    struct Resources
    {
      VertexBufferInfo<3> VB;
      Vulkan::VMIndexBuffer IB;
      RapidVulkan::DescriptorSetLayout DescriptorSetLayout;
      std::vector<FrameResourceRecord> FrameResources;
      RapidVulkan::PipelineLayout PipelineLayout;

      explicit Resources(const uint32_t maxFramesInFlight)
        : FrameResources(maxFramesInFlight)
      {
      }
    };

    struct DepedentResources
    {
      RapidVulkan::GraphicsPipeline OpaquePipeline;
      RapidVulkan::GraphicsPipeline Pipeline;
    };

    Resources m_resources;
    DepedentResources m_dependentResources;

    VertexUBOData m_uboData;

    VkPrimitiveTopology m_topology;
    int m_lightCount;
    bool m_drawOpaque{false};
    bool m_enableDepthTest;

  public:
    MeshRenderVB(const IContentManager& contentManager, const Vulkan::VUDevice& device, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                 const RapidVulkan::DescriptorPool& descriptorPool, const uint32_t maxFramesInFlight, const Procedural::BasicMesh& mesh,
                 const Vulkan::VUTexture& texture0, const Vulkan::VUTexture& texture1, const IO::Path& shaderPath, const bool useHighPrecision,
                 const int lightCount, const bool enableDepthTest);
    MeshRenderVB(const IContentManager& contentManager, const Vulkan::VUDevice& device, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                 const RapidVulkan::DescriptorPool& descriptorPool, const uint32_t maxFramesInFlight, const Procedural::BasicMesh& mesh,
                 const Vulkan::VUTexture& texture0, const Vulkan::VUTexture& texture1, const IO::Path& vertShaderPath, const IO::Path& fragShaderPath,
                 const int lightCount, const bool enableDepthTest);


    void OnBuildResources(const VulkanBasic::BuildResourcesContext& context, const VkRenderPass renderPass) override;
    void OnFreeResources() override;

    void Bind(const VkCommandBuffer hCmdBuffer, const uint32_t frameIndex) override;
    void Draw(const VkCommandBuffer hCmdBuffer) override;
    void Unbind() override;

    void SetWorld(const Matrix& matrix);
    void SetView(const Matrix& matrix);
    void SetProjection(const Matrix& matrix);

    void SetDisplacement(const Vector3& displacement);
    void SetCurrentLayer(const VkCommandBuffer hCmdBuffer, const float currentLayer);

    int GetLightCount() const;
    void SetLightDirection(const int index, const Vector3& lightDirection);
    void SetLightColor(const int index, const Vector3& lightColor);
    void SetLightAmbientColor(const Vector3& ambientColor);

    void SetMaxHairLength(const float maxHairLength);
    void SetDrawOpaque(const bool enabled);

  private:
    void DoConstruct(const IContentManager& contentManager, const Vulkan::VUDevice& device,
                     const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                     const Procedural::BasicMesh& mesh, const Vulkan::VUTexture& texture0, const Vulkan::VUTexture& texture1);
  };
}
#endif
