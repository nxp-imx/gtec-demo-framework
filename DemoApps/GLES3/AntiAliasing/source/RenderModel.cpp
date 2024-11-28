/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include "RenderModel.hpp"
#include <FslAssimp/SceneImporter.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTangentTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr float DefaultZoom = 1.1f;
    }

    using TestMesh = Graphics3D::GenericMesh<VertexPositionColorNormalTangentTexture, uint16_t>;
    using TestScene = Graphics3D::GenericScene<TestMesh>;

    Matrix ExtractRotationMatrix(const Matrix matrix)
    {
      const float* pM = matrix.DirectAccess();

      return {pM[MatrixFields::_M11],
              pM[MatrixFields::_M12],
              pM[MatrixFields::_M13],
              0,
              pM[MatrixFields::_M21],
              pM[MatrixFields::_M22],
              pM[MatrixFields::_M23],
              0,
              pM[MatrixFields::_M31],
              pM[MatrixFields::_M32],
              pM[MatrixFields::_M33],
              0,
              0,
              0,
              0,
              1};
    }
  }

  RenderModel::RenderModel(IContentManager& contentManager, const IO::PathView srcModelPath, const IO::PathView shaderPath,
                           const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath, const IO::Path& normalTexturePath,
                           const float modelScale)
    : m_modelRecord(LoadModel(contentManager, srcModelPath, shaderPath, diffuseTexturePath, specularTexturePath, normalTexturePath, modelScale))
    , m_rotationSpeed(0.0f, -0.6f, 0.0f)
    , m_lightDirection(0.0f, 0.0f, 1.0f)
    , m_fragUboData(Vector3(0.8f, 0.8f, 0.8f), Vector4(0.2f, 0.2f, 0.2f, 0.2f), Vector4(1, 1, 1, 1), 100.0f)
  {
    m_lightDirection.Normalize();
  }


  void RenderModel::Update(const DemoTime& demoTime, const bool isPaused)
  {
    if (!isPaused)
    {
      m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
      m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
      m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
      m_rotation.X = MathHelper::WrapAngle(m_rotation.X);
      m_rotation.Y = MathHelper::WrapAngle(m_rotation.Y);
      m_rotation.Z = MathHelper::WrapAngle(m_rotation.Z);
    }

    m_matrices.View = Matrix::CreateLookAt(Vector3(0, 0.2f, LocalConfig::DefaultZoom), Vector3(), Vector3::Up());
    m_matrices.World = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);

    const auto cameraRotation = ExtractRotationMatrix(m_matrices.View);
    Vector4 cameraSpaceLightDirection = Vector4::Transform(m_lightDirection, cameraRotation);
    m_fragUboData.LightDirection = Vector3(cameraSpaceLightDirection.X, cameraSpaceLightDirection.Y, cameraSpaceLightDirection.Z);
    m_fragUboData.LightDirection.Normalize();

    m_vertexUboData.MatWorldView = m_matrices.World * m_matrices.View;
    m_vertexUboData.MatWorldViewProjection = m_vertexUboData.MatWorldView * m_matrices.Proj;
    m_vertexUboData.MatNormal = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_vertexUboData.MatWorldView)));
  }


  void RenderModel::Draw(const DemoTime& demoTime, const PxSize2D windowSizePx)
  {
    const PxSize2DF windowSizePxf = TypeConverter::To<PxSize2DF>(windowSizePx);
    const float aspectRatio = windowSizePxf.RawHeight() > 0 ? windowSizePxf.RawWidth() / windowSizePxf.RawHeight() : 1.0f;
    m_matrices.Proj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectRatio, 1.0f, 1000.0f);

    DrawModel(m_modelRecord);
  }


  void RenderModel::DrawModel(const ModelRenderRecord& modelRecord)
  {
    if (!modelRecord.Model.SubMeshes.empty())
    {
      const auto& matShaderRecord = modelRecord.Material.Shader;

      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);

      glUseProgram(matShaderRecord.Program.Get());

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, modelRecord.Material.Texture.Diffuse.Get());
      glUniform1i(matShaderRecord.LocTextureDiffuse, 0);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, modelRecord.Material.Texture.Specular.Get());
      glUniform1i(matShaderRecord.LocTextureSpecular, 1);

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, modelRecord.Material.Texture.Normal.Get());
      glUniform1i(matShaderRecord.LocTextureNormal, 2);


      glUniform3fv(matShaderRecord.LocLightDirection, 1, m_fragUboData.LightDirection.DirectAccess());
      glUniform4fv(matShaderRecord.LocMatAmbient, 1, m_fragUboData.MatAmbient.DirectAccess());
      glUniform1f(matShaderRecord.LocMatShininess, m_fragUboData.MatShininess);


      // Enable the attribs the meshes use once (since we use the same mesh layout for everything)
      modelRecord.Model.SubMeshes.front().VertexBuffer.EnableAttribArrays(matShaderRecord.AttribLink);

      glUniformMatrix4fv(matShaderRecord.LocWorldView, 1, 0, m_vertexUboData.MatWorldView.DirectAccess());
      glUniformMatrix4fv(matShaderRecord.LocWorldViewProjection, 1, 0, m_vertexUboData.MatWorldViewProjection.DirectAccess());
      glUniformMatrix3fv(matShaderRecord.LocNormalMatrix, 1, 0, m_vertexUboData.MatNormal.DirectAccess());

      // All sub meshes use the same material, so just enable the attrib arrays once
      // modelRecord.Model.SubMeshes.front().VertexBuffer.EnableAttribArrays(matShaderRecord.AttribLink);

      for (const auto& subMesh : modelRecord.Model.SubMeshes)
      {
        if (subMesh.IndexBuffer.GetCapacity() > 0)
        {
          // Bind and enable the vertex buffer
          glBindBuffer(subMesh.VertexBuffer.GetTarget(), subMesh.VertexBuffer.Get());
          glBindBuffer(subMesh.IndexBuffer.GetTarget(), subMesh.IndexBuffer.Get());

          // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
          subMesh.VertexBuffer.SetVertexAttribPointers(matShaderRecord.AttribLink);


          glDrawElements(GL_TRIANGLES, subMesh.IndexBuffer.GetGLCapacity(), subMesh.IndexBuffer.GetType(), nullptr);
        }
      }

      modelRecord.Model.SubMeshes.front().VertexBuffer.DisableAttribArrays(matShaderRecord.AttribLink);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glUseProgram(0);

      glDisable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
    }
  }


  RenderModel::ModelRenderRecord RenderModel::LoadModel(IContentManager& contentManager, const IO::PathView srcModelPath,
                                                        const IO::PathView shaderPath, const IO::Path& diffuseTexturePath,
                                                        const IO::Path& specularTexturePath, const IO::Path& normalTexturePath,
                                                        const float modelScale)
  {
    // Load the model
    auto contentPath = contentManager.GetContentPath();
    auto modelRecord = LoadModel(contentPath, srcModelPath, modelScale);

    // Load the texture and create the material
    MaterialRecord modelMaterial =
      LoadMaterial(contentManager, shaderPath, diffuseTexturePath, specularTexturePath, normalTexturePath, modelRecord.VertexDecl.AsSpan());

    return {std::move(modelRecord), std::move(modelMaterial)};
  }


  RenderModel::ModelRecord RenderModel::LoadModel(const IO::Path& contentPath, const IO::PathView srcModelPath, const float modelScale)
  {
    FSLLOG3_INFO("Loading model '{}'", srcModelPath);
    auto modelPath = IO::Path::Combine(contentPath, srcModelPath);
    SceneImporter sceneImporter;
    const std::shared_ptr<TestScene> scene = sceneImporter.Load<TestScene>(modelPath, modelScale, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException("Scene did not contain any meshes");
    }

    auto rootNode = scene->GetRootNode();
    if (!rootNode)
    {
      throw NotSupportedException("Scene did not contain a root node");
    }

    // The most basic allocation of vertex and index buffers, not optimized in any way
    std::vector<SubMeshRecord> subMeshes(scene->Meshes.size());
    std::size_t totalVertexCount = 0;
    std::size_t totalIndexCount = 0;
    for (std::size_t i = 0; i < scene->Meshes.size(); ++i)
    {
      auto mesh = scene->Meshes[i];
      subMeshes[i] = SubMeshRecord(GLES3::GLVertexBuffer(mesh->AsReadOnlyFlexVertexSpan(), GL_STATIC_DRAW),
                                   GLES3::GLIndexBuffer(mesh->AsReadOnlyIndexSpan(), GL_STATIC_DRAW), mesh->GetVertexCount(), mesh->GetIndexCount());
      totalVertexCount += mesh->GetVertexCount();
      totalIndexCount += mesh->GetIndexCount();
    }

    FSLLOG3_INFO("Total vertex count: {}, Total index count: {}", totalVertexCount, totalIndexCount);
    return {totalVertexCount, totalIndexCount, std::move(subMeshes), scene->Meshes[0]->AsVertexDeclarationSpan()};
  }


  RenderModel::MaterialRecord RenderModel::LoadMaterial(IContentManager& contentManager, const IO::PathView shaderPath,
                                                        const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath,
                                                        const IO::Path& normalTexturePath, const VertexDeclarationSpan vertexDeclarationSpan)
  {
    auto shader = LoadShader(contentManager, shaderPath, vertexDeclarationSpan);
    auto textures = LoadTextures(contentManager, diffuseTexturePath, specularTexturePath, normalTexturePath);
    return {std::move(shader), std::move(textures)};
  }


  RenderModel::MaterialShaderRecord RenderModel::LoadShader(IContentManager& contentManager, const IO::PathView shaderPath,
                                                            const VertexDeclarationSpan vertexDeclarationSpan)
  {
    constexpr IO::PathView StrShaderName("PerPixelDiffuseSpecNormal");
    const IO::Path strVertShaderFilename(StrShaderName + ".vert");
    const IO::Path strFragShaderFilename(StrShaderName + ".frag");
    const IO::Path strVertShaderPath = IO::Path::Combine(shaderPath, strVertShaderFilename);
    const IO::Path strFragShaderPath = IO::Path::Combine(shaderPath, strFragShaderFilename);

    GLES3::GLProgram program(contentManager.ReadAllText(strVertShaderPath), contentManager.ReadAllText(strFragShaderPath));

    std::array<GLES3::GLVertexAttribLink, 5> attribLink{};

    attribLink[0] = GLES3::GLVertexAttribLink(program.GetAttribLocation("VertexPosition"),
                                              vertexDeclarationSpan.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    attribLink[1] = GLES3::GLVertexAttribLink(program.GetAttribLocation("VertexColor"),
                                              vertexDeclarationSpan.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
    attribLink[2] = GLES3::GLVertexAttribLink(program.GetAttribLocation("VertexNormal"),
                                              vertexDeclarationSpan.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    attribLink[3] = GLES3::GLVertexAttribLink(program.GetAttribLocation("VertexTangent"),
                                              vertexDeclarationSpan.VertexElementGetIndexOf(VertexElementUsage::Tangent, 0));
    attribLink[4] = GLES3::GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"),
                                              vertexDeclarationSpan.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));


    const GLint locWorldView = program.GetUniformLocation("WorldView");
    const GLint locWorldViewProjection = program.GetUniformLocation("WorldViewProjection");
    const GLint locNormalMatrix = program.GetUniformLocation("NormalMatrix");
    const GLint locLightDirection = program.GetUniformLocation("LightDirection");

    const GLint locTextureDiffuse = program.GetUniformLocation("TextureDiffuse");
    const GLint locTextureSpecular = program.GetUniformLocation("TextureSpecular");
    const GLint locTextureNormal = program.GetUniformLocation("TextureNormal");

    const GLint locMatAmbient = program.GetUniformLocation("MatAmbient");
    const GLint locMatShininess = program.GetUniformLocation("MatShininess");

    return {std::move(program), attribLink,         locWorldView,     locWorldViewProjection, locNormalMatrix, locLightDirection,
            locTextureDiffuse,  locTextureSpecular, locTextureNormal, locMatAmbient,          locMatShininess};
  }


  RenderModel::MaterialTextureRecord RenderModel::LoadTextures(IContentManager& contentManager, const IO::Path& diffuseTexturePath,
                                                               const IO::Path& specularTexturePath, const IO::Path& normalTexturePath)
  {
    Bitmap bitmap;

    constexpr GLES3::GLTextureParameters TexParamsMipmap(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    constexpr GLES3::GLTextureParameters TexParams(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

    FSLLOG3_INFO("- Diffuse");
    contentManager.Read(bitmap, diffuseTexturePath, PixelFormat::R8G8B8A8_UNORM);
    GLES3::GLTexture texDiffuse(bitmap, TexParamsMipmap, TextureFlags::GenerateMipMaps);

    FSLLOG3_INFO("- Specular");
    contentManager.Read(bitmap, specularTexturePath, PixelFormat::R8G8B8A8_UNORM);
    GLES3::GLTexture texSpecular(bitmap, TexParamsMipmap, TextureFlags::GenerateMipMaps);

    FSLLOG3_INFO("- Normal");
    contentManager.Read(bitmap, normalTexturePath, PixelFormat::R8G8B8A8_UNORM);
    GLES3::GLTexture texNormal(bitmap, TexParams);

    return {std::move(texDiffuse), std::move(texSpecular), std::move(texNormal)};
  }
}
