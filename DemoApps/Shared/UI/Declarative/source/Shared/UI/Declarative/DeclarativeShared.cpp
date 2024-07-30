/****************************************************************************************************************************************************
 * Copyright 2022-2024 NXP
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

#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Declarative/ControlFactory.hpp>
#include <FslSimpleUI/Declarative/UIReader.hpp>
#include <FslSimpleUI/Declarative/UIXsdWriter.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Declarative/DeclarativeShared.hpp>
#include <Shared/UI/Declarative/OptionParser.hpp>

namespace Fsl
{
  namespace LocalConfig
  {
    constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");
    constexpr IO::PathView DeclarativeUI("DeclarativeUI.xml");
  }

  DeclarativeShared::DeclarativeShared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
  {
    FSLLOG3_INFO("Preparing UI");

    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();


    {    // Build a simple UI
      auto uiFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);

      {    // Check if the user requested the XSD to be saved
        auto optionParser = config.GetOptions<OptionParser>();
        auto saveXsdFilename = optionParser->TryGetXsdSaveFilename();
        if (!saveXsdFilename.IsEmpty())
        {
          FSLLOG3_INFO("Saving UI XSD to '{}'", saveXsdFilename);
          UI::Declarative::ControlFactory factory(uiFactory);
          UI::Declarative::UIXsdWriter::Save(saveXsdFilename, factory);
        }
      }


      m_uiRecord = CreateUI(*contentManager, uiFactory);

      // Register the root layout with the window manager
      m_uiExtension->GetWindowManager()->Add(m_uiRecord.Main);
    }
    OnConfigurationChanged(config.WindowMetrics);
  }


  DeclarativeShared::~DeclarativeShared() = default;


  void DeclarativeShared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& /*theEvent*/)
  {
  }

  void DeclarativeShared::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& /*theEvent*/)
  {
  }


  void DeclarativeShared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
  }


  void DeclarativeShared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSL_PARAM_NOT_USED(windowMetrics);
  }

  void DeclarativeShared::Update(const DemoTime& demoTime)
  {
  }

  void DeclarativeShared::Draw()
  {
    m_uiExtension->Draw();
  }


  DeclarativeShared::UIRecord DeclarativeShared::CreateUI(const IContentManager& contentManager,
                                                          const std::shared_ptr<UI::Theme::IThemeControlFactory>& uiFactory)
  {
    UI::Declarative::ControlFactory factory(uiFactory);

    // FileStream s = new FileStream(fileName, FileMode.Open);
    // std::shared_ptr<UI::BaseWindow> main = XamlReader.Load(s);


    FSLLOG3_INFO("Loading UI from '{}'", LocalConfig::DeclarativeUI);
    auto fullPath = IO::Path::Combine(contentManager.GetContentPath(), LocalConfig::DeclarativeUI);
    std::shared_ptr<UI::BaseWindow> main = UI::Declarative::UIReader::Load(factory, uiFactory->GetContext()->UIDataBindingService, fullPath);
    return {main};
  }

}
