#ifndef SHARED_SPATIALHASHGRID2D_SHARED_HPP
#define SHARED_SPATIALHASHGRID2D_SHARED_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Rectangle2D.hpp>
#include <FslGraphics3D/Build/LineBuilder.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <Shared/SpatialHashGrid2D/BasicSpatialHashGrid2D.hpp>
#include <vector>

namespace Fsl
{
  namespace UI
  {
    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  struct DrawRecord
  {
    Rectangle2D Rect;
    Color BaseColor;
  };

  class Shared final : public UI::EventListener
  {
    struct ResolutionDependentRecord
    {
      uint32_t GridWidthPx{0};
      uint32_t GridHeightPx{0};
      uint32_t GridStepsX{0};
      uint32_t GridStepsY{0};
      std::vector<DrawRecord> Records;
      BasicSpatialHashGrid2D<32> SpatialHashGrid;

      explicit ResolutionDependentRecord(const std::size_t recordCount)
        : Records(recordCount)
      {
      }
      PxPoint2 Offset;
      Rectangle2D CandidateArea;
    };

    struct DragRecord
    {
      bool IsDragging{false};
      PxPoint2 StartDragPx;
      PxPoint2 DragOffsetPx;
    };

    struct UIRecord
    {
      std::shared_ptr<UI::Layout> MainLayout;
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    UIRecord m_ui;

    Fsl::Graphics3D::LineBuilder m_lineBuilder;

    DemoWindowMetrics m_windowMetrics;
    Matrix m_projectionMatrix;

    ResolutionDependentRecord m_resData;

    DragRecord m_drag;

  public:
    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    const Fsl::Graphics3D::LineBuilder& GetLineBuilder() const
    {
      return m_lineBuilder;
    }

    const Matrix& GetProjectionMatrix() const
    {
      return m_projectionMatrix;
    }


    explicit Shared(const DemoAppConfig& config);
    ~Shared() override;
    // From EventListener
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;
    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;
    void OnClickInput(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowInputClickEvent>& theEvent) final;


    void OnKeyEvent(const KeyEvent& event);
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);

  private:
    void SetDefaultValues();
    UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory);
  };
}

#endif
