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

#include "RenderSystemRuntimeSettingsUtil.hpp"
#include <FslSimpleUI/App/UIDemoAppMaterialConfig.hpp>
#include <FslSimpleUI/Render/Base/IRenderSystemBase.hpp>
#include <FslSimpleUI/Render/Base/RenderSystemInfo.hpp>
#include <FslSimpleUI/Render/IMBatch/DrawReorderMethod.hpp>
#include <FslSimpleUI/Render/IMBatch/FlexRenderSystemConfig.hpp>
#include <FslSimpleUI/Render/IMBatch/IFlexRenderSystemConfig.hpp>
#include <Shared/UI/Benchmark/App/TestAppHost.hpp>

namespace Fsl::RenderSystemRuntimeSettingsUtil
{
  namespace
  {
    UI::RenderIMBatch::DrawReorderMethod SelectReorderMethod(const bool reorderEnabled, const bool preferFastReorder)
    {
      if (!reorderEnabled)
      {
        return UI::RenderIMBatch::DrawReorderMethod::Disabled;
      }

      return preferFastReorder ? UI::RenderIMBatch::DrawReorderMethod::LinearConstrained : UI::RenderIMBatch::DrawReorderMethod::SpatialGrid;
    }
  }

  void Apply(TestAppHost& rAppHost, const UI::RenderOptionFlags settings, const bool useSdf)
  {
    const bool batch = UI::RenderOptionFlagsUtil::IsEnabled(settings, UI::RenderOptionFlags::Batch);
    const bool fillBuffers = UI::RenderOptionFlagsUtil::IsEnabled(settings, UI::RenderOptionFlags::FillBuffers);
    const bool depthBuffer = UI::RenderOptionFlagsUtil::IsEnabled(settings, UI::RenderOptionFlags::DepthBuffer);
    const bool drawReorder = UI::RenderOptionFlagsUtil::IsEnabled(settings, UI::RenderOptionFlags::DrawReorder);
    // const bool meshCaching = UI::RenderOptionFlagsUtil::IsEnabled(settings.Settings, UI::RenderOptionFlags::MeshCaching);
    const bool preferFastReorder = UI::RenderOptionFlagsUtil::IsEnabled(settings, UI::RenderOptionFlags::PreferFastReorder);

    {    // update the material system if necessary
      UIDemoAppMaterialConfig newConfig(useSdf, depthBuffer);
      UIDemoAppMaterialConfig oldConfig = rAppHost.GetDefaultMaterialConfig();
      if (newConfig != oldConfig)
      {
        rAppHost.SetDefaultMaterialConfig(newConfig);
      }
    }
    {    // Update the render system
      auto* pRenderSystem = rAppHost.TryGetRenderSystem();
      auto* pFlexRenderSystemConfig = dynamic_cast<UI::RenderIMBatch::IFlexRenderSystemConfig*>(pRenderSystem);
      if (pFlexRenderSystemConfig != nullptr)
      {
        const auto method = SelectReorderMethod(drawReorder, preferFastReorder);

        UI::RenderIMBatch::FlexRenderSystemConfig newConfig(batch, fillBuffers, depthBuffer, method);
        auto oldConfig = pFlexRenderSystemConfig->GetConfig();
        if (oldConfig != newConfig)
        {
          pFlexRenderSystemConfig->SetConfig(newConfig);
        }
      }
    }
  }
}
