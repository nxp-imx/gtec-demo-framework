#ifndef FSLUTIL_OPENGLES3_GLVERTEXELEMENTS_HPP
#define FSLUTIL_OPENGLES3_GLVERTEXELEMENTS_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/OpenGLES3/Common.hpp>
#include <FslUtil/OpenGLES3/GLVertexElement.hpp>
#include <FslUtil/OpenGLES3/GLVertexAttribLink.hpp>
#include <FslUtil/OpenGLES3/GLVertexElementAttribConfig.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <utility>
#include <vector>

namespace Fsl
{
  namespace GLES3
  {
    class GLVertexElements
    {
      std::vector<GLVertexElement> m_vertexElements;
      uint32_t m_originalVertexElementCount{0};
      uint32_t m_vertexElementStride{0};

    public:
      //! @brief Move assignment operator
      GLVertexElements& operator=(GLVertexElements&& other) noexcept
      {
        if (this != &other)
        {
          // Claim ownership here
          m_vertexElements = std::move(other.m_vertexElements);
          m_originalVertexElementCount = other.m_originalVertexElementCount;
          m_vertexElementStride = other.m_vertexElementStride;

          // Remove the data from other
          other.m_originalVertexElementCount = 0;
          other.m_vertexElementStride = 0;
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      GLVertexElements(GLVertexElements&& other) noexcept
        : m_vertexElements(std::move(other.m_vertexElements))
        , m_originalVertexElementCount(other.m_originalVertexElementCount)
        , m_vertexElementStride(other.m_vertexElementStride)
      {
        // Remove the data from other
        other.m_originalVertexElementCount = 0;
        other.m_vertexElementStride = 0;
      }


      //! @brief Create a uninitialized vertex buffer
      GLVertexElements() = default;

      //! @brief Create a initialized vertex buffer
      explicit GLVertexElements(VertexDeclarationSpan vertexDeclaration);


      void Reset() noexcept
      {
        m_vertexElements.clear();
        m_originalVertexElementCount = 0;
        m_vertexElementStride = 0;
      }

      //! @brief Reset the buffer to contain the declaration
      void Reset(VertexDeclarationSpan vertexDeclaration);

      bool IsEqual(VertexDeclarationSpan vertexDeclaration) const;

      //! @brief Enable all attrib arrays binding them in order to index 0 to the last VertexElement.
      //! @note  If Two vertex buffers contain vertex elements in the same format, you can get away with just enabling the attrib's once
      void EnableAttribArrays() const;

      //! @brief Disable all attrib arrays associated with the vertex structure currently stored in this buffer
      void DisableAttribArrays() const;

      //! @brief Enable all attrib arrays binding them in order to the supplied index list.
      //         If count < m_vertexElements.size() only the attributes with supplied indices will be bound.
      //         If count >= m_vertexElements.size() only m_vertexElements.size attributes will be bound (the rest is ignored).
      // void EnableAttribArrays(const GLuint* const pAttributeIndices, const std::size_t count) const;

      ////! @brief Disable all attrib arrays in the supplied index list.
      // void DisableAttribArrays(const GLuint* const pAttributeIndices, const std::size_t count) const;

      //! @brief Enable all vertex elements listed in the supplied link list binding the to the requested index
      void EnableAttribArrays(const GLVertexAttribLink* const pLinks, const std::size_t count) const;

      //! @brief Disable all vertex elements listed in the supplied link
      void DisableAttribArrays(const GLVertexAttribLink* const pLinks, const std::size_t count) const;

      template <std::size_t TSize>
      void EnableAttribArrays(const std::array<GLVertexAttribLink, TSize>& links) const
      {
        EnableAttribArrays(links.data(), links.size());
      }

      //! @brief Enable all vertex elements listed in the supplied link list binding the to the requested index
      void EnableAttribArrays(const std::vector<GLVertexAttribLink>& links) const
      {
        EnableAttribArrays(links.data(), links.size());
      }

      //! @brief Disable all vertex elements listed in the supplied link
      template <std::size_t TSize>
      void DisableAttribArrays(const std::array<GLVertexAttribLink, TSize>& links) const
      {
        DisableAttribArrays(links.data(), links.size());
      }

      //! @brief Disable all vertex elements listed in the supplied link
      void DisableAttribArrays(const std::vector<GLVertexAttribLink>& links) const
      {
        DisableAttribArrays(links.data(), links.size());
      }


      //! @brief Configure all attrib arrays binding them in order to index 0 to the last VertexElement.
      void SetVertexAttribPointers() const;

      //! @brief Configure all attrib arrays binding them in order to the supplied index list.
      //         If count < m_vertexElements.size() only the attributes with supplied indices will be bound.
      //         If count >= m_vertexElements.size() only m_vertexElements.size attributes will be bound (the rest is ignored).
      // void SetVertexAttribPointers(const GLuint* const pAttributeIndices, const std::size_t count) const;

      //! @brief Configure all vertex elements listed in the supplied link list binding the to the requested index
      void SetVertexAttribPointers(const GLVertexAttribLink* const pLinks, const std::size_t count) const;

      //! @brief Configure all vertex elements listed in the supplied link list binding the to the requested index
      template <std::size_t TSize>
      void SetVertexAttribPointers(const std::array<GLVertexAttribLink, TSize>& links) const
      {
        SetVertexAttribPointers(links.data(), links.size());
      }

      //! @brief Configure all vertex elements listed in the supplied link list binding the to the requested index
      void SetVertexAttribPointers(const std::vector<GLVertexAttribLink>& links) const
      {
        SetVertexAttribPointers(links.data(), links.size());
      }

      uint32_t VertexStride() const
      {
        return m_vertexElementStride;
      }

      //! @brief Get the number of attrib's associated with the vertex structure currently stored in this buffer
      uint32_t VertexAttribCount() const
      {
        return m_originalVertexElementCount;
      }

      //! @brief Get the number of attrib's associated with the vertex structure currently stored in this buffer
      //!        the expanded attribs are included (unlike the VertexAttribCount which only includes the original VertexDeclaration size)
      uint32_t ExpandedVertexAttribCount() const
      {
        return static_cast<uint32_t>(m_vertexElements.size());
      }

      //! @brief Get direct access to the vertex attrib elements.
      //! @note The returned pointer is only valid until the next buffer modification method is called on this object.
      const GLVertexElement* VertexAttribDirectAccess() const
      {
        return m_vertexElements.data();
      }

      //! @brief Get the vertex element index, throws a NotSupportedException if the usage and usageIndex combo wasn't found
      int32_t GetVertexElementIndex(const VertexElementUsage usage, const uint32_t usageIndex) const;

      //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
      int32_t VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const;

      std::vector<GLVertexElementAttribConfig> ExtractConfiguration(const ReadOnlySpan<GLVertexAttribLink> attribLinks);
    };
  }
}

#endif
