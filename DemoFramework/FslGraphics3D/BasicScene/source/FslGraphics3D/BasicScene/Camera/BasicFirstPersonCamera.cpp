/****************************************************************************************************************************************************
* Copyright 2017 NXP
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

#include <FslGraphics3D/BasicScene/Camera/BasicFirstPersonCamera.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

namespace Fsl
{
  // Camera class inspired by the tutorial: https://learnopengl.com/#!Getting-started/Camera
  namespace Graphics3D
  {
    namespace
    {
      const float MAX_PITCH = 89.0f * MathHelper::TO_RADS;
    }

    BasicFirstPersonCamera::BasicFirstPersonCamera()
      : m_position()
      , m_forward()
      , m_up()
      , m_yaw(0.0f)
      , m_pitch(0.0f)
    {
    }


    BasicFirstPersonCamera::~BasicFirstPersonCamera()
    {
    }



    Matrix BasicFirstPersonCamera::GetViewMatrix() const
    {
      return Matrix::CreateLookAt(m_position, m_position + m_forward, m_up);
    }


    Vector3 BasicFirstPersonCamera::GetPosition() const
    {
      return m_position;
    }


    void BasicFirstPersonCamera::SetPosition(const Vector3& position)
    {
      m_position = position;
    }


    void BasicFirstPersonCamera::SetPosition(const Vector3& position, const Vector3& target, const Vector3& up)
    {
      m_position = position;
      m_forward = target - position;
      m_forward.Normalize();
      m_up = up;
      m_yaw = std::atan2(m_forward.X, m_forward.Z) + MathHelper::RADS90;
      m_pitch = std::asin(m_forward.Y);
    }


    void BasicFirstPersonCamera::MoveForward(const float amount)
    {
      m_position += m_forward * amount;
    }


    void BasicFirstPersonCamera::MoveBackwards(const float amount)
    {
      m_position -= m_forward * amount;
    }


    void BasicFirstPersonCamera::MoveLeft(const float amount)
    {
      m_position -= Vector3::Normalize(Vector3::Cross(m_forward, m_up)) * amount;
    }


    void BasicFirstPersonCamera::MoveRight(const float amount)
    {
      m_position += Vector3::Normalize(Vector3::Cross(m_forward, m_up)) * amount;
    }


    void BasicFirstPersonCamera::Rotate(const Vector2& amount)
    {
      m_yaw += amount.X * MathHelper::TO_RADS;
      m_pitch += amount.Y * MathHelper::TO_RADS;

      // Make sure that when pitch is out of bounds, screen doesn't get flipped
      if (m_pitch > MAX_PITCH)
        m_pitch = MAX_PITCH;
      if (m_pitch < -MAX_PITCH)
        m_pitch = -MAX_PITCH;

      Vector3 front;
      front.X = std::cos(m_yaw) * std::cos(m_pitch);
      front.Y = std::sin(m_pitch);
      front.Z = std::sin(m_yaw) * std::cos(m_pitch);
      m_forward = Vector3::Normalize(front);
    }

  }
}
