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

#include "RenderScene.hpp"
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslGraphics3D/SceneFormat/BasicSceneFormat.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTangentTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  using namespace GLES3;
  using namespace Graphics3D;
  using namespace SceneFormat;


  using BasicMesh = GenericMesh<VertexPositionNormalTangentTexture, uint16_t>;
  using BasicScene = GenericScene<BasicMesh>;

  namespace
  {
    const auto MODELS_PATH = "Models";
  }


  RenderScene::RenderScene(const DemoAppConfig& config, const int32_t sceneId)
    : m_locWorld(GLValues::INVALID_LOCATION)
    , m_locWorldView(GLValues::INVALID_LOCATION)
    , m_locWorldViewProjection(GLValues::INVALID_LOCATION)
    , m_locNormalMatrix(GLValues::INVALID_LOCATION)
    , m_locTexture0(GLValues::INVALID_LOCATION)
    , m_locTextureSpecular(GLValues::INVALID_LOCATION)
    , m_locTextureNormal(GLValues::INVALID_LOCATION)
    , m_locLightDirection(GLValues::INVALID_LOCATION)
    , m_locLightColor(GLValues::INVALID_LOCATION)
    , m_locMatAmbient(GLValues::INVALID_LOCATION)
    , m_locMatSpecular(GLValues::INVALID_LOCATION)
    , m_locMatShininess(GLValues::INVALID_LOCATION)
    , m_attribLink(4)
    , m_lightDirection(1.0f, 1.0f, 1.0f)
    , m_lightColor(0.8f, 0.8f, 0.8f)
    , m_matAmbient(0.2f, 0.2f, 0.2f, 0.2f)
    , m_matSpecular(1, 1, 1, 1)
    , m_matShininess(80.0f)
  {
    m_lightDirection.Normalize();

    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    IO::Path strFileName;
    IO::Path strTextureFileName;
    IO::Path strTextureGloss;
    IO::Path strTextureSpecular;
    IO::Path strTextureNormal;
    switch (sceneId)
    {
    case 1:
      strFileName = "Knight2/armor.fsf";
      strTextureFileName = "Knight2/armor_default_color.jpg";
      strTextureSpecular = "Knight2/armor_default_metalness.jpg";
      strTextureNormal = "Knight2/armor_default_nmap.jpg";
      strTextureGloss = "Knight2/armor_default_rough.jpg";
      break;
    case 0:
    case 2:
      strFileName = "FuturisticCar/FuturisticCar.fsf";
      strTextureFileName = "FuturisticCar/Futuristic_Car_C.jpg";
      strTextureSpecular = "FuturisticCar/Futuristic_Car_S.jpg";
      strTextureNormal = "FuturisticCar/Futuristic_Car_N.jpg";
      break;
    default:
      throw NotSupportedException("Scene not supported");
    }

    auto contentPath = contentManager->GetContentPath();
    contentPath = IO::Path::Combine(contentPath, MODELS_PATH);
    const auto fullModelPath = IO::Path::Combine(contentPath, strFileName);

    FSLLOG3_INFO("Loading scene '{}'", fullModelPath);
    BasicSceneFormat sceneFormat;
    auto scene = sceneFormat.Load<BasicScene>(fullModelPath);


    FSLLOG3_INFO("Preparing textures");
    {    // prepare textures
      Bitmap bitmap;
      auto texturePath = IO::Path::Combine(MODELS_PATH, strTextureFileName);

      if (strTextureGloss.IsEmpty())
      {
        FSLLOG3_INFO("- Diffuse '{}'", texturePath);
        contentManager->Read(bitmap, texturePath, PixelFormat::R8G8B8A8_UNORM);
      }
      else
      {
        Bitmap bitmapGloss;
        auto glossTexturePath = IO::Path::Combine(MODELS_PATH, strTextureGloss);
        FSLLOG3_INFO("- Diffuse '{}'", texturePath);
        contentManager->Read(bitmap, texturePath, PixelFormat::R8G8B8A8_UNORM);
        FSLLOG3_INFO("- Gloss '{}'", glossTexturePath);
        contentManager->Read(bitmapGloss, glossTexturePath, PixelFormat::R8G8B8A8_UNORM);
        FSLLOG3_INFO("- Combining texture");
        for (uint32_t y = 0; y < bitmap.Height(); ++y)
        {
          for (uint32_t x = 0; x < bitmap.Width(); ++x)
          {
            auto col1 = bitmap.GetNativePixel(x, y);
            auto col2 = bitmapGloss.GetNativePixel(x, y);
            const uint32_t color = (col1 & 0xFFFFFF) | ((col2 & 0xFF) << 24);
            bitmap.SetNativePixel(x, y, color);
          }
        }
      }

      GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_texture.SetData(bitmap, texParams, TextureFlags::GenerateMipMaps);

      if (!strTextureSpecular.IsEmpty())
      {
        auto specTexturePath = IO::Path::Combine(MODELS_PATH, strTextureSpecular);
        FSLLOG3_INFO("- Specular '{}'", specTexturePath);
        contentManager->Read(bitmap, specTexturePath, PixelFormat::R8G8B8A8_UNORM);
        m_textureSpecular.SetData(bitmap, texParams, TextureFlags::GenerateMipMaps);
      }

      if (!strTextureNormal.IsEmpty())
      {
        auto normTexturePath = IO::Path::Combine(MODELS_PATH, strTextureNormal);
        FSLLOG3_INFO("- Normal '{}'", normTexturePath);
        contentManager->Read(bitmap, normTexturePath, PixelFormat::R8G8B8A8_UNORM);
        m_textureNormal.SetData(bitmap, texParams, TextureFlags::GenerateMipMaps);
      }
    }

    FSLLOG3_INFO("Preparing shaders");
    PrepareShader(contentManager, m_textureSpecular.IsValid(), !strTextureGloss.IsEmpty(), m_textureNormal.IsValid());

    // Create index and vertex buffers for all the meshes.
    {
      m_indexBuffers.Resize(scene->Meshes.size(), GL_UNSIGNED_SHORT);
      m_vertexBuffers.Resize(scene->Meshes.size(), BasicMesh::vertex_type::GetVertexDeclaration());
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
      FSLLOG3_INFO("Total vertex count: {}, Total index count: {}, SubMesh count: {}", vertexCount, indexCount, scene->GetMeshCount());
    }
  }


  RenderScene::~RenderScene() = default;

  void RenderScene::Update(const DemoTime& demoTime, const Matrix& cameraViewMatrix, const Matrix& cameraRotation, const Vector3& rotation,
                           const PxSize2D& windowSizePx)
  {
    FSL_PARAM_NOT_USED(demoTime);

    m_matrixWorld = Matrix::CreateRotationX(rotation.X) * Matrix::CreateRotationY(rotation.Y) * Matrix::CreateRotationZ(rotation.Z);
    m_matrixView = cameraViewMatrix;
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f),
                                                              windowSizePx.Width() / static_cast<float>(windowSizePx.Height()), 1, 1000.0f);
    m_matrixWorldView = m_matrixWorld * m_matrixView;
    m_matrixWorldViewProjection = m_matrixWorldView * m_matrixProjection;

    Vector4 cameraSpaceLightDirection = Vector4::Transform(m_lightDirection, cameraRotation);
    m_cameraSpaceLightDirection = Vector3(cameraSpaceLightDirection.X, cameraSpaceLightDirection.Y, cameraSpaceLightDirection.Z);
    m_cameraSpaceLightDirection.Normalize();
    m_matrixNormal = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_matrixWorldView)));
  }


  void RenderScene::Draw()
  {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(m_program.Get());

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_texture.Get());
      if (m_textureSpecular.IsValid() && m_locTextureSpecular != GLValues::INVALID_LOCATION)
      {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_textureSpecular.Get());

        glUniform1i(m_locTextureSpecular, 1);
      }
      if (m_textureNormal.IsValid() && m_locTextureNormal != GLValues::INVALID_LOCATION)
      {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_textureNormal.Get());

        glUniform1i(m_locTextureNormal, 2);
      }

      if (m_locLightDirection != GLValues::INVALID_LOCATION)
      {
        glUniform3fv(m_locLightDirection, 1, m_cameraSpaceLightDirection.DirectAccess());
      }
      if (m_locLightColor != GLValues::INVALID_LOCATION)
      {
        glUniform3fv(m_locLightColor, 1, m_lightColor.DirectAccess());
      }
      if (m_locMatAmbient != GLValues::INVALID_LOCATION)
      {
        glUniform4fv(m_locMatAmbient, 1, m_matAmbient.DirectAccess());
      }
      if (m_locMatSpecular != GLValues::INVALID_LOCATION)
      {
        glUniform4fv(m_locMatSpecular, 1, m_matSpecular.DirectAccess());
      }
      if (m_locMatShininess != GLValues::INVALID_LOCATION)
      {
        glUniform1f(m_locMatShininess, m_matShininess);
      }
      // Load the matrices
      if (m_locWorld != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix4fv(m_locWorld, 1, 0, m_matrixWorld.DirectAccess());
      }
      if (m_locWorldView != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix4fv(m_locWorldView, 1, 0, m_matrixWorldView.DirectAccess());
      }
      if (m_locWorldViewProjection != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix4fv(m_locWorldViewProjection, 1, 0, m_matrixWorldViewProjection.DirectAccess());
      }
      if (m_locNormalMatrix != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix3fv(m_locNormalMatrix, 1, 0, m_matrixNormal.DirectAccess());
      }


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


  void RenderScene::DrawMeshes()
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
        m_vertexBuffers.SetVertexAttribPointers(m_attribLink.data(), m_attribLink.size());

        glDrawElements(GL_TRIANGLES, indexBuffer.GetCapacity(), indexBufferType, nullptr);
      }
    }
  }

  void RenderScene::PrepareShader(const std::shared_ptr<IContentManager>& contentManager, const bool useSpecMap, const bool useGlossMap,
                                  const bool useNormalMap)
  {
    IO::Path shaderPath("Shaders");

    std::string baseShaderName("PerPixelDirectionalSpecular");
    if (useSpecMap)
    {
      baseShaderName += "Map";
    }
    if (useGlossMap)
    {
      baseShaderName += "GlossMap";
    }
    if (useNormalMap)
    {
      baseShaderName += "NormalMap";
    }
    baseShaderName += "Textured";

    // auto strFragShaderName = useTexture ? "BasicShaderDLightTextured.frag" : "BasicShaderDLight.frag";
    const IO::Path strVertShaderFilename(baseShaderName + ".vert");
    const IO::Path strFragShaderFilename(baseShaderName + ".frag");
    // std::string strVertShaderFilename("BasicShaderDLight.vert");
    // std::string strFragShaderFilename("BasicShaderDLight.frag");
    // auto strFragShaderName = useTexture ? "BasicShaderDLightTextured.frag" : "BasicShaderDLight.frag";

    const IO::Path strVertShaderPath = IO::Path::Combine(shaderPath, strVertShaderFilename);
    const IO::Path strFragShaderPath = IO::Path::Combine(shaderPath, strFragShaderFilename);
    m_program.Reset(contentManager->ReadAllText(strVertShaderPath), contentManager->ReadAllText(strFragShaderPath));

    m_locWorld = glGetUniformLocation(m_program.Get(), "World");
    m_locWorldView = glGetUniformLocation(m_program.Get(), "WorldView");
    m_locWorldViewProjection = glGetUniformLocation(m_program.Get(), "WorldViewProjection");
    m_locNormalMatrix = glGetUniformLocation(m_program.Get(), "NormalMatrix");
    m_locTexture0 = glGetUniformLocation(m_program.Get(), "Texture0");
    m_locTextureSpecular = glGetUniformLocation(m_program.Get(), "TextureSpecular");
    m_locTextureNormal = glGetUniformLocation(m_program.Get(), "TextureNormal");
    m_locLightDirection = glGetUniformLocation(m_program.Get(), "LightDirection");
    m_locLightColor = glGetUniformLocation(m_program.Get(), "LightColor");
    m_locMatAmbient = glGetUniformLocation(m_program.Get(), "MatAmbient");
    m_locMatSpecular = glGetUniformLocation(m_program.Get(), "MatSpecular");
    m_locMatShininess = glGetUniformLocation(m_program.Get(), "MatShininess");

    auto vertexDecl = BasicMesh::vertex_type::GetVertexDeclaration();
    m_attribLink[0] =
      GLVertexAttribLink(glGetAttribLocation(m_program.Get(), "VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    m_attribLink[1] =
      GLVertexAttribLink(glGetAttribLocation(m_program.Get(), "VertexNormal"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    m_attribLink[2] = GLVertexAttribLink(glGetAttribLocation(m_program.Get(), "VertexTexCoord"),
                                         vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    if (useNormalMap)
    {
      m_attribLink[3] =
        GLVertexAttribLink(glGetAttribLocation(m_program.Get(), "VertexTangent"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Tangent, 0));
    }
    else
    {
      m_attribLink[3] = GLVertexAttribLink(-1, 0);
    }
  }
}
