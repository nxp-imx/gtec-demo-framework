/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <Shared/ObjectSelection/BoundingBoxUtil.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <array>

namespace Fsl
{
  namespace BoundingBoxUtil
  {
    BoundingBox CalculateAABB(const Matrix& matrix, const BoundingBox& box)
    {
      const std::array<Vector3, 8> vertices = {Vector3(box.Min.X, box.Min.Y, box.Min.Z), Vector3(box.Max.X, box.Min.Y, box.Min.Z),
                                               Vector3(box.Min.X, box.Max.Y, box.Min.Z), Vector3(box.Max.X, box.Max.Y, box.Min.Z),

                                               Vector3(box.Min.X, box.Min.Y, box.Max.Z), Vector3(box.Max.X, box.Min.Y, box.Max.Z),
                                               Vector3(box.Min.X, box.Max.Y, box.Max.Z), Vector3(box.Max.X, box.Max.Y, box.Max.Z)};

      Vector3 min = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
      Vector3 max = Vector3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

      for (uint32_t i = 0; i < 8; ++i)
      {
        const auto entry = Vector4::Transform(vertices[i], matrix);
        min.X = (entry.X < min.X ? entry.X : min.X);
        min.Y = (entry.Y < min.Y ? entry.Y : min.Y);
        min.Z = (entry.Z < min.Z ? entry.Z : min.Z);

        max.X = (entry.X > max.X ? entry.X : max.X);
        max.Y = (entry.Y > max.Y ? entry.Y : max.Y);
        max.Z = (entry.Z > max.Z ? entry.Z : max.Z);
      }
      return {min, max};
    }
  }
}
