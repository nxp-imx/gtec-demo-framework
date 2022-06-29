#ifndef FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLEDECLARATIONARRAY_HPP
#define FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLEDECLARATIONARRAY_HPP
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

#include <FslBase/Attributes.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclarationSpan.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElement.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElementFormatUtil.hpp>
#include <array>

namespace Fsl
{
  template <std::size_t TEntries>
  class BasicMaterialVariableDeclarationArray
  {
    std::array<BasicMaterialVariableElement, TEntries> m_elements{};
    uint32_t m_stride{0};

  public:
    // Request that the compiler generates a copy constructor and assignment operator
    BasicMaterialVariableDeclarationArray(const BasicMaterialVariableDeclarationArray&) = default;
    BasicMaterialVariableDeclarationArray& operator=(const BasicMaterialVariableDeclarationArray&) = default;

    constexpr BasicMaterialVariableDeclarationArray() = default;

    // C++17
    //! @brief Create a material declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    explicit constexpr BasicMaterialVariableDeclarationArray(BasicMaterialVariableDeclarationSpan srcSpan)
      : m_elements()
      , m_stride(srcSpan.stride())
    {
      if (srcSpan.size() != TEntries)
      {
        throw std::invalid_argument("incompatible sized src span");
      }
      for (std::size_t i = 0; i < TEntries; ++i)
      {
        m_elements[i] = srcSpan.At(i);
      }
      // We do not need to verify the elements as the span has been validated
      // VerifyElements(m_elements, m_stride);
    }


    //! @brief Create a material declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    constexpr BasicMaterialVariableDeclarationArray(const std::array<BasicMaterialVariableElement, TEntries>& srcElements, const uint32_t stride)
      : m_elements(srcElements)
      , m_stride(stride)
    {
      // CopyElements(m_elements, srcElements);
      VerifyElements(m_elements, m_stride);
    }

    //! @brief Create a material declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    constexpr void Reset(const std::array<BasicMaterialVariableElement, TEntries>& srcElements, const uint32_t stride)
    {
      m_elements = srcElements;
      // CopyElements(m_elements, srcElements);
      m_stride = stride;
      VerifyElements(m_elements, m_stride);
    }

    //! @brief Get the material stride
    constexpr uint32_t Stride() const
    {
      return m_stride;
    }

    //! @brief Get the number of elements
    constexpr std::size_t Count() const
    {
      return m_elements.size();
    }

    constexpr const BasicMaterialVariableElement* Data() const
    {
      return m_elements.data();
    }

    constexpr BasicMaterialVariableElement At(const std::size_t index) const
    {
      return m_elements.at(index);
    }

    //! @brief Get direct access to the elements
    constexpr const BasicMaterialVariableElement* DirectAccess() const
    {
      return m_elements.data();
    }

    //! @brief Get the element index of for the given usage and usageIndex (if not found a NotFoundException is thrown)
    constexpr int32_t BasicMaterialElementGetIndexOf(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const
    {
      const int32_t index = BasicMaterialElementIndexOf(usage, usageIndex);
      if (index < 0)
      {
        throw NotFoundException("Could not locate a material element of the requested type");
      }
      return index;
    }

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    constexpr int32_t BasicMaterialElementIndexOf(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const
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
    constexpr BasicMaterialVariableElement MaterialElementGet(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const
    {
      for (auto element : m_elements)
      {
        if (element.Usage == usage && element.UsageIndex == usageIndex)
        {
          return element;
        }
      }
      throw NotFoundException("Could not locate a material element of the requested type");
    }

    constexpr BasicMaterialVariableDeclarationSpan AsReadOnlySpan() const noexcept
    {
      return {ReadOnlySpan<BasicMaterialVariableElement>(m_elements.data(), m_elements.size()), m_stride, OptimizationCheckFlag::NoCheck};
    }


    constexpr bool operator==(const BasicMaterialVariableDeclarationArray& rhs) const
    {
      return (m_stride == rhs.m_stride && m_elements == rhs.m_elements);
    }

    constexpr bool operator!=(const BasicMaterialVariableDeclarationArray& rhs) const
    {
      return !(*this == rhs);
    }

  private:
    constexpr static bool Contains(const std::array<BasicMaterialVariableElement, TEntries>& elements, const BasicMaterialVariableElementUsage usage,
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
    constexpr static void VerifyElements(const std::array<BasicMaterialVariableElement, TEntries>& elements, const uint32_t stride)
    {
      // We expect the element offsets to be in order: smallest -> largest
      uint32_t maxOffset = 0;
      for (std::size_t i = 0; i < elements.size(); ++i)
      {
        auto offset = elements[i].Offset + BasicMaterialVariableElementFormatUtil::GetBytesPerElement(elements[i].Format);
        if (offset < maxOffset)
        {
          throw NotSupportedException("The offsets should be in order: smallest -> largest");
        }

        maxOffset = offset;

        if (Contains(elements, elements[i].Usage, elements[i].UsageIndex, i))
        {
          throw NotSupportedException("Duplicated elements: A usage and usageIndex can only be listed once");
        }
      }

      if (maxOffset > stride)
      {
        throw NotSupportedException("At least one of the elements is outside of the stride stride");
      }
    }
  };
}

#endif
