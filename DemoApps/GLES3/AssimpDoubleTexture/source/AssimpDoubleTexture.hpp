#ifndef GLES3_ASSIMPDOUBLETEXTURE_ASSIMPDOUBLETEXTURE_HPP
#define GLES3_ASSIMPDOUBLETEXTURE_ASSIMPDOUBLETEXTURE_HPP
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

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>

// GLM Headers for standard math operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Fsl
{
  struct MeshVertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
  };

  struct Mesh
  {
    std::vector<MeshVertex> vertexData;
    GLuint VAO, posVBO, indexBO;
    int numFaces;

    float diffuse[4];
    float ambient[4];
    float specular[4];
    float shininess;
  };

  class AssimpDoubleTexture : public DemoAppGLES3
  {
    GLES3::GLProgram m_program3D;

    std::vector<Mesh> m_mesh;

    // Attributes needed to render a 3D mesh
    GLint m_attribLocMeshPosition{GLES3::GLValues::INVALID_LOCATION};
    GLint m_attribLocMeshNormal{GLES3::GLValues::INVALID_LOCATION};
    GLint m_attribLocMeshTexCoord{GLES3::GLValues::INVALID_LOCATION};

    // Uniforms needed to render
    // Uniforms
    // Matrices
    glm::mat4 m_modelMatrix{};
    glm::mat4 m_viewMatrix{};
    glm::mat4 m_projectionMatrix{};
    glm::mat4 m_modelViewMatrix{};
    glm::mat4 m_modelViewProjectionMatrix{};
    glm::mat3 m_normalMatrix{};

    // Matrices shader location
    GLint m_modelMatrixLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_viewMatrixLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_projectionMatrixLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_modelViewMatrixLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_modelViewProjectionMatrixLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_normalMatrixLoc{GLES3::GLValues::INVALID_LOCATION};


    // Light values
    glm::vec3 m_lightIntensity{};
    glm::vec4 m_lightPosition{};
    glm::vec3 m_lightColor{};
    GLfloat m_ambientStrength{};
    GLfloat m_specularStrength{};

    // Light Locations
    GLint m_lightIntensityLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_lightLoc{GLES3::GLValues::INVALID_LOCATION};

    GLint m_diffuseLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_ambientLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_specularLoc{GLES3::GLValues::INVALID_LOCATION};
    GLint m_shininessLoc{GLES3::GLValues::INVALID_LOCATION};

    // Textures, this cog example uses 2 textures
    GLES3::GLTexture textureClean;
    GLES3::GLTexture textureDirty;
    GLfloat m_mixFactor{0.0f};
    GLint m_texUnitLoc1{GLES3::GLValues::INVALID_LOCATION};
    GLint m_texUnitLoc2{GLES3::GLValues::INVALID_LOCATION};
    GLint m_mixFactorLoc{GLES3::GLValues::INVALID_LOCATION};

    float m_rotation{0.0f};
    float m_rotationSpeed{-0.6f};

    int m_dirtDirection{0};

  public:
    AssimpDoubleTexture(const DemoAppConfig& config);

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;
    void LoadModel(const std::string& modelPath);
  };
}

#endif
