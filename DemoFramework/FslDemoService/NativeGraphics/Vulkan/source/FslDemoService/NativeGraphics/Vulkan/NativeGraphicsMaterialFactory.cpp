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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsDeviceShaders.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsMaterialFactory.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/TypeConverter.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslUtil/Vulkan1_0/Batch/ConfigHelper.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <utility>

namespace Fsl::Vulkan
{
  namespace
  {
    namespace LocalConfig::PushConstant
    {
      constexpr uint32_t SizeMatrix = 4 * 4 * 4;
      constexpr uint32_t SizeSmooth = 4;
      // constexpr uint32_t Size = SizeMatrix + SizeSmooth;

      constexpr uint32_t OffsetMatrix = 0;
      constexpr uint32_t OffsetSdfSmooth = SizeMatrix;

      constexpr NativeGraphicsMaterialFactory::VariableInfo
        ModelViewProj(BasicMaterialVariableElement(OffsetMatrix, BasicMaterialVariableElementFormat::Matrix4x4,
                                                   BasicMaterialVariableElementUsage::ModelViewProj, 0),
                      SizeMatrix, VK_SHADER_STAGE_VERTEX_BIT);

      constexpr NativeGraphicsMaterialFactory::VariableInfo SdfSmoothing(
        BasicMaterialVariableElement(OffsetSdfSmooth, BasicMaterialVariableElementFormat::Float1, BasicMaterialVariableElementUsage::SdfSmooth, 0),
        SizeSmooth, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    //! Set up the predefined shaders
    std::array<BasicNativeShaderCreateInfo, 3> g_predefindShaders = {
      // PredefinedShaderType::Vertex
      BasicNativeShaderCreateInfo(BasicShaderStageFlag::Vertex, NativeGraphicsDeviceShaders::GetVertexShader(),
                                  NativeGraphicsDeviceShaders::VertexShaderVertexDecl.AsReadOnlySpan()),
      // PredefinedShaderType::Fragment
      BasicNativeShaderCreateInfo(BasicShaderStageFlag::Fragment, NativeGraphicsDeviceShaders::GetFragmentShader(), VertexAttributeDescriptionSpan()),
      // PredefinedShaderType::FragmentSdf
      BasicNativeShaderCreateInfo(BasicShaderStageFlag::Fragment, NativeGraphicsDeviceShaders::GetSdfFragmentShader(),
                                  VertexAttributeDescriptionSpan())};


    RapidVulkan::PipelineLayout CreatePipelineLayout(const VkDevice device, const VkDescriptorSetLayout& descriptorSetLayoutTexture)
    {
      std::array<VkPushConstantRange, 2> pushConstantRange{};

      constexpr auto pushConstModelViewProj = LocalConfig::PushConstant::ModelViewProj;
      constexpr auto pushConstSdfSmooth = LocalConfig::PushConstant::SdfSmoothing;

      pushConstantRange[0].stageFlags = pushConstModelViewProj.ShaderStageFlags;
      pushConstantRange[0].offset = pushConstModelViewProj.VariableElement.Offset;
      pushConstantRange[0].size = pushConstModelViewProj.ByteSize;
      pushConstantRange[1].stageFlags = pushConstSdfSmooth.ShaderStageFlags;
      pushConstantRange[1].offset = pushConstSdfSmooth.VariableElement.Offset;
      pushConstantRange[1].size = pushConstSdfSmooth.ByteSize;

      const std::array<VkDescriptorSetLayout, 1> layouts = {descriptorSetLayoutTexture};

      VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.setLayoutCount = UncheckedNumericCast<uint32_t>(layouts.size());
      pipelineLayoutInfo.pSetLayouts = layouts.data();
      pipelineLayoutInfo.pushConstantRangeCount = UncheckedNumericCast<uint32_t>(pushConstantRange.size());
      pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data();

      return {device, pipelineLayoutInfo};
    }

    void CheckIfMaterialDeclarationIsSupported(const BlendState blendState, const BasicMaterialVariableDeclarationSpan& materialDeclaration)
    {
      switch (blendState)
      {
      case BlendState::Additive:
      case BlendState::AlphaBlend:
      case BlendState::NonPremultiplied:
      case BlendState::Opaque:
        {
          // auto index = materialDeclaration.MaterialElementIndexOf(BasicMaterialVariableElementUsage::ModelViewProj, 0);
          // if (index < 0)
          //{
          //  throw NotSupportedException("materialDeclaration is not compatible");
          //}
          // if (materialDeclaration.At(index).Format != BasicMaterialVariableElementFormat::Matrix4x4)
          //{
          //  throw NotSupportedException("ModelViewProj format is unsupported");
          //}
          break;
        }
      case BlendState::Sdf:
        {
          // auto index = materialDeclaration.MaterialElementIndexOf(BasicMaterialVariableElementUsage::ModelViewProj, 0);
          // if (index < 0)
          //{
          //  throw NotSupportedException("materialDeclaration is not compatible");
          //}
          // if (materialDeclaration.At(index).Format != BasicMaterialVariableElementFormat::Matrix4x4)
          //{
          //  throw NotSupportedException("ModelViewProj format is unsupported");
          //}
          auto index = materialDeclaration.MaterialElementIndexOf(BasicMaterialVariableElementUsage::SdfSmooth, 0);
          if (index < 0)
          {
            throw NotSupportedException("materialDeclaration is not compatible");
          }
          if (materialDeclaration.At(index).Format != BasicMaterialVariableElementFormat::Float1)
          {
            throw NotSupportedException("SdfSmooth format is unsupported");
          }
          break;
        }
      default:
        throw NotSupportedException("Unsupported BlendState");
      }
    }

    RapidVulkan::GraphicsPipeline CreateGraphicsPipeline(const VkDevice device, const VkShaderModule vertexShader,
                                                         const VkShaderModule fragmentShader, const VkPipelineLayout pipelineLayout,
                                                         const VkPipelineCache pipelineCache, const VkRenderPass renderPass, const uint32_t subpass,
                                                         const PxExtent2D& screenExtentPx, const BasicMaterialInfo& materialInfo,
                                                         const VertexDeclarationSpan& vertexDeclaration)
    {
      assert(device != VK_NULL_HANDLE);
      assert(vertexShader != VK_NULL_HANDLE);
      assert(fragmentShader != VK_NULL_HANDLE);
      assert(pipelineLayout != VK_NULL_HANDLE);
      // assert(pipelineCache != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);
      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};
      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].flags = 0;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertexShader;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].flags = 0;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragmentShader;
      pipelineShaderStageCreateInfo[1].pName = "main";
      pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

