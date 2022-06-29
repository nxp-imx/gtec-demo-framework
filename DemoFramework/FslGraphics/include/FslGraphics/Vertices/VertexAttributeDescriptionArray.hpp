#ifndef FSLGRAPHICS_VERTICES_VERTEXATTRIBUTEDESCRIPTIONARRAY_HPP
#define FSLGRAPHICS_VERTICES_VERTEXATTRIBUTEDESCRIPTIONARRAY_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescription.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescriptionSpan.hpp>
#include <FslGraphics/Vertices/VertexElementFormatUtil.hpp>
#include <array>
#include <initializer_list>

namespace Fsl
{
  template <std::size_t TEntries>
  class VertexAttributeDescriptionArray
  {
    std::array<VertexAttributeDescription, TEntries> m_elements{};

  public:
    // Request that the compiler generates a copy constructor and assignment operator
    VertexAttributeDescriptionArray(const VertexAttributeDescriptionArray&) = default;
    VertexAttributeDescriptionArray& operator=(const VertexAttributeDescriptionArray&) = default;

    constexpr VertexAttributeDescriptionArray() = default;

    // C++17
    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements must be sorted according to location (smallest to largest)
    explicit constexpr VertexAttributeDescriptionArray(VertexAttributeDescriptionSpan srcSpan)
      : m_elements()
    {
      if (srcSpan.Count() != TEntries)
      {
        throw std::invalid_argument("incompatible sized src span");
      }
      for (std::size_t i = 0; i < TEntries; ++i)
      {
        m_elements[i] = srcSpan[i];
      }
    }

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements must be sorted according to location (smallest to largest)
    constexpr VertexAttributeDescriptionArray(std::initializer_list<VertexAttributeDescription> list)
      : m_elements()
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
      VerifyElements(m_elements);
    }


    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements must be sorted according to location (smallest to largest)
    explicit constexpr VertexAttributeDescriptionArray(const std::array<VertexAttributeDescription, TEntries>& srcElements)
      : m_elements(srcElements)
    {
      VerifyElements(m_elements);
    }

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements must be sorted according to location (smallest to largest)
    constexpr void Reset(const std::array<VertexAttributeDescription, TEntries>& srcElements)
    {
      m_elements = srcElements;
      VerifyElements(m_elements);
    }

    //! @brief Get the number of elements
    constexpr std::size_t Count() const noexcept
    {
      return m_elements.size();
    }

    constexpr const VertexAttributeDescription* Data() const noexcept
    {
      return m_elements.data();
    }

    constexpr VertexAttributeDescription At(const std::size_t index) const
    {
      return m_elements.at(index);
    }

    //! @brief Get direct access to the elements
    constexpr const VertexAttributeDescription* DirectAccess() const noexcept
    {
      return m_elements.data();
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
    constexpr VertexAttributeDescription VertexElementGet(const VertexElementUsage usage, const uint32_t usageIndex) const
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

    constexpr VertexAttributeDescriptionSpan AsReadOnlySpan() const noexcept
    {
      return {ReadOnlySpan<VertexAttributeDescription>(m_elements.data(), m_elements.size()), OptimizationCheckFlag::NoCheck};
    }


    constexpr bool operator==(const VertexAttributeDescriptionArray& rhs) const noexcept
    {
      return m_elements == rhs.m_elements;
    }

    constexpr bool operator!=(const VertexAttributeDescriptionArray& rhs) const noexcept
    {
      return !(*this == rhs);
    }

  private:
    constexpr static bool Contains(const std::array<VertexAttributeDescription, TEntries>& elements, const VertexElementUsage usage,
                                   const uint32_t usageIndex, const std::size_t ignoreIndex)
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

    // Rewritten because GCC 5 has issues with it
    constexpr static void VerifyElements(const std::array<VertexAttributeDescription, TEntries>& elements)
    {
      // We expect the element locations to be in order: smallest -> largest
      uint32_t maxLocation = 0;
      for (std::size_t i = 0; i < elements.size(); ++i)
      {
        auto location = elements[i].Location;
        if (location < maxLocation)
        {
          throw NotSupportedException("The locations should be in order: smallest -> largest");
        }

        maxLocation = location;

        if (Contains(elements, elements[i].Usage, elements[i].UsageIndex, i))
        {
          throw NotSupportedException("Duplicated elements: A usage and usageIndex can only be listed once");
        }
      }
    }
  };
}

#endif
