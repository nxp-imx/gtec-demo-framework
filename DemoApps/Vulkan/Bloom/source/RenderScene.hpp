#ifndef VULKAN_BLOOM_RENDERSCENE_HPP
#define VULKAN_BLOOM_RENDERSCENE_HPP
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

#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMIndexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUDeviceQueueRecord.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/Bloom/IScene.hpp>
#include <vector>
#include "IVulkanScene.hpp"

namespace Fsl
{
  class RenderScene : public IVulkanScene
  {
    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      Vulkan::VUBufferMemory UboBuffer;
      VkDescriptorSet DescriptorSet{};
    };

    struct ModelMesh
    {
      Vulkan::VMVertexBuffer VertexBuffer;
      Vulkan::VMIndexBuffer IndexBuffer;
      std::array<VkVertexInputAttributeDescription, 4> VertexAttributeDescription;
      VkVertexInputBindingDescription VertexInputBindingDescription{};
    };

    struct Resources
    {
      Vulkan::VUTexture Texture;
      Vulkan::VUTexture TextureNormal;
      Vulkan::VUTexture TextureSpecular;
      RapidVulkan::ShaderModule VertShader;
      RapidVulkan::ShaderModule FragShader;

      ModelMesh Mesh;

      RapidVulkan::DescriptorSetLayout SceneDescriptorSetLayout;
      std::vector<FrameResources> SceneFrameResources;

      RapidVulkan::PipelineLayout ScenePipelineLayout;
    };

    struct DependentResources
    {
      RapidVulkan::GraphicsPipeline Pipeline;
    };


    struct UBOData
    {
      Matrix MatWorldView;
      Matrix MatWorldViewProjection;
      Matrix NormalMatrix;
      Vector4 LightDirection;
      Vector4 LightColor;
      Vector4 MatAmbient;
      Vector4 MatSpecular;
      float MatShininess{0};

      UBOData(const Vector4& lightColor, const Vector4& matAmbient, const Vector4& matSpecular, const float matShininess)
        : LightColor(lightColor)
        , MatAmbient(matAmbient)
        , MatSpecular(matSpecular)
        , MatShininess(matShininess)
      {
      }
    };

    Resources m_resources;
    DependentResources m_dependentResources;

    Matrix m_matrixWorld;
    Matrix m_matrixView;
    Matrix m_matrixProjection;
    Vector3 m_lightDirection;

    UBOData m_uboData;

  public:
    RenderScene(const DemoAppConfig& config, const int32_t sceneId, const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                const uint32_t maxFrames);
    ~RenderScene() override;

    void OnBuildResources(const VulkanBasic::BuildResourcesContext& context, const VkRenderPass hRenderPass) override;
    void OnFreeResources() override;

    void Update(const DemoTime& demoTime, const Matrix& cameraViewMatrix, const Matrix& cameraRotation, const Vector3& rotation,
                const Point2& screenResolution) override;
    void PreDraw(const uint32_t frameIndex, const VkCommandBuffer hCmdBuffer) override;
    void Draw(const uint32_t frameIndex, const VkCommandBuffer hCmdBuffer) override;

  private:
    void PrepareShader(const VkDevice device, const std::shared_ptr<IContentManager>& contentManager, const bool useSpecMap, const bool useGlossMap,
                       const bool useNormalMap);

    static RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                        const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                        const ModelMesh& mesh, const VkRenderPass renderPass, const uint32_t subpass,
                                                        const bool cullEnabled);
  };
}

#endif
