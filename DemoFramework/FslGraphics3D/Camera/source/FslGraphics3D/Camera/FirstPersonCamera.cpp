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

#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace Graphics3D
  {
    namespace
    {
      const float DEFAULT_SENSITIVITY = 0.1f;
    }

    FirstPersonCamera::FirstPersonCamera()
      : m_sensitivity(DEFAULT_SENSITIVITY)
      , m_hasOldPosition(false)
    {
    }

    void FirstPersonCamera::SetPosition(const Vector3& position)
    {
      m_base.SetPosition(position);
    }


    void FirstPersonCamera::SetPosition(const Vector3& position, const Vector3& target, const Vector3& up)
    {
      m_base.SetPosition(position, target, up);
    }


    void FirstPersonCamera::MoveForward(const float amount)
    {
      m_base.MoveForward(amount);
    }


    void FirstPersonCamera::MoveBackwards(const float amount)
    {
      m_base.MoveBackwards(amount);
    }


    void FirstPersonCamera::MoveLeft(const float amount)
    {
      m_base.MoveLeft(amount);
    }


    void FirstPersonCamera::MoveRight(const float amount)
    {
      m_base.MoveRight(amount);
    }


    void FirstPersonCamera::Rotate(const Vector2& amount)
    {
      Vector2 adjustedAmount(amount.X * m_sensitivity, amount.Y * m_sensitivity);
      m_base.Rotate(adjustedAmount);
    }


    void FirstPersonCamera::RotateByRadians(const Vector2& amount)
    {
      Vector2 adjustedAmount(amount.X * m_sensitivity, amount.Y * m_sensitivity);
      m_base.RotateByRadians(adjustedAmount);
    }


    void FirstPersonCamera::RotateViaPosition(const bool rotateCamera, const PxPoint2& currentPosition)
    {
      if (rotateCamera && m_hasOldPosition)
      {
        PxPoint2 deltaPosition = currentPosition - m_oldPosition;
        if (deltaPosition.X != 0 || deltaPosition.Y != 0)
        {
          Rotate(Vector2(deltaPosition.X, -deltaPosition.Y));
        }
      }
      m_oldPosition = currentPosition;
      m_hasOldPosition = true;
    }
  }
}
