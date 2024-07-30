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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoService/NativeGraphics/G2D/NativeGraphicsService.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include "DynamicNativeTexture2D.hpp"
#include "NativeGraphicsBasic2D.hpp"

namespace Fsl::G2D
{
  NativeGraphicsService::NativeGraphicsService(const ServiceProvider& serviceProvider)
    : ANativeGraphicsServiceNo3D(serviceProvider)
  {
    FSLLOG3_WARNING("NativeGraphicsService is mostly a stub");
  }


  NativeGraphicsService::~NativeGraphicsService() = default;


  bool NativeGraphicsService::IsSupported(const DemoHostFeature& activeAPI) const
  {
    return (activeAPI.Name != DemoHostFeatureName::G2D);

    // int major, minor;
    // DemoHostFeatureUtil::DecodeG2DVersion(activeAPI.Version, major, minor);
    // return true;
  }


  void NativeGraphicsService::Capture(Bitmap& rBitmap, const PxRectangle& /*srcRectanglePx*/)
  {
    FSLLOG3_WARNING("G2D::NativeGraphicsService.Capture not implemented");
  }


  std::shared_ptr<INativeGraphicsBasic2D> NativeGraphicsService::CreateBasic2D(const PxExtent2D& extentPx)
  {
    return std::shared_ptr<INativeGraphicsBasic2D>(new NativeGraphicsBasic2D(extentPx));
  }
}
