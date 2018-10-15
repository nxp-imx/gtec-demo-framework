#ifndef FSLUTIL_OPENGLES3_GLVERTEXARRAY_HPP
#define FSLUTIL_OPENGLES3_GLVERTEXARRAY_HPP
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
#include <FslUtil/OpenGLES3/Common.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <GLES3/gl3.h>
#include <FslBase/Attributes.hpp>

namespace Fsl
{
  namespace GLES3
  {
    class GLVertexArray
    {
      GLuint m_handle;

    public:
      GLVertexArray(const GLVertexArray&) = delete;
      GLVertexArray& operator=(const GLVertexArray&) = delete;

      //! @brief Move assignment operator
      GLVertexArray& operator=(GLVertexArray&& other) noexcept
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
          other.m_handle = GLValues::INVALID_HANDLE;
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      GLVertexArray(GLVertexArray&& other) noexcept
        : m_handle(other.m_handle)
      {
        // Remove the data from other
        other.m_handle = GLValues::INVALID_HANDLE;
      }

      //! @brief Create a uninitialized VertexArray
      GLVertexArray();

      //! @brief Create a initialized buffer
      //! @param bInitialize if true the VertexArray is initialized (the param really only exist to differentiate it from the empty constructor)
      GLVertexArray(const bool bInitialize);

      ~GLVertexArray();

      //! @brief If a VertexArray is allocated this will releases it.
      void Reset() noexcept;

      //! @brief Create a initialized buffer
      //! @param bInitialize if true the VertexArray is initialized (the param really only exist to differentiate it from the empty constructor)
      void Reset(const bool bInitialize);

      //! @brief Check if this VertexArray contains a valid gl handle.
      bool IsValid() const
      {
        return m_handle != GLValues::INVALID_HANDLE;
      }

      //! @brief Get the gl handle associated with the buffer.
      //! @return the handle or GLValues::INVALID_HANDLE if the buffer is unallocated.
      GLuint Get() const
      {
        return m_handle;
      }

      //! @brief Get the gl handle associated with the buffer.
      //! @return the handle or GLValues::INVALID_HANDLE if the buffer is unallocated.
      FSL_ATTR_DEPRECATED GLuint GetHandle() const
      {
        return m_handle;
      }

      //! @brief Bind the vertex array
      void Bind() const;

      //! @brief Unbind the vertex array
      void Unbind() const;
    };
  }
}

#endif
