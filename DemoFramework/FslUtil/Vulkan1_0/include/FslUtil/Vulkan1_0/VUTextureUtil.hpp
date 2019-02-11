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

#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    namespace VUTextureUtil
    {
      static NativeTextureArea CalcTextureArea(const TextureRectangle& textureRectangle, const int timesX, const int timesY)
      {
        const Rectangle srcRect = textureRectangle.GetRectangle();
        const Point2 sizeTex = textureRectangle.GetTextureSize();

        assert(timesX == 1 || (timesX != 1 && (srcRect.X() == 0 || srcRect.Width() == sizeTex.X)));
        assert(timesY == 1 || (timesY != 1 && (srcRect.Y() == 0 || srcRect.Height() == sizeTex.Y)));

        assert(static_cast<float>(sizeTex.X) >= 0.0f);
        assert(static_cast<float>(sizeTex.Y) >= 0.0f);
        return NativeTextureArea(srcRect.Left() == 0 ? 0.0f : srcRect.Left() / static_cast<float>(sizeTex.X),
                                 srcRect.Top() == 0 ? 0.0f : srcRect.Top() / static_cast<float>(sizeTex.Y),
                                 srcRect.Right() == sizeTex.X ? 1.0f * timesX : srcRect.Right() / static_cast<float>(sizeTex.X),
                                 srcRect.Bottom() == sizeTex.Y ? 1.0f * timesX : srcRect.Bottom() / static_cast<float>(sizeTex.Y));
      }
    }
  }
}

#endif
