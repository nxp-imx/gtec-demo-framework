#ifndef GLES3_SPRINGBACKGROUND_SPRING_HPP
#define GLES3_SPRINGBACKGROUND_SPRING_HPP
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
#include "PointMass.hpp"

namespace Fsl
{
  class Spring
  {
  public:
    PointMass* m_pEnd1;
    PointMass* m_pEnd2;
    float m_targetLength;
    float m_stiffness;
    float m_damping;


    Spring(PointMass* end1, PointMass* end2, float stiffness, float damping)
      : m_pEnd1(end1)
      , m_pEnd2(end2)
      , m_targetLength(Vector3::Distance(m_pEnd1->m_position, m_pEnd2->m_position) * 0.95f)
      , m_stiffness(stiffness)
      , m_damping(damping)
    {
    }

    void FixedUpdate()
    {
      Vector3 x = m_pEnd1->m_position - m_pEnd2->m_position;

      float length = x.Length();

      if (length > m_targetLength)
      {
        x = (x / length) * (length - m_targetLength);
        Vector3 v1Velocity = m_pEnd1->m_position - m_pEnd1->m_oldPosition;
        Vector3 v2Velocity = m_pEnd2->m_position - m_pEnd2->m_oldPosition;

        Vector3 dv = v2Velocity - v1Velocity;
        Vector3 force = (m_stiffness * x) - (dv * m_damping);

        m_pEnd1->ApplyForce(-force);
        m_pEnd2->ApplyForce(force);
      }
    }
  };
}


#endif
