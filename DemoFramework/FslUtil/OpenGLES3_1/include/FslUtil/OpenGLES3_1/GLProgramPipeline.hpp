#ifndef FSLUTIL_OPENGLES3_1_GLPROGRAMPIPELINE_HPP
#define FSLUTIL_OPENGLES3_1_GLPROGRAMPIPELINE_HPP
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
#include <FslUtil/OpenGLES3/Common.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <GLES3/gl31.h>
#include <string>

namespace Fsl::GLES3
{
  class GLShaderProgram;

  class GLProgramPipeline
  {
    GLuint m_handle;

  public:
    GLProgramPipeline(const GLProgramPipeline&) = delete;
    GLProgramPipeline& operator=(const GLProgramPipeline&) = delete;

    //! @brief Move assignment operator
    GLProgramPipeline& operator=(GLProgramPipeline&& other) noexcept
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
    GLProgramPipeline(GLProgramPipeline&& other) noexcept
      : m_handle(other.m_handle)
    {
      // Remove the data from other
      other.m_handle = GLValues::InvalidHandle;
    }

    //! @brief Reset to a uninitialized state.
    GLProgramPipeline();
    //! @brief Reset to a initialized state.
    //! @param notUsed this parameter is not used.
    explicit GLProgramPipeline(bool notUsed);
    //! @brief Let this GLProgramPipeline object assume control over the given program pipeline handle.
    //! @param handle the GL handle of the program pipeline
    explicit GLProgramPipeline(GLuint handle);
    ~GLProgramPipeline();

    //! @brief Check if this contains a valid gl handle.
    bool IsValid() const
    {
      return m_handle != GLValues::InvalidHandle;
    }

    //! @brief Reset to a uninitialized state.
    void Reset() noexcept;

    //! @brief Reset to a initialized state.
    //! @param notUsed this parameter is not used.
    void Reset(bool notUsed);

    //! @brief Let this GLProgramPipeline object assume control over the given program pipeline handle.
    //! @param handle the GL handle of the program pipeline
    void Reset(GLuint handle);

    //! @brief Get the handle to the shader program
    GLuint Get() const
    {
      return m_handle;
    }

    //! @brief Get the handle to the shader program
    [[deprecated("use one of the other overloads instead")]] GLuint GetHandle() const
    {
      return Get();
    }

    //! @brief Bind the current program pipeline
    void Bind();

    //! @brief Reset the current binding
    void BindClear();


    //! @brief Use program stages
    void UseProgramStages(GLShaderProgram& program, const GLbitfield stages, const bool enabled = true);
  };
}

#endif
