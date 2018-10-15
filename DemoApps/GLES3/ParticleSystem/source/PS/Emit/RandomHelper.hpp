#ifndef PS_EMIT_GLES3_PARTICLESYSTEM_RANDOMHELPER_HPP
#define PS_EMIT_GLES3_PARTICLESYSTEM_RANDOMHELPER_HPP
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

#include <random>
#include <FslGraphics/Color.hpp>
#include <algorithm>

namespace Fsl
{
  class RandomColor
  {
    Color m_min;
    int32_t m_deltaR;
    int32_t m_deltaG;
    int32_t m_deltaB;
    int32_t m_deltaA;
    std::uniform_real_distribution<float> m_dist;

  public:
    RandomColor(const Color& min, const Color& max)
      : m_min(min)
      , m_deltaR(static_cast<int32_t>(max.R()) - static_cast<int32_t>(m_min.R()))
      , m_deltaG(static_cast<int32_t>(max.G()) - static_cast<int32_t>(m_min.G()))
      , m_deltaB(static_cast<int32_t>(max.B()) - static_cast<int32_t>(m_min.B()))
      , m_deltaA(static_cast<int32_t>(max.A()) - static_cast<int32_t>(m_min.A()))
      , m_dist(0.0f, 1.0f)
    {
    }

    template <class TGenerator>
    Color operator()(TGenerator& g) const
    {
      const float val = m_dist(g);
      const int32_t colR = std::max(std::min(static_cast<int32_t>(m_min.R()) + static_cast<int32_t>(m_deltaR * val), 255), 0);
      const int32_t colG = std::max(std::min(static_cast<int32_t>(m_min.G()) + static_cast<int32_t>(m_deltaG * val), 255), 0);
      const int32_t colB = std::max(std::min(static_cast<int32_t>(m_min.B()) + static_cast<int32_t>(m_deltaB * val), 255), 0);
      const int32_t colA = std::max(std::min(static_cast<int32_t>(m_min.A()) + static_cast<int32_t>(m_deltaA * val), 255), 0);
      return Color(colR, colG, colB, colA);
    }
  };

  template <typename T>
  class RandomVector
  {
    T m_min;
    T m_delta;
    std::uniform_real_distribution<float> m_dist;

  public:
    RandomVector(const T& min, const T& max)
      : m_min(min)
      , m_delta(max - min)
      , m_dist(0.0f, 1.0f)
    {
    }

    template <class TGenerator>
    T operator()(TGenerator& g) const
    {
      const float val = m_dist(g);
      return T(m_min + (m_delta * val));
    }
  };
}
#endif
