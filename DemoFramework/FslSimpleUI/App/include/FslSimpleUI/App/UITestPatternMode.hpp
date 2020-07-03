#ifndef FSLSIMPLEUI_APP_UITESTPATTERNMODE_HPP
#define FSLSIMPLEUI_APP_UITESTPATTERNMODE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

namespace Fsl
{
  enum class UITestPatternMode : uint32_t
  {
    // No test pattern atlas is used
    Disabled = 0,
    // The test pattern overwrites the original texture atlas
    Enabled = 1,

    AllowSwitching = 2,

    // A extra test pattern atlas is created which can be enabled later. Its possible to switch between the test atlas and the real one
    // fairly easily (but it consumes roughly 2-3 times more memory)
    DisabledAllowSwitching = AllowSwitching | Disabled,
    // A extra test pattern atlas is created and enabled. Its also possible to switch between the test atlas and the real one
    // fairly easily (but it consumes roughly 2-3 times more memory)
    EnabledAllowSwitching = AllowSwitching | Enabled
  };

  constexpr inline UITestPatternMode operator|(const UITestPatternMode lhs, const UITestPatternMode rhs)
  {
    return static_cast<UITestPatternMode>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
  }

  constexpr inline UITestPatternMode operator&(const UITestPatternMode lhs, const UITestPatternMode rhs)
  {
    return static_cast<UITestPatternMode>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
  }

  namespace UITestPatternModeUtil
  {
    constexpr inline bool IsFlagged(const UITestPatternMode src, const UITestPatternMode flag)
    {
      return (src & flag) == flag;
    }
  }
}

#endif
