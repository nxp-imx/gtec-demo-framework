#ifndef FSLUTIL_OPENGLES2_GLVERTEXBUFFER_HPP
#define FSLUTIL_OPENGLES2_GLVERTEXBUFFER_HPP
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
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslUtil/OpenGLES2/Common.hpp>
#include <FslUtil/OpenGLES2/GLBuffer.hpp>
#include <FslUtil/OpenGLES2/GLVertexAttribLink.hpp>
#include <FslUtil/OpenGLES2/GLVertexElement.hpp>
#include <FslUtil/OpenGLES2/GLVertexElements.hpp>
#include <GLES2/gl2.h>
#include <array>
#include <utility>
#include <vector>

namespace Fsl::GLES2
{
  class GLVertexBuffer : public GLBuffer
  {
    GLVertexElements m_vertexElements;

  public:
    //! @brief Move assignment operator
    GLVertexBuffer& operator=(GLVertexBuffer&& other) noexcept
    {
      if (this != &other)
      {
        // Claim ownership here
        m_vertexElements = std::move(other.m_vertexElements);

        // Remove the data from other

        // Move the base part
        GLBuffer::operator=(std::move(other));
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    GLVertexBuffer(GLVertexBuffer&& other) noexcept
      : GLBuffer(std::move(other))                             // NOLINT(bugprone-use-after-move)
      , m_vertexElements(std::move(other.m_vertexElements))    // NOLINT(bugprone-use-after-move)
    {
      // Remove the data from other
    }


    //! @brief Create a uninitialized vertex buffer
    GLVertexBuffer() = default;

    GLVertexBuffer(const void* const pVertices, const std::size_t elementCount, VertexDeclarationSpan vertexDeclaration, const GLenum usage);

    //! @brief Create a initialized vertex buffer
    template <typename T>
    GLVertexBuffer(const T* const pVertices, const std::size_t elementCount, const GLenum usage)
      : GLVertexBuffer()
    {
      Reset(pVertices, elementCount, usage);
    }

    //! @brief Create a initialized vertex buffer
    template <typename T, std::size_t TSize>
    GLVertexBuffer(const std::array<T, TSize>& vertices, const GLenum usage)
      : GLVertexBuffer()
    {
      Reset(vertices, usage);
    }

    //! @brief Create a initialized vertex buffer
    template <typename T>
    GLVertexBuffer(const std::vector<T>& vertices, const GLenum usage)
      : GLVertexBuffer()
    {
      Reset(vertices, usage);
    }


    void Reset() noexcept override
    {
      m_vertexElements.Reset();
      GLBuffer::Reset();
    }

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const void* const pVertices, const std::size_t elementCount, VertexDeclarationSpan vertexDeclaration, const GLenum usage);

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    template <typename T>
    void Reset(const T* const pVertices, const std::size_t elementCount, const GLenum usage)
    {
      Reset(pVertices, elementCount, T::AsVertexDeclarationSpan(), usage);
    }

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    template <typename T, std::size_t TSize>
    void Reset(const std::array<T, TSize>& vertices, const GLenum usage)
    {
      Reset(vertices.data(), vertices.size(), T::AsVertexDeclarationSpan(), usage);
    }

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    template <typename T>
    void Reset(const std::vector<T>& vertices, const GLenum usage)
    {
      Reset(vertices.data(), vertices.size(), T::AsVertexDeclarationSpan(), usage);
    }


    //! @brief Enable all attrib arrays binding them in order to index 0 to the last VertexElement.
    //! @note  If Two vertex buffers contain vertex elements in the same format, you can get away with just enabling the attrib's once
    void EnableAttribArrays() const
    {
      m_vertexElements.EnableAttribArrays();
    }

    //! @brief Disable all attrib arrays associated with the vertex structure currently stored in this buffer
    void DisableAttribArrays() const
    {
      m_vertexElements.DisableAttribArrays();
    }

    //! @brief Enable all attrib arrays binding them in order to the supplied index list.
    //         If count < m_vertexElements.size() only the attributes with supplied indices will be bound.
    //         If count >= m_vertexElements.size() only m_vertexElements.size attributes will be bound (the rest is ignored).
    // void EnableAttribArrays(const GLuint* const pAttributeIndices, const std::size_t count) const
    //{
    //  m_vertexElements.EnableAttribArrays(pAttributeIndices, count);
    //}

    //! @brief Disable all attrib arrays in the supplied index list.
    // void DisableAttribArrays(const GLuint* const pAttributeIndices, const std::size_t count) const
    //{
    //  m_vertexElements.DisableAttribArrays(pAttributeIndices, count);
    //}

    //! @brief Enable all vertex elements listed in the supplied link list binding the to the requested index
    void EnableAttribArrays(const GLVertexAttribLink* const pLinks, const std::size_t count) const
    {
      m_vertexElements.EnableAttribArrays(pLinks, count);
    }

    //! @brief Disable all vertex elements listed in the supplied link
    void DisableAttribArrays(const GLVertexAttribLink* const pLinks, const std::size_t count) const
    {
      m_vertexElements.DisableAttribArrays(pLinks, count);
    }

    template <std::size_t TSize>
    void EnableAttribArrays(const std::array<GLVertexAttribLink, TSize>& links) const
    {
      m_vertexElements.EnableAttribArrays(links);
    }

    //! @brief Enable all vertex elements listed in the supplied link list binding the to the requested index
    void EnableAttribArrays(const std::vector<GLVertexAttribLink>& links) const
    {
      m_vertexElements.EnableAttribArrays(links);
    }

    //! @brief Disable all vertex elements listed in the supplied link
    template <std::size_t TSize>
    void DisableAttribArrays(const std::array<GLVertexAttribLink, TSize>& links) const
    {
      m_vertexElements.DisableAttribArrays(links);
    }

    //! @brief Disable all vertex elements listed in the supplied link
    void DisableAttribArrays(const std::vector<GLVertexAttribLink>& links) const
    {
      m_vertexElements.DisableAttribArrays(links);
    }

    //! @brief Configure all attrib arrays binding them in order to index 0 to the last VertexElement.
    void SetVertexAttribPointers() const
    {
      m_vertexElements.SetVertexAttribPointers();
    }

    //! @brief Configure all attrib arrays binding them in order to the supplied index list.
    //         If count < m_vertexElements.size() only the attributes with supplied indices will be bound.
    //         If count >= m_vertexElements.size() only m_vertexElements.size attributes will be bound (the rest is ignored).
    // void SetVertexAttribPointers(const GLuint* const pAttributeIndices, const std::size_t count) const
    //{
    //  m_vertexElements.SetVertexAttribPointers(pAttributeIndices, count);
    //}

    //! @brief Configure all vertex elements listed in the supplied link list binding the to the requested index
    void SetVertexAttribPointers(const GLVertexAttribLink* const pLinks, const std::size_t count) const
    {
      m_vertexElements.SetVertexAttribPointers(pLinks, count);
    }

    //! @brief Configure all vertex elements listed in the supplied link list binding the to the requested index
    template <std::size_t TSize>
    void SetVertexAttribPointers(const std::array<GLVertexAttribLink, TSize>& links) const
    {
      m_vertexElements.SetVertexAttribPointers(links);
    }

    //! @brief Configure all vertex elements listed in the supplied link list binding the to the requested index
    void SetVertexAttribPointers(const std::vector<GLVertexAttribLink>& links) const
    {
      m_vertexElements.SetVertexAttribPointers(links);
    }

    //! @brief Get the number of attrib's associated with the vertex structure currently stored in this buffer
    uint32_t VertexAttribCount() const
    {
      return m_vertexElements.VertexAttribCount();
    }

    //! @brief Get the number of attrib's associated with the vertex structure currently stored in this buffer
    //!        the expanded attribs are included (unlike the VertexAttribCount which only includes the original VertexDeclaration size)
    uint32_t ExpandedVertexAttribCount() const
    {
      return m_vertexElements.ExpandedVertexAttribCount();
    }

    //! @brief Get direct access to the vertex attrib elements.
    //! @note The returned pointer is only valid until the next buffer modification method is called on this object.
    const GLVertexElement* VertexAttribDirectAccess() const
    {
      return m_vertexElements.VertexAttribDirectAccess();
    }

    //! @brief Get the vertex element index, throws a NotSupportedException if the usage and usageIndex combo wasn't found
    int32_t GetVertexElementIndex(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      return m_vertexElements.GetVertexElementIndex(usage, usageIndex);
    }

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    int32_t VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      return m_vertexElements.VertexElementIndexOf(usage, usageIndex);
    }
  };
}

#endif
