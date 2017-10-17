#ifndef FSLUTIL_OPENGLES3_GLRENDERBUFFER_HPP
#define FSLUTIL_OPENGLES3_GLRENDERBUFFER_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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
#include <FslBase/Noncopyable.hpp>
#include <FslBase/Math/Point2.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  namespace GLES3
  {
    //! WARNING in development do no use!!!
    class GLRenderBuffer : private Noncopyable
    {
      GLuint m_handle;
      GLenum m_format;
      Point2 m_size;
    public:
      //! @brief Create a uninitialized buffer
      GLRenderBuffer();
      GLRenderBuffer(const Point2& size, const GLenum format);

      ~GLRenderBuffer();

      //! @brief If a buffer is allocated this will releases it.
      void Reset();

      //! @brief Release the existing buffer and replace it with the new one
      //! @param size the size of the buffer
      //! @param format the format of the buffer (for example GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, or GL_STENCIL_INDEX8)
      void Reset(const Point2& size, const GLenum format);

      //! @brief Check if this buffer contains a valid gl handle.
      bool IsValid() const { return m_handle != GLValues::INVALID_HANDLE; }

      //! @brief Get the gl handle associated with the buffer.
      //! @return the handle or GLValues::INVALID_HANDLE if the buffer is unallocated.
      GLuint GetHandle() const { return m_handle; }

      //! @brief Get the format of the buffer
      GLenum GetFormat() const { return m_format; }

      //! @brief Get size of the buffer
      Point2 GetSize() const { return m_size; }
    };

  }
}

#endif
