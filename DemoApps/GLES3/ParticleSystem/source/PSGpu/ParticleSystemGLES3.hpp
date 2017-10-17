#ifndef PSGPU_GLES3_PARTICLESYSTEM_PARTICLESYSTEMGLES3_HPP
#define PSGPU_GLES3_PARTICLESYSTEM_PARTICLESYSTEMGLES3_HPP
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

#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES3_1/GLProgramPipeline.hpp>
#include <FslUtil/OpenGLES3_1/GLShaderProgram.hpp>

#include <cstddef>
#include <memory>
#include "../PS/ParticleDrawContext.hpp"

namespace Fsl
{
  class IContentManager;
  struct DemoTime;

  class ParticleSystemGLES3
  {
    uint32_t m_primitiveCount;
    GLES3::GLTexture m_texRandom;
    GLES3::GLProgram m_programTransform;
    GLES3::GLVertexBuffer m_vertexBuffer1;
    GLES3::GLVertexBuffer m_vertexBuffer2;
    GLES3::GLVertexBuffer* m_pCurrentVertexBuffer;
    GLES3::GLVertexBuffer* m_pOtherVertexBuffer;
    GLuint m_transformFeedbackObject;
    GLuint m_transformFeedbackQuery;
    GLint m_locFeedbackDeltaTime;
    GLES3::GLVertexAttribLink m_particleAttribLinkFeedback[4];

    GLES3::GLProgramPipeline m_pipeline;
    GLES3::GLShaderProgram m_shaderVert;
    GLES3::GLShaderProgram m_shaderFrag;
    GLES3::GLShaderProgram m_shaderGeom;
    GLES3::GLVertexAttribLink m_particleAttribLink[1];
    GLint m_locViewProjectionMatrix;
    GLint m_locWorldViewProjectionMatrix;
  public:
    ParticleSystemGLES3(const uint32_t capacity, const std::shared_ptr<IContentManager>& contentManager);
    ~ParticleSystemGLES3();

    uint32_t GetParticleCount() const { return m_primitiveCount; }

    void Update(const DemoTime& demoTime);
    void Draw(const ParticleDrawContext& context);
  private:
    void ConstructRenderShader(const std::shared_ptr<IContentManager>& contentManager);
  };
}

#endif
