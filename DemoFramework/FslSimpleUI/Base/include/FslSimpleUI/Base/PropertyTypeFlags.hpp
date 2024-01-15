#ifndef FSLSIMPLEUI_BASE_PROPERTYTYPEFLAGS_HPP
#define FSLSIMPLEUI_BASE_PROPERTYTYPEFLAGS_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslSimpleUI/Base/PropertyType.hpp>

namespace Fsl::UI
{
  struct PropertyTypeFlags
  {
    static const int BitsReserved = 16;
    static const uint16_t MASK_PropertyTypeFlags = static_cast<uint16_t>(PropertyType::Content) | static_cast<uint16_t>(PropertyType::ContentDraw) |
                                                   static_cast<uint16_t>(PropertyType::Layout) | static_cast<uint16_t>(PropertyType::Alignment) |
                                                   static_cast<uint16_t>(PropertyType::ScalePolicy) | static_cast<uint16_t>(PropertyType::BaseColor) |
                                                   static_cast<uint16_t>(PropertyType::Other);

    static const uint16_t MASK_LayoutRelated = static_cast<uint16_t>(PropertyType::Content) | static_cast<uint16_t>(PropertyType::Layout);

    static const uint16_t MASK_DrawRelated = MASK_LayoutRelated | static_cast<uint16_t>(PropertyType::ContentDraw) |
                                             static_cast<uint16_t>(PropertyType::Alignment) | static_cast<uint16_t>(PropertyType::ScalePolicy) |
                                             static_cast<uint16_t>(PropertyType::BaseColor) | static_cast<uint16_t>(PropertyType::Other);

    static const uint16_t MASK_Content = static_cast<uint16_t>(PropertyType::Content) | static_cast<uint16_t>(PropertyType::ContentDraw);

    uint16_t Value{0};

    constexpr PropertyTypeFlags() noexcept = default;


    constexpr PropertyTypeFlags(PropertyType type) noexcept    // NOLINT(google-explicit-constructor)
      : Value(static_cast<uint16_t>(type))
    {
    }


    constexpr bool IsFlagged(const PropertyType type) const noexcept
    {
      return (Value & static_cast<uint16_t>(type)) == static_cast<uint16_t>(type);
    }

    //! @brief check if parts or all of the flags are enabled
    constexpr bool IsPartiallyEnabled(const PropertyType type) const noexcept
    {
      return (Value & static_cast<uint16_t>(type)) != 0;
    }

    constexpr bool IsLayoutRelated() const noexcept
    {
      return (Value & MASK_LayoutRelated) != 0;
    }

    constexpr bool IsDrawRelated() const noexcept
    {
      return (Value & MASK_DrawRelated) != 0;
    }

    //! @brief Gets the value ensuring all non PropertyTypeFlags are set to zero.
    constexpr uint16_t GetSafeValue() const noexcept
    {
      return (Value & MASK_PropertyTypeFlags);
    }
  };
}

#endif
