#ifndef FSLGRAPHICS3D_BASICRENDER_TEXTURE_BASICSTATICTEXTURETRACKER_HPP
#define FSLGRAPHICS3D_BASICRENDER_TEXTURE_BASICSTATICTEXTURETRACKER_HPP
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

#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeTextureHandle.hpp>
#include <FslGraphics/Render/Basic/Texture/BasicTextureHandle.hpp>
#include <FslGraphics3D/BasicRender/Texture/ABasicTextureTracker.hpp>
#include <stdexcept>
#include <utility>

namespace Fsl::Graphics3D
{
  class BasicStaticTextureTracker final
    : public ABasicTextureTracker
    , public INativeTexture2D
  {
    BasicTextureHandle m_hTexture;
    BasicNativeTextureHandle m_handle;

  public:
    explicit BasicStaticTextureTracker(const BasicTextureHandle hTexture, const PxExtent3D& extentPx, const bool textureCoordinatesFlipY,
                                       const BasicNativeTextureHandle handle)
      : ABasicTextureTracker(extentPx, textureCoordinatesFlipY)
      , m_hTexture(hTexture)
      , m_handle(handle)
    {
      assert(m_hTexture.IsValid());
      if (!handle.IsValid())
      {
        throw std::invalid_argument("handle must be valid");
      }
    }

    ~BasicStaticTextureTracker() final = default;

    void Dispose() noexcept
    {
      m_hTexture = {};
      m_handle = {};
    }

    BasicNativeTextureHandle TryGetNativeHandle() const noexcept final
    {
      return m_handle;
    }

    NativeTextureArea CalcNativeTextureArea(const PxRectangleU32& imageRectanglePx) const final
    {
      return DoCalcNativeTextureArea(imageRectanglePx);
    }
  };
}

#endif
