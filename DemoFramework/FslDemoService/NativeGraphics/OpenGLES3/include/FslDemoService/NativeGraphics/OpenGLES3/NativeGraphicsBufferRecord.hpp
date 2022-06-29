#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSBUFFERRECORD_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSBUFFERRECORD_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslGraphics/Render/Basic/BasicBufferType.hpp>
#include <FslUtil/OpenGLES3/GLBuffer.hpp>
#include <cassert>

namespace Fsl::GLES3
{
  class NativeGraphicsBufferRecord
  {
    GLBuffer m_buffer;

  public:
    NativeGraphicsBufferRecord() noexcept = default;

    explicit NativeGraphicsBufferRecord(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData, const uint32_t bufferElementCapacity,
                                        const bool isDynamic)
      : m_buffer(GetTarget(bufferType), bufferData, bufferElementCapacity, (isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW))
    {
    }

    bool IsValid() const
    {
      return m_buffer.IsValid();
    }

    const GLBuffer& GetBuffer() const
    {
      return m_buffer;
    }

    void SetBufferData(const uint32_t dstIndex, ReadOnlyFlexSpan bufferData)
    {
      if (bufferData.stride() != m_buffer.GetElementStride())
      {
        throw std::invalid_argument("Supplied buffer is not compatible");
      }
      assert(m_buffer.GetUsage() == GL_DYNAMIC_DRAW);
      m_buffer.SetDataEx(dstIndex, bufferData);
    }

  private:
    static GLenum GetTarget(const BasicBufferType bufferType)
    {
      switch (bufferType)
      {
      case BasicBufferType::Index:
        return GL_ELEMENT_ARRAY_BUFFER;
      case BasicBufferType::Vertex:
        return GL_ARRAY_BUFFER;
      default:
        throw NotSupportedException("Unsupported bufferType");
      }
    }
  };
}

#endif
