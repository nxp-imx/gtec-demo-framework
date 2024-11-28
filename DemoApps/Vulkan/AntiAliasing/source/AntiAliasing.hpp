#ifndef VULKAN_ANTIALIASING_ANTIALIASING_HPP
#define VULKAN_ANTIALIASING_ANTIALIASING_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslUtil/Vulkan1_0/Managed/Draw/VMLineDraw.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/AntiAliasing/AntiAliasingShared.hpp>
#include <memory>
#include <utility>
#include "RenderModel.hpp"

namespace Fsl
{
  class AntiAliasing final : public VulkanBasic::DemoAppVulkanBasic
  {
    using base_type = VulkanBasic::DemoAppVulkanBasic;

    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      VkDescriptorSet DescriptorSetEffect{VK_NULL_HANDLE};
      Vulkan::VUBufferMemory CustomZoomVertUboBuffer;
      Vulkan::VUBufferMemory ZoomVertUboBuffer;
      VkDescriptorSet CustomZoomDescriptorSet{VK_NULL_HANDLE};
      VkDescriptorSet ZoomDescriptorSet{VK_NULL_HANDLE};
    };

    struct VertexUBOData
    {
      Matrix MatViewProjection;
    };

    struct LineProgramInfo
    {
      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;
    };

    struct PostProcessProgramInfo
    {
      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;
    };

    struct LineDrawResources
    {
      LineProgramInfo ProgramInfo;
      Vulkan::VMLineDraw Render;
    };


    struct PostProcessResources
    {
      PostProcessProgramInfo ProgramInfo;
      Vulkan::VMVertexBuffer VertexBuffer;
      RapidVulkan::DescriptorSetLayout DescriptorSetLayout;
      RapidVulkan::PipelineLayout PipelineLayout;

      PostProcessResources() = default;
      PostProcessResources(PostProcessProgramInfo programInfo, Vulkan::VMVertexBuffer vertexBuffer,
                           RapidVulkan::DescriptorSetLayout descriptorSetLayout, RapidVulkan::PipelineLayout pipelineLayout)
        : ProgramInfo(std::move(programInfo))
        , VertexBuffer(std::move(vertexBuffer))
        , DescriptorSetLayout(std::move(descriptorSetLayout))
        , PipelineLayout(std::move(pipelineLayout))
      {
      }
    };

    struct ZoomProgramInfo
    {
      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;
    };

    struct ZoomResources
    {
      ZoomProgramInfo ProgramInfo;
      Vulkan::VMVertexBuffer VertexBuffer;
      Vulkan::VMVertexBuffer VertexBufferCustomZoom;
      RapidVulkan::DescriptorSetLayout DescriptorSetLayout;
      RapidVulkan::PipelineLayout PipelineLayout;

      ZoomResources() = default;
      ZoomResources(ZoomProgramInfo programInfo, Vulkan::VMVertexBuffer vertexBuffer, Vulkan::VMVertexBuffer vertexBufferCustomZoom,
                    RapidVulkan::DescriptorSetLayout descriptorSetLayout, RapidVulkan::PipelineLayout pipelineLayout)
        : ProgramInfo(std::move(programInfo))
        , VertexBuffer(std::move(vertexBuffer))
        , VertexBufferCustomZoom(std::move(vertexBufferCustomZoom))
        , DescriptorSetLayout(std::move(descriptorSetLayout))
        , PipelineLayout(std::move(pipelineLayout))
      {
      }
    };


    struct Resources
    {
      LineDrawResources Line;
      LineDrawResources DividerLine;
      PostProcessResources PostProcess;
      ZoomResources Zoom;

      std::unique_ptr<RenderModel> RenderModelCar;
      std::unique_ptr<RenderModel> RenderModelKnight;

      RapidVulkan::DescriptorPool MainDescriptorPool;

      std::vector<FrameResources> MainFrameResources;

      Resources() noexcept = delete;
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;

      Resources(LineDrawResources line, LineDrawResources dividerLine, PostProcessResources postProcess, ZoomResources zoom,
                std::unique_ptr<RenderModel> renderModelCar, std::unique_ptr<RenderModel> renderModelKnight,
                RapidVulkan::DescriptorPool mainDescriptorPool, std::vector<FrameResources> mainFrameResources)
        : Line(std::move(line))
        , DividerLine(std::move(dividerLine))
        , PostProcess(std::move(postProcess))
        , Zoom(std::move(zoom))
        , RenderModelCar(std::move(renderModelCar))
        , RenderModelKnight(std::move(renderModelKnight))
        , MainDescriptorPool(std::move(mainDescriptorPool))
        , MainFrameResources(std::move(mainFrameResources))
      {
      }
    };

