#ifndef SHARED_MODELINSTANCING_MODELINSTANCINGSHARED_HPP
#define SHARED_MODELINSTANCING_MODELINSTANCINGSHARED_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <Shared/ModelInstancing/MatrixInfo.hpp>
#include <Shared/ModelInstancing/ModelRenderStats.hpp>
#include <algorithm>
#include <memory>
#include <utility>

namespace Fsl
{
  namespace UI::Theme
  {
    class IThemeControlFactory;
  }

  struct MeshInstanceSetup
  {
    const uint32_t MaxInstancesX{1};
    const uint32_t MaxInstancesY{1};
    const uint32_t MaxInstancesZ{1};
    const uint32_t MaxInstances{1};

    MeshInstanceSetup(const uint32_t maxInstancesX, const uint32_t maxInstancesY, const uint32_t maxInstancesZ)
      : MaxInstancesX(std::max(maxInstancesX, 1u))
      , MaxInstancesY(std::max(maxInstancesY, 1u))
      , MaxInstancesZ(std::max(maxInstancesZ, 1u))
      , MaxInstances(SafeCalcMaxInstances(MaxInstancesX, MaxInstancesY, MaxInstancesZ))
    {
    }

  private:
    static uint32_t SafeCalcMaxInstances(const uint32_t maxInstancesX, const uint32_t maxInstancesY, const uint32_t maxInstancesZ)
    {
      const uint64_t res = static_cast<uint64_t>(maxInstancesX) * static_cast<uint64_t>(maxInstancesY) * static_cast<uint64_t>(maxInstancesZ);
      return NumericCast<uint32_t>(res);
    }
  };


  struct MeshInstanceData
  {
    Matrix MatWorld;

    constexpr MeshInstanceData() noexcept = default;
    explicit constexpr MeshInstanceData(const Matrix& matWorld) noexcept
      : MatWorld(matWorld)
    {
    }
  };

  class ModelInstancingShared final : public UI::EventListener
  {
    struct StatsOverlayUI
    {
      std::shared_ptr<UI::BaseWindow> MainOverlay;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelVertices;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelIndices;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelInstanceCount;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelTotalVertices;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelTotalIndices;

      StatsOverlayUI() = default;

      StatsOverlayUI(std::shared_ptr<UI::BaseWindow> mainOverlay, std::shared_ptr<UI::FmtValueLabel<uint32_t>> labelVertices,
                     std::shared_ptr<UI::FmtValueLabel<uint32_t>> labelIndices, std::shared_ptr<UI::FmtValueLabel<uint32_t>> labelInstanceCount,
                     std::shared_ptr<UI::FmtValueLabel<uint32_t>> labelTotalVertices, std::shared_ptr<UI::FmtValueLabel<uint32_t>> labelTotalIndices)
        : MainOverlay(std::move(mainOverlay))
        , LabelVertices(std::move(labelVertices))
        , LabelIndices(std::move(labelIndices))
        , LabelInstanceCount(std::move(labelInstanceCount))
        , LabelTotalVertices(std::move(labelTotalVertices))
        , LabelTotalIndices(std::move(labelTotalIndices))
      {
      }
    };

    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> MainWindow;
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> Instances;
      std::shared_ptr<UI::Switch> SwitchRotate;
      std::shared_ptr<UI::BackgroundLabelButton> ButtonDefault;

      StatsOverlayUI Stats;
    };


    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    MeshInstanceSetup m_instanceSetup;

    UIRecord m_ui;

    Vector3 m_rotationSpeed;
    Vector3 m_rotation;
    MatrixInfo m_matrices;
    std::vector<MeshInstanceData> m_instanceData;

  public:
    explicit ModelInstancingShared(const DemoAppConfig& config);
    ~ModelInstancingShared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    MeshInstanceSetup GetInstanceSetup() const noexcept
    {
      return m_instanceSetup;
    }

    void OnKeyEvent(const KeyEvent& event);

    // From EventListener
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& demoTime);
    void Draw();

    uint32_t GetInstanceCount() const
    {
      return m_ui.Instances->GetValue();
    }

    MatrixInfo GetMatrixInfo() const noexcept
    {
      return m_matrices;
    }
    void SetStats(const ModelRenderStats& stats);

    ReadOnlySpan<MeshInstanceData> GetInstanceSpan();

  private:
    void SetDefaultValues();
    void ToggleRotate();

    static UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const MeshInstanceSetup instanceSetup);
    static StatsOverlayUI CreateStatsOverlayUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context);
  };
}

#endif
