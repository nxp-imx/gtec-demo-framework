/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Math/ViewportUtil.hpp>
#include <limits>

namespace Fsl::ViewportUtil
{
  namespace
  {
    constexpr bool WithinEpsilon(const float lhs, const float rhs) noexcept
    {
      const float delta = lhs - rhs;
      return ((-1.401298E-45f <= delta) && (delta <= std::numeric_limits<float>::epsilon()));
    }

    inline constexpr float Calc(const Matrix& modelViewProjectionMatrix, const Vector3& srcCoordinate) noexcept
    {
      using namespace MatrixFields;

      const auto* pData = modelViewProjectionMatrix.DirectAccess();
      return (((srcCoordinate.X * pData[_M14]) + (srcCoordinate.Y * pData[_M24])) + (srcCoordinate.Z * pData[_M34])) + pData[_M44];
    }
  }


  Vector3 Project(const Viewport& srcViewport, const Vector3& srcCoordinate, const Matrix& projection, const Matrix& view, const Matrix& world)
  {
    const Matrix matrix = Matrix::Multiply(Matrix::Multiply(world, view), projection);
    const float val = Calc(matrix, srcCoordinate);
    Vector3 transformedVector = Vector3::Transform(srcCoordinate, matrix);

    if (!WithinEpsilon(val, 1.0f))
    {
      transformedVector /= val;
    }
    return {(((transformedVector.X + 1.0f) * 0.5f) * static_cast<float>(srcViewport.Width())) + static_cast<float>(srcViewport.X()),
            (((-transformedVector.Y + 1.0f) * 0.5f) * static_cast<float>(srcViewport.Height())) + static_cast<float>(srcViewport.Y()),
            (transformedVector.Z * (srcViewport.MaxDepth() - srcViewport.MinDepth())) + srcViewport.MinDepth()};
  }


  Vector3 Unproject(const Viewport& srcViewport, const Vector3& srcCoordinate, const Matrix& projection, const Matrix& view, const Matrix& world)
  {
    Matrix invertedMatrix = Matrix::Invert(Matrix::Multiply(Matrix::Multiply(world, view), projection));
    const Vector3 srcVec((((srcCoordinate.X - static_cast<float>(srcViewport.X())) / static_cast<float>(srcViewport.Width())) * 2.0f) - 1.0f,
                         -((((srcCoordinate.Y - static_cast<float>(srcViewport.Y())) / static_cast<float>(srcViewport.Height())) * 2.0f) - 1.0f),
                         (srcCoordinate.Z - srcViewport.MinDepth()) / (srcViewport.MaxDepth() - srcViewport.MinDepth()));
    const float val = Calc(invertedMatrix, srcVec);
    Vector3 vector = Vector3::Transform(srcVec, invertedMatrix);
    return !WithinEpsilon(val, 1.0f) ? vector / val : vector;
  }
}
