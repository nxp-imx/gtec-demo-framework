#ifndef PS_GLES3_PARTICLESYSTEM_PARTICLE_HPP
#define PS_GLES3_PARTICLESYSTEM_PARTICLE_HPP
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

#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslGraphics/Color.hpp>

namespace Fsl
{
  struct Particle
  {
    Vector3 Position;
    Vector3 Velocity;
    float Energy;
    float Size;

    //! Set on creation
    float StartEnergy;
    float StartSize;
    float EndSize;
    uint8_t TextureId;

    Particle()
      : Position()
      , Velocity()
      , Energy(0)
      , StartEnergy(0)
      , TextureId(0)
    {
    }

    Particle(const Vector3& position, const Vector3& velocity, const float startSize, const float endSize, const float startEnergy, const uint8_t textureId)
      : Position(position)
      , Velocity(velocity)
      , Energy(startEnergy)
      , Size(startSize)
      , StartEnergy(startEnergy)
      , StartSize(startSize)
      , EndSize(endSize)
      , TextureId(textureId)
    {
    }
  };
}

#endif
