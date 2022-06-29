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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/BasicNativeBeginCustomVulkanFrameInfo.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/BasicNativeDependentCustomVulkanCreateInfo.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsDevice.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsTexture.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/TypeConverter.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBeginFrameInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeDependentCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariable.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariables.hpp>
#include <cassert>

namespace Fsl::Vulkan
{
  namespace
  {
    inline void CmdPushConstants(const VkCommandBuffer hCommandBuffer, const VkPipelineLayout pipelineLayout,
                                 const NativeGraphicsMaterialFactory::VariableInfo& varInfo, const Matrix& matrixMVP)
    {
      assert(varInfo.VariableElement.Usage == BasicMaterialVariableElementUsage::ModelViewProj);
      assert(varInfo.VariableElement.Format == BasicMaterialVariableElementFormat::Matrix4x4);
      assert(varInfo.VariableElement.UsageIndex == 0);

      vkCmdPushConstants(hCommandBuffer, pipelineLayout, varInfo.ShaderStageFlags, varInfo.VariableElement.Offset, varInfo.ByteSize,
                         matrixMVP.DirectAccess());
    }

    inline void CmdPushConstants(const VkCommandBuffer hCommandBuffer, const VkPipelineLayout pipelineLayout,
                                 const BasicMaterialVariables& materialVariables, const NativeGraphicsMaterialFactory::VariableInfo& varInfo)
    {
      BasicMaterialVariable var = materialVariables.GetBasicMaterialVariable(varInfo.VariableElement.Usage, varInfo.VariableElement.UsageIndex);
      vkCmdPushConstants(hCommandBuffer, pipelineLayout, varInfo.ShaderStageFlags, varInfo.VariableElement.Offset, varInfo.ByteSize, var.Data);
    }
  }

  NativeGraphicsDevice::NativeGraphicsDevice(const VUDevice& device, const VkQueue queue, const uint32_t queueFamilyIndex,
                                             const std::shared_ptr<VMBufferManager>& bufferManager,
                                             const std::shared_ptr<VulkanImageCreator>& imageCreator, const uint32_t maxFramesInFlight)
    : m_bufferFactory(bufferManager)
    , m_textureFactory(device, imageCreator, maxFramesInFlight)
    , m_materialFactory(device, m_textureFactory.GetMainDescriptorSetLayout())
  {
    FSL_PARAM_NOT_USED(queue);
    FSL_PARAM_NOT_USED(queueFamilyIndex);
    FSLLOG3_VERBOSE("NativeGraphicsDevice: Construct");
  }

  NativeGraphicsDevice::~NativeGraphicsDevice() noexcept
  {
    FSLLOG3_VERBOSE("NativeGraphicsDevice: Destruct");
    Dispose();
  }


