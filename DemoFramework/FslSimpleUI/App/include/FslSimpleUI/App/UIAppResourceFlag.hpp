#ifndef FSLSIMPLEUI_APP_UIAPPRESOURCEFLAG_HPP
#define FSLSIMPLEUI_APP_UIAPPRESOURCEFLAG_HPP
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
  //! Represents the operations that was performed
  enum class UIAppResourceFlag : uint32_t
  {
    //! A empty flag
    Undefined = 0,
    //! This texture represents a texture atlas
    Atlas = 1,
    //! The texture is dynamic (meaning it can be modified at runtime)
    Dynamic = 0x40,
    //! Disable DP awareness (legacy mode)
    NotDpAware = 0x80,
    //! This is a texture belonging to the UI group
    UIGroup = 0x1000
  };


  constexpr inline UIAppResourceFlag& operator|=(UIAppResourceFlag& rLhs, const UIAppResourceFlag rhs)
  {
    rLhs = static_cast<UIAppResourceFlag>(static_cast<uint32_t>(rLhs) | static_cast<uint32_t>(rhs));
    return rLhs;
  }

  constexpr inline UIAppResourceFlag& operator&=(UIAppResourceFlag& rLhs, const UIAppResourceFlag rhs)
  {
    rLhs = static_cast<UIAppResourceFlag>(static_cast<uint32_t>(rLhs) & static_cast<uint32_t>(rhs));
    return rLhs;
  }

  constexpr inline UIAppResourceFlag operator|(const UIAppResourceFlag lhs, const UIAppResourceFlag rhs)
  {
    return static_cast<UIAppResourceFlag>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
  }

  constexpr inline UIAppResourceFlag operator&(const UIAppResourceFlag lhs, const UIAppResourceFlag rhs)
  {
    return static_cast<UIAppResourceFlag>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
  }

  namespace UIAppResourceFlagUtil
  {
    constexpr inline bool IsFlagged(const UIAppResourceFlag src, const UIAppResourceFlag flag)
    {
      return (src & flag) == flag;
    }
  };
}

#endif
