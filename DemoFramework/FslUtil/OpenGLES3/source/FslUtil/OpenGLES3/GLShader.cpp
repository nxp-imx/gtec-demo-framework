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

#include <FslBase/Log/Log.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLShader.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

namespace Fsl
{
  namespace GLES3
  {
    namespace
    {
      void DumpDebugInformation(const GLuint hShader)
      {
        // We utilize our own local error checking macro here that just logs the error and returns from this method on error since we dont
        // want to throw a exception in a debug function.

        // Get shader source.
        GLint length;
        GL_ON_ERROR_LOG_AND_RETURN(glGetShaderiv(hShader, GL_SHADER_SOURCE_LENGTH, &length));
        std::vector<GLchar> source(std::max(length, 1));
        source[0] = 0;
        GL_ON_ERROR_LOG_AND_RETURN(glGetShaderSource(hShader, static_cast<GLsizei>(source.size()), nullptr, &source[0]));

        FSLLOG("*** Source start ***\n" << &source[0] << "\n*** Source end ***\n\n");

        // Fetch the log
        GL_ON_ERROR_LOG_AND_RETURN(glGetShaderiv(hShader, GL_INFO_LOG_LENGTH, &length));
        std::vector<GLchar> errorLog(std::max(length, 1));
        errorLog[0] = 0;
        GL_ON_ERROR_LOG_AND_RETURN(glGetShaderInfoLog(hShader, static_cast<GLsizei>(errorLog.size()), nullptr, &errorLog[0]));

        FSLLOG("*** Error log start ***\n" << &errorLog[0] << "\n*** Error Log End ***\n\n");
      }
    }

    GLShader::GLShader()
      : m_shaderType(0)
      , m_handle(GLValues::INVALID_HANDLE)
    {
    }


    GLShader::GLShader(const GLint shaderType, const std::string& strShaderCode)
      : m_shaderType(shaderType)
      , m_handle(GLValues::INVALID_HANDLE)
    {
      Reset(shaderType, strShaderCode);
    }


    GLShader::~GLShader()
    {
      Reset();
    }


    void GLShader::Reset()
    {
      if (m_handle != GLValues::INVALID_HANDLE)
      {
        glDeleteShader(m_handle);
        m_handle = GLValues::INVALID_HANDLE;
        m_shaderType = 0;
      }
    }


    void GLShader::Reset(const GLint shaderType, const std::string& strShaderCode)
    {
      Reset();
      m_shaderType = shaderType;

      //if (pszShaderCode == nullptr)
      //  throw std::invalid_argument("pszShader can't be null");

      // Create the new shader of the given type
      m_handle = glCreateShader(shaderType);
      GLenum glError = glGetError();
      if (m_handle == 0 || glError != GL_NO_ERROR)
        throw GLESGraphicsException("Failed to create shader", glError, __FILE__, __LINE__);

      const char* shaderCode[1] = { strShaderCode.c_str() };

      GL_CHECK(glShaderSource(m_handle, 1, shaderCode, nullptr));
      shaderCode[0] = nullptr;

      // Compile the shader now
      GL_CHECK(glCompileShader(m_handle));

      GLint status = GL_FALSE;
      GL_CHECK(glGetShaderiv(m_handle, GL_COMPILE_STATUS, &status));

      // Check if something went wrong
      if (status != GL_TRUE)
      {
        // and verbose is enabled then dump some debug information
        //if (m_verbose)
        DumpDebugInformation(m_handle);
        throw GLESGraphicsException("Shader compilation failed", 0, __FILE__, __LINE__);
      }
    }
  }
}
