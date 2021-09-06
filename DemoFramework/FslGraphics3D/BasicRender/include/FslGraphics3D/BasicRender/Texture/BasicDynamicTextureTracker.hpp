#ifndef FSLGRAPHICS3D_BASICRENDER_TEXTURE_BASICDYNAMICTEXTURETRACKER_HPP
#define FSLGRAPHICS3D_BASICRENDER_TEXTURE_BASICDYNAMICTEXTURETRACKER_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslGraphics/Render/Basic/Texture/BasicTextureHandle.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics3D/BasicRender/Texture/ABasicTextureTracker.hpp>
#include <FslGraphics3D/BasicRender/Texture/BasicDynamicTextureLink.hpp>
#include <memory>

namespace Fsl
{
  namespace Graphics3D
  {
    class BasicDynamicTextureTracker final
      : public ABasicTextureTracker
      , public IDynamicNativeTexture2D
    {
      BasicTextureHandle m_hTexture;
      std::shared_ptr<BasicDynamicTextureLink> m_link;

    public:
      explicit BasicDynamicTextureTracker(const BasicTextureHandle hTexture, const PxExtent3D& extentPx, const bool textureCoordinatesFlipY,
                                          std::shared_ptr<BasicDynamicTextureLink> link)
        : ABasicTextureTracker(extentPx, textureCoordinatesFlipY)
        , m_hTexture(hTexture)
        , m_link(std::move(link))
      {
        assert(m_hTexture.IsValid());
        if (!m_link)
        {
          throw std::invalid_argument("DynamicNativeTextureLink can not be null");
        }
      }

      ~BasicDynamicTextureTracker() final = default;


      void Dispose() noexcept
      {
        m_link.reset();
        m_hTexture = {};
      }

      BasicNativeTextureHandle TryGetNativeHandle() const noexcept final
      {
        BasicDynamicTextureLink* pLink = m_link.get();
        return pLink != nullptr ? pLink->TryGetNativeHandle() : BasicNativeTextureHandle();
      }


      NativeTextureArea CalcNativeTextureArea(const PxRectangleU32& imageRectanglePx) const final
      {
        return DoCalcNativeTextureArea(imageRectanglePx);
      }


      void SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags) final
      {
        BasicDynamicTextureLink* pLink = m_link.get();
        if (pLink == nullptr)
        {
          throw UsageErrorException("BasicDynamicTextureTracker has been marked as disposed");
        }
        m_link->SetData(texture, filterHint, textureFlags);
      }
    };
  }
}

#endif
