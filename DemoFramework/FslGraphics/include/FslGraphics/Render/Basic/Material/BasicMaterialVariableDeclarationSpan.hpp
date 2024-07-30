#ifndef FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLEDECLARATIONSPAN_HPP
#define FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLEDECLARATIONSPAN_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/SpanUtil_ValueCompare.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElement.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElementFormatUtil.hpp>
#include <array>

namespace Fsl
{
  class BasicMaterialVariableDeclarationSpan
  {
    ReadOnlySpan<BasicMaterialVariableElement> m_span;
    uint32_t m_materialStride{0};

  public:
    // Request that the compiler generates a copy constructor and assignment operator
    BasicMaterialVariableDeclarationSpan(const BasicMaterialVariableDeclarationSpan&) = default;
    BasicMaterialVariableDeclarationSpan& operator=(const BasicMaterialVariableDeclarationSpan&) = default;

    constexpr BasicMaterialVariableDeclarationSpan() noexcept = default;

    //! @brief Create a material declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    constexpr BasicMaterialVariableDeclarationSpan(const BasicMaterialVariableElement* const pSrcElements, const std::size_t elementCount,
                                                   const uint32_t materialStride)
      : m_span(pSrcElements, elementCount)
      , m_materialStride(materialStride)
    {
      if (elementCount > 0xFFFFFFFFu || (elementCount > 0 && materialStride == 0))
      {
        throw std::invalid_argument("elementCount must fit in a UInt32, and the stride must be valid");
      }
      VerifyElements(m_span, m_materialStride);
    }

    //! @brief Create a material declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    constexpr BasicMaterialVariableDeclarationSpan(const ReadOnlySpan<BasicMaterialVariableElement>& srcElements, const uint32_t materialStride)
      : m_span(srcElements)
      , m_materialStride(materialStride)
    {
      if (srcElements.size() > 0xFFFFFFFFu || (!srcElements.empty() && materialStride == 0))
      {
        throw std::invalid_argument("elementCount must fit in a UInt32, and the stride must be valid");
      }
      VerifyElements(srcElements, m_materialStride);
    }

    //! @brief Create a material declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    constexpr BasicMaterialVariableDeclarationSpan(const ReadOnlySpan<BasicMaterialVariableElement>& srcElements, const uint32_t materialStride,
                                                   const OptimizationCheckFlag /*flag*/) noexcept
      : m_span(srcElements)
      , m_materialStride(materialStride)
    {
      assert(srcElements.size() <= 0xFFFFFFFFu);
      assert(srcElements.size() == 0 || materialStride > 0u);
      assert(IsValidElements(srcElements, m_materialStride));
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool empty() const
    {
      return m_span.empty();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const BasicMaterialVariableElement* data() const
    {
      return m_span.data();
    }

    //! @brief Get the number of elements
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr uint32_t size() const
    {
      assert(m_span.size() <= 0xFFFFFFFFu);
      return static_cast<uint32_t>(m_span.size());
    }

    //! @brief Get the material stride
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr uint32_t stride() const
    {
      return m_materialStride;
    }

    constexpr ReadOnlySpan<BasicMaterialVariableElement> AsReadOnlySpan() const
    {
      return m_span;
    }

    constexpr BasicMaterialVariableElement At(const std::size_t index) const
    {
      return m_span.at(index);
    }

    //! @brief Get the element index of for the given usage and usageIndex (if not found a NotFoundException is thrown)
    constexpr int32_t MaterialElementGetIndexOf(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const
    {
      const int32_t index = MaterialElementIndexOf(usage, usageIndex);
      if (index < 0)
      {
        throw NotFoundException("Could not locate a material element of the requested type");
      }
      return index;
    }

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    constexpr int32_t MaterialElementIndexOf(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const
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
    constexpr const BasicMaterialVariableElement& MaterialElementGet(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const
    {
      return m_span[MaterialElementGetIndexOf(usage, usageIndex)];
    }

    constexpr bool operator==(const BasicMaterialVariableDeclarationSpan& rhs) const
    {
      return (m_materialStride == rhs.m_materialStride && SpanUtil::ValueEquals(m_span, rhs.m_span));
    }

    constexpr bool operator!=(const BasicMaterialVariableDeclarationSpan& rhs) const
    {
      return !(*this == rhs);
    }


  private:
    constexpr static bool Contains(const ReadOnlySpan<BasicMaterialVariableElement>& srcElements, const BasicMaterialVariableElementUsage usage,
                                   const uint32_t usageIndex, const std::size_t ignoreIndex)
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

    constexpr static void VerifyElements(const ReadOnlySpan<BasicMaterialVariableElement>& srcElements, const uint32_t materialStride)
    {
      // We expect the element offsets to be in order: smallest -> largest
      uint32_t maxOffset = 0;
      for (std::size_t i = 0; i < srcElements.size(); ++i)
      {
        auto offset = srcElements[i].Offset + BasicMaterialVariableElementFormatUtil::GetBytesPerElement(srcElements[i].Format);
        if (offset < maxOffset)
        {
          throw NotSupportedException("The offsets should be in order: smallest -> largest");
        }

        maxOffset = offset;

        if (Contains(srcElements, srcElements[i].Usage, srcElements[i].UsageIndex, i))
        {
          throw NotSupportedException("Duplicated elements: A usage and usageIndex can only be listed once");
        }
      }

      if (maxOffset > materialStride)
      {
        throw NotSupportedException("At least one of the elements is outside of the material stride");
      }
    }

    constexpr static bool IsValidElements(const ReadOnlySpan<BasicMaterialVariableElement>& srcElements, const uint32_t materialStride)
    {
      // We expect the element offsets to be in order: smallest -> largest
      uint32_t maxOffset = 0;
      for (std::size_t i = 0; i < srcElements.size(); ++i)
      {
        auto offset = srcElements[i].Offset + BasicMaterialVariableElementFormatUtil::GetBytesPerElement(srcElements[i].Format);
        if (offset < maxOffset)
        {
          return false;
        }

        maxOffset = offset;

        if (Contains(srcElements, srcElements[i].Usage, srcElements[i].UsageIndex, i))
        {
          return false;
        }
      }
      return (maxOffset <= materialStride);
    }
  };
}

#endif
