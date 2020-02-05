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

#include <Shared/Camera/Adapter/OpenCV/CameraAdapterOpenCV.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtExtent2D.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <cassert>

namespace Fsl
{
  namespace Helios
  {
    namespace
    {
      const int DEFAULT_CAMERA_DEVICE = 0;
    }

    CameraAdapterOpenCV::CameraAdapterOpenCV(const CameraAdapterAllocateInfo& allocateInfo)
      : m_cap(DEFAULT_CAMERA_DEVICE)
      , m_vidFrame()
      , m_config(allocateInfo.Extent, PixelFormat::R8G8B8_UNORM)
      , m_frameId(0)
    {
      if (!m_cap.isOpened())
      {
        throw std::runtime_error("Failed to open camera device");
      }

      const double desiredFPS = 60.0;

      // Camera is open by this time, let's fetch its features
      const double defaultWidth = std::max(m_cap.get(cv::CAP_PROP_FRAME_WIDTH), 0.0);
      const double defaultHeight = std::max(m_cap.get(cv::CAP_PROP_FRAME_HEIGHT), 0.0);
      const double defaultFormat = m_cap.get(cv::CAP_PROP_FORMAT);
      const double defaultFPS = m_cap.get(cv::CAP_PROP_FPS);
      FSLLOG3_VERBOSE("Default settings:");
      FSLLOG3_VERBOSE("Width: {}", defaultWidth);
      FSLLOG3_VERBOSE("Height: {}", defaultHeight);
      FSLLOG3_VERBOSE("Format: {}", defaultFormat);
      FSLLOG3_VERBOSE("FPS: {}", defaultFPS);

      const Extent2D defaultExtent(static_cast<uint32_t>(defaultWidth), static_cast<uint32_t>(defaultHeight));

      // Only modify
      if (allocateInfo.Flags.IsEnabled(CameraAdapterAllocateFlags::CustomExtent) && defaultExtent != allocateInfo.Extent)
      {
        FSLLOG3_VERBOSE("Trying to set custom resolution: {}", allocateInfo.Extent);
        // WARNING: this is not something that works for all open cv cameras
        m_cap.set(cv::CAP_PROP_FRAME_WIDTH, m_config.Extent.Width);
        m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, m_config.Extent.Height);

        const double actualWidth = m_cap.get(cv::CAP_PROP_FRAME_WIDTH);
        const double actualHeight = m_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        const Extent2D actualExtent(static_cast<uint32_t>(actualWidth), static_cast<uint32_t>(actualHeight));
        if (actualExtent != allocateInfo.Extent)
        {
          FSLLOG3_WARNING("Failed to set custom resolution using: {}", actualExtent);
          m_config = CameraAdapterConfig(actualExtent, m_config.ActivePixelFormat);
        }
      }
      else
      {
        m_config = CameraAdapterConfig(defaultExtent, m_config.ActivePixelFormat);
        FSLLOG3_VERBOSE("Using default resolution: {}", m_config.Extent);
      }

      if (defaultFPS < desiredFPS)
      {
        FSLLOG3_VERBOSE("Trying to set custom FPS: {}", desiredFPS)
        m_cap.set(cv::CAP_PROP_FRAME_WIDTH, desiredFPS);
        const double actualFPS = m_cap.get(cv::CAP_PROP_FPS);
        if (actualFPS != desiredFPS)
        {
          FSLLOG3_WARNING("Failed to set FPS using: {}", actualFPS);
        }
      }

      // FIX: for now we always request a new format
      m_cap.set(cv::CAP_PROP_FORMAT, CV_8UC3);
      const double actualFormat = m_cap.get(cv::CAP_PROP_FORMAT);
      FSLLOG3_VERBOSE("Format: {}", actualFormat);
    }

    CameraAdapterOpenCV::~CameraAdapterOpenCV() = default;

    CameraAdapterConfig CameraAdapterOpenCV::GetConfig() const
    {
      return m_config;
    }

    bool CameraAdapterOpenCV::TryRender(RawBitmapEx& rTargetBitmap, uint32_t& rFrameId)
    {
      // The camera class will have checked these so we just have asserts here.
      assert(rTargetBitmap.GetExtent() == m_config.Extent);
      assert(rTargetBitmap.Stride() == m_config.Stride);
      assert(rTargetBitmap.GetPixelFormat() == m_config.ActivePixelFormat);
      rFrameId = 0;

      bool hasNewFrame = m_cap.read(m_vidFrame);
      if (!hasNewFrame)
      {
        FSLLOG3_DEBUG_WARNING("Failed to read a new frame");
        return false;
      }

      ++m_frameId;
      if (m_frameId == 0)
      {
        ++m_frameId;
      }

      // FIX: proper conversion code might be able to skip this
      cv::cvtColor(m_vidFrame, m_vidFrame, cv::COLOR_BGR2RGB);

      // FIX: switch this to more robust conversion code
      if (!m_vidFrame.isContinuous())
      {
        FSLLOG3_WARNING("Unhandled: internal vidframe is not continuous");
        return false;
      }

      // Copy the data to the targetBitmap
      switch (rTargetBitmap.GetOrigin())
      {
      case BitmapOrigin::Undefined:
      case BitmapOrigin::UpperLeft:
        std::memcpy(rTargetBitmap.Content(), m_vidFrame.ptr(), m_config.Extent.Height * m_config.Stride);
        break;
      case BitmapOrigin::LowerLeft:
      {
        // Do a 'horizontal' flip during the copy
        auto* pDst = static_cast<uint8_t*>(rTargetBitmap.Content());
        const auto* const pSrcStart = static_cast<const uint8_t*>(m_vidFrame.ptr());
        const uint8_t* pSrc = pSrcStart + ((m_config.Extent.Height - 1) * m_config.Stride);
        const auto stride = m_config.Stride;
        const auto bytesPerScanline = PixelFormatUtil::CalcMinimumStride(m_config.Extent.Width, m_config.ActivePixelFormat);
        while (pSrc >= pSrcStart)
        {
          std::memcpy(pDst, pSrc, bytesPerScanline);
          pSrc -= stride;
          pDst += stride;
        }
        break;
      }
      default:
        FSLLOG3_WARNING("Unsupported bitmap origin");
        return false;
      }
      rFrameId = m_frameId;
      return true;
    }
  }
}
