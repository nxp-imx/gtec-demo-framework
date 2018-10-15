#ifndef FSLUTIL_OPENGLES3_GLTEXTUREPARAMETERS3_HPP
#define FSLUTIL_OPENGLES3_GLTEXTUREPARAMETERS3_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/OpenGLES3/Common.hpp>
#include <FslUtil/OpenGLES3/GLTextureParameters.hpp>
#include <FslBase/Attributes.hpp>
#include <GLES3/gl3.h>
#include <cassert>

namespace Fsl
{
  namespace GLES3
  {
    // Texture filter wrapping
    struct GLTextureParameters3
    {
      GLenum MinFilter{GL_NEAREST};
      GLenum MagFilter{GL_NEAREST};
      GLenum WrapS{GL_REPEAT};
      GLenum WrapT{GL_REPEAT};
      GLenum WrapR{GL_REPEAT};

      //! @brief Set it to the default values -> MinFilter: GL_NEAREST, MagFilter = GL_NEAREST, WrapS = GL_REPEAT, WrapT = GL_REPEAT, WrapR =
      //! GL_REPEAT
      GLTextureParameters3() = default;

      //! @brief Set it to the default values -> MinFilter: GL_NEAREST, MagFilter = GL_NEAREST, WrapS = GL_REPEAT, WrapT = GL_REPEAT, WrapR =
      //! GL_REPEAT
      FSL_ATTR_DEPRECATED GLTextureParameters3(const GLTextureParameters& value)
        : MinFilter(value.MinFilter)
        , MagFilter(value.MagFilter)
        , WrapS(value.WrapS)
        , WrapT(value.WrapT)
        , WrapR(GL_REPEAT)
      {
      }

      GLTextureParameters3(const GLenum minFilter, const GLenum magFilter, const GLenum wrapS, const GLenum wrapT, const GLenum wrapR)
        : MinFilter(minFilter)
        , MagFilter(magFilter)
        , WrapS(wrapS)
        , WrapT(wrapT)
        , WrapR(wrapR)
      {
        assert(IsValid());
      }

      //! @brief Check if the content is considered valid
      bool IsValid() const;
    };
  }
}

#endif
