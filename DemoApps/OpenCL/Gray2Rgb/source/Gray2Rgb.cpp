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

#include "Gray2Rgb.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslBase/System/HighResolutionTimerUtil.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslUtil/OpenCL1_2/OpenCLHelper.hpp>
#include <FslUtil/OpenCL1_2/ProgramEx.hpp>
#include <RapidOpenCL1/Check.hpp>
#include <RapidOpenCL1/Values.hpp>
#include <CL/cl.h>
#include <array>
#include <cstring>
#include <vector>
#include "OptionParser.hpp"

namespace Fsl
{
  namespace
  {
    std::vector<RapidOpenCL1::Buffer> AllocateMemory(const cl_context context, const std::size_t size)
    {
      std::vector<RapidOpenCL1::Buffer> deviceImg(2);

      deviceImg[0].Reset(context, CL_MEM_READ_WRITE, size * 3, nullptr);
      deviceImg[1].Reset(context, CL_MEM_READ_WRITE, size, nullptr);
      return deviceImg;
    }
  }

  Gray2Rgb::Gray2Rgb(const DemoAppConfig& config)
    : DemoAppOpenCL(config)
    , m_context(CL_DEVICE_TYPE_GPU)
  {
    auto optionParser = config.GetOptions<OptionParser>();
    m_bmpPath = IO::Path(optionParser->GetBmpFileName());
    m_outputPath = IO::Path(optionParser->GetOutputFileName());

    m_srcBitmap = TightBitmap(GetContentManager()->ReadBitmap(m_bmpPath, PixelFormat::R8G8B8_UNORM).Release());
  }


  Gray2Rgb::~Gray2Rgb() = default;


  void Gray2Rgb::Run()
  {
    HighResolutionTimer timer;
    const std::size_t sourceImagePixelCount = m_srcBitmap.GetPixelCount().RawUnsignedValue();

    std::vector<RapidOpenCL1::Buffer> deviceImg = AllocateMemory(m_context.Get(), sourceImagePixelCount);

    const std::string program = GetContentManager()->ReadAllText("colorconversions.cl");
    OpenCL::ProgramEx rgbProgram(m_context.Get(), m_context.GetDeviceId(), program);
    OpenCL::ProgramEx grayProgram(m_context.Get(), m_context.GetDeviceId(), program);

    FSLLOG3_INFO("Creating kernels...");
    FSLLOG3_INFO("Please wait for compiling and building kernels, about one minute...");
    std::vector<RapidOpenCL1::Kernel> kernels(2);
    kernels[0].Reset(rgbProgram.Get(), "rgb_to_gray");
    kernels[1].Reset(grayProgram.Get(), "gray_to_rgb");

    assert(m_srcBitmap.GetBytesPerPixel() == 3u);

    std::vector<uint8_t> rgbBuffer(m_srcBitmap.GetByteSize());

    clSetKernelArg(kernels[0].Get(), 0, sizeof(cl_mem), deviceImg[0].GetPointer());
    clSetKernelArg(kernels[0].Get(), 1, sizeof(cl_mem), deviceImg[1].GetPointer());
    const auto imgWidth = UncheckedNumericCast<cl_int>(m_srcBitmap.RawWidth());
    clSetKernelArg(kernels[0].Get(), 2, sizeof(cl_int), &imgWidth);
    const auto imgHeight = UncheckedNumericCast<cl_int>(m_srcBitmap.RawHeight());
    clSetKernelArg(kernels[0].Get(), 3, sizeof(cl_int), &imgHeight);
    {
      constexpr const int Dimension = 2;
      const std::array<std::size_t, Dimension> global = {m_srcBitmap.RawUnsignedWidth(), m_srcBitmap.RawUnsignedHeight()};
      constexpr std::array<std::size_t, Dimension> Local = {4, 16};

      FSLLOG3_INFO("Creating Command Queue...");
      RapidOpenCL1::CommandQueue commandQueue(m_context.Get(), m_context.GetDeviceId(), 0);

      FSLLOG3_INFO("Running RGB to GRAY kernel...");
      // Since we only use one byte per pixel, we can use the pixel count for size
      std::vector<char> grayBuffer(sourceImagePixelCount);
      auto startTime = timer.GetNativeTicks();

      auto srcBitmapSpan = m_srcBitmap.AsSpan();
      RAPIDOPENCL_CHECK(
        clEnqueueWriteBuffer(commandQueue.Get(), deviceImg[0].Get(), CL_TRUE, 0, srcBitmapSpan.size(), srcBitmapSpan.data(), 0, nullptr, nullptr));

      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(commandQueue.Get(), kernels[0].Get(), Dimension, nullptr, global.data(), Local.data(), 0, nullptr, nullptr));
      RAPIDOPENCL_CHECK(
        clEnqueueReadBuffer(commandQueue.Get(), deviceImg[1].Get(), CL_TRUE, 0, grayBuffer.size(), grayBuffer.data(), 0, nullptr, nullptr));
      auto endTime = timer.GetNativeTicks();
      TimeSpan timeSpan = HighResolutionTimerUtil::ToTimeSpan(endTime - startTime, timer.GetNativeTickFrequency());
      FSLLOG3_INFO("Process time: {} ms", TimeSpanUtil::ToMillisecondsUInt64(timeSpan));

      FSLLOG3_INFO("Creating kernels...");
      FSLLOG3_INFO("Please wait for compiling and building kernels, about one minute...");

      clSetKernelArg(kernels[1].Get(), 0, sizeof(cl_mem), deviceImg[1].GetPointer());
      clSetKernelArg(kernels[1].Get(), 1, sizeof(cl_mem), deviceImg[0].GetPointer());
      clSetKernelArg(kernels[1].Get(), 2, sizeof(cl_int), &imgWidth);
      clSetKernelArg(kernels[1].Get(), 3, sizeof(cl_int), &imgHeight);
      startTime = 0;
      endTime = 0;
      FSLLOG3_INFO("Running GRAY to RGB kernel...");
      startTime = timer.GetNativeTicks();

      RAPIDOPENCL_CHECK(
        clEnqueueWriteBuffer(commandQueue.Get(), deviceImg[1].Get(), CL_TRUE, 0, grayBuffer.size(), grayBuffer.data(), 0, nullptr, nullptr));

      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(commandQueue.Get(), kernels[1].Get(), Dimension, nullptr, global.data(), Local.data(), 0, nullptr, nullptr));
      RAPIDOPENCL_CHECK(
        clEnqueueReadBuffer(commandQueue.Get(), deviceImg[0].Get(), CL_TRUE, 0, rgbBuffer.size(), rgbBuffer.data(), 0, nullptr, nullptr));
      clFinish(commandQueue.Get());
      endTime = timer.GetNativeTicks();
      timeSpan = HighResolutionTimerUtil::ToTimeSpan(endTime - startTime, timer.GetNativeTickFrequency());
      FSLLOG3_INFO("Process time: {} ms", TimeSpanUtil::ToMillisecondsUInt64(timeSpan));
    }

    Bitmap imageData2(std::move(rgbBuffer), m_srcBitmap.GetSize(), m_srcBitmap.GetPixelFormat(), m_srcBitmap.GetOrigin());

    FSLLOG3_INFO("Saving images...");
    GetPersistentDataManager()->Write(m_outputPath, imageData2, ImageFormat::Bmp, PixelFormat::B8G8R8_UNORM);
  }
}
