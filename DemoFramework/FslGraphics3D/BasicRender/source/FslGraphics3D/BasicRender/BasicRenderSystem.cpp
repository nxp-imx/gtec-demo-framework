/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslGraphics3D/BasicRender/BasicRenderSystem.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlyFlexSpanUtil.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/RawTextureHelper.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics3D/BasicRender/BasicRenderSystemCreateInfo.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    BasicRenderSystem::BasicRenderSystem(const Graphics3D::BasicRenderSystemCreateInfo& createInfo)
      : m_deviceResources(std::make_unique<DeviceResources>(createInfo))
    {
    }

    BasicRenderSystem::~BasicRenderSystem()
    {
      Dispose();
    }


    void BasicRenderSystem::Dispose() noexcept
    {
      if (!m_deviceResources)
      {
        return;
      }
      if (m_dependentResources.IsValid)
      {
        FSLLOG3_WARNING("DependentResources are still allocated, trying to force free them");
        try
        {
          DestroyDependentResources();
        }
        catch (std::exception& ex)
        {
          FSLLOG3_ERROR("DestroyDependentResources failed with: {}, trying to continue", ex.what());
        }
      }

      // Free the device resources
      m_deviceResources.reset();
    }


    void BasicRenderSystem::CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo)
    {
      if (!m_deviceResources)
      {
        FSLLOG3_ERROR("DestroyDependentResources: called on disposed object, call ignored");
        return;
      }

      try
      {
        m_deviceResources->Device->CreateDependentResources(createInfo);
        m_deviceResources->CreateDependentResources();
        m_dependentResources = DependentResources(true);
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Failed to create dependent resources: ", ex.what());
        DestroyDependentResources();
        throw;
      }
    }

    void BasicRenderSystem::DestroyDependentResources()
    {
      if (!m_deviceResources)
      {
        FSLLOG3_ERROR("DestroyDependentResources: called on disposed object, call ignored");
        return;
      }

      try
      {
        m_dependentResources = {};
        m_deviceResources->DestroyDependentResources();
        m_deviceResources->Device->DestroyDependentResources();
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Failed to destroy dependent resources: ", ex.what());
        throw;
      }
    }

    void BasicRenderSystem::OnRenderSystemEvent(const BasicRenderSystemEvent theEvent)
    {
      if (!m_deviceResources)
      {
        FSLLOG3_ERROR("OnRenderSystemEvent: called on disposed object, call ignored");
        return;
      }
      m_deviceResources->OnRenderSystemEvent(theEvent);
    }

    void BasicRenderSystem::PreUpdate()
    {
      if (!m_deviceResources)
      {
        FSLLOG3_ERROR("PreUpdate: called on disposed object, call ignored");
        return;
      }
      m_deviceResources->PreUpdate();
    }


    void BasicRenderSystem::BeginFrame(const BasicNativeBeginFrameInfo& frameInfo)
    {
      if (!m_deviceResources)
      {
        FSLLOG3_ERROR("BasicRenderSystem::BeginFrame called on disposed object");
        return;
      }
      if (m_frame.IsValid)
      {
        FSLLOG3_ERROR("Already in a frame, force ending it");
        EndFrame();
      }
      m_frame = FrameRecord(true);
      m_deviceResources->Device->BeginFrame(frameInfo);
    }


    void BasicRenderSystem::EndFrame()
    {
      if (!m_deviceResources)
      {
        FSLLOG3_ERROR("BasicRenderSystem::EndFrame called on disposed object");
        return;
      }
      if (!m_frame.IsValid)
      {
        FSLLOG3_WARNING("BasicRenderSystem::EndFrame: Ending frame that was not begun");
        return;
      }
      m_deviceResources->Device->EndFrame();
      m_frame = {};
    }

    std::shared_ptr<INativeTexture2D> BasicRenderSystem::CreateTexture2D(const Bitmap& bitmap, const Texture2DFilterHint filterHint,
                                                                         const TextureFlags textureFlags)
    {
      RawBitmap raw;
      Bitmap::ScopedDirectAccess access(bitmap, raw);
      return CreateTexture2D(raw, filterHint, textureFlags);
    }


    std::shared_ptr<IDynamicNativeTexture2D> BasicRenderSystem::CreateDynamicTexture2D(const Bitmap& bitmap, const Texture2DFilterHint filterHint,
                                                                                       const TextureFlags textureFlags)
    {
      RawBitmap raw;
      Bitmap::ScopedDirectAccess access(bitmap, raw);
      return CreateDynamicTexture2D(raw, filterHint, textureFlags);
    }


    std::shared_ptr<INativeTexture2D> BasicRenderSystem::CreateTexture2D(const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                                                                         const TextureFlags textureFlags)
    {
      return CreateTexture2D(RawTextureHelper::ToRawTexture(bitmap), filterHint, textureFlags);
    }


    std::shared_ptr<IDynamicNativeTexture2D> BasicRenderSystem::CreateDynamicTexture2D(const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                                                                                       const TextureFlags textureFlags)
    {
      return CreateDynamicTexture2D(RawTextureHelper::ToRawTexture(bitmap), filterHint, textureFlags);
    }


    std::shared_ptr<INativeTexture2D> BasicRenderSystem::CreateTexture2D(const Texture& texture, const Texture2DFilterHint filterHint,
                                                                         const TextureFlags textureFlags)
    {
      RawTexture raw;
      Texture::ScopedDirectAccess access(texture, raw);
      return CreateTexture2D(raw, filterHint, textureFlags);
    }


    std::shared_ptr<IDynamicNativeTexture2D> BasicRenderSystem::CreateDynamicTexture2D(const Texture& texture, const Texture2DFilterHint filterHint,
                                                                                       const TextureFlags textureFlags)
    {
      RawTexture raw;
      Texture::ScopedDirectAccess access(texture, raw);
      return CreateDynamicTexture2D(raw, filterHint, textureFlags);
    }


    std::shared_ptr<INativeTexture2D> BasicRenderSystem::CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                         const TextureFlags textureFlags)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CreateTexture2D called on disposed object");
      }
      return m_deviceResources->Textures.CreateTexture2D(texture, filterHint, textureFlags);
    }

    std::shared_ptr<IDynamicNativeTexture2D>
      BasicRenderSystem::CreateDynamicTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CreateDynamicTexture2D called on disposed object");
      }
      return m_deviceResources->Textures.CreateDynamicTexture2D(texture, filterHint, textureFlags);
    }


    PxExtent2D BasicRenderSystem::GetTextureExtentPx(const std::shared_ptr<INativeTexture2D>& texture) const
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CreateDynamicTexture2D called on disposed object");
      }
      return m_deviceResources->Textures.GetTextureExtentPx(texture);
    }


    const IBasicNativeTexture* BasicRenderSystem::TryGetNativeTexture(const BasicNativeTextureHandle& hTexture) const
    {
      return m_deviceResources ? m_deviceResources->Textures.TryGetNativeTexture(hTexture) : nullptr;
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------


    std::shared_ptr<IBasicStaticBuffer> BasicRenderSystem::CreateBuffer(const ReadOnlySpan<uint16_t> indexSpan, const BasicBufferUsage usage)
    {
      return DoCreateBuffer(BasicBufferType::Index, ReadOnlyFlexSpanUtil::AsSpan(indexSpan), usage);
    }

    std::shared_ptr<IBasicDynamicBuffer> BasicRenderSystem::CreateDynamicBuffer(const ReadOnlySpan<uint16_t> indexSpan)
    {
      return DoCreateDynamicBuffer(BasicBufferType::Index, ReadOnlyFlexSpanUtil::AsSpan(indexSpan));
    }

    std::shared_ptr<IBasicDynamicBuffer> BasicRenderSystem::CreateDynamicBuffer(const ReadOnlySpan<uint16_t> indexSpan, const uint32_t capacity)
    {
      return DoCreateDynamicBuffer(BasicBufferType::Index, ReadOnlyFlexSpanUtil::AsSpan(indexSpan), capacity);
    }

    std::shared_ptr<IBasicStaticBuffer> BasicRenderSystem::CreateStaticBuffer(const ReadOnlySpan<uint16_t> indexSpan)
    {
      return DoCreateStaticBuffer(BasicBufferType::Index, ReadOnlyFlexSpanUtil::AsSpan(indexSpan));
    }

    std::shared_ptr<IBasicStaticBuffer> BasicRenderSystem::CreateBuffer(const ReadOnlyFlexVertexSpan& vertexSpan, const BasicBufferUsage usage)
    {
      return DoCreateBuffer(BasicBufferType::Vertex, vertexSpan.AsFlexSpan(), usage);
    }

    std::shared_ptr<IBasicDynamicBuffer> BasicRenderSystem::CreateDynamicBuffer(const ReadOnlyFlexVertexSpan& vertexSpan)
    {
      return DoCreateDynamicBuffer(BasicBufferType::Vertex, vertexSpan.AsFlexSpan());
    }


    std::shared_ptr<IBasicDynamicBuffer> BasicRenderSystem::CreateDynamicBuffer(const ReadOnlyFlexVertexSpan& vertexSpan, const uint32_t capacity)
    {
      return DoCreateDynamicBuffer(BasicBufferType::Vertex, vertexSpan.AsFlexSpan(), capacity);
    }

    std::shared_ptr<IBasicStaticBuffer> BasicRenderSystem::CreateStaticBuffer(const ReadOnlyFlexVertexSpan& vertexSpan)
    {
      return DoCreateStaticBuffer(BasicBufferType::Vertex, vertexSpan.AsFlexSpan());
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    BasicMaterial BasicRenderSystem::CreateMaterial(const BasicMaterialCreateInfo& createInfo, const std::shared_ptr<INativeTexture2D>& texture,
                                                    const bool isDynamic)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CreateMaterial called on disposed object");
      }
      return m_deviceResources->Materials.CreateMaterial(createInfo, texture, isDynamic);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    BasicMaterial BasicRenderSystem::CloneMaterial(const BasicMaterial& sourceMaterial, const std::shared_ptr<INativeTexture2D>& texture,
                                                   const bool isDynamic)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CloneMaterial called on disposed object");
      }
      return m_deviceResources->Materials.CloneMaterial(sourceMaterial, texture, isDynamic);
    }


    BasicMaterial BasicRenderSystem::CloneMaterial(const BasicMaterial& sourceMaterial, const BasicMaterialInfo& materialInfo, const bool isDynamic)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CloneMaterial called on disposed object");
      }
      return m_deviceResources->Materials.CloneMaterial(sourceMaterial, materialInfo, isDynamic);
    }


    // -----------------------------------------------------------------------------------------------------------------------------------------------

    BasicMaterialInfo BasicRenderSystem::GetMaterialInfo(const BasicMaterial& sourceMaterial) const
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("GetMaterialInfo called on disposed object");
      }
      return m_deviceResources->Materials.GetMaterialInfo(sourceMaterial);
    }

    void BasicRenderSystem::SetMaterialInfo(const BasicMaterial& sourceMaterial, const BasicMaterialInfo& materialInfo)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("SetMaterialInfo called on disposed object");
      }
      return m_deviceResources->Materials.SetMaterialInfo(sourceMaterial, materialInfo);
    }

    std::shared_ptr<INativeTexture2D> BasicRenderSystem::GetMaterialTexture(const BasicMaterial& hMaterial) const
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("GetMaterialTexture called on disposed object");
      }

      auto ptr = TryGetMaterialTexture(hMaterial);
      return ptr ? ptr : throw NotFoundException("GetMaterialTexture texture not found");
    }

    std::shared_ptr<INativeTexture2D> BasicRenderSystem::TryGetMaterialTexture(const BasicMaterial& hMaterial) const
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CreateMaterial called on disposed object");
      }
      return m_deviceResources->Materials.TryGetMaterialTexture(hMaterial);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::BeginCache()
    {
      if (m_frame.IsValid && !m_frame.BeginCommands)
      {
        if (m_frame.CacheState != CachingState::Invalid)
        {
          FSLLOG3_ERROR("Previous BeginCache was not terminated with a EndCache call, so forcing it here.");
          EndCache();
        }
        DoBeginCache(false);
      }
      else
      {
        FSLLOG3_ERROR_IF(m_frame.BeginCommands, "BeginCache called inside a BeginCmds block, call ignored");
        FSLLOG3_ERROR_IF(!m_frame.BeginCommands, "BeginCache called outside a BeginFrame/EndFrame block, call ignored");
      }
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::EndCache()
    {
      if (m_frame.IsValid && !m_frame.BeginCommands && m_frame.CacheState != CachingState::Invalid)
      {
        FSLLOG3_ERROR_IF(m_frame.CacheState == CachingState::ForceCached, "EndCache called without BeginCache");
        DoEndCache();
      }
      else
      {
        FSLLOG3_ERROR_IF(m_frame.BeginCommands, "BeginCache called inside a BeginCmds block, call ignored");
        FSLLOG3_ERROR_IF(!m_frame.BeginCommands, "BeginCache called outside a BeginFrame/EndFrame block, call ignored");
      }
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::BeginCmds()
    {
      if (!m_frame.IsValid)
      {
        throw UsageErrorException("BeginCmds outside a BeginFrame/EndFrame block");
      }
      if (m_frame.BeginCommands)
      {
        FSLLOG3_ERROR("Previous BeginCmds was not terminated with a EndCmds call, so forcing it here.");
        EndCmds();
      }
      if (m_frame.CacheState == CachingState::Invalid)
      {
        DoBeginCache(true);
      }
      m_deviceResources->Device->BeginCmds();
      m_frame.BeginCommands = true;
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::EndCmds()
    {
      if (!m_frame.IsValid)
      {
        throw UsageErrorException("BeginCmds outside a BeginFrame/EndFrame block");
      }
      if (m_frame.BeginCommands)
      {
        m_frame.BeginCommands = false;
        m_deviceResources->Device->EndCmds();

        if (m_frame.CacheState == CachingState::ForceCached)
        {
          DoEndCache();
        }
      }
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------


    void BasicRenderSystem::CmdSetCamera(const BasicCameraInfo& cameraInfo)
    {
      if (!m_frame.IsValid || !m_frame.BeginCommands)
      {
        throw UsageErrorException("CmdSetCamera called outside a BeginFrame/EndFrame or BeginCmds/EndCmds block");
      }

      DeviceResources* pDeviceResources = m_deviceResources.get();
      if (pDeviceResources == nullptr)
      {
        throw UsageErrorException("CmdSetCamera called on disposed object");
      }

      pDeviceResources->Device->CmdSetCamera(cameraInfo);
    }


    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::CmdBindMaterial(const BasicMaterial& material)
    {
      if (!m_frame.IsValid || !m_frame.BeginCommands)
      {
        throw UsageErrorException("CmdBindMaterial called outside a BeginFrame/EndFrame or BeginCmds/EndCmds block");
      }

      DeviceResources* pDeviceResources = m_deviceResources.get();
      if (pDeviceResources == nullptr)
      {
        throw UsageErrorException("CmdBindMaterial called on disposed object");
      }

      const BasicMaterialRecord* pRecord = pDeviceResources->Materials.TryGetMaterialRecord(material);
      if (pRecord == nullptr)
      {
        FSLLOG3_ERROR("CmdBindMaterial called with unknown material, call ignored");
        return;
      }

      // For now we only have one texture per material
      assert(pRecord->Texture);
      assert(pRecord->Native.IsValid());
      auto hNativeTexture = pRecord->Texture->TryGetNativeHandle();
      pDeviceResources->Device->CmdBindMaterial(pRecord->Native.NativeHandle, material.AsPushConstants(),
                                                ReadOnlySpan<BasicNativeTextureHandle>(&hNativeTexture, 1));
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::CmdBindIndexBuffer(const std::shared_ptr<IBasicStaticBuffer>& indexBuffer)
    {
      if (!m_frame.IsValid || !m_frame.BeginCommands)
      {
        throw UsageErrorException("CmdBindIndexBuffer called outside a BeginFrame/EndFrame or BeginCmds/EndCmds block");
      }

      DeviceResources* pDeviceResources = m_deviceResources.get();
      if (pDeviceResources == nullptr)
      {
        throw UsageErrorException("CmdBindIndexBuffer called on disposed object");
      }

      // BasicNativeBufferHandle hNative = pDeviceResources->Buffers.TryGetNativeHandle();
      BasicNativeBufferHandle hNative = indexBuffer->TryGetNativeHandle();
      if (!hNative.IsValid())
      {
        FSLLOG3_ERROR("CmdBindIndexBuffer called with unknown index buffer");
        return;
      }

      pDeviceResources->Device->CmdBindIndexBuffer(hNative);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::CmdBindVertexBuffer(const std::shared_ptr<IBasicStaticBuffer>& vertexBuffer)
    {
      if (!m_frame.IsValid || !m_frame.BeginCommands)
      {
        throw UsageErrorException("CmdBindVertexBuffer called outside a BeginFrame/EndFrame or BeginCmds/EndCmds block");
      }

      DeviceResources* pDeviceResources = m_deviceResources.get();
      if (pDeviceResources == nullptr)
      {
        throw UsageErrorException("CmdBindVertexBuffer called on disposed object");
      }

      // BasicNativeBufferHandle hNative = pDeviceResources->Buffers.TryGetNativeHandle(vertexBuffer);
      BasicNativeBufferHandle hNative = vertexBuffer->TryGetNativeHandle();
      if (!hNative.IsValid())
      {
        FSLLOG3_ERROR("CmdBindVertexBuffer called with unknown vertex buffer");
        return;
      }

      pDeviceResources->Device->CmdBindVertexBuffer(hNative);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::CmdDraw(const uint32_t vertexCount, const uint32_t firstVertex)
    {
      if (!m_frame.IsValid || !m_frame.BeginCommands)
      {
        throw UsageErrorException("CmdDraw called outside a BeginFrame/EndFrame or BeginCmds/EndCmds block");
      }

      DeviceResources* pDeviceResources = m_deviceResources.get();
      if (pDeviceResources == nullptr)
      {
        throw UsageErrorException("CmdDraw called on disposed object");
      }

      pDeviceResources->Device->CmdDraw(vertexCount, firstVertex);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::CmdDrawIndexed(const uint32_t indexCount, const uint32_t firstIndex)
    {
      if (!m_frame.IsValid || !m_frame.BeginCommands)
      {
        throw UsageErrorException("CmdDrawIndexed called outside a BeginFrame/EndFrame or BeginCmds/EndCmds block");
      }

      DeviceResources* pDeviceResources = m_deviceResources.get();
      if (pDeviceResources == nullptr)
      {
        throw UsageErrorException("CmdDrawIndexed called on disposed object");
      }

      pDeviceResources->Device->CmdDrawIndexed(indexCount, firstIndex);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::DoBeginCache(const bool forced)
    {
      assert(m_frame.IsValid);
      assert(m_frame.CacheState == CachingState::Invalid);
      assert(!m_frame.BeginCommands);
      m_deviceResources->Device->BeginCache();
      m_frame.CacheState = forced ? CachingState::ForceCached : CachingState::UserCached;
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    void BasicRenderSystem::DoEndCache()
    {
      assert(m_frame.IsValid);
      assert(m_frame.CacheState != CachingState::Invalid);
      assert(!m_frame.BeginCommands);

      m_deviceResources->Device->EndCache();
      m_frame.CacheState = CachingState::Invalid;
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    std::shared_ptr<IBasicStaticBuffer> BasicRenderSystem::DoCreateBuffer(const BasicBufferType bufferType, const ReadOnlyFlexSpan span,
                                                                          const BasicBufferUsage usage)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CreateBuffer called on disposed object");
      }
      switch (usage)
      {
      case BasicBufferUsage::Dynamic:
        return DoCreateDynamicBuffer(bufferType, span);
      case BasicBufferUsage::Static:
        return DoCreateStaticBuffer(bufferType, span);
      default:
        throw NotSupportedException("Unsupported usage");
      }
    }

    std::shared_ptr<IBasicDynamicBuffer> BasicRenderSystem::DoCreateDynamicBuffer(const BasicBufferType bufferType, const ReadOnlyFlexSpan span)
    {
      return DoCreateDynamicBuffer(bufferType, span, NumericCast<uint32_t>(span.size()));
    }


    std::shared_ptr<IBasicDynamicBuffer> BasicRenderSystem::DoCreateDynamicBuffer(const BasicBufferType bufferType, const ReadOnlyFlexSpan span,
                                                                                  const uint32_t capacity)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CreateDynamicBuffer called on disposed object");
      }
      return m_deviceResources->Buffers.CreateDynamicBuffer(bufferType, span, capacity);
    }

    std::shared_ptr<IBasicStaticBuffer> BasicRenderSystem::DoCreateStaticBuffer(const BasicBufferType bufferType, const ReadOnlyFlexSpan span)
    {
      if (!m_deviceResources)
      {
        throw UsageErrorException("CreateStaticBuffer called on disposed object");
      }
      return m_deviceResources->Buffers.CreateStaticBuffer(bufferType, span);
    }
  }
}
