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
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsDevice.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBeginCustomFrameInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeDependentCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElementFormatUtil.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariables.hpp>
#include <cassert>

namespace Fsl::GLES2
{
  namespace
  {
    void SetUniform(GLint loc, const BasicMaterialVariable& matVariable)
    {
      if (loc >= 0)
      {
        switch (matVariable.ElementFormat)
        {
        case BasicMaterialVariableElementFormat::Float1:
          static_assert(BasicMaterialVariableElementFormatUtil::GetBytesPerElement(BasicMaterialVariableElementFormat::Float1) == sizeof(GLfloat),
                        "size expectations does not match");
          glUniform1fv(loc, 1, static_cast<const GLfloat*>(matVariable.Data));
          break;
        case BasicMaterialVariableElementFormat::Matrix4x4:
          static_assert(BasicMaterialVariableElementFormatUtil::GetBytesPerElement(BasicMaterialVariableElementFormat::Matrix4x4) ==
                          (sizeof(GLfloat) * 4 * 4),
                        "size expectations does not match");
          glUniformMatrix4fv(loc, 1, 0, static_cast<const GLfloat*>(matVariable.Data));
          break;
        default:
          throw NotSupportedException("BasicMaterialVariable element format is unsupported");
        }
      }
    }
  }

  // NOTE: The binding of vertex buffer attribs can be done a lot smarter.
  //       It would be better if we just applied the 'diff' between the last used program requirements and the new one.
  NativeGraphicsDevice::NativeGraphicsDevice()
  {
    FSLLOG3_VERBOSE("NativeGraphicsDevice::Construct");
  }

  NativeGraphicsDevice::~NativeGraphicsDevice() noexcept
  {
    FSLLOG3_VERBOSE("NativeGraphicsDevice::Destruct");
    Dispose();
  }


  void NativeGraphicsDevice::Dispose() noexcept
  {
    if (m_isDisposed)
    {
      return;
    }
    FSLLOG3_VERBOSE("NativeGraphicsDevice::Dispose");
    m_isDisposed = true;
    m_textureFactory.Dispose();
    m_bufferFactory.Dispose();
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  GLTextureInfo NativeGraphicsDevice::TryGetTextureInfo(const BasicNativeTextureHandle hTexture) const
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


  ReadOnlySpan<BasicNativeShaderCreateInfo> NativeGraphicsDevice::GetPredefinedShaders() const
  {
    return m_materialFactory.GetPredefinedShaders();
  }


  BasicNativeShaderHandle NativeGraphicsDevice::CreateShader(const BasicNativeShaderCreateInfo& createInfo)
  {
    return m_materialFactory.CreateShader(createInfo);
  }

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
    m_materialFactory.CreateDependentResources(createInfo.ExtentPx);
    m_dependentResources = DependentResources(TypeConverter::To<PxSize2D>(createInfo.ExtentPx));
  }


  void NativeGraphicsDevice::DestroyDependentResources()
  {
    m_materialFactory.DestroyDependentResources();
    m_dependentResources = {};
  }


  void NativeGraphicsDevice::BeginFrame(const BasicNativeBeginFrameInfo& /*frameInfo*/)
  {
    assert(!m_frame.IsValid);
    m_frame = Frame(true);
  }


  void NativeGraphicsDevice::EndFrame()
  {
    FSLLOG3_ERROR_IF(!m_frame.IsValid, "Ending a frame that wasnt begun");
    m_frame = {};
  }


  void NativeGraphicsDevice::BeginCache()
  {
    assert(m_frame.IsValid);
    //! If this fires BeginCache was called twice in a row
    assert(!m_frame.Cache.IsValid);
    //! If this fires BeginCache was called during a BeginCmds section
    assert(!m_frame.Commands.IsValid);
    if (m_frame.IsValid)
    {
      m_frame.Cache = CacheRecord(true);
      m_frame.Cache.SavedState.DisableDepthTest();
      m_frame.Cache.SavedState.DisableScissor();
      m_frame.Cache.SavedState.ActiveTexture0();
    }
  }

