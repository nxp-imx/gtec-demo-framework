#ifndef SHARED_OBJECTSELECTION_MENUUI_HPP
#define SHARED_OBJECTSELECTION_MENUUI_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>

namespace Fsl
{
  namespace UI::Theme
  {
    class IThemeControlFactory;
  }

  class OptionParser;

  class MenuUI : public UI::EventListener
  {
    UI::CallbackEventListenerScope m_uiEventListener;
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<UI::Theme::IThemeControlFactory> m_themeFactory;

    std::shared_ptr<OptionParser> m_optionParser;

    std::shared_ptr<UI::FillLayout> m_rootLayout;

    std::shared_ptr<UI::Background> m_layoutMenu;
    std::shared_ptr<UI::Switch> m_cbMenuDrawNearPlaneMouse;
    std::shared_ptr<UI::Switch> m_cbMenuDrawFarPlaneMouse;
    std::shared_ptr<UI::Switch> m_cbMenuOrientedBoundingBox;
    std::shared_ptr<UI::Switch> m_cbMenuAxisAlignedBoundingBox;
    // std::shared_ptr<UI::Switch> m_cbMenuFinalBloom;

    bool m_drawNearPlaneMouse;
    bool m_drawFarPlaneMouse;
    bool m_drawOrientedBoundingBox;
    bool m_drawAxisAlignedBoundingBox;
    bool m_randomSeedEnabled;
    // bool m_renderFinalBloom;

  public:
    explicit MenuUI(const DemoAppConfig& config);
    ~MenuUI() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    void OnKeyEvent(const KeyEvent& event);

    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;

    bool IsDrawNearPlaneMouseEnabled() const
    {
      return m_drawNearPlaneMouse;
    }

    void SetDrawNearPlaneMouseEnabled(bool enabled);

    bool IsDrawFarPlaneMouseEnabled() const
    {
      return m_drawFarPlaneMouse;
    }

    void SetDrawFarPlaneMouseEnabled(bool enabled);

    bool IsDrawOrientedBoundingBoxEnabled() const
    {
      return m_drawOrientedBoundingBox;
    }

    void SetDrawOrientedBoundingBoxEnabled(bool enabled);

    bool IsDrawAxisAlignedBoundingBoxEnabled() const
    {
      return m_drawAxisAlignedBoundingBox;
    }

    void SetDrawAxisAlignedBoundingBoxEnabled(bool enabled);

    // bool IsFinalBloomEnabled() const { return m_renderFinalBloom; }
    // void SetFinalBloomEnabled(bool enabled);

    bool IsRandomSeedEnabled() const
    {
      return m_randomSeedEnabled;
    }

    bool IsPlaneEnabled() const
    {
      return true;
    }

    bool IsObjectsEnabled() const
    {
      return true;
    }

    bool IsForceBindEnabled() const
    {
      return false;
    }

    void Draw();

  private:
    void BuildUI();
    void UpdateControls();
  };
}

#endif
