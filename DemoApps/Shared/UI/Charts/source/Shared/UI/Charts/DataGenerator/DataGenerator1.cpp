/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <Shared/UI/Charts/DataGenerator/DataGenerator1.hpp>
#include <cmath>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr float SinSpeed1 = 2.0f;
      constexpr float SinSpeed2 = -1.5f;
    }
  }

  DataGenerator1::DataGenerator1() = default;


  void DataGenerator1::Reset()
  {
    m_sinAngle1 = 0;
    m_sinAngle2 = 0;
  }

  DataGenerator1::Result DataGenerator1::Next(const float deltaTime)
  {
    m_sinAngle1 += LocalConfig::SinSpeed1 * deltaTime;
    m_sinAngle2 += LocalConfig::SinSpeed2 * deltaTime;

    // static_cast<uint32_t>((std::sin(m_sinAngle1) + std::sin(m_sinAngle2) + 2.0f) * 25.0f, 0

    const float val0 = (std::sin(m_sinAngle1) + 1.0f) * 25.0f;
    const float val1 = ((std::sin(m_sinAngle2) + 1.0f) * 25.0f);
    const float roundedVal0 = std::round(val0);
    const float roundedVal1 = std::round(val1 + (val0 - roundedVal0));

    return {static_cast<uint32_t>(roundedVal0), static_cast<uint32_t>(roundedVal1)};
  }
}
