#ifndef GLES3_SPRINGBACKGROUND_POINTMASS_HPP
#define GLES3_SPRINGBACKGROUND_POINTMASS_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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


namespace Fsl
{
  class PointMass
  {
  protected:
    Vector3 m_acceleration;
    float m_damping{0.98f};

  public:
    Vector3 m_position;
    Vector3 m_oldPosition;
    float m_inverseMass{0};

  public:
    PointMass()
      : m_acceleration(0, 0, 0)
      , m_position(0, 0, 0)
      , m_oldPosition(0, 0, 0)
    {
    }

    PointMass(const Vector3& position, const float invMass)
      : m_acceleration(0, 0, 0)
      , m_position(position)
      , m_oldPosition(position)
      , m_inverseMass(invMass)
    {
    }

    void ApplyForce(const Vector3& force)
    {
      m_acceleration += force * m_inverseMass;
    }

    void IncreaseDamping(const float factor)
    {
      m_damping *= factor;
    }

    void FixedUpdate()
    {
      auto velocity = ((m_position - m_oldPosition) + m_acceleration) * m_damping;
      m_oldPosition = m_position;
      m_position += velocity;

      m_acceleration = Vector3(0, 0, 0);
      if (velocity.LengthSquared() < (0.001f * 0.001f))
      {
        m_oldPosition = m_position;
      }

      m_damping = 0.98f;
    }
  };
}


#endif
