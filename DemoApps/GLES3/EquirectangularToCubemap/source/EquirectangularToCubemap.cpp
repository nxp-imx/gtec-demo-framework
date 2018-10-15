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

#include "EquirectangularToCubemap.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLFrameBuffer.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include "RenderCube.hpp"
#include "OptionParser.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    const Vector3 DEFAULT_CAMERA_POSITION(0.0f, 0.0f, 0.0f);
    const Vector3 DEFAULT_CAMERA_TARGET(0.0f, 0.0f, -4.0f);

    struct ProgramLocations
    {
      GLint ViewMatrix;
      GLint ProjMatrix;
      GLint EquirectangularMap;
      ProgramLocations()
        : ViewMatrix(GLES3::GLValues::INVALID_LOCATION)
        , ProjMatrix(GLES3::GLValues::INVALID_LOCATION)
        , EquirectangularMap(GLES3::GLValues::INVALID_LOCATION)
      {
      }
    };

    struct ProgramInfo
    {
      GLES3::GLProgram Program;
      ProgramLocations Location;
    };

    ProgramInfo CreateShader(const std::shared_ptr<IContentManager>& contentManager)
    {
      const std::string vert = contentManager->ReadAllText("EquirectangularToCubemap.vert");
      const std::string frag = contentManager->ReadAllText("EquirectangularToCubemap.frag");

      ProgramInfo info;
      info.Program.Reset(vert, frag);

      info.Location.ViewMatrix = info.Program.GetUniformLocation("g_view");
      info.Location.ProjMatrix = info.Program.GetUniformLocation("g_projection");
      info.Location.EquirectangularMap = info.Program.GetUniformLocation("g_equirectangularMap");
      return info;
    }

    GLES3::GLFrameBuffer CreateFrameBuffer(const Point2 resolution, const GLTextureImageParameters& texImageParams)
    {
      GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      return GLFrameBuffer(resolution, params, texImageParams, GL_DEPTH_COMPONENT24);
    }


    std::array<glm::mat4, 6> GenerateCubemapViewpoints()
    {
      std::array<glm::mat4, 6> captureViews = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),     // POS-X
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),    // NEG-X
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),      // POS-Y
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),    // NEG-Y
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),     // POS-Z
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))     // NEG-Z
      };
      return captureViews;
    }


    GLTexture GenerateCubemap(const std::shared_ptr<IContentManager>& contentManager, const IO::Path& texturePath, const Point2& resolution,
                              const bool hdr)
    {
      auto tex = contentManager->ReadTexture(texturePath);
      GLTextureImageParameters texImageParams;
      GLTextureParameters texParams;
      if (hdr)
      {
        texImageParams = GLTextureImageParameters(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
        texParams = GLTextureParameters(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      }
      else
      {
        texImageParams = GLTextureImageParameters(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
        texParams = GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      }

      GLTexture texture(tex, texParams);
      auto programInfo = CreateShader(contentManager);
      auto framebuffer = CreateFrameBuffer(resolution, texImageParams);
      auto viewpoints = GenerateCubemapViewpoints();
      glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

      GLuint envCubemap;
      glGenTextures(1, &envCubemap);
      GL_CHECK_FOR_ERROR();
      glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
      GL_CHECK_FOR_ERROR();
      for (unsigned int i = 0; i < 6; ++i)
      {
        // GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z

        // With EXT_color_buffer_float the format GL_RGBA16F is supported as Texture and renderbuffer color formats.
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texImageParams.InternalFormat, resolution.X, resolution.Y, 0, texImageParams.Format,
                     texImageParams.Type, nullptr);
        GL_CHECK_FOR_ERROR();
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      GL_CHECK_FOR_ERROR();

      glViewport(0, 0, resolution.X, resolution.Y);
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);

      glUseProgram(programInfo.Program.Get());
      glUniform1i(programInfo.Location.EquirectangularMap, 0);
      glUniformMatrix4fv(programInfo.Location.ProjMatrix, 1, 0, glm::value_ptr(captureProjection));

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture.Get());

      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.Get());

      RenderCube renderCube(programInfo.Program);
      renderCube.BeginDraw();
      for (uint32_t i = 0; i < viewpoints.size(); ++i)
      {
        GL_CHECK_FOR_ERROR();
        glUniformMatrix4fv(programInfo.Location.ViewMatrix, 1, 0, glm::value_ptr(viewpoints[i]));
        GL_CHECK_FOR_ERROR();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        GL_CHECK_FOR_ERROR();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK_FOR_ERROR();
        renderCube.DrawCube();
        // DrawCubeNow();
        GL_CHECK_FOR_ERROR();
      }
      renderCube.EndDraw();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glUseProgram(0);

      GL_CHECK_FOR_ERROR();
      return GLTexture(envCubemap, resolution, GL_TEXTURE_CUBE_MAP);
    }
  }


  EquirectangularToCubemap::EquirectangularToCubemap(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_matrixProjection(Matrix::GetIdentity())
    , m_rotationSpeed(5.4f, 0.0f)
  {
    const auto options = config.GetOptions<OptionParser>();

    m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());
    const auto contentManager = GetContentManager();

    const bool hdrEnabled = options->GetScene() == SceneState::Scene2;
    if (hdrEnabled && !GLUtil::HasExtension("GL_EXT_color_buffer_float"))
    {
      throw GraphicsException("This scene requires support for 'GL_EXT_color_buffer_float'");
    }

    IO::Path texturePath(hdrEnabled ? "floral_tent/floral_tent_1k.hdr" : "Stairs/equirectangular.jpg");

    Point2 resolution(2048, 2048);
    m_cubemapTexture = GenerateCubemap(contentManager, texturePath, resolution, hdrEnabled);

    std::string texture = "Stairs";

    m_skyboxProgram.Reset(contentManager->ReadAllText("skybox.vert"), contentManager->ReadAllText("skybox.frag"));
    m_skyboxMesh.Reset(m_skyboxProgram.Program);

    const auto screenResolution = GetScreenResolution();
    float aspect = static_cast<float>(screenResolution.X) / screenResolution.Y;
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 1000.0f);

    GL_CHECK_FOR_ERROR();
  }


  EquirectangularToCubemap::~EquirectangularToCubemap() = default;


  void EquirectangularToCubemap::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
    {
      const bool mouseCapture = event.IsPressed();
      if (m_demoAppControl->TryEnableMouseCaptureMode(mouseCapture))
      {
        m_mouseCaptureEnabled = mouseCapture;
      }
      else
      {
        m_mouseCaptureEnabled = false;
      }
      event.Handled();
      break;
    }
    case VirtualMouseButton::Middle:
      if (event.IsPressed())
      {
        m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void EquirectangularToCubemap::Update(const DemoTime& demoTime)
  {
    if (!m_mouseCaptureEnabled)
    {
      m_camera.RotateByRadians(m_rotationSpeed * demoTime.DeltaTime);
    }
    UpdateCameraControlInput(demoTime, m_keyboard->GetState());
  }


  void EquirectangularToCubemap::Draw(const DemoTime& demoTime)
  {
    const auto resolution = GetScreenResolution();
    glViewport(0, 0, resolution.X, resolution.Y);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawScene(m_skyboxProgram, m_skyboxMesh, m_cubemapTexture, m_camera.GetViewMatrix(), m_matrixProjection);
  }


  void EquirectangularToCubemap::DrawScene(const SkyboxProgram& programInfo, const SkyboxMesh& mesh, const GLES3::GLTexture& texture,
                                           const Matrix& matrixView, const Matrix& matrixProjection)
  {
    const auto& program = programInfo.Program;
    const auto& location = programInfo.Location;

    // Set the shader program
    glUseProgram(program.Get());

    // Load the matrices
    assert(location.ViewMatrix != GLValues::INVALID_LOCATION);
    assert(location.ProjMatrix != GLValues::INVALID_LOCATION);
    assert(location.SkyboxSampler != GLValues::INVALID_LOCATION);

    glUniformMatrix4fv(location.ViewMatrix, 1, 0, matrixView.DirectAccess());
    glUniformMatrix4fv(location.ProjMatrix, 1, 0, matrixProjection.DirectAccess());

    // Bind the vertex array
    mesh.VertexArray.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.Get());
    glDrawArrays(GL_TRIANGLES, 0, mesh.VertexBuffer.GetCapacity());
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    mesh.VertexArray.Unbind();

    glUseProgram(0);
  }


  void EquirectangularToCubemap::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      if (!m_mouseCaptureEnabled)
      {
        const bool rotateCamera = mouseState.IsRightButtonPressed();
        m_camera.RotateViaPosition(rotateCamera, mouseState.Position);
      }
      else
      {
        if (mouseState.IsRightButtonPressed())
        {
          const auto rawPosition = Vector2(mouseState.RawPosition.X, -mouseState.RawPosition.Y);
          m_camera.Rotate(rawPosition);
        }
      }
    }

    // Keyboard camera movement
    const float movementSpeed = 2.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::W))
    {
      m_camera.MoveForward(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::S))
    {
      m_camera.MoveBackwards(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::A))
    {
      m_camera.MoveLeft(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::D))
    {
      m_camera.MoveRight(movementSpeed);
    }
  }
}
