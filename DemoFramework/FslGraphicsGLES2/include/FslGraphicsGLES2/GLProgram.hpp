#ifndef FSLGRAPHICSGLES2_GLPROGRAM_HPP
#define FSLGRAPHICSGLES2_GLPROGRAM_HPP
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
#include <FslGraphicsGLES2/Common.hpp>
#include <FslGraphicsGLES2/GLValues.hpp>
#include <FslGraphicsGLES2/GLShader.hpp>
#include <FslBase/Noncopyable.hpp>
#include <GLES2/gl2.h>
#include <deque>
#include <string>

namespace Fsl
{
  namespace GLES2
  {
    class GLProgram : private Noncopyable
    {
      GLuint m_handle;
    public:
      //! @brief Create a uninitialized program, use Reset to configure it
      GLProgram();

      //! @brief Let this GLProgram object assume control over the given program handle.
      GLProgram(const GLuint handle);

      //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
      GLProgram(const GLShader& vertexShader, const GLShader& fragmentShader, const std::deque<std::string>& attributes);

      //! @brief Create a program from the supplied vertex, fragment shader.
      GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader);

      //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
      GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const std::string& strAttribute);

      //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
      GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const char*const*const pAttributes);

      //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
      GLProgram(const std::string& strVertexShader, const std::string& strFragmentShader, const std::deque<std::string>& attributes);

      ~GLProgram();

      //! @brief Check if this contains a valid gl handle.
      bool IsValid() const { return m_handle != GLValues::INVALID_HANDLE; }

      //! @brief Reset to a uninitialized state.
      void Reset();

      //! @brief Let this GLProgram object assume control over the given program handle.
      void Reset(const GLuint handle);

      //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
      void Reset(const GLShader& vertexShader, const GLShader& fragmentShader, const std::deque<std::string>& attributes);

      //! @brief Create a program from the supplied vertex, fragment shader.
      void Reset(const std::string& strVertexShader, const std::string& strFragmentShader);

      //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
      void Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const std::string& strAttribute);

      //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
      void Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const char*const*const pAttributes);

      //! @brief Create a program from the supplied vertex, fragment shader and attribute list.
      void Reset(const std::string& strVertexShader, const std::string& strFragmentShader, const std::deque<std::string>& attributes);

      //! @brief Get the handle to the shader program
      GLuint Get() const
      {
        return m_handle;
      }

      //! @brief Get the handle to the shader program
      FSL_ATTR_DEPRECATED GLuint GetHandle() const
      {
        return m_handle;
      }
    };
  }
}

#endif
