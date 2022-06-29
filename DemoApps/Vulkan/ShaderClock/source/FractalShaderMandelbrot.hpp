#ifndef VULKAN_SHADERCLOCK_FRACTALSHADERMANDELBROT_HPP
#define VULKAN_SHADERCLOCK_FRACTALSHADERMANDELBROT_HPP
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
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Vulkan/Basic/BuildResourcesContext.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUDeviceQueueRecord.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include "QuadMesh.hpp"

namespace Fsl
{
  class FractalShaderMandelbrot
  {
    struct Resources
    {
      RapidVulkan::ShaderModule VertShader;
      RapidVulkan::ShaderModule FragShader;
      RapidVulkan::ShaderModule FragShaderHeatmap;
      Vulkan::VMVertexBuffer VertexBuffer;
      QuadMesh Mesh;
      RapidVulkan::DescriptorSetLayout SceneDescriptorSetLayout;
      VkDescriptorSet DescriptorSet{};
      RapidVulkan::PipelineLayout ScenePipelineLayout;
    };

    struct DependentResources
    {
      RapidVulkan::GraphicsPipeline Pipeline;
      RapidVulkan::GraphicsPipeline HeatmapPipeline;
    };

    struct PushConstants
    {
      Vector2 Coordinate;
      float Scale{0};
      float HeatmapScale{0};
      uint32_t MaxIterations{0};
    };

    Resources m_resources;
    DependentResources m_dependentResources;

    Point2 m_screenResolution;

    PushConstants m_pushConstants;
    Vector2 m_angle;
    bool m_allowHeatmap;
    bool m_showHeatmap;

  public:
    FractalShaderMandelbrot(const DemoAppConfig& config, const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                            const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                            const uint32_t maxFrames, const bool allowHeatmap);
    ~FractalShaderMandelbrot();

    void ShowHeatmap(const bool value)
    {
      m_showHeatmap = m_allowHeatmap ? value : false;
    }

    void SetHeatmapScale(const float value)
    {
      m_pushConstants.HeatmapScale = value;
    }

    void SetMaxIterations(const uint32_t value)
    {
      m_pushConstants.MaxIterations = value;
    }

    void OnBuildResources(const VulkanBasic::BuildResourcesContext& context, const VkRenderPass hRenderPass);
    void OnFreeResources();

    void Update(const DemoTime& demoTime);
    void Draw(const uint32_t frameIndex, const VkCommandBuffer hCmdBuffer);
  };
}

#endif
