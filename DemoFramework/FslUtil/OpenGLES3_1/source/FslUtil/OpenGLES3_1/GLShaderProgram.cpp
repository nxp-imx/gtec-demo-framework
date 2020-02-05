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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3_1/GLShaderProgram.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

namespace Fsl
{
  namespace GLES3
  {
    namespace
    {
      void DumpDebugInformation(const GLuint hProgram, const std::string& strShaderCode)
      {
        FSLLOG3_INFO("*** Source start ***\n{}\n*** Source end ***\n\n", strShaderCode);

        GLint errorBufSize, errorLength;
        glGetProgramiv(hProgram, GL_INFO_LOG_LENGTH, &errorBufSize);

        std::vector<char> errorLog(std::max(errorBufSize, 1));
        errorLog[0] = 0;

        glGetProgramInfoLog(hProgram, static_cast<GLsizei>(errorLog.size()), &errorLength, &errorLog[0]);

        // GetShaderStageName(target)

        FSLLOG3_INFO(
          "*** GLShaderProgram: Error log start ***\n{}\n*** GLShaderProgram: Error Log End ***\n(If the log is empty try compiling with "
          "GLShader)\n\n",
          &errorLog[0]);
      }
    }


    GLShaderProgram::GLShaderProgram()
      : m_shaderType(0)
      , m_handle(GLValues::INVALID_HANDLE)
    {
    }


    GLShaderProgram::GLShaderProgram(const GLint shaderType, const std::string& strShaderCode)
      : m_shaderType(shaderType)
      , m_handle(GLValues::INVALID_HANDLE)
    {
      Reset(shaderType, strShaderCode);
    }


    GLShaderProgram::~GLShaderProgram()
    {
      Reset();
    }


    void GLShaderProgram::Reset() noexcept
    {
      if (m_handle != GLValues::INVALID_HANDLE)
      {
        glDeleteProgram(m_handle);
        m_handle = GLValues::INVALID_HANDLE;
      }
    }


    void GLShaderProgram::Reset(const GLint shaderType, const std::string& strShaderCode)
    {
      Reset();
      m_shaderType = shaderType;

      const char* shaderCode[1] = {strShaderCode.c_str()};

      // Create the new shader of the given type
      m_handle = glCreateShaderProgramv(shaderType, 1, shaderCode);
      shaderCode[0] = nullptr;
      GLenum glError = glGetError();
      if (m_handle == 0 || glError != GL_NO_ERROR)
      {
        throw GLESGraphicsException("Failed to create shader", glError, __FILE__, __LINE__);
      }

      GLint status = GL_FALSE;
      GL_CHECK(glGetProgramiv(m_handle, GL_LINK_STATUS, &status));
      // Check if something went wrong
      if (status != GL_TRUE)
      {
        // and verbose is enabled then dump some debug information
        // if (m_verbose)
        DumpDebugInformation(m_handle, strShaderCode);

        glDeleteProgram(m_handle);
        m_handle = 0;

        throw GLESGraphicsException("Shader compilation failed", 0, __FILE__, __LINE__);
      }
    }
  }
}
