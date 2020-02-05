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

#include <FslGraphics3D/Camera/BasicFirstPersonCamera.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
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
      const float MAX_PITCH = MathHelper::ToRadians(89.0f);

      float Clamp(const float value)
      {
        // Basic clamping that works ok at low speeds
        if (value >= MathHelper::ToRadians(360))
        {
          return value - MathHelper::ToRadians(360);
        }
        if (value <= -MathHelper::ToRadians(360))
        {
          return value + MathHelper::ToRadians(360);
        }
        return value;
      }
    }

    BasicFirstPersonCamera::BasicFirstPersonCamera()
      : m_worldUp(0.0f, 0.0f, 1.0f)
      , m_yaw(-90.0f)
      , m_pitch(0.0f)
    {
      RecalcCameraVectors();
    }


    BasicFirstPersonCamera::~BasicFirstPersonCamera() = default;


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
      m_worldUp = up;

      auto forward = target - position;
      forward.Normalize();

      m_yaw = std::atan2(forward.X, forward.Z);
      m_pitch = std::asin(forward.Y);

      RecalcCameraVectors();
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
      m_position -= m_right * amount;
    }


    void BasicFirstPersonCamera::MoveRight(const float amount)
    {
      m_position += m_right * amount;
    }

    void BasicFirstPersonCamera::Rotate(const Vector2& amount)
    {
      RotateByRadians(Vector2(MathHelper::ToRadians(amount.X), MathHelper::ToRadians(amount.Y)));
    }

    void BasicFirstPersonCamera::RotateByRadians(const Vector2& amount)
    {
      m_yaw -= amount.X;
      m_pitch += amount.Y;
      m_yaw = Clamp(m_yaw);
      m_pitch = Clamp(m_pitch);

      // Make sure that when pitch is out of bounds, screen doesn't get flipped
      if (m_pitch > MAX_PITCH)
      {
        m_pitch = MAX_PITCH;
      }
      if (m_pitch < -MAX_PITCH)
      {
        m_pitch = -MAX_PITCH;
      }

      RecalcCameraVectors();
    }

    void BasicFirstPersonCamera::RecalcCameraVectors()
    {
      const float cosYaw = std::cos(m_yaw);
      const float sinYaw = std::sin(m_yaw);
      const float cosPitch = std::cos(m_pitch);
      const float sinPitch = std::sin(m_pitch);

      // Fist we calc the new front vector and use it to calc the right and up vectors
      m_forward.X = cosPitch * sinYaw;
      m_forward.Y = sinPitch;
      m_forward.Z = cosPitch * cosYaw;
      m_forward.Normalize();

      m_right = Vector3::Normalize(Vector3::Cross(m_forward, m_worldUp));
      m_up = Vector3::Normalize(Vector3::Cross(m_right, m_forward));
    }
  }
}