      VkVertexInputBindingDescription vertexBindings{};
      vertexBindings.binding = 0;
      vertexBindings.stride = vertexDeclaration.VertexStride();
      vertexBindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      std::array<VkVertexInputAttributeDescription, 16> nativeVertexAttributes{};
      auto nativeVertexAttributeSpan = SpanUtil::AsSpan(nativeVertexAttributes).subspan(0, vertexDeclaration.size());
      ConfigHelper::ExtractVertexAttributes(nativeVertexAttributeSpan, vertexDeclaration);

      VkPipelineVertexInputStateCreateInfo vertexInputState{};
      vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertexInputState.vertexBindingDescriptionCount = 1;
      vertexInputState.pVertexBindingDescriptions = &vertexBindings;
      vertexInputState.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(nativeVertexAttributeSpan.size());
      vertexInputState.pVertexAttributeDescriptions = nativeVertexAttributeSpan.data();

      VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
      inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

      VkViewport viewport{};
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      switch (materialInfo.ViewportMode)
      {
      case BasicViewportMode::CustomOriginTopLeft:
        viewport.x = static_cast<float>(materialInfo.Viewport.X);
        viewport.y = static_cast<float>(materialInfo.Viewport.Y);
        viewport.width = static_cast<float>(materialInfo.Viewport.Width);
        viewport.height = static_cast<float>(materialInfo.Viewport.Height);
        break;
      case BasicViewportMode::CustomOriginBottomLeft:
        {
          const int32_t newY = UncheckedNumericCast<int32_t>(screenExtentPx.Height) - materialInfo.Viewport.Bottom();
          viewport.x = static_cast<float>(materialInfo.Viewport.X);
          viewport.y = static_cast<float>(newY);
          viewport.width = static_cast<float>(materialInfo.Viewport.Width);
          viewport.height = static_cast<float>(materialInfo.Viewport.Height);
          break;
        }
      case BasicViewportMode::Fullscreen:
        viewport.width = static_cast<float>(screenExtentPx.Width);
        viewport.height = static_cast<float>(screenExtentPx.Height);
        break;
      default:
        throw NotSupportedException("Unsupported viewport mode");
      }

