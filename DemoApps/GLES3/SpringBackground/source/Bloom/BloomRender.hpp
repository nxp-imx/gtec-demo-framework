#ifndef BLOOM_GLES3_SPRINGBACKGROUND_BLOOMRENDER_HPP
#define BLOOM_GLES3_SPRINGBACKGROUND_BLOOMRENDER_HPP
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
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeBatch2D.hpp>
#include <FslUtil/OpenGLES3/GLFrameBuffer.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <deque>
#include <vector>

namespace Fsl
{
  class IScene;

  struct BloomConfig
  {
    bool IsBrightPassEnabled{true};
    bool IsScaleInputSequentiallyEnabled{true};
    bool IsBlurEnabled{true};
    bool IsFinalSceneEnabled{true};
    bool IsFinalBloomEnabled{true};
    bool IsShowBuffersEnabled{false};
    float BlendLevel{1.0f};

    BloomConfig() = default;
  };


  class BloomRender
  {
    Point2 m_screenResolution;
    std::shared_ptr<GLES3::NativeBatch2D> m_batch;
    Vector3 m_rotation;
    Vector3 m_rotationSpeed;
    Vector3 m_storedStartRotation;
    GLES3::GLProgram m_programCopy;
    GLES3::GLProgram m_programBrightPass;
    GLES3::GLProgram m_programBlurHPass;
    GLES3::GLProgram m_programBlurVPass;
    GLES3::GLProgram m_programBloomPass;

    GLES3::GLVertexBuffer m_vbFullScreen;

    GLES3::GLFrameBuffer m_fbBlur16A;
    GLES3::GLFrameBuffer m_fbBlur16B;
    GLES3::GLFrameBuffer m_fbBlur32A;
    GLES3::GLFrameBuffer m_fbBlur32B;
    GLES3::GLFrameBuffer m_fbBlur64A;
    GLES3::GLFrameBuffer m_fbBlur64B;
    GLES3::GLFrameBuffer m_fbBlur128A;
    GLES3::GLFrameBuffer m_fbBlur128B;
    GLES3::GLFrameBuffer m_fbBlur256A;
    GLES3::GLFrameBuffer m_fbBlur256B;
    GLES3::GLFrameBuffer m_fbRender256;

    GLint m_locBlurHTexSize;
    GLint m_locBlurVTexSize;
    GLint m_locBloomTexture256;
    GLint m_locBloomTexture128;
    GLint m_locBloomTexture64;
    GLint m_locBloomTexture32;
    GLint m_locBloomTexture16;
    GLint m_locBloomTextureLevel;

    // bool m_renderUI;
    std::string m_strShaderVertPass;
    BloomConfig m_config;

  public:
    explicit BloomRender(const DemoAppConfig& config);
    ~BloomRender();

    BloomConfig GetConfig() const
    {
      return m_config;
    }
    void SetConfig(const BloomConfig& config)
    {
      m_config = config;
    }

    void Update(const DemoTime& demoTime);
    void Draw(IScene& scene);

  private:
    void DrawFinalComposite(IScene& scene);
    void PostProcessBlurH(const GLES3::GLFrameBuffer& dst, const GLES3::GLFrameBuffer& src);
    void PostProcessBlurV(const GLES3::GLFrameBuffer& dst, const GLES3::GLFrameBuffer& src);
    void PostProcess(const GLES3::GLFrameBuffer& dst, const GLES3::GLFrameBuffer& src, const GLES3::GLProgram& program);
    void PostProcess(const GLES3::GLFrameBuffer& dst, const GLES3::GLFrameBuffer& src);
  };
}

#endif
