#ifndef FSLUTIL_OPENGLES2_GLPROGRAM_HPP
#define FSLUTIL_OPENGLES2_GLPROGRAM_HPP
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
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslUtil/OpenGLES2/Common.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLBindAttribLocation.hpp>
#include <FslUtil/OpenGLES2/GLShader.hpp>
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <GLES2/gl2.h>
#include <string>

namespace Fsl::GLES2
{
  class GLProgram
  {
    GLuint m_handle;

  public:
    GLProgram(const GLProgram&) = delete;
    GLProgram& operator=(const GLProgram&) = delete;

    //! @brief Move assignment operator
    GLProgram& operator=(GLProgram&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        m_handle = other.m_handle;

        // Remove the data from other
        other.m_handle = GLValues::InvalidHandle;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    GLProgram(GLProgram&& other) noexcept
      : m_handle(other.m_handle)
    {
      // Remove the data from other
      other.m_handle = GLValues::InvalidHandle;
    }

    //! @brief Create a uninitialized program, use Reset to configure it
    GLProgram();

    //! @brief Let this GLProgram object assume control over the given program handle.
    explicit GLProgram(const GLuint handle);

    //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
    GLProgram(const GLShader& vertexShader, const GLShader& fragmentShader);

    //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
    GLProgram(const GLShader& vertexShader, const GLShader& fragmentShader, const ReadOnlySpan<GLBindAttribLocation> attributes);

    //! @brief Create a program from the supplied vertex, fragment shader.
    GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader);

    //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
    GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const ReadOnlySpan<GLBindAttribLocation> attributes);

    ~GLProgram();

    //! @brief Check if this contains a valid gl handle.
    bool IsValid() const noexcept
    {
      return m_handle != GLValues::InvalidHandle;
    }

    //! @brief Reset to a uninitialized state.
    void Reset() noexcept;

    //! @brief Let this GLProgram object assume control over the given program handle.
    void Reset(const GLuint handle);

    //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
    void Reset(const GLShader& vertexShader, const GLShader& fragmentShader);

    //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
    void Reset(const GLShader& vertexShader, const GLShader& fragmentShader, const ReadOnlySpan<GLBindAttribLocation> attributes);

    //! @brief Create a program from the supplied vertex, fragment shader.
    void Reset(const std::string& strVertexShader, const std::string& strFragmentShader);

    //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
    void Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const ReadOnlySpan<GLBindAttribLocation> attributes);

    //! @brief Get the handle to the shader program
    GLuint Get() const noexcept
    {
      return m_handle;
    }

    //! @brief Get the uniform location or throw a exception if not found
    GLint GetUniformLocation(const char* const pszName) const
    {
      const auto result = glGetUniformLocation(m_handle, pszName);
      if (result != -1)
      {
        return result;
      }
      throw GraphicsException(std::string("Failed to get uniform location: ") + (pszName != nullptr ? pszName : "nullptr"));
    }

    //! @brief Try to get the uniform location
    GLint TryGetUniformLocation(const char* const pszName) const noexcept
    {
      return glGetUniformLocation(m_handle, pszName);
    }


    GLint GetAttribLocation(const char* const pszName) const
    {
      const auto result = glGetAttribLocation(m_handle, pszName);
      if (result != -1)
      {
        return result;
      }
      throw GraphicsException(std::string("Failed to get attrib location: ") + (pszName != nullptr ? pszName : "nullptr"));
    }


    GLint TryGetAttribLocation(const char* const pszName) const noexcept
    {
      return glGetAttribLocation(m_handle, pszName);
    }
  };
}

#endif
