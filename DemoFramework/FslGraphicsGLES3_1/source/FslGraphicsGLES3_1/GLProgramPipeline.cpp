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
#include <FslGraphicsGLES3/Exceptions.hpp>
#include <FslGraphicsGLES3/GLCheck.hpp>
#include <FslGraphicsGLES3_1/GLProgramPipeline.hpp>
#include <FslGraphicsGLES3_1/GLShaderProgram.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

namespace Fsl
{
  namespace GLES3
  {
    GLProgramPipeline::GLProgramPipeline()
      : m_handle(GLValues::INVALID_HANDLE)
    {
    }


    GLProgramPipeline::GLProgramPipeline(bool notUsed)
      : m_handle(GLValues::INVALID_HANDLE)
    {
      Reset(notUsed);
    }


    GLProgramPipeline::GLProgramPipeline(GLuint handle)
      : m_handle(handle)
    {
    }

    GLProgramPipeline::~GLProgramPipeline()
    {
      Reset();
    }


    void GLProgramPipeline::Reset()
    {
      if (m_handle != 0)
      {
        glDeleteProgramPipelines(1, &m_handle);
        m_handle = GLValues::INVALID_HANDLE;
      }
    }


    void GLProgramPipeline::Reset(bool notUsed)
    {
      Reset();
      GL_CHECK(glGenProgramPipelines(1, &m_handle));
    }


    void GLProgramPipeline::Reset(GLuint handle)
    {
      Reset();
      m_handle = handle;
    }


    void GLProgramPipeline::Bind()
    {
      glBindProgramPipeline(m_handle);
    }


    void GLProgramPipeline::BindClear()
    {
      glBindProgramPipeline(0);
    }


    void GLProgramPipeline::UseProgramStages(GLShaderProgram& program, const GLbitfield stages, const bool enabled)
    {
      assert(m_handle != GLValues::INVALID_HANDLE);
      assert(program.IsValid());

      glUseProgramStages(m_handle, stages, enabled ? program.Get() : GLValues::INVALID_HANDLE);
    }

  }
}
