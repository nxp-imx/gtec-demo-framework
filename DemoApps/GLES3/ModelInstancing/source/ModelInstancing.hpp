#ifndef GLES3_MODELINSTANCING_MODELINSTANCING_HPP
#define GLES3_MODELINSTANCING_MODELINSTANCING_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLDataBuffer.hpp>
#include <FslUtil/OpenGLES3/GLIndexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <Shared/ModelInstancing/ModelInstancingShared.hpp>
#include <array>

namespace Fsl
{
  namespace Graphics3D
  {
    class SceneNode;
  }

  class ModelInstancing final : public DemoAppGLES3
  {
    struct Resources
    {
      GLES3::GLTexture Texture;
      GLES3::GLIndexBuffer IndexBuffer;
      GLES3::GLVertexBuffer VertexBuffer;
      GLES3::GLDataBuffer InstanceBuffer;
      GLES3::GLProgram Program;
      GLint LocView = GLES3::GLValues::INVALID_LOCATION;
      GLint LocProjection = GLES3::GLValues::INVALID_LOCATION;
      GLint LocTexture0 = GLES3::GLValues::INVALID_LOCATION;
      GLint LocLightDirection = GLES3::GLValues::INVALID_LOCATION;
      GLint LocLightColor = GLES3::GLValues::INVALID_LOCATION;
      GLint LocAmbientColor = GLES3::GLValues::INVALID_LOCATION;

      std::array<GLES3::GLVertexAttribLink, 4> AttribLink{};
      std::array<GLES3::GLVertexAttribLink, 1> InstanceAttribLink{};
    };

    struct VertexData
    {
      Matrix MatView;
      Matrix MatProjection;
    };

    struct FragData
    {
      Vector3 LightDirection;
      Vector3 LightColor;
      Vector3 AmbientColor;

      FragData() = default;
      FragData(const Vector3& lightDirection, const Vector3& lightColor, const Vector3& ambientColor)
        : LightDirection(lightDirection)
        , LightColor(lightColor)
        , AmbientColor(ambientColor)
      {
      }
    };

    Resources m_resources;

    VertexData m_vertexUboData;
    FragData m_fragUboData;

    ModelInstancingShared m_shared;

  public:
    explicit ModelInstancing(const DemoAppConfig& config);
    ~ModelInstancing() final;

  protected:
    void OnKeyEvent(const KeyEvent& event) final;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void Draw(const FrameInfo& frameInfo) final;

  private:
    void DrawMeshes(const uint32_t instanceCount);
  };
}

#endif
