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

#include <FslGraphics3D/Camera/ArcballCamera.hpp>
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
      const float MIN_ZOOM = 1.0f;
      const float MAX_ZOOM = std::numeric_limits<float>::max();

      inline Vector3 MapToSphere(const Vector2& screenResolutionBounds, const Point2& point)
      {
        Vector2 scaledPoint;

        // Adjust point coords and scale down to range of [-1 ... 1]
        scaledPoint.X = (point.X * screenResolutionBounds.X) - 1.0f;
        scaledPoint.Y = 1.0f - (point.Y * screenResolutionBounds.Y);

        // Compute the square of the length of the vector to the point from the center
        const float length = (scaledPoint.X * scaledPoint.X) + (scaledPoint.Y * scaledPoint.Y);

        // If the point is mapped outside of the sphere... (length > radius squared)
        if (length > 1.0f)
        {
          // Compute a normalizing factor (radius / sqrt(length))
          const float norm = 1.0f / std::sqrt(length);
          // Return the "normalized" vector, a point on the sphere
          return Vector3(scaledPoint.X * norm, scaledPoint.Y * norm, 0.0f);
        }

        // Else it's on the inside
        // Return a vector to a point mapped inside the sphere sqrt(radius squared - length)
        return Vector3(scaledPoint.X, scaledPoint.Y, std::sqrt(1.0f - length));
      }


      Quaternion CalcRotation(const Vector3& vec1, const Vector3& vec2)
      {
        // Compute the vector perpendicular to the begin and end vectors
        const Vector3 perpendicularVector = Vector3::Cross(vec1, vec2);
        // Compute the length of the perpendicular vector
        if (perpendicularVector.Length() > std::numeric_limits<float>::epsilon())
        {
          // It's non zero
          // We're ok, so return the perpendicular vector as the transform after all
          // In the quaternion values, w is cosine (theta / 2), where theta is rotation angle
          return Quaternion(perpendicularVector, Vector3::Dot(vec1, vec2));
        }

        // it's zero and therefor the begin and end vectors coincide, so use an identity transform
        return Quaternion::Identity();
      }
    }


    ArcballCamera::ArcballCamera(const Point2& screenResolution)
      : m_rotationMatrix(Matrix::GetIdentity())
      , m_zoom(MIN_ZOOM)
      , m_isDragging(false)
      , m_dragRotation(Quaternion::Identity())
    {
      SetScreenResolution(screenResolution);
    }


    ArcballCamera::~ArcballCamera() = default;


    void ArcballCamera::SetScreenResolution(const Point2& screenResolution)
    {
      if (screenResolution.X < 1 || screenResolution.Y < 1)
      {
        throw std::invalid_argument("Invalid resolution");
      }

      // Set adjustment factor for width/height
      m_screenResolutionBounds = Vector2(1.0f / ((screenResolution.X - 1.0f) * 0.5f), 1.0f / ((screenResolution.Y - 1.0f) * 0.5f));
    }


    float ArcballCamera::GetMinZoom() const
    {
      return MIN_ZOOM;
    }


    float ArcballCamera::GetMaxZoom() const
    {
      return MAX_ZOOM;
    }


    float ArcballCamera::GetZoom() const
    {
      return m_zoom;
    }


    void ArcballCamera::SetZoom(const float value)
    {
      m_zoom = MathHelper::Clamp(value, MIN_ZOOM, MAX_ZOOM);
    }


    void ArcballCamera::AddZoom(const float value)
    {
      m_zoom = MathHelper::Clamp(m_zoom + value, MIN_ZOOM, MAX_ZOOM);
    }


    void ArcballCamera::BeginDrag(const Point2& position)
    {
      FSLLOG3_WARNING_IF(m_isDragging, "Already dragging. Please end the drag before starting a new one.");
      m_isDragging = true;
      // Map the point to the sphere
      m_dragStart = MapToSphere(m_screenResolutionBounds, position);
      m_dragRotation = Quaternion::Identity();
    }


    void ArcballCamera::Drag(const Point2& position)
    {
      if (!m_isDragging)
      {
        return;
      }

      // Map the point to the sphere
      m_dragCurrent = MapToSphere(m_screenResolutionBounds, position);

      // Return the quaternion equivalent to the rotation
      m_dragRotation = CalcRotation(m_dragStart, m_dragCurrent);
    }


    void ArcballCamera::EndDrag(const Point2& position)
    {
      FSLLOG3_WARNING_IF(!m_isDragging, "Not dragging, please start a drag before stopping it");
      Drag(position);
      m_dragStart = MapToSphere(m_screenResolutionBounds, position);
      m_rotationMatrix *= Matrix::CreateFromQuaternion(m_dragRotation);
      m_isDragging = false;
    }


    void ArcballCamera::CancelDrag()
    {
      m_isDragging = false;
    }


    Matrix ArcballCamera::GetViewMatrix() const
    {
      if (m_isDragging)
      {
        return m_rotationMatrix * Matrix::CreateFromQuaternion(m_dragRotation) * Matrix::CreateTranslation(0, 0, -m_zoom);
      }

      return m_rotationMatrix * Matrix::CreateTranslation(0, 0, -m_zoom);
    }


    Matrix ArcballCamera::GetRotationMatrix() const
    {
      if (m_isDragging)
      {
        return m_rotationMatrix * Matrix::CreateFromQuaternion(m_dragRotation);
      }

      return m_rotationMatrix;
    }


    void ArcballCamera::ResetRotation()
    {
      CancelDrag();
      m_rotationMatrix = Matrix::GetIdentity();
    }


    void ArcballCamera::SetRotation(const Matrix& matrix)
    {
      CancelDrag();
      m_rotationMatrix = matrix;
    }
  }
}
