#ifndef FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALINFO_HPP
#define FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALINFO_HPP
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

#include <FslBase/Math/Pixel/PxViewport.hpp>
#include <FslGraphics/Render/Basic/BasicCullMode.hpp>
#include <FslGraphics/Render/Basic/BasicFrontFace.hpp>
#include <FslGraphics/Render/Basic/BasicViewportMode.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialDepthInfo.hpp>
#include <FslGraphics/Render/BlendState.hpp>

namespace Fsl
{
  struct BasicMaterialInfo
  {
    BlendState Blend{BlendState::Additive};
    BasicCullMode CullMode{BasicCullMode::Back};
    BasicFrontFace CullFrontFace{BasicFrontFace::CounterClockwise};
    BasicMaterialDepthInfo Depth;
    BasicViewportMode ViewportMode{BasicViewportMode::Fullscreen};
    PxViewport Viewport;

    constexpr BasicMaterialInfo() noexcept = default;
    constexpr BasicMaterialInfo(const BlendState blend, const BasicCullMode cullMode, const BasicFrontFace cullFrontFace,
                                const BasicMaterialDepthInfo& depthInfo) noexcept
      : Blend(blend)
      , CullMode(cullMode)
      , CullFrontFace(cullFrontFace)
      , Depth(depthInfo)
    {
    }

    constexpr BasicMaterialInfo(const BlendState blend, const BasicCullMode cullMode, const BasicFrontFace cullFrontFace,
                                const BasicMaterialDepthInfo& depthInfo, const BasicViewportMode viewportMode, const PxViewport& viewport) noexcept
      : Blend(blend)
      , CullMode(cullMode)
      , CullFrontFace(cullFrontFace)
      , Depth(depthInfo)
      , ViewportMode(viewportMode)
      , Viewport(viewport)
    {
    }


    constexpr bool operator==(const BasicMaterialInfo& rhs) const noexcept
    {
      return Blend == rhs.Blend && CullMode == rhs.CullMode && CullFrontFace == rhs.CullFrontFace && Depth == rhs.Depth &&
             ViewportMode == rhs.ViewportMode && Viewport == rhs.Viewport;
    }

    constexpr bool operator!=(const BasicMaterialInfo& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
