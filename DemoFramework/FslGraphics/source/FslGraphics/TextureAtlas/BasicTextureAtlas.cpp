/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxThicknessU.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <limits>
#include <utility>


namespace Fsl
{
  // constexpr const uint32_t DEFAULT_DP = 160;

  namespace
  {
    void ValidateThicknessU(const NamedAtlasTexture& textureEntry, const PxThicknessU& thickness, const StringViewLite& debugHelp)
    {
      if (thickness.Left >= textureEntry.TextureInfo.ExtentPx.Width || thickness.Right >= textureEntry.TextureInfo.ExtentPx.Width ||
          thickness.SumX() >= textureEntry.TextureInfo.ExtentPx.Width)
      {
        throw std::invalid_argument(fmt::format("invalid {} thickness {} for texture '{}' of size {}", debugHelp, thickness, textureEntry.Name,
                                                textureEntry.TextureInfo.ExtentPx));
      }
      if (thickness.Top >= textureEntry.TextureInfo.ExtentPx.Height || thickness.Bottom >= textureEntry.TextureInfo.ExtentPx.Height ||
          thickness.SumY() >= textureEntry.TextureInfo.ExtentPx.Height)
      {
        throw std::invalid_argument(fmt::format("invalid {} thickness {} for texture '{}' of size {}", debugHelp, thickness, textureEntry.Name,
                                                textureEntry.TextureInfo.ExtentPx));
      }
    }

  }

  uint32_t BasicTextureAtlas::Count() const
  {
    return UncheckedNumericCast<uint32_t>(m_entries.size());
  }


  const NamedAtlasTexture& BasicTextureAtlas::GetEntry(const uint32_t index) const
  {
    if (index >= m_entries.size())
    {
      throw std::invalid_argument("Out of bounds");
    }
    return m_entries[index];
  }


  uint32_t BasicTextureAtlas::NineSliceCount() const
  {
    return UncheckedNumericCast<uint32_t>(m_nineSlices.size());
  }

  const TextureAtlasNineSlicePatch& BasicTextureAtlas::GetNineSlicePatch(const uint32_t index) const
  {
    if (index >= m_nineSlices.size())
    {
      throw std::invalid_argument("Out of bounds");
    }
    const auto& record = m_nineSlices[index];
    if (record.TextureIndex >= m_entries.size())
    {
      throw NotFoundException("linked texture is invalid");
    }
    return record;
  }


  void BasicTextureAtlas::Reset(const uint32_t capacity)
  {
    m_entries.resize(capacity);
  }


  void BasicTextureAtlas::SetEntry(const uint32_t index, const PxRectangleU32& rectanglePx, const PxThicknessU& trimPx, const uint32_t dpi,
                                   IO::Path path)
  {
    if (static_cast<std::size_t>(index) >= m_entries.size())
    {
      throw std::invalid_argument("out of bounds");
    }
    if (rectanglePx.Width >= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
    {
      throw NotSupportedException("Rectangle width too large");
    }
    if (rectanglePx.Height >= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
    {
      throw NotSupportedException("Rectangle height too large");
    }
    if (rectanglePx.Right() < rectanglePx.Left())
    {
      throw NotSupportedException("Invalid rectangle");
    }
    if (rectanglePx.Bottom() < rectanglePx.Top())
    {
      throw NotSupportedException("Invalid rectangle");
    }
    if (dpi == 0u)
    {
      throw NotSupportedException("dpi can not be zero");
    }
    if (dpi > (160 * 10))
    {
      throw NotSupportedException("dpi exceeded limit");
    }

    m_entries[index] = NamedAtlasTexture(std::move(path), AtlasTextureInfo(rectanglePx, trimPx, dpi));
  }

  void BasicTextureAtlas::AddNineSlice(const uint32_t textureIndex, const PxThicknessU& nineSlicePx, const PxThicknessU& contentMarginPx,
                                       const AtlasNineSliceFlags flags)
  {
    if (static_cast<std::size_t>(textureIndex) >= m_entries.size())
    {
      throw std::invalid_argument("out of bounds");
    }
    auto itrFind = std::find_if(m_nineSlices.begin(), m_nineSlices.end(),
                                [textureIndex](const TextureAtlasNineSlicePatch& entry) { return entry.TextureIndex == textureIndex; });
    if (itrFind != m_nineSlices.end())
    {
      throw UsageErrorException("There can only be one nine slice associated with each texture");
    }

    auto& rTextureEntry = m_entries[textureIndex];
    ValidateThicknessU(rTextureEntry, nineSlicePx, "nineslice");
    ValidateThicknessU(rTextureEntry, contentMarginPx, "contentMargin");

    m_nineSlices.emplace_back(textureIndex, AtlasNineSlicePatchInfo(nineSlicePx, contentMarginPx, flags));
  }

}
