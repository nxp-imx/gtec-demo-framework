#ifndef FSLGRAPHICSGLES3_1_GLSHADERPROGRAM_HPP
#define FSLGRAPHICSGLES3_1_GLSHADERPROGRAM_HPP
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
#include <FslGraphicsGLES3/Common.hpp>
#include <FslGraphicsGLES3/GLValues.hpp>
#include <GLES3/gl31.h>
#include <FslBase/Attributes.hpp>
#include <FslBase/Noncopyable.hpp>
#include <string>

namespace Fsl
{
  namespace GLES3
  {

    class GLShaderProgram : private Noncopyable
    {
      GLint m_shaderType;
      GLuint m_handle;
    public:
      GLShaderProgram();
      GLShaderProgram(const GLint shaderType, const std::string& strShaderCode);
      ~GLShaderProgram();

      //! @brief Check if this contains a valid gl handle.
      bool IsValid() const { return m_handle != GLValues::INVALID_HANDLE; }

      void Reset();
      void Reset(const GLint shaderType, const std::string& strShaderCode);

      GLint GetType() const
      {
        return m_shaderType;
      }

      GLuint Get() const
      {
        return m_handle;
      }

      FSL_ATTR_DEPRECATED GLuint GetHandle() const
      {
        return Get();
      }
    };

  }
}

#endif
