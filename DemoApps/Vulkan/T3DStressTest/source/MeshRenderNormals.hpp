#ifndef VULKAN_T3DSTRESSTEST_MESHRENDERNORMALS_HPP
#define VULKAN_T3DSTRESSTEST_MESHRENDERNORMALS_HPP
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
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics3D/Procedural/BasicMesh.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <vector>
#include "MeshRender.hpp"
#include "VertexBufferInfo.hpp"

namespace Fsl
{
  // Render the model vertex normals
  class MeshRenderNormals : public MeshRender
  {
    struct VertexUBOData
    {
      Matrix WorldViewProjection;
    };

    struct FrameResourceRecord
    {
      Vulkan::VUBufferMemory UboBuffer;
      VkDescriptorSet DescriptorSet{};
    };

    struct Resources
    {
      RapidVulkan::ShaderModule VertShader;
      RapidVulkan::ShaderModule FragShader;
      VertexBufferInfo<1> VB;
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
      RapidVulkan::GraphicsPipeline Pipeline;
    };

    Resources m_resources;
    DepedentResources m_dependentResources;

    VertexUBOData m_uboData;
    bool m_enableDepthTest;

  public:
    MeshRenderNormals(const IContentManager& contentManager, const Vulkan::VUDevice& device,
                      const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                      const uint32_t maxFramesInFlight, const Procedural::BasicMesh& mesh, const bool enableDepthTest);
    ~MeshRenderNormals() override;

    void OnBuildResources(const VulkanBasic::BuildResourcesContext& context, const VkRenderPass renderPass) override;
    void OnFreeResources() override;

    void SetWorldViewProjection(const Matrix& matrix);

    void Bind(const VkCommandBuffer hCmdBuffer, const uint32_t frameIndex) override;
    void Draw(const VkCommandBuffer hCmdBuffer) override;
    void Unbind() override;
  };
}
#endif
