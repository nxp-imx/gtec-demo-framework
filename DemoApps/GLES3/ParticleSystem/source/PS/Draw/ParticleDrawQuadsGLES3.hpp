#ifndef PS_DRAW_GLES3_PARTICLESYSTEM_PARTICLEDRAWQUADSGLES3_HPP
#define PS_DRAW_GLES3_PARTICLESYSTEM_PARTICLEDRAWQUADSGLES3_HPP
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

#include "IParticleDraw.hpp"
#include "../Particle.hpp"
#include <FslGraphics/Vertices/VertexPositionColorNormalTexture.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <array>
#include <memory>
#include <vector>

namespace Fsl
{
  struct DemoTime;
  class IContentManager;

  class ParticleDrawQuadsGLES3 : public IParticleDraw
  {
  public:
    using TVertex = VertexPositionColorNormalTexture;

  private:
    std::vector<TVertex> m_buffer;
    GLES3::GLProgram m_program;
    std::array<GLES3::GLVertexAttribLink, 4> m_particleAttribLink;
    GLES3::GLVertexBuffer m_vertexBuffer;
    GLES3::GLVertexBuffer m_vertexBuffer2;
    GLES3::GLVertexBuffer* m_pCurrentBuffer;
    GLES3::GLVertexBuffer* m_pOtherBuffer;
    GLint m_locWorldViewProjectionMatrix;
    GLint m_locWorldViewMatrix;
    GLint m_locProjMatrix;

  public:
    ParticleDrawQuadsGLES3(const std::shared_ptr<IContentManager>& contentManager, const std::size_t capacity);
    ParticleDrawQuadsGLES3(const std::shared_ptr<IContentManager>& contentManager, const std::size_t capacity, const bool useDoubleBuffering);

    void Draw(const ParticleDrawContext& context, const uint8_t* pParticles, const uint32_t particleCount, const uint32_t particleStride) override;

  private:
    void Construct(const std::shared_ptr<IContentManager>& contentManager);
  };
}

#endif