  void NativeGraphicsDevice::Dispose() noexcept
  {
    if (m_isDisposed)
    {
      return;
    }
    FSLLOG3_VERBOSE("NativeGraphicsDevice: Dispose");
    m_isDisposed = true;
    m_textureFactory.Dispose();
    m_bufferFactory.Dispose();
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  ReadOnlySpan<BasicNativeShaderCreateInfo> NativeGraphicsDevice::GetPredefinedShaders() const
  {
    return Fsl::Vulkan::NativeGraphicsMaterialFactory::GetPredefinedShaders();
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  VUTextureInfo NativeGraphicsDevice::TryGetTextureInfo(const BasicNativeTextureHandle hTexture) const
  {
    return m_textureFactory.TryGetTextureInfo(hTexture);
  }


  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Graphics3D::INativeBufferFactory
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  Graphics3D::NativeBufferFactoryCaps NativeGraphicsDevice::GetBufferCaps() const
  {
    // The buffer factory handles the disposed case
    return m_bufferFactory.GetBufferCaps();
  }

  BasicNativeBufferHandle NativeGraphicsDevice::CreateBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData,
                                                             const uint32_t bufferElementCapacity, const bool isDynamic)
  {
    // The buffer factory handles the disposed case
    return m_bufferFactory.CreateBuffer(bufferType, bufferData, bufferElementCapacity, isDynamic);
  }

  bool NativeGraphicsDevice::DestroyBuffer(const BasicNativeBufferHandle hBuffer)
  {
    // The buffer factory handles the disposed case
    return m_bufferFactory.DestroyBuffer(hBuffer);
  }

  void NativeGraphicsDevice::SetBufferData(const BasicNativeBufferHandle hBuffer, const uint32_t dstIndex, ReadOnlyFlexSpan bufferData)
  {
    m_bufferFactory.SetBufferData(hBuffer, dstIndex, bufferData);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Graphics3D::INativeShaderFactory
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  BasicNativeShaderHandle NativeGraphicsDevice::CreateShader(const BasicNativeShaderCreateInfo& createInfo)
  {
    return m_materialFactory.CreateShader(createInfo);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  bool NativeGraphicsDevice::DestroyShader(const BasicNativeShaderHandle hShader) noexcept
  {
    return m_materialFactory.DestroyShader(hShader);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Graphics3D::INativeMaterialFactory
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void NativeGraphicsDevice::CreateMaterials(Span<BasicNativeMaterialHandle> dstMaterialHandles,
                                             ReadOnlySpan<BasicNativeMaterialCreateInfo> createInfoSpan)
  {
    // The factory handles the disposed case
    m_materialFactory.CreateMaterials(dstMaterialHandles, createInfoSpan);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  bool NativeGraphicsDevice::DestroyMaterial(const BasicNativeMaterialHandle hMaterial)
  {
    // The factory handles the disposed case
    return m_materialFactory.DestroyMaterial(hMaterial);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Graphics3D::INativeTextureFactory
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  Graphics3D::NativeTextureFactoryCaps NativeGraphicsDevice::GetTextureCaps() const
  {
    // The texture factory handles the disposed case
    return m_textureFactory.GetTextureCaps();
  }

  BasicNativeTextureHandle NativeGraphicsDevice::CreateTexture(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                               const TextureFlags textureFlags, const bool isDynamic)
  {
    // The texture factory handles the disposed case
    return m_textureFactory.CreateTexture(texture, filterHint, textureFlags, isDynamic);
  }

  bool NativeGraphicsDevice::DestroyTexture(const BasicNativeTextureHandle hTexture)
  {
    // The texture factory handles the disposed case
    return m_textureFactory.DestroyTexture(hTexture);
  }


  void NativeGraphicsDevice::SetTextureData(const BasicNativeTextureHandle hTexture, const RawTexture& texture, const Texture2DFilterHint filterHint,
                                            const TextureFlags textureFlags)
  {
    // The texture factory handles the disposed case
    return m_textureFactory.SetTextureData(hTexture, texture, filterHint, textureFlags);
  }


  const IBasicNativeTexture* NativeGraphicsDevice::TryGetTexture(const BasicNativeTextureHandle hTexture) const
  {
    // The texture factory handles the disposed case
    return m_textureFactory.TryGetTexture(hTexture);
  }


  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Graphics3D::INativeDevice
  // -------------------------------------------------------------------------------------------------------------------------------------------------


  void NativeGraphicsDevice::CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo)
  {
    const auto* const pCustomInfo = dynamic_cast<const BasicNativeDependentCustomVulkanCreateInfo*>(createInfo.pCustomCreateInfo);
    if (pCustomInfo == nullptr)
    {
      throw std::invalid_argument("frameInfo.pCustomInfo not of the expected type or null");
    }

    m_materialFactory.CreateDependentResources(pCustomInfo->PipelineCache, pCustomInfo->MainRenderPass, pCustomInfo->SubpassSystemUI,
                                               createInfo.ExtentPx);
  }


  void NativeGraphicsDevice::DestroyDependentResources()
  {
    m_materialFactory.DestroyDependentResources();
  }


  void NativeGraphicsDevice::BeginFrame(const BasicNativeBeginFrameInfo& frameInfo)
  {
    const auto* const pCustomInfo = dynamic_cast<const BasicNativeBeginCustomVulkanFrameInfo*>(frameInfo.pCustomInfo);
    if (pCustomInfo == nullptr)
    {
      throw std::invalid_argument("frameInfo.pCustomInfo not of the expected type or null");
    }
    if (pCustomInfo->CommandBuffer == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("Command buffer can not be invalid");
    }
    assert(!m_frame.IsValid());
    m_frame = Frame(pCustomInfo->CommandBuffer);
  }


  void NativeGraphicsDevice::EndFrame()
  {
    FSLLOG3_ERROR_IF(!m_frame.IsValid(), "Ending a frame that wasnt begun");
    m_frame = {};
  }


  void NativeGraphicsDevice::BeginCmds()
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid());
    if (m_frame.IsValid())
    {
      //! @brief if this fires then the previous begin was not terminated correctly
      assert(!m_frame.Commands.IsActive);
      m_frame.Commands = CommandSection(true);
      m_materialFactory.ClearCachedCameraChangeIds();
    }
  }


  void NativeGraphicsDevice::EndCmds()
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid());
    if (m_frame.Commands.IsActive)
    {
      m_frame.Commands = {};
    }
  }


  void NativeGraphicsDevice::CmdSetCamera(const BasicCameraInfo& cameraInfo)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid());
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsActive);

    m_frame.Commands.CameraInfo = ExtendedCameraInfo(cameraInfo);
    ++m_frame.Commands.CameraChangeId;

    // Setup the model view projection matrix for the currently bound material program
    if (m_frame.Commands.BoundMaterialHandle.IsValid())
    {
      NativeGraphicsMaterialFactory::MaterialRecord& rRecord = m_materialFactory.GetMaterial(m_frame.Commands.BoundMaterialHandle);
      if (rRecord.CameraChangeId != m_frame.Commands.CameraChangeId)
      {
        rRecord.CameraChangeId = m_frame.Commands.CameraChangeId;
        CmdPushConstants(m_frame.CommandBuffer, m_frame.Commands.BoundPipelineLayout, rRecord.VarModelViewProj,
                         m_frame.Commands.CameraInfo.ModelViewProjection);
      }
    }
  }


  void NativeGraphicsDevice::CmdBindIndexBuffer(const BasicNativeBufferHandle indexBuffer)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid());
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsActive);

    const VMBuffer& buffer = m_bufferFactory.GetBuffer(indexBuffer);

    m_frame.Commands.BoundIndexBufferHandle = indexBuffer;

    constexpr VkDeviceSize offset = 0u;
    vkCmdBindIndexBuffer(m_frame.CommandBuffer, buffer.GetBuffer(), offset, VK_INDEX_TYPE_UINT16);
  }


  void NativeGraphicsDevice::CmdBindMaterial(const BasicNativeMaterialHandle material, const BasicMaterialVariables& materialVariables,
                                             const ReadOnlySpan<BasicNativeTextureHandle> textures)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid());
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsActive);

    if (textures.size() != 1u)
    {
      throw NotSupportedException("We expect to get one texture");
    }


    NativeGraphicsMaterialFactory::MaterialRecord& rRecord = m_materialFactory.GetMaterial(material);
    assert(rRecord.IsValid());

    m_frame.Commands.BoundMaterialHandle = material;
    m_frame.Commands.BoundPipelineLayout = rRecord.PipelineLayout;

    vkCmdBindPipeline(m_frame.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rRecord.Pipeline);

    const VkDescriptorSet descriptorSet = m_textureFactory.GetDescriptorSet(textures[0]);
    vkCmdBindDescriptorSets(m_frame.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_frame.Commands.BoundPipelineLayout, 0, 1, &descriptorSet, 0,
                            nullptr);

    // Push the materials push constants
    if (m_frame.Commands.CameraInfo.IsValid)
    {
      // This can be optimized even further as we know all materials use the same layout
      if (rRecord.CameraChangeId != m_frame.Commands.CameraChangeId)
      {
        rRecord.CameraChangeId = m_frame.Commands.CameraChangeId;
        CmdPushConstants(m_frame.CommandBuffer, m_frame.Commands.BoundPipelineLayout, rRecord.VarModelViewProj,
                         m_frame.Commands.CameraInfo.ModelViewProjection);
      }
    }
    if (rRecord.IsSdfBased)
    {
      CmdPushConstants(m_frame.CommandBuffer, m_frame.Commands.BoundPipelineLayout, materialVariables, rRecord.VarSdfSmoothing);
    }
  }


  void NativeGraphicsDevice::CmdBindVertexBuffer(const BasicNativeBufferHandle vertexBuffer)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid());
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsActive);

    const VMBuffer& buffer = m_bufferFactory.GetBuffer(vertexBuffer);

    m_frame.Commands.BoundVertexBufferHandle = vertexBuffer;

    const VkDeviceSize offset = 0u;
    vkCmdBindVertexBuffers(m_frame.CommandBuffer, 0, 1, buffer.GetBufferPointer(), &offset);
  }


  void NativeGraphicsDevice::CmdDraw(const uint32_t vertexCount, const uint32_t firstVertex)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid());
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsActive);
    // If this fires no material was bound
    assert(m_frame.Commands.BoundMaterialHandle.IsValid());
    // If this fires no vertex buffer was bound
    assert(m_frame.Commands.BoundVertexBufferHandle.IsValid());
    // If this fires the camera was not set
    assert(m_frame.Commands.CameraInfo.IsValid);

    vkCmdDraw(m_frame.CommandBuffer, vertexCount, 1, firstVertex, 0);
  }

  void NativeGraphicsDevice::CmdDrawIndexed(const uint32_t indexCount, const uint32_t firstIndex)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid());
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsActive);
    // If this fires no material was bound
    assert(m_frame.Commands.BoundMaterialHandle.IsValid());
    // If this fires no vertex buffer was bound
    assert(m_frame.Commands.BoundVertexBufferHandle.IsValid());
    // If this fires no index buffer was bound
    assert(m_frame.Commands.BoundIndexBufferHandle.IsValid());
    // If this fires the camera was not set
    assert(m_frame.Commands.CameraInfo.IsValid);

    constexpr int32_t vertexOffset = 0;
    vkCmdDrawIndexed(m_frame.CommandBuffer, indexCount, 1, firstIndex, vertexOffset, 0);
  }
}