      VkRect2D scissor{};
      scissor.extent = TypeConverter::UncheckedTo<VkExtent2D>(screenExtentPx);

      VkPipelineViewportStateCreateInfo viewportState{};
      viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.pViewports = &viewport;
      viewportState.scissorCount = 1;
      viewportState.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo rasterizationState{};
      rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
      rasterizationState.cullMode = TypeConverter::ChangeTo<VkCullModeFlags>(materialInfo.CullMode);
      rasterizationState.frontFace = TypeConverter::ChangeTo<VkFrontFace>(materialInfo.CullFrontFace);
      rasterizationState.lineWidth = 1.0f;

      VkPipelineMultisampleStateCreateInfo multisampleState{};
      multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

      VkPipelineDepthStencilStateCreateInfo depthStencilState{};
      depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilState.depthTestEnable = materialInfo.Depth.TestEnable ? VK_TRUE : VK_FALSE;
      depthStencilState.depthWriteEnable = materialInfo.Depth.WriteEnable ? VK_TRUE : VK_FALSE;
      depthStencilState.depthCompareOp = TypeConverter::ChangeTo<VkCompareOp>(materialInfo.Depth.CompareOp);

      auto blendAttachmentState = ConfigHelper::CreatePipelineColorBlendAttachmentState(materialInfo.Blend);

      VkPipelineColorBlendStateCreateInfo colorBlendState{};
      colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlendState.attachmentCount = 1;
      colorBlendState.pAttachments = &blendAttachmentState;

      VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
      pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(pipelineShaderStageCreateInfo.size());
      pipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
      pipelineCreateInfo.pVertexInputState = &vertexInputState;
      pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
      pipelineCreateInfo.pViewportState = &viewportState;
      pipelineCreateInfo.pRasterizationState = &rasterizationState;
      pipelineCreateInfo.pMultisampleState = &multisampleState;
      pipelineCreateInfo.pDepthStencilState = &depthStencilState;
      pipelineCreateInfo.pColorBlendState = &colorBlendState;
      pipelineCreateInfo.pDynamicState = nullptr;
      pipelineCreateInfo.layout = pipelineLayout;
      pipelineCreateInfo.renderPass = renderPass;
      pipelineCreateInfo.subpass = subpass;

