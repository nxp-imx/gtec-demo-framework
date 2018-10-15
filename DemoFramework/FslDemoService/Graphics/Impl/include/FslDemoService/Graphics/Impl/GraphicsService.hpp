#ifndef FSLDEMOSERVICE_GRAPHICS_IMPL_GRAPHICSSERVICE_HPP
#define FSLDEMOSERVICE_GRAPHICS_IMPL_GRAPHICSSERVICE_HPP
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

#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/Graphics/Control/IGraphicsServiceControl.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <memory>

namespace Fsl
{
  class INativeGraphicsService;
  class INativeGraphicsBasic2D;

  class GraphicsService
    : public ThreadLocalService
    , public IGraphicsService
    , public IGraphicsServiceControl
  {
    using NativeGraphicsServiceDeque = std::deque<std::shared_ptr<INativeGraphicsService>>;
    NativeGraphicsServiceDeque m_nativeGraphicsServices;

    std::shared_ptr<INativeGraphicsService> m_nativeService;
    std::shared_ptr<INativeGraphicsBasic2D> m_nativBasic2D;
    std::shared_ptr<IBasic2D> m_basic2D;
    std::shared_ptr<INativeBatch2D> m_nativeBatch2D;
    Point2 m_resolution;

  public:
    GraphicsService(const ServiceProvider& serviceProvider);
    ~GraphicsService() override;

    // From IGraphicsService
    void Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat) override;
    void Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const Rectangle& srcRectangle) override;
    std::shared_ptr<IBasic2D> GetBasic2D() override;
    std::shared_ptr<INativeBatch2D> GetNativeBatch2D() override;
    std::shared_ptr<INativeGraphics> GetNativeGraphics() override;

    // From IGraphicsServiceControl
    void Reset() override;
    void Configure(const DemoHostFeature& activeAPI) override;
    void SetScreenResolution(const Point2& resolution, const bool preallocateBasic2D) override;

  private:
  };
}

#endif
