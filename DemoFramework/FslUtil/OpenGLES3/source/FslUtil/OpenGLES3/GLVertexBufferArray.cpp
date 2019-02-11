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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLVertexBufferArray.hpp>
#include <FslBase/Log/Log.hpp>

#include <algorithm>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace GLES3
  {
    GLVertexBufferArray::GLVertexBufferArray(const std::size_t capacity, const VertexDeclaration& vertexDeclaration)
      : GLBufferArray(capacity, GL_ARRAY_BUFFER, vertexDeclaration.VertexStride())
      , m_vertexElements(vertexDeclaration)
    {
    }


    void GLVertexBufferArray::Resize(const std::size_t capacity, const VertexDeclaration& vertexDeclaration)
    {
      if (!m_vertexElements.IsEqual(vertexDeclaration))
      {
        if (IsValid())
        {
          throw NotSupportedException("The Vertex declaration did not match the array configuration");
        }


        // Allow resize of invalid array to maintain compatibility.
        m_vertexElements.Reset(vertexDeclaration);
      }
      DoResize(capacity, GL_ARRAY_BUFFER, vertexDeclaration.VertexStride());
    }


    void GLVertexBufferArray::Reset(const std::size_t arrayIndex, const void* const pVertices, const std::size_t elementCount,
                                    const VertexDeclaration& vertexDeclaration, const GLenum usage)
    {
      if (!m_vertexElements.IsEqual(vertexDeclaration))
      {
        throw NotSupportedException("The Vertex declaration did not match the array configuration");
      }

      GLBufferArray::Reset(arrayIndex, GL_ARRAY_BUFFER, pVertices, elementCount, vertexDeclaration.VertexStride(), usage);
    }
  }
}
