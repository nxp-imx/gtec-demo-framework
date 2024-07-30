/****************************************************************************************************************************************************
 * Copyright 2018, 2024 NXP
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

#include "AssimpDoubleTexture.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>

// Will use generic assimp header
#include <assimp/Importer.hpp>    //OO version Header!
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Fsl
{
  using namespace GLES3;
  namespace
  {
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    void Color4ToFloat4(const aiColor4D* c, float f[4])
    {
      f[0] = c->r;
      f[1] = c->g;
      f[2] = c->b;
      f[3] = c->a;
    }
  }


  AssimpDoubleTexture::AssimpDoubleTexture(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    auto contentManger = GetContentManager();
    auto contentPath = contentManger->GetContentPath();

    m_program3D.Reset(contentManger->ReadAllText("3dAsset.vert"), contentManger->ReadAllText("3dAsset.frag"));

    const GLuint hProgram = m_program3D.Get();
    glUseProgram(hProgram);

    // Define Attribute and Uniform Locations for the 3D Car
    m_attribLocMeshPosition = glGetAttribLocation(hProgram, "g_vinPosition");
    m_attribLocMeshNormal = glGetAttribLocation(hProgram, "g_vinNormal");
    m_attribLocMeshTexCoord = glGetAttribLocation(hProgram, "g_vinTexCoord");

    m_modelViewMatrixLoc = glGetUniformLocation(hProgram, "u_modelViewMatrix");
    m_modelViewProjectionMatrixLoc = glGetUniformLocation(hProgram, "u_modelViewProjectionMatrix");
    m_normalMatrixLoc = glGetUniformLocation(hProgram, "u_normalMatrix");

    m_lightIntensityLoc = glGetUniformLocation(hProgram, "lightIntensity");
    m_lightLoc = glGetUniformLocation(hProgram, "lightPosition");
    m_diffuseLoc = glGetUniformLocation(hProgram, "Kd");
    m_ambientLoc = glGetUniformLocation(hProgram, "Ka");
    m_specularLoc = glGetUniformLocation(hProgram, "Ks");
    m_shininessLoc = glGetUniformLocation(hProgram, "shininess");

    m_viewMatrix = glm::lookAt(glm::vec3(0, 4.0, -9.5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    const float aspectRatio = GetWindowAspectRatio();
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

    m_lightPosition = m_viewMatrix * glm::vec4(0, 3.0f, -2.0, 1.0f);
    GL_CHECK(glUniform4fv(m_lightLoc, 1, glm::value_ptr(m_lightPosition)));

    m_lightIntensity = glm::vec3(0.95, 0.95, 0.95);
    GL_CHECK(glUniform3fv(m_lightIntensityLoc, 1, glm::value_ptr(m_lightIntensity)));

    auto modelPath = IO::Path::Combine(contentPath, "Cog/CogUV.obj");
    LoadModel(modelPath.ToAsciiString());

    // DOUBLE TEXTURE CODE:
    // Get the texture units uniform locations:
    m_texUnitLoc1 = glGetUniformLocation(hProgram, "texUnit1");
    m_texUnitLoc2 = glGetUniformLocation(hProgram, "texUnit2");
    m_mixFactorLoc = glGetUniformLocation(hProgram, "mixFactor");

    {    // Populate the texture objects
      Bitmap bitmap;
      const GLTextureParameters texParams(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      contentManger->Read(bitmap, "Cog/Clean.jpg", PixelFormat::R8G8B8A8_UNORM);
      m_textureClean.SetData(bitmap, texParams, TextureFlags::GenerateMipMaps);

      contentManger->Read(bitmap, "Cog/Dirty.jpg", PixelFormat::R8G8B8A8_UNORM);
      m_textureDirty.SetData(bitmap, texParams, TextureFlags::GenerateMipMaps);
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }


  void AssimpDoubleTexture::Update(const DemoTime& demoTime)
  {
    m_rotation += m_rotationSpeed * demoTime.DeltaTime * 1.5f;
    m_modelMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::rotate(m_modelMatrix, m_rotation, glm::vec3(0.0, 1.0, 0.0));
    // Update the mix Factor
    if (0 == m_dirtDirection)
    {
      m_mixFactor += 0.6f * demoTime.DeltaTime;
      if (m_mixFactor > 1.0f)
      {
        m_mixFactor = 1.0f;
        m_dirtDirection = 1;
      }
    }
    else
    {
      m_mixFactor -= 0.6f * demoTime.DeltaTime;
      if (m_mixFactor < 0.0f)
      {
        m_mixFactor = 0.0f;
        m_dirtDirection = 0;
      }
    }
  }


  void AssimpDoubleTexture::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_program3D.Get());

    m_modelViewMatrix = m_viewMatrix * m_modelMatrix;
    m_modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix;
    m_normalMatrix = glm::mat3(glm::vec3(m_modelViewMatrix[0]), glm::vec3(m_modelViewMatrix[1]), glm::vec3(m_modelViewMatrix[2]));
    glUniformMatrix4fv(m_modelViewMatrixLoc, 1, 0, glm::value_ptr(m_modelViewMatrix));
    glUniformMatrix4fv(m_modelViewProjectionMatrixLoc, 1, 0, glm::value_ptr(m_modelViewProjectionMatrix));
    glUniformMatrix3fv(m_normalMatrixLoc, 1, 0, glm::value_ptr(m_normalMatrix));

    // ACTIVATE BOTH TEXTURE SLOTS
    glUniform1i(m_texUnitLoc1, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureClean.Get());

    glUniform1i(m_texUnitLoc2, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textureDirty.Get());

    // Write Dirt Factor
    glUniform1f(m_mixFactorLoc, m_mixFactor);

    for (unsigned int i = 0; i < m_mesh.size(); i++)
    {
      glBindVertexArray(m_mesh[i].VAO);
      if (m_diffuseLoc != GLValues::InvalidLocation)
      {
        GL_CHECK(glUniform3fv(m_diffuseLoc, 1, m_mesh[i].Diffuse));
      }
      if (m_ambientLoc != GLValues::InvalidLocation)
      {
        GL_CHECK(glUniform3fv(m_ambientLoc, 1, m_mesh[i].Ambient));
      }
      if (m_specularLoc != GLValues::InvalidLocation)
      {
        GL_CHECK(glUniform3fv(m_specularLoc, 1, m_mesh[i].Specular));
      }
      if (m_shininessLoc != GLValues::InvalidLocation)
      {
        GL_CHECK(glUniform1f(m_shininessLoc, m_mesh[i].Shininess));
      }
      glDrawElements(GL_TRIANGLES, m_mesh[i].NumFaces * 3, GL_UNSIGNED_INT, nullptr);
      glBindVertexArray(0);
    }
  }

  void AssimpDoubleTexture::LoadModel(const std::string& modelPath)
  {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate);
    try
    {
      if ((scene == nullptr) || ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0u) || (scene->mRootNode == nullptr))
      {
        throw IOException(std::string("Failed to read the requested model: ") + modelPath);
      }
      FSLLOG3_INFO("Scene Properly Imported");
    }
    catch (const std::exception&)
    {
      importer.FreeScene();
      throw;
    }
    // Once a Scene is Loaded, fill the Vertex Buffer Objects of the Mesh Struct

    if (scene->mNumMeshes != 0u)
    {
      m_mesh.resize(scene->mNumMeshes);
    }
    else
    {
      throw GraphicsException("No Meshes Available on the Scene");
    }

    for (unsigned int n = 0; n < scene->mNumMeshes; ++n)
    {
      const aiMesh* mesh = scene->mMeshes[n];
      if (!(mesh->HasPositions()) || !(mesh->HasNormals()) || !(mesh->HasTextureCoords(0)))
      {
        throw GraphicsException("One of the Meshes did not have the required vertex Data\n Positions, Normals and Texture Coordinates");
      }

      std::vector<unsigned int> faceArray;
      for (unsigned int t = 0; t < mesh->mNumFaces; ++t)
      {
        const aiFace* face = &mesh->mFaces[t];
        faceArray.push_back(face->mIndices[0]);
        faceArray.push_back(face->mIndices[1]);
        faceArray.push_back(face->mIndices[2]);
      }
      m_mesh[n].NumFaces = NumericCast<int>(mesh->mNumFaces);

      // Store All Vertices Data into the Mesh Structure
      m_mesh[n].VertexData.resize(mesh->mNumVertices);
      for (unsigned int i = 0; i < mesh->mNumVertices; i++)
      {
        m_mesh[n].VertexData[i].Position.x = mesh->mVertices[i].x;
        m_mesh[n].VertexData[i].Position.y = mesh->mVertices[i].y;
        m_mesh[n].VertexData[i].Position.z = mesh->mVertices[i].z;
        m_mesh[n].VertexData[i].Normal.x = mesh->mNormals[i].x;
        m_mesh[n].VertexData[i].Normal.y = mesh->mNormals[i].y;
        m_mesh[n].VertexData[i].Normal.z = mesh->mNormals[i].z;
        m_mesh[n].VertexData[i].TexCoords.x = mesh->mTextureCoords[0][i].x;
        m_mesh[n].VertexData[i].TexCoords.y = mesh->mTextureCoords[0][i].y;
      }

      // VAO storing VBOs
      glGenVertexArrays(1, &(m_mesh[n].VAO));
      glBindVertexArray(m_mesh[n].VAO);

      // Buffer for Indices
      glGenBuffers(1, &(m_mesh[n].IndexBo));
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh[n].IndexBo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, UncheckedNumericCast<GLsizei>(sizeof(unsigned int) * mesh->mNumFaces * 3), faceArray.data(),
                   GL_STATIC_DRAW);

      // VBO for the whole Mesh
      glGenBuffers(1, &(m_mesh[n].PosVbo));
      glBindBuffer(GL_ARRAY_BUFFER, m_mesh[n].PosVbo);
      glBufferData(GL_ARRAY_BUFFER, UncheckedNumericCast<GLsizei>(m_mesh[n].VertexData.size() * sizeof(MeshVertex)), &(m_mesh[n].VertexData[0]),
                   GL_STATIC_DRAW);

      // Assign the VBO Attributes with offsets
      glEnableVertexAttribArray(m_attribLocMeshPosition);
      glVertexAttribPointer(m_attribLocMeshPosition, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), nullptr);
      glEnableVertexAttribArray(m_attribLocMeshNormal);
      glVertexAttribPointer(m_attribLocMeshNormal, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, Normal)));
      glEnableVertexAttribArray(m_attribLocMeshTexCoord);
      glVertexAttribPointer(m_attribLocMeshTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                            reinterpret_cast<void*>(offsetof(MeshVertex, TexCoords)));

      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

      // Load the materials
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

      aiColor4D aiDiffuse;
      if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &aiDiffuse))
      {
        Color4ToFloat4(&aiDiffuse, m_mesh[n].Diffuse);
      }

      aiColor4D aiAmbient;
      if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &aiAmbient))
      {
        Color4ToFloat4(&aiAmbient, m_mesh[n].Ambient);
      }

      aiColor4D aiSpecular;
      if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &aiSpecular))
      {
        Color4ToFloat4(&aiSpecular, m_mesh[n].Specular);
      }

      unsigned int max = 0;
      aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &m_mesh[n].Shininess, &max);
    }
  }
}
