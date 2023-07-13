#ifndef FSLUTIL_OPENGLES2_GLTEXTUREINFO_HPP
#define FSLUTIL_OPENGLES2_GLTEXTUREINFO_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslUtil/OpenGLES2/Common.hpp>
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <GLES2/gl2.h>

namespace Fsl
{
  class Bitmap;

  namespace GLES2
  {
    //! @brief A simple container for texture information.
    //! @note This is not a RAII object so you are responsible for managing the lifetime of the texture object.
    struct GLTextureInfo
    {
      GLuint Handle;
      PxExtent3D Extent;

      //! @brief Create a uninitialized texture (use SetData to add texture data to it)
      constexpr GLTextureInfo()
        : Handle(GLValues::INVALID_HANDLE)
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      constexpr GLTextureInfo(const GLuint handle, const PxSize2D& size)
        : GLTextureInfo(handle, PxExtent3D::Create(size.RawWidth(), size.RawHeight(), 1))
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      constexpr GLTextureInfo(const GLuint handle, const PxExtent2D& extent)
        : GLTextureInfo(handle, PxExtent3D(extent, PxValueU(1u)))
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      constexpr GLTextureInfo(const GLuint handle, const PxExtent3D& extent)
        : Handle(handle)
        , Extent(extent)
      {
      }

      constexpr void Reset()
      {
        Handle = GLValues::INVALID_HANDLE;
        Extent = PxExtent3D();
      }

      constexpr bool IsValid() const
      {
        return Handle != GLValues::INVALID_HANDLE;
      }

      constexpr bool operator==(const GLTextureInfo& rhs) const
      {
        return Handle == rhs.Handle && Extent == rhs.Extent;
      }
      constexpr bool operator!=(const GLTextureInfo& rhs) const
      {
        return !(*this == rhs);
      }
    };
  }
}

#endif
