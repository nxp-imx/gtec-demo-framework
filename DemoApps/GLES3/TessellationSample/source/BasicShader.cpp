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

#include "BasicShader.hpp"
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>

namespace Fsl
{
  using namespace GLES3;

  BasicShader::BasicShader(const std::shared_ptr<IContentManager>& contentManager, VertexDeclarationSpan vertexDeclaration)
    : LocWorldView(GLValues::InvalidLocation)
    , LocWorldViewProjection(GLValues::InvalidLocation)
    , LocNormalMatrix(GLValues::InvalidLocation)
    , LocTextureDiffuse(GLValues::InvalidLocation)
    , LocTextureNormal(GLValues::InvalidLocation)
    , LocLightDirection(GLValues::InvalidLocation)
    , LocMatAmbient(GLValues::InvalidLocation)
    , LocMatSpecular(GLValues::InvalidLocation)
    , LocMatShininess(GLValues::InvalidLocation)
    , AttribLink(4)
  {
    Program.Reset(contentManager->ReadAllText("Shaders/BumpedDLightSpecular.vert"), contentManager->ReadAllText("Shaders/BumpedDLightSpecular.frag"));

    const auto hProgram = Program.Get();

    LocWorldView = glGetUniformLocation(hProgram, "WorldView");
    LocWorldViewProjection = glGetUniformLocation(hProgram, "WorldViewProjection");
    LocNormalMatrix = glGetUniformLocation(hProgram, "NormalMatrix");
    LocTextureDiffuse = glGetUniformLocation(hProgram, "TextureDiffuse");
    LocTextureNormal = glGetUniformLocation(hProgram, "TextureNormal");
    LocLightDirection = glGetUniformLocation(hProgram, "LightDirection");
    LocMatAmbient = glGetUniformLocation(hProgram, "MatAmbient");
    LocMatSpecular = glGetUniformLocation(hProgram, "MatSpecular");
    LocMatShininess = glGetUniformLocation(hProgram, "MatShininess");

    // Prepare the attrib link
    AttribLink[0] =
      GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexPosition"), vertexDeclaration.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    AttribLink[1] =
      GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexNormal"), vertexDeclaration.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    AttribLink[2] =
      GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexTangent"), vertexDeclaration.VertexElementGetIndexOf(VertexElementUsage::Tangent, 0));
    AttribLink[3] = GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexTexCoord"),
                                       vertexDeclaration.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    GL_CHECK_FOR_ERROR();
  }


  void BasicShader::UpdateUniforms(const RenderCameraConfig& cameraConfig, const RenderMaterial& material, const int32_t activeTexDiffuseId,
                                   const int32_t activeTexNormalId)
  {
    // Load the matrices
    if (LocWorldView != GLValues::InvalidLocation)
    {
      glUniformMatrix4fv(LocWorldView, 1, 0, cameraConfig.WorldView.DirectAccess());
    }

    if (LocWorldViewProjection != GLValues::InvalidLocation)
    {
      glUniformMatrix4fv(LocWorldViewProjection, 1, 0, cameraConfig.WorldViewProjection.DirectAccess());
    }

    if (LocNormalMatrix != GLValues::InvalidLocation)
    {
      glUniformMatrix3fv(LocNormalMatrix, 1, 0, cameraConfig.NormalMatrix.DirectAccess());
    }

    if (LocLightDirection != GLValues::InvalidLocation)
    {
      glUniform3fv(LocLightDirection, 1, cameraConfig.CameraSpaceLightDirection.DirectAccess());
    }

    if (LocMatAmbient != GLValues::InvalidLocation)
    {
      glUniform3fv(LocMatAmbient, 1, material.Ambient.DirectAccess());
    }

    if (LocMatSpecular != GLValues::InvalidLocation)
    {
      glUniform3fv(LocMatSpecular, 1, material.Specular.DirectAccess());
    }

    if (LocMatShininess != GLValues::InvalidLocation)
    {
      glUniform1f(LocMatShininess, material.Shininess);
    }

    if (LocTextureDiffuse != GLValues::InvalidLocation)
    {
      glUniform1i(LocTextureDiffuse, activeTexDiffuseId);
    }

    if (LocTextureNormal != GLValues::InvalidLocation)
    {
      glUniform1i(LocTextureNormal, activeTexNormalId);
    }
  }
}
