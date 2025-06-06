#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSSERVICE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSSERVICE_HPP
#ifdef FSL_ENABLE_GRAPHICS_ES3
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

#include <FslDemoService/NativeGraphics/BasicRender/ANativeGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsDevice.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>

namespace Fsl::GLES3
{
  class NativeGraphicsService final : public ANativeGraphicsService
  {
    std::shared_ptr<NativeGraphicsDevice> m_device;

  public:
    explicit NativeGraphicsService(const ServiceProvider& serviceProvider);
    ~NativeGraphicsService() final;

    // From INativeGraphicsService
    bool IsSupported(const DemoHostFeature& activeAPI) const final;
    void Capture(Bitmap& rBitmap, const PxRectangle& srcRectanglePx) final;
    std::shared_ptr<INativeGraphicsBasic2D> CreateBasic2D(const PxExtent2D& extentPx) final;
    std::shared_ptr<INativeBatch2D> CreateNativeBatch2D(const PxExtent2D& extentPx) final;

    // INativeGraphicsServiceControl
    void CreateDevice(const NativeGraphicsDeviceCreateInfo& createInfo) final;
    void DestroyDevice() noexcept final;

  protected:
    std::shared_ptr<Graphics3D::INativeDevice> GetNativeDevice() final;
  };
}

#endif
#endif
