#ifndef FSLSIMPLEUI_BASE_WINDOWFLAGS_HPP
#define FSLSIMPLEUI_BASE_WINDOWFLAGS_HPP
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

namespace Fsl::UI
{
  static_assert(static_cast<uint32_t>(ItemVisibility::Visible) == 0, "ItemVisibility enum expectation not met");
  static_assert(static_cast<uint32_t>(ItemVisibility::Hidden) == 1, "ItemVisibility enum expectation not met");
  static_assert(static_cast<uint32_t>(ItemVisibility::Collapsed) == 2, "ItemVisibility enum expectation not met");

  struct WindowFlags
  {
  private:
    uint32_t m_value{0};

  public:
    static constexpr int FlagBitsReserved = 10;
    static constexpr int VisibilityBitsReserved = 2;
    static constexpr int VisibilityShift = FlagBitsReserved;
    static constexpr const uint32_t VisibilityMask = (0x1 | 0x2) << VisibilityShift;

    static constexpr int BitsReserved = FlagBitsReserved + VisibilityBitsReserved;
    static constexpr const uint32_t MASK = (1 << BitsReserved) - 1;

    enum Enum
    {
      //! enabled the WinInit callback (called when the window has been added to the window manager
      WinInit = 0x01,
      LayoutDirty = 0x02,
      //! Mark rendered content as dirty
      ContentRenderingDirty = 0x04,
      UpdateEnabled = 0x08,
      DrawEnabled = 0x10,
      ClickInput = 0x20,
      MouseOver = 0x40,
      //! enable the WinResolve callback (called before the window layout cycle is started to help resolve complex state issues)
      ResolveEnabled = 0x100,
      //! enable the WinPostLayout callback (called after layout has been resolved)
      PostLayoutEnabled = 0x200,

      All = WinInit | LayoutDirty | UpdateEnabled | DrawEnabled | ClickInput | MouseOver | ResolveEnabled | PostLayoutEnabled | ContentRenderingDirty,
      InputAll = ClickInput | MouseOver
    };


    constexpr WindowFlags() noexcept = default;


    constexpr WindowFlags(const Enum flags) noexcept    // NOLINT(google-explicit-constructor)
      : m_value(flags)
    {
    }


    constexpr explicit WindowFlags(const uint32_t flags) noexcept
      : m_value(flags)
    {
    }

    constexpr inline uint32_t GetValue() const noexcept
    {
      return m_value;
    }

    constexpr inline bool IsEnabled(Enum flag) const noexcept
    {
      return (m_value & static_cast<uint32_t>(flag)) != 0;
    }

    constexpr inline bool IsOnlyFlagEnabled(Enum flag) const noexcept
    {
      return (m_value == static_cast<uint32_t>(flag));
    }

    constexpr inline void Enable(Enum flag) noexcept
    {
      m_value |= static_cast<uint32_t>(flag);
    }

    constexpr inline void Disable(Enum flag) noexcept
    {
      m_value &= ~static_cast<uint32_t>(flag);
    }

    constexpr inline void Set(Enum flag, const bool enabled) noexcept
    {
      m_value = enabled ? (m_value | static_cast<uint32_t>(flag)) : (m_value & ~static_cast<uint32_t>(flag));
    }


    constexpr inline ItemVisibility GetVisibility() const noexcept
    {
      return static_cast<ItemVisibility>((m_value & WindowFlags::VisibilityMask) >> WindowFlags::VisibilityShift);
    }

    constexpr inline void SetVisibility(const ItemVisibility visibility) noexcept
    {
      m_value = (m_value & (~WindowFlags::VisibilityMask)) |
                ((static_cast<uint32_t>(visibility) << WindowFlags::VisibilityShift) & WindowFlags::VisibilityMask);
    }
  };
}

#endif
