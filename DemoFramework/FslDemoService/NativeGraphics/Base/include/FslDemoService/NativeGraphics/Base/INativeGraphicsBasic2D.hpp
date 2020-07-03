#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_BASE_INATIVEGRAPHICSBASIC2D_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_BASE_INATIVEGRAPHICSBASIC2D_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>

namespace Fsl
{
  struct Color;
  struct Vector2;

  //! @brief This represents the minimal set of drawing operations that are supported on all hardware.
  class INativeGraphicsBasic2D
  {
  public:
    virtual ~INativeGraphicsBasic2D() = default;

    //! @brief Update the current resolution (can not be called during a begin/end block)
    virtual void SetScreenExtent(const PxExtent2D& currentExtentPx) = 0;

    //! @brief Begin the draw sequence (all draw calls must occur inside a begin/end block. Begin end blocks can not be nested!
    virtual void Begin() = 0;
    virtual void End() = 0;

    //! @brief Draw a array of points
    virtual void DrawPoints(const Vector2* const pDst, const uint32_t length, const Color& color) = 0;

    //! @brief Draw the string at position using a monospaced font (the font is chosen by the back-end)
    //! @param characters a array of characters
    //! @param length the number of characters that the 'characters' contain.
    //! @param position the position to draw the string at
    //! @param the color to use for rendering
    //! @note This is a minimal string rendering routine
    virtual void DrawString(const StringViewLite& strView, const Vector2& dstPosition) = 0;

    //! @brief Get the size of one monospaced font character
    virtual PxSize2D FontSize() const = 0;
  };
}

#endif
