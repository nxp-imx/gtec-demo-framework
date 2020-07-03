#ifndef FSLGRAPHICS_TEXTUREATLAS_TESTATLASTEXTUREGENERATOR_HPP
#define FSLGRAPHICS_TEXTUREATLAS_TESTATLASTEXTUREGENERATOR_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>

namespace Fsl
{
  class ITextureAtlas;

  namespace TestAtlasTextureGenerator
  {
    extern void PatchWithTestPattern(Bitmap& rBitmap);
    extern void PatchWithTestPattern(Bitmap& rBitmap, const ITextureAtlas& sourceAtlas);
    extern void PatchWithTestPattern(Texture& rTexture);
    extern void PatchWithTestPattern(Texture& rTexture, const ITextureAtlas& sourceAtlas);

    extern Bitmap CreateTestPatternBitmap(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin,
                                          const ITextureAtlas& sourceAtlas);
    extern Bitmap CreateTestPatternBitmap(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);
    extern Texture CreateTestPatternTexture(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin,
                                            const ITextureAtlas& sourceAtlas);
    extern Texture CreateTestPatternTexture(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);
  }
}


#endif
