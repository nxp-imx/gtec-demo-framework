#ifndef GLES3_HDR03_SKYBOXTONEMAPPING_HDR03_SKYBOXTONEMAPPING_HPP
#define GLES3_HDR03_SKYBOXTONEMAPPING_HDR03_SKYBOXTONEMAPPING_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/Transition/TransitionValue.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslDemoApp/Base/Service/Keyboard/IKeyboard.hpp>
#include <FslDemoApp/Base/Service/Mouse/IMouse.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/FloatSliderAndValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/CanvasLayout.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLVertexArray.hpp>
#include <FslUtil/OpenGLES3/GLFrameBuffer.hpp>
#include <Shared/OpenGLES3/CubeMapping/SkyboxMesh.hpp>
#include <Shared/OpenGLES3/CubeMapping/SkyboxProgram.hpp>
#include <vector>
#include <utility>
#include "OptionParser.hpp"

namespace Fsl
{
  class HDR03_SkyboxTonemapping
    : public DemoAppGLES3
    , public UI::EventListener
  {
    enum Tonemapper
    {
      LinearNoGamma = 0,
      Linear,
      ReinhardLum,
      Uncharted2Lum,
      Custom,
      COUNT
    };

    struct TonemapProgramLocations
    {
      GLint Exposure;
      TonemapProgramLocations()
        : Exposure(GLES3::GLValues::INVALID_LOCATION)
      {
      }
    };

    struct TonemapProgramInfo
    {
      GLES3::GLProgram Program;
      TonemapProgramLocations Location;
    };

    struct SimpleMesh
    {
      GLES3::GLVertexBuffer VertexBuffer;
      GLES3::GLVertexArray VertexArray;

      SimpleMesh() = default;

      SimpleMesh(GLES3::GLVertexBuffer&& vertexBuffer, GLES3::GLVertexArray&& vertexArray)
        : VertexBuffer(std::move(vertexBuffer))
        , VertexArray(std::move(vertexArray))
      {
      }
    };

    struct Scene
    {
      GLES3::GLTexture CubemapTexture;
      SkyboxProgram Program;
      SkyboxMesh Mesh;

      Scene() = default;
    };

    struct RenderRecord
    {
      std::string Name;
      TransitionValue SplitX;
      TransitionValue LabelAlpha;

      RenderRecord() = default;

      RenderRecord(TransitionCache& rTransitionCache, const TransitionTimeSpan& timeSplitX, const TransitionTimeSpan& timeLabel)
        : SplitX(rTransitionCache, timeSplitX)
        , LabelAlpha(rTransitionCache, timeLabel)
      {
      }
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<UI::CanvasLayout> m_rootCanvas;
    std::shared_ptr<UI::BaseWindow> m_configWindow;
    std::shared_ptr<UI::FloatSliderAndValueLabel> m_exposureSlider;
    std::vector<std::shared_ptr<UI::CheckBox>> m_checkboxes;

    std::shared_ptr<INativeBatch2D> m_nativeBatch;
    std::shared_ptr<AtlasFont> m_defaultFont;

    std::shared_ptr<IKeyboard> m_keyboard;
    std::shared_ptr<IMouse> m_mouse;
    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    bool m_mouseCaptureEnabled;
    Graphics3D::FirstPersonCamera m_camera;
    Matrix m_matrixModel;
    Matrix m_matrixView;
    Matrix m_matrixProjection;

    Scene m_scene;

    std::vector<TonemapProgramInfo> m_programTonemap;

    GLES3::GLFrameBuffer m_hdrFrameBuffer;

    SimpleMesh m_meshQuad;

    float m_exposure;

    TransitionCache m_transitionCache;
    std::vector<RenderRecord> m_render;
    uint32_t m_sceneRenderFlags;
    Vector2 m_rotationSpeed;

  public:
    HDR03_SkyboxTonemapping(const DemoAppConfig& config);
    ~HDR03_SkyboxTonemapping() override;
    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void ToggleState(const SceneFlags newState);
    void SetState(const SceneFlags newState, const bool enabled);
    void SetStateViaFlags(const uint32_t flags);
    void UpdateUIState();
    void UpdateStateBasedOnCheckboxes(const std::shared_ptr<UI::CheckBox>& source);
    void UpdateInput(const DemoTime& demoTime);
    void UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState);
    void UpdateExposureInput(const DemoTime& demoTime, const KeyboardState& keyboardState);
    void UpdateSceneTransition(const DemoTime& demoTime);

    void DrawLabels();
    void DrawScene(const Scene& scene, const Matrix& matrixView, const Matrix& matrixProjection);
    void DrawTonemappedScene(const TonemapProgramInfo& programInfo, const GLES3::GLTextureInfo& hdrFramebufferTexInfo);
    void PrepareTransition(std::vector<RenderRecord>& rRender, const std::size_t entries);
    void ForceCompleteTransitions(std::vector<RenderRecord>& rRender);
    void PrepareScene(const std::shared_ptr<IContentManager>& contentManager, Scene& rScene);

    static void CreateTonemappers(std::vector<TonemapProgramInfo>& rVector, const std::shared_ptr<IContentManager>& contentManager);
    static TonemapProgramInfo CreateTonemapShader(const std::shared_ptr<IContentManager>& contentManager, const Tonemapper tonemapper);
    static const char* GetTonemapperShaderName(const Tonemapper tonemapper);
    GLES3::GLFrameBuffer CreateHdrFrameBuffer();
    SimpleMesh CreateQuadVertexArray(const GLES3::GLProgram& program);
    std::shared_ptr<UI::CanvasLayout> CreateUI();
    std::shared_ptr<UI::BaseWindow> CreateConfigDialog(const std::vector<RenderRecord>& render);
  };
}

#endif
