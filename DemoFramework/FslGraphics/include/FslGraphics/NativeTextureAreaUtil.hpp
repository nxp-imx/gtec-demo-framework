#ifndef FSLGRAPHICS_NATIVETEXTUREAREAUTIL_HPP
#define FSLGRAPHICS_NATIVETEXTUREAREAUTIL_HPP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <stdexcept>

namespace Fsl
{
  namespace NativeTextureAreaUtil
  {
    //! @brief Calculate a texture area that fits with the default way to do it in Vulkan
    inline constexpr NativeTextureArea FastCalcNativeTextureArea(const PxRectangleU32& imageRectanglePx, const PxExtent2D textureExtentPx) noexcept
    {
      return {imageRectanglePx.Left() == 0 ? 0.0f : imageRectanglePx.Left() / static_cast<float>(textureExtentPx.Width),
              imageRectanglePx.Top() == 0 ? 0.0f : imageRectanglePx.Top() / static_cast<float>(textureExtentPx.Height),
              imageRectanglePx.Right() >= textureExtentPx.Width ? 1.0f : imageRectanglePx.Right() / static_cast<float>(textureExtentPx.Width),
              imageRectanglePx.Bottom() >= textureExtentPx.Height ? 1.0f : imageRectanglePx.Bottom() / static_cast<float>(textureExtentPx.Height)};
    }

    //! @brief Calculate a texture area that fits with the default way to do it in OpenGL
    inline constexpr NativeTextureArea FastCalcNativeTextureAreaYFlipped(const PxRectangleU32& imageRectanglePx,
                                                                         const PxExtent2D textureExtentPx) noexcept
    {
      return {
        imageRectanglePx.Left() == 0 ? 0.0f : imageRectanglePx.Left() / static_cast<float>(textureExtentPx.Width),
        1.0f - (imageRectanglePx.Top() == 0 ? 0.0f : imageRectanglePx.Top() / static_cast<float>(textureExtentPx.Height)),
        imageRectanglePx.Right() >= textureExtentPx.Width ? 1.0f : imageRectanglePx.Right() / static_cast<float>(textureExtentPx.Width),
        1.0f - (imageRectanglePx.Bottom() >= textureExtentPx.Height ? 1.0f : imageRectanglePx.Bottom() / static_cast<float>(textureExtentPx.Height))};
    }

    //! @brief Calculate a texture area that fits with the default way to do it in Vulkan
    inline constexpr NativeTextureArea CalcNativeTextureArea(const PxRectangleU32& imageRectanglePx, const PxExtent2D textureExtentPx)
    {
      if (imageRectanglePx.Right() > textureExtentPx.Width || imageRectanglePx.Bottom() > textureExtentPx.Height)
      {
        throw std::invalid_argument("the supplied imageRectangle does not fit inside the given extent");
      }
      return FastCalcNativeTextureArea(imageRectanglePx, textureExtentPx);
    }

    //! @brief Calculate a texture area that fits with the default way to do it in OpenGL
    inline constexpr NativeTextureArea CalcNativeTextureAreaYFlipped(const PxRectangleU32& imageRectanglePx, const PxExtent2D textureExtentPx)
    {
      if (imageRectanglePx.Right() > textureExtentPx.Width || imageRectanglePx.Bottom() > textureExtentPx.Height)
      {
        throw std::invalid_argument("the supplied imageRectangle does not fit inside the given extent");
      }
      return FastCalcNativeTextureAreaYFlipped(imageRectanglePx, textureExtentPx);
    }
  };
}

#endif
