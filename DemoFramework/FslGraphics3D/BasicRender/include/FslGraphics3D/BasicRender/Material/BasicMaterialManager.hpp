#ifndef FSLGRAPHICS3D_BASICRENDER_MATERIAL_BASICMATERIALMANAGER_HPP
#define FSLGRAPHICS3D_BASICRENDER_MATERIAL_BASICMATERIALMANAGER_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialHandle.hpp>
#include <FslGraphics/Render/Basic/BasicMaterial.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Render/Basic/BasicRenderSystemEvent.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeMaterialFactory.hpp>
#include <FslGraphics3D/BasicRender/Material/BasicMaterialRecord.hpp>
#include <FslGraphics3D/BasicRender/Material/BasicNativeMaterialManager.hpp>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  namespace Graphics3D
  {
    //! Manage the lifetime of all native textures.
    //! This is needed to ensure that we don't destroy any native texture that might still be in use by a command queue.
    class BasicMaterialManager final
    {
      uint32_t m_maxFramesInFlight;
      std::shared_ptr<INativeMaterialFactory> m_factory;
      HandleVector<BasicMaterialRecord> m_records;

      BasicNativeMaterialManager m_nativeMaterialManager;

    public:
      explicit BasicMaterialManager(const uint32_t maxFramesInFlight, std::shared_ptr<INativeMaterialFactory> factory);
      ~BasicMaterialManager();

      void CreateDependentResources();
      void OnRenderSystemEvent(const BasicRenderSystemEvent theEvent);
      void DestroyDependentResources();

      BasicMaterial CreateMaterial(const BasicMaterialCreateInfo& createInfo, std::shared_ptr<INativeTexture2D> texture, const bool isDynamic);
      BasicMaterial CloneMaterial(const BasicMaterial& sourceMaterial, const std::shared_ptr<INativeTexture2D>& texture, const bool isDynamic);
      BasicMaterial CloneMaterial(const BasicMaterial& sourceMaterial, const BasicMaterialInfo& materialInfo, const bool isDynamic);

      BasicMaterialInfo GetMaterialInfo(const BasicMaterial& sourceMaterial) const;
      void SetMaterialInfo(const BasicMaterial& sourceMaterial, const BasicMaterialInfo& materialInfo);

      const BasicMaterialRecord* TryGetMaterialRecord(const BasicMaterial& sourceMaterial)
      {
        return m_records.TryGet(sourceMaterial.GetHandle().Value);
      }

      std::shared_ptr<INativeTexture2D> TryGetMaterialTexture(const BasicMaterial& sourceMaterial);
      BasicNativeMaterialHandle TryGetNativeHandle(const BasicMaterial& material) const;

      uint32_t MaterialCount() const;

      //! We expect this to be called once, early in the frame
      void PreUpdate();

    private:
      void CollectGarbage(const uint32_t deferCount, const bool force = false);
      void ForceFreeAll();
    };
  }
}

#endif
