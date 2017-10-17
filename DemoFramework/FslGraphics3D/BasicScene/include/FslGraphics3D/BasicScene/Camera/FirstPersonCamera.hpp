#ifndef FSLGRAPHICS3D_BASICSCENE_CAMERA_FIRSTPERSONCAMERA_HPP
#define FSLGRAPHICS3D_BASICSCENE_CAMERA_FIRSTPERSONCAMERA_HPP
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
#include <FslBase/Math/Point2.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    class FirstPersonCamera
    {
      BasicFirstPersonCamera m_base;
      float m_sensitivity;

      bool m_hasOldPosition;
      Point2 m_oldPosition;
    public:
      FirstPersonCamera();
      ~FirstPersonCamera();

      //! @brief Get the current view matrix for the camera
      Matrix GetViewMatrix() const
      {
        return m_base.GetViewMatrix();
      }

      Vector3 GetPosition() const
      {
        return m_base.GetPosition();
      }

      void SetPosition(const Vector3& position);
      void SetPosition(const Vector3& position, const Vector3& target, const Vector3& up);

      void MoveForward(const float amount);
      void MoveBackwards(const float amount);
      void MoveLeft(const float amount);
      void MoveRight(const float amount);

      void Rotate(const Vector2& amount);

      //! @brief
      //! @note If rotateCamera is true the camera rotation will be updated else only the internal position will be touched
      void RotateViaPosition(const bool rotateCamera, const Point2& currentPosition);
    };
  }
}

#endif
