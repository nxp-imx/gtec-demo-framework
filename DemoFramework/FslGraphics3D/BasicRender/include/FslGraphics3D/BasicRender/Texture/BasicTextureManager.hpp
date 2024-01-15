#ifndef FSLGRAPHICS3D_BASICRENDER_TEXTURE_BASICTEXTUREMANAGER_HPP
#define FSLGRAPHICS3D_BASICRENDER_TEXTURE_BASICTEXTUREMANAGER_HPP
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
#include <FslGraphics/Render/Basic/Adapter/BasicNativeTextureHandle.hpp>
#include <FslGraphics/Render/Basic/BasicRenderSystemEvent.hpp>
#include <FslGraphics/Render/Basic/Texture/BasicTextureHandle.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslGraphics3D/BasicRender/Adapter/NativeTextureFactoryCaps.hpp>
#include <FslGraphics3D/BasicRender/Texture/BasicDynamicTextureTracker.hpp>
#include <FslGraphics3D/BasicRender/Texture/BasicStaticTextureTracker.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl::Graphics3D
{
  class INativeTextureFactory;

  //! Manage the lifetime of all native textures.
  //! This is needed to ensure that we don't destroy any native texture that might still be in use by a command queue.
  class BasicTextureManager final
  {
    struct TextureRecord
    {
      bool IsDynamic{false};
      constexpr TextureRecord() noexcept = default;
      constexpr explicit TextureRecord(const bool isDynamic) noexcept
        : IsDynamic(isDynamic)
      {
      }
    };

    struct StaticRecord
    {
      StaticRecord(const StaticRecord&) = delete;
      StaticRecord& operator=(const StaticRecord&) = delete;

      BasicTextureHandle Handle;
      std::weak_ptr<BasicStaticTextureTracker> Texture;
      BasicNativeTextureHandle NativeHandle;
      uint32_t DeferCount{0};

      StaticRecord() noexcept = default;

      explicit StaticRecord(const BasicTextureHandle handle, std::weak_ptr<BasicStaticTextureTracker> nativeTexture,
                            BasicNativeTextureHandle nativeHandle, const uint32_t deferCount)
        : Handle(handle)
        , Texture(std::move(nativeTexture))
        , NativeHandle(nativeHandle)
        , DeferCount(deferCount)
      {
      }

      StaticRecord(StaticRecord&& other) noexcept
        : Handle(other.Handle)
        , Texture(std::move(other.Texture))
        , NativeHandle(other.NativeHandle)
        , DeferCount(other.DeferCount)
      {
        // remove from other
        other.Handle = {};
        other.NativeHandle = {};
        other.DeferCount = 0;
      }

      StaticRecord& operator=(StaticRecord&& other) noexcept
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          Reset();

          Handle = other.Handle;
          Texture = std::move(other.Texture);
          NativeHandle = other.NativeHandle;
          DeferCount = other.DeferCount;

          // remove from other
          other.Handle = {};
          other.NativeHandle = {};
          other.DeferCount = 0;
        }
        return *this;
      }

      void Reset() noexcept
      {
        // Use destruction order
        DeferCount = 0;
        NativeHandle = {};
        Texture.reset();
        Handle = {};
      }
    };

    struct DynamicRecord
    {
      DynamicRecord(const DynamicRecord&) = delete;
      DynamicRecord& operator=(const DynamicRecord&) = delete;

      BasicTextureHandle Handle;
      std::weak_ptr<BasicDynamicTextureTracker> Texture;
      std::shared_ptr<BasicDynamicTextureLink> LinkTexture;
      uint32_t DeferCount{0};

      DynamicRecord() noexcept = default;

      explicit DynamicRecord(const BasicTextureHandle handle, std::weak_ptr<BasicDynamicTextureTracker> nativeTexture,
                             std::shared_ptr<BasicDynamicTextureLink> linkTexture, const uint32_t deferCount)
        : Handle(handle)
        , Texture(std::move(nativeTexture))
        , LinkTexture(std::move(linkTexture))
        , DeferCount(deferCount)
      {
      }

      DynamicRecord(DynamicRecord&& other) noexcept
        : Handle(other.Handle)
        , Texture(std::move(other.Texture))
        , LinkTexture(std::move(other.LinkTexture))
        , DeferCount(other.DeferCount)
      {
        // remove from other
        other.Handle = {};
        other.DeferCount = 0;
      }

      DynamicRecord& operator=(DynamicRecord&& other) noexcept
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          Reset();

          Handle = other.Handle;
          Texture = std::move(other.Texture);
          LinkTexture = std::move(other.LinkTexture);
          DeferCount = other.DeferCount;

          // remove from other
          other.Handle = {};
          other.DeferCount = 0;
        }
        return *this;
      }

      void Reset() noexcept
      {
        // Use destruction order
        DeferCount = 0;
        LinkTexture.reset();
        Texture.reset();
        Handle = {};
      }
    };

    struct DependentResources
    {
      // DependentResources(const DependentResources&) = delete;
      // DependentResources& operator=(const DependentResources&) = delete;
      // DependentResources(DependentResources&& other) noexcept = delete;
      // DependentResources& operator=(DependentResources&& other) noexcept = delete;

      bool IsValid{false};

      constexpr DependentResources() noexcept = default;

      constexpr explicit DependentResources(bool isValid) noexcept
        : IsValid(isValid)
      {
      }

      void Reset()
      {
        IsValid = false;
      }
    };

    uint32_t m_maxFramesInFlight;
    std::shared_ptr<INativeTextureFactory> m_factory;
    NativeTextureFactoryCaps m_factoryCaps;
    HandleVector<TextureRecord> m_textures;
    std::vector<StaticRecord> m_staticRecords;
    std::vector<DynamicRecord> m_dynamicRecords;
    DependentResources m_dependentResources;

  public:
    explicit BasicTextureManager(const uint32_t maxFramesInFlight, std::shared_ptr<INativeTextureFactory> factory);
    ~BasicTextureManager();

    void CreateDependentResources();
    void OnRenderSystemEvent(const BasicRenderSystemEvent theEvent);
    void DestroyDependentResources();

    std::shared_ptr<INativeTexture2D> CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                      const TextureFlags textureFlags);

    std::shared_ptr<IDynamicNativeTexture2D> CreateDynamicTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                    const TextureFlags textureFlags);


    PxExtent2D GetTextureExtentPx(const std::shared_ptr<INativeTexture2D>& texture) const noexcept;

    const IBasicNativeTexture* TryGetNativeTexture(const BasicNativeTextureHandle hTexture) const noexcept;

    //! We expect this to be called once, early in the frame
    void PreUpdate();

  private:
    void DoCollectGarbage(const uint32_t deferCount, const bool force = false) noexcept;
    void ForceFreeAllTextures() noexcept;
  };
}
#endif
