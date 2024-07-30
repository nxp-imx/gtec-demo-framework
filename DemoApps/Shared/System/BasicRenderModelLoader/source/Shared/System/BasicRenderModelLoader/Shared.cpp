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

#include <FslAssimp/SceneImporter.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Vector.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
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
#include <Shared/System/BasicRenderModelLoader/Shared.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");
      constexpr IO::PathView ModelPath("Models/Knight2/armor.obj");
      constexpr IO::PathView ModelTexturePath("Models/Knight2/armor_default_color.jpg");

      constexpr float DefaultModelScale = 5.0f;
      constexpr float DefaultZoom = 10.0f;

      constexpr bool RotateDefault = true;
    }

    using TestMesh = Graphics3D::GenericMesh<VertexPositionColorTexture, uint16_t>;
    using TestScene = Graphics3D::GenericScene<TestMesh>;

  }

  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
    , m_render(config.DemoServiceProvider.Get<IGraphicsService>()->GetBasicRenderSystem())
    , m_rotationSpeed(0.0f, -0.6f, 0.0f)
  {
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    m_ui = CreateUI(*uiControlFactory);
    m_uiExtension->SetMainWindow(m_ui.MainWindow);

    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();
    m_modelRecord = LoadModel(*contentManager, *m_render);

    m_ui.Stats.LabelVertices->SetContent(NumericCast<uint32_t>(m_modelRecord.Model.VertexCount));
    m_ui.Stats.LabelIndices->SetContent(NumericCast<uint32_t>(m_modelRecord.Model.IndexCount));
    m_ui.Stats.LabelSubMeshes->SetContent(NumericCast<uint32_t>(m_modelRecord.Model.SubMeshes.size()));


    OnConfigurationChanged(config.WindowMetrics);
  }


  Shared::~Shared() = default;


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
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


  void Shared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_ui.ButtonDefault)
    {
      SetDefaultValues();
    }
  }


  void Shared::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(theEvent);
  }


  void Shared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_matrices.Proj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), windowMetrics.AspectRatio(), 1.0f, 1000.0f);
    m_matrices.View = Matrix::CreateTranslation(0, 0, -LocalConfig::DefaultZoom);
    m_matrices.Model = Matrix::GetIdentity();
  }


  void Shared::Update(const DemoTime& demoTime)
  {
    if (m_ui.SwitchRotate->IsChecked())
    {
      m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
      m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
      m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
    }
    m_matrices.Model = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
  }


  void Shared::Draw(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    m_render->BeginCmds();
    m_render->CmdSetCamera(BasicCameraInfo(m_matrices.Model, m_matrices.View, m_matrices.Proj));
    DrawModel(*m_render, m_modelRecord);
    m_render->EndCmds();

    // Draw the UI
    m_uiExtension->Draw();
  }


  void Shared::SetDefaultValues()
  {
    m_ui.SwitchRotate->SetIsChecked(LocalConfig::RotateDefault);
    m_rotation = {};
  }


  void Shared::ToggleRotate()
  {
    m_ui.SwitchRotate->Toggle();
  }


  void Shared::DrawModel(IBasicRenderSystem& render, const ModelRenderRecord& modelRecord)
  {
    render.CmdBindMaterial(modelRecord.Material);
    for (std::size_t i = 0; i < modelRecord.Model.SubMeshes.size(); ++i)
    {
      const auto& submesh = modelRecord.Model.SubMeshes[i];
      render.CmdBindVertexBuffer(submesh.VertexBuffer);
      render.CmdBindIndexBuffer(submesh.IndexBuffer);
      render.CmdDrawIndexed(submesh.IndexCount, 0);
    }
  }


  Shared::UIRecord Shared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = uiFactory.GetContext();

    auto btnDefault = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Set defaults");
    btnDefault->SetAlignmentX(UI::ItemAlignment::Far);
    btnDefault->SetAlignmentY(UI::ItemAlignment::Center);

    auto switchRotate = uiFactory.CreateSwitch("Rotate", LocalConfig::RotateDefault);
    switchRotate->SetAlignmentX(UI::ItemAlignment::Center);
    switchRotate->SetAlignmentY(UI::ItemAlignment::Center);

    auto stats = CreateStatsOverlayUI(uiFactory, context);

    auto bottomBarLayout = std::make_shared<UI::GridLayout>(context);
    bottomBarLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    bottomBarLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bottomBarLayout->AddChild(switchRotate, 0, 0);
    bottomBarLayout->AddChild(btnDefault, 0, 0);

    auto bottomBar = uiFactory.CreateBottomBar(bottomBarLayout);

    auto fillLayout = std::make_shared<UI::FillLayout>(context);
    fillLayout->AddChild(stats.MainOverlay);
    fillLayout->AddChild(bottomBar);

    return {fillLayout, switchRotate, btnDefault, stats};
  }


  Shared::StatsOverlayUI Shared::CreateStatsOverlayUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context)
  {
    auto lblDesc0 = uiFactory.CreateLabel("Vertices:");
    auto lblDesc1 = uiFactory.CreateLabel("Indices:");
    auto lblDesc2 = uiFactory.CreateLabel("SubMeshes:");

    auto lbl0 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(0));
    auto lbl1 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(0));
    auto lbl2 = uiFactory.CreateFmtValueLabel(static_cast<uint32_t>(0));
    lbl0->SetAlignmentX(UI::ItemAlignment::Far);
    lbl1->SetAlignmentX(UI::ItemAlignment::Far);
    lbl2->SetAlignmentX(UI::ItemAlignment::Far);

    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 70.0f));

    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    layout->AddChild(lblDesc0, 0, 0);
    layout->AddChild(lblDesc1, 0, 1);
    layout->AddChild(lblDesc2, 0, 2);

    layout->AddChild(lbl0, 1, 0);
    layout->AddChild(lbl1, 1, 1);
    layout->AddChild(lbl2, 1, 2);

    std::shared_ptr<UI::Background> mainLayout = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Transparent, layout);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Far);
    return {mainLayout, lbl0, lbl1, lbl2};
  }


  Shared::ModelRenderRecord Shared::LoadModel(IContentManager& contentManager, IBasicRenderSystem& render)
  {
    // Load the model
    auto contentPath = contentManager.GetContentPath();
    auto modelRecord = LoadModel(render, contentPath, LocalConfig::ModelPath);

    // Load the texture and create the material
    BasicMaterial modelMaterial;
    {
      const BasicMaterialDepthInfo depthInfoWriteCompare(true, true, BasicCompareOp::Less);
      constexpr auto BitmapOrigin = BitmapOrigin::LowerLeft;
      auto texture = contentManager.ReadTexture(LocalConfig::ModelTexturePath, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin);
      auto modelTexture = render.CreateTexture2D(texture, Texture2DFilterHint::Smooth);
      modelMaterial = render.CreateMaterial(BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Back, BasicFrontFace::CounterClockwise,
                                                                    depthInfoWriteCompare, modelRecord.VertexDecl.AsSpan()),
                                            modelTexture);
    }
    return {modelRecord, std::move(modelMaterial)};
  }


  Shared::ModelRecord Shared::LoadModel(IBasicRenderSystem& render, const IO::Path& contentPath, const IO::Path& srcModelPath)
  {
    FSLLOG3_INFO("Loading model '{}'", srcModelPath);
    auto modelPath = IO::Path::Combine(contentPath, srcModelPath);
    SceneImporter sceneImporter;
    const std::shared_ptr<TestScene> scene = sceneImporter.Load<TestScene>(modelPath, LocalConfig::DefaultModelScale, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException("Scene did not contain any meshes");
    }

    auto rootNode = scene->GetRootNode();
    if (!rootNode)
    {
      throw NotSupportedException("Scene did not contain a root node");
    }

    // The most basic allocation of vertex and index buffers, not optimized in any way
    std::vector<SubMeshRecord> subMeshes(scene->Meshes.size());
    std::size_t totalVertexCount = 0;
    std::size_t totalIndexCount = 0;
    for (std::size_t i = 0; i < scene->Meshes.size(); ++i)
    {
      auto mesh = scene->Meshes[i];
      subMeshes[i] = SubMeshRecord(render.CreateStaticBuffer(mesh->AsReadOnlyFlexVertexSpan()),
                                   render.CreateStaticBuffer(mesh->AsReadOnlyIndexSpan()), mesh->GetVertexCount(), mesh->GetIndexCount());
      totalVertexCount += mesh->GetVertexCount();
      totalIndexCount += mesh->GetIndexCount();
    }

    FSLLOG3_INFO("Total vertex count: {}, Total index count: {}", totalVertexCount, totalIndexCount);
    return {totalVertexCount, totalIndexCount, subMeshes, scene->Meshes[0]->AsVertexDeclarationSpan()};
  }
}
