#ifndef FSLUTIL_OPENGLES3_GLMULTISAMPLEFRAMEBUFFERTEXTUREEXT_HPP
#define FSLUTIL_OPENGLES3_GLMULTISAMPLEFRAMEBUFFERTEXTUREEXT_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Attributes.hpp>
#include <FslUtil/OpenGLES3/Common.hpp>
#include <FslUtil/OpenGLES3/GLRenderBuffer.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLTextureImageParameters.hpp>

namespace Fsl::GLES3
{
  class GLMultisampleFrameBufferTextureExt
  {
    GLuint m_handle;
    PxSize2D m_size;
    GLTexture m_texture;
    GLRenderBuffer m_depthStencilRenderBuffer;

  public:
    GLMultisampleFrameBufferTextureExt(const GLMultisampleFrameBufferTextureExt&) = delete;
    GLMultisampleFrameBufferTextureExt& operator=(const GLMultisampleFrameBufferTextureExt&) = delete;

    // move assignment operator
    GLMultisampleFrameBufferTextureExt& operator=(GLMultisampleFrameBufferTextureExt&& other) noexcept;
    // move constructor
    GLMultisampleFrameBufferTextureExt(GLMultisampleFrameBufferTextureExt&& other) noexcept;

    //! @brief Create a uninitialized buffer
    GLMultisampleFrameBufferTextureExt();

    GLMultisampleFrameBufferTextureExt(const PxSize2D& size, const GLsizei maxSamples, const GLES3::GLTextureImageParameters texImageParams,
                                       const GLenum depthBufferFormat);

    ~GLMultisampleFrameBufferTextureExt();

    //! @brief If a buffer is allocated this will releases it.
    void Reset() noexcept;


    void Reset(const PxSize2D& size, const GLsizei maxSamples, const GLES3::GLTextureImageParameters texImageParams, const GLenum depthBufferFormat);

    //! @brief Check if this buffer contains a valid gl handle.
    bool IsValid() const noexcept
    {
      return m_handle != GLValues::InvalidHandle;
    }

    //! @brief Get the gl handle associated with the buffer.
    //! @return the handle or GLValues::InvalidHandle if the buffer is unallocated.
    GLuint Get() const noexcept
    {
      return m_handle;
    }

    //! @brief Get the size of the framebuffer
    PxSize2D GetSize() const noexcept
    {
      return m_size;
    }

    //! @brief Get the texture
    GLTextureInfo GetTextureInfo() const
    {
      return m_texture.GetTextureInfo();
    }

    //! @brief Extract information about this texture as a GLTextureInfo struct
    operator GLTextureInfo() const    // NOLINT(google-explicit-constructor)
    {
      return m_texture.GetTextureInfo();
    }
  };
}

#endif
