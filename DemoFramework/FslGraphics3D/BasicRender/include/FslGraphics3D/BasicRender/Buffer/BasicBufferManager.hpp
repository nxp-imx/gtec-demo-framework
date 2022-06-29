#ifndef FSLGRAPHICS3D_BASICRENDER_BUFFER_BASICBUFFERMANAGER_HPP
#define FSLGRAPHICS3D_BASICRENDER_BUFFER_BASICBUFFERMANAGER_HPP
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

#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslGraphics/Render/Basic/BasicBufferType.hpp>
#include <FslGraphics/Render/Basic/BasicRenderSystemEvent.hpp>
#include <FslGraphics3D/BasicRender/Adapter/NativeBufferFactoryCaps.hpp>
#include <FslGraphics3D/BasicRender/Buffer/BasicDynamicBufferTracker.hpp>
#include <FslGraphics3D/BasicRender/Buffer/BasicStaticBufferTracker.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl::Graphics3D
{
  class INativeBufferFactory;

  //! Manage the lifetime of all native textures.
  //! This is needed to ensure that we don't destroy any native texture that might still be in use by a command queue.
  class BasicBufferManager final
  {
    struct StaticRecord
    {
      BasicBufferType Type{BasicBufferType::Index};
      BasicNativeBufferHandle NativeHandle{};
      std::weak_ptr<BasicStaticBufferTracker> BasicUserObjectTracker;
      uint32_t DeferCount{0};

      StaticRecord() = default;

      explicit StaticRecord(const BasicBufferType bufferType, const BasicNativeBufferHandle nativeHandle,
                            std::weak_ptr<BasicStaticBufferTracker> native, const uint32_t deferCount)
        : Type(bufferType)
        , NativeHandle(nativeHandle)
        , BasicUserObjectTracker(std::move(native))
        , DeferCount(deferCount)
      {
      }
    };
    struct DynamicRecord
    {
      std::weak_ptr<BasicDynamicBufferTracker> BasicUserObject;
      std::shared_ptr<BasicDynamicBufferLink> Link;
      uint32_t DeferCount{0};

      DynamicRecord() = default;

      explicit DynamicRecord(std::weak_ptr<BasicDynamicBufferTracker> native, std::shared_ptr<BasicDynamicBufferLink> link, const uint32_t deferCount)
        : BasicUserObject(std::move(native))
        , Link(std::move(link))
        , DeferCount(deferCount)
      {
      }
    };

    struct DependentResources
    {
      bool IsValid{false};

      constexpr DependentResources() noexcept = default;

      constexpr explicit DependentResources(bool isValid) noexcept
        : IsValid(isValid)
      {
      }
    };


    uint32_t m_maxFramesInFlight;
    std::shared_ptr<INativeBufferFactory> m_factory;
    NativeBufferFactoryCaps m_factoryCaps;
    std::vector<StaticRecord> m_staticRecords;
    std::vector<DynamicRecord> m_dynamicRecords;
    DependentResources m_dependentResources;

  public:
    explicit BasicBufferManager(const uint32_t maxFramesInFlight, std::shared_ptr<INativeBufferFactory> factory);
    ~BasicBufferManager();

    void CreateDependentResources();
    void OnRenderSystemEvent(const BasicRenderSystemEvent theEvent);
    void DestroyDependentResources();

    std::shared_ptr<IBasicStaticBuffer> CreateStaticBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData);
    std::shared_ptr<IBasicDynamicBuffer> CreateDynamicBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData, const uint32_t capacity);


    //! We expect this to be called once, early in the frame
    void PreUpdate();

  private:
    void CollectGarbage(const uint32_t deferCount, const bool force = false);
    void ForceFreeAllBuffers();
  };
}

#endif
