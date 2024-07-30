/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

// OpenCL 1.2 project

#include "Colorseg.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/TypeConverter_Double.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslBase/System/HighResolutionTimerUtil.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslUtil/OpenCL1_2/OpenCLHelper.hpp>
#include <FslUtil/OpenCL1_2/ProgramEx.hpp>
#include <Shared/OpenCL/Helper/BitmapHelper.hpp>
#include <Shared/OpenCL/Helper/TightPlanarBitmapR8G8B8.hpp>
#include <CL/cl.h>
#include <array>
#include <cstring>
#include "OptionParser.hpp"

namespace Fsl
{
  namespace
  {
    namespace BufferConfig
    {
      // Buffer index
      constexpr std::size_t SrcBufferIndex = 0;
      constexpr std::size_t DstBufferIndex = 1;
      // The maximum amount of buffers used
      constexpr std::size_t MaxBufferCount = 2;
    }

    // RGB format byte size
    namespace PixelFormatByteSize
    {
      constexpr std::size_t BppRgb888 = 3;
    }

    // Conversion
    enum class Conversion : uint8_t
    {
      RgbToHsv = 0,
      HsvToRgb = 1
    };

    std::vector<RapidOpenCL1::Buffer> AllocateMemory(const cl_context context, const std::size_t numPixels)
    {
      std::vector<RapidOpenCL1::Buffer> deviceImg(BufferConfig::MaxBufferCount);
      deviceImg[BufferConfig::SrcBufferIndex].Reset(context, CL_MEM_READ_WRITE, numPixels * PixelFormatByteSize::BppRgb888, nullptr);
      deviceImg[BufferConfig::DstBufferIndex].Reset(context, CL_MEM_READ_WRITE, numPixels * PixelFormatByteSize::BppRgb888, nullptr);
      return deviceImg;
    }
  }


  Colorseg::Colorseg(const DemoAppConfig& config)
    : DemoAppOpenCL(config)
    , m_context(CL_DEVICE_TYPE_GPU)
  {
    auto optionParser = config.GetOptions<OptionParser>();
    m_bmpPath = IO::Path(optionParser->GetBmpFileName());
    m_hsvOutputPath = IO::Path(optionParser->GetHsvOutputFileName());
    m_rgbOutputPath = IO::Path(optionParser->GetRgbOutputFileName());

    m_srcBitmap = TightBitmap(GetContentManager()->ReadBitmap(m_bmpPath, PixelFormat::R8G8B8_UNORM).Release());
  }


  Colorseg::~Colorseg() = default;


