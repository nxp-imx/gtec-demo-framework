#ifndef FSLSIMPLEUI_RENDER_IMBATCH_MATERIALLOOKUP_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_MATERIALLOOKUP_HPP
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
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchMaterialHandle.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchMaterialId.hpp>
#include <map>
#include <memory>
#include <utility>
#include "MaterialStats.hpp"


namespace Fsl
{
  class ISprite;

  class MaterialLookup
  {
    struct Record
    {
      SpriteMaterialInfo MaterialInfo;
      uint32_t RefCount{0};

      Record() = default;
      explicit Record(const SpriteMaterialInfo& materialInfo)
        : MaterialInfo(materialInfo)
        , RefCount(1)
      {
      }
    };

    SpriteMaterialInfo m_defaultMaterialInfo;
    HandleVector<Record> m_materials;
    BatchMaterialHandle m_defaultHandle;
    std::map<SpriteMaterialId, BatchMaterialHandle> m_spriteToBatchDict;
    MaterialStats m_stats;

  public:
    explicit MaterialLookup(const SpriteMaterialInfo& defaultMaterialInfo);

    const MaterialStats& GetStats() const
    {
      return m_stats;
    }

    uint32_t GetCount() const
    {
      return UncheckedNumericCast<uint32_t>(m_materials.Count());
    }

    //! WARNING: Do only use this for lookup, never to store the handle
    BatchMaterialHandle TryGetHandle(const ISprite* const pSprite, const uint32_t spriteMaterialIndex);

    BatchMaterialHandle Acquire(const ISprite* const pSprite, const uint32_t spriteMaterialIndex);

    bool Release(const BatchMaterialHandle batchMaterialHandle);

    void SetSpriteMaterialInfo(const BatchMaterialHandle batchMaterialHandle, const SpriteMaterialInfo& materialInfo)
    {
      const uint32_t index = m_materials.FastHandleToIndex(batchMaterialHandle.Value);
      m_materials[index].MaterialInfo = materialInfo;
    }

    const SpriteMaterialInfo& GetSpriteMaterialInfo(const BatchMaterialHandle batchMaterialHandle) const noexcept
    {
      assert(m_materials.IsValidHandle(batchMaterialHandle.Value));
      return batchMaterialHandle != m_defaultHandle ? m_materials.FastGet(batchMaterialHandle.Value).MaterialInfo : m_defaultMaterialInfo;
    }

    const SpriteMaterialInfo& FastGetSpriteMaterialInfo(const BatchMaterialId batchMaterialId) const noexcept
    {
      return m_materials[batchMaterialId.Value].MaterialInfo;
    }

    BatchMaterialId GetMaterialIndex(const BatchMaterialHandle batchMaterialHandle) const
    {
      assert(m_materials.IsValidHandle(batchMaterialHandle.Value));
      return batchMaterialHandle != m_defaultHandle ? BatchMaterialId(m_materials.FastHandleToIndex(batchMaterialHandle.Value)) : BatchMaterialId();
    }
  };
}

#endif