    struct FrameBufferResources
    {
      AntiAliasingMethod ActiveAntiAliasingMethod{AntiAliasingMethod::Disabled};

    private:
      PxSize2D m_size;
      VkSampleCountFlagBits m_sampleCountFlagBit{VK_SAMPLE_COUNT_1_BIT};

    public:
      Vulkan::VUImageMemoryView DepthImageView;
      RapidVulkan::RenderPass RP;
      Vulkan::VUTexture UnresolvedColor;
      Vulkan::VUTexture ResolvedColor;
      RapidVulkan::Sampler NearestSampler;
      RapidVulkan::Framebuffer FB;
      PxSize1D ScalePx;

      FrameBufferResources() = default;
      FrameBufferResources(const FrameBufferResources&) = delete;
      FrameBufferResources& operator=(const FrameBufferResources&) = delete;
      FrameBufferResources(FrameBufferResources&& other) noexcept = default;
      FrameBufferResources& operator=(FrameBufferResources&& other) noexcept = default;

      FrameBufferResources(const AntiAliasingMethod activeAntiAliasingMethod, const PxSize2D sizePx, const VkSampleCountFlagBits sampleCountFlagBit,
                           Vulkan::VUImageMemoryView depthImageView, RapidVulkan::RenderPass rp, Vulkan::VUTexture unresolvedColor,
                           Vulkan::VUTexture resolvedColor, RapidVulkan::Sampler nearestSampler, RapidVulkan::Framebuffer fb, const PxSize1D scalePx)
        : ActiveAntiAliasingMethod(activeAntiAliasingMethod)
        , m_size(sizePx)
        , m_sampleCountFlagBit(sampleCountFlagBit)
        , DepthImageView(std::move(depthImageView))
        , RP(std::move(rp))
        , UnresolvedColor(std::move(unresolvedColor))
        , ResolvedColor(std::move(resolvedColor))
        , NearestSampler(std::move(nearestSampler))
        , FB(std::move(fb))
        , ScalePx(scalePx)
      {
      }

      void Reset() noexcept
      {
        // Reset in destruction order
        ScalePx = {};
        FB.Reset();
        ResolvedColor.Reset();
        UnresolvedColor.Reset();
        RP.Reset();
        DepthImageView.Reset();
        m_sampleCountFlagBit = VK_SAMPLE_COUNT_1_BIT;
        m_size = {};
        ActiveAntiAliasingMethod = AntiAliasingMethod::Disabled;
      }

      VkDescriptorImageInfo GetDescriptorImageInfo() const
      {
        if (ResolvedColor.IsValid())
        {
          return ResolvedColor.GetDescriptorImageInfo();
        }
        return UnresolvedColor.GetDescriptorImageInfo();
      }

      VkImage GetImage() const
      {
        if (ResolvedColor.IsValid())
        {
          return ResolvedColor.GetImage();
        }
        return UnresolvedColor.GetImage();
      }

      VkExtent2D GetFrameBufferExtent() const noexcept
      {
        return {UncheckedNumericCast<uint32_t>(m_size.RawWidth()), UncheckedNumericCast<uint32_t>(m_size.RawHeight())};
      }

      PxSize2D GetFrameBufferSize() const noexcept
      {
        return m_size;
      }

      VkSampleCountFlagBits GetSampleCountFlagBit() const noexcept
      {
        return m_sampleCountFlagBit;
      }
    };

    struct CustomZoomFrameBufferResources
    {
    private:
      PxSize2D m_size;

    public:
      RapidVulkan::RenderPass RP;
      Vulkan::VUTexture Color;
      RapidVulkan::Framebuffer FB;

      CustomZoomFrameBufferResources() = default;
      CustomZoomFrameBufferResources(const CustomZoomFrameBufferResources&) = delete;
      CustomZoomFrameBufferResources& operator=(const CustomZoomFrameBufferResources&) = delete;
      CustomZoomFrameBufferResources(CustomZoomFrameBufferResources&& other) noexcept = default;
      CustomZoomFrameBufferResources& operator=(CustomZoomFrameBufferResources&& other) noexcept = default;

      CustomZoomFrameBufferResources(const PxSize2D sizePx, RapidVulkan::RenderPass rp, Vulkan::VUTexture color, RapidVulkan::Framebuffer fb)
        : m_size(sizePx)
        , RP(std::move(rp))
        , Color(std::move(color))
        , FB(std::move(fb))
      {
      }

      void Reset() noexcept
      {
        // Reset in destruction order
        FB.Reset();
        Color.Reset();
        RP.Reset();
        m_size = {};
      }

      VkExtent2D GetFrameBufferExtent() const noexcept
      {
        return {UncheckedNumericCast<uint32_t>(m_size.RawWidth()), UncheckedNumericCast<uint32_t>(m_size.RawHeight())};
      }


