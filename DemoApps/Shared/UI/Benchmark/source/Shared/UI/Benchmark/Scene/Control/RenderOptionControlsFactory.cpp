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

#include "RenderOptionControlsFactory.hpp"
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/UI/Benchmark/TextConfig.hpp>

namespace Fsl
{
  namespace RenderOptionControlsFactory
  {
    RenderOptionControls CreateRenderMethodControls(UI::Theme::IThemeControlFactory& uiFactory)
    {
      auto switchRenderOptionBatch = uiFactory.CreateSwitch(TextConfig::Batch, true);
      auto switchRenderOptionFillBuffers = uiFactory.CreateSwitch(TextConfig::FillBuffers, true);
      auto switchRenderOptionDepthBuffer = uiFactory.CreateSwitch(TextConfig::DepthBuffer, true);
      auto switchRenderOptionDrawReorder = uiFactory.CreateSwitch(TextConfig::DrawReorder, true);
      auto switchRenderOptionMeshCaching = uiFactory.CreateSwitch(TextConfig::MeshCaching, false);

      return {switchRenderOptionBatch, switchRenderOptionFillBuffers, switchRenderOptionDepthBuffer, switchRenderOptionDrawReorder,
              switchRenderOptionMeshCaching};
    }
  }
}
