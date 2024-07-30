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
#include <FslSimpleUI/Base/Mesh/SpriteFontMesh.hpp>
#include <cassert>

namespace Fsl::UI
{
  bool SpriteFontMesh::SetText(const std::string& str)
  {
    return SetText(StringViewLite(str));
  }

  bool SpriteFontMesh::SetText(std::string&& str)
  {
    bool changed = false;
    if (str != m_text)
    {
      m_text = std::move(str);
      changed = true;
      MeshHandle hMesh = Get();
      if (hMesh.IsValid())
      {
        auto meshManager = GetMeshManager().lock();
        if (!meshManager)
        {
          throw UsageErrorException("mesh manager no longer valid");
        }

        UpdateMeshHandle(meshManager->SetMeshText(Get(), StringViewLite(m_text)));
      }
    }
    return changed;
  }


  bool SpriteFontMesh::SetText(StringViewLite str)
  {
    bool changed = false;
    if (str != m_text)
    {
      m_text = str;
      changed = true;
      MeshHandle hMesh = Get();
      if (hMesh.IsValid())
      {
        auto meshManager = GetMeshManager().lock();
        if (!meshManager)
        {
          throw UsageErrorException("mesh manager no longer valid");
        }
        UpdateMeshHandle(meshManager->SetMeshText(Get(), str));
      }
    }
    return changed;
  }

  PxSize2D SpriteFontMesh::MinimalMeasure() const
  {
    PxSize2D measuredPx;
    if (IsValid() && !m_text.empty())
    {
      const SpriteFont& font = GetSpriteObject();
      measuredPx = font.MeasureString(StringViewLite(m_text));
    }
    return measuredPx;
  }


  PxSize2D SpriteFontMesh::Measure() const
  {
    PxSize2D measuredPx;
    if (IsValid() && !m_text.empty())
    {
      const SpriteFont& font = GetSpriteObject();
      measuredPx = PxSize2D(font.MeasureString(StringViewLite(m_text)).Width(), font.GetInfo().ScaledLineSpacingPx);
    }
    return measuredPx;
  }

  SpriteFontMeasureInfo SpriteFontMesh::ComplexMeasure() const
  {
    PxSize2D minimalSize;
    PxSize2D measureSize;
    if (IsValid() && !m_text.empty())
    {
      const SpriteFont& font = GetSpriteObject();
      minimalSize = font.MeasureString(StringViewLite(m_text));
      measureSize = PxSize2D(minimalSize.Width(), font.GetInfo().ScaledLineSpacingPx);
    }
    return {minimalSize, measureSize};
  }

  PxSize2D SpriteFontMesh::Measure(const std::string& str) const
  {
    return Measure(StringViewLite(str));
  }

  PxSize2D SpriteFontMesh::Measure(StringViewLite str) const
  {
    PxSize2D measuredPx;
    if (IsValid() && !str.empty())
    {
      const SpriteFont& font = GetSpriteObject();
      measuredPx = PxSize2D(font.MeasureString(str).Width(), font.GetInfo().ScaledLineSpacingPx);
    }
    return measuredPx;
  }

}
