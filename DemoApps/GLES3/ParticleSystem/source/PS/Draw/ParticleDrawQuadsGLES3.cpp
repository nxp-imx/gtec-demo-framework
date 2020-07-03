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

#include "ParticleDrawQuadsGLES3.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <algorithm>
#include <cassert>
#include "../ParticleDrawContext.hpp"
#include <cstddef>

namespace Fsl
{
  using namespace GLES3;

  ParticleDrawQuadsGLES3::ParticleDrawQuadsGLES3(const std::shared_ptr<IContentManager>& contentManager, const std::size_t capacity)
    : m_buffer(capacity * 6)
    , m_vertexBuffer(m_buffer, GL_DYNAMIC_DRAW)
    , m_pCurrentBuffer(&m_vertexBuffer)
    , m_pOtherBuffer(nullptr)
    , m_locWorldViewProjectionMatrix(GLValues::INVALID_LOCATION)
    , m_locWorldViewMatrix(GLValues::INVALID_LOCATION)
    , m_locProjMatrix(GLValues::INVALID_LOCATION)
  {
    Construct(contentManager);
  }


  ParticleDrawQuadsGLES3::ParticleDrawQuadsGLES3(const std::shared_ptr<IContentManager>& contentManager, const std::size_t capacity,
                                                 const bool useDoubleBuffering)
    : m_buffer(capacity * 6)
    , m_vertexBuffer(m_buffer, GL_DYNAMIC_DRAW)
    , m_vertexBuffer2(m_buffer, GL_DYNAMIC_DRAW)
    , m_pCurrentBuffer(&m_vertexBuffer)
    , m_pOtherBuffer(useDoubleBuffering ? &m_vertexBuffer2 : nullptr)
    , m_locWorldViewProjectionMatrix(GLValues::INVALID_LOCATION)
    , m_locWorldViewMatrix(GLValues::INVALID_LOCATION)
    , m_locProjMatrix(GLValues::INVALID_LOCATION)
  {
    Construct(contentManager);
  }


  void ParticleDrawQuadsGLES3::Draw(const ParticleDrawContext& context, const uint8_t* pParticles, const uint32_t particleCount,
                                    const uint32_t particleStride)
  {
    const GLuint hProgram = m_program.Get();
    // Set the shader program
    glUseProgram(hProgram);
    // Load the matrices
    glUniformMatrix4fv(m_locWorldViewProjectionMatrix, 1, 0, context.MatrixWorldViewProjection.DirectAccess());
    glUniformMatrix4fv(m_locWorldViewMatrix, 1, 0, context.MatrixWorldView.DirectAccess());
    glUniformMatrix4fv(m_locProjMatrix, 1, 0, context.MatrixProjection.DirectAccess());

    Vector4 col = Color::White().ToVector4();

    // HighResolutionTimer timer;
    // auto start = timer.GetTime();

    // 0 3
    // |\|
    // 1 2

    TVertex* pDst = m_buffer.data();
    const uint32_t count = particleCount * 6;

    const Vector2 t0(0, 1);
    const Vector2 t1(0, 0);
    const Vector2 t2(1, 0);
    const Vector2 t3(0, 1);
    const Vector2 t4(1, 0);
    const Vector2 t5(1, 1);

    for (uint32_t i = 0; i < count; i += 6)
    {
      const auto* pSrcParticle = reinterpret_cast<const Particle*>(pParticles);

      pDst[i + 0].Position = pSrcParticle->Position;
      pDst[i + 1].Position = pSrcParticle->Position;
      pDst[i + 2].Position = pSrcParticle->Position;
      pDst[i + 3].Position = pSrcParticle->Position;
      pDst[i + 4].Position = pSrcParticle->Position;
      pDst[i + 5].Position = pSrcParticle->Position;

      pDst[i + 0].TextureCoordinate = t0;
      pDst[i + 1].TextureCoordinate = t1;
      pDst[i + 2].TextureCoordinate = t2;
      pDst[i + 3].TextureCoordinate = t3;
      pDst[i + 4].TextureCoordinate = t4;
      pDst[i + 5].TextureCoordinate = t5;

      pDst[i + 0].Normal.Z = pSrcParticle->Size;
      pDst[i + 1].Normal.Z = pSrcParticle->Size;
      pDst[i + 2].Normal.Z = pSrcParticle->Size;
      pDst[i + 3].Normal.Z = pSrcParticle->Size;
      pDst[i + 4].Normal.Z = pSrcParticle->Size;
      pDst[i + 5].Normal.Z = pSrcParticle->Size;

      pDst[i + 0].Color = col;
      pDst[i + 1].Color = col;
      pDst[i + 2].Color = col;
      pDst[i + 3].Color = col;
      pDst[i + 4].Color = col;
      pDst[i + 5].Color = col;

      pParticles += particleStride;
    }

    // auto end = timer.GetTime();
    // FSLLOG3_INFO("Particles-ToVertices Time: " << end - start);

    glBindBuffer(m_pCurrentBuffer->GetTarget(), m_pCurrentBuffer->Get());
    m_pCurrentBuffer->SetDataFast(0, m_buffer.data(), particleCount * 6);
    m_pCurrentBuffer->EnableAttribArrays(m_particleAttribLink);

    glDrawArrays(GL_TRIANGLES, 0, particleCount * 6);

    // Swap the buffers if double buffering is enabled
    if (m_pOtherBuffer != nullptr)
    {
      std::swap(m_pCurrentBuffer, m_pOtherBuffer);
    }
  }

  void ParticleDrawQuadsGLES3::Construct(const std::shared_ptr<IContentManager>& contentManager)
  {
    // prepare the particle program
    m_program.Reset(contentManager->ReadAllText("ShaderSphereBillboard.vert"), contentManager->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();
    auto vertexDecl = TVertex::GetVertexDeclaration();
    m_particleAttribLink[0] =
      GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    m_particleAttribLink[1] =
      GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexColor"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
    m_particleAttribLink[2] =
      GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexNormal"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    m_particleAttribLink[3] = GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexTexCoord"),
                                                 vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));

    m_locWorldViewProjectionMatrix = glGetUniformLocation(hProgram, "WorldViewProjection");
    m_locWorldViewMatrix = glGetUniformLocation(hProgram, "WorldView");
    m_locProjMatrix = glGetUniformLocation(hProgram, "Projection");

    for (uint32_t i = 0; i < m_buffer.size(); i += 6)
    {
      m_buffer[i + 0].Normal = Vector3(-0.5f, +0.5f, 1.0f);
      m_buffer[i + 1].Normal = Vector3(-0.5f, -0.5f, 1.0f);
      m_buffer[i + 2].Normal = Vector3(+0.5f, -0.5f, 1.0f);
      m_buffer[i + 3].Normal = Vector3(-0.5f, +0.5f, 1.0f);
      m_buffer[i + 4].Normal = Vector3(+0.5f, -0.5f, 1.0f);
      m_buffer[i + 5].Normal = Vector3(+0.5f, +0.5f, 1.0f);
    }
  }
}
