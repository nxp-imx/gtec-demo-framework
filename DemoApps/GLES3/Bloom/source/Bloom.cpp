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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include "Bloom.hpp"
#include <GLES3/gl3.h>
#include <iostream>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <Shared/Bloom/GaussianShaderBuilder.hpp>
#include "RenderScene.hpp"
#include "WhiteRectScene.hpp"
#include "VBHelper.hpp"


namespace Fsl
{
  using namespace GLES3;
  using namespace Graphics3D;

  namespace
  {
    const float DEFAULT_ZOOM = 10;
    const int SIZE_MOD = 2;
    const int SIZE_16 = 16 * SIZE_MOD;
    const int SIZE_32 = 32 * SIZE_MOD;
    const int SIZE_64 = 64 * SIZE_MOD;
    const int SIZE_128 = 128 * SIZE_MOD;
    const int SIZE_256 = 256 * SIZE_MOD;

    const char* const g_pszShaderAttributeArray[] = {"VertexPosition", "VertexTexCoord", nullptr};

    const GLTextureImageParameters g_defaultFBImageParams(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
  }

  // Bloom as described here
  // The idea is not to create the most accurate bloom, but something that is fairly fast.
  // http://prideout.net/archive/bloom/
  // http://kalogirou.net/2006/05/20/how-to-do-good-bloom-for-hdr-rendering/

