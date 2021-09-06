#ifndef FSLGRAPHICS_TEXTUREATLAS_ATLASNINESLICEFLAGS_HPP
#define FSLGRAPHICS_TEXTUREATLAS_ATLASNINESLICEFLAGS_HPP
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  //! WARNING: do not modify the values of this enum as they match the BTA fileformat!
  enum class AtlasNineSliceFlags : uint32_t
  {
    //! All slices are opaque
    Opaque = 0x00,

    //! Slice0 contains transparent pixels
    Slice0Transparent = 1,
    //! Slice1 contains transparent pixels
    Slice1Transparent = 1 << 1,
    //! Slice2 contains transparent pixels
    Slice2Transparent = 1 << 2,
    //! Slice3 contains transparent pixels
    Slice3Transparent = 1 << 3,
    //! Slice4 contains transparent pixels
    Slice4Transparent = 1 << 4,
    //! Slice5 contains transparent pixels
    Slice5Transparent = 1 << 5,
    //! Slice6 contains transparent pixels
    Slice6Transparent = 1 << 6,
    //! Slice7 contains transparent pixels
    Slice7Transparent = 1 << 7,
    //! Slice8 contains transparent pixels
    Slice8Transparent = 1 << 8,

    //! All slices are transparent
    Transparent = Slice0Transparent | Slice1Transparent | Slice2Transparent | Slice3Transparent | Slice4Transparent | Slice5Transparent |
                  Slice6Transparent | Slice7Transparent | Slice8Transparent
  };

  constexpr inline AtlasNineSliceFlags operator|(const AtlasNineSliceFlags lhs, const AtlasNineSliceFlags rhs) noexcept
  {
    return static_cast<AtlasNineSliceFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
  }

  constexpr inline AtlasNineSliceFlags operator&(const AtlasNineSliceFlags lhs, const AtlasNineSliceFlags rhs) noexcept
  {
    return static_cast<AtlasNineSliceFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
  }


  namespace AtlasNineSliceFlagsUtil
  {
    constexpr inline bool IsEnabled(const AtlasNineSliceFlags srcFlag, AtlasNineSliceFlags flag) noexcept
    {
      return (srcFlag & flag) == flag;
    }

    constexpr inline void Enable(AtlasNineSliceFlags& rDstFlag, AtlasNineSliceFlags flag) noexcept
    {
      rDstFlag = rDstFlag | flag;
    }


    constexpr inline void Disable(AtlasNineSliceFlags& rDstFlag, AtlasNineSliceFlags flag) noexcept
    {
      rDstFlag = rDstFlag & (AtlasNineSliceFlags(~static_cast<uint32_t>(flag)));
    }

    constexpr inline void Set(AtlasNineSliceFlags& rDstFlag, AtlasNineSliceFlags flag, const bool enabled) noexcept
    {
      rDstFlag = enabled ? (rDstFlag | flag) : (rDstFlag & (AtlasNineSliceFlags(~static_cast<uint32_t>(flag))));
    }

    constexpr inline AtlasNineSliceFlags GetTransparencyFlags(AtlasNineSliceFlags flags) noexcept
    {
      return static_cast<AtlasNineSliceFlags>(static_cast<uint32_t>(flags) & static_cast<uint32_t>(AtlasNineSliceFlags::Transparent));
    }
  };
}

#endif
