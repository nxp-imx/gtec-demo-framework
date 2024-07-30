/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <Shared/Camera/Adapter/Interface/ICameraAdapter.hpp>
#include <Shared/Camera/System/Camera.hpp>
#include <cassert>
#include <utility>

namespace Fsl::Helios
{
  Camera::Camera(const CameraConfig& cameraConfig, std::shared_ptr<ICameraAdapter> nativeCamera)
    : m_cameraConfig(cameraConfig)
    , m_nativeCamera(std::move(nativeCamera))
  {
    if (cameraConfig.NativePixelFormat == PixelFormat::Undefined)
    {
      throw std::invalid_argument("CameraConfig.NativePixelFormat can not be PixelFormat::Undefined");
    }
    if (cameraConfig.NativeStride < PixelFormatUtil::CalcMinimumStride(m_cameraConfig.Extent.Width, m_cameraConfig.NativePixelFormat))
    {
      throw std::invalid_argument("CameraConfig.NativeStride is smaller than allowed");
    }
    if (!m_nativeCamera)
    {
      throw std::invalid_argument("nativeCamera can not be null");
    }
  }

  Camera::~Camera() = default;


  PxSize2D Camera::GetSize() const noexcept
  {
    return TypeConverter::UncheckedTo<PxSize2D>(m_cameraConfig.Extent);
  }


  bool Camera::TryRender(RawBitmapEx& rTargetBitmap, uint32_t& rFrameId)
  {
    if (rTargetBitmap.GetExtent() != m_cameraConfig.Extent)
    {
      throw std::invalid_argument("rTargetBitmap extent does not match the native extent");
    }
    if (rTargetBitmap.Stride() != m_cameraConfig.NativeStride)
    {
      throw std::invalid_argument("rTargetBitmap stride does not match the native stride");
    }
    if (rTargetBitmap.GetPixelFormat() != m_cameraConfig.NativePixelFormat)
    {
      throw std::invalid_argument("rTargetBitmap PixelFormat does not match the native PixelFormat");
    }

    // We check this during construction
    assert(m_nativeCamera);
    bool result = m_nativeCamera->TryRender(rTargetBitmap, rFrameId);
    if (result && rFrameId == 0)
    {
      throw std::runtime_error("The camera system returned a invalid frame id");
    }
    return result;
  }
}
