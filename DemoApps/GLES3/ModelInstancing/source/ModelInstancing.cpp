/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include "ModelInstancing.hpp"
#include <FslAssimp/SceneImporter.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/ModelInstancing/MeshUtil.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  using namespace GLES3;
  namespace
  {
    namespace LocalConfig
    {
      constexpr float DefaultModelScale = 1.0;
    }
  }

  ModelInstancing::ModelInstancing(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_fragUboData(Vector3(0.0f, 0.0f, 1.0f), Vector3(0.8f, 0.8f, 0.8f), Vector3(0.2f, 0.2f, 0.2f))
    , m_shared(config)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_shared.GetUIDemoAppExtension());

    m_fragUboData.LightDirection.Normalize();
    auto contentManger = GetContentManager();
    auto contentPath = contentManger->GetContentPath();

    // Load the texture
    FSLLOG3_INFO("Loading texture");
    {
      Bitmap bitmap;
      contentManger->Read(bitmap, "Models/Knight2/armor_default_color.jpg", PixelFormat::R8G8B8_UNORM);
      GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_resources.Texture.Reset(bitmap, texParams, TextureFlags::GenerateMipMaps);
    }

    FSLLOG3_INFO("Loading scene");
    // Load the scene
    // Default is Fast
    // aiProcessPreset_TargetRealtime_Fast
    // aiProcessPreset_TargetRealtime_Quality
    // aiProcessPreset_TargetRealtime_MaxQuality
    auto modelPath = IO::Path::Combine(contentPath, "Models/Knight2/armor.obj");
    SceneImporter sceneImporter;
    const std::shared_ptr<MeshUtil::DemoScene> scene = sceneImporter.Load<MeshUtil::DemoScene>(modelPath, LocalConfig::DefaultModelScale, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException("Scene did not contain any meshes");
    }

    auto rootNode = scene->GetRootNode();
    if (!rootNode)
    {
      throw NotSupportedException("Scene did not contain a root node");
    }

    FSLLOG3_INFO("Preparing");

    // Create index and vertex buffers
    {
      MeshUtil::DemoMeshRecord meshRecord = MeshUtil::ToSingleMesh(*scene);
      m_shared.SetStats(ModelRenderStats(NumericCast<uint32_t>(meshRecord.Vertices.size()), NumericCast<uint32_t>(meshRecord.Indices.size())));

      std::size_t vertexCount = 0;
      std::size_t indexCount = 0;
      for (std::size_t i = 0; i < scene->Meshes.size(); ++i)
      {
        auto mesh = scene->Meshes[i];
        m_resources.IndexBuffer.Reset(meshRecord.Indices, GL_STATIC_DRAW);
        m_resources.VertexBuffer.Reset(meshRecord.Vertices, GL_STATIC_DRAW);

        vertexCount += mesh->GetVertexCount();
        indexCount += mesh->GetIndexCount();
      }
      FSLLOG3_INFO("Total vertex count: {}, Total index count: {}", vertexCount, indexCount);
      m_resources.InstanceBuffer.Reset(GL_ARRAY_BUFFER, MeshInstancingConfig::ModelMaxInstances, sizeof(MeshInstanceData), GL_DYNAMIC_DRAW);
    }


    {    // Prepare shaders
      m_resources.Program.Reset(contentManger->ReadAllText("BasicShaderDLight.vert"), contentManger->ReadAllText("BasicShaderDLightTextured.frag"));
      m_resources.LocView = m_resources.Program.GetUniformLocation("View");
      m_resources.LocProjection = m_resources.Program.GetUniformLocation("Projection");
      m_resources.LocTexture0 = m_resources.Program.GetUniformLocation("Texture0");
      m_resources.LocLightDirection = m_resources.Program.GetUniformLocation("LightDirection");
      m_resources.LocLightColor = m_resources.Program.GetUniformLocation("LightColor");
      m_resources.LocAmbientColor = m_resources.Program.GetUniformLocation("AmbientColor");

      constexpr auto vertexDecl = MeshUtil::DemoMesh::vertex_type::GetVertexDeclarationArray();
      m_resources.AttribLink[0] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexPosition"),
                                                     vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
      m_resources.AttribLink[1] =
        GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexColor"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
      m_resources.AttribLink[2] =
        GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexNormal"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
      m_resources.AttribLink[3] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexTexCoord"),
                                                     vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
      // Instanced data
      m_resources.InstanceAttribLink[0] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("InstanceModelMatrix"), 0);
    }

    GL_CHECK_FOR_ERROR();
    FSLLOG3_INFO("Ready");
  }


  ModelInstancing::~ModelInstancing() = default;

  void ModelInstancing::OnKeyEvent(const KeyEvent& event)
  {
    DemoAppGLES3::OnKeyEvent(event);

    m_shared.OnKeyEvent(event);
  }


  void ModelInstancing::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    DemoAppGLES3::ConfigurationChanged(windowMetrics);

    m_shared.OnConfigurationChanged(windowMetrics);
  }


  void ModelInstancing::Update(const DemoTime& demoTime)
  {
    m_shared.Update(demoTime);

    auto matrixInfo = m_shared.GetMatrixInfo();

    // Update Vertex
    m_vertexUboData.MatView = matrixInfo.Model * matrixInfo.View;
    m_vertexUboData.MatProjection = matrixInfo.Proj;
  }


  void ModelInstancing::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    auto span = m_shared.GetInstanceSpan();
    m_resources.InstanceBuffer.SetData(0, span);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(m_resources.Program.Get());

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_resources.Texture.Get());

      glUniform3fv(m_resources.LocLightDirection, 1, m_fragUboData.LightDirection.DirectAccess());
      glUniform3fv(m_resources.LocLightColor, 1, m_fragUboData.LightColor.DirectAccess());
      glUniform3fv(m_resources.LocAmbientColor, 1, m_fragUboData.AmbientColor.DirectAccess());

      if (!span.empty())
      {
        DrawMeshes(UncheckedNumericCast<uint32_t>(span.size()));
      }

      // GL_CHECK_FOR_ERROR();

      // Disable everything
      glBindBuffer(m_resources.IndexBuffer.GetTarget(), 0);
      glBindBuffer(m_resources.VertexBuffer.GetTarget(), 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glUseProgram(0);

      m_shared.Draw();
    }
  }


  void ModelInstancing::DrawMeshes(const uint32_t instanceCount)
  {
    // Load the matrices
    glUniformMatrix4fv(m_resources.LocView, 1, 0, m_vertexUboData.MatView.DirectAccess());
    glUniformMatrix4fv(m_resources.LocProjection, 1, 0, m_vertexUboData.MatProjection.DirectAccess());

    glEnableVertexAttribArray(m_resources.InstanceAttribLink[0].AttribIndex);
    glEnableVertexAttribArray(m_resources.InstanceAttribLink[0].AttribIndex + 1);
    glEnableVertexAttribArray(m_resources.InstanceAttribLink[0].AttribIndex + 2);
    glEnableVertexAttribArray(m_resources.InstanceAttribLink[0].AttribIndex + 3);

    const auto indexBufferType = m_resources.IndexBuffer.GetType();
    if (m_resources.IndexBuffer.GetCapacity() > 0)
    {
      // Bind and enable the vertex buffer
      glBindBuffer(m_resources.VertexBuffer.GetTarget(), m_resources.VertexBuffer.Get());
      glBindBuffer(m_resources.IndexBuffer.GetTarget(), m_resources.IndexBuffer.Get());

      // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
      m_resources.VertexBuffer.EnableAttribArrays(m_resources.AttribLink);

      // glBindBuffer(m_resources.InstanceBuffer.GetTarget(), m_resources.InstanceBuffer.Get());
      glBindBuffer(GL_ARRAY_BUFFER, m_resources.InstanceBuffer.Get());

      constexpr auto matrixStride = sizeof(MeshInstanceData);
      glVertexAttribPointer(m_resources.InstanceAttribLink[0].AttribIndex + 0, 4, GL_FLOAT, GL_FALSE, matrixStride,
                            reinterpret_cast<GLvoid*>(0 * 4 * 4));
      glVertexAttribPointer(m_resources.InstanceAttribLink[0].AttribIndex + 1, 4, GL_FLOAT, GL_FALSE, matrixStride,
                            reinterpret_cast<GLvoid*>(1 * 4 * 4));
      glVertexAttribPointer(m_resources.InstanceAttribLink[0].AttribIndex + 2, 4, GL_FLOAT, GL_FALSE, matrixStride,
                            reinterpret_cast<GLvoid*>(2 * 4 * 4));
      glVertexAttribPointer(m_resources.InstanceAttribLink[0].AttribIndex + 3, 4, GL_FLOAT, GL_FALSE, matrixStride,
                            reinterpret_cast<GLvoid*>(3 * 4 * 4));

      // Turn on instancing by setting the attribute to instanced mode
      glVertexAttribDivisor(m_resources.InstanceAttribLink[0].AttribIndex + 0, 1);
      glVertexAttribDivisor(m_resources.InstanceAttribLink[0].AttribIndex + 1, 1);
      glVertexAttribDivisor(m_resources.InstanceAttribLink[0].AttribIndex + 2, 1);
      glVertexAttribDivisor(m_resources.InstanceAttribLink[0].AttribIndex + 3, 1);

      glDrawElementsInstanced(GL_TRIANGLES, m_resources.IndexBuffer.GetGLCapacity(), indexBufferType, nullptr,
                              UncheckedNumericCast<GLsizei>(instanceCount));
    }
  }
}
