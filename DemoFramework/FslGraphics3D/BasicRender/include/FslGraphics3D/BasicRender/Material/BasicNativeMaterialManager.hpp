#ifndef FSLGRAPHICS3D_BASICRENDER_MATERIAL_BASICNATIVEMATERIALMANAGER_HPP
#define FSLGRAPHICS3D_BASICRENDER_MATERIAL_BASICNATIVEMATERIALMANAGER_HPP
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
#include <FslBase/Span/TypedFlexSpan.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialHandle.hpp>
#include <FslGraphics3D/BasicRender/Material/BasicMaterialRecord.hpp>
#include <FslGraphics3D/BasicRender/Material/BasicNativeMaterialRecord.hpp>
#include <memory>
#include <vector>

namespace Fsl::Graphics3D
{
  class IBasicShaderLookup;
  class INativeMaterialFactory;

  class BasicNativeMaterialManager
  {
    struct DeferredRemoveRecord
    {
      uint32_t DeferCount{0};
      BasicNativeMaterialHandle NativeHandle;

      constexpr DeferredRemoveRecord() noexcept = default;
      constexpr DeferredRemoveRecord(uint32_t deferCount, BasicNativeMaterialHandle nativeHandle) noexcept
        : DeferCount(deferCount)
        , NativeHandle(nativeHandle)
      {
      }
    };

    const IBasicShaderLookup& m_shaderLookup;
    std::shared_ptr<INativeMaterialFactory> m_factory;
    uint32_t m_maxFramesInFlight{0};

    std::vector<BasicNativeMaterialCreateInfo> m_materialCreationScratchpad;
    std::vector<BasicNativeMaterialHandle> m_nativeMaterialsScratchpad;
    std::vector<DeferredRemoveRecord> m_deferredRemove;

    HandleVector<BasicNativeMaterialHandle> m_nativeTextures;

  public:
    explicit BasicNativeMaterialManager(const IBasicShaderLookup& m_shaderLookup);
    ~BasicNativeMaterialManager();

    bool IsValid() const
    {
      return m_factory != nullptr;
    }

    void Init(const std::shared_ptr<INativeMaterialFactory>& factory, const uint32_t capacity, const uint32_t maxFramesInFlight);
    void Shutdown();
    void PreUpdate();

    void CreateMaterials(TypedFlexSpan<BasicNativeMaterialRecord> dst, ReadOnlyTypedFlexSpan<BasicMaterialDetailsRecord> src);
    BasicNativeMaterialRecord CreateMaterial(const BasicMaterialDetailsRecord& src);

    bool ScheduleRemove(const InternalMaterialHandle handle);


  private:
    void CollectGarbage(const bool force);

    // void ProcessDeferredRemove();
    // bool Remove(const BasicNativeMaterialHandle& handle);
    void EnsureCapacity(const std::size_t minCapacity);
  };

}

#endif
