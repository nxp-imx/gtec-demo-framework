#ifndef VULKAN_FRACTALSHADER_FRACTALSHADERJULIA_HPP
#define VULKAN_FRACTALSHADER_FRACTALSHADERJULIA_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUDeviceQueueRecord.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/FractalShader/IScene.hpp>
#include <Shared/FractalShader/JuliaHelper.hpp>
#include "IVulkanScene.hpp"
#include "QuadMesh.hpp"

namespace Fsl
{
  class FractalShaderJulia : public IVulkanScene
  {
    struct Resources
    {
      RapidVulkan::ShaderModule VertShader;
      RapidVulkan::ShaderModule FragShader;
      Vulkan::VMVertexBuffer VertexBuffer;
      QuadMesh Mesh;
      RapidVulkan::DescriptorSetLayout SceneDescriptorSetLayout;
      VkDescriptorSet DescriptorSet{};
      RapidVulkan::PipelineLayout ScenePipelineLayout;

      Resources() = default;
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;
    };

    struct DependentResources
    {
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
      }
    };

    struct PushConstants
    {
      Vector2 Coordinate;
      int32_t MaxIterations{0};
    };

    Resources m_resources;
    DependentResources m_dependentResources;

    Point2 m_screenResolution;

    PushConstants m_pushConstants;

    Config m_config;

    JuliaHelper m_helper;

  public:
    FractalShaderJulia(const DemoAppConfig& config, const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                       const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                       const uint32_t maxFrames);
    ~FractalShaderJulia() override;

  protected:
    void OnBuildResources(const VulkanBasic::BuildResourcesContext& context, const VkRenderPass hRenderPass) override;
    void OnFreeResources() override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const uint32_t frameIndex, const VkCommandBuffer hCmdBuffer) override;
  };
}

#endif
