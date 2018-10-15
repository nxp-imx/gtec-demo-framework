/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include "FurShaderBase.hpp"
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <algorithm>
#include <cassert>


namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    const IO::Path GetVert(const IO::Path& shaderPath, const bool useHighPrecision)
    {
      return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur_hp.vert" : "Fur_lp.vert"));
    }


    const IO::Path GetFrag(const IO::Path& shaderPath, const bool useHighPrecision, const int lightCount)
    {
      switch (lightCount)
      {
      case 1:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur1_hp.frag" : "Fur1_lp.frag"));
      case 2:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur2_hp.frag" : "Fur2_lp.frag"));
      case 3:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur3_hp.frag" : "Fur3_lp.frag"));
      case 4:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur4_hp.frag" : "Fur4_lp.frag"));
      default:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "FurN_hp.frag" : "FurN_lp.frag"));
      }
    }
  }

  FurShaderBase::FurShaderBase(const IContentManager& contentManager, const IO::Path& shaderPath, const bool useHighPrecision, const int lightCount)
    : ShaderBase(contentManager.ReadAllText(GetVert(shaderPath, useHighPrecision)),
                 contentManager.ReadAllText(GetFrag(shaderPath, useHighPrecision, lightCount)))
    , m_lightCount(lightCount)
    , m_locWorld(GLValues::INVALID_LOCATION)
    , m_locView(GLValues::INVALID_LOCATION)
    , m_locProjection(GLValues::INVALID_LOCATION)
    , m_locTexture0(GLValues::INVALID_LOCATION)
    , m_locTexture1(GLValues::INVALID_LOCATION)
    , m_locLightCount(GLValues::INVALID_LOCATION)
    , m_locAmbientColor(GLValues::INVALID_LOCATION)
    , m_locMaxHairLength(GLValues::INVALID_LOCATION)
    , m_locDisplacement(GLValues::INVALID_LOCATION)
  {
    Construct(lightCount);
  }


  FurShaderBase::FurShaderBase(const IContentManager& contentManager, const IO::Path& vertShaderPath, const IO::Path& fragShaderPath,
                               const int lightCount)
    : ShaderBase(contentManager.ReadAllText(vertShaderPath), contentManager.ReadAllText(fragShaderPath))
    , m_lightCount(lightCount)
    , m_locWorld(GLValues::INVALID_LOCATION)
    , m_locView(GLValues::INVALID_LOCATION)
    , m_locProjection(GLValues::INVALID_LOCATION)
    , m_locTexture0(GLValues::INVALID_LOCATION)
    , m_locTexture1(GLValues::INVALID_LOCATION)
    , m_locLightCount(GLValues::INVALID_LOCATION)
    , m_locAmbientColor(GLValues::INVALID_LOCATION)
    , m_locMaxHairLength(GLValues::INVALID_LOCATION)
    , m_locDisplacement(GLValues::INVALID_LOCATION)
  {
    Construct(lightCount);
  }


  void FurShaderBase::SetWorld(const Matrix& matrix)
  {
    glUniformMatrix4fv(m_locWorld, 1, GL_FALSE, matrix.DirectAccess());
  }

  void FurShaderBase::SetView(const Matrix& matrix)
  {
    glUniformMatrix4fv(m_locView, 1, GL_FALSE, matrix.DirectAccess());
  }

  void FurShaderBase::SetProjection(const Matrix& matrix)
  {
    glUniformMatrix4fv(m_locProjection, 1, GL_FALSE, matrix.DirectAccess());
  }


  void FurShaderBase::SetDisplacement(const Vector3& displacement)
  {
    assert(IsLoaded());
    glUniform3f(m_locDisplacement, displacement.X, displacement.Y, displacement.Z);
  }

  int FurShaderBase::GetLightCount() const
  {
    return m_lightCount;
  }

  void FurShaderBase::SetLightDirection(const int index, const Vector3& lightDirection)
  {
    // This is very inefficient, we should actually only send it to the driver on bind and only dirty ones (but that goes for all the params)
    assert(IsLoaded());
    if (index < 0 || index > m_lightCount)
    {
      throw std::invalid_argument("invalid light index");
    }

    if (m_lightCount <= 4)
    {
      glUniform3f(m_locLightDirection[index], lightDirection.X, lightDirection.Y, lightDirection.Z);
    }
    else
    {
      m_lightDirection[index * 3 + 0] = lightDirection.X;
      m_lightDirection[index * 3 + 1] = lightDirection.Y;
      m_lightDirection[index * 3 + 2] = lightDirection.Z;
      glUniform3fv(m_locLightDirection[0], m_lightCount * 3, &m_lightDirection[0]);
    }
  }


  void FurShaderBase::SetLightColor(const int index, const Vector3& lightColor)
  {
    // This is very inefficient, we should actually only send it to the driver on bind and only dirty ones (but that goes for all the params)

    assert(IsLoaded());
    if (index < 0 || index > m_lightCount)
    {
      throw std::invalid_argument("invalid light index");
    }

    if (m_lightCount <= 4)
    {
      glUniform3f(m_locLightColor[index], lightColor.X, lightColor.Y, lightColor.Z);
    }
    else
    {
      m_lightColor[index * 3 + 0] = lightColor.X;
      m_lightColor[index * 3 + 1] = lightColor.Y;
      m_lightColor[index * 3 + 2] = lightColor.Z;
      glUniform3fv(m_locLightColor[0], m_lightCount * 3, &m_lightColor[0]);
    }
  }


  void FurShaderBase::SetLightAmbientColor(const Vector3& ambientColor)
  {
    assert(IsLoaded());
    glUniform3f(m_locAmbientColor, ambientColor.X, ambientColor.Y, ambientColor.Z);
  }


  void FurShaderBase::SetMaxHairLength(const float maxHairLength)
  {
    assert(IsLoaded());
    glUniform1f(m_locMaxHairLength, maxHairLength);
  }


  void FurShaderBase::SetTexture0(const int unit)
  {
    assert(IsLoaded());
    glUniform1i(m_locTexture1, unit);
  }


  void FurShaderBase::SetTexture1(const int unit)
  {
    assert(IsLoaded());
    glUniform1i(m_locTexture1, unit);
  }


  void FurShaderBase::SetLightCount(const int lightCount)
  {
    assert(IsLoaded());
    if (m_locLightCount != GLValues::INVALID_LOCATION)
    {
      glUniform1f(m_locLightCount, GLfloat(lightCount));
    }
  }

  void FurShaderBase::Construct(const int32_t lightCount)
  {
    const GLuint hProgram = Get();

    // Get attribute locations of non-fixed attributes like color and texture coordinates.
    m_shaderConfig.Position = GL_CHECK(glGetAttribLocation(hProgram, "VertexPosition"));
    m_shaderConfig.Normal = GL_CHECK(glGetAttribLocation(hProgram, "VertexNormal"));
    m_shaderConfig.TexCoord = GL_CHECK(glGetAttribLocation(hProgram, "TexCoord"));

    // Get uniform locations
    m_locWorld = GL_CHECK(glGetUniformLocation(hProgram, "World"));
    m_locView = GL_CHECK(glGetUniformLocation(hProgram, "View"));
    m_locProjection = GL_CHECK(glGetUniformLocation(hProgram, "Projection"));
    m_locTexture0 = GL_CHECK(glGetUniformLocation(hProgram, "Texture0"));
    m_locTexture1 = GL_CHECK(glGetUniformLocation(hProgram, "Texture1"));

    // Fill the arrays with default values

    if (lightCount <= 4)
    {
      m_locLightDirection.resize(lightCount);
      m_locLightColor.resize(lightCount);
      std::fill(m_locLightDirection.begin(), m_locLightDirection.end(), GLValues::INVALID_LOCATION);
      std::fill(m_locLightColor.begin(), m_locLightColor.end(), GLValues::INVALID_LOCATION);

      m_locLightDirection[0] = GL_CHECK(glGetUniformLocation(hProgram, "LightDirection1"));
      m_locLightColor[0] = GL_CHECK(glGetUniformLocation(hProgram, "LightColor1"));
      if (lightCount >= 2)
      {
        m_locLightDirection[1] = GL_CHECK(glGetUniformLocation(hProgram, "LightDirection2"));
        m_locLightColor[1] = GL_CHECK(glGetUniformLocation(hProgram, "LightColor2"));
      }
      if (lightCount >= 3)
      {
        m_locLightDirection[2] = GL_CHECK(glGetUniformLocation(hProgram, "LightDirection3"));
        m_locLightColor[2] = GL_CHECK(glGetUniformLocation(hProgram, "LightColor3"));
      }
      if (lightCount >= 4)
      {
        m_locLightDirection[3] = GL_CHECK(glGetUniformLocation(hProgram, "LightDirection4"));
        m_locLightColor[3] = GL_CHECK(glGetUniformLocation(hProgram, "LightColor4"));
      }
    }
    else
    {
      m_locLightDirection.resize(1);
      m_locLightColor.resize(1);
      m_locLightDirection[0] = GL_CHECK(glGetUniformLocation(hProgram, "LightDirection"));
      m_locLightColor[0] = GL_CHECK(glGetUniformLocation(hProgram, "LightColor"));
      m_locLightCount = GL_CHECK(glGetUniformLocation(hProgram, "LightCount"));
      m_lightDirection.resize(3 * lightCount);
      m_lightColor.resize(3 * lightCount);
      std::fill(m_lightDirection.begin(), m_lightDirection.end(), 0.0f);
      std::fill(m_lightColor.begin(), m_lightColor.end(), 0.0f);
    }

    m_locAmbientColor = GL_CHECK(glGetUniformLocation(hProgram, "AmbientColor"));

    m_locMaxHairLength = GL_CHECK(glGetUniformLocation(hProgram, "MaxHairLength"));

    m_locDisplacement = GL_CHECK(glGetUniformLocation(hProgram, "Displacement"));


    {    // Set the default values
      ScopedUse scope(*this);

      SetWorld(Matrix::GetIdentity());
      SetView(Matrix::GetIdentity());
      SetProjection(Matrix::GetIdentity());

      SetDisplacement(Vector3::Zero());

      for (int i = 0; i < lightCount; ++i)
      {
        SetLightDirection(i, Vector3(0.0f, 1.0f, 0.0f));
        SetLightColor(i, Vector3::One());
      }
      SetLightCount(lightCount);
      SetLightAmbientColor(Vector3::One());

      SetMaxHairLength(1.0f);

      SetTexture0(0);
      SetTexture1(0);
    }
  }
}
