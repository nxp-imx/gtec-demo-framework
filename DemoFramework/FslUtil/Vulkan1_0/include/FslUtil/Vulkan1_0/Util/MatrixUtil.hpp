#ifndef FSLUTIL_VULKAN1_0_UTIL_MATRIXUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_MATRIXUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

namespace Fsl
{
  namespace Vulkan
  {
    namespace MatrixUtil
    {
      //! @brief Get a matrix that can be used to convert a projection matrix to be compatible with Vulkan
      //! @note  If you are using the Matrix::CreateProjection methods this are the one you want.
      //!        - https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
      //!        - https://vulkan.lunarg.com/doc/sdk/1.1.85.0/windows/tutorial/html/07-init_uniform_buffer.html
      //!        - http://anki3d.org/vulkan-coordinate-system/
      inline Matrix GetClipMatrixForDepthZeroToOne()
      {
        // Vulkan clip space has inverted Y.
        return Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
      }

      //! @brief Get a matrix that can be used to convert a projection matrix to be compatible with Vulkan
      inline Matrix GetClipMatrixForDepthMinusOneToOne()
      {
        // Vulkan clip space has inverted Y and half Z.
        return Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);
      }

      //! @brief Get a matrix that can be used to convert a projection matrix to be compatible with Vulkan
      //! @note  If you are using the Matrix::CreateProjection methods this are the one you want.
      //!        - https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
      //!        - https://vulkan.lunarg.com/doc/sdk/1.1.85.0/windows/tutorial/html/07-init_uniform_buffer.html
      //!        - http://anki3d.org/vulkan-coordinate-system/
      inline Matrix GetClipMatrix()
      {
        // Vulkan clip space has inverted Y.
        return GetClipMatrixForDepthZeroToOne();
      }
    }
  }
}

#endif
