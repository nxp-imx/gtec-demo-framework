/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include "SoftISP.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslUtil/OpenCL1_2/OpenCLHelper.hpp>
#include <FslUtil/OpenCL1_2/ProgramEx.hpp>
#include <RapidOpenCL1/Check.hpp>
#include <RapidOpenCL1/Values.hpp>
#include <CL/cl.h>
#include <array>
#include <cstring>
#include "OptionParser.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr std::size_t Bins = 256;
      constexpr std::size_t ImgWidth = 1920;
      constexpr std::size_t ImgHeight = 1080;
      constexpr std::size_t ImgSize = ImgWidth * ImgHeight;
    }

    double GetExecutionTime(const cl_event event)
    {
      cl_ulong start = 0;
      cl_ulong end = 0;
      cl_int err = 0;
      err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, nullptr);
      err |= clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, nullptr);
      if (err != CL_SUCCESS)
      {
        return 0;
      }
      return static_cast<double>(1.0e-6 * static_cast<double>(end - start));    // convert nanoseconds to ms
    }


    void ConvertToRGBA(const RapidOpenCL1::Kernel& kernel, const RapidOpenCL1::Buffer& inBuffer, RapidOpenCL1::Buffer& outBuffer,
                       const RapidOpenCL1::CommandQueue& commandQueue, Span<uint8_t> dstSpan)
    {
      const std::array<std::size_t, 2> globalWorkSize = {LocalConfig::ImgWidth / 4, LocalConfig::ImgHeight / 2};
      const std::array<std::size_t, 2> localWorkSize = {8, 4};
      cl_event event = nullptr;
      clSetKernelArg(kernel.Get(), 0, sizeof(cl_mem), inBuffer.GetPointer());
      clSetKernelArg(kernel.Get(), 1, sizeof(cl_mem), outBuffer.GetPointer());
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(commandQueue.Get(), kernel.Get(), 2, nullptr, globalWorkSize.data(), localWorkSize.data(), 0, nullptr, &event));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &event));

      // Validate our assumptions about the buffer size
      static_assert(sizeof(cl_char) == 1);    // The rest of the code assumes this is true, so lets get a compile time error if its not
      assert(dstSpan.size() == sizeof(cl_char) * LocalConfig::ImgSize * 4);

      RAPIDOPENCL_CHECK(clEnqueueReadBuffer(commandQueue.Get(), outBuffer.Get(), CL_FALSE, 0, dstSpan.size(), dstSpan.data(), 0, nullptr, nullptr));
    }
  }


  SoftISP::SoftISP(const DemoAppConfig& config)
    : DemoAppOpenCL(config)
    , m_pixelValueR(0)
    , m_pixelValueG(0)
    , m_pixelValueB(0)
    , m_save(true)
  {
    auto optionParser = config.GetOptions<OptionParser>();
    m_denoiseEn = optionParser->GetDenoiseStatus();
    m_cycleNum = NumericCast<int32_t>(optionParser->GetCycleNumStatus());

    FSLLOG3_INFO("Denoise status: {}", m_denoiseEn);
    FSLLOG3_INFO("CycleNum status: {}", m_cycleNum);
  }


  SoftISP::~SoftISP() = default;


  void SoftISP::Run()
  {
    FSLLOG3_INFO("Initializing device(s)...");

    // create the OpenCL context on available GPU devices
    OpenCL::ContextEx context(CL_DEVICE_TYPE_GPU);

    FSLLOG3_INFO("Creating Command Queue...");
    RapidOpenCL1::CommandQueue commandQueue;
    commandQueue.Reset(context.Get(), context.GetDeviceId(), CL_QUEUE_PROFILING_ENABLE);

    AllocateMemory(context.Get(), LocalConfig::ImgSize);

    const std::string strProgram = GetContentManager()->ReadAllText("isp_kernel.cl");
    OpenCL::ProgramEx program(context.Get(), context.GetDeviceId(), strProgram);

    FSLLOG3_INFO("Creating kernels...");
    FSLLOG3_INFO("Please wait for compiling and building kernels, about one minute...");
    std::array<RapidOpenCL1::Kernel, 10> kernels{};
    kernels[0].Reset(program.Get(), "badpixel");
    kernels[1].Reset(program.Get(), "sigma");
    kernels[2].Reset(program.Get(), "awb");
    kernels[3].Reset(program.Get(), "equalize1");
    kernels[4].Reset(program.Get(), "equalize2");
    kernels[5].Reset(program.Get(), "equalize3");
    kernels[6].Reset(program.Get(), "debayer");
    if (m_denoiseEn)
    {
      kernels[7].Reset(program.Get(), "rgba2yuyv");
      kernels[8].Reset(program.Get(), "bilateral");
      kernels[9].Reset(program.Get(), "yuyv2rgba");
    }

    clSetKernelArg(kernels[0].Get(), 0, sizeof(cl_mem), m_deviceImg[0].GetPointer());
    clSetKernelArg(kernels[0].Get(), 1, sizeof(cl_mem), m_deviceImg[1].GetPointer());

    clSetKernelArg(kernels[1].Get(), 0, sizeof(cl_mem), m_deviceImg[1].GetPointer());
    clSetKernelArg(kernels[1].Get(), 1, sizeof(cl_mem), m_pixelValue[0].GetPointer());
    clSetKernelArg(kernels[1].Get(), 2, sizeof(cl_mem), m_pixelValue[1].GetPointer());
    clSetKernelArg(kernels[1].Get(), 3, sizeof(cl_mem), m_pixelValue[2].GetPointer());

    clSetKernelArg(kernels[2].Get(), 0, sizeof(cl_mem), m_deviceImg[1].GetPointer());
    clSetKernelArg(kernels[2].Get(), 1, sizeof(cl_mem), m_pixelValue[0].GetPointer());
    clSetKernelArg(kernels[2].Get(), 2, sizeof(cl_mem), m_pixelValue[1].GetPointer());
    clSetKernelArg(kernels[2].Get(), 3, sizeof(cl_mem), m_pixelValue[2].GetPointer());
    clSetKernelArg(kernels[2].Get(), 4, sizeof(cl_mem), m_deviceImg[2].GetPointer());

    clSetKernelArg(kernels[3].Get(), 0, sizeof(cl_mem), m_deviceImg[1].GetPointer());
    clSetKernelArg(kernels[3].Get(), 1, sizeof(cl_mem), m_deviceDist[0].GetPointer());
    clSetKernelArg(kernels[3].Get(), 2, sizeof(cl_mem), m_deviceDist[1].GetPointer());
    clSetKernelArg(kernels[3].Get(), 3, sizeof(cl_mem), m_deviceDist[2].GetPointer());

    clSetKernelArg(kernels[4].Get(), 0, sizeof(cl_mem), m_deviceDist[0].GetPointer());
    clSetKernelArg(kernels[4].Get(), 1, sizeof(cl_mem), m_deviceDist[1].GetPointer());
    clSetKernelArg(kernels[4].Get(), 2, sizeof(cl_mem), m_deviceDist[2].GetPointer());
    clSetKernelArg(kernels[4].Get(), 3, sizeof(cl_mem), m_deviceDist[3].GetPointer());
    clSetKernelArg(kernels[4].Get(), 4, sizeof(cl_mem), m_deviceDist[4].GetPointer());
    clSetKernelArg(kernels[4].Get(), 5, sizeof(cl_mem), m_deviceDist[5].GetPointer());

    clSetKernelArg(kernels[5].Get(), 0, sizeof(cl_mem), m_deviceImg[1].GetPointer());
    clSetKernelArg(kernels[5].Get(), 1, sizeof(cl_mem), m_deviceDist[3].GetPointer());
    clSetKernelArg(kernels[5].Get(), 2, sizeof(cl_mem), m_deviceDist[4].GetPointer());
    clSetKernelArg(kernels[5].Get(), 3, sizeof(cl_mem), m_deviceDist[5].GetPointer());
    clSetKernelArg(kernels[5].Get(), 4, sizeof(cl_mem), m_deviceImg[3].GetPointer());

    clSetKernelArg(kernels[6].Get(), 0, sizeof(cl_mem), m_deviceImg[3].GetPointer());
    clSetKernelArg(kernels[6].Get(), 1, sizeof(cl_mem), m_deviceImg[4].GetPointer());

    if (m_denoiseEn)
    {
      clSetKernelArg(kernels[7].Get(), 0, sizeof(cl_mem), m_deviceImg[4].GetPointer());
      clSetKernelArg(kernels[7].Get(), 1, sizeof(cl_mem), m_deviceImg[5].GetPointer());
      clSetKernelArg(kernels[7].Get(), 2, sizeof(cl_mem), m_deviceImg[6].GetPointer());
      clSetKernelArg(kernels[8].Get(), 0, sizeof(cl_mem), m_deviceImg[5].GetPointer());
      clSetKernelArg(kernels[8].Get(), 1, sizeof(cl_mem), m_deviceImg[7].GetPointer());
      clSetKernelArg(kernels[9].Get(), 0, sizeof(cl_mem), m_deviceImg[7].GetPointer());
      clSetKernelArg(kernels[9].Get(), 1, sizeof(cl_mem), m_deviceImg[6].GetPointer());
      clSetKernelArg(kernels[9].Get(), 2, sizeof(cl_mem), m_deviceImg[8].GetPointer());
    }
    const std::array<std::size_t, 2> globalWorkSizeDiv8 = {LocalConfig::ImgWidth / 4, LocalConfig::ImgHeight / 2};
    const std::array<std::size_t, 2> localWorkSize32 = {8, 4};
    const std::array<std::size_t, 2> globalWorkSizeDiv16 = {LocalConfig::ImgWidth / 8, LocalConfig::ImgHeight / 2};
    const std::array<std::size_t, 2> localWorkSize16 = {8, 2};

    const std::array<std::size_t, 2> globalWorkSizeStd = {LocalConfig::ImgWidth, LocalConfig::ImgHeight};
    const std::array<std::size_t, 2> globalWorkSizeDenoise = {LocalConfig::ImgWidth / 8, LocalConfig::ImgHeight};
    const std::array<std::size_t, 2> globalWorkSizeOne = {32, 2};
    GetContentManager()->ReadAllBytes(m_dst0.data(), LocalConfig::ImgSize, "bayer.data");

    RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), m_deviceImg[0].Get(), CL_TRUE, 0, sizeof(cl_char) * LocalConfig::ImgSize,
                                           m_dst0.data(), 0, nullptr, nullptr));

    cl_event hEvent = nullptr;
    RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[0].Get(), 2, nullptr, globalWorkSizeDiv8.data(), localWorkSize32.data(), 0,
                                             nullptr, &hEvent));
    RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
    double time = GetExecutionTime(hEvent);
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: badpixel): {} ms", time);

    m_pixelValueR = 0;
    m_pixelValueG = 0;
    m_pixelValueB = 0;

    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(commandQueue.Get(), m_pixelValue[0].Get(), CL_FALSE, 0, sizeof(cl_int), &m_pixelValueR, 0, nullptr, nullptr));
    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(commandQueue.Get(), m_pixelValue[1].Get(), CL_FALSE, 0, sizeof(cl_int), &m_pixelValueG, 0, nullptr, nullptr));
    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(commandQueue.Get(), m_pixelValue[2].Get(), CL_FALSE, 0, sizeof(cl_int), &m_pixelValueB, 0, nullptr, nullptr));

    double timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[1].Get(), 2, nullptr, globalWorkSizeDiv16.data(), localWorkSize32.data(),
                                               0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: sigma): {} ms", timeTotal / m_cycleNum);

    timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[2].Get(), 2, nullptr, globalWorkSizeDiv16.data(), localWorkSize16.data(),
                                               0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: awb): {} ms", timeTotal / m_cycleNum);

    if ((!m_inDistR.empty()) && (!m_inDistG.empty()) && (!m_inDistB.empty()))
    {
      m_inDistR.clear();
      m_inDistG.clear();
      m_inDistB.clear();
    }
    RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), m_deviceDist[0].Get(), CL_FALSE, 0, sizeof(cl_int) * LocalConfig::Bins,
                                           m_inDistR.data(), 0, nullptr, nullptr));
    RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), m_deviceDist[1].Get(), CL_FALSE, 0, sizeof(cl_int) * LocalConfig::Bins,
                                           m_inDistG.data(), 0, nullptr, nullptr));
    RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), m_deviceDist[2].Get(), CL_FALSE, 0, sizeof(cl_int) * LocalConfig::Bins,
                                           m_inDistB.data(), 0, nullptr, nullptr));

    timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[3].Get(), 2, nullptr, globalWorkSizeDiv16.data(), localWorkSize16.data(),
                                               0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: equalize1): {} ms", timeTotal / m_cycleNum);

    timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[4].Get(), 2, nullptr, globalWorkSizeOne.data(), localWorkSize16.data(), 0,
                                               nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: equalize2): {} ms", timeTotal / m_cycleNum);

    timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[5].Get(), 2, nullptr, globalWorkSizeDiv16.data(), localWorkSize16.data(),
                                               0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: equalize3): {} ms", timeTotal / m_cycleNum);

    RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[6].Get(), 2, nullptr, globalWorkSizeDiv8.data(), localWorkSize32.data(), 0,
                                             nullptr, &hEvent));
    RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
    time = GetExecutionTime(hEvent);
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: debayer): {} ms", time);

    if (m_denoiseEn)
    {
      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[7].Get(), 2, nullptr, globalWorkSizeStd.data(), localWorkSize32.data(), 0,
                                               nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      FSLLOG3_INFO("Kernel execution time on GPU (kernel: rgba2yuyv): {} ms", time);

      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[8].Get(), 2, nullptr, globalWorkSizeDenoise.data(), localWorkSize32.data(),
                                               0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      FSLLOG3_INFO("Kernel execution time on GPU (kernel: bilateral): {} ms", time);

      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernels[9].Get(), 2, nullptr, globalWorkSizeStd.data(), localWorkSize32.data(), 0,
                                               nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      FSLLOG3_INFO("Kernel execution time on GPU (kernel: yuyv2rgba): {} ms", time);
    }

    if (m_save)
    {
      Bitmap bitmap;
      FSLLOG3_INFO("Saving images...");
      ConvertToRGBA(kernels[6], m_deviceImg[0], m_deviceImg[4], commandQueue, SpanUtil::AsSpan(m_dst4));
      const char* fileName = "0-SourceImage.bmp";
      CopyToBMP(bitmap, fileName, SpanUtil::AsSpan(m_dst4));

      ConvertToRGBA(kernels[6], m_deviceImg[1], m_deviceImg[4], commandQueue, SpanUtil::AsSpan(m_dst4));
      fileName = "1-RemoveBadPixel.bmp";
      CopyToBMP(bitmap, fileName, SpanUtil::AsSpan(m_dst4));

      ConvertToRGBA(kernels[6], m_deviceImg[2], m_deviceImg[4], commandQueue, SpanUtil::AsSpan(m_dst4));
      fileName = "2-WhiteBalance.bmp";
      CopyToBMP(bitmap, fileName, SpanUtil::AsSpan(m_dst4));

      ConvertToRGBA(kernels[6], m_deviceImg[3], m_deviceImg[4], commandQueue, SpanUtil::AsSpan(m_dst4));
      fileName = "3-Equalization.bmp";
      CopyToBMP(bitmap, fileName, SpanUtil::AsSpan(m_dst4));
      if (m_denoiseEn)
      {
        RAPIDOPENCL_CHECK(clEnqueueReadBuffer(commandQueue.Get(), m_deviceImg[8].Get(), CL_FALSE, 0, sizeof(cl_char) * LocalConfig::ImgSize * 4,
                                              m_dst5.data(), 0, nullptr, nullptr));
        fileName = "4-RemoveNoise.bmp";
        CopyToBMP(bitmap, fileName, SpanUtil::AsSpan(m_dst5));
      }
    }
  }


  void SoftISP::AllocateMemory(const cl_context context, const std::size_t size)
  {
    m_dst0.resize(size);
    m_dst1.resize(size);
    m_dst2.resize(size);
    m_dst3.resize(size);
    m_dst4.resize(size * 4);
    m_dst5.resize(size * 4);
    m_yBuf.resize(size);
    m_uvBuf.resize(size * 2);
    m_yBufOut.resize(size);
    m_inDistR.resize(LocalConfig::Bins);
    m_inDistG.resize(LocalConfig::Bins);
    m_inDistB.resize(LocalConfig::Bins);
    m_outDistR.resize(LocalConfig::Bins);
    m_outDistG.resize(LocalConfig::Bins);
    m_outDistB.resize(LocalConfig::Bins);
    m_pixelValue.resize(3);
    m_deviceDist.resize(6);

    m_deviceImg.resize(9);
    const auto defaultFlags = CL_MEM_COPY_HOST_PTR;
    m_deviceImg[0].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_dst0.data());
    m_deviceImg[1].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_dst1.data());
    m_deviceImg[2].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_dst2.data());
    m_deviceImg[3].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_dst3.data());
    m_deviceImg[4].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size * 4, m_dst4.data());
    if (m_denoiseEn)
    {
      m_deviceImg[5].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_yBuf.data());
      m_deviceImg[6].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size * 2, m_uvBuf.data());
      m_deviceImg[7].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_yBufOut.data());
      m_deviceImg[8].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size * 4, m_dst5.data());
    }

    m_pixelValue[0].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int), &m_pixelValueR);
    m_pixelValue[1].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int), &m_pixelValueG);
    m_pixelValue[2].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int), &m_pixelValueB);

    m_deviceDist[0].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * LocalConfig::Bins, m_inDistR.data());
    m_deviceDist[1].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * LocalConfig::Bins, m_inDistG.data());
    m_deviceDist[2].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * LocalConfig::Bins, m_inDistB.data());
    m_deviceDist[3].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * LocalConfig::Bins, m_outDistR.data());
    m_deviceDist[4].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * LocalConfig::Bins, m_outDistG.data());
    m_deviceDist[5].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * LocalConfig::Bins, m_outDistB.data());
  }


  void SoftISP::CopyToBMP(Bitmap& rBitmap, const IO::Path& fileName, const Span<uint8_t> span)
  {
    rBitmap.Reset(span, PxSize2D::Create(UncheckedNumericCast<int32_t>(LocalConfig::ImgWidth), UncheckedNumericCast<int32_t>(LocalConfig::ImgHeight)),
                  PixelFormat::B8G8R8A8_UNORM, BitmapOrigin::UpperLeft);
    GetPersistentDataManager()->Write(fileName, rBitmap);
  }
}
