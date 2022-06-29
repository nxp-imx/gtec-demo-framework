#ifndef FSLGRAPHICS3D_CAMERA_BASICFIRSTPERSONCAMERA_HPP
#define FSLGRAPHICS3D_CAMERA_BASICFIRSTPERSONCAMERA_HPP
/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>

namespace Fsl::Graphics3D
{
  class BasicFirstPersonCamera
  {
    Vector3 m_position;
    Vector3 m_forward;
    Vector3 m_up;
    Vector3 m_right;
    Vector3 m_worldUp;

    float m_yaw;
    float m_pitch;

  public:
    BasicFirstPersonCamera();
    ~BasicFirstPersonCamera() = default;

    //! @brief Get the current view matrix for the camera
    Matrix GetViewMatrix() const;

    Vector3 GetPosition() const;

    void SetPosition(const Vector3& position);
    void SetPosition(const Vector3& position, const Vector3& target, const Vector3& up);

    void MoveForward(const float amount);
    void MoveBackwards(const float amount);
    void MoveLeft(const float amount);
    void MoveRight(const float amount);

    void Rotate(const Vector2& amount);
    void RotateByRadians(const Vector2& amount);

  private:
    // @brief Recalculates the camera vectors using m_yaw, m_picth and m_worldUp
    void RecalcCameraVectors();
  };
}

#endif
