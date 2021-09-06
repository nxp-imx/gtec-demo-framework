#ifndef FSLGRAPHICS3D_BASICRENDER_TEXTURE_ABASICTEXTURETRACKER_HPP
#define FSLGRAPHICS3D_BASICRENDER_TEXTURE_ABASICTEXTURETRACKER_HPP
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
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/NativeTextureAreaUtil.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    class INativeTexture;

    class ABasicTextureTracker
    {
      PxExtent3D m_extentPx;
      bool m_textureCoordinatesFlipY{false};

    public:
      constexpr ABasicTextureTracker() = default;
      constexpr explicit ABasicTextureTracker(const PxExtent3D& extentPx, const bool textureCoordinatesFlipY)
        : m_extentPx(extentPx)
        , m_textureCoordinatesFlipY(textureCoordinatesFlipY)
      {
      }

      virtual ~ABasicTextureTracker() = default;


      constexpr PxExtent2D GetExtent2D() const
      {
        return {m_extentPx.Width, m_extentPx.Height};
      }

      constexpr PxExtent3D GetExtent3D() const
      {
        return m_extentPx;
      }

    protected:
      NativeTextureArea DoCalcNativeTextureArea(const PxRectangleU32& imageRectanglePx) const
      {
        const auto extentPx = GetExtent2D();
        if (!m_textureCoordinatesFlipY)
        {
          return NativeTextureAreaUtil::CalcNativeTextureArea(imageRectanglePx, extentPx);
        }
        return NativeTextureAreaUtil::CalcNativeTextureAreaYFlipped(imageRectanglePx, extentPx);
      }
    };
  }
}

#endif