  void Colorseg::Run()
  {
    const auto imgWidth = UncheckedNumericCast<cl_int>(m_srcBitmap.RawWidth());
    const auto imgHeight = UncheckedNumericCast<cl_int>(m_srcBitmap.RawHeight());

    const auto srcPixelCount = UncheckedNumericCast<std::size_t>(m_srcBitmap.GetPixelCount().RawValue());
    std::vector<RapidOpenCL1::Buffer> deviceData = AllocateMemory(m_context.Get(), srcPixelCount);
    const std::string strProgram = GetContentManager()->ReadAllText("colorconversions.cl");
    OpenCL::ProgramEx program(m_context.Get(), m_context.GetDeviceId(), strProgram);
    RapidOpenCL1::Kernel kernels(program.Get(), "rgb_to_hsv_conversions");

    std::vector<uint8_t> hsvData(srcPixelCount * PixelFormatByteSize::BppRgb888);
    std::vector<uint8_t> finalRgbData(srcPixelCount * PixelFormatByteSize::BppRgb888);

    FSLLOG3_INFO("Creating kernels...");
    FSLLOG3_INFO("Please wait for compiling and building kernels, about one minute...");
    auto cvtcolorFlag = Conversion::RgbToHsv;

    clSetKernelArg(kernels.Get(), 0, sizeof(cl_mem), deviceData[BufferConfig::SrcBufferIndex].GetPointer());
    clSetKernelArg(kernels.Get(), 1, sizeof(cl_mem), deviceData[BufferConfig::DstBufferIndex].GetPointer());
    clSetKernelArg(kernels.Get(), 2, sizeof(int), &imgWidth);
    clSetKernelArg(kernels.Get(), 3, sizeof(int), &imgHeight);
    clSetKernelArg(kernels.Get(), 4, sizeof(uint8_t), &cvtcolorFlag);
    {
      constexpr const int Dimension = 2;
      const std::array<std::size_t, Dimension> global = {m_srcBitmap.RawUnsignedWidth(), m_srcBitmap.RawUnsignedHeight()};
      constexpr std::array<std::size_t, Dimension> Local = {4, 16};

      FSLLOG3_INFO("Creating Command Queue...");
      RapidOpenCL1::CommandQueue commandQueue(m_context.Get(), m_context.GetDeviceId(), 0);

      const auto srcBitmapSpan = m_srcBitmap.AsSpan();
      HighResolutionTimer timer;
      FSLLOG3_INFO("Convert RGB to HSV...");
      auto startTime = timer.GetNativeTicks();
      RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), deviceData[BufferConfig::SrcBufferIndex].Get(), CL_TRUE, 0, srcBitmapSpan.size(),
                                             srcBitmapSpan.data(), 0, nullptr, nullptr));
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(commandQueue.Get(), kernels.Get(), Dimension, nullptr, global.data(), Local.data(), 0, nullptr, nullptr));
      RAPIDOPENCL_CHECK(clEnqueueReadBuffer(commandQueue.Get(), deviceData[BufferConfig::DstBufferIndex].Get(), CL_TRUE, 0, hsvData.size(),
                                            hsvData.data(), 0, nullptr, nullptr));
      clFinish(commandQueue.Get());
      auto endTime = timer.GetNativeTicks();
      TimeSpan timeSpan = HighResolutionTimerUtil::ToTimeSpan(endTime - startTime, timer.GetNativeTickFrequency());
      FSLLOG3_INFO("Process time: {} ms", TimeSpanUtil::ToMillisecondsUInt64(timeSpan));

      for (cl_int i = 0; i < static_cast<cl_int>(imgWidth * imgHeight); i++)
      {
        // segmenting the blue color
        if (hsvData[i * 3 + 1] < 30)
        {
          hsvData[i * 3] = 0;
          hsvData[i * 3 + 1] = 0;
          hsvData[i * 3 + 2] = 0;
        }
        else if (((hsvData[i * 3]) <= 90) || ((hsvData[i * 3]) >= 140))
        {
          hsvData[i * 3] = 0;
          hsvData[i * 3 + 1] = 0;
          hsvData[i * 3 + 2] = 0;
        }
      }

      startTime = 0;
      endTime = 0;
      cvtcolorFlag = Conversion::HsvToRgb;
      clSetKernelArg(kernels.Get(), 4, sizeof(uint8_t), &cvtcolorFlag);
      FSLLOG3_INFO("Convert HSV to RGB...");
      startTime = timer.GetNativeTicks();
      RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), deviceData[BufferConfig::DstBufferIndex].Get(), CL_TRUE, 0, hsvData.size(),
                                             hsvData.data(), 0, nullptr, nullptr));
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(commandQueue.Get(), kernels.Get(), Dimension, nullptr, global.data(), Local.data(), 0, nullptr, nullptr));
      RAPIDOPENCL_CHECK(clEnqueueReadBuffer(commandQueue.Get(), deviceData[BufferConfig::SrcBufferIndex].Get(), CL_TRUE, 0, finalRgbData.size(),
                                            finalRgbData.data(), 0, nullptr, nullptr));
      clFinish(commandQueue.Get());
      endTime = timer.GetNativeTicks();
      timeSpan = HighResolutionTimerUtil::ToTimeSpan(endTime - startTime, timer.GetNativeTickFrequency());
      FSLLOG3_INFO("Process time: {} ms", TimeSpanUtil::ToMillisecondsUInt64(timeSpan));
    }

    Bitmap rgbImage = Bitmap(std::move(finalRgbData), m_srcBitmap.GetExtent(), m_srcBitmap.GetPixelFormat(), m_srcBitmap.GetOrigin());
    Bitmap hsvImage = Bitmap(std::move(hsvData), m_srcBitmap.GetExtent(), m_srcBitmap.GetPixelFormat(), m_srcBitmap.GetOrigin());

    FSLLOG3_INFO("Saving HSV images...");
    GetPersistentDataManager()->Write(m_hsvOutputPath, hsvImage, ImageFormat::Bmp, PixelFormat::B8G8R8_UNORM);
    FSLLOG3_INFO("Saving RGB images...");
    GetPersistentDataManager()->Write(m_rgbOutputPath, rgbImage, ImageFormat::Bmp, PixelFormat::B8G8R8_UNORM);
  }
}
