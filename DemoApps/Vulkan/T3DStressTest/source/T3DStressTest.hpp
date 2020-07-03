#ifndef VULKAN_T3DSTRESSTEST_T3DSTRESSTEST_HPP
#define VULKAN_T3DSTRESSTEST_T3DSTRESSTEST_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslGraphics3D/Procedural/BasicMesh.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/T3DStressTest/Config.hpp>
#include "MeshRenderNormals.hpp"
#include "MeshRenderVB.hpp"
#include "MeshRenderVBInstanced.hpp"

namespace Fsl
{
  class T3DStressTest : public VulkanBasic::DemoAppVulkanBasic
  {
    struct MeshStuffRecord
    {
      Procedural::BasicMesh Mesh;
      MeshRenderVB RenderVB;
      MeshRenderVBInstanced RenderInstanced;
      MeshRenderNormals RenderNormals;

      MeshStuffRecord(const IContentManager& contentManager, const Vulkan::VUDevice& device,
                      const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                      const uint32_t maxFramesInFlight, const Procedural::BasicMesh& mesh, const Vulkan::VUTexture& texture0,
                      const Vulkan::VUTexture& texture1, const bool useHighPrecision, const int lightCount, const bool enableDepthTest)
        : Mesh(mesh)
        , RenderVB(contentManager, device, bufferManager, descriptorPool, maxFramesInFlight, mesh, texture0, texture1, "MultiPass", useHighPrecision,
                   lightCount, enableDepthTest)
        , RenderInstanced(contentManager, device, bufferManager, descriptorPool, maxFramesInFlight, mesh, texture0, texture1, "Instanced",
                          useHighPrecision, lightCount, enableDepthTest)
        , RenderNormals(contentManager, device, bufferManager, descriptorPool, maxFramesInFlight, mesh, enableDepthTest)
      {
      }

      void OnBuildResources(const VulkanBasic::BuildResourcesContext& context, const VkRenderPass renderPass)
      {
        FSLLOG3_VERBOSE3("MeshStuffRecord::OnBuildResources");
        RenderVB.OnBuildResources(context, renderPass);
        RenderInstanced.OnBuildResources(context, renderPass);
        RenderNormals.OnBuildResources(context, renderPass);
      }
      void OnFreeResources()
      {
        RenderNormals.OnFreeResources();
        RenderInstanced.OnFreeResources();
        RenderVB.OnFreeResources();
      }
    };

    struct Resources
    {
      RapidVulkan::DescriptorPool MainDescriptorPool;
      std::unique_ptr<MeshStuffRecord> MeshStuff;
      Vulkan::VUTexture Tex1;
      Vulkan::VUTexture Tex2;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
    };

    Config m_config;
    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;
    Resources m_resources;
    DependentResources m_dependentResources;

    int m_xAngle;
    int m_yAngle;
    int m_zAngle;

    Matrix m_rotate;
    Matrix m_view;
    Matrix m_perspective;
    Matrix m_MVP;
    Matrix m_world;

    Vector3 m_gravity;
    Vector3 m_displacement;
    float m_radians;

  public:
    explicit T3DStressTest(const DemoAppConfig& config);

  protected:
    void FixedUpdate(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;
    AppDrawResult TrySwapBuffers(const DemoTime& demoTime) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

    void DrawToCommandBuffer(const VkCommandBuffer hCmdBuffer, const uint32_t cmdBufferIndex, const uint32_t frameIndex);
    void ForceFinishEachFrame();
  };
}

#endif
