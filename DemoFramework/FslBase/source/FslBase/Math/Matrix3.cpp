/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3Fields.hpp>
#include <cassert>
#include <cmath>

namespace Fsl
{
  using namespace Matrix3Fields;


  Matrix3 Matrix3::Invert(const Matrix3& matrix)
  {
    const float* const pMatrix = matrix.m;

    const double determinant = matrix.Determinant();
    const double invDet = 1.0 / determinant;

    // Matrix3 result(OptimizationFlag::NoInitialization);
    Matrix3 result;
    result.m[_M11] = static_cast<float>(invDet * (pMatrix[_M22] * pMatrix[_M33] - pMatrix[_M23] * pMatrix[_M32]));
    result.m[_M21] = static_cast<float>(invDet * (pMatrix[_M23] * pMatrix[_M31] - pMatrix[_M21] * pMatrix[_M33]));
    result.m[_M31] = static_cast<float>(invDet * (pMatrix[_M21] * pMatrix[_M32] - pMatrix[_M22] * pMatrix[_M31]));
    result.m[_M12] = static_cast<float>(invDet * (pMatrix[_M13] * pMatrix[_M32] - pMatrix[_M12] * pMatrix[_M33]));
    result.m[_M22] = static_cast<float>(invDet * (pMatrix[_M11] * pMatrix[_M33] - pMatrix[_M13] * pMatrix[_M31]));
    result.m[_M32] = static_cast<float>(invDet * (pMatrix[_M12] * pMatrix[_M31] - pMatrix[_M11] * pMatrix[_M32]));
    result.m[_M13] = static_cast<float>(invDet * (pMatrix[_M12] * pMatrix[_M23] - pMatrix[_M13] * pMatrix[_M22]));
    result.m[_M23] = static_cast<float>(invDet * (pMatrix[_M13] * pMatrix[_M21] - pMatrix[_M11] * pMatrix[_M23]));
    result.m[_M33] = static_cast<float>(invDet * (pMatrix[_M11] * pMatrix[_M22] - pMatrix[_M12] * pMatrix[_M21]));
    return result;
  }
}
