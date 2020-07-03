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

#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

namespace Fsl
{
  namespace GLES3
  {
    namespace
    {
      void DumpDebugInformation(const GLuint hProgram)
      {
        GLint errorBufSize = 0;
        GLint errorLength = 0;
        glGetProgramiv(hProgram, GL_INFO_LOG_LENGTH, &errorBufSize);

        std::vector<char> errorLog(std::max(errorBufSize, 1));
        errorLog[0] = 0;

        glGetProgramInfoLog(hProgram, static_cast<GLsizei>(errorLog.size()), &errorLength, errorLog.data());
        FSLLOG3_INFO("*** Error log start ***\n{}\n*** Error Log End ***\n\n", &errorLog[0]);
      }


      GLuint Build(const GLShader& vertexShader, const GLShader& fragmentShader, const std::deque<std::string>& attributes,
                   const std::function<void(GLuint)>& postCompilePreLinkCallback)
      {
        if (vertexShader.GetType() != GL_VERTEX_SHADER)
        {
          throw std::invalid_argument("vertexShader should be of type GL_VERTEX_SHADER");
        }
        if (fragmentShader.GetType() != GL_FRAGMENT_SHADER)
        {
          throw std::invalid_argument("fragmentShader should be of type GL_FRAGMENT_SHADER");
        }

        GLuint handle = GL_CHECK(glCreateProgram());
        if (handle == 0)
        {
          throw GLESGraphicsException("Failed to create program", 0, __FILE__, __LINE__);
        }

        GL_CHECK(glAttachShader(handle, vertexShader.Get()));
        GL_CHECK(glAttachShader(handle, fragmentShader.Get()));

        assert(std::numeric_limits<GLuint>::max() >= attributes.size());
        for (GLuint i = 0; i < attributes.size(); ++i)
        {
          GL_CHECK(glBindAttribLocation(handle, i, attributes[i].c_str()));
        }

        if (postCompilePreLinkCallback)
        {
          postCompilePreLinkCallback(handle);
          GL_CHECK_FOR_ERROR();
        }

        // Link the vertex shader and fragment shader together
        GL_CHECK(glLinkProgram(handle));

        // Release the shaders from the program now that we are linked
        GL_CHECK(glDetachShader(handle, vertexShader.Get()));
        GL_CHECK(glDetachShader(handle, fragmentShader.Get()));

        // Check for link success
        GLint nLinkResult = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &nLinkResult);
        if (0 == nLinkResult)
        {
          DumpDebugInformation(handle);
          throw GLESGraphicsException("Failed to link program", 0, __FILE__, __LINE__);
        }
        return handle;
      }


      GLuint Build(const std::string& strVertexShader, const std::string& strFragmentShader, const std::deque<std::string>& attributes,
                   const std::function<void(GLuint)>& postCompilePreLinkCallback)
      {
        // if (pszVertexShader == nullptr)
        //  throw std::invalid_argument("pszVertexShader can't be null");
        // if (pszFragmentShader == nullptr)
        //  throw std::invalid_argument("pszFragmentShader can't be null");

        // Compile the shaders
        const GLShader vertexShader(GL_VERTEX_SHADER, strVertexShader);
        const GLShader fragmentShader(GL_FRAGMENT_SHADER, strFragmentShader);
        // Link them into the program
        return Build(vertexShader, fragmentShader, attributes, postCompilePreLinkCallback);
      }

      GLuint Build(const std::string& strVertexShader, const std::string& strFragmentShader,
                   const std::function<void(GLuint)>& postCompilePreLinkCallback)
      {
        std::deque<std::string> attributes;
        return Build(strVertexShader, strFragmentShader, attributes, postCompilePreLinkCallback);
      }


      GLuint Build(const std::string& strVertexShader, const std::string& strFragmentShader, const std::string& strAttribute,
                   const std::function<void(GLuint)>& postCompilePreLinkCallback)
      {
        // if (pszAttribute == nullptr)
        //  throw std::invalid_argument("pszAttribute can't be null");

        std::deque<std::string> attributes;
        attributes.push_back(strAttribute);
        return Build(strVertexShader, strFragmentShader, attributes, postCompilePreLinkCallback);
      }


      GLuint Build(const std::string& strVertexShader, const std::string& strFragmentShader, const char* const* const pAttributes,
                   const std::function<void(GLuint)>& postCompilePreLinkCallback)
      {
        if (pAttributes == nullptr)
        {
          throw std::invalid_argument("pAttributes can't be null");
        }

        const char* const* pAttr = pAttributes;
        std::deque<std::string> attributes;
        while (*pAttr != nullptr)
        {
          attributes.emplace_back(*pAttr);
          ++pAttr;
        }
        return Build(strVertexShader, strFragmentShader, attributes, postCompilePreLinkCallback);
      }
    }

    GLProgram::GLProgram()
      : m_handle(GLValues::INVALID_HANDLE)
    {
    }


    GLProgram::GLProgram(const GLuint handle)
      : m_handle(handle)
    {
    }


