#ifndef FSLSIMPLEUI_RENDER_BASE_COMMAND_ENCODEDCOMMANDSTATE_HPP
#define FSLSIMPLEUI_RENDER_BASE_COMMAND_ENCODEDCOMMANDSTATE_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslSimpleUI/Render/Base/Command/DrawCommandType.hpp>
#include <FslSimpleUI/Render/Base/Command/EncodedCommandStateFlags.hpp>
#include <cassert>

namespace Fsl::UI
{
  struct EncodedCommandState
  {
  private:
    EncodedCommandStateFlags m_flags{EncodedCommandStateFlags::Nothing};

  public:
    constexpr EncodedCommandState() noexcept = default;

    constexpr explicit EncodedCommandState(const DrawCommandType commandType) noexcept
    {
      assert(static_cast<uint32_t>(commandType) <= static_cast<uint32_t>(EncodedCommandStateFlags::MaskDrawCommandType));
      m_flags = static_cast<EncodedCommandStateFlags>(commandType);
    }

    constexpr EncodedCommandState(const DrawCommandType commandType, const bool clippingEnabled) noexcept
    {
      assert(static_cast<uint32_t>(commandType) <= static_cast<uint32_t>(EncodedCommandStateFlags::MaskDrawCommandType));

      m_flags = static_cast<EncodedCommandStateFlags>(commandType);
      m_flags |= clippingEnabled ? EncodedCommandStateFlags::ClipEnabled : EncodedCommandStateFlags::Nothing;
    }

    constexpr DrawCommandType Type() const noexcept
    {
      return static_cast<DrawCommandType>(m_flags & EncodedCommandStateFlags::MaskDrawCommandType);
    }


    constexpr bool IsClipEnabled() const noexcept
    {
      return EncodedCommandStateFlagsUtil::IsEnabled(m_flags, EncodedCommandStateFlags::ClipEnabled);
    }

    constexpr bool operator==(const EncodedCommandState rhs) const noexcept
    {
      return m_flags == rhs.m_flags;
    }

    constexpr bool operator!=(const EncodedCommandState rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
