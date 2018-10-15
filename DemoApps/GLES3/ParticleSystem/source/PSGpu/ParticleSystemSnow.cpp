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

#include "ParticleSystemSnow.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include "ParticleSnowGPU.hpp"
#include <random>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    void SeedParticles(std::vector<ParticleSnowGPU>& rParticles, const Vector3& ranges)
    {
      std::mt19937 random;

      std::uniform_real_distribution<float> randomPositionX(-ranges.X, ranges.X);
      std::uniform_real_distribution<float> randomPositionY(-ranges.Y, ranges.Y);
      std::uniform_real_distribution<float> randomPositionZ(-ranges.Z, ranges.Z);
      std::uniform_real_distribution<float> randomVelocityY(-4.0f, -2.00f);

      for (auto itr = rParticles.begin(); itr != rParticles.end(); ++itr)
      {
        itr->Position = Vector3(randomPositionX(random), randomPositionY(random), randomPositionZ(random));
        itr->Velocity = Vector3(0, randomVelocityY(random), 0);
      }
    }

    void PostCompilePreLinkCallback(const GLuint hProgram)
    {
      const char* particleAttribLinkFeedback[] = {"Out_ParticlePosition", "Out_ParticleVelocity", "Out_ParticleEnergy"};
      glTransformFeedbackVaryings(hProgram, 3, particleAttribLinkFeedback, GL_INTERLEAVED_ATTRIBS);

      GL_CHECK_FOR_ERROR();
    }
  }


  ParticleSystemSnow::ParticleSystemSnow(const uint32_t capacity, const std::shared_ptr<IContentManager>& contentManager, const Vector3& ranges,
                                         const float pointSize)
    : m_capacity(capacity)
    , m_ranges(ranges)
    , m_pointSize(pointSize)
    , m_programTransform(contentManager->ReadAllText("PSSnow_TransformFeedbackShader.vert"),
                         contentManager->ReadAllText("PSSnow_TransformFeedbackShader.frag"), PostCompilePreLinkCallback)
    , m_vertexBuffer2(nullptr, capacity, ParticleSnowGPU::GetVertexDeclaration(), GL_STREAM_DRAW)
    , m_pCurrentVertexBuffer(&m_vertexBuffer1)
    , m_pOtherVertexBuffer(&m_vertexBuffer2)
    , m_transformFeedbackObject(0)
    , m_transformFeedbackQuery(0)
    , m_locFeedbackDeltaTime(GLValues::INVALID_LOCATION)
    , m_locFeedbackUpperBoundaryY(GLValues::INVALID_LOCATION)
    , m_locFeedbackLowerBoundaryY(GLValues::INVALID_LOCATION)
    , m_locViewProjectionMatrix(GLValues::INVALID_LOCATION)
    , m_locWorldViewProjectionMatrix(GLValues::INVALID_LOCATION)
    , m_locPointSize(GLValues::INVALID_LOCATION)
  {
    std::vector<ParticleSnowGPU> particles(capacity);
    SeedParticles(particles, ranges);
    m_vertexBuffer1.Reset(particles, GL_STREAM_DRAW);

    {
      const auto hProgram = m_programTransform.Get();
      const auto vertexDecl = ParticleSnowGPU::GetVertexDeclaration();
      m_particleAttribLinkFeedback[0] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "ParticlePosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
      m_particleAttribLinkFeedback[1] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "ParticleVelocity"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Custom, 0));
      m_particleAttribLinkFeedback[2] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "ParticleEnergy"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Custom, 1));

      m_locFeedbackDeltaTime = glGetUniformLocation(hProgram, "DeltaTime");
      m_locFeedbackUpperBoundaryY = glGetUniformLocation(hProgram, "UpperBoundaryY");
      m_locFeedbackLowerBoundaryY = glGetUniformLocation(hProgram, "LowerBoundaryY");
    }

    GL_CHECK(glGenTransformFeedbacks(1, &m_transformFeedbackObject));
    GL_CHECK(glGenQueries(1, &m_transformFeedbackQuery));

    ConstructRenderShader(contentManager);
  }


  ParticleSystemSnow::~ParticleSystemSnow()
  {
    glDeleteQueries(1, &m_transformFeedbackQuery);
    glDeleteTransformFeedbacks(1, &m_transformFeedbackObject);
  }


  void ParticleSystemSnow::Update(const DemoTime& demoTime)
  {
    // Advance the simulation using the GPU :)
    glEnable(GL_RASTERIZER_DISCARD);
    glBindBuffer(m_pCurrentVertexBuffer->GetTarget(), m_pCurrentVertexBuffer->Get());
    m_pCurrentVertexBuffer->EnableAttribArrays(m_particleAttribLinkFeedback, 3);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedbackObject);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_pOtherVertexBuffer->Get());

    glUseProgram(m_programTransform.Get());
    glUniform1f(m_locFeedbackDeltaTime, demoTime.DeltaTime);
    glUniform1f(m_locFeedbackUpperBoundaryY, m_ranges.Y);
    glUniform1f(m_locFeedbackLowerBoundaryY, -m_ranges.Y);

    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_transformFeedbackQuery);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, m_capacity);
    glEndTransformFeedback();
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

    m_pCurrentVertexBuffer->DisableAttribArrays();
    glDisable(GL_RASTERIZER_DISCARD);

    std::swap(m_pCurrentVertexBuffer, m_pOtherVertexBuffer);
  }


  void ParticleSystemSnow::Draw(const ParticleDrawContext& context)
  {
    glEnable(GL_CULL_FACE);

    glUseProgram(0);

    m_pipeline.Bind();
    // configure it
    m_pipeline.UseProgramStages(m_shaderVert, GL_VERTEX_SHADER_BIT);
    m_pipeline.UseProgramStages(m_shaderGeom, GL_GEOMETRY_SHADER_BIT_EXT, true);
    m_pipeline.UseProgramStages(m_shaderFrag, GL_FRAGMENT_SHADER_BIT);

    // Load the matrices
    if (m_locViewProjectionMatrix >= 0)
    {
      glProgramUniformMatrix4fv(m_shaderGeom.Get(), m_locViewProjectionMatrix, 1, GL_FALSE, context.MatrixWorldView.DirectAccess());
    }
    if (m_locWorldViewProjectionMatrix >= 0)
    {
      glProgramUniformMatrix4fv(m_shaderGeom.Get(), m_locWorldViewProjectionMatrix, 1, GL_FALSE, context.MatrixWorldViewProjection.DirectAccess());
    }
    if (m_locPointSize >= 0)
    {
      glProgramUniform1f(m_shaderGeom.Get(), m_locPointSize, m_pointSize);
    }

    glBindBuffer(m_pCurrentVertexBuffer->GetTarget(), m_pCurrentVertexBuffer->Get());
    m_pCurrentVertexBuffer->EnableAttribArrays(m_particleAttribLink, 1);

    glDrawArrays(GL_POINTS, 0, m_capacity);

    m_pCurrentVertexBuffer->DisableAttribArrays();
    m_pipeline.BindClear();
  }


  void ParticleSystemSnow::ConstructRenderShader(const std::shared_ptr<IContentManager>& contentManager)
  {
    glUseProgram(0);

    std::string strVert = contentManager->ReadAllText("PSSnow_Render.vert");
    std::string strFrag = contentManager->ReadAllText("PSSnow_Render.frag");
    std::string strGeom = contentManager->ReadAllText("PSSnow_Render.geom");

    {
      // GLShader shaderVert(GL_VERTEX_SHADER, strVert);
      // GLShader shaderFrag(GL_FRAGMENT_SHADER, strFrag);
      // GLShader shaderGeom(GL_GEOMETRY_SHADER_EXT, strGeom);
    }

    m_shaderVert.Reset(GL_VERTEX_SHADER, strVert);
    m_shaderGeom.Reset(GL_GEOMETRY_SHADER_EXT, strGeom);
    m_shaderFrag.Reset(GL_FRAGMENT_SHADER, strFrag);
    GL_CHECK_FOR_ERROR();

    auto vertexDecl = ParticleSnowGPU::GetVertexDeclaration();

    m_particleAttribLink[0] = GLVertexAttribLink(glGetAttribLocation(m_shaderVert.Get(), "ParticlePosition"),
                                                 vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));

    m_locViewProjectionMatrix = glGetUniformLocation(m_shaderGeom.Get(), "WorldView");
    m_locWorldViewProjectionMatrix = glGetUniformLocation(m_shaderGeom.Get(), "WorldViewProjection");
    m_locPointSize = glGetUniformLocation(m_shaderGeom.Get(), "PointSize");

    // if (m_locViewProjectionMatrix < 0 || m_locWorldViewProjectionMatrix < 0)
    //  throw NotSupportedException("The shader does not conform to the expected behavior");

    glUseProgram(0);
    m_pipeline.Reset(true);
    GL_CHECK_FOR_ERROR();
  }
}
