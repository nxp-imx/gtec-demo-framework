#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSTEXTURE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSTEXTURE_HPP
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
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeTexture.hpp>
#include <FslUtil/OpenGLES2/GLTexture.hpp>
#include <cassert>

namespace Fsl::GLES2
{
  class NativeGraphicsTexture final : public Graphics3D::INativeTexture
  {
    PxExtent3D m_extentPx{};
    GLTexture m_texture;

  public:
    NativeGraphicsTexture() = default;
    NativeGraphicsTexture(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags);

    void Destroy();

    bool IsValid() const
    {
      return m_texture.IsValid();
    }

    inline GLTextureInfo ToTextureInfo() const
    {
      assert(IsValid());
      return {Get(), PxExtent3D{m_extentPx.Width, m_extentPx.Height, m_extentPx.Depth}};
    }

    //! @brief Get the native texture handle
    GLuint Get() const
    {
      assert(IsValid());
      return m_texture.Get();
    }


    // PxSize2D GetSize() const
    //{
    //  return m_sizePx;
    //}

    // NativeTextureArea CalcNativeTextureArea(const PxRectangleU32& imageRectanglePx) const final
    //{
    //  return TextureUtil::CalcTextureArea(imageRectanglePx, m_sizePx);
    //}

    // Graphics3D::INativeTexture
    void SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags) final;
  };
}

#endif
