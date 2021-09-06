#ifndef PSGPU_GLES3_PARTICLESYSTEM_PARTICLEGPU_HPP
#define PSGPU_GLES3_PARTICLESYSTEM_PARTICLEGPU_HPP
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

#include <FslBase/Math/Vector3.hpp>
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>

namespace Fsl
{
  enum class ParticleGPUType
  {
    Normal = 0,
    Emitter = 1,
  };

  class ParticleGPU
  {
  public:
    Vector3 Position;
    Vector3 Velocity;
    float Energy{0};
    float Type{0};

    ParticleGPU() = default;

    ParticleGPU(const Vector3& position, const Vector3& velocity, const float startEnergy, const ParticleGPUType type)
      : Position(position)
      , Velocity(velocity)
      , Energy(startEnergy)
      , Type(static_cast<float>(type))
    {
    }

    constexpr static VertexDeclarationArray<4> GetVertexDeclarationArray()
    {
      constexpr std::array<VertexElementEx, 4> elements = {
        VertexElementEx(offsetof(ParticleGPU, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
        VertexElementEx(offsetof(ParticleGPU, Velocity), VertexElementFormat::Vector3, VertexElementUsage::Custom, 0),
        VertexElementEx(offsetof(ParticleGPU, Energy), VertexElementFormat::Single, VertexElementUsage::Custom, 1),
        VertexElementEx(offsetof(ParticleGPU, Type), VertexElementFormat::Single, VertexElementUsage::Custom, 2),
      };
      return VertexDeclarationArray<4>(elements, sizeof(ParticleGPU));
    }


    // IMPROVEMENT: In C++17 this could be a constexpr since array .data() becomes a constexpr
    //              At least this workaround still gives us compile time validation of the vertex element data
    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<4> decl = GetVertexDeclarationArray();
      return decl.AsReadOnlySpan();
    }
  };
}

#endif
