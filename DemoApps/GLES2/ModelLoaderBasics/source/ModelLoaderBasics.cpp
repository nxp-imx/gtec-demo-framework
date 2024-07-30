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

#include "ModelLoaderBasics.hpp"
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
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>

namespace Fsl
{
  using namespace GLES2;
  using namespace Graphics3D;

  using TestMesh = GenericMesh<VertexPositionColorNormalTexture, uint16_t>;
  using TestScene = GenericScene<TestMesh>;

  namespace
  {
    constexpr float DefaultZoom = 10;
    constexpr float DefaultModelScale = 5;
  }

  ModelLoaderBasics::ModelLoaderBasics(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_rotationSpeed(0.0f, -0.6f, 0.0f)
    , m_fragUboData(Vector3(0.0f, 0.0f, 1.0f), Vector3(0.8f, 0.8f, 0.8f), Vector3(0.2f, 0.2f, 0.2f))
  {
    m_fragUboData.LightDirection.Normalize();
    auto contentManger = GetContentManager();
    auto contentPath = contentManger->GetContentPath();

    // Load the texture
    {
      Bitmap bitmap;
      contentManger->Read(bitmap, "Models/Knight2/armor_default_color.jpg", PixelFormat::R8G8B8_UNORM);
      GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_resources.Texture.Reset(bitmap, texParams, TextureFlags::GenerateMipMaps);
    }

    // Load the scene
    // Default is Fast
    // aiProcessPreset_TargetRealtime_Fast
    // aiProcessPreset_TargetRealtime_Quality
    // aiProcessPreset_TargetRealtime_MaxQuality
    auto modelPath = IO::Path::Combine(contentPath, "Models/Knight2/armor.obj");
    SceneImporter sceneImporter;
    const std::shared_ptr<TestScene> scene = sceneImporter.Load<TestScene>(modelPath, DefaultModelScale, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException("Scene did not contain any meshes");
    }

    auto rootNode = scene->GetRootNode();
    if (!rootNode)
    {
      throw NotSupportedException("Scene did not contain a root node");
    }

    // Create index and vertex buffers for all the meshes.
    {
      m_resources.IndexBuffers.Resize(scene->Meshes.size(), GL_UNSIGNED_SHORT);
      m_resources.VertexBuffers.Resize(scene->Meshes.size(), TestMesh::vertex_type::AsVertexDeclarationSpan());
      std::size_t vertexCount = 0;
      std::size_t indexCount = 0;
      for (std::size_t i = 0; i < scene->Meshes.size(); ++i)
      {
        auto mesh = scene->Meshes[i];
        m_resources.IndexBuffers.Reset(i, mesh->GetIndexArray(), GL_STATIC_DRAW);
        m_resources.VertexBuffers.Reset(i, mesh->GetVertexArray(), GL_STATIC_DRAW);

        vertexCount += mesh->GetVertexCount();
        indexCount += mesh->GetIndexCount();
      }
      FSLLOG3_INFO("Total vertex count: {}, Total index count: {}", vertexCount, indexCount);
    }


    {    // Prepare shaders
      m_resources.Program.Reset(contentManger->ReadAllText("BasicShaderDLight.vert"), contentManger->ReadAllText("BasicShaderDLightTextured.frag"));
      m_resources.LocWorldView = m_resources.Program.TryGetUniformLocation("WorldView");
      m_resources.LocWorldViewProjection = m_resources.Program.GetUniformLocation("WorldViewProjection");
      m_resources.LocNormalMatrix = m_resources.Program.GetUniformLocation("NormalMatrix");
      m_resources.LocTexture0 = m_resources.Program.GetUniformLocation("Texture0");
      m_resources.LocLightDirection = m_resources.Program.GetUniformLocation("LightDirection");
      m_resources.LocLightColor = m_resources.Program.GetUniformLocation("LightColor");
      m_resources.LocAmbientColor = m_resources.Program.GetUniformLocation("AmbientColor");

      constexpr auto VertexDecl = TestMesh::vertex_type::GetVertexDeclarationArray();
      m_resources.AttribLink[0] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexPosition"),
                                                     VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
      m_resources.AttribLink[1] =
        GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexColor"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
      m_resources.AttribLink[2] =
        GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexNormal"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
      m_resources.AttribLink[3] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexTexCoord"),
                                                     VertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    }

    GL_CHECK_FOR_ERROR();
  }


  ModelLoaderBasics::~ModelLoaderBasics() = default;


  void ModelLoaderBasics::Update(const DemoTime& demoTime)
  {
    const auto aspectRatio = GetWindowAspectRatio();

    m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
    m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
    m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
    m_matrixWorld = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
    m_matrixView = Matrix::CreateTranslation(0, 0, -DefaultZoom);
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectRatio, 1, 1000.0f);

    // Update Vertex UBO
    m_vertexUboData.MatWorldView = m_matrixWorld * m_matrixView;
    m_vertexUboData.MatWorldViewProjection = m_vertexUboData.MatWorldView * m_matrixProjection;
    m_vertexUboData.MatNormal = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_vertexUboData.MatWorldView)));
  }


  void ModelLoaderBasics::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(m_resources.Program.Get());

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_resources.Texture.Get());

      glUniform3fv(m_resources.LocLightDirection, 1, m_fragUboData.LightDirection.DirectAccess());
      glUniform3fv(m_resources.LocLightColor, 1, m_fragUboData.LightColor.DirectAccess());
      glUniform3fv(m_resources.LocAmbientColor, 1, m_fragUboData.AmbientColor.DirectAccess());


      // Enable the attribs the meshes use once (since we use the same mesh layout for everything)
      for (const auto& attribLink : m_resources.AttribLink)
      {
        if (attribLink.AttribIndex >= 0)
        {
          glEnableVertexAttribArray(attribLink.AttribIndex);
        }
      }

      DrawMeshes();

      // GL_CHECK_FOR_ERROR();

      // Disable everything
      glBindBuffer(m_resources.IndexBuffers.GetTarget(), 0);
      glBindBuffer(m_resources.VertexBuffers.GetTarget(), 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glUseProgram(0);
    }
  }


  void ModelLoaderBasics::DrawMeshes()
  {
    // Load the matrices
    glUniformMatrix4fv(m_resources.LocWorldView, 1, 0, m_vertexUboData.MatWorldView.DirectAccess());
    glUniformMatrix4fv(m_resources.LocWorldViewProjection, 1, 0, m_vertexUboData.MatWorldViewProjection.DirectAccess());
    glUniformMatrix3fv(m_resources.LocNormalMatrix, 1, 0, m_vertexUboData.MatNormal.DirectAccess());

    const auto indexBufferType = m_resources.IndexBuffers.GetType();
    for (int32_t i = 0; i < m_resources.IndexBuffers.Length(); ++i)
    {
      auto indexBuffer = m_resources.IndexBuffers.Get(i);
      auto vertexBuffer = m_resources.VertexBuffers.Get(i);
      if (indexBuffer.GetCapacity() > 0)
      {
        // Bind and enable the vertex buffer
        glBindBuffer(m_resources.VertexBuffers.GetTarget(), vertexBuffer.Get());
        glBindBuffer(m_resources.IndexBuffers.GetTarget(), indexBuffer.Get());

        // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
        m_resources.VertexBuffers.SetVertexAttribPointers(m_resources.AttribLink);

        glDrawElements(GL_TRIANGLES, indexBuffer.GetGLCapacity(), indexBufferType, nullptr);
      }
    }
  }
}