      PxSize2D GetFrameBufferSize() const noexcept
      {
        return m_size;
      }
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      FrameBufferResources Offscreen;
      CustomZoomFrameBufferResources CustomZoom;
      RapidVulkan::GraphicsPipeline PostProcessPipeline;
      RapidVulkan::GraphicsPipeline CustomZoomPipeline;
      RapidVulkan::GraphicsPipeline ZoomPipeline;

      DependentResources() noexcept = default;
      DependentResources(const DependentResources&) = delete;
      DependentResources& operator=(const DependentResources&) = delete;
      DependentResources(DependentResources&& other) noexcept = delete;
      DependentResources& operator=(DependentResources&& other) noexcept = delete;

      void Reset() noexcept
      {
        // Reset in destruction order
        ZoomPipeline.Reset();
        CustomZoomPipeline.Reset();
        PostProcessPipeline.Reset();
        CustomZoom.Reset();
        Offscreen.Reset();
        MainRenderPass.Reset();
      }
    };

    AntiAliasingShared m_shared;
    std::shared_ptr<ITextureService> m_textureService;
    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;

    Resources m_resources;

    DependentResources m_dependentResources;
    VertexUBOData m_linesVertexUboData;
    VertexUBOData m_dividerLinesVertexUboData;
    VertexUBOData m_customZoomVertexUboData;
    VertexUBOData m_zoomVertexUboData;

  public:
    explicit AntiAliasing(const DemoAppConfig& config);
    ~AntiAliasing() final;

  protected:
    void OnKeyEvent(const KeyEvent& event) final;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void Resolve(const DemoTime& demoTime) final;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) final;

  private:
    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) final;
    void OnFreeResources() final;

    static FrameBufferResources CreateFramebufferResources(const Vulkan::VUDevice& device, const VulkanBasic::BuildResourcesContext& context,
                                                           const PxSize2D sizePx, const AntiAliasingMethod aaMethod);
    static CustomZoomFrameBufferResources CreateCustomZoomResources(const Vulkan::VUDevice& device, const VulkanBasic::BuildResourcesContext& context,
                                                                    const PxSize2D maxZoomAreaSizePx);

    static LineProgramInfo CreateLineProgram(const Vulkan::VUDevice& device, const std::shared_ptr<IContentManager>& contentManager);
    static PostProcessProgramInfo CreatePostProcessProgram(const Vulkan::VUDevice& device, const std::shared_ptr<IContentManager>& contentManager);
    static ZoomProgramInfo CreateZoomProgram(const Vulkan::VUDevice& device, const std::shared_ptr<IContentManager>& contentManager);

    static AntiAliasing::Resources CreateResources(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                   const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, ITextureService* pTextureService,
                                                   const std::shared_ptr<IContentManager>& contentManager, const uint32_t maxFramesInFlight);

    void RecreateFrameBufferIfNecessary();
    PxSize2D ApplyViewport(const VkCommandBuffer hCmdBuffer, const ContentWindowId contentWindowId);

    static void GenerateCustomZoomVertices(Vulkan::VMVertexBuffer& rVertexBuffer, const PxSize2D textureSizePx, const PxSize1D texScalePx,
                                           const UI::Custom::ZoomInfo& zoomInfo);
    static void GenerateCustomZoomVertices2(Vulkan::VMVertexBuffer& rVertexBuffer, const PxSize2D sizePx, const UI::Custom::ZoomInfo& zoomInfo);
    static void GenerateZoomVertices(Vulkan::VMVertexBuffer& rVertexBuffer, const PxSize2D sizePx, const PxSize1D texScalePx,
                                     const UI::Custom::ZoomInfo& zoomInfo);

    void DrawToMultisampleFB(RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext, const DemoTime& demoTime);
    void DrawToZoomFB(FrameResources& rFrameResources, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext,
                      const UI::Custom::ZoomInfo& zoomInfo, const PxSize2D srcSizePx, const PxSize1D srcTexScalePx);
    void DrawFinalComposite(FrameResources& rFrameResources, const VkCommandBuffer hCmdBuffer, const VkExtent2D& extent,
                            const UI::Custom::ZoomInfo& zoomInfo, const bool useCustomZoom);

    // static void ResolveFrameBuffer(const GLuint hDst, const PxSize2D dstSizePx, const GLuint hSrc, const PxSize2D srcSizePx);
    // static void DrawFrameBuffer(const PostProcessResources& resources, const GLuint hTexture);
    // static void DrawZoomArea(const GLuint hZoomTexture, const ZoomProgramInfo& programInfo, const Vulkan::VMVertexBuffer& vertexBuffer,
    //                          const Matrix& matWorldViewProjection, const bool useLinearFiltering = false);
  };
}

#endif
