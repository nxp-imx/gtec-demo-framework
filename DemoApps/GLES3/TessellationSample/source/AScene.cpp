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

#include "AScene.hpp"
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslDemoApp/Service/Events/Basic/MouseWheelEvent.hpp>
#include <FslDemoApp/Service/Content/IContentManager.hpp>
#include <cassert>
#include <limits>

// Because of inconsistency in khronos extension definition both the 31 and 2 headers are needed
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>

#include "OptionParser.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    const float DEFAULT_ZOOM = 350;
    const float DEFAULT_MODEL_SCALE = 5;
  }

  AScene::AScene(const DemoAppConfig& config, const std::shared_ptr<OptionParser>& options)
    : m_basicRenderingEnabled(false)
    , m_cullEnabled(false)
    , m_screenResolution(config.ScreenResolution)
    , m_fieldOfView(MathHelper::ToRadians(options->GetFieldOfView()))
    , m_camera(config.ScreenResolution)
    , m_tessellationConfig()
    , m_cameraConfig()
    , m_material()
    , m_basicShader(config.DemoServiceProvider.Get<IContentManager>(), vertex_type::GetVertexDeclaration())
    , m_tessShader(config.DemoServiceProvider.Get<IContentManager>(), vertex_type::GetVertexDeclaration())
    , m_indexBuffers()
    , m_vertexBuffers()
    , m_textureDiffuse()
    , m_textureDiffuseWhite()
    , m_textureDisplacement()
    , m_textureNoDisplacement()
    , m_lightDirection(0.2f, 0.4f, 1.0f)
    , m_lightColor(0.8f, 0.8f, 0.8f)
    , m_rotationSpeed(0.0f, -0.6f, 0.0f)
    , m_rotation()
  {
    m_camera.SetZoom(DEFAULT_ZOOM);
    m_lightDirection.Normalize();
  }


  void AScene::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsPressed())
      return;

    switch (event.GetKey())
    {
    case VirtualKey::E:
      event.Handled();
      m_rotation = Vector3::Zero();
      break;
    case VirtualKey::T:
      event.Handled();
      m_basicRenderingEnabled = !m_basicRenderingEnabled;
      break;
    default:
      break;
    }
  }

  void AScene::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    switch (event.GetButton())
    {
    case VirtualMouseButton::Left:
    {
      if (event.IsPressed())
        m_camera.BeginDrag(event.GetPosition());
      else if (m_camera.IsDragging())
        m_camera.EndDrag(event.GetPosition());
      event.Handled();
    }
    break;
    case VirtualMouseButton::Right:
      if (event.IsPressed())
      {
        m_camera.ResetRotation();
        m_camera.SetZoom(DEFAULT_ZOOM);
        m_rotation = Vector3();
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void AScene::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (m_camera.IsDragging())
    {
      m_camera.Drag(event.GetPosition());
      event.Handled();
    }
  }


  void AScene::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    m_camera.AddZoom(event.GetDelta() * -0.1f);
  }


  void AScene::Update(const DemoTime& demoTime, const RenderDrawConfig& config)
  {
    m_cameraConfig.World = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
    //m_cameraConfig.View = Matrix::CreateTranslation(0.0f, 0.0f, -2.0f);
    m_cameraConfig.View = m_camera.GetViewMatrix();
    m_cameraConfig.Projection = Matrix::CreatePerspectiveFieldOfView(m_fieldOfView, m_screenResolution.X / (float)m_screenResolution.Y, 1, 1000.0f);

    m_cameraConfig.WorldView = m_cameraConfig.World * m_cameraConfig.View;
    m_cameraConfig.WorldViewProjection = m_cameraConfig.WorldView * m_cameraConfig.Projection;

    Vector4 cameraSpaceLightDirection = Vector4::Transform(m_lightDirection, m_camera.GetRotationMatrix());
    m_cameraConfig.CameraSpaceLightDirection = Vector3(cameraSpaceLightDirection.X, cameraSpaceLightDirection.Y, cameraSpaceLightDirection.Z);
    m_cameraConfig.CameraSpaceLightDirection.Normalize();
    m_cameraConfig.NormalMatrix = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_cameraConfig.WorldView)));


    if (config.UseRotation)
    {
      m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
      m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
      m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
    }

    m_tessShader.UpdateUniforms(m_tessellationConfig, m_cameraConfig, m_material, 0, 1, 2);
  }


  void AScene::Draw(const RenderDrawConfig& config)
  {

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    if (m_cullEnabled)
      glEnable(GL_CULL_FACE);
    else
      glDisable(GL_CULL_FACE);

    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Do the actual tessellated draw
    if (!m_basicRenderingEnabled)
    {
      // Disable any active program
      glUseProgram(0);

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, config.UseTexture ? m_textureDiffuse.Get() : m_textureDiffuseWhite.Get());
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_textureNormals.Get());
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, config.UseDisplacement ? m_textureDisplacement.Get() : m_textureNoDisplacement.Get());

      // Bind the pipeline
      auto& rTess = m_tessShader;
      auto& rPipeline = rTess.Pipeline;

      rPipeline.Bind();
      // configure it
      rPipeline.UseProgramStages(rTess.ShaderVert, GL_VERTEX_SHADER_BIT);
      rPipeline.UseProgramStages(rTess.ShaderControl, GL_TESS_CONTROL_SHADER_BIT_EXT);
      rPipeline.UseProgramStages(rTess.ShaderEval, GL_TESS_EVALUATION_SHADER_BIT_EXT);
      rPipeline.UseProgramStages(rTess.ShaderGeomWireframe, GL_GEOMETRY_SHADER_BIT_EXT, config.UseWireframe);
      rPipeline.UseProgramStages(rTess.ShaderFrag, GL_FRAGMENT_SHADER_BIT);

      const auto indexBufferType = m_indexBuffers.GetType();
      for (int32_t i = 0; i < m_indexBuffers.Length(); ++i)
      {
        auto indexBuffer = m_indexBuffers.Get(i);
        auto vertexBuffer = m_vertexBuffers.Get(i);

        // Bind and enable the vertex buffer
        glBindBuffer(m_vertexBuffers.GetTarget(), vertexBuffer.Get());
        glBindBuffer(m_indexBuffers.GetTarget(), indexBuffer.Get());

        assert(rTess.AttribLink.size() <= std::numeric_limits<uint32_t>::max());

        m_vertexBuffers.EnableAttribArrays(rTess.AttribLink.data(), rTess.AttribLink.size());

        // Draw the patch
        glDrawElements(GL_PATCHES_EXT, indexBuffer.GetCapacity(), indexBufferType, (void*)0);

        m_vertexBuffers.DisableAttribArrays();
      }

      // Disable everything
      glBindBuffer(m_indexBuffers.GetTarget(), 0);
      glBindBuffer(m_vertexBuffers.GetTarget(), 0);
      rPipeline.BindClear();
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
      DrawBasic(config);
    }
  }


  void AScene::DrawBasic(const RenderDrawConfig& config)
  {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(m_basicShader.Program.Get());

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, config.UseTexture ? m_textureDiffuse.Get() : m_textureDiffuseWhite.Get());
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_textureNormals.Get());

      m_basicShader.UpdateUniforms(m_cameraConfig, m_material, 0, 1);

      // Enable the attribs the meshes use once (since we use the same mesh layout for everything)
      for (std::size_t i = 0; i < m_basicShader.AttribLink.size(); ++i)
      {
        if (m_basicShader.AttribLink[i].AttribIndex >= 0)
          glEnableVertexAttribArray(m_basicShader.AttribLink[i].AttribIndex);
      }

      DrawBasicMeshes();

      //GL_CHECK_FOR_ERROR();

      // Disable everything
      glBindBuffer(m_indexBuffers.GetTarget(), 0);
      glBindBuffer(m_vertexBuffers.GetTarget(), 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glUseProgram(0);
    }
  }


  void AScene::DrawBasicMeshes()
  {
    const auto indexBufferType = m_indexBuffers.GetType();
    for (int32_t i = 0; i < m_indexBuffers.Length(); ++i)
    {
      auto indexBuffer = m_indexBuffers.Get(i);
      auto vertexBuffer = m_vertexBuffers.Get(i);
      if (indexBuffer.GetCapacity() > 0)
      {
        // Bind and enable the vertex buffer
        glBindBuffer(m_vertexBuffers.GetTarget(), vertexBuffer.Get());
        glBindBuffer(m_indexBuffers.GetTarget(), indexBuffer.Get());

        // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
        m_vertexBuffers.SetVertexAttribPointers(m_basicShader.AttribLink.data(),  m_basicShader.AttribLink.size());

        glDrawElements(GL_TRIANGLES, indexBuffer.GetCapacity(), indexBufferType, (void*)0);
      }
    }
  }

}
