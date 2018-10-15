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

namespace Fsl
{
  namespace UI
  {
    struct WindowFlags
    {
    private:
      uint32_t m_value{0};

    public:
      static const int BitsReserved = 8;
      static const uint32_t MASK = (1 << BitsReserved) - 1;

      enum Enum
      {
        //! enabled the WinInit callback (called when the window has been added to the window manager
        WinInit = 0x01,
        LayoutDirty = 0x02,
        UpdateEnabled = 0x04,
        DrawEnabled = 0x08,
        ClickInput = 0x10,
        //! enable the WinResolve callback (called before the window layout cycle is started to help resolve complex state issues)
        ResolveEnabled = 0x20,

        All = WinInit | LayoutDirty | UpdateEnabled | DrawEnabled | ClickInput | ResolveEnabled,
      };


      WindowFlags() = default;


      WindowFlags(const Enum flags)
        : m_value(flags)
      {
      }


      explicit WindowFlags(const uint32_t flags)
        : m_value(flags)
      {
      }

      inline uint32_t GetValue() const
      {
        return m_value;
      }

      inline bool IsEnabled(Enum flag) const
      {
        return (m_value & static_cast<uint32_t>(flag)) != 0;
      }

      inline bool IsOnlyFlagEnabled(Enum flag) const
      {
        return (m_value == static_cast<uint32_t>(flag));
      }

      inline void Enable(Enum flag)
      {
        m_value |= static_cast<uint32_t>(flag);
      }

      inline void Disable(Enum flag)
      {
        m_value &= ~static_cast<uint32_t>(flag);
      }

      inline void Set(Enum flag, const bool enabled)
      {
        m_value = enabled ? (m_value | static_cast<uint32_t>(flag)) : (m_value & ~static_cast<uint32_t>(flag));
      }
    };
  }
}

#endif
