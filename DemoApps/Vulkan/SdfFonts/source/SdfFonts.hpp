#ifndef VULKAN_SDFFONTS_SDFFONTS_HPP
#define VULKAN_SDFFONTS_SDFFONTS_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/ReadOnlySpan.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <Shared/SdfFonts/Shared.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <array>
#include <vector>
#include "DynamicMesh.hpp"

namespace Fsl
{
  class SdfFonts : public VulkanBasic::DemoAppVulkanBasic
  {
    struct PushConstantRecord
    {
      Matrix ModelViewProj;
      float Smoothing{};
      float ShadowOffsetX{};
      float ShadowOffsetY{};
      float ShadowSmoothing{};
      float OutlineDistance{};
    };

    struct MeshRecord
    {
      PxPoint2 Offset;
      BitmapFontConfig FontConfig;

      DynamicMesh Mesh;

      MeshRecord() = default;
      MeshRecord(const PxPoint2& offset, const BitmapFontConfig& fontConfig, DynamicMesh mesh)
        : Offset(offset)
        , FontConfig(fontConfig)
        , Mesh(std::move(mesh))
      {
      }
    };

    struct FontRecord
    {
      TextureAtlasBitmapFont Font;
      Vulkan::VUTexture Texture;
    };

    struct ExampleRecord
    {
      FontRecord Font;
      MeshRecord Mesh;
      MeshRecord ScaledMesh;
    };

    struct Resources
    {
      std::shared_ptr<Vulkan::VMBufferManager> BufferManager;
      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      VkDescriptorSet DescriptorSetNormal{};
      VkDescriptorSet DescriptorSetSdf{};
      RapidVulkan::PipelineLayout MainPipelineLayout;

      ExampleRecord Normal;
      ExampleRecord Sdf;

      RapidVulkan::ShaderModule VertShader;
      RapidVulkan::ShaderModule FragShaderText;
      RapidVulkan::ShaderModule FragShaderSdf;
      RapidVulkan::ShaderModule FragShaderSdfOutline;
      RapidVulkan::ShaderModule FragShaderSdfShadow;
      RapidVulkan::ShaderModule FragShaderSdfShadowAndOutline;

      AtlasTexture2D FillTexture;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::GraphicsPipeline PipelineNormal;
      RapidVulkan::GraphicsPipeline PipelineSdf;
      RapidVulkan::GraphicsPipeline PipelineSdfOutline;
      RapidVulkan::GraphicsPipeline PipelineSdfShadow;
      RapidVulkan::GraphicsPipeline PipelineSdfShadowAndOutline;
    };

    Shared m_shared;

    std::shared_ptr<INativeBatch2D> m_nativeBatch;

    Resources m_resources;
    std::vector<FontGlyphPosition> m_positionsScratchpad;

    DependentResources m_dependentResources;

    PushConstantRecord m_pushConstants;

  public:
    explicit SdfFonts(const DemoAppConfig& config);
    ~SdfFonts() override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

  private:
    void DrawNow(const VkCommandBuffer hCmdBuffer, const uint32_t currentFrameIndex);
    void DrawMeshes(const VkCommandBuffer hCmdBuffer, const uint32_t currentFrameIndex, const FontDrawConfig& fontDrawConfig,
                    const RapidVulkan::GraphicsPipeline& sdfPipeline);
    void DrawTextMesh(const VkCommandBuffer hCmdBuffer, const uint32_t currentFrameIndex, const MeshRecord& mesh, const Vulkan::VUTexture& texture,
                      const RapidVulkan::GraphicsPipeline& pipeline, const VkDescriptorSet descriptorSet, const FontDrawConfig& fontDrawConfig);

    ExampleRecord PrepareExample(const uint32_t maxFramesInFlight, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                 const IContentManager& contentManager, const int32_t lineYPx, const IO::Path& bitmapFontPath,
                                 const IO::Path& fontAtlasTexturePath, const StringViewLite& strView,
                                 std::vector<FontGlyphPosition>& rPositionsScratchpad);

    MeshRecord GenerateMesh(const uint32_t maxFramesInFlight, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                            const PxPoint2& dstPositionPx, const FontRecord& fontRecord, const BitmapFontConfig& fontConfig,
                            const StringViewLite& strView, std::vector<FontGlyphPosition>& rPositionsScratchpad);
    void RegenerateMeshOnDemand(MeshRecord& rMeshRecord, const uint32_t currentFrameIndex, const PxPoint2& dstPositionPx,
                                const FontRecord& fontRecord, const BitmapFontConfig fontConfig, const StringViewLite& strView,
                                std::vector<FontGlyphPosition>& rPositionsScratchpad);

    const RapidVulkan::GraphicsPipeline& SelectPipeline(const SdfFontMode fontSdfMode);
  };
}

#endif
