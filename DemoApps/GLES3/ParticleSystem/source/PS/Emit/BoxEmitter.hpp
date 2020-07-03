#ifndef PS_EMIT_GLES3_PARTICLESYSTEM_BOXEMITTER_HPP
#define PS_EMIT_GLES3_PARTICLESYSTEM_BOXEMITTER_HPP
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

#include "IParticleEmitter.hpp"
#include "EmitCounter.hpp"
#include "../Particle.hpp"
#include <random>
#include <vector>

namespace Fsl
{
  class BoxEmitter : public IParticleEmitter
  {
    EmitCounter m_emitCounter;
    bool m_isInitialized;
    std::mt19937 m_random;
    int32_t m_particlesPerSecond;
    float m_energyMin;
    float m_energyMax;
    float m_startSizeMin;
    float m_startSizeMax;
    float m_endSizeMin;
    float m_endSizeMax;
    // Color m_startColorMin;
    // Color m_startColorMax;
    // Color m_endColorMin;
    // Color m_endColorMax;
    float m_speedMin;
    float m_speedMax;
    uint8_t m_textureIdMin;
    uint8_t m_textureIdMax;
    Vector3 m_box;

    std::vector<Particle> m_scratchpad;

  public:
    BoxEmitter();

    int32_t GetParticlesPerSecond() const;
    void SetParticlesPerSecond(const int32_t value);
    void Update(IParticleContainer& container, const DemoTime& demoTime) override;

  private:
    std::size_t SpawnParticles(std::vector<Particle>& rDst, const std::size_t count);
  };
}

#endif