  void NativeGraphicsDevice::EndCache()
  {
    assert(m_frame.IsValid);
    //! If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    //! If this fires BeginCache was called during a BeginCmds section
    assert(!m_frame.Commands.IsValid);
    m_frame.Cache = {};
  }


  void NativeGraphicsDevice::BeginCmds()
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid);
    // If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    if (m_frame.IsValid)
    {
      //! @brief if this fires then the previous begin was not terminated correctly
      assert(!m_frame.Commands.IsValid);
      m_frame.Commands = CommandsRecord(true);
      m_materialFactory.ClearCachedCameraChangeIds();
    }
  }


  void NativeGraphicsDevice::EndCmds()
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid);
    // If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    if (m_frame.IsValid)
    {
      // DisableAttribArrays();
      m_frame.Commands = {};    // This restores the old GL settings!!
    }
  }


  void NativeGraphicsDevice::CmdSetCamera(const BasicCameraInfo& cameraInfo)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid);
    // If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsValid);

    m_frame.Commands.SetCameraInfo(ExtendedCameraInfo(cameraInfo));

    // Setup the model view projection matrix for the currently bound material program
    if (m_frame.Commands.MaterialHandle.IsValid())
    {
      // This only executes updates anything if the camera was modified
      m_materialFactory.SetMaterialCameraInfo(m_frame.Commands.MaterialHandle, m_frame.Commands.CameraChangeId, m_frame.Commands.CameraInfo);
    }
  }


  void NativeGraphicsDevice::CmdBindIndexBuffer(const BasicNativeBufferHandle indexBuffer)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid);
    // If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsValid);

    if (indexBuffer.IsValid())
    {
      const NativeGraphicsBufferRecord& record = m_bufferFactory.GetBuffer(indexBuffer);
      assert(record.GetBuffer().GetTarget() == GL_ELEMENT_ARRAY_BUFFER);
      m_frame.Cache.SavedState.BindIndexBuffer(record.GetBuffer().Get());
    }
    else
    {
      m_frame.Cache.SavedState.BindIndexBuffer(0);
    }
    m_frame.Commands.IndexBufferHandle = indexBuffer;
  }


  void NativeGraphicsDevice::CmdBindMaterial(const BasicNativeMaterialHandle material, const BasicMaterialVariables& materialVariables,
                                             const ReadOnlySpan<BasicNativeTextureHandle> textures)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid);
    // If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsValid);

    if (textures.size() != 1u)
    {
      throw NotSupportedException("textures.size() is expected to be 1");
    }

    // DisableAttribArrays();

    const NativeGraphicsMaterialFactory::MaterialRecord& record = m_materialFactory.GetMaterial(material);
    assert(record.IsValid());
    m_frame.Cache.SavedState.UseProgram(record.CachedProgramHandle);

    auto textureInfo = m_textureFactory.TryGetTextureInfo(textures[0]);
    assert(textureInfo.IsValid());
    m_frame.Cache.SavedState.BindTexture(textureInfo.Handle);
    m_frame.Commands.MaterialHandle = material;

    switch (record.MaterialInfo.Blend)
    {
    case BlendState::Additive:
      m_frame.Cache.SavedState.EnableBlend();
      m_frame.Cache.SavedState.BlendFunc(GL_ONE, GL_ONE);
      break;
    case BlendState::AlphaBlend:
      m_frame.Cache.SavedState.EnableBlend();
      m_frame.Cache.SavedState.BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      break;
    case BlendState::NonPremultiplied:
      m_frame.Cache.SavedState.EnableBlend();
      m_frame.Cache.SavedState.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;
    case BlendState::Sdf:
      {
        m_frame.Cache.SavedState.EnableBlend();
        m_frame.Cache.SavedState.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        SetUniform(record.CachedLocSmoothing, materialVariables.GetBasicMaterialVariable(BasicMaterialVariableElementUsage::SdfSmooth, 0u));
        break;
      }
    case BlendState::Opaque:
    default:
      m_frame.Cache.SavedState.DisableBlend();
      break;
    }

    switch (record.MaterialInfo.CullMode)
    {
    case BasicCullMode::Front:
      m_frame.Cache.SavedState.EnableCullFace();
      m_frame.Cache.SavedState.CullFace(GL_FRONT);
      break;
    case BasicCullMode::Back:
      m_frame.Cache.SavedState.EnableCullFace();
      m_frame.Cache.SavedState.CullFace(GL_BACK);
      break;
    case BasicCullMode::FrontAndBack:
      m_frame.Cache.SavedState.EnableCullFace();
      m_frame.Cache.SavedState.CullFace(GL_FRONT_AND_BACK);
      break;
    case BasicCullMode::Disabled:
    default:
      m_frame.Cache.SavedState.DisableCullFace();
      break;
    }

    switch (record.MaterialInfo.CullFrontFace)
    {
    case BasicFrontFace::CounterClockwise:
      m_frame.Cache.SavedState.CullFrontFace(GL_CCW);
      break;
    case BasicFrontFace::Clockwise:
    default:
      m_frame.Cache.SavedState.CullFrontFace(GL_CW);
      break;
    }

    if (record.MaterialInfo.Depth.TestEnable)
    {
      m_frame.Cache.SavedState.EnableDepthTest();
    }
    else
    {
      m_frame.Cache.SavedState.DisableDepthTest();
    }

    if (record.MaterialInfo.Depth.WriteEnable)
    {
      m_frame.Cache.SavedState.EnableDepthWrite();
    }
    else
    {
      m_frame.Cache.SavedState.DisableDepthWrite();
    }


    switch (record.MaterialInfo.Depth.CompareOp)
    {
    case BasicCompareOp::Never:
      m_frame.Cache.SavedState.DepthFunc(GL_NEVER);
      break;
    case BasicCompareOp::AlwaysMatch:
      m_frame.Cache.SavedState.DepthFunc(GL_ALWAYS);
      break;
    case BasicCompareOp::Equal:
      m_frame.Cache.SavedState.DepthFunc(GL_EQUAL);
      break;
    case BasicCompareOp::Greater:
      m_frame.Cache.SavedState.DepthFunc(GL_GREATER);
      break;
    case BasicCompareOp::GreaterOrEqual:
      m_frame.Cache.SavedState.DepthFunc(GL_GEQUAL);
      break;
    case BasicCompareOp::Less:
      m_frame.Cache.SavedState.DepthFunc(GL_LESS);
      break;
    case BasicCompareOp::LessOrEqual:
      m_frame.Cache.SavedState.DepthFunc(GL_LEQUAL);
      break;
    case BasicCompareOp::NotEqual:
      m_frame.Cache.SavedState.DepthFunc(GL_NOTEQUAL);
      break;
    default:
      FSLLOG3_ERROR("Unsupported compare op");
      break;
    }

    switch (record.MaterialInfo.ViewportMode)
    {
    case BasicViewportMode::Fullscreen:
      {
        const PxViewport viewport(0, 0, m_dependentResources.WindowSizePx.Width(), m_dependentResources.WindowSizePx.Height());
        m_frame.Cache.SavedState.SetViewport(viewport);
        break;
      }
    case BasicViewportMode::CustomOriginTopLeft:
      {
        const PxViewport& viewport = record.MaterialInfo.Viewport;
        const int32_t newY = m_dependentResources.WindowSizePx.Height() - viewport.Bottom();
        PxViewport newViewport(viewport.X(), newY, viewport.Width(), viewport.Height());
        m_frame.Cache.SavedState.SetViewport(newViewport);
        break;
      }
    case BasicViewportMode::CustomOriginBottomLeft:
      m_frame.Cache.SavedState.SetViewport(record.MaterialInfo.Viewport);
      break;
    default:
      FSLLOG3_ERROR("Viewport mode not supported");
      break;
    }

    if (m_frame.Commands.CameraInfo.IsValid)
    {
      // This only executes updates anything if the camera was modified
      m_materialFactory.SetMaterialCameraInfo(record, m_frame.Commands.CameraChangeId, m_frame.Commands.CameraInfo);
    }
  }


  void NativeGraphicsDevice::CmdBindVertexBuffer(const BasicNativeBufferHandle vertexBuffer)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid);
    // If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsValid);

    if (vertexBuffer.IsValid())
    {
      const NativeGraphicsBufferRecord& record = m_bufferFactory.GetBuffer(vertexBuffer);
      assert(record.GetBuffer().GetTarget() == GL_ARRAY_BUFFER);
      m_frame.Cache.SavedState.BindVertexBuffer(record.GetBuffer().Get());
    }
    else
    {
      m_frame.Cache.SavedState.BindVertexBuffer(0);
    }
    m_frame.Commands.VertexBufferHandle = vertexBuffer;
    m_frame.Commands.VertexBufferModified = true;
  }


  void NativeGraphicsDevice::CmdDraw(const uint32_t vertexCount, const uint32_t firstVertex)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid);
    // If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsValid);
    // If this fires no material was bound
    assert(m_frame.Commands.MaterialHandle.IsValid());
    // If this fires no vertex buffer was bound
    assert(m_frame.Commands.VertexBufferHandle.IsValid());
    // If this fires the camera was not set
    assert(m_frame.Commands.CameraInfo.IsValid);

    // Set the vertex buffer attrib arrays if the vertex buffer was modified (late bind)
    if (m_frame.Commands.VertexBufferModified)
    {
      m_frame.Commands.VertexBufferModified = false;
      const NativeGraphicsMaterialFactory::MaterialRecord* pMaterialRecord = m_materialFactory.TryGetMaterial(m_frame.Commands.MaterialHandle);
      if (pMaterialRecord != nullptr)
      {
        const VertexElementAttribLinks& vertexElementAttribLinks = m_materialFactory.GetVertexElementAttribLinks(*pMaterialRecord);
        m_frame.Cache.SavedState.ChangeVertexAttribsLinks(vertexElementAttribLinks);
      }
    }

    glDrawArrays(GL_TRIANGLES, NumericCast<GLint>(firstVertex), NumericCast<GLsizei>(vertexCount));
  }


  void NativeGraphicsDevice::CmdDrawIndexed(const uint32_t indexCount, const uint32_t firstIndex)
  {
    // If this fires BeginFrame was not called.
    assert(m_frame.IsValid);
    // If this fires BeginCache was not called
    assert(m_frame.Cache.IsValid);
    // If this fires BeginCmds was not called
    assert(m_frame.Commands.IsValid);
    // If this fires no material was bound
    assert(m_frame.Commands.MaterialHandle.IsValid());
    // If this fires no vertex buffer was bound
    assert(m_frame.Commands.VertexBufferHandle.IsValid());
    // If this fires no index buffer was bound
    assert(m_frame.Commands.IndexBufferHandle.IsValid());
    // If this fires the camera was not set
    assert(m_frame.Commands.CameraInfo.IsValid);

    // Set the vertex buffer attrib arrays if the vertex buffer was modified (late bind)
    if (m_frame.Commands.VertexBufferModified)
    {
      m_frame.Commands.VertexBufferModified = false;
      const NativeGraphicsMaterialFactory::MaterialRecord* pMaterialRecord = m_materialFactory.TryGetMaterial(m_frame.Commands.MaterialHandle);
      if (pMaterialRecord != nullptr)
      {
        const VertexElementAttribLinks& vertexElementAttribLinks = m_materialFactory.GetVertexElementAttribLinks(*pMaterialRecord);
        m_frame.Cache.SavedState.ChangeVertexAttribsLinks(vertexElementAttribLinks);
      }
    }

    glDrawElements(GL_TRIANGLES, UncheckedNumericCast<GLsizei>(indexCount), GL_UNSIGNED_SHORT,
                   reinterpret_cast<const void*>(sizeof(uint16_t) * firstIndex));
  }

  // void NativeGraphicsDevice::DisableAttribArrays()
  //{
  //  if (m_frame.Commands.VertexBufferHandle.IsValid() && m_frame.Commands.MaterialHandle.IsValid())
  //  {
  //    const NativeGraphicsMaterialFactory::MaterialRecord& oldMaterialRecord = m_materialFactory.GetMaterial(m_frame.Commands.MaterialHandle);
  //    assert(oldMaterialRecord.IsValid());
  //    oldMaterialRecord.VertexElements.DisableAttribArrays(oldMaterialRecord.AttribLink);
  //  }
  //}
}