  Bloom::Bloom(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_menuUI(config)
    , m_batch(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
    , m_camera(config.ScreenResolution)
    , m_rotationSpeed(0, -0.6f, 0)
    , m_fbBlur16A(Point2(SIZE_16, SIZE_16), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur16B(Point2(SIZE_16, SIZE_16), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur32A(Point2(SIZE_32, SIZE_32), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur32B(Point2(SIZE_32, SIZE_32), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur64A(Point2(SIZE_64, SIZE_64), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur64B(Point2(SIZE_64, SIZE_64), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur128A(Point2(SIZE_128, SIZE_128), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur128B(Point2(SIZE_128, SIZE_128), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur256A(Point2(SIZE_256, SIZE_256), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbBlur256B(Point2(SIZE_256, SIZE_256), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams)
    , m_fbRender256(Point2(SIZE_256, SIZE_256), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_defaultFBImageParams,
                    GL_DEPTH_COMPONENT16)
    , m_locBlurHTexSize(GLValues::INVALID_LOCATION)
    , m_locBlurVTexSize(GLValues::INVALID_LOCATION)
    , m_locBloomTexture256(GLValues::INVALID_LOCATION)
    , m_locBloomTexture128(GLValues::INVALID_LOCATION)
    , m_locBloomTexture64(GLValues::INVALID_LOCATION)
    , m_locBloomTexture32(GLValues::INVALID_LOCATION)
    , m_locBloomTexture16(GLValues::INVALID_LOCATION)
    , m_locBloomTextureLevel(GLValues::INVALID_LOCATION)
    , m_renderUI(true)
    , m_gaussianBlurKernelWeight(m_menuUI.GetKernelWeightMod())
    , m_activeBlueShaderType(BlurShaderType::Gaussian5X5)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    {
      int res;
      glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &res);
      FSLLOG("GL_MAX_COLOR_ATTACHMENTS: " << res);
    }

    m_camera.SetZoom(DEFAULT_ZOOM);
    m_storedStartRotation = m_rotation;

    VBHelper::BuildVB(m_vbFullScreen, BoxF(-1, -1, 1, 1), BoxF(0.0f, 0.0f, 1.0f, 1.0f));

    const auto contentManager = GetContentManager();

    m_strShaderVertPass = contentManager->ReadAllText("Shaders/Pass.vert");

    m_programBrightPass.Reset(m_strShaderVertPass, contentManager->ReadAllText("Shaders/BrightPass.frag"), g_pszShaderAttributeArray);
    m_programCopy.Reset(m_strShaderVertPass, contentManager->ReadAllText("Shaders/CopyPass.frag"), g_pszShaderAttributeArray);
    m_programBloomPass.Reset(m_strShaderVertPass, contentManager->ReadAllText("Shaders/BloomPass.frag"), g_pszShaderAttributeArray);

    // Prepare the blur shader
    SetBlurShader(BlurShaderType::Gaussian5X5);

    m_locBloomTexture256 = glGetUniformLocation(m_programBloomPass.Get(), "Texture256");
    m_locBloomTexture128 = glGetUniformLocation(m_programBloomPass.Get(), "Texture128");
    m_locBloomTexture64 = glGetUniformLocation(m_programBloomPass.Get(), "Texture64");
    m_locBloomTexture32 = glGetUniformLocation(m_programBloomPass.Get(), "Texture32");
    m_locBloomTexture16 = glGetUniformLocation(m_programBloomPass.Get(), "Texture16");
    m_locBloomTextureLevel = glGetUniformLocation(m_programBloomPass.Get(), "Level");

    GL_CHECK_FOR_ERROR();


    m_sceneRender = std::make_shared<RenderScene>(config, m_menuUI.GetSceneId());
    m_sceneWhiteRect = std::make_shared<WhiteRectScene>(config);

    if (m_menuUI.GetSceneId() == 2)
    {
      m_scene = m_sceneWhiteRect;
    }
    else
    {
      m_scene = m_sceneRender;
    }
  }


  Bloom::~Bloom() = default;


  void Bloom::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);

    if (!event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::U:
      m_renderUI = !m_renderUI;
      event.Handled();
      break;
    case VirtualKey::Code1:
      m_scene = m_sceneRender;
      event.Handled();
      break;
    case VirtualKey::Code2:
      m_scene = m_sceneWhiteRect;
      event.Handled();
      break;
    case VirtualKey::Code5:
      SetBlurShader(BlurShaderType::Gaussian5X5);
      event.Handled();
      break;
    case VirtualKey::Code6:
      SetBlurShader(BlurShaderType::Gaussian9X9);
      event.Handled();
      break;
    case VirtualKey::Code7:
      SetBlurShader(BlurShaderType::Custom);
      event.Handled();
      break;
    default:
      break;
    }
  }


  void Bloom::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Left:
    {
      if (event.IsPressed())
      {
        m_camera.BeginDrag(event.GetPosition());
      }
      else if (m_camera.IsDragging())
      {
        m_camera.EndDrag(event.GetPosition());
      }
      event.Handled();
    }
    break;
    case VirtualMouseButton::Right:
      if (event.IsPressed())
      {
        m_camera.ResetRotation();
        m_camera.SetZoom(DEFAULT_ZOOM);
        m_rotation = m_storedStartRotation;
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void Bloom::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }


    if (m_camera.IsDragging())
    {
      m_camera.Drag(event.GetPosition());
      event.Handled();
    }
  }


  void Bloom::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    m_camera.AddZoom(event.GetDelta() * -0.001f);
  }

  void Bloom::Update(const DemoTime& demoTime)
  {
    if (m_menuUI.IsRotateEnabled())
    {
      m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
      m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
      m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
    }

    if (m_scene)
    {
      m_scene->Update(demoTime, m_camera.GetViewMatrix(), m_camera.GetRotationMatrix(), m_rotation, GetScreenResolution());
    }

    if (m_menuUI.GetKernelWeightMod() != m_gaussianBlurKernelWeight)
    {
      m_gaussianBlurKernelWeight = m_menuUI.GetKernelWeightMod();
      SetBlurShader(m_activeBlueShaderType);
    }
  }


  void Bloom::Draw(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);

    if (!m_scene)
    {
      return;
    }

    glEnable(GL_DEPTH_TEST);

    // 1. Render the scene to a low res frame buffer
    {
      auto& fb = m_fbRender256;
      glBindFramebuffer(GL_FRAMEBUFFER, fb.Get());
      glViewport(0, 0, fb.GetSize().X, fb.GetSize().Y);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      m_scene->Draw();
    }

    // Since we are only doing opaque 2d-composition type operations we can disable blend and depth testing
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    // 2. Apply bright pass
    if (m_menuUI.IsBrightPassEnabled())
    {
      PostProcess(m_fbBlur256A, m_fbRender256, m_programBrightPass);
    }
    else
    {
      PostProcess(m_fbBlur256A, m_fbRender256, m_programCopy);
    }

    // 3. copy to the smaller blur render targets
    if (m_menuUI.IsScaleInputSequentiallyEnabled())
    {
      PostProcess(m_fbBlur128A, m_fbBlur256A, m_programCopy);
      PostProcess(m_fbBlur64A, m_fbBlur128A, m_programCopy);
      PostProcess(m_fbBlur32A, m_fbBlur64A, m_programCopy);
      PostProcess(m_fbBlur16A, m_fbBlur32A, m_programCopy);
    }
    else
    {
      PostProcess(m_fbBlur128A, m_fbBlur256A, m_programCopy);
      PostProcess(m_fbBlur64A, m_fbBlur256A, m_programCopy);
      PostProcess(m_fbBlur32A, m_fbBlur256A, m_programCopy);
      PostProcess(m_fbBlur16A, m_fbBlur256A, m_programCopy);
    }

    if (m_menuUI.IsBlurEnabled())
    {
      // 4A. Blur the content X
      PostProcessBlurH(m_fbBlur256B, m_fbBlur256A);
      PostProcessBlurH(m_fbBlur128B, m_fbBlur128A);
      PostProcessBlurH(m_fbBlur64B, m_fbBlur64A);
      PostProcessBlurH(m_fbBlur32B, m_fbBlur32A);
      PostProcessBlurH(m_fbBlur16B, m_fbBlur16A);

      // 4B. Blur the content Y
      PostProcessBlurV(m_fbBlur256A, m_fbBlur256B);
      PostProcessBlurV(m_fbBlur128A, m_fbBlur128B);
      PostProcessBlurV(m_fbBlur64A, m_fbBlur64B);
      PostProcessBlurV(m_fbBlur32A, m_fbBlur32B);
      PostProcessBlurV(m_fbBlur16A, m_fbBlur16B);
    }

    DrawFinalComposite();


    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    if (m_renderUI)
    {
      m_menuUI.Draw();
    }
  }

  void Bloom::DrawFinalComposite()
  {
    const Point2 screenResolution = GetScreenResolution();

    // Composite everything
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenResolution.X, screenResolution.Y);

    if (m_menuUI.IsFinalSceneEnabled())
    {
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      m_scene->Draw();
    }
    else
    {
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Draw bloom with a fullscreen additive pass
    if (m_menuUI.IsFinalBloomEnabled())
    {
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);

      auto& vb = m_vbFullScreen;

      glUseProgram(m_programBloomPass.Get());
      glUniform1i(m_locBloomTexture256, 0);
      glUniform1i(m_locBloomTexture128, 1);
      glUniform1i(m_locBloomTexture64, 2);
      glUniform1i(m_locBloomTexture32, 3);
      glUniform1i(m_locBloomTexture16, 4);
      glUniform1f(m_locBloomTextureLevel, m_menuUI.GetBlendLevel());


      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_fbBlur256A.GetTextureInfo().Handle);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_fbBlur128A.GetTextureInfo().Handle);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, m_fbBlur64A.GetTextureInfo().Handle);
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, m_fbBlur32A.GetTextureInfo().Handle);
      glActiveTexture(GL_TEXTURE4);
      glBindTexture(GL_TEXTURE_2D, m_fbBlur16A.GetTextureInfo().Handle);

      glBindBuffer(vb.GetTarget(), vb.Get());
      vb.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      vb.DisableAttribArrays();
    }

