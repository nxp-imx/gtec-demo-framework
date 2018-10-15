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

#include "ParticleDrawPointsGLES3.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <algorithm>
#include <cassert>
#include "../ParticleDrawContext.hpp"
#include <cstddef>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    VertexDeclaration GetVertexDeclaration(const uint32_t cbParticleRecord)
    {
      assert(cbParticleRecord >= sizeof(Particle));
      static VertexElementEx elements[] = {
        VertexElementEx(offsetof(Particle, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
        VertexElementEx(offsetof(Particle, Size), VertexElementFormat::Single, VertexElementUsage::PointSize, 0),
      };
      return VertexDeclaration(elements, sizeof(elements) / sizeof(VertexElementEx), cbParticleRecord);
    }
  }

  ParticleDrawPointsGLES3::ParticleDrawPointsGLES3(const std::shared_ptr<IContentManager>& contentManager, const std::size_t capacity,
                                                   const uint32_t cbParticleRecord)
    : m_cbParticleRecord(cbParticleRecord)
    , m_vertexBuffer(nullptr, capacity, GetVertexDeclaration(cbParticleRecord), GL_DYNAMIC_DRAW)
    , m_pCurrentBuffer(&m_vertexBuffer)
    , m_pOtherBuffer(nullptr)
    , m_locWorldViewProjectionMatrix(GLValues::INVALID_LOCATION)
    , m_locResolutionMod(GLValues::INVALID_LOCATION)
  {
    Construct(contentManager);
  }


  ParticleDrawPointsGLES3::ParticleDrawPointsGLES3(const std::shared_ptr<IContentManager>& contentManager, const std::size_t capacity,
                                                   const uint32_t cbParticleRecord, const bool useDoubleBuffering)
    : m_cbParticleRecord(cbParticleRecord)
    , m_vertexBuffer(nullptr, capacity, GetVertexDeclaration(cbParticleRecord), GL_DYNAMIC_DRAW)
    , m_vertexBuffer2(nullptr, capacity, GetVertexDeclaration(cbParticleRecord), GL_DYNAMIC_DRAW)
    , m_pCurrentBuffer(&m_vertexBuffer)
    , m_pOtherBuffer(useDoubleBuffering ? &m_vertexBuffer2 : nullptr)
    , m_locWorldViewProjectionMatrix(GLValues::INVALID_LOCATION)
    , m_locResolutionMod(GLValues::INVALID_LOCATION)
  {
    Construct(contentManager);
  }


  void ParticleDrawPointsGLES3::Draw(const ParticleDrawContext& context, const uint8_t* pParticles, const uint32_t particleCount,
                                     const uint32_t particleStride)
  {
    const GLuint hProgram = m_program.Get();
    // Set the shader program
    glUseProgram(hProgram);
    // Load the matrices
    glUniformMatrix4fv(m_locWorldViewProjectionMatrix, 1, 0, context.MatrixWorldViewProjection.DirectAccess());
    glUniform1f(m_locResolutionMod, context.ScreenAspectRatio);

    glBindBuffer(m_pCurrentBuffer->GetTarget(), m_pCurrentBuffer->Get());
    m_pCurrentBuffer->SetDataFast(0, pParticles, particleCount);
    m_pCurrentBuffer->EnableAttribArrays(m_particleAttribLink.data(), m_particleAttribLink.size());

    glDrawArrays(GL_POINTS, 0, particleCount);

    m_pCurrentBuffer->DisableAttribArrays(m_particleAttribLink.data(), m_particleAttribLink.size());

    // Swap the buffers if double buffering is enabled
    if (m_pOtherBuffer != nullptr)
    {
      std::swap(m_pCurrentBuffer, m_pOtherBuffer);
    }
  }


  void ParticleDrawPointsGLES3::Construct(const std::shared_ptr<IContentManager>& contentManager)
  {
    m_program.Reset(contentManager->ReadAllText("ShaderPoint.vert"), contentManager->ReadAllText("ShaderPoint.frag"));

    const GLuint hProgram = m_program.Get();
    auto vertexDecl = GetVertexDeclaration(m_cbParticleRecord);

    m_particleAttribLink[0] =
      GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    m_particleAttribLink[1] =
      GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexPointSize"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::PointSize, 0));

    m_locWorldViewProjectionMatrix = glGetUniformLocation(hProgram, "WorldViewProjection");
    m_locResolutionMod = glGetUniformLocation(hProgram, "ResolutionMod");

    if (m_locWorldViewProjectionMatrix < 0 || m_locResolutionMod < 0)
    {
      throw NotSupportedException("The shader does not conform to the expected behavior");
    }
  }
}
