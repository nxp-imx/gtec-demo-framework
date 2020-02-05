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

#include "ParticleSystemGLES3.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Color.hpp>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include "ParticleGPU.hpp"
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
    void DumpDebugInformation(const GLuint hProgram)
    {
      GLint errorBufSize, errorLength;
      glGetProgramiv(hProgram, GL_INFO_LOG_LENGTH, &errorBufSize);

      std::vector<char> errorLog(std::max(errorBufSize, 1));
      errorLog[0] = 0;

      glGetProgramInfoLog(hProgram, static_cast<GLsizei>(errorLog.size()), &errorLength, &errorLog[0]);
      FSLLOG3_INFO("*** Error log start ***\n{}\n*** Error Log End ***\n\n", &errorLog[0]);
    }


    GLuint Build(const GLShader& vertexShader, const GLShader& fragmentShader, const std::function<void(GLuint)>& postCompilePreLinkCallback,
                 const std::shared_ptr<IContentManager>& contentManager)
    {
      if (vertexShader.GetType() != GL_VERTEX_SHADER)
      {
        throw std::invalid_argument("vertexShader should be of type GL_VERTEX_SHADER");
      }
      if (fragmentShader.GetType() != GL_FRAGMENT_SHADER)
      {
        throw std::invalid_argument("fragmentShader should be of type GL_FRAGMENT_SHADER");
      }

      GLuint handle = GL_CHECK(glCreateProgram());
      if (handle == 0)
      {
        throw GLESGraphicsException("Failed to create program", 0, __FILE__, __LINE__);
      }

      GLShader geomShader(GL_GEOMETRY_SHADER_EXT, contentManager->ReadAllText("PS_TransformFeedbackShader.geom"));

      GL_CHECK(glAttachShader(handle, vertexShader.Get()));
      GL_CHECK(glAttachShader(handle, geomShader.Get()));
      GL_CHECK(glAttachShader(handle, fragmentShader.Get()));


      if (postCompilePreLinkCallback)
      {
        postCompilePreLinkCallback(handle);
        GL_CHECK_FOR_ERROR();
      }

      // Link the vertex shader and fragment shader together
      GL_CHECK(glLinkProgram(handle));

      // Release the shaders from the program now that we are linked
      GL_CHECK(glDetachShader(handle, vertexShader.Get()));
      GL_CHECK(glDetachShader(handle, fragmentShader.Get()));

      // GL_CHECK(glDetachShader(handle, geomShader.Get()));

      // Check for link success
      GLint nLinkResult = 0;
      glGetProgramiv(handle, GL_LINK_STATUS, &nLinkResult);
      if (0 == nLinkResult)
      {
        DumpDebugInformation(handle);
        throw GLESGraphicsException("Failed to link program", 0, __FILE__, __LINE__);
      }
      return handle;
    }

    void SeedParticles(std::vector<ParticleGPU>& rParticles)
    {
      std::mt19937 random;

      std::uniform_real_distribution<float> randomPositionX(-100, 100);
      std::uniform_real_distribution<float> randomPositionY(-100, 100);
      std::uniform_real_distribution<float> randomPositionZ(-100, 100);
      std::uniform_real_distribution<float> randomVelocityY(8.0f, 20.00f);

      for (auto itr = rParticles.begin(); itr != rParticles.end(); ++itr)
      {
        itr->Position = Vector3(randomPositionX(random), randomPositionY(random), randomPositionZ(random));
        itr->Velocity = Vector3(0, randomVelocityY(random), 0);
      }
    }


    void GenerateRandomTexture(GLTexture& rTexture, const int32_t width, const BitmapOrigin bitmapOrigin)
    {
      std::mt19937 random;
      Bitmap bitmap(width, 1, PixelFormat::R8G8B8A8_UNORM);

      std::uniform_int_distribution<uint16_t> randomVal(0, 255);

      for (uint32_t i = 0; i < bitmap.Width(); ++i)
      {
        auto r = static_cast<uint8_t>(randomVal(random));
        auto g = static_cast<uint8_t>(randomVal(random));
        auto b = static_cast<uint8_t>(randomVal(random));
        auto a = static_cast<uint8_t>(randomVal(random));
        bitmap.SetNativePixel(i, 0u, Color(r, g, b, a).PackedValue());
      }
      GLTextureParameters texParams(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      rTexture.Reset(bitmap, texParams);
    }


    void PostCompilePreLinkCallback(const GLuint hProgram)
    {
      const char* particleAttribLinkFeedback[] = {"block.ParticlePosition", "block.ParticleVelocity", "block.ParticleEnergy", "block.ParticleType"};
      glTransformFeedbackVaryings(hProgram, 4, particleAttribLinkFeedback, GL_INTERLEAVED_ATTRIBS);

      GL_CHECK_FOR_ERROR();
    }
  }


  ParticleSystemGLES3::ParticleSystemGLES3(const uint32_t capacity, const std::shared_ptr<IContentManager>& contentManager)
    : m_primitiveCount(capacity)
    , m_vertexBuffer2(nullptr, capacity, ParticleGPU::GetVertexDeclaration(), GL_STREAM_DRAW)
    , m_pCurrentVertexBuffer(&m_vertexBuffer1)
    , m_pOtherVertexBuffer(&m_vertexBuffer2)
    , m_transformFeedbackObject(0)
    , m_transformFeedbackQuery(0)
    , m_locFeedbackDeltaTime(GLValues::INVALID_LOCATION)
    , m_locViewProjectionMatrix(GLValues::INVALID_LOCATION)
    , m_locWorldViewProjectionMatrix(GLValues::INVALID_LOCATION)
  {
    GLShader vertShader(GL_VERTEX_SHADER, contentManager->ReadAllText("PS_TransformFeedbackShader.vert"));
    GLShader fragShader(GL_FRAGMENT_SHADER, contentManager->ReadAllText("PS_TransformFeedbackShader.frag"));
    auto hProg = Build(vertShader, fragShader, PostCompilePreLinkCallback, contentManager);
    m_programTransform.Reset(hProg);

    std::vector<ParticleGPU> particles(capacity);
    SeedParticles(particles);
    // particles[0].Type = static_cast<float>(ParticleGPUType::Emitter);
    // particles[0].Velocity = Vector3();
    // particles[0].Energy = 100;
    m_vertexBuffer1.Reset(particles, GL_STREAM_DRAW);
    // m_primitiveCount = 1;

    {
      const auto hProgram = m_programTransform.Get();
      const auto vertexDecl = ParticleGPU::GetVertexDeclaration();
      m_particleAttribLinkFeedback[0] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "ParticlePosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
      m_particleAttribLinkFeedback[1] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "ParticleVelocity"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Custom, 0));
      m_particleAttribLinkFeedback[2] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "ParticleEnergy"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Custom, 1));
      m_particleAttribLinkFeedback[3] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "ParticleType"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Custom, 2));

      m_locFeedbackDeltaTime = glGetUniformLocation(hProgram, "DeltaTime");
    }


    GL_CHECK(glGenTransformFeedbacks(1, &m_transformFeedbackObject));
    GL_CHECK(glGenQueries(1, &m_transformFeedbackQuery));

    GenerateRandomTexture(m_texRandom, 256, contentManager->GetPreferredBitmapOrigin());


    ConstructRenderShader(contentManager);
  }


  ParticleSystemGLES3::~ParticleSystemGLES3()
  {
    glDeleteQueries(1, &m_transformFeedbackQuery);
    glDeleteTransformFeedbacks(1, &m_transformFeedbackObject);
  }


  void ParticleSystemGLES3::Update(const DemoTime& demoTime)
  {
    // Advance the simulation using the GPU :)
    glEnable(GL_RASTERIZER_DISCARD);
    glBindBuffer(m_pCurrentVertexBuffer->GetTarget(), m_pCurrentVertexBuffer->Get());
    m_pCurrentVertexBuffer->EnableAttribArrays(m_particleAttribLinkFeedback, sizeof(m_particleAttribLinkFeedback) / sizeof(GLVertexAttribLink));

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedbackObject);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_pOtherVertexBuffer->Get());

    glUseProgram(m_programTransform.Get());
    glUniform1f(m_locFeedbackDeltaTime, demoTime.DeltaTime);

    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_transformFeedbackQuery);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, m_primitiveCount);
    glEndTransformFeedback();
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

    m_pCurrentVertexBuffer->DisableAttribArrays();
    glDisable(GL_RASTERIZER_DISCARD);

    std::swap(m_pCurrentVertexBuffer, m_pOtherVertexBuffer);


    GLuint numPrimitives = 0;
    glGetQueryObjectuiv(m_transformFeedbackQuery, GL_QUERY_RESULT, &numPrimitives);

    m_primitiveCount = numPrimitives;

    // FSLLOG3_INFO("PrimitiveCount: " << m_primitiveCount);
  }


  void ParticleSystemGLES3::Draw(const ParticleDrawContext& context)
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

    glBindBuffer(m_pCurrentVertexBuffer->GetTarget(), m_pCurrentVertexBuffer->Get());
    m_pCurrentVertexBuffer->EnableAttribArrays(m_particleAttribLink, 1);

    glDrawArrays(GL_POINTS, 0, m_primitiveCount);

    m_pCurrentVertexBuffer->DisableAttribArrays();
    m_pipeline.BindClear();
  }


  void ParticleSystemGLES3::ConstructRenderShader(const std::shared_ptr<IContentManager>& contentManager)
  {
    glUseProgram(0);

    std::string strVert = contentManager->ReadAllText("PS_Render.vert");
    std::string strFrag = contentManager->ReadAllText("PS_Render.frag");
    std::string strGeom = contentManager->ReadAllText("PS_Render.geom");

    {
      // GLShader shaderVert(GL_VERTEX_SHADER, strVert);
      // GLShader shaderFrag(GL_FRAGMENT_SHADER, strFrag);
      // GLShader shaderGeom(GL_GEOMETRY_SHADER_EXT, strGeom);
    }

    m_shaderVert.Reset(GL_VERTEX_SHADER, strVert);
    m_shaderGeom.Reset(GL_GEOMETRY_SHADER_EXT, strGeom);
    m_shaderFrag.Reset(GL_FRAGMENT_SHADER, strFrag);
    GL_CHECK_FOR_ERROR();

    auto vertexDecl = ParticleGPU::GetVertexDeclaration();

    m_particleAttribLink[0] = GLVertexAttribLink(glGetAttribLocation(m_shaderVert.Get(), "ParticlePosition"),
                                                 vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));

    m_locViewProjectionMatrix = glGetUniformLocation(m_shaderGeom.Get(), "WorldView");
    m_locWorldViewProjectionMatrix = glGetUniformLocation(m_shaderGeom.Get(), "WorldViewProjection");

    // if (m_locViewProjectionMatrix < 0 || m_locWorldViewProjectionMatrix < 0)
    //  throw NotSupportedException("The shader does not conform to the expected behavior");

    glUseProgram(0);
    m_pipeline.Reset(true);
    GL_CHECK_FOR_ERROR();
  }
}
