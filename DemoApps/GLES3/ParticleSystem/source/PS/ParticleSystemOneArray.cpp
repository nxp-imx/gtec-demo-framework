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

#include "ParticleSystemOneArray.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <algorithm>
#include <cassert>
#include <cstddef>

namespace Fsl
{
  ParticleSystemOneArray::ParticleSystemOneArray(const std::shared_ptr<IParticleDraw>& particleDraw, const std::size_t capacity)
    : m_particles(capacity)
    , m_particleDraw(particleDraw)
    , m_particleCount(0)
  {
  }


  uint32_t ParticleSystemOneArray::GetParticleCount() const
  {
    return m_particleCount;
  }


  void ParticleSystemOneArray::AddEmitter(const std::shared_ptr<IParticleEmitter>& emitter)
  {
    m_emitters.push_back(emitter);
  }


  void ParticleSystemOneArray::Update(const DemoTime& demoTime)
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
    ParticleRecord* pParticles = m_particles.data();
    assert(m_particleCount <= m_particles.size());
    for (uint32_t i = 0; i < m_particleCount; ++i)
    {
      Particle& rParticle = pParticles[i].Data;
      rParticle.Energy -= deltaTime;

      // Interpolate the size
      rParticle.Size = rParticle.StartSize + ((rParticle.EndSize - rParticle.StartSize) * (1.0f - (rParticle.Energy / rParticle.StartEnergy)));

      // Add gravity to velocity
      rParticle.Velocity.X += accumulatedGravityVelocity.X;
      rParticle.Velocity.Y += accumulatedGravityVelocity.Y;
      rParticle.Velocity.Z += accumulatedGravityVelocity.Z;

      // Add the velocity
      rParticle.Position.X += rParticle.Velocity.X * deltaTime;
      rParticle.Position.Y += rParticle.Velocity.Y * deltaTime;
      rParticle.Position.Z += rParticle.Velocity.Z * deltaTime;
      // rParticle.RotationAngle += rParticle.RotationVelocity;

      pParticles[dstIndex].Index = i;
      dstIndex += rParticle.Energy <= 0 ? 1 : 0;
    }


    // Check if we need to do some garbage collection
    if (dstIndex > 0)
    {
      ParticleSystemGCFast(m_particles, m_particleCount, dstIndex);
      // ParticleSystemGC(m_particles, m_particleCount, dstIndex);
      // FSLLOG("GC: Time: " << end - start << " Count: " << dstIndex);
    }

    // auto end = timer.GetTime();
    // FSLLOG("ParticleSystem update time: " << end - start);
  }

  void ParticleSystemOneArray::Draw(const ParticleDrawContext& context)
  {
    const uint8_t* pSrcParticles = reinterpret_cast<uint8_t*>(m_particles.data()) + offsetof(ParticleRecord, Data);
    m_particleDraw->Draw(context, pSrcParticles, m_particleCount, sizeof(ParticleRecord));
  }

  void ParticleSystemOneArray::AddParticles(const Particle* pParticles, const std::size_t& count)
  {
    // Silently ignore a attempt to add too many particles.
    const std::size_t actualCount = std::min(count, m_particles.size() - m_particleCount);
    std::size_t i = 0;
    while (i < actualCount)
    {
      m_particles[m_particleCount].Data = pParticles[i];
      ++m_particleCount;
      ++i;
    }
  }


  void ParticleSystemOneArray::ParticleSystemGCFast(std::vector<ParticleRecord>& rParticles, uint32_t& rNumParticles, const uint32_t numEntriesToGC)
  {
    // This delete does not keep the particles in order but it ought to execute fairly fast and involve a minimum of data movement
    assert(numEntriesToGC <= rNumParticles);
    assert(rNumParticles <= rParticles.size());
    if (numEntriesToGC >= rNumParticles)
    {
      rNumParticles = 0;
      return;
    }
    const uint32_t count = std::min(numEntriesToGC, rNumParticles - numEntriesToGC);
    uint32_t lastEntryIndex = rNumParticles - 1;
    ParticleRecord* pParticles = rParticles.data();
    for (uint32_t i = 0; i < count; ++i)
    {
      while (pParticles[lastEntryIndex].Data.Energy < 0)
      {
        --lastEntryIndex;
        assert(lastEntryIndex < rNumParticles);
      }

      assert(pParticles[i].Index < rNumParticles);
      pParticles[pParticles[i].Index].Data = pParticles[lastEntryIndex].Data;
      --lastEntryIndex;
    }

    assert(numEntriesToGC <= rNumParticles);
    rNumParticles -= numEntriesToGC;
  }


  void ParticleSystemOneArray::ParticleSystemGC(std::vector<ParticleRecord>& rParticles, uint32_t& rNumParticles, const uint32_t numEntriesToGC)
  {
    // This delete keeps the particles in order, but it moves a lot of data around on GC :(
    assert(numEntriesToGC <= rNumParticles);
    assert(rNumParticles <= rParticles.size());
    if (numEntriesToGC >= rNumParticles)
    {
      rNumParticles = 0;
      return;
    }


    const uint32_t count = numEntriesToGC - 1;
    uint32_t finalDstIndex = rParticles[0].Index;
    uint32_t startMoveIndex = finalDstIndex;
    uint32_t followingIndex = finalDstIndex + 1;
    uint32_t i = 1;
    ParticleRecord* pParticles = rParticles.data();
    while (i < count)
    {
      // Skip forward as long as we are in a continuous segment
      while (i < count && (rParticles[i].Index) == followingIndex)
      {
        ++followingIndex;
        ++i;
      }

      // move particle entries
      auto endMoveIndex = rParticles[i].Index;
      while (followingIndex < endMoveIndex)
      {
        assert(finalDstIndex < rNumParticles);
        assert(followingIndex < rNumParticles);
        pParticles[finalDstIndex].Data = rParticles[followingIndex].Data;
        ++followingIndex;
        ++finalDstIndex;
      }
      startMoveIndex = endMoveIndex;
    }

    // Move the last particles
    ++startMoveIndex;
    while (startMoveIndex < rNumParticles)
    {
      assert(finalDstIndex < rNumParticles);
      assert(startMoveIndex < rNumParticles);
      pParticles[finalDstIndex].Data = rParticles[startMoveIndex].Data;
      ++startMoveIndex;
      ++finalDstIndex;
    }
    assert(numEntriesToGC <= rNumParticles);
    rNumParticles -= numEntriesToGC;
  }
}