      return {device, pipelineCache, pipelineCreateInfo};
    }
  }


  NativeGraphicsMaterialFactory::NativeGraphicsMaterialFactory(const VUDevice& device, const VkDescriptorSetLayout mainDescriptorSetLayout)
    : m_resources(device.Get())
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::Construct");
    assert(mainDescriptorSetLayout != VK_NULL_HANDLE);
    m_resources.MainPipelineLayout = CreatePipelineLayout(device.Get(), mainDescriptorSetLayout);
  }


  NativeGraphicsMaterialFactory::~NativeGraphicsMaterialFactory() noexcept
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::Destruct");
    DestroyDependentResources();
    FSLLOG3_WARNING_IF(!m_resources.Shaders.Empty(), "There are still {} shaders active, force freeing them", m_resources.Shaders.Count());
  }


  ReadOnlySpan<BasicNativeShaderCreateInfo> NativeGraphicsMaterialFactory::GetPredefinedShaders()
  {
    return ReadOnlySpanUtil::AsSpan(g_predefindShaders);
  }


  void NativeGraphicsMaterialFactory::CreateDependentResources(const VkPipelineCache pipelineCache, const VkRenderPass renderPass,
                                                               const uint32_t subpass, const PxExtent2D& screenExtentPx)
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::CreateDependentResources");
    try
    {
      m_dependentResources = DependentResources(pipelineCache, renderPass, subpass, screenExtentPx);
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("CreateDependentResources failed with: {}", ex.what());
      DestroyDependentResources();
      throw;
    }
  }


  void NativeGraphicsMaterialFactory::DestroyDependentResources() noexcept
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::DestroyDependentResources");
    FSLLOG3_WARNING_IF(!m_dependentResources.Materials.Empty(), "There are still {} materials active, force freeing them",
                       m_dependentResources.Materials.Count());
    FSLLOG3_WARNING_IF(!m_dependentResources.Pipelines.Empty(), "There are still {} pipelines active, force freeing them",
                       m_dependentResources.Pipelines.Count());
    m_dependentResources = {};
  }


  void NativeGraphicsMaterialFactory::ClearCachedCameraChangeIds() noexcept
  {
    const uint32_t count = m_dependentResources.Materials.Count();
    for (uint32_t i = 0; i < count; ++i)
    {
      m_dependentResources.Materials[i].CameraChangeId = 0;
    }
  }


  BasicNativeShaderHandle NativeGraphicsMaterialFactory::CreateShader(const BasicNativeShaderCreateInfo& createInfo)
  {
    if (!createInfo.IsValid())
    {
      throw std::invalid_argument("createInfo must be valid");
    }

    const int32_t handleValue = m_resources.Shaders.Add(ShaderRecord(
      createInfo.Flag,
      RapidVulkan::ShaderModule(m_resources.Device, 0, createInfo.Shader.byte_size(), reinterpret_cast<const uint32_t*>(createInfo.Shader.data())),
      VertexAttributeDescriptions(createInfo.VertexAttributeDescSpan)));

    return BasicNativeShaderHandle(handleValue);
  }


  bool NativeGraphicsMaterialFactory::DestroyShader(const BasicNativeShaderHandle hShader) noexcept
  {
    return m_resources.Shaders.Remove(hShader.Value);
  }

  void NativeGraphicsMaterialFactory::CreateMaterials(Span<BasicNativeMaterialHandle> dstMaterialHandles,
                                                      ReadOnlySpan<BasicNativeMaterialCreateInfo> createInfoSpan)
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::CreateMaterials");
    if (!m_dependentResources.IsValid)
    {
      throw UsageErrorException("Cant CreateMaterials in the current state");
    }

    try
    {
      // Ensure that we have enough capacity
      m_dependentResources.Materials.Reserve(UncheckedNumericCast<uint32_t>(m_dependentResources.Materials.Count() + createInfoSpan.size()));

      for (uint32_t i = 0; i < createInfoSpan.size(); ++i)
      {
        assert(!dstMaterialHandles[i].IsValid());
        int32_t handle = m_dependentResources.Materials.Add(CreateMaterial(createInfoSpan[i]));
        dstMaterialHandles[i] = BasicNativeMaterialHandle(handle);
      }
    }
    catch (const std::exception&)
    {
      // Clear all temporarily written handles
      for (uint32_t i = 0; i < dstMaterialHandles.size(); ++i)
      {
        if (dstMaterialHandles[i].IsValid())
        {
          m_dependentResources.Materials.Remove(dstMaterialHandles[i].Value);
          dstMaterialHandles[i] = {};
        }
      }
      throw;
    }
  }

  bool NativeGraphicsMaterialFactory::DestroyMaterial(const BasicNativeMaterialHandle hMaterial) noexcept
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::DestroyPipeline({})", hMaterial.Value);
    if (!m_dependentResources.IsValid)
    {
      FSLLOG3_ERROR("Can't destroy a material in this state");
      return false;
    }
    const MaterialRecord* pMaterial = m_dependentResources.Materials.TryGet(hMaterial.Value);
    if (pMaterial == nullptr)
    {
      return false;
    }

    [[maybe_unused]] bool unreferenced = UnreferencePipeline(pMaterial->PipelineHandle);
    assert(unreferenced);
    return m_dependentResources.Materials.Remove(hMaterial.Value);
  }


  NativeGraphicsMaterialFactory::MaterialRecord NativeGraphicsMaterialFactory::CreateMaterial(const BasicNativeMaterialCreateInfo& createInfo)
  {
    assert(createInfo.IsValid());

    CheckIfMaterialDeclarationIsSupported(createInfo.MaterialInfo.Blend, createInfo.MaterialDeclaration);

    uint32_t index = TryFindPipelineIndex(createInfo);
    if (index >= m_dependentResources.Pipelines.Count())
    {
      m_dependentResources.Pipelines.Add(CreatePipeline(createInfo));
    }
    assert(index < m_dependentResources.Pipelines.Count());

    auto hPipeline = m_dependentResources.Pipelines.FastIndexToHandle(index);

    // Reference pipeline
    PipelineRecord& rRecord = m_dependentResources.Pipelines[index];
    ++rRecord.ReferenceCount;

    return MaterialRecord(hPipeline, rRecord.Pipeline.Get(), rRecord.PipelineLayout, LocalConfig::PushConstant::ModelViewProj,
                          LocalConfig::PushConstant::SdfSmoothing, createInfo.MaterialInfo.Blend == BlendState::Sdf);
  }


  uint32_t NativeGraphicsMaterialFactory::TryFindPipelineIndex(const BasicNativeMaterialCreateInfo& createInfo)
  {
    const uint32_t count = m_dependentResources.Pipelines.Count();
    for (uint32_t i = 0; i < count; ++i)
    {
      const PipelineRecord& record = m_dependentResources.Pipelines[i];
      if (createInfo.IsCompatible(record.MaterialInfo, record.VertexDecl.AsSpan(), record.VertexShaderHandle, record.FragmentShaderHandle))
      {
        return i;
      }
    }
    return m_dependentResources.Pipelines.Count();
  }


  NativeGraphicsMaterialFactory::PipelineRecord NativeGraphicsMaterialFactory::CreatePipeline(const BasicNativeMaterialCreateInfo& createInfo)
  {
    const ShaderRecord& vertRecord = m_resources.Shaders.Get(createInfo.VertexShaderHandle.Value);
    const ShaderRecord& fragRecord = m_resources.Shaders.Get(createInfo.FragmentShaderHandle.Value);
    const VkShaderModule hVert = vertRecord.Shader.Get();
    const VkShaderModule hFrag = fragRecord.Shader.Get();

    return PipelineRecord(CreateGraphicsPipeline(m_resources.Device, hVert, hFrag, m_resources.MainPipelineLayout.Get(),
                                                 m_dependentResources.PipelineCache, m_dependentResources.RenderPass, m_dependentResources.Subpass,
                                                 m_dependentResources.ScreenExtentPx, createInfo.MaterialInfo, createInfo.VertexDeclaration),
                          m_resources.MainPipelineLayout.Get(), createInfo.MaterialInfo, createInfo.VertexDeclaration, createInfo.VertexShaderHandle,
                          createInfo.FragmentShaderHandle);
  }


  bool NativeGraphicsMaterialFactory::UnreferencePipeline(const HandleVector<PipelineRecord>::handle_type hPipeline) noexcept
  {
    PipelineRecord* rPipelineRecord = m_dependentResources.Pipelines.TryGet(hPipeline);
    bool found = rPipelineRecord != nullptr;
    if (found)
    {
      assert(rPipelineRecord->ReferenceCount > 0u);
      if (rPipelineRecord->ReferenceCount > 1)
      {
        --rPipelineRecord->ReferenceCount;
      }
      else
      {
        m_dependentResources.Pipelines.Remove(hPipeline);
      }
    }
    return found;
  }
}
