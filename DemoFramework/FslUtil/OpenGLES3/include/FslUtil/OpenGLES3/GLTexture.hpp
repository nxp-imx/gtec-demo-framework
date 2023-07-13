#ifndef FSLUTIL_OPENGLES3_GLTEXTURE_HPP
#define FSLUTIL_OPENGLES3_GLTEXTURE_HPP
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
#include <FslBase/Attributes.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <FslUtil/OpenGLES3/Common.hpp>
#include <FslUtil/OpenGLES3/GLTextureInfo.hpp>
#include <FslUtil/OpenGLES3/GLTextureParameters.hpp>
#include <FslUtil/OpenGLES3/GLTextureParameters3.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  class Bitmap;
  class Texture;
  class RawBitmap;
  class RawCubeBitmap;
  class RawTexture;

  namespace GLES3
  {
    class GLTexture
    {
      GLuint m_handle;
      GLenum m_target{GL_TEXTURE_2D};
      PxExtent3D m_extent;

    public:
      GLTexture(const GLTexture&) = delete;
      GLTexture& operator=(const GLTexture&) = delete;

      // move assignment operator
      GLTexture& operator=(GLTexture&& other) noexcept;
      // move constructor
      GLTexture(GLTexture&& other) noexcept;


      //! @brief Create a uninitialized texture (use SetData to add texture data to it)
      GLTexture();

      //! @brief Let this GLTexture object assume control over the given texture handle.
      //! @param handle the GL handle to the texture
      //! @param the target type specified to glBindTexture when the handle was created.
      //! @param extent the extent of the texture
      GLTexture(const GLuint handle, const PxExtent2D& extent, const GLenum target = GL_TEXTURE_2D)
        : GLTexture(handle, PxExtent3D(extent, PxValueU(1u)), target)
      {
      }

      //! @brief Let this GLTexture object assume control over the given texture handle.
      //! @param handle the GL handle to the texture
      //! @param the target type specified to glBindTexture when the handle was created.
      //! @param extent the extent of the texture
      GLTexture(const GLuint handle, const PxExtent3D& extent, const GLenum target = GL_TEXTURE_2D);

      //! @brief Create a texture with the supplied content
      GLTexture(const Bitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Create a texture with the supplied content
      GLTexture(const RawBitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Create a texture with the supplied content
      GLTexture(const Bitmap& bitmapPosX, const Bitmap& bitmapNegX, const Bitmap& bitmapPosY, const Bitmap& bitmapNegY, const Bitmap& bitmapPosZ,
                const Bitmap& bitmapNegZ, const GLTextureParameters3& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Create a texture with the supplied content
      GLTexture(const RawBitmap& bitmapPosX, const RawBitmap& bitmapNegX, const RawBitmap& bitmapPosY, const RawBitmap& bitmapNegY,
                const RawBitmap& bitmapPosZ, const RawBitmap& bitmapNegZ, const GLTextureParameters3& textureParameters,
                const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Create a texture with the supplied content
      GLTexture(const RawCubeBitmap& cubeBitmap, const GLTextureParameters3& textureParameters,
                const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Create a texture with the supplied content
      GLTexture(const Texture& texture, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Create a texture with the supplied content
      GLTexture(const Texture& texture, const GLTextureParameters3& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Create a texture with the supplied content
      GLTexture(const RawTexture& texture, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Create a texture with the supplied content
      GLTexture(const RawTexture& texture, const GLTextureParameters3& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Let this GLTexture object assume control over the given texture handle.
      //! @param handle the GL handle to the texture
      //! @param the target type specified to glBindTexture when the handle was created.
      //! @param size the size of the texture
      //! @note  Legacy compatibility constructor
      GLTexture(const GLuint handle, const PxSize2D& size, const GLenum target = GL_TEXTURE_2D)
        : GLTexture(handle, PxExtent2D(size), target)
      {
      }

      ~GLTexture();

      //! @brief Check if this contains a valid gl handle.
      bool IsValid() const
      {
        return m_handle != GLValues::INVALID_HANDLE;
      }


      //! @brief If a texture is allocated this will releases it.
      void Reset() noexcept;

      //! @brief Let this GLTexture object assume control over the given texture handle.
      //! @param handle the GL handle to the texture
      //! @param the target type specified to glBindTexture when the handle was created.
      //! @param extent the extent of the texture
      void Reset(const GLuint handle, const PxExtent2D& extent, const GLenum target = GL_TEXTURE_2D)
      {
        Reset(handle, PxExtent3D(extent, PxValueU(1)), target);
      }

      //! @brief Let this GLTexture object assume control over the given texture handle.
      //! @param handle the GL handle to the texture
      //! @param the target type specified to glBindTexture when the handle was created.
      //! @param extent the extent of the texture
      void Reset(const GLuint handle, const PxExtent3D& extent, const GLenum target = GL_TEXTURE_2D);

      //! @brief Reset the data of the texture
      void Reset(const Bitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Reset the data of the texture
      void Reset(const RawBitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Reset the data of the texture
      void Reset(const Bitmap& bitmapPosX, const Bitmap& bitmapNegX, const Bitmap& bitmapPosY, const Bitmap& bitmapNegY, const Bitmap& bitmapPosZ,
                 const Bitmap& bitmapNegZ, const GLTextureParameters3& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Reset the data of the texture
      void Reset(const RawBitmap& bitmapPosX, const RawBitmap& bitmapNegX, const RawBitmap& bitmapPosY, const RawBitmap& bitmapNegY,
                 const RawBitmap& bitmapPosZ, const RawBitmap& bitmapNegZ, const GLTextureParameters3& textureParameters,
                 const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Reset the data of the texture
      void Reset(const RawCubeBitmap& cubeBitmap, const GLTextureParameters3& textureParameters,
                 const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Reset the data of the texture
      void Reset(const Texture& texture, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Reset the data of the texture
      void Reset(const Texture& texture, const GLTextureParameters3& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Reset the data of the texture
      void Reset(const RawTexture& texture, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Reset the data of the texture
      void Reset(const RawTexture& texture, const GLTextureParameters3& textureParameters,
                 const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Let this GLTexture object assume control over the given texture handle.
      //! @param handle the GL handle to the texture
      //! @param the target type specified to glBindTexture when the handle was created.
      //! @param size the size of the texture
      //! @note  Legacy compatibility Reset
      void Reset(const GLuint handle, const PxSize2D& size, const GLenum target = GL_TEXTURE_2D)
      {
        Reset(handle, PxExtent2D(size), target);
      }

      //! @brief Set the data of the texture
      void SetData(const Bitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Set the data of the texture
      void SetData(const RawBitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Set the data of the texture
      void SetData(const Bitmap& bitmapPosX, const Bitmap& bitmapNegX, const Bitmap& bitmapPosY, const Bitmap& bitmapNegY, const Bitmap& bitmapPosZ,
                   const Bitmap& bitmapNegZ, const GLTextureParameters3& textureParameters,
                   const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Set the data of the texture
      void SetData(const RawBitmap& bitmapPosX, const RawBitmap& bitmapNegX, const RawBitmap& bitmapPosY, const RawBitmap& bitmapNegY,
                   const RawBitmap& bitmapPosZ, const RawBitmap& bitmapNegZ, const GLTextureParameters3& textureParameters,
                   const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Set the data of the texture
      void SetData(const RawCubeBitmap& cubeBitmap, const GLTextureParameters3& textureParameters,
                   const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Set the data of the texture
      void SetData(const Texture& texture, const GLTextureParameters& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Set the data of the texture
      void SetData(const Texture& texture, const GLTextureParameters3& textureParameters, const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Set the data of the texture
      void SetData(const RawTexture& texture, const GLTextureParameters& textureParameters,
                   const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Set the data of the texture
      void SetData(const RawTexture& texture, const GLTextureParameters3& textureParameters,
                   const TextureFlags textureFlags = TextureFlags::NotDefined);

      //! @brief Get the gl texture handle associated with the texture.
      //! @return the handle or GLValues::INVALID_HANDLE if the texture is unallocated.
      GLuint Get() const
      {
        return m_handle;
      }

      //! @brief Get the gl texture handle associated with the texture.
      //! @return the handle or GLValues::INVALID_HANDLE if the texture is unallocated.
      //! @deprecated use Get instead
      [[deprecated("use one of the other overloads instead")]] GLuint GetHandle() const
      {
        return m_handle;
      }

      //! @brief Get the gl texture target associated with the texture.
      //! @return the handle or GLValues::INVALID_HANDLE if the texture is unallocated.
      GLuint GetTarget() const
      {
        return m_target;
      }

      //! @brief Get the texture size.
      PxExtent3D GetExtent() const
      {
        return m_extent;
      }

      //! @brief Extract information about this texture as a GLTextureInfo struct
      GLTextureInfo GetTextureInfo() const
      {
        return {m_handle, m_extent};
      }

      //! @brief Extract information about this texture as a GLTextureInfo struct
      operator GLTextureInfo() const    // NOLINT(google-explicit-constructor)
      {
        return {m_handle, m_extent};
      }

      PxSize2D GetSize() const;
    };
  }
}

#endif
