#ifndef FSLUTIL_OPENGLES2_TEXTUREUTIL_HPP
#define FSLUTIL_OPENGLES2_TEXTUREUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <cassert>

namespace Fsl::GLES2::TextureUtil
{
  //! @brief Get the native texture area of the given textureRectangle
  constexpr inline NativeTextureArea CalcTextureArea(const PxRectangleU32& srcRect, const PxSize2D& textureSize)
  {
    return {srcRect.RawLeft() == 0u ? 0.0f : static_cast<float>(srcRect.RawLeft()) / static_cast<float>(textureSize.RawWidth()),
            1.0f - (srcRect.RawTop() == 0u ? 0.0f : static_cast<float>(srcRect.RawTop()) / static_cast<float>(textureSize.RawHeight())),
            srcRect.RawRight() == static_cast<uint32_t>(textureSize.RawWidth())
              ? 1.0f
              : static_cast<float>(srcRect.RawRight()) / static_cast<float>(textureSize.RawWidth()),
            1.0f - (srcRect.RawBottom() == static_cast<uint32_t>(textureSize.RawHeight())
                      ? 1.0f
                      : static_cast<float>(srcRect.RawBottom()) / static_cast<float>(textureSize.RawHeight()))};
  }

  //! @brief Get the native texture area of the given textureRectangle
  constexpr inline NativeTextureArea CalcTextureArea(const PxRectangle& srcRect, const PxSize2D& textureSize)
  {
    return {srcRect.RawLeft() == 0 ? 0.0f : static_cast<float>(srcRect.RawLeft()) / static_cast<float>(textureSize.RawWidth()),
            1.0f - (srcRect.RawTop() == 0 ? 0.0f : static_cast<float>(srcRect.RawTop()) / static_cast<float>(textureSize.RawHeight())),
            srcRect.RawRight() == textureSize.RawWidth() ? 1.0f : static_cast<float>(srcRect.RawRight()) / static_cast<float>(textureSize.RawWidth()),
            1.0f - (srcRect.RawBottom() == textureSize.RawHeight()
                      ? 1.0f
                      : static_cast<float>(srcRect.RawBottom()) / static_cast<float>(textureSize.RawHeight()))};
  }

  //! @brief Get the native texture area of the given textureRectangle
  constexpr inline NativeTextureArea CalcTextureArea(const PxRectangle& srcRect, const PxSize2D& textureSize, const int timesX, const int timesY)
  {
    assert(timesX == 1 || (timesX != 1 && (srcRect.RawWidth() == 0 || srcRect.Width() == textureSize.Width())));
    assert(timesY == 1 || (timesY != 1 && (srcRect.RawHeight() == 0 || srcRect.Height() == textureSize.Height())));

    assert(static_cast<float>(textureSize.RawWidth()) >= 0.0f);
    assert(static_cast<float>(textureSize.RawHeight()) >= 0.0f);
    return {srcRect.RawLeft() == 0 ? 0.0f : static_cast<float>(srcRect.RawLeft()) / static_cast<float>(textureSize.RawWidth()),
            1.0f - (srcRect.RawTop() == 0 ? 0.0f : static_cast<float>(srcRect.RawTop()) / static_cast<float>(textureSize.RawHeight())),
            srcRect.RawRight() == textureSize.RawWidth() ? 1.0f * static_cast<float>(timesX)
                                                         : static_cast<float>(srcRect.RawRight()) / static_cast<float>(textureSize.RawWidth()),
            1.0f - (srcRect.RawBottom() == textureSize.RawHeight()
                      ? 1.0f * static_cast<float>(timesY)
                      : static_cast<float>(srcRect.RawBottom()) / static_cast<float>(textureSize.RawHeight()))};
  }


  //! @brief Get the native texture area of the given textureRectangle
  constexpr inline NativeTextureArea CalcTextureArea(const TextureRectangle& textureRectangle)
  {
    return CalcTextureArea(textureRectangle.GetRectangle(), textureRectangle.GetTextureSize());
  }

  //! @brief Get the native texture area of the given textureRectangle
  constexpr inline NativeTextureArea CalcTextureArea(const TextureRectangle& textureRectangle, const int timesX, const int timesY)
  {
    return CalcTextureArea(textureRectangle.GetRectangle(), textureRectangle.GetTextureSize(), timesX, timesY);
  }
}

#endif
