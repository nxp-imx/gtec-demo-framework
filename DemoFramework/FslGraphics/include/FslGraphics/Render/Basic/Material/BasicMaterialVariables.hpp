#ifndef FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLES_HPP
#define FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLES_HPP
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
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariable.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclarationSpan.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>
//#include <type_traits>

namespace Fsl
{
  // Represents a memory region with push constants (treat it like a temporary stack only data type).
  class BasicMaterialVariables
  {
  public:
    using size_type = uint32_t;
    using difference_type = std::ptrdiff_t;
    using const_pointer = const void*;

  protected:
    const const_pointer m_pData{nullptr};
    const size_type m_byteSize{0u};
    const BasicMaterialVariableDeclarationSpan m_materialDeclaration;

  public:
    // constexpr BasicMaterialVariables() noexcept = default;
    constexpr BasicMaterialVariables(const BasicMaterialVariables& other) noexcept = default;


    explicit constexpr BasicMaterialVariables(const const_pointer pData, const size_type byteSize,
                                              BasicMaterialVariableDeclarationSpan materialDeclaration)
      : m_pData(pData)
      , m_byteSize(byteSize)
      , m_materialDeclaration(materialDeclaration)
    {
      if (m_pData == nullptr && m_byteSize > 0)
      {
        throw std::invalid_argument("if pData is null then byteSize must be 0");
      }
      if (materialDeclaration.empty() || m_byteSize != m_materialDeclaration.stride())
      {
        throw std::invalid_argument("material data is incompatible with the supplied BasicMaterialDeclaration");
      }
    }

    //! @brief overload that allows you to create a BasicMaterialFlexSpan from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr BasicMaterialVariables(const const_pointer pData, const size_type byteSize,
                                              BasicMaterialVariableDeclarationSpan materialDeclaration, const OptimizationCheckFlag /*flag*/) noexcept
      : m_pData(pData)
      , m_byteSize(byteSize)
      , m_materialDeclaration(materialDeclaration)
    {
      // material data must be compatible with the supplied materialDeclaration
      assert(pData != nullptr || byteSize == 0u);
      assert(m_byteSize == m_materialDeclaration.stride());
      assert(!materialDeclaration.empty());
    }

    constexpr BasicMaterialVariable GetBasicMaterialVariable(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const
    {
      const BasicMaterialVariableElement& element = m_materialDeclaration.MaterialElementGet(usage, usageIndex);
      return BasicMaterialVariable(m_pData, m_byteSize, element.Offset, element.Format);
    }


    constexpr const_pointer data() const noexcept
    {
      return m_pData;
    }

    constexpr size_type byteSize() const noexcept
    {
      return m_byteSize;
    }

    constexpr bool empty() const noexcept
    {
      return m_byteSize == 0u;
    }

    constexpr BasicMaterialVariableDeclarationSpan AsBasicMaterialDeclarationSpan() const noexcept
    {
      return m_materialDeclaration;
    }
  };
}

#endif
