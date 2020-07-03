#ifndef FSLUTIL_VULKAN1_0_VUTEXTUREUTIL_HPP
#define FSLUTIL_VULKAN1_0_VUTEXTUREUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
#include <FslBase/Math/Pixel/PxRectangleU.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    namespace VUTextureUtil
    {
      //! @brief Get the native texture area of the given textureRectangle
      constexpr inline NativeTextureArea CalcTextureArea(const PxRectangleU& srcRect, const PxExtent2D& textureSize)
      {
        assert(static_cast<float>(textureSize.Width) >= 0.0f);
        assert(static_cast<float>(textureSize.Height) >= 0.0f);
        return {srcRect.Left() == 0 ? 0.0f : srcRect.Left() / static_cast<float>(textureSize.Width),
                srcRect.Top() == 0 ? 0.0f : srcRect.Top() / static_cast<float>(textureSize.Height),
                srcRect.Right() == textureSize.Width ? 1.0f : srcRect.Right() / static_cast<float>(textureSize.Width),
                srcRect.Bottom() == textureSize.Height ? 1.0f : srcRect.Bottom() / static_cast<float>(textureSize.Height)};
      }

      //! @brief Get the native texture area of the given textureRectangle
      constexpr inline NativeTextureArea CalcTextureArea(const PxRectangleU& srcRect, const PxSize2D& textureSize)
      {
        assert(static_cast<float>(textureSize.Width()) >= 0.0f);
        assert(static_cast<float>(textureSize.Height()) >= 0.0f);
        return {
          srcRect.Left() == 0 ? 0.0f : srcRect.Left() / static_cast<float>(textureSize.Width()),
          srcRect.Top() == 0 ? 0.0f : srcRect.Top() / static_cast<float>(textureSize.Height()),
          srcRect.Right() == UncheckedNumericCast<uint32_t>(textureSize.Width()) ? 1.0f : srcRect.Right() / static_cast<float>(textureSize.Width()),
          srcRect.Bottom() == UncheckedNumericCast<uint32_t>(textureSize.Height()) ? 1.0f
                                                                                   : srcRect.Bottom() / static_cast<float>(textureSize.Height())};
      }

      //! @brief Get the native texture area of the given textureRectangle
      constexpr inline NativeTextureArea CalcTextureArea(const PxRectangle& srcRect, const PxSize2D& textureSize)
      {
        assert(static_cast<float>(textureSize.Width()) >= 0.0f);
        assert(static_cast<float>(textureSize.Height()) >= 0.0f);
        return {srcRect.Left() == 0 ? 0.0f : srcRect.Left() / static_cast<float>(textureSize.Width()),
                srcRect.Top() == 0 ? 0.0f : srcRect.Top() / static_cast<float>(textureSize.Height()),
                srcRect.Right() == textureSize.Width() ? 1.0f : srcRect.Right() / static_cast<float>(textureSize.Width()),
                srcRect.Bottom() == textureSize.Height() ? 1.0f : srcRect.Bottom() / static_cast<float>(textureSize.Height())};
      }

      //! @brief Get the native texture area of the given textureRectangle
      constexpr inline NativeTextureArea CalcTextureArea(const PxRectangle& srcRect, const PxSize2D& textureSize, const int timesX, const int timesY)
      {
        assert(timesX == 1 || (timesX != 1 && (srcRect.X() == 0 || srcRect.Width() == textureSize.Width())));
        assert(timesY == 1 || (timesY != 1 && (srcRect.Y() == 0 || srcRect.Height() == textureSize.Height())));

        assert(static_cast<float>(textureSize.Width()) >= 0.0f);
        assert(static_cast<float>(textureSize.Height()) >= 0.0f);
        return {srcRect.Left() == 0 ? 0.0f : srcRect.Left() / static_cast<float>(textureSize.Width()),
                srcRect.Top() == 0 ? 0.0f : srcRect.Top() / static_cast<float>(textureSize.Height()),
                srcRect.Right() == textureSize.Width() ? float(timesX) : srcRect.Right() / static_cast<float>(textureSize.Width()),
                srcRect.Bottom() == textureSize.Height() ? float(timesY) : srcRect.Bottom() / static_cast<float>(textureSize.Height())};
      }


      //! @brief Get the native texture area of the given textureRectangle
      constexpr inline NativeTextureArea CalcTextureArea(const TextureRectangle& textureRectangle)
      {
        return CalcTextureArea(textureRectangle.GetRectangle(), textureRectangle.GetTextureSize());
      }

      //! @brief Get the native texture area of the given textureRectangle
      constexpr inline NativeTextureArea CalcTextureArea(const TextureRectangle& textureRectangle, const int timesX, const int timesY)
      {
        const PxRectangle srcRect = textureRectangle.GetRectangle();
        const PxSize2D textureSize = textureRectangle.GetTextureSize();
        return CalcTextureArea(srcRect, textureSize, timesX, timesY);
      }
    }
  }
}

#endif
