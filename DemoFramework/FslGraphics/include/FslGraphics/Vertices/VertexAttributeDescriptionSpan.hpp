#ifndef FSLGRAPHICS_VERTICES_VERTEXATTRIBUTEDESCRIPTIONSPAN_HPP
#define FSLGRAPHICS_VERTICES_VERTEXATTRIBUTEDESCRIPTIONSPAN_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/SpanUtil_ValueCompare.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescription.hpp>
#include <array>

namespace Fsl
{
  class VertexAttributeDescriptionSpan
  {
    ReadOnlySpan<VertexAttributeDescription> m_span;

  public:
    // Request that the compiler generates a copy constructor and assignment operator
    VertexAttributeDescriptionSpan(const VertexAttributeDescriptionSpan&) = default;
    VertexAttributeDescriptionSpan& operator=(const VertexAttributeDescriptionSpan&) = default;

    constexpr VertexAttributeDescriptionSpan() = default;

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    constexpr VertexAttributeDescriptionSpan(const VertexAttributeDescription* const pSrcElements, const std::size_t elementCount)
      : m_span(pSrcElements, elementCount)
    {
      if (elementCount > 0xFFFFFFFFu)
      {
        throw std::invalid_argument("elementCount must fit in a UInt32, and the stride must be valid");
      }
      VerifyElements(m_span);
    }

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    explicit constexpr VertexAttributeDescriptionSpan(const ReadOnlySpan<VertexAttributeDescription>& srcElements)
      : m_span(srcElements)
    {
      if (srcElements.size() > 0xFFFFFFFFu)
      {
        throw std::invalid_argument("elementCount must fit in a UInt32, and the stride must be valid");
      }
      VerifyElements(srcElements);
    }

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    constexpr VertexAttributeDescriptionSpan(const ReadOnlySpan<VertexAttributeDescription>& srcElements,
                                             const OptimizationCheckFlag /*flag*/) noexcept
      : m_span(srcElements)
    {
      assert(srcElements.size() <= 0xFFFFFFFFu);
      assert(IsValidElements(srcElements));
    }

    constexpr bool Empty() const noexcept
    {
      return m_span.empty();
    }

    //! @brief Get the number of elements
    constexpr uint32_t Count() const noexcept
    {
      assert(m_span.size() <= 0xFFFFFFFFu);
      return static_cast<uint32_t>(m_span.size());
    }

    constexpr ReadOnlySpan<VertexAttributeDescription> AsReadOnlySpan() const noexcept
    {
      return m_span;
    }

    constexpr const VertexAttributeDescription* DirectAccess() const noexcept
    {
      return m_span.data();
    }


    constexpr const VertexAttributeDescription& operator[](const std::size_t pos) const noexcept
    {
      return m_span[pos];
    }

    constexpr const VertexAttributeDescription& At(const std::size_t index) const
    {
      return m_span.at(index);
    }

    //! @brief Get the element index of for the given usage and usageIndex (if not found a NotFoundException is thrown)
    constexpr int32_t VertexAttributeGetIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      const int32_t index = VertexAttributeIndexOf(usage, usageIndex);
      if (index < 0)
      {
        throw NotFoundException("Could not locate a vertex element of the requested type");
      }
      return index;
    }

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    constexpr int32_t VertexAttributeIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const noexcept
    {
      for (std::size_t i = 0; i < m_span.size(); ++i)
      {
        if (m_span[i].Usage == usage && m_span[i].UsageIndex == usageIndex)
        {
          return static_cast<int32_t>(i);
        }
      }
      return -1;
    }

    //! @brief Get the element for the given usage and usageIndex (if not found a NotFoundException is thrown)
    constexpr const VertexAttributeDescription& VertexAttributeGet(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      return m_span[VertexAttributeGetIndexOf(usage, usageIndex)];
    }

    constexpr bool operator==(const VertexAttributeDescriptionSpan& rhs) const noexcept
    {
      return SpanUtil::ValueEquals(m_span, rhs.m_span);
    }

    constexpr bool operator!=(const VertexAttributeDescriptionSpan& rhs) const noexcept
    {
      return !(*this == rhs);
    }


    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const VertexAttributeDescription* data() const noexcept
    {
      return m_span.data();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr uint32_t size() const noexcept
    {
      assert(m_span.size() <= 0xFFFFFFFFu);
      return static_cast<uint32_t>(m_span.size());
    }

  private:
    constexpr static bool Contains(const ReadOnlySpan<VertexAttributeDescription>& srcElements, const VertexElementUsage usage,
                                   const uint32_t usageIndex, const std::size_t ignoreIndex) noexcept
    {
      for (std::size_t i = 0; i < srcElements.size(); ++i)
      {
        if (i != ignoreIndex && srcElements[i].Usage == usage && srcElements[i].UsageIndex == usageIndex)
        {
          return true;
        }
      }
      return false;
    }

    constexpr static void VerifyElements(const ReadOnlySpan<VertexAttributeDescription>& srcElements)
    {
      // We expect the element locations to be in order: smallest -> largest
      uint32_t maxLocation = 0;
      for (std::size_t i = 0; i < srcElements.size(); ++i)
      {
        auto location = srcElements[i].Location;
        if (location < maxLocation)
        {
          throw NotSupportedException("The locations should be in order: smallest -> largest");
        }

        maxLocation = location;

        if (Contains(srcElements, srcElements[i].Usage, srcElements[i].UsageIndex, i))
        {
          throw NotSupportedException("Duplicated elements: A usage and usageIndex can only be listed once");
        }
      }
    }

    constexpr static bool IsValidElements(const ReadOnlySpan<VertexAttributeDescription>& srcElements) noexcept
    {
      // We expect the element locations to be in order: smallest -> largest
      uint32_t maxLocation = 0;
      for (std::size_t i = 0; i < srcElements.size(); ++i)
      {
        auto location = srcElements[i].Location;
        if (location < maxLocation)
        {
          return false;
        }

        maxLocation = location;

        if (Contains(srcElements, srcElements[i].Usage, srcElements[i].UsageIndex, i))
        {
          return false;
        }
      }
      return true;
    }
  };
}

#endif
