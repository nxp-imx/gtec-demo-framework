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

#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsTextureFactory.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsTexture.hpp>
#include <utility>

namespace Fsl
{
  namespace GLES2
  {
    NativeGraphicsTextureFactory::NativeGraphicsTextureFactory()
    {
      FSLLOG3_VERBOSE3("NativeGraphicsTextureFactory::Construct");
    }

    NativeGraphicsTextureFactory::~NativeGraphicsTextureFactory() noexcept
    {
      FSLLOG3_VERBOSE3("NativeGraphicsTextureFactory::Destruct");
      Dispose();
    }


    Graphics3D::NativeTextureFactoryCaps NativeGraphicsTextureFactory::GetTextureCaps() const
    {
      return Graphics3D::NativeTextureFactoryCaps::Dynamic | Graphics3D::NativeTextureFactoryCaps::TextureCoordinatesFlipY;
    }

    void NativeGraphicsTextureFactory::Dispose() noexcept
    {
      if (m_isDisposed)
      {
        return;
      }
      FSLLOG3_VERBOSE3("NativeGraphicsTextureFactory::Dispose");
      m_isDisposed = true;
      if (m_textures.Count() > 0)
      {
        try
        {
          FSLLOG3_WARNING("NativeGraphicsTextureFactory: There are still {} allocated textures, force freeing them", m_textures.Count());
          m_textures.Clear();
        }
        catch (const std::exception& ex)
        {
          // This is the best we can do since this is not supposed to occur
          FSLLOG3_ERROR("NativeGraphicsTextureFactory consumed exception during force free: {}", ex.what());
        }
      }
    }


    BasicNativeTextureHandle NativeGraphicsTextureFactory::CreateTexture(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                         const TextureFlags textureFlags, const bool isDynamic)
    {
      FSLLOG3_VERBOSE6("NativeGraphicsTextureFactory::CreateTexture");
      if (m_isDisposed)
      {
        throw UsageErrorException("Trying to create texture after dispose");
      }
      if (isDynamic)
      {
        throw NotSupportedException("Dynamic textures are not supported");
      }

      auto handle = m_textures.Add(NativeGraphicsTexture(texture, filterHint, textureFlags));

      FSLLOG3_VERBOSE6("NativeGraphicsTextureFactory::CreateTexture handle: {}", handle);
      return BasicNativeTextureHandle(handle);
    }

    bool NativeGraphicsTextureFactory::DestroyTexture(const BasicNativeTextureHandle hTexture)
    {
      FSLLOG3_VERBOSE6("NativeGraphicsTextureFactory::DestroyTexture({})", hTexture.Value);
      if (m_isDisposed)
      {
        FSLLOG3_ERROR("hTexture is invalid, call ignored");
        return false;
      }
      if (!m_textures.Remove(hTexture.Value))
      {
        FSLLOG3_ERROR("tried to free a unknown texture, call ignored");
        return false;
      }
      return true;
    }

    void NativeGraphicsTextureFactory::SetTextureData(const BasicNativeTextureHandle hTexture, const RawTexture& texture,
                                                      const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    {
      FSLLOG3_VERBOSE6("NativeGraphicsTextureFactory::SetTextureData({})", hTexture.Value);
      if (m_isDisposed)
      {
        throw UsageErrorException("Trying to SetTextureData after dispose");
      }
      NativeGraphicsTexture& rTexture = m_textures.Get(hTexture.Value);
      rTexture.SetData(texture, filterHint, textureFlags);
    }


    const IBasicNativeTexture* NativeGraphicsTextureFactory::TryGetTexture(const BasicNativeTextureHandle hTexture) const
    {
      return !m_isDisposed ? m_textures.TryGet(hTexture.Value) : nullptr;
    }
  }
}
