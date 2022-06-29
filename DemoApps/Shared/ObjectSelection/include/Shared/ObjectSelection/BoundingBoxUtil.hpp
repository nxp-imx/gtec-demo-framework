#ifndef SHARED_OBJECTSELECTION_BOUNDINGBOXUTIL_HPP
#define SHARED_OBJECTSELECTION_BOUNDINGBOXUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <limits>
#include <vector>

namespace Fsl::BoundingBoxUtil
{
  template <typename TVertex>
  inline BoundingBox CalculateBoundingBox(const std::vector<TVertex>& vertices)
  {
    Vector3 min = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vector3 max = Vector3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (const auto& entry : vertices)
    {
      min.X = (entry.Position.X < min.X ? entry.Position.X : min.X);
      min.Y = (entry.Position.Y < min.Y ? entry.Position.Y : min.Y);
      min.Z = (entry.Position.Z < min.Z ? entry.Position.Z : min.Z);
      max.X = (entry.Position.X > max.X ? entry.Position.X : max.X);
      max.Y = (entry.Position.Y > max.Y ? entry.Position.Y : max.Y);
      max.Z = (entry.Position.Z > max.Z ? entry.Position.Z : max.Z);
    }
    return {min, max};
  }


  BoundingBox CalculateAABB(const Matrix& matrix, const BoundingBox& box);
}

#endif
