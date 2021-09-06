#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSTEXTUREFACTORY_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSTEXTUREFACTORY_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsTexture.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeTextureHandle.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslGraphics3D/BasicRender/Adapter/NativeTextureFactoryCaps.hpp>
#include <FslUtil/OpenGLES2/GLTextureInfo.hpp>

namespace Fsl
{
  namespace GLES2
  {
    class NativeGraphicsTextureFactory
    {
      HandleVector<NativeGraphicsTexture> m_textures;
      bool m_isDisposed{false};

    public:
      explicit NativeGraphicsTextureFactory();
      ~NativeGraphicsTextureFactory() noexcept;

      void Dispose() noexcept;

      GLTextureInfo TryGetTextureInfo(const BasicNativeTextureHandle hTexture) const
      {
        const NativeGraphicsTexture* const pTexture = m_textures.TryGet(hTexture.Value);
        return pTexture != nullptr ? pTexture->ToTextureInfo() : GLTextureInfo();
      }

      Graphics3D::NativeTextureFactoryCaps GetTextureCaps() const;
      BasicNativeTextureHandle CreateTexture(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags,
                                             const bool isDynamic);
      bool DestroyTexture(const BasicNativeTextureHandle hTexture);

      void SetTextureData(const BasicNativeTextureHandle hTexture, const RawTexture& texture, const Texture2DFilterHint filterHint,
                          const TextureFlags textureFlags);

      const IBasicNativeTexture* TryGetTexture(const BasicNativeTextureHandle hTexture) const;
    };
  }
}

#endif
