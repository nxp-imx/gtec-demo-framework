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
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>

#include <FslBase/Math/MathHelper.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>

#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslBase/Math/MatrixConverter.hpp>

#include <GLES2/gl2.h>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>

#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTangentTexture.hpp>

namespace Fsl
{
  using namespace GLES2;
  using namespace Graphics3D;

  using TestMesh = GenericMesh<VertexPositionColorNormalTexture, uint16_t>;
  using TestScene = GenericScene<TestMesh>;

  namespace
  {
    const float DEFAULT_ZOOM = 10;
    const float DEFAULT_MODEL_SCALE = 5;
  }

  ModelLoaderBasics::ModelLoaderBasics(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_locWorldView(GLValues::INVALID_LOCATION)
    , m_locWorldViewProjection(GLValues::INVALID_LOCATION)
    , m_locNormalMatrix(GLValues::INVALID_LOCATION)
    , m_locTexture0(GLValues::INVALID_LOCATION)
    , m_locLightDirection(GLValues::INVALID_LOCATION)
    , m_locLightColor(GLValues::INVALID_LOCATION)
    , m_locAmbientColor(GLValues::INVALID_LOCATION)
    , m_attribLink(4)
    , m_rotationSpeed(0.0f, -0.6f, 0.0f)
    , m_lightDirection(0.0f, 0.0f, 1.0f)
    , m_lightColor(0.8f, 0.8f, 0.8f)
    , m_ambientColor(0.2f, 0.2f, 0.2f)
  {
    m_lightDirection.Normalize();
    auto contentManger = GetContentManager();
    auto contentPath = contentManger->GetContentPath();

    // Load the texture
    {
      Bitmap bitmap;
      contentManger->Read(bitmap, "Knight2/armor_default_color.jpg", PixelFormat::R8G8B8_UNORM);
      GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_texture.SetData(bitmap, texParams, TextureFlags::GenerateMipMaps);
    }

    // Load the scene
    // Default is Fast
    // aiProcessPreset_TargetRealtime_Fast
    // aiProcessPreset_TargetRealtime_Quality
    // aiProcessPreset_TargetRealtime_MaxQuality
    auto modelPath = IO::Path::Combine(contentPath, "Knight2/armor.obj");
    SceneImporter sceneImporter;
    const std::shared_ptr<TestScene> scene = sceneImporter.Load<TestScene>(modelPath, DEFAULT_MODEL_SCALE, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException("Scene did not contain any meshes");
    }

    m_rootNode = scene->GetRootNode();
    if (!m_rootNode)
    {
      throw NotSupportedException("Scene did not contain a root node");
    }


    {    // Prepare shaders
      m_program.Reset(contentManger->ReadAllText("BasicShaderDLight.vert"), contentManger->ReadAllText("BasicShaderDLightTextured.frag"));

      m_locWorldView = glGetUniformLocation(m_program.Get(), "WorldView");
      m_locWorldViewProjection = glGetUniformLocation(m_program.Get(), "WorldViewProjection");
      m_locNormalMatrix = glGetUniformLocation(m_program.Get(), "NormalMatrix");
      m_locTexture0 = glGetUniformLocation(m_program.Get(), "Texture0");
      m_locLightDirection = glGetUniformLocation(m_program.Get(), "LightDirection");
      m_locLightColor = glGetUniformLocation(m_program.Get(), "LightColor");
      m_locAmbientColor = glGetUniformLocation(m_program.Get(), "AmbientColor");

      auto vertexDecl = TestMesh::vertex_type::GetVertexDeclaration();
      m_attribLink[0] = GLVertexAttribLink(glGetAttribLocation(m_program.Get(), "VertexPosition"),
                                           vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
      m_attribLink[1] =
        GLVertexAttribLink(glGetAttribLocation(m_program.Get(), "VertexColor"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
      m_attribLink[2] =
        GLVertexAttribLink(glGetAttribLocation(m_program.Get(), "VertexNormal"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
      m_attribLink[3] = GLVertexAttribLink(glGetAttribLocation(m_program.Get(), "VertexTexCoord"),
                                           vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    }

    // Create index and vertex buffers for all the meshes.
    {
      m_indexBuffers.Resize(scene->Meshes.size(), GL_UNSIGNED_SHORT);
      m_vertexBuffers.Resize(scene->Meshes.size(), TestMesh::vertex_type::GetVertexDeclaration());
      std::size_t vertexCount = 0;
      std::size_t indexCount = 0;
      for (std::size_t i = 0; i < scene->Meshes.size(); ++i)
      {
        auto mesh = scene->Meshes[i];
        m_indexBuffers.Reset(i, mesh->GetIndexArray(), GL_STATIC_DRAW);
        m_vertexBuffers.Reset(i, mesh->GetVertexArray(), GL_STATIC_DRAW);

        vertexCount += mesh->GetVertexCount();
        indexCount += mesh->GetIndexCount();
      }
      FSLLOG("Total vertex count: " << vertexCount << ", Total index count : " << indexCount);
    }
    GL_CHECK_FOR_ERROR();
  }


  ModelLoaderBasics::~ModelLoaderBasics() = default;


  void ModelLoaderBasics::Update(const DemoTime& demoTime)
  {
    const Point2 screenResolution = GetScreenResolution();

    m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
    m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
    m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
    m_matrixWorld = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
    m_matrixView = Matrix::CreateTranslation(0, 0, -DEFAULT_ZOOM);
    m_matrixProjection =
      Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), screenResolution.X / static_cast<float>(screenResolution.Y), 1, 1000.0f);
    m_matrixWorldView = m_matrixWorld * m_matrixView;
    m_matrixWorldViewProjection = m_matrixWorldView * m_matrixProjection;
    m_matrixNormal = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_matrixWorldView)));
  }


  void ModelLoaderBasics::Draw(const DemoTime& demoTime)
  {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(m_program.Get());

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_texture.Get());

      glUniform3fv(m_locLightDirection, 1, m_lightDirection.DirectAccess());
      glUniform3fv(m_locLightColor, 1, m_lightColor.DirectAccess());
      glUniform3fv(m_locAmbientColor, 1, m_ambientColor.DirectAccess());


      // Enable the attribs the meshes use once (since we use the same mesh layout for everything)
      for (std::size_t i = 0; i < m_attribLink.size(); ++i)
      {
        if (m_attribLink[i].AttribIndex >= 0)
        {
          glEnableVertexAttribArray(m_attribLink[i].AttribIndex);
        }
      }

      DrawMeshes();

      // GL_CHECK_FOR_ERROR();

      // Disable everything
      glBindBuffer(m_indexBuffers.GetTarget(), 0);
      glBindBuffer(m_vertexBuffers.GetTarget(), 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glUseProgram(0);
    }
  }


  void ModelLoaderBasics::DrawMeshes()
  {
    // Load the matrices
    glUniformMatrix4fv(m_locWorldView, 1, 0, m_matrixWorldView.DirectAccess());
    glUniformMatrix4fv(m_locWorldViewProjection, 1, 0, m_matrixWorldViewProjection.DirectAccess());
    glUniformMatrix3fv(m_locNormalMatrix, 1, 0, m_matrixNormal.DirectAccess());

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
        m_vertexBuffers.SetVertexAttribPointers(m_attribLink.data(), m_attribLink.size());

        glDrawElements(GL_TRIANGLES, indexBuffer.GetCapacity(), indexBufferType, nullptr);
      }
    }
  }
}
