/****************************************************************************************************************************************************
 * Copyright 2021-2024 NXP
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

#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/SpatialHashGrid2D/Shared.hpp>
#include <random>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t ObjectCount = 200;
      constexpr uint32_t Seed = 1337;

      // constexpr uint32_t GridChunkSize = 32;

      constexpr float VirtualSize = 2020.0f;

    }

    namespace RenderColor
    {
      constexpr Color NotSelected(0xFF404040);
      constexpr Color Candidate(Colors::Blue());
      constexpr Color CandidateArea(Colors::Red());
      constexpr Color SelectionArea(Colors::Green());
    }

    namespace UILegendColor
    {
      constexpr UI::UIColor NotSelected(RenderColor::NotSelected);
      constexpr UI::UIColor Candidate(RenderColor::Candidate);
      constexpr UI::UIColor CandidateArea(RenderColor::CandidateArea);
      constexpr UI::UIColor SelectionArea(RenderColor::SelectionArea);
    }

    void FillVector(std::vector<DrawRecord>& rRecords, const uint32_t seed, const PxSize2D sizePx)
    {
      std::mt19937 random(seed);

      std::uniform_real_distribution<float> randomPositionX(-10.0f, 2000.0f);
      std::uniform_real_distribution<float> randomPositionY(-10.0f, 2000.0f);
      std::uniform_real_distribution<float> randomWidth1(20.0f, 200.0f);
      std::uniform_real_distribution<float> randomWidth2(20.0f, 400.0f);
      std::uniform_real_distribution<float> randomHeight(10.0f, 100.0f);

      const float scaleX = static_cast<float>(sizePx.RawWidth()) / LocalConfig::VirtualSize;
      const float scaleY = static_cast<float>(sizePx.RawHeight()) / LocalConfig::VirtualSize;

      uint32_t index = 0;
      for (auto& rRecord : rRecords)
      {
        const float posX = randomPositionX(random) * scaleX;
        const float posY = randomPositionY(random) * scaleY;
        const float width = ((index % 3) != 0 ? randomWidth1(random) : randomWidth2(random)) * scaleX;
        const float height = randomHeight(random) * scaleY;
        rRecord.Rect = Rectangle2D(TypeConverter::ChangeTo<int32_t>(posX), TypeConverter::ChangeTo<int32_t>(posY),
                                   TypeConverter::ChangeTo<uint32_t>(width), TypeConverter::ChangeTo<uint32_t>(height));
        rRecord.BaseColor = RenderColor::NotSelected;
        ++index;
      }
    }

    std::shared_ptr<UI::Layout> CreateLegend(UI::Theme::IThemeControlFactory& uiFactory, const UI::UIColor color, const StringViewLite strView)
    {
      auto colorMarkerImage = uiFactory.CreateImage(uiFactory.GetResources().GetColorMarkerNineSliceSprite());
      colorMarkerImage->SetAlignmentY(UI::ItemAlignment::Center);
      colorMarkerImage->SetContentColor(color);
      auto label = uiFactory.CreateLabel(strView);
      auto stack = std::make_shared<UI::StackLayout>(uiFactory.GetContext());
      stack->SetOrientation(UI::LayoutOrientation::Horizontal);
      stack->SetSpacing(DpSize1DF::Create(4));
      stack->AddChild(colorMarkerImage);
      stack->AddChild(label);
      return stack;
    }
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi",
                                                         UITestPatternMode::DisabledAllowSwitching))
    , m_windowMetrics(config.WindowMetrics)
    , m_resData(LocalConfig::ObjectCount)
  {
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);

    m_ui = CreateUI(*uiControlFactory);
    m_uiExtension->GetWindowManager()->Add(m_ui.MainLayout);


    ConfigurationChanged(config.WindowMetrics);
    FSLLOG3_INFO("Ready");
  }


  Shared::~Shared() = default;


  void Shared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& /*theEvent*/)
  {
    // if (theEvent->GetSource() == m_uiRecord.BtnSetDefaultValues)
    //{
    //  SetDefaultValues();
    //}
  }

  void Shared::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }
  }


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::Space:
      {
        SetDefaultValues();
        break;
      }
    default:
      break;
    }
  }


  void Shared::OnClickInput(const std::shared_ptr<UI::WindowInputClickEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }

    if (theEvent->IsBegin())
    {
      if (!m_drag.IsDragging)
      {
        m_drag.IsDragging = true;
        m_drag.StartDragPx = theEvent->GetScreenPosition();
        m_drag.DragOffsetPx = PxPoint2();
      }
      else
      {
        m_drag.DragOffsetPx = theEvent->GetScreenPosition() - m_drag.StartDragPx;
      }
    }
    else
    {
      m_resData.Offset += m_drag.DragOffsetPx;
      m_drag = {};
    }
  }


  void Shared::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    // Cancel existing drag operations
    m_drag = {};

    m_windowMetrics = windowMetrics;
    FillVector(m_resData.Records, LocalConfig::Seed, m_windowMetrics.GetSizePx());

    const auto sizePx = windowMetrics.GetSizePx();

    const uint32_t desiredStepSizeX = BitsUtil::NextPowerOfTwo(sizePx.RawWidth() / 16);
    const uint32_t desiredStepSizeY = BitsUtil::NextPowerOfTwo(sizePx.RawHeight() / 16);
    const uint32_t shiftX = BitsUtil::IndexOf(desiredStepSizeX);
    const uint32_t shiftY = BitsUtil::IndexOf(desiredStepSizeY);
    const uint32_t stepsX = 1 << shiftX;
    const uint32_t stepsY = 1 << shiftY;
    m_resData.GridStepsX = ((sizePx.RawWidth() / stepsX) + ((sizePx.RawWidth() % stepsX) != 0 ? 1 : 0));
    m_resData.GridStepsY = ((sizePx.RawHeight() / stepsY) + ((sizePx.RawHeight() % stepsY) != 0 ? 1 : 0));
    m_resData.GridWidthPx = m_resData.GridStepsX * stepsX;
    m_resData.GridHeightPx = m_resData.GridStepsY * stepsY;

    m_resData.SpatialHashGrid = BasicSpatialHashGrid2D<32>(UncheckedNumericCast<uint16_t>(m_resData.GridStepsX),
                                                           UncheckedNumericCast<uint16_t>(m_resData.GridStepsY), shiftX, shiftY);

    {
      const auto count = UncheckedNumericCast<uint32_t>(m_resData.Records.size());
      for (uint32_t i = 0; i < count; ++i)
      {
        if (!m_resData.SpatialHashGrid.TryAdd(m_resData.Records[i].Rect, i))
        {
          FSLLOG3_WARNING("Failed to add entry to grid cell");
        }
      }
    }

    {
      const float scaleX = static_cast<float>(sizePx.RawWidth()) / LocalConfig::VirtualSize;
      const float scaleY = static_cast<float>(sizePx.RawHeight()) / LocalConfig::VirtualSize;
      const float width = 400 * scaleX;
      const float height = 200 * scaleY;
      const uint32_t widthPx = TypeConverter::ChangeTo<uint32_t>(width);
      const uint32_t heightPx = TypeConverter::ChangeTo<uint32_t>(height);
      m_resData.Offset = PxPoint2::Create(TypeConverter::ChangeTo<int32_t>((LocalConfig::VirtualSize / 2) * scaleX),
                                          TypeConverter::ChangeTo<int32_t>((LocalConfig::VirtualSize / 2) * scaleY));
      m_resData.CandidateArea = Rectangle2D(0, 0, widthPx, heightPx);
    }
  }


  void Shared::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    const auto sizePxf = TypeConverter::To<PxSize2DF>(m_windowMetrics.GetSizePx());

    const float screenOffsetX = sizePxf.RawWidth() / 2.0f;
    const float screenOffsetY = sizePxf.RawHeight() / 2.0f;

    m_projectionMatrix = Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, 2.0f) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                         Matrix::CreateOrthographic(sizePxf.RawWidth(), sizePxf.RawHeight(), 1.0f, 100.0f);
  }


  void Shared::Draw(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    constexpr float ZPos = 0;
    m_lineBuilder.Clear();
    m_lineBuilder.AddGridXY(
      Rectangle(0, 0, UncheckedNumericCast<int32_t>(m_resData.GridWidthPx), UncheckedNumericCast<int32_t>(m_resData.GridHeightPx)), ZPos,
      m_resData.GridStepsX, m_resData.GridStepsY, Color(0xFF909090));

    // Dummy implementation
    Rectangle2D candidateRect = m_resData.CandidateArea;
    candidateRect.Offset += TypeConverter::UncheckedTo<Offset2D>(m_resData.Offset + m_drag.DragOffsetPx);
    {
      // Set all entries to the default not selected color
      for (auto& rRecord : m_resData.Records)
      {
        rRecord.BaseColor = RenderColor::NotSelected;
      }

      // run though the candidate cells in the grid
      {
        const auto cellSizeX = UncheckedNumericCast<int32_t>(m_resData.GridWidthPx / m_resData.GridStepsX);
        const auto cellSizeY = UncheckedNumericCast<int32_t>(m_resData.GridHeightPx / m_resData.GridStepsY);

        const int32_t cellCountX = m_resData.SpatialHashGrid.GetCellCountX();
        const int32_t cellCountY = m_resData.SpatialHashGrid.GetCellCountY();
        int32_t cellStartX = m_resData.SpatialHashGrid.ToXCell(candidateRect.Left());
        int32_t cellEndX = m_resData.SpatialHashGrid.ToXCell(candidateRect.Right());
        int32_t cellStartY = m_resData.SpatialHashGrid.ToYCell(candidateRect.Top());
        int32_t cellEndY = m_resData.SpatialHashGrid.ToYCell(candidateRect.Bottom());

        if (cellStartX < cellCountX && cellEndX >= 0 && cellStartY < cellCountY && cellEndY >= 0)
        {
          cellStartX = std::max(cellStartX, 0);
          cellStartY = std::max(cellStartY, 0);
          cellEndX = std::min(cellEndX, cellCountX - 1);
          cellEndY = std::min(cellEndY, cellCountY - 1);

          m_lineBuilder.Add(
            Rectangle::FromLeftTopRigtBottom(cellStartX * cellSizeX, cellStartY * cellSizeY, (cellEndX + 1) * cellSizeX, (cellEndY + 1) * cellSizeY),
            RenderColor::CandidateArea);

          for (int32_t y = cellStartY; y <= cellEndY; ++y)
          {
            for (int32_t x = cellStartX; x <= cellEndX; ++x)
            {
              auto span = m_resData.SpatialHashGrid.GetChunkEntries(x, y);
              for (std::size_t i = 0; i < span.size(); ++i)
              {
                m_resData.Records[span[i].Id].BaseColor = RenderColor::Candidate;
              }
            }
          }
        }
      }
    }

    // constexpr Color color = UIColors::White();
    for (const auto& record : m_resData.Records)
    {
      m_lineBuilder.Add(record.Rect, record.BaseColor);
    }

    {    // Draw the candidate area
      m_lineBuilder.Add(candidateRect, RenderColor::SelectionArea);
    }

    m_uiExtension->Draw();
  }


  void Shared::SetDefaultValues()
  {
  }


  Shared::UIRecord Shared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto legend0 = CreateLegend(uiFactory, UILegendColor::SelectionArea, "Selection area");
    auto legend1 = CreateLegend(uiFactory, UILegendColor::CandidateArea, "Candidate area");
    auto legend2 = CreateLegend(uiFactory, UILegendColor::Candidate, "Candidate cells");

    auto leftBarLayout = std::make_shared<UI::StackLayout>(context);
    leftBarLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    leftBarLayout->SetAlignmentX(UI::ItemAlignment::Center);
    leftBarLayout->SetSpacing(DpSize1DF::Create(10));
    leftBarLayout->AddChild(legend0);
    leftBarLayout->AddChild(legend1);
    leftBarLayout->AddChild(legend2);


    auto menuBar = uiFactory.CreateBottomBar(leftBarLayout, UI::Theme::BarType::Transparent);

    auto mainLayout = std::make_shared<UI::FillLayout>(context);
    mainLayout->AddChild(menuBar);
    return {mainLayout};
  }
}
