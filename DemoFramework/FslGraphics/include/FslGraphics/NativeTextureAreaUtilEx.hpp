#ifndef FSLGRAPHICS_NATIVETEXTUREAREAUTILEX_HPP
#define FSLGRAPHICS_NATIVETEXTUREAREAUTILEX_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <stdexcept>

namespace Fsl
{
  namespace NativeTextureAreaUtil
  {
    //! @brief Calculate a texture area that fits with the default way to do it in Vulkan
    inline constexpr NativeTextureArea FastCalcNativeTextureArea(const PxRectangle& imageRectanglePx, const PxSize2D textureSizePx) noexcept
    {
      return {
        imageRectanglePx.Left() == PxValue(0) ? 0.0f : static_cast<float>(imageRectanglePx.RawLeft()) / static_cast<float>(textureSizePx.RawWidth()),
        imageRectanglePx.Top() == PxValue(0) ? 0.0f : static_cast<float>(imageRectanglePx.RawTop()) / static_cast<float>(textureSizePx.RawHeight()),
        imageRectanglePx.Right() >= textureSizePx.RawWidth()
          ? 1.0f
          : static_cast<float>(imageRectanglePx.RawRight()) / static_cast<float>(textureSizePx.RawWidth()),
        imageRectanglePx.Bottom() >= textureSizePx.RawWidth()
          ? 1.0f
          : static_cast<float>(imageRectanglePx.RawBottom()) / static_cast<float>(textureSizePx.RawHeight())};
    }

    //! @brief Calculate a texture area that fits with the default way to do it in OpenGL
    inline constexpr NativeTextureArea FastCalcNativeTextureAreaYFlipped(const PxRectangle& imageRectanglePx, const PxSize2D textureSizePx) noexcept
    {
      return {
        imageRectanglePx.Left() == PxValue(0) ? 0.0f : static_cast<float>(imageRectanglePx.RawLeft()) / static_cast<float>(textureSizePx.RawWidth()),
        1.0f - (imageRectanglePx.Top() == PxValue(0) ? 0.0f
                                                     : static_cast<float>(imageRectanglePx.RawTop()) / static_cast<float>(textureSizePx.RawHeight())),
        imageRectanglePx.Right() >= textureSizePx.Width()
          ? 1.0f
          : static_cast<float>(imageRectanglePx.RawRight()) / static_cast<float>(textureSizePx.RawWidth()),
        1.0f - (imageRectanglePx.Bottom() >= textureSizePx.Height()
                  ? 1.0f
                  : static_cast<float>(imageRectanglePx.RawBottom()) / static_cast<float>(textureSizePx.RawHeight()))};
    }

    //! @brief Calculate a texture area that fits with the default way to do it in Vulkan
    inline constexpr NativeTextureArea CalcNativeTextureArea(const PxRectangle& imageRectanglePx, const PxSize2D textureSizePx)
    {
      if (imageRectanglePx.Right() > textureSizePx.Width() || imageRectanglePx.Bottom() > textureSizePx.Height())
      {
        throw std::invalid_argument("the supplied imageRectangle does not fit inside the given extent");
      }
      return FastCalcNativeTextureArea(imageRectanglePx, textureSizePx);
    }

    inline constexpr NativeTextureArea CalcNativeTextureAreaYFlipped(const PxRectangle& imageRectanglePx, const PxSize2D textureSizePx)
    {
      if (imageRectanglePx.Right() > textureSizePx.Width() || imageRectanglePx.Bottom() > textureSizePx.Height())
      {
        throw std::invalid_argument("the supplied imageRectangle does not fit inside the given extent");
      }
      return FastCalcNativeTextureAreaYFlipped(imageRectanglePx, textureSizePx);
    }

    inline constexpr NativeTextureArea CalcNativeTextureAreaClipped(const PxRectangle& imageRectanglePx, const PxSize2D textureSizePx)
    {
      PxRectangle clippedImageRectanglePx(PxRectangle::Intersect(imageRectanglePx, PxRectangle(PxPoint2(), textureSizePx)));
      return FastCalcNativeTextureArea(clippedImageRectanglePx, textureSizePx);
    }

    inline constexpr NativeTextureArea CalcNativeTextureAreaYFlippedClipped(const PxRectangle& imageRectanglePx, const PxSize2D textureSizePx)
    {
      PxRectangle clippedImageRectanglePx(PxRectangle::Intersect(imageRectanglePx, PxRectangle(PxPoint2(), textureSizePx)));
      return FastCalcNativeTextureAreaYFlipped(clippedImageRectanglePx, textureSizePx);
    }
  };
}

#endif
