#ifndef GLES3_TESSELLATIONSAMPLE_TESSELLATIONSHADER_HPP
#define GLES3_TESSELLATIONSAMPLE_TESSELLATIONSHADER_HPP
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

#include <FslBase/Noncopyable.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLVertexAttribLink.hpp>
#include <FslUtil/OpenGLES3_1/GLProgramPipeline.hpp>
#include <FslUtil/OpenGLES3_1/GLShaderProgram.hpp>
#include <memory>
#include <vector>
#include "RenderCameraConfig.hpp"
#include "RenderMaterial.hpp"
#include "RenderTessellationConfig.hpp"

namespace Fsl
{
  struct Matrix;
  struct Matrix3;
  struct Vector3;
  struct Vector4;
  class IContentManager;
  class VertexDeclaration;

  class TessellationShader : private Noncopyable
  {
  public:
    GLES3::GLProgramPipeline Pipeline;
    GLES3::GLShaderProgram ShaderVert;
    GLES3::GLShaderProgram ShaderControl;
    GLES3::GLShaderProgram ShaderEval;
    GLES3::GLShaderProgram ShaderFrag;
    GLES3::GLShaderProgram ShaderGeomWireframe;
    GLint LocTessLevelInner;
    GLint LocTessLevelOuter;
    GLint LocWorld;
    GLint LocWorldView;
    GLint LocWorldViewProjection;
    GLint LocNormalMatrix;
    GLint LocTextureDiffuse;
    GLint LocTextureNormalEval;
    GLint LocTextureNormalFrag;
    GLint LocTextureDisplacement;
    GLint LocLightDirection;
    GLint LocDisplacementFactor;
    GLint LocDisplacementMod;
    GLint LocMatAmbient;
    GLint LocMatSpecular;
    GLint LocMatShininess;

    std::vector<GLES3::GLVertexAttribLink> AttribLink;

    TessellationShader(const std::shared_ptr<IContentManager>& contentManager, const VertexDeclaration& vertexDeclaration);

    void UpdateUniforms(const RenderTessellationConfig& tessRenderConfig, const RenderCameraConfig& cameraConfig, const RenderMaterial& material,
                        const int32_t activeTexDiffuseId, const int32_t activeTexNormalId, const int32_t activeTexDisplaceId);
  };
}

#endif
