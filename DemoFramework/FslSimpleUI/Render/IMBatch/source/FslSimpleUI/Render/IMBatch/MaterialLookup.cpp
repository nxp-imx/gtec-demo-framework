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

#include "MaterialLookup.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Sprite/ISprite.hpp>

namespace Fsl::UI::RenderIMBatch
{
  MaterialLookup::MaterialLookup(const SpriteMaterialInfo& defaultMaterialInfo)
    : m_defaultMaterialInfo(defaultMaterialInfo)
  {
    if (!defaultMaterialInfo.IsValid())
    {
      throw std::invalid_argument("defaultMaterialInfo must be valid");
    }

    if (!defaultMaterialInfo.IsOpaque)
    {
      throw std::invalid_argument("defaultMaterialInfo must be opaque");
    }
    m_defaultHandle = BatchMaterialHandle(m_materials.Add(Record(defaultMaterialInfo)));
  }


  BatchMaterialHandle MaterialLookup::TryGetHandle(const ISprite* const pSprite, const uint32_t spriteMaterialIndex)
  {
    if (pSprite == nullptr)
    {
      return m_defaultHandle;
    }
    if (spriteMaterialIndex >= pSprite->GetMaterialCount())
    {
      FSLLOG3_WARNING("spriteMaterialIndex is out of bounds");
      return m_defaultHandle;
    }
    const auto& materialInfo = pSprite->GetMaterialInfo(spriteMaterialIndex);
    const auto itrFind = m_spriteToBatchDict.find(materialInfo.Id);
    return itrFind != m_spriteToBatchDict.end() ? itrFind->second : BatchMaterialHandle::Invalid();
  }


  BatchMaterialHandle MaterialLookup::Acquire(const ISprite* const pSprite, const uint32_t spriteMaterialIndex)
  {
    if (pSprite == nullptr)
    {
      return m_defaultHandle;
    }
    if (spriteMaterialIndex >= pSprite->GetMaterialCount())
    {
      FSLLOG3_WARNING("spriteMaterialIndex is out of bounds");
      return m_defaultHandle;
    }

    const auto& materialInfo = pSprite->GetMaterialInfo(spriteMaterialIndex);
    const auto itrFind = m_spriteToBatchDict.find(materialInfo.Id);
    if (itrFind != m_spriteToBatchDict.end())
    {
      ++m_materials.Get(itrFind->second.Value).RefCount;
      return itrFind->second;
    }

    // New material type
    BatchMaterialHandle batchHandle = BatchMaterialHandle(m_materials.Add(Record(materialInfo)));
    m_spriteToBatchDict.emplace(materialInfo.Id, batchHandle);
    if (materialInfo.IsOpaque)
    {
      ++m_stats.OpaqueCount;
    }
    else
    {
      ++m_stats.TransparentCount;
    }
    return batchHandle;
  }

  bool MaterialLookup::Release(const BatchMaterialHandle batchMaterialHandle)
  {
    if (batchMaterialHandle == m_defaultHandle)
    {
      // This is the default material -> so do nothing
      return true;
    }

    Record* pRecord = m_materials.TryGet(batchMaterialHandle.Value);
    if (pRecord != nullptr)
    {
      if (pRecord->RefCount > 1u)
      {
        --pRecord->RefCount;
        return true;
      }
      // This is the last reference, so its time to erase the material

      if (pRecord->MaterialInfo.IsOpaque)
      {
        assert(m_stats.OpaqueCount > 0u);
        --m_stats.OpaqueCount;
      }
      else
      {
        assert(m_stats.TransparentCount > 0u);
        --m_stats.TransparentCount;
      }

      m_spriteToBatchDict.erase(pRecord->MaterialInfo.Id);
      return m_materials.Remove(batchMaterialHandle.Value);
    }
    return false;
  }
}
