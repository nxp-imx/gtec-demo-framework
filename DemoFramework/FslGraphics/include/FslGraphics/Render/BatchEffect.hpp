#ifndef FSLGRAPHICS_RENDER_BATCHEFFECT_HPP
#define FSLGRAPHICS_RENDER_BATCHEFFECT_HPP
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

#include <cstdint>

namespace Fsl
{
  enum class BatchEffect : uint32_t
  {
    NoEffect,    // This would have been named 'None' bit it conflicts with X11 headers.
    Rotate90Clockwise = 1,
    Rotate180Clockwise = 2,
    Rotate270Clockwise = 4,
    FlipHorizontal = 0x010,
    FlipVertical = 0x020,

    RotateMask = Rotate90Clockwise | Rotate180Clockwise | Rotate270Clockwise
  };

  constexpr inline BatchEffect& operator|=(BatchEffect& rLhs, const BatchEffect rhs)
  {
    rLhs = static_cast<BatchEffect>(static_cast<uint32_t>(rLhs) | static_cast<uint32_t>(rhs));
    return rLhs;
  }

  constexpr inline BatchEffect& operator&=(BatchEffect& rLhs, const BatchEffect rhs)
  {
    rLhs = static_cast<BatchEffect>(static_cast<uint32_t>(rLhs) & static_cast<uint32_t>(rhs));
    return rLhs;
  }

  constexpr inline BatchEffect operator|(const BatchEffect lhs, const BatchEffect rhs)
  {
    return static_cast<BatchEffect>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
  }

  constexpr inline BatchEffect operator&(const BatchEffect lhs, const BatchEffect rhs)
  {
    return static_cast<BatchEffect>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
  }

  namespace BatchEffectUtil
  {
    constexpr inline bool IsFlagged(const BatchEffect src, const BatchEffect flag)
    {
      return (src & flag) == flag;
    }
  }
}

#endif
