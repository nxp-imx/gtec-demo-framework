#ifndef FSLGRAPHICS_VERTICES_BASICVERTEXDECLARATIONARRAY_HPP
#define FSLGRAPHICS_VERTICES_BASICVERTEXDECLARATIONARRAY_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Attributes.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslGraphics/Vertices/VertexElement.hpp>
#include <FslGraphics/Vertices/VertexElementFormatUtil.hpp>
#include <array>
#include <initializer_list>

namespace Fsl
{
  template <std::size_t TEntries>
  class BasicVertexDeclarationArray
  {
    std::array<VertexElement, TEntries> m_elements{};
    uint32_t m_minimumVertexSize{0};

  public:
    // Request that the compiler generates a copy constructor and assignment operator
    BasicVertexDeclarationArray(const BasicVertexDeclarationArray&) = default;
    BasicVertexDeclarationArray& operator=(const BasicVertexDeclarationArray&) = default;

    constexpr BasicVertexDeclarationArray() = default;

    // C++17
    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    explicit constexpr BasicVertexDeclarationArray(VertexDeclarationSpan srcSpan)
    {
      if (srcSpan.Count() != TEntries)
      {
        throw std::invalid_argument("incompatible sized src span");
      }
      for (std::size_t i = 0; i < TEntries; ++i)
      {
        m_elements[i] = srcSpan.At(i);
      }
      // We do not need to verify the elements as the span has been validated
      m_minimumVertexSize = VerifyElementsAndCalcMinimumVertexSize(m_elements);
    }

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements must be sorted according to location (smallest to largest)
    constexpr BasicVertexDeclarationArray(std::initializer_list<VertexElement> list)
    {
      if (list.size() != TEntries)
      {
        throw std::invalid_argument("incompatible sized initializer list");
      }
      {    // copy the initializer list
        std::size_t i = 0;
        for (const auto& entry : list)
        {
          m_elements[i] = entry;
          ++i;
        }
      }
      m_minimumVertexSize = VerifyElementsAndCalcMinimumVertexSize(m_elements);
    }


    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will must be sorted according to offset (smallest to largest)
    explicit constexpr BasicVertexDeclarationArray(const std::array<VertexElement, TEntries>& srcElements)
      : m_elements(srcElements)
      , m_minimumVertexSize(VerifyElementsAndCalcMinimumVertexSize(m_elements))
    {
    }

    //! @brief Get the number of elements
    constexpr uint32_t MinimumVertexSize() const noexcept
    {
      return m_minimumVertexSize;
    }

    //! @brief Get the number of elements
    constexpr std::size_t Count() const noexcept
    {
      return m_elements.size();
    }

    constexpr const VertexElement* Data() const noexcept
    {
      return m_elements.data();
    }

    constexpr VertexElement At(const std::size_t index) const
    {
      return m_elements.at(index);
    }

    //! @brief Get direct access to the elements
    constexpr const VertexElement* DirectAccess() const noexcept
    {
      return m_elements.data();
    }

    constexpr const std::array<VertexElement, TEntries>& DirectArrayAccess() const noexcept
    {
      return m_elements;
    }

    //! @brief Get the element index of for the given usage and usageIndex (if not found a NotFoundException is thrown)
    constexpr int32_t VertexElementGetIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      const int32_t index = VertexElementIndexOf(usage, usageIndex);
      if (index < 0)
      {
        throw NotFoundException("Could not locate a vertex element of the requested type");
      }
      return index;
    }

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    constexpr int32_t VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const noexcept
    {
      for (std::size_t i = 0; i < m_elements.size(); ++i)
      {
        if (m_elements[i].Usage == usage && m_elements[i].UsageIndex == usageIndex)
        {
          return static_cast<int32_t>(i);
        }
      }
      return -1;
    }

    //! @brief Get the element for the given usage and usageIndex (if not found a NotFoundException is thrown)
    constexpr VertexElement VertexElementGet(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      for (auto element : m_elements)
      {
        if (element.Usage == usage && element.UsageIndex == usageIndex)
        {
          return element;
        }
      }
      throw NotFoundException("Could not locate a vertex element of the requested type");
    }

    constexpr ReadOnlySpan<VertexElement> AsReadOnlySpan() const noexcept
    {
      return ReadOnlySpan<VertexElement>(m_elements.data(), m_elements.size());
    }


    constexpr bool operator==(const BasicVertexDeclarationArray& rhs) const noexcept
    {
      return (m_minimumVertexSize == rhs.m_minimumVertexSize && m_elements == rhs.m_elements);
    }

    constexpr bool operator!=(const BasicVertexDeclarationArray& rhs) const noexcept
    {
      return !(*this == rhs);
    }

  private:
    constexpr static bool Contains(const std::array<VertexElement, TEntries>& elements, const VertexElementUsage usage, const uint32_t usageIndex,
                                   const std::size_t ignoreIndex) noexcept
    {
      for (std::size_t i = 0; i < elements.size(); ++i)
      {
        if (i != ignoreIndex && elements[i].Usage == usage && elements[i].UsageIndex == usageIndex)
        {
          return true;
        }
      }
      return false;
    }

    constexpr static uint32_t VerifyElementsAndCalcMinimumVertexSize(const std::array<VertexElement, TEntries>& elements)
    {
      // We expect the element offsets to be in order: smallest -> largest
      uint32_t maxOffset = 0;
      for (std::size_t i = 0; i < elements.size(); ++i)
      {
        auto offset = elements[i].Offset;
        if (offset < maxOffset)
        {
          throw NotSupportedException("The offsets should be in order: smallest -> largest");
        }

        maxOffset = offset + VertexElementFormatUtil::GetBytesPerElement(elements[i].Format);

        if (Contains(elements, elements[i].Usage, elements[i].UsageIndex, i))
        {
          throw NotSupportedException("Duplicated elements: A usage and usageIndex can only be listed once");
        }
      }
      return maxOffset;
    }
  };
}

#endif
