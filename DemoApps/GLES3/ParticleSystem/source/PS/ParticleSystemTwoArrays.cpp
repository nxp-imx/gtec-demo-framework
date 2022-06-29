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

#include "ParticleSystemTwoArrays.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>

namespace Fsl
{
  ParticleSystemTwoArrays::ParticleSystemTwoArrays(std::shared_ptr<IParticleDraw> particleDraw, const std::size_t capacity)
    : m_particles1(capacity)
    , m_particles2(capacity)
    , m_pCurrent(m_particles1.data())
    , m_pOld(m_particles2.data())
    , m_particleDraw(std::move(particleDraw))
    , m_particleCount(0)
  {
  }


  uint32_t ParticleSystemTwoArrays::GetParticleCount() const
  {
    return m_particleCount;
  }


  void ParticleSystemTwoArrays::AddEmitter(const std::shared_ptr<IParticleEmitter>& emitter)
  {
    m_emitters.push_back(emitter);
  }


  void ParticleSystemTwoArrays::Update(const DemoTime& demoTime)
  {
    // Allow the emitters to create new particles
    // BEWARE that this allows the emitters to call the 'AddParticles' functions
    for (auto itr = m_emitters.begin(); itr != m_emitters.end(); ++itr)
    {
      (*itr)->Update(*this, demoTime);
    }


    // HighResolutionTimer timer;
    // auto start = timer.GetTime();

    const float deltaTime = demoTime.DeltaTime;
    Vector3 accumulatedGravityVelocity = m_gravity * deltaTime;
    uint32_t dstIndex = 0;
    ParticleRecord* pParticles = m_pCurrent;
    ParticleRecord* pDstParticles = m_pOld;
    assert(m_particleCount <= m_particles1.size());
    for (std::size_t i = 0; i < m_particleCount; ++i)
    {
      pDstParticles[dstIndex].Data = pParticles[i].Data;
      pDstParticles[dstIndex].Data.Energy -= deltaTime;

      // Interpolate the size
      pDstParticles[dstIndex].Data.Size =
        pDstParticles[dstIndex].Data.StartSize + ((pDstParticles[dstIndex].Data.EndSize - pDstParticles[dstIndex].Data.StartSize) *
                                                  (1.0f - (pDstParticles[dstIndex].Data.Energy / pDstParticles[dstIndex].Data.StartEnergy)));

      // Add gravity to velocity
      pDstParticles[dstIndex].Data.Velocity.X += accumulatedGravityVelocity.X;
      pDstParticles[dstIndex].Data.Velocity.Y += accumulatedGravityVelocity.Y;
      pDstParticles[dstIndex].Data.Velocity.Z += accumulatedGravityVelocity.Z;

      // Add the velocity
      pDstParticles[dstIndex].Data.Position.X += pDstParticles[dstIndex].Data.Velocity.X * deltaTime;
      pDstParticles[dstIndex].Data.Position.Y += pDstParticles[dstIndex].Data.Velocity.Y * deltaTime;
      pDstParticles[dstIndex].Data.Position.Z += pDstParticles[dstIndex].Data.Velocity.Z * deltaTime;
      // rParticle.RotationAngle += rParticle.RotationVelocity;

      dstIndex += pDstParticles[dstIndex].Data.Energy >= 0 ? 1 : 0;
    }
    m_particleCount = dstIndex;
    m_pCurrent = pDstParticles;
    m_pOld = pParticles;

    // auto end = timer.GetTime();
    // FSLLOG3_INFO(  "ParticleSystem update time: " << end - start);
  }

  void ParticleSystemTwoArrays::Draw(const ParticleDrawContext& context)
  {
    const uint8_t* pSrcParticles = reinterpret_cast<uint8_t*>(m_pCurrent) + offsetof(ParticleRecord, Data);
    m_particleDraw->Draw(context, pSrcParticles, m_particleCount, sizeof(ParticleRecord));
  }


  void ParticleSystemTwoArrays::AddParticles(const Particle* pParticles, const std::size_t& count)
  {
    // Silently ignore a attempt to add too many particles.
    const std::size_t actualCount = std::min(count, m_particles1.size() - m_particleCount);
    std::size_t i = 0;
    while (i < actualCount)
    {
      m_pCurrent[m_particleCount].Data = pParticles[i];
      ++m_particleCount;
      ++i;
    }
  }
}
