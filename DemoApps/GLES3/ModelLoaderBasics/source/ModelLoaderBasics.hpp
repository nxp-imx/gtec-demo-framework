#ifndef GLES3_MODELLOADERBASICS_MODELLOADERBASICS_HPP
#define GLES3_MODELLOADERBASICS_MODELLOADERBASICS_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLIndexBufferArray.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBufferArray.hpp>
#include <array>

namespace Fsl
{
  namespace Graphics3D
  {
    class SceneNode;
  }

  class ModelLoaderBasics : public DemoAppGLES3
  {
    struct Resources
    {
      GLES3::GLTexture Texture;
      GLES3::GLIndexBufferArray IndexBuffers;
      GLES3::GLVertexBufferArray VertexBuffers;
      GLES3::GLProgram Program;
      GLint LocWorldView = GLES3::GLValues::InvalidLocation;
      GLint LocWorldViewProjection = GLES3::GLValues::InvalidLocation;
      GLint LocNormalMatrix = GLES3::GLValues::InvalidLocation;
      GLint LocTexture0 = GLES3::GLValues::InvalidLocation;
      GLint LocLightDirection = GLES3::GLValues::InvalidLocation;
      GLint LocLightColor = GLES3::GLValues::InvalidLocation;
      GLint LocAmbientColor = GLES3::GLValues::InvalidLocation;

      std::array<GLES3::GLVertexAttribLink, 4> AttribLink{};
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
      Vector3 AmbientColor;

      FragUBOData() = default;
      FragUBOData(const Vector3& lightDirection, const Vector3& lightColor, const Vector3& ambientColor)
        : LightDirection(lightDirection)
        , LightColor(lightColor)
        , AmbientColor(ambientColor)
      {
      }
    };

    Resources m_resources;

    Matrix m_matrixWorld;
    Matrix m_matrixView;
    Matrix m_matrixProjection;
    Vector3 m_rotationSpeed;
    Vector3 m_rotation;

    VertexUBOData m_vertexUboData;
    FragUBOData m_fragUboData;

  public:
    explicit ModelLoaderBasics(const DemoAppConfig& config);
    ~ModelLoaderBasics() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void DrawMeshes();
  };
}

#endif
