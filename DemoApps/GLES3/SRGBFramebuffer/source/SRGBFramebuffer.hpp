#ifndef GLES3_SRGBFRAMEBUFFER_SRGBFRAMEBUFFER_HPP
#define GLES3_SRGBFRAMEBUFFER_SRGBFRAMEBUFFER_HPP
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
#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLVertexArray.hpp>
#include <vector>

namespace Fsl
{
  class SRGBFramebuffer final
    : public DemoAppGLES3
    , public UI::EventListener
  {
    enum class State
    {
      Split2,
      Scene1,
      Scene2
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    std::shared_ptr<IKeyboard> m_keyboard;
    std::shared_ptr<IMouse> m_mouse;
    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    bool m_mouseCaptureEnabled;
    Graphics3D::FirstPersonCamera m_camera;
    Matrix m_matrixWorldViewL;
    Matrix m_matrixProjectionL;
    Matrix m_matrixWorldViewR;
    Matrix m_matrixProjectionR;

    GLES3::GLTexture m_texLinear;
    GLES3::GLTexture m_texSRGB;

    GLES3::GLProgram m_program;
    GLint m_hModelViewMatrixLoc;
    GLint m_hProjMatrixLoc;
    GLint m_hLightPositions;
    GLint m_hLightColors;
    GLint m_hViewPos;

    GLES3::GLVertexBuffer m_vertexBuffer;
    GLES3::GLVertexArray m_vertexArray;

    std::vector<Vector3> m_lightPositions;
    std::vector<Vector3> m_lightColors;

    State m_state;
    TransitionCache m_transitionCache;
    TransitionValue m_splitX;
    TransitionValue m_splitSceneWidthL;
    TransitionValue m_splitSceneWidthR;
    TransitionValue m_splitSceneAlphaL;
    TransitionValue m_splitSceneAlphaR;

    std::shared_ptr<UI::Label> m_labelLeft;
    std::shared_ptr<UI::Label> m_labelRight;
    std::shared_ptr<UI::Switch> m_leftCB;
    std::shared_ptr<UI::Switch> m_rightCB;

  public:
    explicit SRGBFramebuffer(const DemoAppConfig& config);
    ~SRGBFramebuffer() final;

    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;

  protected:
    void OnKeyEvent(const KeyEvent& event) final;
    void OnMouseButtonEvent(const MouseButtonEvent& event) final;
    void Update(const DemoTime& demoTime) final;
    void Draw(const FrameInfo& frameInfo) final;

  private:
    void UpdateInput(const DemoTime& demoTime);
    void UpdateSceneTransition(const DemoTime& demoTime);
    void DrawScenes();
    void PrepareTransition();
    void PrepareLights();
    void CreateTextures(const std::shared_ptr<IContentManager>& contentManager);
    GLES3::GLProgram CreateShader(const std::shared_ptr<IContentManager>& contentManager, const bool hasSRGBFramebuffer);
    void CreateVertexArray(const GLES3::GLProgram& program);
    void CreateUI(const bool hasSRGBFramebuffer);
    void SetState(State state);
    void UpdateUIToState();
  };
}

#endif
