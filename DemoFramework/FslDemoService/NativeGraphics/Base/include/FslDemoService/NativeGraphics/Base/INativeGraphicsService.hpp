#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_BASE_INATIVEGRAPHICSSERVICE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_BASE_INATIVEGRAPHICSSERVICE_HPP
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
#include <FslBase/Math/Rectangle.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeature.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>

namespace Fsl
{
  class Bitmap;
  class INativeGraphicsBasic2D;
  class INativeBatch2D;

  class INativeGraphicsService : public INativeGraphics
  {
  public:
    ~INativeGraphicsService() override = default;

    //! @brief Check if the given API is supported by this native graphics service
    virtual bool IsSupported(const DemoHostFeature& activeAPI) const = 0;

    //! @brief Capture the current content of the display after a glFinish.
    //! @param rBitmap will be resized to the srcRectangle dimensions and filled with the content of the back buffer.
    virtual void Capture(Bitmap& rBitmap, const Rectangle& srcRectangle) = 0;

    //! @brief Create a basic 2d instance
    virtual std::shared_ptr<INativeGraphicsBasic2D> CreateBasic2D(const Point2& currentResolution) = 0;

    //! @brief Create a NativeBatch2d instance
    virtual std::shared_ptr<INativeBatch2D> CreateNativeBatch2D(const Point2& currentResolution) = 0;
  };
}

#endif
