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

#include <Shared/EnvironmentMapping/EnvState.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <cassert>

namespace Fsl
{
  EnvState::EnvState(const Point2& screenResolution)
    : m_screenResolution(screenResolution)
    , m_stepsPerCircle(360)
    , m_time(0)
    , m_timeStep(20)
    , Eye(0, 0, -1)
    , LookAt()
    , UpVec(0, -1, 0)
    , EyeVector(0, 0, 1)
    , ViewMatrix()
    , ProjMatrix()
  {
  }


  EnvState::~EnvState()
  {
  }


  void EnvState::Update(const float deltaTime)
  {
    m_time += m_timeStep * deltaTime;
    // Compute the angle and advance the time counter.
    float fAngle = m_time * (2.0f * MathHelper::PI) / m_stepsPerCircle;

    Matrix eyeMatrix(Matrix::CreateRotationY(fAngle));

    // Set eye position.
    Vector4 eyeTransform = Vector4::Transform(Eye, eyeMatrix);

    EyeVector.X = eyeTransform.X;
    EyeVector.Y = eyeTransform.Y;
    EyeVector.Z = eyeTransform.Z;

    LookAt = Vector3::Zero();

    Matrix::CreateLookAt(ViewMatrix, EyeVector, LookAt, UpVec);

    float fAspect = m_screenResolution.X / float(m_screenResolution.Y);

    ProjMatrix = Matrix::CreatePerspectiveFieldOfView(MathHelper::PI / 4.0f, fAspect, 1.0f, 100.0f);
  }


  void EnvState::ExtractTransform(Matrix& rMatrix)
  {
    // Compute transformation matrix.
    rMatrix = ViewMatrix * ProjMatrix;
  }
}
