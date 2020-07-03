#ifndef FSLSIMPLEUI_BASE_LAYOUT_GRIDROWCOLUMNDEFINITION_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_GRIDROWCOLUMNDEFINITION_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

namespace Fsl
{
  namespace UI
  {
    enum class GridUnitType
    {
      //! Fixed size in dp
      Fixed = 0,
      //! Fixed size in px
      FixedPx = 1,
      //! Auto value, any specified size is ignored
      Auto = 2,
      //! Star
      Star = 3
    };

    struct GridRowColumnDefinitionBase
    {
      GridUnitType Unit;
      float Size;

      //! @brief defaults to Star of size 1
      constexpr GridRowColumnDefinitionBase() noexcept
        : GridRowColumnDefinitionBase(GridUnitType::Star, 1.0f)
      {
      }

      //! @brief Configurable construction with fixed size 1
      //! @param unit Unit type
      explicit constexpr GridRowColumnDefinitionBase(const GridUnitType unit) noexcept
        : GridRowColumnDefinitionBase(unit, 1.0f)
      {
      }

      //! @brief Configurable construction
      //! @param unit Unit type, determining how size parameter is interpreted
      //! @param size Size (height/width) of row/column. Must not be Size::Auto(). Is ignored if weight is Auto.
      //! @exception BFC::InvalidArgumentException if size has Size::Auto() value
      constexpr GridRowColumnDefinitionBase(const GridUnitType unit, const float size) noexcept
        : Unit(unit)
        , Size(size)
      {
      }

      constexpr bool operator==(const GridRowColumnDefinitionBase& rhs) const noexcept
      {
        return Unit == rhs.Unit && Size == rhs.Size;
      }

      constexpr bool operator!=(const GridRowColumnDefinitionBase& rhs) const noexcept
      {
        return Unit != rhs.Unit || Size != rhs.Size;
      }
    };

    struct GridColumnDefinition : public GridRowColumnDefinitionBase
    {
      constexpr GridColumnDefinition() = default;
      explicit constexpr GridColumnDefinition(const GridUnitType unit)
        : GridRowColumnDefinitionBase(unit)
      {
      }
      constexpr GridColumnDefinition(const GridUnitType unit, const float size)
        : GridRowColumnDefinitionBase(unit, size)
      {
      }

      explicit constexpr GridColumnDefinition(const GridRowColumnDefinitionBase& base)
        : GridRowColumnDefinitionBase(base)
      {
      }
    };

    struct GridRowDefinition : public GridRowColumnDefinitionBase
    {
      constexpr GridRowDefinition() = default;
      explicit constexpr GridRowDefinition(const GridUnitType unit)
        : GridRowColumnDefinitionBase(unit)
      {
      }
      constexpr GridRowDefinition(const GridUnitType unit, const float size)
        : GridRowColumnDefinitionBase(unit, size)
      {
      }
      explicit constexpr GridRowDefinition(const GridRowColumnDefinitionBase& base)
        : GridRowColumnDefinitionBase(base)
      {
      }
    };
  }
}

#endif
