#ifdef FSL_ENABLE_GRAPHICS_OPENVG
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

#include <FslDemoService/NativeGraphics/OpenVG/NativeGraphicsService.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslUtil/OpenVG/VGUtil.hpp>
#include "NativeGraphicsBasic2D.hpp"

namespace Fsl
{
  namespace OpenVG
  {
    NativeGraphicsService::NativeGraphicsService(const ServiceProvider& serviceProvider)
      : ANativeGraphicsServiceNo3D(serviceProvider)
    {
    }


    NativeGraphicsService::~NativeGraphicsService() = default;


    bool NativeGraphicsService::IsSupported(const DemoHostFeature& activeAPI) const
    {
      if (activeAPI.Name != DemoHostFeatureName::OpenVG)
      {
        return false;
      }

      int major = 0;
      int minor = 0;
      DemoHostFeatureUtil::DecodeOpenVGVersion(activeAPI.Version, major, minor);
      return major == 1;
    }


    void NativeGraphicsService::Capture(Bitmap& rBitmap, const Rectangle& srcRectangle)
    {
      OpenVG::VGUtil::Capture(rBitmap, PixelFormat::R8G8B8A8_UINT, srcRectangle);
    }


    std::shared_ptr<INativeGraphicsBasic2D> NativeGraphicsService::CreateBasic2D(const PxExtent2D& extentPx)
    {
      return std::shared_ptr<INativeGraphicsBasic2D>(new NativeGraphicsBasic2D(extentPx));
    }
  }
}
#endif
