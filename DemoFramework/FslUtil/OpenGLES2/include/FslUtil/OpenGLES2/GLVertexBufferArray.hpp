#ifndef FSLUTIL_OPENGLES2_GLVERTEXBUFFERARRAY_HPP
#define FSLUTIL_OPENGLES2_GLVERTEXBUFFERARRAY_HPP
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
#include <FslUtil/OpenGLES2/Common.hpp>
#include <FslUtil/OpenGLES2/GLBufferArray.hpp>
#include <FslUtil/OpenGLES2/GLVertexElement.hpp>
#include <FslUtil/OpenGLES2/GLVertexAttribLink.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <GLES2/gl2.h>
#include <vector>

namespace Fsl
{
  namespace GLES2
  {

    class GLVertexBufferArray : public GLBufferArray
    {
      std::vector<GLVertexElement> m_vertexElements;
      uint32_t m_originalVertexElementCount;
    public:
      //! @brief Create a empty array
      GLVertexBufferArray();

      //! @brief Create a array of the given capacity
      GLVertexBufferArray(const std::size_t capacity, const VertexDeclaration& vertexDeclaration);

      //! @brief Resize the array
      void Resize(const std::size_t capacity, const VertexDeclaration& vertexDeclaration);


      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer internally)
      void Reset(const std::size_t arrayIndex, const void*const pVertices, const std::size_t elementCount, const VertexDeclaration& vertexDeclaration, const GLenum usage);

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer internally)
      template<typename T>
      void Reset(const std::size_t arrayIndex, const T*const pVertices, const std::size_t elementCount, const GLenum usage)
      {
        Reset(arrayIndex, pVertices, elementCount, T::GetVertexDeclaration(), usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer internally)
      template<typename T>
      void Reset(const std::size_t arrayIndex, const std::vector<T>& vertices, const GLenum usage)
      {
        Reset(arrayIndex, vertices.data(), vertices.size(), T::GetVertexDeclaration(), usage);
      }

      //! @brief Enable all attrib arrays binding them in order to index 0 to the last VertexElement.
      //! @note  If Two vertex buffers contain vertex elements in the same format, you can get away with just enabling the attrib's once
      void EnableAttribArrays() const;

      //! @brief Disable all attrib arrays associated with the vertex structure currently stored in this buffer
      void DisableAttribArrays() const;

      //! @brief Enable all attrib arrays binding them in order to the supplied index list.
      //         If count < m_vertexElements.size() only the attributes with supplied indices will be bound.
      //         If count >= m_vertexElements.size() only m_vertexElements.size attributes will be bound (the rest is ignored).
      void EnableAttribArrays(const GLuint*const pAttributeIndices, const std::size_t count) const;

      //! @brief Disable all attrib arrays in the supplied index list.
      void DisableAttribArrays(const GLuint*const pAttributeIndices, const std::size_t count) const;

      //! @brief Enable all vertex elements listed in the supplied link list binding the to the requested index
      void EnableAttribArrays(const GLVertexAttribLink*const pLinks, const std::size_t count) const;

      //! @brief Disable all vertex elements listed in the supplied link
      void DisableAttribArrays(const GLVertexAttribLink*const pLinks, const std::size_t count) const;


      //! @brief Configure all attrib arrays binding them in order to index 0 to the last VertexElement.
      void SetVertexAttribPointers() const;

      //! @brief Configure all attrib arrays binding them in order to the supplied index list.
      //         If count < m_vertexElements.size() only the attributes with supplied indices will be bound.
      //         If count >= m_vertexElements.size() only m_vertexElements.size attributes will be bound (the rest is ignored).
      void SetVertexAttribPointers(const GLuint*const pAttributeIndices, const std::size_t count) const;

      //! @brief Configure all vertex elements listed in the supplied link list binding the to the requested index
      void SetVertexAttribPointers(const GLVertexAttribLink*const pLinks, const std::size_t count) const;


      //! @brief Get the number of attrib's associated with the vertex structure currently stored in this buffer
      uint32_t VertexAttribCount() const;

      //! @brief Get the number of attrib's associated with the vertex structure currently stored in this buffer
      //!        the expanded attribs are included (unlike the VertexAttribCount which only includes the original VertexDeclaration size)
      uint32_t ExpandedVertexAttribCount() const;

      //! @brief Get direct access to the vertex attrib elements.
      //! @note The returned pointer is only valid until the next buffer modification method is called on this object.
      const GLVertexElement* VertexAttribDirectAccess() const;

      //! @brief Get the vertex element index, throws a NotSupportedException if the usage and usageIndex combo wasn't found
      int32_t GetVertexElementIndex(const VertexElementUsage usage, const uint32_t usageIndex) const;

      //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
      int32_t VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const;
    };

  }
}
#endif
