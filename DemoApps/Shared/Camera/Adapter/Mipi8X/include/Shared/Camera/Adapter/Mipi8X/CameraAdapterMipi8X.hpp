#ifndef SHARED_CAMERA_ADAPTER_MIPI8X_CAMERAADAPTERMIPI8X_HPP
#define SHARED_CAMERA_ADAPTER_MIPI8X_CAMERAADAPTERMIPI8X_HPP
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

#include <Shared/Camera/Adapter/Interface/ICameraAdapter.hpp>
#include <Shared/Camera/Adapter/Interface/CameraAdapterAllocateInfo.hpp>

namespace Fsl
{
  namespace Helios
  {
    class CameraAdapterMipi8X : public ICameraAdapter
    {
      Extent2D m_extent;
      PixelFormat m_pixelFormat;
      int m_cam_num;
      uint32_t m_frameId;

    public:
      CameraAdapterMipi8X(const CameraAdapterMipi8X&) = delete;
      CameraAdapterMipi8X& operator=(const CameraAdapterMipi8X&) = delete;

      CameraAdapterMipi8X(const CameraAdapterAllocateInfo& allocateInfo);
      ~CameraAdapterMipi8X();

      virtual CameraAdapterConfig GetConfig() const override;
      virtual bool TryRender(RawBitmapEx& rTargetBitmap, uint32_t& rFrameId) override;
    };
  }
}
#endif