    // Draw some debug overlays
    if (m_menuUI.IsShowBuffersEnabled())
    {
      float dstX = 0;
      m_batch->Begin();
      m_batch->Draw(m_fbRender256, Vector2(dstX, 0), Color::White());
      dstX += m_fbRender256.GetSize().X;
      m_batch->Draw(m_fbBlur256A, Vector2(dstX, 0.0f), Color::White());
      dstX += m_fbBlur256A.GetSize().X;
      m_batch->Draw(m_fbBlur128A, Vector2(dstX, 0.0f), Color::White());
      dstX += m_fbBlur128A.GetSize().X;
      m_batch->Draw(m_fbBlur64A, Vector2(dstX, 0.0f), Color::White());
      dstX += m_fbBlur64A.GetSize().X;
      m_batch->Draw(m_fbBlur32A, Vector2(dstX, 0.0f), Color::White());
      dstX += m_fbBlur32A.GetSize().X;
      m_batch->Draw(m_fbBlur16A, Vector2(dstX, 0.0f), Color::White());
      // dstX += m_fbBlur16A.GetSize().X;
      m_batch->End();
    }
  }


  void Bloom::PostProcessBlurH(const GLFrameBuffer& dst, const GLFrameBuffer& src)
  {
    glUseProgram(m_programBlurHPass.Get());
    // glUseProgram(m_programCopy.Get());
    if (m_locBlurHTexSize >= 0)
    {
      glUniform1f(m_locBlurHTexSize, 1.0f / src.GetSize().X);
    }
    PostProcess(dst, src);
  }


