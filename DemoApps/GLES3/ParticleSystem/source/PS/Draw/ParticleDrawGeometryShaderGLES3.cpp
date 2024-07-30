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

#include "ParticleDrawGeometryShaderGLES3.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include "../ParticleDrawContext.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    VertexDeclarationArray<2> GetVertexDeclarationArray(const uint32_t cbParticleRecord)
    {
      assert(cbParticleRecord >= sizeof(Particle));
      static std::array<VertexElement, 2> g_elements = {
        VertexElement(offsetof(Particle, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
        VertexElement(offsetof(Particle, Size), VertexElementFormat::Single, VertexElementUsage::PointSize, 0),
      };
      return {g_elements, cbParticleRecord};
    }

    GLES3::GLVertexBuffer CreateEmptyDynamicVertexBuffer(std::size_t capacity, const uint32_t cbParticleRecord)
    {
      const auto vertexDeclArray = GetVertexDeclarationArray(cbParticleRecord);
      return {nullptr, capacity, vertexDeclArray.AsReadOnlySpan(), GL_DYNAMIC_DRAW};
    }
  }

  ParticleDrawGeometryShaderGLES3::ParticleDrawGeometryShaderGLES3(const std::shared_ptr<IContentManager>& contentManager, const std::size_t capacity,
                                                                   const uint32_t cbParticleRecord)
    : m_cbParticleRecord(cbParticleRecord)
    , m_vertexBuffer(CreateEmptyDynamicVertexBuffer(capacity, cbParticleRecord))
    , m_pCurrentBuffer(&m_vertexBuffer)
    , m_pOtherBuffer(nullptr)
    , m_locViewProjectionMatrix(GLValues::InvalidLocation)
    , m_locWorldViewProjectionMatrix(GLValues::InvalidLocation)
  {
    Construct(contentManager);
  }


  ParticleDrawGeometryShaderGLES3::ParticleDrawGeometryShaderGLES3(const std::shared_ptr<IContentManager>& contentManager, const std::size_t capacity,
                                                                   const uint32_t cbParticleRecord, const bool useDoubleBuffering)
    : m_cbParticleRecord(cbParticleRecord)
    , m_vertexBuffer(CreateEmptyDynamicVertexBuffer(capacity, cbParticleRecord))
    , m_vertexBuffer2(CreateEmptyDynamicVertexBuffer(capacity, cbParticleRecord))
    , m_pCurrentBuffer(&m_vertexBuffer)
    , m_pOtherBuffer(useDoubleBuffering ? &m_vertexBuffer2 : nullptr)
    , m_locViewProjectionMatrix(GLValues::InvalidLocation)
    , m_locWorldViewProjectionMatrix(GLValues::InvalidLocation)
  {
    Construct(contentManager);
  }


  void ParticleDrawGeometryShaderGLES3::Draw(const ParticleDrawContext& context, const uint8_t* pParticles, const uint32_t particleCount,
                                             const uint32_t /*particleStride*/)
  {
    if (particleCount <= 0)
    {
      return;
    }

    // glDisable(GL_CULL_FACE);
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

    glBindBuffer(m_pCurrentBuffer->GetTarget(), m_pCurrentBuffer->Get());
    m_pCurrentBuffer->SetDataFast(0, pParticles, particleCount);
    m_pCurrentBuffer->EnableAttribArrays(m_particleAttribLink.data(), m_particleAttribLink.size());

    glDrawArrays(GL_POINTS, 0, UncheckedNumericCast<GLsizei>(particleCount));

    // Swap the buffers if double buffering is enabled
    if (m_pOtherBuffer != nullptr)
    {
      std::swap(m_pCurrentBuffer, m_pOtherBuffer);
    }

    m_pCurrentBuffer->DisableAttribArrays(m_particleAttribLink.data(), m_particleAttribLink.size());
    m_pipeline.BindClear();
  }


  void ParticleDrawGeometryShaderGLES3::Construct(const std::shared_ptr<IContentManager>& contentManager)
  {
    glUseProgram(0);

    std::string strVert = contentManager->ReadAllText("CPUParticlesGeometryShader.vert");
    std::string strFrag = contentManager->ReadAllText("CPUParticlesGeometryShader.frag");
    std::string strGeom = contentManager->ReadAllText("CPUParticlesGeometryShader.geom");

    {
      // GLShader shaderVert(GL_VERTEX_SHADER, strVert);
      // GLShader shaderFrag(GL_FRAGMENT_SHADER, strFrag);
      // GLShader shaderGeom(GL_GEOMETRY_SHADER_EXT, strGeom);
    }

    m_shaderVert.Reset(GL_VERTEX_SHADER, strVert);
    m_shaderGeom.Reset(GL_GEOMETRY_SHADER_EXT, strGeom);
    m_shaderFrag.Reset(GL_FRAGMENT_SHADER, strFrag);
    GL_CHECK_FOR_ERROR();

    const auto vertexDecl = GetVertexDeclarationArray(m_cbParticleRecord);

    m_particleAttribLink[0] = GLVertexAttribLink(glGetAttribLocation(m_shaderVert.Get(), "VertexPosition"),
                                                 vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    m_particleAttribLink[1] = GLVertexAttribLink(glGetAttribLocation(m_shaderVert.Get(), "VertexPointSize"),
                                                 vertexDecl.VertexElementGetIndexOf(VertexElementUsage::PointSize, 0));
    // m_particleAttribLink[2] = GLVertexAttribLink(glGetAttribLocation(m_shaderVert.Get(), "VertexColor"),
    // vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));

    m_locViewProjectionMatrix = glGetUniformLocation(m_shaderGeom.Get(), "WorldView");
    m_locWorldViewProjectionMatrix = glGetUniformLocation(m_shaderGeom.Get(), "WorldViewProjection");

    // if (m_locViewProjectionMatrix < 0 || m_locWorldViewProjectionMatrix < 0)
    //  throw NotSupportedException("The shader does not conform to the expected behavior");

    glUseProgram(0);
    m_pipeline.Reset(true);
    GL_CHECK_FOR_ERROR();
  }
}
