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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsTexture.hpp>
#include <utility>

namespace Fsl::GLES2
{
  namespace
  {
    inline GLTextureParameters Convert(const Texture2DFilterHint filterHint)
    {
      const GLenum filter = (filterHint == Texture2DFilterHint::Nearest ? GL_NEAREST : GL_LINEAR);
      return {filter, filter, GL_REPEAT, GL_REPEAT};
    }
  }

  NativeGraphicsTexture::NativeGraphicsTexture(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    : m_extentPx(texture.GetExtent())
    , m_texture(texture, Convert(filterHint), textureFlags)
  {
  }


  void NativeGraphicsTexture::Destroy()
  {
    if (!m_texture.IsValid())
    {
      return;
    }
    m_texture.Reset();
  }

  void NativeGraphicsTexture::SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
  {
    auto params = Convert(filterHint);
    m_texture.SetData(texture, params, textureFlags);
  }

}
