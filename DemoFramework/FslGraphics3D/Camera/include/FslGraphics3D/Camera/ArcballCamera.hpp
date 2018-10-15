#ifndef FSLGRAPHICS3D_CAMERA_ARCBALLCAMERA_HPP
#define FSLGRAPHICS3D_CAMERA_ARCBALLCAMERA_HPP
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

/*
 * The very basic Arcball handling was inspired by the "ArcBall Rotation Control, Revisited" By Terence J.Grant(tjgrant@tatewake.com)
 * NeHe tutorial Lesson 48 - http://nehe.gamedev.net/tutorial/arcball_rotation/19003/
 * Which was based on Bretton Wade's, which is based on Ken Shoemake's from Graphic Gems IV p.175
 */

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    class ArcballCamera
    {
      Vector2 m_screenResolutionBounds;
      Matrix m_rotationMatrix;
      float m_zoom;
      bool m_isDragging;
      Vector3 m_dragStart;
      Vector3 m_dragCurrent;
      Quaternion m_dragRotation;

    public:
      ArcballCamera(const Point2& screenResolution);
      ~ArcballCamera();

      void SetScreenResolution(const Point2& screenResolution);

      float GetMinZoom() const;
      float GetMaxZoom() const;
      float GetZoom() const;
      void SetZoom(const float value);
      void AddZoom(const float value);


      //! @brief Check if the camera is being dragged
      bool IsDragging() const
      {
        return m_isDragging;
      }

      //! @brief Start a camera drag operation
      void BeginDrag(const Point2& position);

      //! @brief Drag the camera around the object
      void Drag(const Point2& position);

      //! @brief End the drag operation
      void EndDrag(const Point2& position);

      //! @brief If a drag is active this will cancel it. If not active this does nothing.
      void CancelDrag();

      //! @brief Get the current view matrix for the camera
      Matrix GetViewMatrix() const;

      //! @brief Get the current rotation matrix for the camera
      Matrix GetRotationMatrix() const;

      //! @brief Reset the rotation
      void ResetRotation();

      //! @brief Set the rotation
      void SetRotation(const Matrix& matrix);
    };
  }
}

#endif
