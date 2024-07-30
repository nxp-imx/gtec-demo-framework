#ifndef FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLE_HPP
#define FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLE_HPP
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
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElementFormat.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElementFormatUtil.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>

namespace Fsl
{
  // Represents a memory region containing a single variable (treat it like a temporary stack only data type).
  class BasicMaterialVariable
  {
  public:
    using size_type = uint32_t;
    using difference_type = std::ptrdiff_t;
    using const_pointer = const void*;


    const const_pointer Data{nullptr};
    const BasicMaterialVariableElementFormat ElementFormat{BasicMaterialVariableElementFormat::Undefined};

    constexpr BasicMaterialVariable() noexcept = default;
    constexpr BasicMaterialVariable(const BasicMaterialVariable& other) noexcept = default;


    explicit constexpr BasicMaterialVariable(const_pointer pData, const size_type dataByteSize, const size_type dataOffset,
                                             const BasicMaterialVariableElementFormat elementFormat)
      : Data(pData != nullptr ? static_cast<const uint8_t*>(pData) + dataOffset : nullptr)
      , ElementFormat(elementFormat)
    {
      if (pData == nullptr && elementFormat != BasicMaterialVariableElementFormat::Undefined)
      {
        throw std::invalid_argument("if pData is null elementFormat must be Undefined");
      }
      if (dataOffset > dataByteSize)
      {
        throw std::invalid_argument("dataOffset is invalid");
      }
      if (BasicMaterialVariableElementFormatUtil::GetBytesPerElement(elementFormat) > (dataByteSize - dataOffset))
      {
        throw std::invalid_argument("element is out of bounds");
      }
    }

    //! @brief overload that allows you to create a BasicMaterialFlexSpan from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr BasicMaterialVariable(const_pointer pData, [[maybe_unused]] const size_type dataByteSize,
                                             [[maybe_unused]] const size_type dataOffset, const BasicMaterialVariableElementFormat elementFormat,
                                             const OptimizationCheckFlag /*flag*/) noexcept
      : Data(pData)
      , ElementFormat(elementFormat)
    {
      // material data must be compatible with the supplied materialDeclaration
      assert(pData == nullptr || elementFormat == BasicMaterialVariableElementFormat::Undefined);
      assert(dataOffset <= dataByteSize);
      assert(UncheckedNumericCast<uint32_t>(BasicMaterialVariableElementFormatUtil::TryGetBytesPerElement(elementFormat)) <=
             (dataByteSize - dataOffset));
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool empty() const noexcept
    {
      return ElementFormat == BasicMaterialVariableElementFormat::Undefined;
    }
  };
}

#endif
