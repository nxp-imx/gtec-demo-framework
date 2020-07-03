/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <Shared/Stats/Shared.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoService/CpuStats/ICpuStatsService.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
//#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>

#include <cassert>

namespace Fsl
{
  using namespace UI;

  namespace
  {
    // std::shared_ptr<Label> CreateLabel(const std::shared_ptr<WindowContext>& context, const std::string& text, const ItemAlignment alignX,
    //                                   const ItemAlignment alignY, const std::shared_ptr<AtlasFont>& font)
    //{
    //  auto label = std::make_shared<Label>(context);
    //  label->SetContent(text);
    //  label->SetAlignmentX(alignX);
    //  label->SetAlignmentY(alignY);
    //  label->SetFont(font);
    //  return label;
    //}
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_cpuStats(config.DemoServiceProvider.TryGet<ICpuStatsService>())
  {
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    auto windowContext = m_uiExtension->GetContext();

    auto cpuCount = m_cpuStats ? m_cpuStats->GetCpuCount() : 0u;

    // m_cpuStats->TryGetApplicationCpuUsage
    // m_cpuStats->TryGetApplicationRamUsage
    // m_cpuStats->TryGetCpuUsage()

    m_cores.resize(cpuCount);

    auto gridLayoutInfo = std::make_shared<GridLayout>(windowContext);
    {
      gridLayoutInfo->SetAlignmentX(ItemAlignment::Near);
      gridLayoutInfo->SetAlignmentY(ItemAlignment::Near);
      gridLayoutInfo->AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
      gridLayoutInfo->AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));

      gridLayoutInfo->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
      gridLayoutInfo->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));

      uint32_t yPos = 0;
      auto label = std::make_shared<Label>(windowContext);
      label->SetContent("App CPU:");
      m_fmtValueLabelAppCPU = std::make_shared<UI::FmtValueLabel<float>>(windowContext);
      m_fmtValueLabelAppCPU->SetFormatString("{:6.1f}%");
      m_fmtValueLabelAppCPU->SetAlignmentX(ItemAlignment::Far);

      gridLayoutInfo->AddChild(label);
      gridLayoutInfo->AddChild(m_fmtValueLabelAppCPU);
      gridLayoutInfo->Set(label, 0, yPos);
      gridLayoutInfo->Set(m_fmtValueLabelAppCPU, 1, yPos);

      ++yPos;
      label = std::make_shared<Label>(windowContext);
      label->SetContent("App Memory:");
      m_fmtValueLabelAppMemory = std::make_shared<UI::FmtValueLabel<uint64_t>>(windowContext);
      m_fmtValueLabelAppMemory->SetFormatString("{:10} KB");
      m_fmtValueLabelAppMemory->SetAlignmentX(ItemAlignment::Far);

      gridLayoutInfo->AddChild(label);
      gridLayoutInfo->AddChild(m_fmtValueLabelAppMemory);
      gridLayoutInfo->Set(label, 0, yPos);
      gridLayoutInfo->Set(m_fmtValueLabelAppMemory, 1, yPos);
    }


    auto gridLayoutCores = std::make_shared<GridLayout>(windowContext);
    {
      gridLayoutCores->SetAlignmentX(ItemAlignment::Far);
      gridLayoutCores->SetAlignmentY(ItemAlignment::Near);
      gridLayoutCores->AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
      gridLayoutCores->AddColumnDefinition(GridColumnDefinition(GridUnitType::Fixed, 90));

      for (uint32_t i = 0; i < cpuCount; ++i)
      {
        gridLayoutCores->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));

        auto label = std::make_shared<Label>(windowContext);
        label->SetContent(fmt::format("CPU{}:", i));
        auto fmtValueLabel = std::make_shared<UI::FmtValueLabel<float>>(windowContext);
        fmtValueLabel->SetFormatString("{:.1f}");
        fmtValueLabel->SetAlignmentX(ItemAlignment::Far);
        m_cores[i] = fmtValueLabel;

        gridLayoutCores->AddChild(label);
        gridLayoutCores->AddChild(fmtValueLabel);
        gridLayoutCores->Set(label, 0, i);
        gridLayoutCores->Set(fmtValueLabel, 1, i);
      }
    }

    // Create the root layout and add it to the window manager
    m_rootLayout = std::make_shared<FillLayout>(windowContext);
    m_rootLayout->AddChild(gridLayoutInfo);
    m_rootLayout->AddChild(gridLayoutCores);

    // Register the root layout with the window manager
    m_uiExtension->GetWindowManager()->Add(m_rootLayout);
  }


  Shared::~Shared() = default;


  void Shared::OnSelect(const RoutedEventArgs& /*args*/, const std::shared_ptr<WindowSelectEvent>& /*theEvent*/)
  {
    // if (theEvent->GetSource() == m_button1)
    //{
    //  FSLLOG3_INFO("Button1 selected");
    //}
    // else if (theEvent->GetSource() == m_button2)
    //{
    //  FSLLOG3_INFO("Button2 selected");
    //}
    // else if (theEvent->GetSource() == m_complexButton)
    //{
    //  FSLLOG3_INFO("Complex button selected");
    //}
  }

  void Shared::Update()
  {
    if (m_cpuStats)
    {
      for (uint32_t i = 0; i < m_cores.size(); ++i)
      {
        float usagePercentage = 0.0f;
        if (m_cpuStats->TryGetCpuUsage(usagePercentage, i))
        {
          m_cores[i]->SetContent(usagePercentage);
        }
      }

      float appCpuUsage = 0.0f;
      if (m_cpuStats->TryGetApplicationCpuUsage(appCpuUsage))
      {
        m_fmtValueLabelAppCPU->SetContent(appCpuUsage);
      }

      uint64_t appRamUsage = 0u;
      if (m_cpuStats->TryGetApplicationRamUsage(appRamUsage))
      {
        m_fmtValueLabelAppMemory->SetContent(appRamUsage / 1024);
      }
    }
  }

  void Shared::Draw()
  {
    m_uiExtension->Draw();
  }
}
