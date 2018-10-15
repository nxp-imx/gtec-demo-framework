/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

// http://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml

#include <FslUtil/OpenGLES2/GLTextureParameters.hpp>
#include <cassert>

namespace Fsl
{
  namespace GLES2
  {
    bool GLTextureParameters::IsValid() const
    {
      const bool b1 = (MinFilter == GL_NEAREST || MinFilter == GL_LINEAR || MinFilter == GL_NEAREST_MIPMAP_NEAREST ||
                       MinFilter == GL_LINEAR_MIPMAP_NEAREST || MinFilter == GL_NEAREST_MIPMAP_LINEAR || MinFilter == GL_LINEAR_MIPMAP_LINEAR);
      const bool b2 = (MagFilter == GL_NEAREST || MagFilter == GL_LINEAR);
      const bool b3 = (WrapS == GL_CLAMP_TO_EDGE || WrapS == GL_MIRRORED_REPEAT || WrapS == GL_REPEAT || WrapS == GL_CLAMP_TO_EDGE);
      const bool b4 = (WrapT == GL_CLAMP_TO_EDGE || WrapT == GL_MIRRORED_REPEAT || WrapT == GL_REPEAT || WrapT == GL_CLAMP_TO_EDGE);
      return b1 && b2 && b3 && b4;
    }
  }
}
