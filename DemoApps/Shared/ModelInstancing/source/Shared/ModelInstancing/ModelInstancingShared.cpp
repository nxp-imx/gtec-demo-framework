/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Vector.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/ModelInstancing/ModelInstancingShared.hpp>
#include <Shared/ModelInstancing/OptionParser.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");

      constexpr float DefaultZoom = 14.0f;
      constexpr bool RotateDefault = true;

      constexpr float SpeedX = -0.6f;
      constexpr float SpeedY = -0.3f;
      constexpr float SpeedZ = -0.2f;
      constexpr float StartMod = -80;

      // constexpr ConstrainedValue<uint32_t> Instances(MeshInstancingConfig::ModelMaxInstances, 0, MeshInstancingConfig::ModelMaxInstances);
    }

    MeshInstanceSetup ExtractSetup(const OptionParser& options)
    {
      return {options.GetMaxInstancesX(), options.GetMaxInstancesY(), options.GetMaxInstancesZ()};
    }

  }

  ModelInstancingShared::ModelInstancingShared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
    , m_instanceSetup(ExtractSetup(*config.GetOptions<OptionParser>()))
    , m_rotationSpeed(LocalConfig::SpeedX, LocalConfig::SpeedY, LocalConfig::SpeedZ)
    , m_rotation(LocalConfig::SpeedX * LocalConfig::StartMod, LocalConfig::SpeedY * LocalConfig::StartMod,
                 LocalConfig::SpeedZ * LocalConfig::StartMod)
    , m_instanceData(m_instanceSetup.MaxInstances)
  {
    FSLLOG3_INFO("MaxInstancesX: {}", m_instanceSetup.MaxInstancesX);
    FSLLOG3_INFO("MaxInstancesY: {}", m_instanceSetup.MaxInstancesY);
    FSLLOG3_INFO("MaxInstancesZ: {}", m_instanceSetup.MaxInstancesZ);
    FSLLOG3_INFO("MaxInstances: {}", m_instanceSetup.MaxInstances);

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    m_ui = CreateUI(*uiControlFactory, m_instanceSetup);
    m_uiExtension->SetMainWindow(m_ui.MainWindow);

    // auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    {
      constexpr float SpacingX = 1;
      constexpr float SpacingY = 1;
      constexpr float SpacingZ = 1;
      const float xStart = -SpacingX * (static_cast<float>(m_instanceSetup.MaxInstancesX) / 2.0f);
      const float yStart = SpacingY * (static_cast<float>(m_instanceSetup.MaxInstancesY) / 2.0f);
      const float zStart = SpacingZ * (static_cast<float>(m_instanceSetup.MaxInstancesZ) / 2.0f);
      uint32_t dstIndex = 0;
      for (uint32_t z = 0; z < m_instanceSetup.MaxInstancesZ; ++z)
      {
        for (uint32_t y = 0; y < m_instanceSetup.MaxInstancesY; ++y)
        {
          for (uint32_t x = 0; x < m_instanceSetup.MaxInstancesX; ++x)
          {
            float xPos = xStart + static_cast<float>(x) * SpacingX;
            float yPos = yStart - static_cast<float>(y) * SpacingY;
            float zPos = zStart - static_cast<float>(z) * SpacingZ;
            m_instanceData[dstIndex] = MeshInstanceData(Matrix::CreateTranslation(xPos, yPos, zPos));
            ++dstIndex;
          }
        }
      }
    }

    OnConfigurationChanged(config.WindowMetrics);
  }


  ModelInstancingShared::~ModelInstancingShared() = default;


  void ModelInstancingShared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::Q:
      m_ui.Instances->SubValue(m_instanceSetup.MaxInstancesX * m_instanceSetup.MaxInstancesY);
      break;
    case VirtualKey::E:
      m_ui.Instances->AddValue(m_instanceSetup.MaxInstancesX * m_instanceSetup.MaxInstancesY);
      break;
    case VirtualKey::DownArrow:
      m_ui.Instances->SubValue(1);
      break;
    case VirtualKey::LeftArrow:
      m_ui.Instances->SubValue(m_instanceSetup.MaxInstancesX);
      break;
    case VirtualKey::UpArrow:
      m_ui.Instances->AddValue(1);
      break;
    case VirtualKey::RightArrow:
      m_ui.Instances->AddValue(m_instanceSetup.MaxInstancesX);
      break;
    case VirtualKey::R:
      ToggleRotate();
      break;
    case VirtualKey::Space:
      SetDefaultValues();
      break;
    default:
      break;
    }
  }


  void ModelInstancingShared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_ui.ButtonDefault)
    {
      SetDefaultValues();
    }
  }


  void ModelInstancingShared::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(theEvent);
  }


  void ModelInstancingShared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_matrices.Proj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), windowMetrics.AspectRatio(), 1.0f, 1000.0f);
    m_matrices.View = Matrix::CreateTranslation(0, 0, -LocalConfig::DefaultZoom);
    m_matrices.Model = Matrix::GetIdentity();
  }


  void ModelInstancingShared::Update(const DemoTime& demoTime)
  {
    if (m_ui.SwitchRotate->IsChecked())
    {
      m_rotation.X = MathHelper::WrapAngle(m_rotation.X + (m_rotationSpeed.X * demoTime.DeltaTime));
      m_rotation.Y = MathHelper::WrapAngle(m_rotation.Y + (m_rotationSpeed.Y * demoTime.DeltaTime));
      m_rotation.Z = MathHelper::WrapAngle(m_rotation.Z + (m_rotationSpeed.Z * demoTime.DeltaTime));
    }
    m_matrices.Model = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
  }


  void ModelInstancingShared::Draw()
  {
    m_uiExtension->Draw();
  }


  void ModelInstancingShared::SetStats(const ModelRenderStats& stats)
  {
    m_ui.Stats.LabelVertices->SetContent(stats.VertexCount);
    m_ui.Stats.LabelIndices->SetContent(stats.IndexCount);

    {    // Data bind
      auto propertyHandle = m_ui.Instances->GetPropertyHandle(UI::SliderAndFmtValueLabel<uint32_t>::PropertyValue);

      const auto vertexCount = stats.VertexCount;
      const auto indexCount = stats.IndexCount;
      auto totalVertices = std::make_shared<Fsl::DataBinding::ConverterBinding<uint32_t, uint32_t>>([vertexCount](const uint32_t instanceCount)
                                                                                                    { return instanceCount * vertexCount; });
      auto totalIndices = std::make_shared<Fsl::DataBinding::ConverterBinding<uint32_t, uint32_t>>([indexCount](const uint32_t instanceCount)
                                                                                                   { return instanceCount * indexCount; });

      m_ui.Stats.LabelTotalIndices->SetBinding(UI::FmtValueLabel<uint32_t>::PropertyContent, DataBinding::Binding(totalIndices, propertyHandle));
      m_ui.Stats.LabelTotalVertices->SetBinding(UI::FmtValueLabel<uint32_t>::PropertyContent, DataBinding::Binding(totalVertices, propertyHandle));
    }
  }


  ReadOnlySpan<MeshInstanceData> ModelInstancingShared::GetInstanceSpan()
  {
    return SpanUtil::AsReadOnlySpan(m_instanceData, 0, GetInstanceCount());
  }


  void ModelInstancingShared::SetDefaultValues()
  {
    m_ui.SwitchRotate->SetIsChecked(LocalConfig::RotateDefault);
    m_ui.Instances->SetValue(m_instanceSetup.MaxInstances);
    m_rotation = {LocalConfig::SpeedX * LocalConfig::StartMod, LocalConfig::SpeedY * LocalConfig::StartMod,
                  LocalConfig::SpeedZ * LocalConfig::StartMod};
  }


  void ModelInstancingShared::ToggleRotate()
  {
    m_ui.SwitchRotate->Toggle();
  }


  ModelInstancingShared::UIRecord ModelInstancingShared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const MeshInstanceSetup instanceSetup)
  {
    auto context = uiFactory.GetContext();

    auto btnDefault = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Set defaults");
    btnDefault->SetAlignmentX(UI::ItemAlignment::Center);
    btnDefault->SetAlignmentY(UI::ItemAlignment::Far);

    auto lblInstances = uiFactory.CreateLabel("Instances:");
    ConstrainedValue<uint32_t> instances(instanceSetup.MaxInstances, 0, instanceSetup.MaxInstances);
    auto sliderInstances = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, instances);
    sliderInstances->SetAlignmentX(UI::ItemAlignment::Stretch);
    sliderInstances->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto switchRotate = uiFactory.CreateSwitch("Rotate", LocalConfig::RotateDefault);
    switchRotate->SetAlignmentX(UI::ItemAlignment::Stretch);
    switchRotate->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto stack = std::make_shared<UI::StackLayout>(context);
    stack->SetAlignmentX(UI::ItemAlignment::Stretch);
    stack->SetAlignmentY(UI::ItemAlignment::Center);
    stack->AddChild(lblInstances);
    stack->AddChild(sliderInstances);
    stack->AddChild(switchRotate);

    auto stats = CreateStatsOverlayUI(uiFactory, context);

    auto leftBarLayout = std::make_shared<UI::GridLayout>(context);
    leftBarLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    leftBarLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
    leftBarLayout->AddChild(stack, 0, 0);
    leftBarLayout->AddChild(btnDefault, 0, 0);

    auto bottomBar = uiFactory.CreateLeftBar(leftBarLayout);

    auto fillLayout = std::make_shared<UI::FillLayout>(context);
    fillLayout->AddChild(stats.MainOverlay);
    fillLayout->AddChild(bottomBar);

    {    // Data bind
      auto binding = sliderInstances->GetPropertyHandle(UI::SliderAndFmtValueLabel<uint32_t>::PropertyValue);
      stats.LabelInstanceCount->SetBinding(UI::FmtValueLabel<uint32_t>::PropertyContent, binding);
    }

    return {fillLayout, sliderInstances, switchRotate, btnDefault, stats};
  }


  ModelInstancingShared::StatsOverlayUI ModelInstancingShared::CreateStatsOverlayUI(UI::Theme::IThemeControlFactory& uiFactory,
                                                                                    const std::shared_ptr<UI::WindowContext>& context)
  {
    auto lblDesc0 = uiFactory.CreateLabel("Mesh vertices:");
    auto lblDesc1 = uiFactory.CreateLabel("Mesh indices:");
    auto lblDesc2 = uiFactory.CreateLabel("Instances:");
    auto lblDesc3 = uiFactory.CreateLabel("Total vertices:");
    auto lblDesc4 = uiFactory.CreateLabel("Total indices:");

    auto lbl0 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(0));
    auto lbl1 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(0));
    auto lbl2 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(1));
    auto lbl3 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(0));
    auto lbl4 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(0));
    lbl0->SetAlignmentX(UI::ItemAlignment::Far);
    lbl1->SetAlignmentX(UI::ItemAlignment::Far);
    lbl2->SetAlignmentX(UI::ItemAlignment::Far);
    lbl3->SetAlignmentX(UI::ItemAlignment::Far);
    lbl4->SetAlignmentX(UI::ItemAlignment::Far);

    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 100.0f));

    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    layout->AddChild(lblDesc0, 0, 0);
    layout->AddChild(lblDesc1, 0, 1);
    layout->AddChild(lblDesc2, 0, 2);
    layout->AddChild(lblDesc3, 0, 3);
    layout->AddChild(lblDesc4, 0, 4);

    layout->AddChild(lbl0, 1, 0);
    layout->AddChild(lbl1, 1, 1);
    layout->AddChild(lbl2, 1, 2);
    layout->AddChild(lbl3, 1, 3);
    layout->AddChild(lbl4, 1, 4);

    std::shared_ptr<UI::Background> mainLayout = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Transparent, layout);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Far);
    return {mainLayout, lbl0, lbl1, lbl2, lbl3, lbl4};
  }

}
