#ifndef GLES2_BLOOM_BLOOM_HPP
#define GLES2_BLOOM_BLOOM_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeBatch2D.hpp>
#include <FslGraphics3D/Camera/ArcballCamera.hpp>
#include <FslUtil/OpenGLES2/GLFrameBuffer.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>
#include <Shared/Bloom/MenuUI.hpp>
#include <deque>
#include <vector>


namespace Fsl
{
  class IScene;

  class Bloom : public DemoAppGLES2
  {
    enum class BlurShaderType
    {
      Gaussian5X5,
      Gaussian9X9,
      Custom
    };

    MenuUI m_menuUI;

    std::shared_ptr<GLES2::NativeBatch2D> m_batch;
    Graphics3D::ArcballCamera m_camera;
    Vector3 m_rotation;
    Vector3 m_rotationSpeed;
    Vector3 m_storedStartRotation;
    std::shared_ptr<IScene> m_sceneRender;
    std::shared_ptr<IScene> m_sceneWhiteRect;
    std::shared_ptr<IScene> m_scene;
    GLES2::GLProgram m_programCopy;
    GLES2::GLProgram m_programBrightPass;
    GLES2::GLProgram m_programBlurHPass;
    GLES2::GLProgram m_programBlurVPass;
    GLES2::GLProgram m_programBloomPass;

    GLES2::GLVertexBuffer m_vbFullScreen;

    GLES2::GLFrameBuffer m_fbBlur16A;
    GLES2::GLFrameBuffer m_fbBlur16B;
    GLES2::GLFrameBuffer m_fbBlur32A;
    GLES2::GLFrameBuffer m_fbBlur32B;
    GLES2::GLFrameBuffer m_fbBlur64A;
    GLES2::GLFrameBuffer m_fbBlur64B;
    GLES2::GLFrameBuffer m_fbBlur128A;
    GLES2::GLFrameBuffer m_fbBlur128B;
    GLES2::GLFrameBuffer m_fbBlur256A;
    GLES2::GLFrameBuffer m_fbBlur256B;
    GLES2::GLFrameBuffer m_fbRender256;

    GLint m_locBlurHTexSize;
    GLint m_locBlurVTexSize;
    GLint m_locBloomTexture256;
    GLint m_locBloomTexture128;
    GLint m_locBloomTexture64;
    GLint m_locBloomTexture32;
    GLint m_locBloomTexture16;
    GLint m_locBloomTextureLevel;

    bool m_renderUI;
    float m_gaussianBlurKernelWeight;
    std::string m_strShaderVertPass;
    BlurShaderType m_activeBlueShaderType;

  public:
    explicit Bloom(const DemoAppConfig& config);
    ~Bloom() override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void OnMouseWheelEvent(const MouseWheelEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void DrawFinalComposite();
    void PostProcessBlurH(const GLES2::GLFrameBuffer& dst, const GLES2::GLFrameBuffer& src);
    void PostProcessBlurV(const GLES2::GLFrameBuffer& dst, const GLES2::GLFrameBuffer& src);
    void PostProcess(const GLES2::GLFrameBuffer& dst, const GLES2::GLFrameBuffer& src, const GLES2::GLProgram& program);
    void PostProcess(const GLES2::GLFrameBuffer& dst, const GLES2::GLFrameBuffer& src);

    void SetBlurShader(const BlurShaderType shaderType);
  };
}

#endif
