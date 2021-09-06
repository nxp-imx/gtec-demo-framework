#ifndef FSLGRAPHICS3D_BASICRENDER_TEXTURE_BASICDYNAMICTEXTURELINK_HPP
#define FSLGRAPHICS3D_BASICRENDER_TEXTURE_BASICDYNAMICTEXTURELINK_HPP
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
#include <FslGraphics/Render/Basic/BasicRenderSystemEvent.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeTextureHandle.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeTexture.hpp>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  class RawTexture;
  class IBasicNativeTexture;

  namespace Graphics3D
  {
    class INativeTexture;
    class INativeTextureFactory;

    class BasicDynamicTextureLink final
    {
      struct Record
      {
        BasicNativeTextureHandle NativeHandle;
        uint32_t DeferCount{0};
        bool IsInUse{false};
        bool DeferredReuse{false};
      };

      std::shared_ptr<INativeTextureFactory> m_factory;
      std::vector<Record> m_textures;
      uint32_t m_activeIndex{0};
      bool m_setDataSupported{false};
      bool m_swapchainValid{true};
      bool m_isDestroyed{false};

    public:
      BasicDynamicTextureLink(const uint32_t maxFramesInFlight, std::shared_ptr<INativeTextureFactory> factory, const RawTexture& texture,
                              const Texture2DFilterHint filterHint, const TextureFlags textureFlags, const bool setDataSupported);
      ~BasicDynamicTextureLink();
      void Destroy();

      void OnRenderSystemEvent(const BasicRenderSystemEvent theEvent);

      void CollectGarbage();


      void SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags);

      //! @brief Try to get the native texture
      BasicNativeTextureHandle TryGetNativeHandle() const noexcept
      {
        return m_activeIndex < m_textures.size() ? m_textures[m_activeIndex].NativeHandle : BasicNativeTextureHandle();
      }

    private:
      static void SetData(Record& rRecord, INativeTextureFactory& textureFactory, const RawTexture& texture, const Texture2DFilterHint filterHint,
                          const TextureFlags textureFlags, const bool setDataSupported);
    };
  }
}

#endif
