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

#include "TessellationShader.hpp"
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
// Because of inconsistency in khronos extension definition both the 31 and 2 headers are needed
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>

namespace Fsl
{
  using namespace GLES3;

  TessellationShader::TessellationShader(const std::shared_ptr<IContentManager>& contentManager, const VertexDeclaration& vertexDeclaration)
    : Pipeline()
    , ShaderVert()
    , ShaderControl()
    , ShaderEval()
    , ShaderFrag()
    , ShaderGeomWireframe()
    , LocTessLevelInner(GLValues::INVALID_LOCATION)
    , LocTessLevelOuter(GLValues::INVALID_LOCATION)
    , LocWorld(GLValues::INVALID_LOCATION)
    , LocWorldView(GLValues::INVALID_LOCATION)
    , LocWorldViewProjection(GLValues::INVALID_LOCATION)
    , LocNormalMatrix(GLValues::INVALID_LOCATION)
    , LocTextureDiffuse(GLValues::INVALID_LOCATION)
    , LocTextureNormalEval(GLValues::INVALID_LOCATION)
    , LocTextureNormalFrag(GLValues::INVALID_LOCATION)
    , LocTextureDisplacement(GLValues::INVALID_LOCATION)
    , LocLightDirection(GLValues::INVALID_LOCATION)
    , LocDisplacementFactor(GLValues::INVALID_LOCATION)
    , LocDisplacementMod(GLValues::INVALID_LOCATION)
    , LocMatAmbient(GLValues::INVALID_LOCATION)
    , LocMatSpecular(GLValues::INVALID_LOCATION)
    , LocMatShininess(GLValues::INVALID_LOCATION)
    , AttribLink(4)
  {
    const std::string strVert = contentManager->ReadAllText("Shaders/Tesselation.vert");
    const std::string strControl = contentManager->ReadAllText("Shaders/Tesselation.tesc");
    const std::string strEvaluation = contentManager->ReadAllText("Shaders/Tesselation.tese");
    const std::string strGeomWireframe = contentManager->ReadAllText("Shaders/TesselationWireframe.geom");
    const std::string strFrag = contentManager->ReadAllText("Shaders/Tesselation.frag");

    //GLShader shaderVert(GL_VERTEX_SHADER, strVert);
    //GLShader shaderControl(GL_TESS_CONTROL_SHADER_EXT, strControl);
    //GLShader shaderEval(GL_TESS_EVALUATION_SHADER_EXT, strEvaluation);
    //GLShader shaderFrag(GL_FRAGMENT_SHADER, strFrag);
    //GLShader shaderGeom(GL_GEOMETRY_SHADER_EXT, strGeom);
    //GLShader shaderGeomWireframe(GL_GEOMETRY_SHADER_EXT, strGeomWireframe);

    ShaderVert.Reset(GL_VERTEX_SHADER, strVert);
    ShaderControl.Reset(GL_TESS_CONTROL_SHADER_EXT, strControl);
    ShaderEval.Reset(GL_TESS_EVALUATION_SHADER_EXT, strEvaluation);
    ShaderGeomWireframe.Reset(GL_GEOMETRY_SHADER_EXT, strGeomWireframe);
    ShaderFrag.Reset(GL_FRAGMENT_SHADER, strFrag);

    LocTessLevelInner = glGetUniformLocation(ShaderControl.Get(), "TessLevelInner");
    LocTessLevelOuter = glGetUniformLocation(ShaderControl.Get(), "TessLevelOuter");
    LocWorld = glGetUniformLocation(ShaderEval.Get(), "World");
    LocWorldView = glGetUniformLocation(ShaderEval.Get(), "WorldView");
    LocWorldViewProjection = glGetUniformLocation(ShaderEval.Get(), "WorldViewProjection");
    LocTextureNormalEval = glGetUniformLocation(ShaderEval.Get(), "TextureNormal");
    LocTextureDisplacement = glGetUniformLocation(ShaderEval.Get(), "TextureDisplacement");
    LocNormalMatrix = glGetUniformLocation(ShaderEval.Get(), "NormalMatrix");
    LocLightDirection = glGetUniformLocation(ShaderEval.Get(), "LightDirection");
    LocDisplacementFactor = glGetUniformLocation(ShaderEval.Get(), "DisplacementFactor");
    LocDisplacementMod = glGetUniformLocation(ShaderEval.Get(), "DisplacementMod");
    LocTextureDiffuse = glGetUniformLocation(ShaderFrag.Get(), "TextureDiffuse");
    LocTextureNormalFrag = glGetUniformLocation(ShaderFrag.Get(), "TextureNormal");
    LocMatAmbient = glGetUniformLocation(ShaderFrag.Get(), "MatAmbient");
    LocMatSpecular = glGetUniformLocation(ShaderFrag.Get(), "MatSpecular");
    LocMatShininess = glGetUniformLocation(ShaderFrag.Get(), "MatShininess");

    if (LocTessLevelInner < 0 || LocTessLevelOuter < 0 || LocWorldViewProjection < 0)
      throw GraphicsException("Unable to locate uniform");

    AttribLink[0] = GLVertexAttribLink(glGetAttribLocation(ShaderVert.Get(), "Position"), vertexDeclaration.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    AttribLink[1] = GLVertexAttribLink(glGetAttribLocation(ShaderVert.Get(), "Normal"), vertexDeclaration.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    AttribLink[2] = GLVertexAttribLink(glGetAttribLocation(ShaderVert.Get(), "Tangent"), vertexDeclaration.VertexElementGetIndexOf(VertexElementUsage::Tangent, 0));
    AttribLink[3] = GLVertexAttribLink(glGetAttribLocation(ShaderVert.Get(), "TexCoord"), vertexDeclaration.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    GL_CHECK_FOR_ERROR();

    glUseProgram(0);
    Pipeline.Reset(true);
    GL_CHECK_FOR_ERROR();
  }


  void TessellationShader::UpdateUniforms(const RenderTessellationConfig& tessRenderConfig, const RenderCameraConfig& cameraConfig, const RenderMaterial& material,
                                          const int32_t activeTexDiffuseId, const int32_t activeTexNormalId, const int32_t activeTexDisplaceId)
  {
    if (LocTessLevelInner != GLValues::INVALID_LOCATION)
      glProgramUniform1f(ShaderControl.Get(), LocTessLevelInner, tessRenderConfig.TessLevelInner);

    if (LocTessLevelOuter != GLValues::INVALID_LOCATION)
      glProgramUniform1f(ShaderControl.Get(), LocTessLevelOuter, tessRenderConfig.TessLevelOuter);

    if (LocWorld != GLValues::INVALID_LOCATION)
      glProgramUniformMatrix4fv(ShaderEval.Get(), LocWorld, 1, GL_FALSE, cameraConfig.World.DirectAccess());

    if (LocWorldView != GLValues::INVALID_LOCATION)
      glProgramUniformMatrix4fv(ShaderEval.Get(), LocWorldView, 1, GL_FALSE, cameraConfig.WorldView.DirectAccess());

    if (LocWorldViewProjection != GLValues::INVALID_LOCATION)
      glProgramUniformMatrix4fv(ShaderEval.Get(), LocWorldViewProjection, 1, GL_FALSE, cameraConfig.WorldViewProjection.DirectAccess());

    if (LocNormalMatrix != GLValues::INVALID_LOCATION)
      glProgramUniformMatrix3fv(ShaderEval.Get(), LocNormalMatrix, 1, GL_FALSE, cameraConfig.NormalMatrix.DirectAccess());

    if (LocTextureDiffuse != GLValues::INVALID_LOCATION)
      glProgramUniform1i(ShaderFrag.Get(), LocTextureDiffuse, activeTexDiffuseId);

    if (LocTextureNormalEval != GLValues::INVALID_LOCATION)
      glProgramUniform1i(ShaderFrag.Get(), LocTextureNormalEval, activeTexNormalId);

    if (LocTextureNormalFrag != GLValues::INVALID_LOCATION)
      glProgramUniform1i(ShaderFrag.Get(), LocTextureNormalFrag, activeTexNormalId);

    if (LocTextureDisplacement != GLValues::INVALID_LOCATION)
      glProgramUniform1i(ShaderEval.Get(), LocTextureDisplacement, activeTexDisplaceId);

    if (LocLightDirection != GLValues::INVALID_LOCATION)
      glProgramUniform3fv(ShaderEval.Get(), LocLightDirection, 1, cameraConfig.CameraSpaceLightDirection.DirectAccess());

    if (LocDisplacementFactor != GLValues::INVALID_LOCATION)
      glProgramUniform1f(ShaderEval.Get(), LocDisplacementFactor, tessRenderConfig.DisplacementFactor);

    if (LocDisplacementMod != GLValues::INVALID_LOCATION)
      glProgramUniform1f(ShaderEval.Get(), LocDisplacementMod, tessRenderConfig.DisplacementMod);

    if (LocMatAmbient != GLValues::INVALID_LOCATION)
      glProgramUniform3fv(ShaderFrag.Get(), LocMatAmbient, 1, material.Ambient.DirectAccess());

    if (LocMatSpecular != GLValues::INVALID_LOCATION)
      glProgramUniform3fv(ShaderFrag.Get(), LocMatSpecular, 1, material.Specular.DirectAccess());

    if (LocMatShininess != GLValues::INVALID_LOCATION)
      glProgramUniform1f(ShaderFrag.Get(), LocMatShininess, material.Shininess);
  }

}
