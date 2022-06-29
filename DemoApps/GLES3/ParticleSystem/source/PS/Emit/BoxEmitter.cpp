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

#include "BoxEmitter.hpp"
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <algorithm>
#include <cassert>
#include "../IParticleContainer.hpp"
#include "RandomHelper.hpp"

namespace Fsl
{
  BoxEmitter::BoxEmitter()
    : m_isInitialized(false)
    , m_particlesPerSecond(300)
    , m_energyMin(1.2f * 3)
    , m_energyMax(2.2f * 3)
    , m_startSizeMin(0.7f)
    , m_startSizeMax(1.0f)
    , m_endSizeMin(0.0f)
    , m_endSizeMax(0.0f)
    //, m_startColorMin(Color::White())
    //, m_startColorMax(Color::White())
    //, m_endColorMin(Color::Transparent())
    //, m_endColorMax(Color::Transparent())
    , m_speedMin(1)
    , m_speedMax(4)
    , m_textureIdMin(0)
    , m_textureIdMax(0)
    , m_box(1, 1, 1)
  {
  }


  int32_t BoxEmitter::GetParticlesPerSecond() const
  {
    return m_particlesPerSecond;
  }


  void BoxEmitter::SetParticlesPerSecond(const int32_t value)
  {
    m_particlesPerSecond = std::max(value, 0);
  }


  void BoxEmitter::Update(IParticleContainer& container, const DemoTime& demoTime)
  {
    if (!m_isInitialized)
    {
      m_isInitialized = true;
      return;
    }

    m_emitCounter.Update(TimeSpanUtil::ToClampedMillisecondsUInt64(demoTime.ElapsedTime), m_particlesPerSecond);
    std::size_t particleCount = 0;
    if (m_emitCounter.Count() > 0)
    {
      particleCount += SpawnParticles(m_scratchpad, m_emitCounter.Count());
    }

    // if (m_bursts.Count > 0)
    //  particleCount += HandleBursts(ps, demoTime, ref emitterMoveDelta);

    if (particleCount > 0)
    {
      container.AddParticles(m_scratchpad.data(), particleCount);
    }
  }


  std::size_t BoxEmitter::SpawnParticles(std::vector<Particle>& rDst, const std::size_t count)
  {
    if (count > rDst.size())
    {
      rDst.resize(count);
    }

    Vector3 startPos(m_box * -0.5f);

    std::uniform_int_distribution<int32_t> randomTexture(m_textureIdMin, m_textureIdMax);
    std::uniform_real_distribution<float> randomPositionX(startPos.X, startPos.X + m_box.X);
    std::uniform_real_distribution<float> randomPositionY(startPos.Y, startPos.Y + m_box.Y);
    std::uniform_real_distribution<float> randomPositionZ(startPos.Z, startPos.Z + m_box.Z);
    std::uniform_real_distribution<float> randomEnergy(m_energyMin, m_energyMax);
    std::uniform_real_distribution<float> randomSpeed(m_speedMin, m_speedMax);
    std::uniform_real_distribution<float> randomStartSize(m_startSizeMin, m_startSizeMax);
    std::uniform_real_distribution<float> randomEndSize(m_endSizeMin, m_endSizeMax);
    // const RandomColor randomStartColor(m_startColorMin, m_startColorMax);
    // const RandomColor randomEndColor(m_endColorMin, m_endColorMax);

    for (std::size_t i = 0; i < count; ++i)
    {
      const auto textureId = static_cast<uint8_t>(randomTexture(m_random));

      const Vector3 pos(randomPositionX(m_random), randomPositionY(m_random), randomPositionZ(m_random));

      // Velocity
      const float energy = randomEnergy(m_random);
      const float velocitySpeed = randomSpeed(m_random);

      // size
      const float startSize = randomStartSize(m_random);
      const float endSize = randomEndSize(m_random);

      // Color
      // const auto startColor = randomStartColor(m_random);
      // const auto endColor = randomEndColor(m_random);
      rDst[i] = Particle(pos, Vector3(0, velocitySpeed, 0), startSize, endSize, energy, textureId);
      // rDst[i] = Particle(pos, Vector3(0, velocitySpeed, 0), 1, 1, energy, textureId);
    }
    return count;
  }
}
