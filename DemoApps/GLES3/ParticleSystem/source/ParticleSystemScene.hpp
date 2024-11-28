#ifndef GLES3_PARTICLESYSTEM_PARTICLESYSTEMSCENE_HPP
#define GLES3_PARTICLESYSTEM_PARTICLESYSTEMSCENE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslGraphics3D/Camera/ArcballCamera.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <array>
#include "AScene.hpp"
#include "PS/ParticleDrawContext.hpp"

namespace Fsl
{
  class BoxEmitter;
  class IGraphicsService;
  class IParticleSystem;
  class ParticleSystemGLES3;
  class ParticleSystemSnow;

  namespace UI
  {
    class Label;
    class RadioButton;
    class Switch;
    class WindowContext;
  }

  class ParticleSystemScene : public AScene
  {
    enum class ParticleSystemType
    {
      Points,
      Quads,
      Instancing,
      GeometryShader
    };


    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<UI::FmtValueLabel<int32_t>> m_valueLabelParticleCount;
    std::shared_ptr<UI::SliderAndFmtValueLabel<int32_t>> m_sliderEmit;
    std::shared_ptr<UI::RadioButton> m_particleSystemPoints;
    std::shared_ptr<UI::RadioButton> m_particleSystemQuads;
    std::shared_ptr<UI::RadioButton> m_particleSystemGeometryShader;
    std::shared_ptr<UI::Switch> m_cbParticleSystemGPU1;
    std::shared_ptr<UI::Switch> m_cbParticleSystemGPU2;
    std::shared_ptr<UI::FmtValueLabel<int32_t>> m_valueLabelGPUParticleCount;
    bool m_allowAdvancedTechniques;

    Graphics3D::ArcballCamera m_camera;
    GLES3::GLProgram m_program;
    GLES3::GLVertexBuffer m_vbCube;
    std::array<GLES3::GLVertexAttribLink, 3> m_cubeAttribLink;
    GLES3::GLTexture m_texParticle;
    GLES3::GLTexture m_texParticleSnow;
    GLES3::GLTexture m_texCube;
    std::shared_ptr<BoxEmitter> m_boxEmitter;
    std::shared_ptr<IParticleSystem> m_particleSystem;
    std::shared_ptr<ParticleSystemGLES3> m_particleSystemGpu;
    std::shared_ptr<ParticleSystemSnow> m_particleSystemGpu2;

    GLint m_locWorldViewMatrix;
    GLint m_locProjMatrix;
    ParticleDrawContext m_particleDrawContext;
    Vector3 m_rotation;
    Vector3 m_rotationSpeed;
    bool m_rotate;

    ParticleSystemType m_particleSystemType;

  public:
    ParticleSystemScene(const DemoAppConfig& config, const std::shared_ptr<UIDemoAppExtension>& uiExtension);
    ~ParticleSystemScene() override;

    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void OnMouseWheelEvent(const MouseWheelEvent& event) override;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;
    void Update(const DemoTime& demoTime) override;
    void Draw() override;

  private:
    void DrawCube();
    void DrawParticles();
    void BuildUI(const std::shared_ptr<IContentManager>& contentManager, const std::shared_ptr<UIDemoAppExtension>& uiExtension);
    void SetParticleSystem(const ParticleSystemType type, const bool force);
  };
}

#endif
