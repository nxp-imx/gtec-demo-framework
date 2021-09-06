#ifndef FSLGRAPHICS3D_BASICRENDER_ADAPTER_NATIVEBUFFERFACTORYCAPS_HPP
#define FSLGRAPHICS3D_BASICRENDER_ADAPTER_NATIVEBUFFERFACTORYCAPS_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
  namespace Graphics3D
  {
    enum class NativeBufferFactoryCaps : uint32_t
    {
      // A empty flag
      NotDefined = 0x00,

      // The factory supports the creation of dynamic buffers where you can call SetData on them.
      Dynamic = 0x01,
    };

    constexpr inline NativeBufferFactoryCaps operator|(const NativeBufferFactoryCaps lhs, const NativeBufferFactoryCaps rhs)
    {
      return static_cast<NativeBufferFactoryCaps>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }

    constexpr inline NativeBufferFactoryCaps operator&(const NativeBufferFactoryCaps lhs, const NativeBufferFactoryCaps rhs)
    {
      return static_cast<NativeBufferFactoryCaps>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
    }


    namespace NativeBufferFactoryCapsUtil
    {
      inline bool IsEnabled(const NativeBufferFactoryCaps srcFlag, NativeBufferFactoryCaps flag)
      {
        return (srcFlag & flag) == flag;
      }

      inline void Enable(NativeBufferFactoryCaps& rDstFlag, NativeBufferFactoryCaps flag)
      {
        rDstFlag = rDstFlag | flag;
      }


      inline void Disable(NativeBufferFactoryCaps& rDstFlag, NativeBufferFactoryCaps flag)
      {
        rDstFlag = rDstFlag & (NativeBufferFactoryCaps(~static_cast<uint32_t>(flag)));
      }

      inline void Set(NativeBufferFactoryCaps& rDstFlag, NativeBufferFactoryCaps flag, const bool enabled)
      {
        rDstFlag = enabled ? (rDstFlag | flag) : (rDstFlag & (NativeBufferFactoryCaps(~static_cast<uint32_t>(flag))));
      }
    };
  }
}

#endif
