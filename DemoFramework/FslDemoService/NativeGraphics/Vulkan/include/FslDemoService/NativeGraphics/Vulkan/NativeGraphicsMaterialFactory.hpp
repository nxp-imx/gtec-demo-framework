#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSMATERIALFACTORY_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSMATERIALFACTORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialHandle.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderHandle.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElement.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElementFormat.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElementFormatUtil.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescriptions.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <memory>
#include <utility>

namespace Fsl::Vulkan
{
  class NativeGraphicsMaterialFactory
  {
    struct ShaderRecord
    {
      BasicShaderStageFlag Flag{BasicShaderStageFlag::Undefined};
      RapidVulkan::ShaderModule Shader;
      VertexAttributeDescriptions ShaderVertexAttributeDescriptions;

      ShaderRecord() = default;
      ShaderRecord(const BasicShaderStageFlag flag, RapidVulkan::ShaderModule shader, VertexAttributeDescriptions shaderVertexAttributeDescriptions)
        : Flag(flag)
        , Shader(std::move(shader))
        , ShaderVertexAttributeDescriptions(std::move(shaderVertexAttributeDescriptions))
      {
      }
    };

    struct PipelineRecord
    {
      RapidVulkan::GraphicsPipeline Pipeline;
      VkPipelineLayout PipelineLayout{VK_NULL_HANDLE};
      BasicMaterialInfo MaterialInfo;
      VertexDeclaration VertexDecl;
      BasicNativeShaderHandle VertexShaderHandle;
      BasicNativeShaderHandle FragmentShaderHandle;
      uint32_t ReferenceCount{0};

      PipelineRecord() = default;

      explicit PipelineRecord(RapidVulkan::GraphicsPipeline pipeline, const VkPipelineLayout pipelineLayout, const BasicMaterialInfo& materialInfo,
                              VertexDeclarationSpan vertexDeclaration, const BasicNativeShaderHandle vertexShaderHandle,
                              const BasicNativeShaderHandle fragmentShaderHandle)
        : Pipeline(std::move(pipeline))
        , PipelineLayout(pipelineLayout)
        , MaterialInfo(materialInfo)
        , VertexDecl(vertexDeclaration)
        , VertexShaderHandle(vertexShaderHandle)
        , FragmentShaderHandle(fragmentShaderHandle)
      {
      }

      bool IsValid() const
      {
        return Pipeline.IsValid();
      }
    };

  public:
    struct VariableInfo
    {
      BasicMaterialVariableElement VariableElement;
      uint32_t ByteSize{0};
      VkShaderStageFlags ShaderStageFlags{0};

      constexpr VariableInfo() noexcept = default;
      constexpr VariableInfo(const BasicMaterialVariableElement variableElement, const uint32_t byteSize,
                             const VkShaderStageFlags shaderStageFlags) noexcept
        : VariableElement(variableElement)
        , ByteSize(byteSize)
        , ShaderStageFlags(shaderStageFlags)
      {
        assert(byteSize == BasicMaterialVariableElementFormatUtil::GetBytesPerElement(variableElement.Format));
      }
    };

    struct MaterialRecord
    {
      HandleVector<PipelineRecord>::handle_type PipelineHandle{HandleVectorConfig::InvalidHandle};
      VkPipeline Pipeline{VK_NULL_HANDLE};
      VkPipelineLayout PipelineLayout{VK_NULL_HANDLE};
      VariableInfo VarModelViewProj;
      VariableInfo VarSdfSmoothing;
      bool IsSdfBased{false};
      uint32_t CameraChangeId{0};

      MaterialRecord() noexcept = default;

