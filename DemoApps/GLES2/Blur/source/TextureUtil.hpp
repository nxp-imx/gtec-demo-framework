#ifndef GLES2_BLUR_TEXTUREUTIL_HPP
#define GLES2_BLUR_TEXTUREUTIL_HPP
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
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsTexture.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <memory>

namespace Fsl::TextureUtil
{
  inline GLuint ToNative(const IBasicRenderSystem& renderSystem, const std::shared_ptr<INativeTexture2D>& tex)
  {
    if (!tex)
    {
      throw NotSupportedException("no native");
    }
    BasicNativeTextureHandle hTexture = tex->TryGetNativeHandle();
    if (!hTexture.IsValid())
    {
      throw NotSupportedException("There are currently no actual texture associated, call ignored");
    }
    const auto* pNativeTexture = dynamic_cast<const GLES2::NativeGraphicsTexture*>(renderSystem.TryGetNativeTexture(hTexture));
    if (pNativeTexture == nullptr)
    {
      throw NotSupportedException("texture not of expected type");
    }

    GLES2::GLTextureInfo textureInfo = pNativeTexture->ToTextureInfo();
    if (!textureInfo.IsValid())
    {
      throw UsageErrorException("the texture is not valid");
    }
    return textureInfo.Handle;
  }

}

#endif
