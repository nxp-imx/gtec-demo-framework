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

#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLVertexElement.hpp>
#include <FslGraphics/Vertices/VertexElementFormatUtil.hpp>
#include <cassert>
#include <cstdint>
#include <limits>

namespace Fsl
{
  namespace GLES2
  {
    namespace
    {
      GLenum ConvertToNativeType(const VertexElementFormat format)
      {
        switch (format)
        {
        case VertexElementFormat::Single:
        case VertexElementFormat::Vector2:
        case VertexElementFormat::Vector3:
        case VertexElementFormat::Vector4:
        case VertexElementFormat::Matrix4x4:
          return GL_FLOAT;
        case VertexElementFormat::X8_UINT:
        case VertexElementFormat::X8_UNORM:
        case VertexElementFormat::X8Y8_UINT:
        case VertexElementFormat::X8Y8_UNORM:
        case VertexElementFormat::X8Y8Z8_UINT:
        case VertexElementFormat::X8Y8Z8_UNORM:
        case VertexElementFormat::X8Y8Z8W8_UINT:
        case VertexElementFormat::X8Y8Z8W8_UNORM:
          return GL_UNSIGNED_BYTE;
        default:
          throw NotSupportedException("Unknown VertexElementFormat");
        }
      }

      GLboolean IsNormalized(const VertexElementFormat format)
      {
        switch (format)
        {
        case VertexElementFormat::Single:
        case VertexElementFormat::Vector2:
        case VertexElementFormat::Vector3:
        case VertexElementFormat::Vector4:
        case VertexElementFormat::Matrix4x4:
        case VertexElementFormat::X8Y8Z8W8_UINT:
        case VertexElementFormat::X8_UINT:
        case VertexElementFormat::X8Y8_UINT:
        case VertexElementFormat::X8Y8Z8_UINT:
          return GL_FALSE;
        case VertexElementFormat::X8_UNORM:
        case VertexElementFormat::X8Y8_UNORM:
        case VertexElementFormat::X8Y8Z8_UNORM:
        case VertexElementFormat::X8Y8Z8W8_UNORM:
          return GL_TRUE;
        default:
          throw NotSupportedException("Unknown VertexElementFormat");
        }
      }

      inline GLint GetElementCount(const VertexElementFormat format)
      {
        auto res = VertexElementFormatUtil::GetElementCount(format);
        assert(res < static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
        assert(static_cast<GLint>(res) <= std::numeric_limits<GLint>::max());
        return static_cast<GLint>(res);
      }
    }

    GLVertexElement::GLVertexElement() = default;


    GLVertexElement::GLVertexElement(const VertexElementEx& source)
      : Source(source)
      , Size((GetElementCount(source.Format)))
      , Type(ConvertToNativeType(source.Format))
      , Normalized(IsNormalized(source.Format))
      , Pointer(reinterpret_cast<const GLvoid*>(intptr_t(source.Offset)))
    {
    }


    void GLVertexElement::Reset(const VertexElementEx& source)
    {
      Source = source;
      Size = GetElementCount(source.Format);
      Type = ConvertToNativeType(source.Format);
      Normalized = IsNormalized(source.Format);
      Pointer = reinterpret_cast<const GLvoid*>(intptr_t(source.Offset));
      ExtendedIndex = 0;
    }


    void GLVertexElement::Reset(const VertexElementEx& source, const VertexElementFormat internalFormat, const int32_t offsetAdd)
    {
      Source = source;
      Size = GetElementCount(internalFormat);
      Type = ConvertToNativeType(internalFormat);
      Normalized = IsNormalized(source.Format);
      Pointer = reinterpret_cast<const GLvoid*>(intptr_t(source.Offset + offsetAdd));
      ExtendedIndex = 0;
    }
  }
}
