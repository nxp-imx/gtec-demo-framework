#ifndef FSLGRAPHICS3D_BASICRENDER_ADAPTER_NATIVETEXTUREFACTORYCAPS_HPP
#define FSLGRAPHICS3D_BASICRENDER_ADAPTER_NATIVETEXTUREFACTORYCAPS_HPP
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
    enum class NativeTextureFactoryCaps : uint32_t
    {
      // A empty flag
      NotDefined = 0x00,

      // The texture factory supports the creation of dynamic textures where you can call SetData on them.
      Dynamic = 0x01,

      // The texture coordinates should be flipped in Y
      TextureCoordinatesFlipY = 0x02,
    };

    constexpr inline NativeTextureFactoryCaps operator|(const NativeTextureFactoryCaps lhs, const NativeTextureFactoryCaps rhs)
    {
      return static_cast<NativeTextureFactoryCaps>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }

    constexpr inline NativeTextureFactoryCaps operator&(const NativeTextureFactoryCaps lhs, const NativeTextureFactoryCaps rhs)
    {
      return static_cast<NativeTextureFactoryCaps>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
    }


    namespace NativeTextureFactoryCapsUtil
    {
      inline bool IsEnabled(const NativeTextureFactoryCaps srcFlag, NativeTextureFactoryCaps flag)
      {
        return (srcFlag & flag) == flag;
      }

      inline void Enable(NativeTextureFactoryCaps& rDstFlag, NativeTextureFactoryCaps flag)
      {
        rDstFlag = rDstFlag | flag;
      }


      inline void Disable(NativeTextureFactoryCaps& rDstFlag, NativeTextureFactoryCaps flag)
      {
        rDstFlag = rDstFlag & (NativeTextureFactoryCaps(~static_cast<uint32_t>(flag)));
      }

      inline void Set(NativeTextureFactoryCaps& rDstFlag, NativeTextureFactoryCaps flag, const bool enabled)
      {
        rDstFlag = enabled ? (rDstFlag | flag) : (rDstFlag & (NativeTextureFactoryCaps(~static_cast<uint32_t>(flag))));
      }
    };
  }
}

#endif
