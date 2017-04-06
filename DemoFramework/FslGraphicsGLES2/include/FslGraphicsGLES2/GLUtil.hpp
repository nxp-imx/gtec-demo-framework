#ifndef FSLGRAPHICSGLES2_GLUTIL_HPP
#define FSLGRAPHICSGLES2_GLUTIL_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslGraphicsGLES2/Common.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/PrimitiveType.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <GLES2/gl2.h>

namespace Fsl
{
  class Bitmap;

  namespace GLES2
  {
    class GLUtil
    {
    public:

      //! @brief Check if the given OpenGLES extension is available
      //! @note Do not expect mind blowing performance from this!
      //! @note If you need to check for multiple extensions consider rolling your own or wait for the helper method to get added :)
      static bool HasExtension(const char*const pszExtensionName);

      //! @brief Capture the current content of the display after a glFinish.
      //! @param rBitmap will be resized to fit the screen
      //! @param pixelFormat the desired pixel format (R8G8B8A8_UINT or B8G8R8A8_UINT)
      //! @note  Uses the current viewport to determine the capture size
      //         Throws a exception if rBitmap has a unsupported bitmap format
      static void Capture(Bitmap& rBitmap, const PixelFormat pixelFormat);

      //! @brief Capture the current content of the display after a glFinish.
      //! @param rBitmap will be resized to the srcRectangle dimensions.
      //! @param pixelFormat the desired pixel format (R8G8B8A8_UINT or B8G8R8A8_UINT)
      //! @note  Throws a exception if rBitmap has a unsupported bitmap format
      static void Capture(Bitmap& rBitmap, const PixelFormat pixelFormat, const Rectangle& srcRectangle);

      //! @brief Convert the primitive type to the corresponding GL primitive type
      static GLenum Convert(const PrimitiveType primitiveType);
    };
  }
}

#endif
