#ifndef FSLSIMPLEUI_BASE_SYSTEM_TREENODEFLAGS_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_TREENODEFLAGS_HPP
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
#include <FslSimpleUI/Base/WindowFlags.hpp>

namespace Fsl
{
  namespace UI
  {
    struct TreeNodeFlags
    {
      uint32_t Value{0};

      enum Enum
      {
        LayoutDirty = WindowFlags::LayoutDirty,
        UpdateEnabled = WindowFlags::UpdateEnabled,
        DrawEnabled = WindowFlags::DrawEnabled,
        ClickInput = WindowFlags::ClickInput,
        ResolveEnabled = WindowFlags::ResolveEnabled,
        Disposed = 0x01 << WindowFlags::BitsReserved,
      };


      TreeNodeFlags() = default;

      TreeNodeFlags(const Enum flag)
        : Value(static_cast<uint32_t>(flag))
      {
      }

      explicit TreeNodeFlags(const WindowFlags& flags)
        : Value(flags.GetValue())
      {
      }

      explicit TreeNodeFlags(const uint32_t flags)
        : Value(flags)
      {
      }

      bool IsRunning() const
      {
        return !IsFlagged(Disposed);
      }


      bool IsDisposed() const
      {
        return IsFlagged(Disposed);
      }


      inline bool IsFlagged(Enum flag) const
      {
        return ((Value & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag));
      }


      bool IsFlagged(const WindowFlags& flags) const
      {
        return ((Value & flags.GetValue()) == flags.GetValue());
      }


      void EnableFlag(Enum flag)
      {
        Value |= static_cast<uint32_t>(flag);
      }
    };
  }
}

#endif
