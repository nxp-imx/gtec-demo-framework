#ifndef GLES3_ANTIALIASING_RENDERMODEL_HPP
#define GLES3_ANTIALIASING_RENDERMODEL_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslUtil/OpenGLES3/GLIndexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexAttribLink.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  class RenderModel final
  {
    struct SubMeshRecord
    {
      GLES3::GLVertexBuffer VertexBuffer;
      GLES3::GLIndexBuffer IndexBuffer;
      uint32_t VertexCount{0};
      uint32_t IndexCount{0};

      SubMeshRecord() = default;
      SubMeshRecord(GLES3::GLVertexBuffer vertexBuffer, GLES3::GLIndexBuffer indexBuffer, const uint32_t vertexCount, const uint32_t indexCount)
        : VertexBuffer(std::move(vertexBuffer))
        , IndexBuffer(std::move(indexBuffer))
        , VertexCount(vertexCount)
        , IndexCount(indexCount)
      {
      }

      SubMeshRecord& operator=(SubMeshRecord&& other) noexcept = default;
      SubMeshRecord(SubMeshRecord&& other) noexcept = default;
    };

    struct ModelRecord
    {
      std::size_t VertexCount{0};
      std::size_t IndexCount{0};
      std::vector<SubMeshRecord> SubMeshes;
      VertexDeclaration VertexDecl;

      ModelRecord() = default;
      ModelRecord(const std::size_t vertexCount, const std::size_t indexCount, std::vector<SubMeshRecord> subMeshes,
                  VertexDeclarationSpan vertexDeclarationSpan)
        : VertexCount(vertexCount)
        , IndexCount(indexCount)
        , SubMeshes(std::move(subMeshes))
        , VertexDecl(vertexDeclarationSpan)
      {
      }
    };

    struct MaterialTextureRecord
    {
      GLES3::GLTexture Diffuse;
      GLES3::GLTexture Specular;
      GLES3::GLTexture Normal;
    };

    struct MaterialShaderRecord
    {
      GLES3::GLProgram Program;
      std::array<GLES3::GLVertexAttribLink, 5> AttribLink;

      GLint LocWorldView = GLES3::GLValues::InvalidLocation;
      GLint LocWorldViewProjection = GLES3::GLValues::InvalidLocation;
      GLint LocNormalMatrix = GLES3::GLValues::InvalidLocation;
      GLint LocLightDirection = GLES3::GLValues::InvalidLocation;

      GLint LocTextureDiffuse = GLES3::GLValues::InvalidLocation;
      GLint LocTextureSpecular = GLES3::GLValues::InvalidLocation;
      GLint LocTextureNormal = GLES3::GLValues::InvalidLocation;
      GLint LocMatAmbient = GLES3::GLValues::InvalidLocation;
      GLint LocMatShininess = GLES3::GLValues::InvalidLocation;
    };

    struct MaterialRecord
    {
      MaterialShaderRecord Shader;
      MaterialTextureRecord Texture;
    };

    struct ModelRenderRecord
    {
      ModelRecord Model;
      MaterialRecord Material;

      ModelRenderRecord() = default;
      ModelRenderRecord(ModelRecord model, MaterialRecord material)
        : Model(std::move(model))
        , Material(std::move(material))
      {
      }
    };

    struct Matrices
    {
      Matrix World;
      Matrix View;
      Matrix Proj;
    };

    struct VertexUBOData
    {
      Matrix MatWorldView;
      Matrix MatWorldViewProjection;
      Matrix3 MatNormal;
    };

    struct FragUBOData
    {
      Vector3 LightDirection;
      Vector3 LightColor;
      Vector4 MatAmbient;
      Vector4 MatSpecular;
      float MatShininess{0};

      FragUBOData() = default;
      FragUBOData(const Vector3& lightColor, const Vector4& matAmbient, const Vector4& matSpecular, const float matShininess)
        : LightColor(lightColor)
        , MatAmbient(matAmbient)
        , MatSpecular(matSpecular)
        , MatShininess(matShininess)
      {
      }
    };

    ModelRenderRecord m_modelRecord;

    Vector3 m_rotationSpeed;
    Vector3 m_rotation;
    Matrices m_matrices;
    Vector3 m_lightDirection;

    VertexUBOData m_vertexUboData;
    FragUBOData m_fragUboData;

  public:
    RenderModel(IContentManager& contentManager, const IO::PathView srcModelPath, const IO::PathView shaderPath, const IO::Path& diffuseTexturePath,
                const IO::Path& specularTexturePath, const IO::Path& normalTexturePath, const float modelScale);

    void Update(const DemoTime& demoTime, const bool isPaused);
    void Draw(const DemoTime& demoTime, const PxSize2D windowSizePx);

  private:
    void DrawModel(const ModelRenderRecord& modelRecord);

    ModelRenderRecord LoadModel(IContentManager& contentManager, const IO::PathView srcModelPath, const IO::PathView shaderPath,
                                const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath, const IO::Path& normalTexturePath,
                                const float modelScale);
    ModelRecord LoadModel(const IO::Path& contentPath, const IO::PathView srcModelPath, const float modelScale);

    MaterialRecord LoadMaterial(IContentManager& contentManager, const IO::PathView shaderPath, const IO::Path& diffuseTexturePath,
                                const IO::Path& specularTexturePath, const IO::Path& normalTexturePath,
                                const VertexDeclarationSpan vertexDeclarationSpan);
    MaterialShaderRecord LoadShader(IContentManager& contentManager, const IO::PathView shaderPath,
                                    const VertexDeclarationSpan vertexDeclarationSpan);
    MaterialTextureRecord LoadTextures(IContentManager& contentManager, const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath,
                                       const IO::Path& normalTexturePath);
  };
}

#endif
