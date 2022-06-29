#ifndef FSLSIMPLEUI_BASE_BASEWINDOWFLAGS_HPP
#define FSLSIMPLEUI_BASE_BASEWINDOWFLAGS_HPP
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
#include <FslSimpleUI/Base/ItemVisibility.hpp>
#include <FslSimpleUI/Base/WindowFlags.hpp>

namespace Fsl::UI
{
  struct BaseWindowFlags
  {
    static constexpr const int BitShiftBaseWindowFlags = WindowFlags::BitsReserved;
    static constexpr const uint32_t MASK_WindowFlags = (1 << WindowFlags::BitsReserved) - 1;

    enum Enum
    {
      LayoutDirty = WindowFlags::LayoutDirty,
      UpdateEnabled = WindowFlags::UpdateEnabled,
      DrawEnabled = WindowFlags::DrawEnabled,
      VisibilityMask = WindowFlags::VisibilityMask,
      InBatchPropertyUpdate = (0x01 << BitShiftBaseWindowFlags),
      InLayoutArrange = (0x02 << BitShiftBaseWindowFlags),
      InLayoutMeasure = (0x04 << BitShiftBaseWindowFlags),
      CachedEventReady = (0x08 << BitShiftBaseWindowFlags)
    };

    uint32_t Value{0};

    constexpr BaseWindowFlags() noexcept = default;

    explicit constexpr BaseWindowFlags(const uint32_t flags) noexcept
      : Value(flags)
    {
    }

    explicit constexpr BaseWindowFlags(const WindowFlags flags) noexcept
      : Value(flags.GetValue())
    {
    }

    inline constexpr bool IsEnabled(Enum flag) const noexcept
    {
      return (Value & static_cast<uint32_t>(flag)) != 0;
    }

    inline bool IsEnabled(const WindowFlags& flags) const
    {
      return (Value & flags.GetValue()) == flags.GetValue();
    }

    inline constexpr void Enable(Enum flag) noexcept
    {
      Value |= static_cast<uint32_t>(flag);
    }

    inline constexpr void Enable(const WindowFlags flags) noexcept
    {
      Value |= flags.GetValue();
    }

    inline constexpr void Disable(Enum flag) noexcept
    {
      Value &= ~static_cast<uint32_t>(flag);
    }

    inline constexpr void Disable(const WindowFlags flags) noexcept
    {
      Value &= ~flags.GetValue();
    }

    inline constexpr void Set(Enum flag, const bool enabled) noexcept
    {
      Value = enabled ? (Value | static_cast<uint32_t>(flag)) : (Value & ~static_cast<uint32_t>(flag));
    }

    constexpr inline ItemVisibility GetVisibility() const noexcept
    {
      return static_cast<ItemVisibility>((Value & WindowFlags::VisibilityMask) >> WindowFlags::VisibilityShift);
    }

    constexpr inline void SetVisibility(const ItemVisibility visibility) noexcept
    {
      Value = (Value & (~WindowFlags::VisibilityMask)) |
              ((static_cast<uint32_t>(visibility) << WindowFlags::VisibilityShift) & WindowFlags::VisibilityMask);
    }

    //! Conversion operator for easy conversion to WindowFlags
    explicit constexpr operator WindowFlags() const noexcept
    {
      return WindowFlags(Value & MASK_WindowFlags);
    }
  };
}

#endif
