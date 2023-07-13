#ifndef FSLGRAPHICS_UNITTEST_HELPER_SPRITE_MATERIAL_TEST_SPRITEMATERIALIMPL_HPP
#define FSLGRAPHICS_UNITTEST_HELPER_SPRITE_MATERIAL_TEST_SPRITEMATERIALIMPL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU16.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/Sprite/Material/ISpriteMaterial.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>

namespace Fsl
{
  class SpriteMaterialImpl final : public ISpriteMaterial
  {
    SpriteMaterialId m_materialId;
    PxExtent2D m_extentPx;

  public:
    constexpr SpriteMaterialImpl(const SpriteMaterialId id, const PxExtent2D extentPx)
      : m_materialId(id)
      , m_extentPx(extentPx)
    {
    }

    SpriteMaterialId GetId() const
    {
      return m_materialId;
    }

    PxExtent2D GetExtent() const
    {
      return m_extentPx;
    }

    static constexpr NativeTextureArea TestCalcNativeTextureArea(const PxExtent2D extentPx, const PxRectangleU16 imageRectanglePx)
    {
      if (imageRectanglePx.Right() > extentPx.Width || imageRectanglePx.Bottom() > extentPx.Height)
      {
        throw std::invalid_argument("the supplied imageRectangle does not fit inside the given extent");
      }

      return {imageRectanglePx.RawLeft() == 0 ? 0.0f : static_cast<float>(imageRectanglePx.RawLeft()) / static_cast<float>(extentPx.Width.Value),
              imageRectanglePx.RawTop() == 0 ? 0.0f : static_cast<float>(imageRectanglePx.RawTop()) / static_cast<float>(extentPx.Height.Value),
              imageRectanglePx.Right() >= extentPx.Width ? 1.0f
                                                         : static_cast<float>(imageRectanglePx.RawRight()) / static_cast<float>(extentPx.Width.Value),
              imageRectanglePx.Bottom() >= extentPx.Height
                ? 1.0f
                : static_cast<float>(imageRectanglePx.RawBottom()) / static_cast<float>(extentPx.Height.Value)};
    }

    static constexpr NativeTextureArea TestCalcNativeTextureArea(const PxExtent2D& extentPx, const PxRectangleU32& imageRectanglePx)
    {
      if (imageRectanglePx.Right() > extentPx.Width || imageRectanglePx.Bottom() > extentPx.Height)
      {
        throw std::invalid_argument("the supplied imageRectangle does not fit inside the given extent");
      }

      return {imageRectanglePx.RawLeft() == 0 ? 0.0f : static_cast<float>(imageRectanglePx.RawLeft()) / static_cast<float>(extentPx.Width.Value),
              imageRectanglePx.RawTop() == 0 ? 0.0f : static_cast<float>(imageRectanglePx.RawTop()) / static_cast<float>(extentPx.Height.Value),
              imageRectanglePx.Right() >= extentPx.Width ? 1.0f
                                                         : static_cast<float>(imageRectanglePx.RawRight()) / static_cast<float>(extentPx.Width.Value),
              imageRectanglePx.Bottom() >= extentPx.Height
                ? 1.0f
                : static_cast<float>(imageRectanglePx.RawBottom()) / static_cast<float>(extentPx.Height.Value)};
    }
  };
}

#endif