      explicit MaterialRecord(HandleVector<PipelineRecord>::handle_type pipelineHandle, const VkPipeline pipeline,
                              const VkPipelineLayout pipelineLayout, const VariableInfo& varModelViewProj, const VariableInfo& varSdfSmoothing,
                              const bool isSdfBased) noexcept
        : PipelineHandle(pipelineHandle)
        , Pipeline(pipeline)
        , PipelineLayout(pipelineLayout)
        , VarModelViewProj(varModelViewProj)
        , VarSdfSmoothing(varSdfSmoothing)
        , IsSdfBased(isSdfBased)
      {
      }

      bool IsValid() const
      {
        return Pipeline != VK_NULL_HANDLE;
      }
    };

  private:
    struct Resources
    {
      VkDevice Device{VK_NULL_HANDLE};
      // BasicNativeShaderHandle VertexShaderHandle;
      // BasicNativeShaderHandle FragmentShaderHandle;
      // BasicNativeShaderHandle SdfFragmentShaderHandle;
      //  RapidVulkan::ShaderModule VertexShader;
      //  RapidVulkan::ShaderModule FragmentShader;
      //  RapidVulkan::ShaderModule SdfFragmentShader;
      RapidVulkan::PipelineLayout MainPipelineLayout;

      HandleVector<ShaderRecord> Shaders;

      explicit Resources(VkDevice device)
        : Device(device)
      {
      }
    };

    struct DependentResources
    {
      bool IsValid{false};
      VkPipelineCache PipelineCache{VK_NULL_HANDLE};
      VkRenderPass RenderPass{VK_NULL_HANDLE};
      uint32_t Subpass{0};
      PxExtent2D ScreenExtentPx;

      HandleVector<PipelineRecord> Pipelines;
      HandleVector<MaterialRecord> Materials;

      DependentResources() = default;

      explicit DependentResources(const VkPipelineCache pipelineCache, const VkRenderPass renderPass, const uint32_t subpass,
                                  const PxExtent2D& screenExtentPx)
        : IsValid(true)
        , PipelineCache(pipelineCache)
        , RenderPass(renderPass)
        , Subpass(subpass)
        , ScreenExtentPx(screenExtentPx)
      {
      }
    };

    Resources m_resources;
    DependentResources m_dependentResources;

  public:
    NativeGraphicsMaterialFactory(const VUDevice& device, const VkDescriptorSetLayout mainDescriptorSetLayout);
    ~NativeGraphicsMaterialFactory() noexcept;

    static ReadOnlySpan<BasicNativeShaderCreateInfo> GetPredefinedShaders();

    void CreateDependentResources(const VkPipelineCache pipelineCache, const VkRenderPass renderPass, const uint32_t subpass,
                                  const PxExtent2D& screenExtentPx);
    void DestroyDependentResources() noexcept;

    BasicNativeShaderHandle CreateShader(const BasicNativeShaderCreateInfo& createInfo);
    bool DestroyShader(const BasicNativeShaderHandle hShader) noexcept;

    void CreateMaterials(Span<BasicNativeMaterialHandle> dstMaterialHandles, ReadOnlySpan<BasicNativeMaterialCreateInfo> createInfoSpan);
    bool DestroyMaterial(const BasicNativeMaterialHandle hMaterial) noexcept;

    const MaterialRecord& GetMaterial(const BasicNativeMaterialHandle hMaterial) const
    {
      return m_dependentResources.Materials.Get(hMaterial.Value);
    }

    MaterialRecord& GetMaterial(const BasicNativeMaterialHandle hMaterial)
    {
      return m_dependentResources.Materials.Get(hMaterial.Value);
    }

    void ClearCachedCameraChangeIds() noexcept;

  private:
    MaterialRecord CreateMaterial(const BasicNativeMaterialCreateInfo& createInfo);
    uint32_t TryFindPipelineIndex(const BasicNativeMaterialCreateInfo& createInfo);
    PipelineRecord CreatePipeline(const BasicNativeMaterialCreateInfo& createInfo);

    bool UnreferencePipeline(const HandleVector<PipelineRecord>::handle_type hPipeline) noexcept;
  };
}

#endif
