#ifndef FSLGRAPHICS3D_BASICRENDER_ADAPTER_INATIVETEXTURE_HPP
#define FSLGRAPHICS3D_BASICRENDER_ADAPTER_INATIVETEXTURE_HPP
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

#include <FslGraphics/Render/Basic/Adapter/IBasicNativeTexture.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/TextureFlags.hpp>

namespace Fsl
{
  class ReadOnlyRawTexture;

  namespace Graphics3D
  {
    /// <summary>
    /// Very basic native texture that assumes that the user will take 'Vulkan' like responsibility for the use of it.
    /// So you can only modify it if its dynamic, supported by the implementation and most importantely if no rendering is using it or scheduled to
    /// use it.
    /// </summary>
    class INativeTexture : public IBasicNativeTexture
    {
    public:
      //! @brief Set the data of the texture
      //! @note  This is only valid if the texture was created as a dynamic texture and if the texture factory capabilities reported
      //!        that SetData was supported.
      //!        WARNING: can only be called once during a frame, and its up to the caller to ensure that no rendering is currently using
      //!                 or scheduled to use the existing texture data.
      virtual void SetData(const ReadOnlyRawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags) = 0;
    };
  }
}

#endif
