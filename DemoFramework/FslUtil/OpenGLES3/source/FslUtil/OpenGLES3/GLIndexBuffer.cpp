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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLIndexBuffer.hpp>
#include <algorithm>

namespace Fsl::GLES3
{
  GLIndexBuffer::GLIndexBuffer() = default;


  GLIndexBuffer::GLIndexBuffer(const void* const pIndices, const std::size_t elementCount, const uint32_t elementStride, const GLenum usage,
                               const GLenum type)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, elementStride, usage)
    , m_type(type)
  {
  }


  GLIndexBuffer::GLIndexBuffer(const uint8_t* const pIndices, const std::size_t elementCount, const GLenum usage)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, sizeof(uint8_t), usage)
    , m_type(GL_UNSIGNED_BYTE)
  {
  }


  GLIndexBuffer::GLIndexBuffer(const uint16_t* const pIndices, const std::size_t elementCount, const GLenum usage)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, sizeof(uint16_t), usage)
    , m_type(GL_UNSIGNED_SHORT)
  {
  }


  GLIndexBuffer::GLIndexBuffer(const ReadOnlySpan<uint8_t> indices, const GLenum usage)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint8_t), usage)
    , m_type(GL_UNSIGNED_BYTE)
  {
  }


  GLIndexBuffer::GLIndexBuffer(const ReadOnlySpan<uint16_t> indices, const GLenum usage)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint16_t), usage)
    , m_type(GL_UNSIGNED_SHORT)
  {
  }


  GLIndexBuffer::GLIndexBuffer(const std::vector<uint8_t>& indices, const GLenum usage)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint8_t), usage)
    , m_type(GL_UNSIGNED_BYTE)
  {
  }


  GLIndexBuffer::GLIndexBuffer(const std::vector<uint16_t>& indices, const GLenum usage)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint16_t), usage)
    , m_type(GL_UNSIGNED_SHORT)
  {
  }


  void GLIndexBuffer::Reset(const void* const pIndices, const std::size_t elementCount, const uint32_t elementStride, const GLenum usage,
                            const GLenum type)
  {
    DoReset(GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, elementStride, usage);
    m_type = type;
  }


  void GLIndexBuffer::Reset(const uint8_t* const pIndices, const std::size_t elementCount, const GLenum usage)
  {
    DoReset(GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, sizeof(uint8_t), usage);
    m_type = GL_UNSIGNED_BYTE;
  }


  void GLIndexBuffer::Reset(const uint16_t* const pIndices, const std::size_t elementCount, const GLenum usage)
  {
    DoReset(GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, sizeof(uint16_t), usage);
    m_type = GL_UNSIGNED_SHORT;
  }


  void GLIndexBuffer::Reset(const ReadOnlySpan<uint8_t>& indices, const GLenum usage)
  {
    DoReset(GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint8_t), usage);
    m_type = GL_UNSIGNED_BYTE;
  }


  void GLIndexBuffer::Reset(const ReadOnlySpan<uint16_t>& indices, const GLenum usage)
  {
    DoReset(GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint16_t), usage);
    m_type = GL_UNSIGNED_SHORT;
  }


  void GLIndexBuffer::Reset(const std::vector<uint8_t>& indices, const GLenum usage)
  {
    DoReset(GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint8_t), usage);
    m_type = GL_UNSIGNED_BYTE;
  }


  void GLIndexBuffer::Reset(const std::vector<uint16_t>& indices, const GLenum usage)
  {
    DoReset(GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint16_t), usage);
    m_type = GL_UNSIGNED_SHORT;
  }
}
