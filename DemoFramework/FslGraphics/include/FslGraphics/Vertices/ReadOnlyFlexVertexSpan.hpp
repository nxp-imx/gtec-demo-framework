#ifndef FSLGRAPHICS_VERTICES_READONLYFLEXVERTEXSPAN_HPP
#define FSLGRAPHICS_VERTICES_READONLYFLEXVERTEXSPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

namespace Fsl
{
  // This is similar to ReadOnlySpan but it represents a 'void*' style memory segment and it contains a vertex declaration that describe how to
  // interpret it.
  class ReadOnlyFlexVertexSpan
  {
  public:
    static constexpr std::size_t extent = ReadOnlyFlexSpan::extent;

    using size_type = ReadOnlyFlexSpan::size_type;
    using difference_type = ReadOnlyFlexSpan::difference_type;
    using const_pointer = ReadOnlyFlexSpan::const_pointer;

  protected:
    ReadOnlyFlexSpan m_span;
    VertexDeclarationSpan m_vertexDeclaration;

  public:
    constexpr ReadOnlyFlexVertexSpan() noexcept = default;
    constexpr ReadOnlyFlexVertexSpan(const ReadOnlyFlexVertexSpan& other) noexcept = default;


    explicit constexpr ReadOnlyFlexVertexSpan(ReadOnlyFlexSpan span, VertexDeclarationSpan vertexDeclaration)
      : m_span(span.data(), span.length(), vertexDeclaration.VertexStride())
      , m_vertexDeclaration(vertexDeclaration)
    {
      if (vertexDeclaration.Empty() || span.stride() != m_vertexDeclaration.VertexStride())
      {
        throw std::invalid_argument("vertex data is incompatible with the supplied vertexDeclaration");
      }
    }

    //! @brief overload that allows you to create a ReadOnlyFlexVertexSpan from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr ReadOnlyFlexVertexSpan(ReadOnlyFlexSpan span, VertexDeclarationSpan vertexDeclaration,
                                              const OptimizationCheckFlag /*flag*/) noexcept
      : m_span(span)
      , m_vertexDeclaration(vertexDeclaration)
    {
      // vertex data must be compatible with the supplied vertexDeclaration
      assert(m_span.stride() == m_vertexDeclaration.VertexStride());
      assert(!vertexDeclaration.Empty());
    }

    explicit constexpr ReadOnlyFlexVertexSpan(const_pointer pData, size_type count, VertexDeclarationSpan vertexDeclaration)
      : m_span(pData, count, vertexDeclaration.VertexStride())
      , m_vertexDeclaration(vertexDeclaration)
    {
      if (vertexDeclaration.Empty())
      {
        throw std::invalid_argument("vertex data is incompatible with the supplied vertexDeclaration");
      }
    }

    //! @brief overload that allows you to create a ReadOnlyFlexVertexSpan from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr ReadOnlyFlexVertexSpan(const_pointer pData, size_type count, VertexDeclarationSpan vertexDeclaration,
                                              const OptimizationCheckFlag flag) noexcept
      : m_span(pData, count, vertexDeclaration.VertexStride(), flag)
      , m_vertexDeclaration(vertexDeclaration)
    {
      assert(!vertexDeclaration.Empty());
    }

    constexpr const_pointer data() const noexcept
    {
      return m_span.data();
    }

    constexpr size_type size() const noexcept
    {
      return m_span.size();
    }

    //! @brief return the element size in bytes
    constexpr size_type stride() const noexcept
    {
      return m_span.stride();
    }

    constexpr bool empty() const noexcept
    {
      return m_span.empty();
    }

    constexpr size_type length() const noexcept
    {
      return m_span.length();
    }

    constexpr ReadOnlyFlexSpan AsFlexSpan() const noexcept
    {
      return m_span;
    }

    constexpr VertexDeclarationSpan AsVertexDeclarationSpan() const noexcept
    {
      return m_vertexDeclaration;
    }

    //! @brief Returns a view of the substring [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    constexpr ReadOnlyFlexVertexSpan subspan(size_type pos = 0, size_type count = extent) const
    {
      return ReadOnlyFlexVertexSpan(m_span.subspan(pos, count), m_vertexDeclaration, OptimizationCheckFlag::NoCheck);
    }
  };
}

#endif
