/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLVertexElements.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl::GLES3
{
  namespace
  {
    std::size_t CountMatrixElements(VertexDeclarationSpan vertexDeclaration)
    {
      std::size_t count = 0;
      const std::size_t elementCount = vertexDeclaration.Count();
      for (std::size_t i = 0; i < elementCount; ++i)
      {
        count += (vertexDeclaration.At(i).Format == VertexElementFormat::Matrix4x4) ? 1 : 0;
      }
      return count;
    }


    // Convert from the platform independent vertex declaration to a GL one
    void ConvertToNative(std::vector<GLVertexElement>& rVertexElements, VertexDeclarationSpan vertexDeclaration)
    {
      assert(vertexDeclaration.Count() <= std::numeric_limits<uint32_t>::max());
      const auto numMatrixElements = CountMatrixElements(vertexDeclaration);

      const auto convertedElementCount = (vertexDeclaration.Count() - numMatrixElements) + (numMatrixElements * 4);

      if (convertedElementCount != rVertexElements.size())
      {
        rVertexElements.resize(convertedElementCount);
      }

      const auto srcCount = vertexDeclaration.Count();
      const VertexElement* pSrcElements = vertexDeclaration.DirectAccess();
      const VertexElement* const pSrcElementsEnd = pSrcElements + srcCount;
      GLVertexElement* pDst = rVertexElements.data();

      auto extendedIndex = srcCount;
      GLVertexElement* pDstAfterNormalData = rVertexElements.data() + extendedIndex;

      while (pSrcElements < pSrcElementsEnd)
      {
        if (pSrcElements->Format != VertexElementFormat::Matrix4x4)
        {
          pDst->Reset(*pSrcElements);
          ++pDst;
        }
        else
        {
          // Expand Matrix4x4 elements to the OpenGL ES representation
          pDst->Reset(*pSrcElements, VertexElementFormat::Vector4, 0);
          pDst->ExtendedIndex = extendedIndex;
          ++pDst;
          extendedIndex += 3;

          // We store the extended attributes after all the normal ones to ensure that the index between the VertexDeclation and our internal ones
          // are the same
          pDstAfterNormalData->Reset(*pSrcElements, VertexElementFormat::Vector4, sizeof(float) * 4);
          ++pDstAfterNormalData;
          pDstAfterNormalData->Reset(*pSrcElements, VertexElementFormat::Vector4, sizeof(float) * 4 * 2);
          ++pDstAfterNormalData;
          pDstAfterNormalData->Reset(*pSrcElements, VertexElementFormat::Vector4, sizeof(float) * 4 * 3);
          ++pDstAfterNormalData;
        }
        ++pSrcElements;
      }
    }


    bool IsEqualTo(VertexDeclarationSpan vertexDeclaration, const std::vector<GLVertexElement>& vertexElements,
                   const std::size_t originalVertexElementCount)
    {
      if (vertexDeclaration.Count() != originalVertexElementCount)
      {
        return false;
      }

      const auto srcCount = vertexDeclaration.Count();
      const VertexElement* pSrcElements = vertexDeclaration.DirectAccess();
      const VertexElement* const pSrcElementsEnd = pSrcElements + srcCount;
      const GLVertexElement* pDst = vertexElements.data();

      while (pSrcElements < pSrcElementsEnd)
      {
        if (pDst->Source != *pSrcElements)
        {
          return false;
        }
        ++pSrcElements;
        ++pDst;
      }
      return true;
    }


    inline void EnableAttribArray(const GLVertexElement& vertexElement, const GLuint attribIndex, const uint32_t vertexStride,
                                  const ReadOnlySpan<GLVertexElement> vertexElements)
    {
      const auto stride = UncheckedNumericCast<GLsizei>(vertexStride);

      glVertexAttribPointer(attribIndex, vertexElement.Size, vertexElement.Type, vertexElement.Normalized, stride, vertexElement.Pointer);
      glEnableVertexAttribArray(attribIndex);
      // if this is a matrix element we need to enable the rest of the components
      if (vertexElement.Source.Format == VertexElementFormat::Matrix4x4)
      {
        {
          const GLVertexElement& extendedElement = vertexElements[vertexElement.ExtendedIndex];
          glVertexAttribPointer(attribIndex + 1, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, stride,
                                extendedElement.Pointer);
          glEnableVertexAttribArray(attribIndex + 1);
        }
        {
          const GLVertexElement& extendedElement = vertexElements[vertexElement.ExtendedIndex + 1];
          glVertexAttribPointer(attribIndex + 2, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, stride,
                                extendedElement.Pointer);
          glEnableVertexAttribArray(attribIndex + 2);
        }
        {
          const GLVertexElement& extendedElement = vertexElements[vertexElement.ExtendedIndex + 2];
          glVertexAttribPointer(attribIndex + 3, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, stride,
                                extendedElement.Pointer);
          glEnableVertexAttribArray(attribIndex + 3);
        }
      }
    }


    inline void DisableAttribArray(const GLVertexElement& vertexElement, const GLuint attribIndex)
    {
      glDisableVertexAttribArray(attribIndex);

      // if this is a matrix element we need to enable the rest of the components
      if (vertexElement.Source.Format == VertexElementFormat::Matrix4x4)
      {
        glDisableVertexAttribArray(attribIndex + 1);
        glDisableVertexAttribArray(attribIndex + 2);
        glDisableVertexAttribArray(attribIndex + 3);
      }
    }

    inline void EnableAttribPointer(const GLVertexElement& vertexElement, const GLuint attribIndex, const uint32_t vertexStride,
                                    const ReadOnlySpan<GLVertexElement> vertexElements)
    {
      const auto stride = UncheckedNumericCast<GLsizei>(vertexStride);
      glVertexAttribPointer(attribIndex, vertexElement.Size, vertexElement.Type, vertexElement.Normalized, stride, vertexElement.Pointer);
      // if this is a matrix element we need to enable the rest of the components
      if (vertexElement.Source.Format == VertexElementFormat::Matrix4x4)
      {
        {
          const GLVertexElement& extendedElement = vertexElements[vertexElement.ExtendedIndex];
          glVertexAttribPointer(attribIndex + 1, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, stride,
                                extendedElement.Pointer);
        }
        {
          const GLVertexElement& extendedElement = vertexElements[vertexElement.ExtendedIndex + 1];
          glVertexAttribPointer(attribIndex + 2, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, stride,
                                extendedElement.Pointer);
        }
        {
          const GLVertexElement& extendedElement = vertexElements[vertexElement.ExtendedIndex + 2];
          glVertexAttribPointer(attribIndex + 3, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, stride,
                                extendedElement.Pointer);
        }
      }
    }
  }


  GLVertexElements::GLVertexElements(VertexDeclarationSpan vertexDeclaration)
    : m_originalVertexElementCount(vertexDeclaration.Count())
    , m_vertexElementStride(vertexDeclaration.VertexStride())
  {
    ConvertToNative(m_vertexElements, vertexDeclaration);
  }


  void GLVertexElements::Reset(VertexDeclarationSpan vertexDeclaration)
  {
    m_originalVertexElementCount = vertexDeclaration.Count();
    m_vertexElementStride = vertexDeclaration.VertexStride();

    ConvertToNative(m_vertexElements, vertexDeclaration);
  }

  bool GLVertexElements::IsEqual(VertexDeclarationSpan vertexDeclaration) const
  {
    return IsEqualTo(vertexDeclaration, m_vertexElements, m_originalVertexElementCount);
  }


  void GLVertexElements::EnableAttribArrays() const
  {
    const auto vertexStride = UncheckedNumericCast<GLsizei>(VertexStride());
    const GLVertexElement* pVertexElements = m_vertexElements.data();
    const GLVertexElement* const pVertexElementsEnd = pVertexElements + m_vertexElements.size();

    GLuint attribIndex = 0;
    while (pVertexElements < pVertexElementsEnd)
    {
      // We dont need special handling of the 'extended' elements here since we enable everything
      glVertexAttribPointer(attribIndex, pVertexElements->Size, pVertexElements->Type, pVertexElements->Normalized, vertexStride,
                            pVertexElements->Pointer);
      glEnableVertexAttribArray(attribIndex);
      ++pVertexElements;
      ++attribIndex;
    }
  }


  void GLVertexElements::DisableAttribArrays() const
  {
    const auto count = static_cast<GLuint>(m_vertexElements.size());
    GLuint index = 0;
    while (index < count)
    {
      // We don't need special handling of the 'extended' elements here since we disable everything
      glDisableVertexAttribArray(index);
      ++index;
    }
  }


  void GLVertexElements::EnableAttribArrays(const GLVertexAttribLink* const pLinks, const std::size_t count) const
  {
    if (pLinks == nullptr)
    {
      throw std::invalid_argument("pLinks can not be null");
    }
    if (count > std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("We only support 32bit of elements");
    }

    const ReadOnlySpan<GLVertexElement> vertexElementSpan = SpanUtil::AsReadOnlySpan(m_vertexElements);
    const auto vertexStride = VertexStride();
    assert(count <= std::numeric_limits<uint32_t>::max());
    for (uint32_t i = 0; i < static_cast<uint32_t>(count); ++i)
    {
      if (pLinks[i].AttribIndex >= 0)    // if present in shader
      {
        assert(pLinks[i].VertexElementIndex < vertexElementSpan.size());
        EnableAttribArray(vertexElementSpan[pLinks[i].VertexElementIndex], pLinks[i].AttribIndex, vertexStride, vertexElementSpan);
      }
    }
  }


  void GLVertexElements::DisableAttribArrays(const GLVertexAttribLink* const pLinks, const std::size_t count) const
  {
    if (pLinks == nullptr)
    {
      throw std::invalid_argument("pLinks can not be null");
    }
    if (count > std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("We only support 32bit of elements");
    }

    const ReadOnlySpan<GLVertexElement> vertexElementSpan = SpanUtil::AsReadOnlySpan(m_vertexElements);
    for (uint32_t i = 0; i < static_cast<uint32_t>(count); ++i)
    {
      if (pLinks[i].AttribIndex >= 0)
      {
        assert(pLinks[i].VertexElementIndex < vertexElementSpan.size());
        DisableAttribArray(vertexElementSpan[pLinks[i].VertexElementIndex], pLinks[i].AttribIndex);
      }
    }
  }


  void GLVertexElements::SetVertexAttribPointers() const
  {
    const auto vertexStride = UncheckedNumericCast<GLsizei>(VertexStride());
    const GLVertexElement* pVertexElements = m_vertexElements.data();
    const GLVertexElement* const pVertexElementsEnd = pVertexElements + m_vertexElements.size();

    GLuint attribIndex = 0;
    while (pVertexElements < pVertexElementsEnd)
    {
      // We dont need special handling of the 'extended' elements here since we enable everything
      glVertexAttribPointer(attribIndex, pVertexElements->Size, pVertexElements->Type, pVertexElements->Normalized, vertexStride,
                            pVertexElements->Pointer);
      ++pVertexElements;
      ++attribIndex;
    }
  }


  void GLVertexElements::SetVertexAttribPointers(const GLVertexAttribLink* const pLinks, const std::size_t count) const
  {
    if (count > std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("We only support 32bit of elements");
    }

    const ReadOnlySpan<GLVertexElement> vertexElementSpan = SpanUtil::AsReadOnlySpan(m_vertexElements);
    const auto vertexStride = VertexStride();
    for (uint32_t i = 0; i < static_cast<uint32_t>(count); ++i)
    {
      if (pLinks[i].AttribIndex >= 0)
      {
        assert(pLinks[i].VertexElementIndex < vertexElementSpan.size());
        EnableAttribPointer(vertexElementSpan[pLinks[i].VertexElementIndex], pLinks[i].AttribIndex, vertexStride, vertexElementSpan);
      }
    }
  }


  int32_t GLVertexElements::GetVertexElementIndex(const VertexElementUsage usage, const uint32_t usageIndex) const
  {
    const auto index = VertexElementIndexOf(usage, usageIndex);
    if (index < 0)
    {
      throw UsageErrorException("The supplied usage and usageIndex combo was not found");
    }
    return index;
  }


  int32_t GLVertexElements::VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const
  {
    assert(m_vertexElements.size() <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
    for (std::size_t i = 0; i < m_vertexElements.size(); ++i)
    {
      if (m_vertexElements[i].Source.Usage == usage && m_vertexElements[i].Source.UsageIndex == usageIndex)
      {
        return static_cast<int32_t>(i);
      }
    }
    return -1;
  }


  std::vector<GLVertexElementAttribConfig> GLVertexElements::ExtractConfiguration(const ReadOnlySpan<GLVertexAttribLink> attribLinks)
  {
    std::vector<GLVertexElementAttribConfig> res(m_vertexElements.size());
    res.clear();

    const ReadOnlySpan<GLVertexElement> vertexElementSpan = SpanUtil::AsReadOnlySpan(m_vertexElements);
    // const auto vertexStride = VertexStride();
    const auto count = UncheckedNumericCast<uint32_t>(attribLinks.size());
    for (uint32_t i = 0; i < count; ++i)
    {
      if (attribLinks[i].AttribIndex >= 0)
      {
        assert(attribLinks[i].VertexElementIndex < vertexElementSpan.size());

        GLint attribIndex = attribLinks[i].AttribIndex;
        const GLVertexElement& vertexElement = vertexElementSpan[attribLinks[i].VertexElementIndex];

        res.emplace_back(attribIndex, vertexElement.Size, vertexElement.Type, vertexElement.Normalized, vertexElement.Pointer);
        // if this is a matrix element we need to enable the rest of the components
        if (vertexElement.Source.Format == VertexElementFormat::Matrix4x4)
        {
          {
            const GLVertexElement& extendedElement = vertexElementSpan[vertexElement.ExtendedIndex];
            res.emplace_back(attribIndex + 1, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, extendedElement.Pointer);
          }
          {
            const GLVertexElement& extendedElement = vertexElementSpan[vertexElement.ExtendedIndex + 1];
            res.emplace_back(attribIndex + 2, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, extendedElement.Pointer);
          }
          {
            const GLVertexElement& extendedElement = vertexElementSpan[vertexElement.ExtendedIndex + 2];
            res.emplace_back(attribIndex + 3, extendedElement.Size, extendedElement.Type, extendedElement.Normalized, extendedElement.Pointer);
          }
        }
      }
    }
    return res;
  }
}
