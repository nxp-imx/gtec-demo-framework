#ifndef SHARED_SYSTEM_BASICRENDERMODELLOADER_SHARED_HPP
#define SHARED_SYSTEM_BASICRENDERMODELLOADER_SHARED_HPP
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

#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslGraphics/Render/Basic/BasicMaterial.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  namespace UI::Theme
  {
    class IThemeControlFactory;
  }

  class Shared final : public UI::EventListener
  {
    struct StatsOverlayUI
    {
      std::shared_ptr<UI::BaseWindow> MainOverlay;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelVertices;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelIndices;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelSubMeshes;

      StatsOverlayUI() = default;

      StatsOverlayUI(std::shared_ptr<UI::BaseWindow> mainOverlay, std::shared_ptr<UI::FmtValueLabel<uint32_t>> labelVertices,
                     std::shared_ptr<UI::FmtValueLabel<uint32_t>> labelIndices, std::shared_ptr<UI::FmtValueLabel<uint32_t>> lavelSubMeshes)
        : MainOverlay(std::move(mainOverlay))
        , LabelVertices(std::move(labelVertices))
        , LabelIndices(std::move(labelIndices))
        , LabelSubMeshes(std::move(lavelSubMeshes))
      {
      }
    };

    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> MainWindow;
      std::shared_ptr<UI::Switch> SwitchRotate;
      std::shared_ptr<UI::BackgroundLabelButton> ButtonDefault;

      StatsOverlayUI Stats;
    };

    struct SubMeshRecord
    {
      std::shared_ptr<IBasicStaticBuffer> VertexBuffer;
      std::shared_ptr<IBasicStaticBuffer> IndexBuffer;
      uint32_t VertexCount{0};
      uint32_t IndexCount{0};

      SubMeshRecord() = default;
      SubMeshRecord(std::shared_ptr<IBasicStaticBuffer> vertexBuffer, std::shared_ptr<IBasicStaticBuffer> indexBuffer, const uint32_t vertexCount,
                    const uint32_t indexCount)
        : VertexBuffer(std::move(vertexBuffer))
        , IndexBuffer(std::move(indexBuffer))
        , VertexCount(vertexCount)
        , IndexCount(indexCount)
      {
      }
    };

    struct ModelRecord
    {
      std::size_t VertexCount{0};
      std::size_t IndexCount{0};
      std::vector<SubMeshRecord> SubMeshes;
      VertexDeclaration VertexDecl;

      ModelRecord() = default;
      ModelRecord(const std::size_t vertexCount, const std::size_t indexCount, std::vector<SubMeshRecord> subMeshes,
                  VertexDeclarationSpan vertexDeclarationSpan)
        : VertexCount(vertexCount)
        , IndexCount(indexCount)
        , SubMeshes(std::move(subMeshes))
        , VertexDecl(vertexDeclarationSpan)
      {
      }
    };

    struct ModelRenderRecord
    {
      ModelRecord Model;
      BasicMaterial Material;

      ModelRenderRecord() = default;
      ModelRenderRecord(ModelRecord model, BasicMaterial material)
        : Model(std::move(model))
        , Material(std::move(material))
      {
      }
    };

    struct Matrices
    {
      Matrix Model;
      Matrix View;
      Matrix Proj;
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    std::shared_ptr<IBasicRenderSystem> m_render;

    UIRecord m_ui;

    ModelRenderRecord m_modelRecord;

    Vector3 m_rotationSpeed;
    Vector3 m_rotation;
    Matrices m_matrices;

  public:
    explicit Shared(const DemoAppConfig& config);
    ~Shared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    void OnKeyEvent(const KeyEvent& event);

    // From EventListener
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);

  private:
    void SetDefaultValues();
    void ToggleRotate();

    static void DrawModel(IBasicRenderSystem& render, const ModelRenderRecord& model);


    static UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory);
    static StatsOverlayUI CreateStatsOverlayUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context);

    static ModelRenderRecord LoadModel(IContentManager& contentManager, IBasicRenderSystem& render);
    static ModelRecord LoadModel(IBasicRenderSystem& render, const IO::Path& contentPath, const IO::Path& srcModelPath);
  };
}

#endif
