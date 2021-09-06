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

#include <Shared/UI/Benchmark/Scene/Control/RenderOptionControls.hpp>
#include <FslSimpleUI/Render/Base/RenderSystemInfo.hpp>

namespace Fsl
{
  namespace
  {
    // bool IsEnabled(const UI::RenderSystemInfo& systemInfo, const UI::RenderOptionFlags flag)
    // {
    //   return UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Settings, flag) && UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Available, flag);
    // }
  }

  void RenderOptionControls::RestoreUISettings(const UI::RenderSystemInfo& systemInfo)
  {
    {
      const bool batch = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Available, UI::RenderOptionFlags::Batch);
      const bool fillBuffers = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Available, UI::RenderOptionFlags::FillBuffers);
      const bool depthBuffer = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Available, UI::RenderOptionFlags::DepthBuffer);
      const bool drawReorder = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Available, UI::RenderOptionFlags::DrawReorder);
      const bool meshCaching = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Available, UI::RenderOptionFlags::MeshCaching);

      SwitchBatch->SetEnabled(batch);
      SwitchFillBuffers->SetEnabled(fillBuffers);
      SwitchDepthBuffer->SetEnabled(depthBuffer);
      SwitchDrawReorder->SetEnabled(drawReorder);
      SwitchMeshCaching->SetEnabled(meshCaching);
    }

    {
      const bool batch = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Settings, UI::RenderOptionFlags::Batch);
      const bool fillBuffers = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Settings, UI::RenderOptionFlags::FillBuffers);
      const bool depthBuffer = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Settings, UI::RenderOptionFlags::DepthBuffer);
      const bool drawReorder = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Settings, UI::RenderOptionFlags::DrawReorder);
      const bool meshCaching = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Settings, UI::RenderOptionFlags::MeshCaching);

      SwitchBatch->SetIsChecked(batch);
      SwitchFillBuffers->SetIsChecked(fillBuffers);
      SwitchDepthBuffer->SetIsChecked(depthBuffer);
      SwitchDrawReorder->SetIsChecked(drawReorder);
      SwitchMeshCaching->SetIsChecked(meshCaching);
    }
  }


  void RenderOptionControls::StoreUISettings(UI::RenderSystemInfo& rSystemInfo)
  {
    const bool batch = SwitchBatch->IsChecked();
    const bool fillBuffers = SwitchFillBuffers->IsChecked();
    const bool depthBuffer = SwitchDepthBuffer->IsChecked();
    const bool drawReorder = SwitchDrawReorder->IsChecked();
    const bool meshCaching = SwitchMeshCaching->IsChecked();

    rSystemInfo.Set(UI::RenderOptionFlags::Batch, batch);
    rSystemInfo.Set(UI::RenderOptionFlags::FillBuffers, fillBuffers);
    rSystemInfo.Set(UI::RenderOptionFlags::DepthBuffer, depthBuffer);
    rSystemInfo.Set(UI::RenderOptionFlags::DrawReorder, drawReorder);
    rSystemInfo.Set(UI::RenderOptionFlags::MeshCaching, meshCaching);
  }


  void RenderOptionControls::SetEnabled(const bool isEnabled)
  {
    if (SwitchBatch)
    {
      SwitchBatch->SetEnabled(isEnabled);
    }
    if (SwitchFillBuffers)
    {
      SwitchFillBuffers->SetEnabled(isEnabled);
    }
    if (SwitchDepthBuffer)
    {
      SwitchDepthBuffer->SetEnabled(isEnabled);
    }
    if (SwitchDrawReorder)
    {
      SwitchDrawReorder->SetEnabled(isEnabled);
    }
    if (SwitchMeshCaching)
    {
      SwitchMeshCaching->SetEnabled(isEnabled);
    }
  }

  void RenderOptionControls::FinishAnimation()
  {
    if (SwitchBatch)
    {
      SwitchBatch->FinishAnimation();
    }
    if (SwitchFillBuffers)
    {
      SwitchFillBuffers->FinishAnimation();
    }
    if (SwitchDepthBuffer)
    {
      SwitchDepthBuffer->FinishAnimation();
    }
    if (SwitchDrawReorder)
    {
      SwitchDrawReorder->FinishAnimation();
    }
    if (SwitchMeshCaching)
    {
      SwitchMeshCaching->FinishAnimation();
    }
  }

}