    GLProgram::GLProgram(const GLShader& vertexShader, const GLShader& fragmentShader, const std::deque<std::string>& attributes)
      : m_handle(Build(vertexShader, fragmentShader, attributes, std::function<void(GLuint)>()))
    {
    }


    GLProgram::GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader)
      : m_handle(Build(strVertexShader, strFragmentShader, std::function<void(GLuint)>()))
    {
    }


    GLProgram::GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const std::string& strAttribute)
      : m_handle(Build(strVertexShader, strFragmentShader, strAttribute, std::function<void(GLuint)>()))
    {
    }


    GLProgram::GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const char* const* const pAttributes)
      : m_handle(Build(strVertexShader, strFragmentShader, pAttributes, std::function<void(GLuint)>()))
    {
    }


    GLProgram::GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const std::deque<std::string>& attributes)
      : m_handle(Build(strVertexShader, strFragmentShader, attributes, std::function<void(GLuint)>()))
    {
    }


    GLProgram::GLProgram(const GLShader& vertexShader, const GLShader& fragmentShader, const std::deque<std::string>& attributes,
                         const std::function<void(GLuint)>& postCompilePreLinkCallback)
      : m_handle(Build(vertexShader, fragmentShader, attributes, postCompilePreLinkCallback))
    {
    }


    GLProgram::GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader,
                         const std::function<void(GLuint)>& postCompilePreLinkCallback)
      : m_handle(Build(strVertexShader, strFragmentShader, postCompilePreLinkCallback))
    {
    }


    GLProgram::GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const std::string& strAttribute,
                         const std::function<void(GLuint)>& postCompilePreLinkCallback)
      : m_handle(Build(strVertexShader, strFragmentShader, strAttribute, postCompilePreLinkCallback))
    {
    }


    GLProgram::GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const char* const* const pAttributes,
                         const std::function<void(GLuint)>& postCompilePreLinkCallback)
      : m_handle(Build(strVertexShader, strFragmentShader, pAttributes, postCompilePreLinkCallback))
    {
    }


    GLProgram::GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const std::deque<std::string>& attributes,
                         const std::function<void(GLuint)>& postCompilePreLinkCallback)
      : m_handle(Build(strVertexShader, strFragmentShader, attributes, postCompilePreLinkCallback))
    {
    }


    GLProgram::~GLProgram()
    {
      Reset();
    }


    void GLProgram::Reset() noexcept
    {
      if (m_handle != GLValues::INVALID_HANDLE)
      {
        glDeleteProgram(m_handle);
        m_handle = GLValues::INVALID_HANDLE;
      }
    }


    void GLProgram::Reset(GLuint handle)
    {
      Reset();
      m_handle = handle;
    }


    void GLProgram::Reset(const GLShader& vertexShader, const GLShader& fragmentShader, const std::deque<std::string>& attributes)
    {
      Reset();
      m_handle = Build(vertexShader, fragmentShader, attributes, std::function<void(GLuint)>());
    }


    void GLProgram::Reset(const std::string& strVertexShader, const std::string& strFragmentShader)
    {
      Reset();
      m_handle = Build(strVertexShader, strFragmentShader, std::function<void(GLuint)>());
    }


    void GLProgram::Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const std::string& strAttribute)
    {
      Reset();
      m_handle = Build(strVertexShader, strFragmentShader, strAttribute, std::function<void(GLuint)>());
    }


    void GLProgram::Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const char* const* const pAttributes)
    {
      Reset();
      m_handle = Build(strVertexShader, strFragmentShader, pAttributes, std::function<void(GLuint)>());
    }


    void GLProgram::Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const std::deque<std::string>& attributes)
    {
      Reset();
      m_handle = Build(strVertexShader, strFragmentShader, attributes, std::function<void(GLuint)>());
    }


    void GLProgram::Reset(const GLShader& vertexShader, const GLShader& fragmentShader, const std::deque<std::string>& attributes,
                          const std::function<void(GLuint)>& postCompilePreLinkCallback)
    {
      Reset();
      m_handle = Build(vertexShader, fragmentShader, attributes, postCompilePreLinkCallback);
    }


    void GLProgram::Reset(const std::string& strVertexShader, const std::string& strFragmentShader,
                          const std::function<void(GLuint)>& postCompilePreLinkCallback)
    {
      Reset();
      m_handle = Build(strVertexShader, strFragmentShader, postCompilePreLinkCallback);
    }


    void GLProgram::Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const std::string& strAttribute,
                          const std::function<void(GLuint)>& postCompilePreLinkCallback)
    {
      Reset();
      m_handle = Build(strVertexShader, strFragmentShader, strAttribute, postCompilePreLinkCallback);
    }


    void GLProgram::Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const char* const* const pAttributes,
                          const std::function<void(GLuint)>& postCompilePreLinkCallback)
    {
      Reset();
      m_handle = Build(strVertexShader, strFragmentShader, pAttributes, postCompilePreLinkCallback);
    }


    void GLProgram::Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const std::deque<std::string>& attributes,
                          const std::function<void(GLuint)>& postCompilePreLinkCallback)
    {
      Reset();
      m_handle = Build(strVertexShader, strFragmentShader, attributes, postCompilePreLinkCallback);
    }
  }
}
