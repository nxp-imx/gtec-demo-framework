#ifndef FSLSIMPLEUI_RENDER_BASE_RENDERSYSTEMINFO_HPP
#define FSLSIMPLEUI_RENDER_BASE_RENDERSYSTEMINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslSimpleUI/Render/Base/RenderOptionFlags.hpp>

namespace Fsl::UI
{
  struct RenderSystemInfo
  {
    // The flags that can be tweaked at run-time
    RenderOptionFlags Available{UI::RenderOptionFlags::NoFlags};
    // The flags that are currently set
    RenderOptionFlags Settings{RenderOptionFlags::NoFlags};
    // The default flag values.
    RenderOptionFlags DefaultSettings{RenderOptionFlags::NoFlags};

    constexpr RenderSystemInfo() noexcept = default;
    constexpr RenderSystemInfo(const RenderOptionFlags available, const RenderOptionFlags settings, const RenderOptionFlags defaultSettings) noexcept
      : Available(available)
      , Settings(settings)
      , DefaultSettings(defaultSettings)
    {
    }

    void Set(const RenderOptionFlags flag, const bool enabled)
    {
      UI::RenderOptionFlagsUtil::Set(
        Settings, flag,
        (UI::RenderOptionFlagsUtil::IsEnabled(Available, flag) ? enabled : UI::RenderOptionFlagsUtil::IsEnabled(DefaultSettings, flag)));
    }

    void Set(const RenderOptionFlags flags)
    {
      SetFlagIfEnabled(RenderOptionFlags::Batch, flags);
      SetFlagIfEnabled(RenderOptionFlags::FillBuffers, flags);
      SetFlagIfEnabled(RenderOptionFlags::DepthBuffer, flags);
      SetFlagIfEnabled(RenderOptionFlags::DrawReorder, flags);
      SetFlagIfEnabled(RenderOptionFlags::MeshCaching, flags);
      SetFlagIfEnabled(RenderOptionFlags::PreferFastReorder, flags);
    }

  private:
    void SetFlagIfEnabled(RenderOptionFlags flag, RenderOptionFlags sourceFlags)
    {
      Set(flag, UI::RenderOptionFlagsUtil::IsEnabled(sourceFlags, flag));
    }
  };
}


#endif
