#ifndef SHARED_ANTIALIASING_ANTIALIASINGSHARED_HPP
#define SHARED_ANTIALIASING_ANTIALIASINGSHARED_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Transition/TransitionPxRectangle.hpp>
#include <FslGraphics3D/Build/LineBuilder.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/AntiAliasing/AntiAliasingConfig.hpp>
#include <Shared/AntiAliasing/AntiAliasingMethod.hpp>
#include <Shared/AntiAliasing/ContentWindowId.hpp>
#include <Shared/AntiAliasing/CustomControls/ZoomArea.hpp>
#include <Shared/AntiAliasing/CustomControls/ZoomInfo.hpp>
#include <Shared/AntiAliasing/CustomUIConfig.hpp>
#include <Shared/AntiAliasing/OptionParser.hpp>
#include <array>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  class AntiAliasingShared final : public UI::EventListener
  {
    struct UIAntiAliasingRecord
    {
      AntiAliasingMethod Method{AntiAliasingMethod::Disabled};
      std::shared_ptr<UI::RadioButton> Button;

      UIAntiAliasingRecord() = default;
      UIAntiAliasingRecord(const AntiAliasingMethod method, std::shared_ptr<UI::RadioButton> button)
        : Method(method)
        , Button(std::move(button))
      {
      }
    };

    struct UIContentSelectionRecord
    {
      ContentWindowId ContentId{ContentWindowId::Car};
      std::shared_ptr<UI::RadioButton> Button;

      UIContentSelectionRecord() = default;
      UIContentSelectionRecord(const ContentWindowId contentWindowId, std::shared_ptr<UI::RadioButton> button)
        : ContentId(contentWindowId)
        , Button(std::move(button))
      {
      }
    };

    struct UIContentRecord
    {
      std::shared_ptr<UI::BaseWindow> MainLayout;
      std::shared_ptr<UI::BaseWindow> Content1;
      std::shared_ptr<UI::BaseWindow> Content2;
      std::shared_ptr<UI::BaseWindow> Content3;
      std::shared_ptr<UI::Custom::ZoomArea> ZoomArea;
      std::shared_ptr<UI::BaseWindow> DividerH;
      std::shared_ptr<UI::BaseWindow> DividerV;

      UIContentRecord() = default;

      UIContentRecord(std::shared_ptr<UI::BaseWindow> mainLayout, std::shared_ptr<UI::BaseWindow> content1, std::shared_ptr<UI::BaseWindow> content2,
                      std::shared_ptr<UI::BaseWindow> content3, std::shared_ptr<UI::Custom::ZoomArea> zoomArea,
                      std::shared_ptr<UI::BaseWindow> dividerH, std::shared_ptr<UI::BaseWindow> dividerV)
        : MainLayout(std::move(mainLayout))
        , Content1(std::move(content1))
        , Content2(std::move(content2))
        , Content3(std::move(content3))
        , ZoomArea(std::move(zoomArea))
        , DividerH(std::move(dividerH))
        , DividerV(std::move(dividerV))
      {
      }
    };

    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> MainLayout;
      std::vector<UIAntiAliasingRecord> AntiAliasingSolutions;
      std::vector<UIContentSelectionRecord> Scene;
      std::shared_ptr<UI::Switch> FastResolve;
      std::shared_ptr<UI::CheckBox> CbPause;
      UIContentRecord Content;

      UIRecord() = default;

      UIRecord(std::shared_ptr<UI::BaseWindow> mainLayout, std::vector<UIAntiAliasingRecord> antiAliasingSolutions,
               std::vector<UIContentSelectionRecord> scene, std::shared_ptr<UI::Switch> fastResolve, std::shared_ptr<UI::CheckBox> cbPause,
               UIContentRecord content)
        : MainLayout(std::move(mainLayout))
        , AntiAliasingSolutions(std::move(antiAliasingSolutions))
        , Scene(std::move(scene))
        , FastResolve(std::move(fastResolve))
        , CbPause(std::move(cbPause))
        , Content(std::move(content))
      {
      }
    };

    struct LinesAnimRecord
    {
      float Radians1{0.0f};
      float Radians2{0.0f};
      float Radians3{0.0f};
      float RotationSpeed1{0.3f};
      float RotationSpeed2{0.25f};
      float RotationSpeed3{0.2f};
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<OptionParser> m_defaultOptions;

    DemoWindowMetrics m_cachedWindowMetrics;

    UIRecord m_ui;

    Fsl::Graphics3D::LineBuilder m_lineBuilder;
    Fsl::Graphics3D::LineBuilder m_dividerLineBuilder;

    LinesAnimRecord m_lineAnim;
    Matrix m_matView;
    Matrix m_matProjection;

    bool m_forceCompleteAnimation{false};
    std::array<TransitionPxRectangle, 3> m_windowRectangle;
    std::size_t m_focusedWindow{0};

    AntiAliasingMethod m_activeAntiAliasingMethod{AntiAliasingMethod::Disabled};

  public:
    explicit AntiAliasingShared(const DemoAppConfig& config, const AntiAliasingConfig& aaConfig, const CustomUIConfig& customUIConfig);
    ~AntiAliasingShared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    const Fsl::Graphics3D::LineBuilder& GetLineBuilder() const
    {
      return m_lineBuilder;
    }

    const Fsl::Graphics3D::LineBuilder& GetDividerLineBuilder() const
    {
      return m_dividerLineBuilder;
    }

    bool IsPaused() const
    {
      return m_ui.CbPause->IsChecked();
    }

    const Matrix& GetViewMatrix() const
    {
      return m_matView;
    }

    const Matrix& GetProjectionMatrix() const
    {
      return m_matProjection;
    }

    AntiAliasingMethod GetActiveAntiAliasingMethod() const noexcept
    {
      return m_activeAntiAliasingMethod;
    }

    bool UseMSAAFastResolve() const
    {
      return m_ui.FastResolve->IsChecked();
    }

    PxRectangle GetContentWindowRectangleInScreenspace(const ContentWindowId contentWindowId) const;
    UI::Custom::ZoomInfo GetResolvedZoomInfo() const;


    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;

    void OnKeyEvent(const KeyEvent& event);

    void Update(const DemoTime& demoTime);
    void Resolve(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);

  private:
    void SetToDefaults();
    UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const SupportedSampleCountFlags supportedMSAASampleFlags,
                      const CustomUIConfig& customUIConfig);
    UIContentRecord CreateContentUI(UI::Theme::IThemeControlFactory& uiFactory);

    void SafeSetIsChecked(const std::vector<UIContentSelectionRecord>& entries, const std::size_t index, const bool value);
    void SafeSetIsChecked(const std::vector<UIAntiAliasingRecord>& entries, const std::size_t index, const bool value);
    bool TrySetAAMethod(const AntiAliasingMethod aaMethod);
    bool TrySetContent(const ContentWindowId contentWindowId);
  };
}

#endif