  void Bloom::PostProcessBlurV(const GLFrameBuffer& dst, const GLFrameBuffer& src)
  {
    glUseProgram(m_programBlurVPass.Get());
    // glUseProgram(m_programCopy.Get());
    if (m_locBlurVTexSize >= 0)
    {
      glUniform1f(m_locBlurVTexSize, 1.0f / src.GetSize().Y);
    }
    PostProcess(dst, src);
  }


  void Bloom::PostProcess(const GLFrameBuffer& dst, const GLFrameBuffer& src, const GLProgram& program)
  {
    glUseProgram(program.Get());
    PostProcess(dst, src);
  }


  void Bloom::PostProcess(const GLFrameBuffer& dst, const GLFrameBuffer& src)
  {
    auto& fb = dst;
    auto& vb = m_vbFullScreen;
    glBindFramebuffer(GL_FRAMEBUFFER, fb.Get());
    glViewport(0, 0, fb.GetSize().X, fb.GetSize().Y);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, src.GetTextureInfo().Handle);

    glBindBuffer(vb.GetTarget(), vb.Get());
    vb.EnableAttribArrays();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    vb.DisableAttribArrays();
  }


  void Bloom::SetBlurShader(const BlurShaderType shaderType)
  {
    const float gaussianBlurKernelWeightMod = m_menuUI.GetKernelWeightMod() / float(m_menuUI.GetKernelWeightRange());

    auto contentManager = GetContentManager();
    switch (shaderType)
    {
    case BlurShaderType::Custom:
      m_programBlurHPass.Reset(m_strShaderVertPass, contentManager->ReadAllText("Shaders/BlurHPass.frag"), g_pszShaderAttributeArray);
      m_programBlurVPass.Reset(m_strShaderVertPass, contentManager->ReadAllText("Shaders/BlurVPass.frag"), g_pszShaderAttributeArray);
      break;
    case Bloom::Gaussian9X9:
      m_programBlurHPass.Reset(
        m_strShaderVertPass,
        GaussianShaderBuilder::Build9x9(contentManager->ReadAllText("Shaders/GaussianTemplate9HPass.frag"), gaussianBlurKernelWeightMod),
        g_pszShaderAttributeArray);
      m_programBlurVPass.Reset(
        m_strShaderVertPass,
        GaussianShaderBuilder::Build9x9(contentManager->ReadAllText("Shaders/GaussianTemplate9VPass.frag"), gaussianBlurKernelWeightMod),
        g_pszShaderAttributeArray);
      break;
    case Bloom::Gaussian5X5:
    default:
      m_programBlurHPass.Reset(
        m_strShaderVertPass,
        GaussianShaderBuilder::Build5x5(contentManager->ReadAllText("Shaders/GaussianTemplate5HPass.frag"), gaussianBlurKernelWeightMod),
        g_pszShaderAttributeArray);
      m_programBlurVPass.Reset(
        m_strShaderVertPass,
        GaussianShaderBuilder::Build5x5(contentManager->ReadAllText("Shaders/GaussianTemplate5VPass.frag"), gaussianBlurKernelWeightMod),
        g_pszShaderAttributeArray);
      break;
    }

    m_locBlurHTexSize = glGetUniformLocation(m_programBlurHPass.Get(), "TexSize");
    m_locBlurVTexSize = glGetUniformLocation(m_programBlurVPass.Get(), "TexSize");
    m_activeBlueShaderType = shaderType;
  }
}
