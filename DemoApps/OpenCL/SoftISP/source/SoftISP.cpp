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
#include "OptionParser.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslUtil/OpenCL1_2/ProgramEx.hpp>
#include <FslUtil/OpenCL1_2/OpenCLHelper.hpp>
#include <RapidOpenCL1/Check.hpp>
#include <RapidOpenCL1/Values.hpp>
#include <CL/cl.h>
#include <string.h>
#include <fstream>

using namespace RapidOpenCL1;

namespace Fsl
{
  using namespace OpenCL;

  namespace
  {
    cl_uint GetDeviceCount(const cl_context context)
    {
      std::size_t nDeviceBytes;
      RAPIDOPENCL_CHECK(clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, nullptr, &nDeviceBytes));
      return static_cast<cl_uint>(nDeviceBytes / sizeof(cl_device_id));
    }


    cl_uint GetNumComputeUnits(const cl_platform_id platform, const cl_device_type deviceType)
    {
      // Get all the devices
      FSLLOG3_INFO("Get the Device info and select Device...");
      const auto devices = OpenCLHelper::GetDeviceIDs(platform, deviceType);

      // Set target device and Query number of compute units on targetDevice
      FSLLOG3_INFO("# of Devices Available = {}", devices.size());

      cl_uint numComputeUnits;
      RAPIDOPENCL_CHECK(clGetDeviceInfo(devices[0], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(numComputeUnits), &numComputeUnits, nullptr));

      FSLLOG3_INFO("# of Compute Units = {}", numComputeUnits);
      return numComputeUnits;
    }


    double GetExecutionTime(const cl_event event)
    {
      cl_ulong start;
      cl_ulong end;
      cl_int err;
      err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, nullptr);
      err |= clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, nullptr);
      if (err)
      {
        return 0;
      }
      return static_cast<double>(1.0e-6 * (end - start));    // convert nanoseconds to ms
    }


    void ConvertToRGBA(const Kernel& kernel, const Buffer& inBuffer, Buffer& outBuffer, const CommandQueue& commandQueue, void* ptr)
    {
      const std::size_t imgWid = 1920;
      const std::size_t imgHei = 1080;
      const std::size_t imgSize = imgWid * imgHei;
      const std::size_t globalWorkSize[2] = {imgWid / 4, imgHei / 2};
      const std::size_t localWorkSize[2] = {8, 4};
      cl_event event;
      clSetKernelArg(kernel.Get(), 0, sizeof(cl_mem), inBuffer.GetPointer());
      clSetKernelArg(kernel.Get(), 1, sizeof(cl_mem), outBuffer.GetPointer());
      RAPIDOPENCL_CHECK(clEnqueueNDRangeKernel(commandQueue.Get(), kernel.Get(), 2, nullptr, globalWorkSize, localWorkSize, 0, nullptr, &event));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &event));
      RAPIDOPENCL_CHECK(
        clEnqueueReadBuffer(commandQueue.Get(), outBuffer.Get(), CL_FALSE, 0, sizeof(cl_char) * imgSize * 4, ptr, 0, nullptr, nullptr));
    }
  }


  SoftISP::SoftISP(const DemoAppConfig& config)
    : DemoAppOpenCL(config)
    , m_context()
    , m_deviceId(0)
    , m_commandQueue()
    , m_pixelValueR(0)
    , m_pixelValueG(0)
    , m_pixelValueB(0)
    , m_save(true)
  {
    auto optionParser = config.GetOptions<OptionParser>();
    m_denoiseEn = optionParser->GetDenoiseStatus();
    m_cycleNum = optionParser->GetCycleNumStatus();
    FSLLOG3_INFO("Denoise status: {}", m_denoiseEn);
    FSLLOG3_INFO("CycleNum status: {}", m_cycleNum);
    FSLLOG3_INFO("Initializing device(s)...");
    const cl_device_type deviceType = CL_DEVICE_TYPE_GPU;
    // create the OpenCL context on available GPU devices
    m_context.Reset(deviceType);

    if (GetDeviceCount(m_context.Get()) <= 0)
      throw InitFailedException("No OpenCL specific devices!");
    const cl_uint ciComputeUnitsCount = GetNumComputeUnits(m_context.GetPlatformId(), deviceType);
    FSLLOG3_INFO("# compute units = {}", ciComputeUnitsCount);

    FSLLOG3_INFO("Getting device id...");
    RAPIDOPENCL_CHECK(clGetContextInfo(m_context.Get(), CL_CONTEXT_DEVICES, sizeof(cl_device_id), &m_deviceId, nullptr));

    FSLLOG3_INFO("Creating Command Queue...");
    m_commandQueue.Reset(m_context.Get(), m_deviceId, CL_QUEUE_PROFILING_ENABLE);
  }


  SoftISP::~SoftISP()
  {
  }


  void SoftISP::Run()
  {
    AllocateMemory(m_context.Get(), m_imgSize);

    const std::string strProgram = GetContentManager()->ReadAllText("isp_kernel.cl");
    ProgramEx program(m_context.Get(), m_deviceId, strProgram);

    const int KERNEL_NUM = 10;
    FSLLOG3_INFO("Creating kernels...");
    FSLLOG3_INFO("Please wait for compiling and building kernels, about one minute...");
    Kernel kernels[KERNEL_NUM];
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
    const std::size_t globalWorkSizeDiv8[2] = {m_imgWid / 4, m_imgHei / 2};
    const std::size_t localWorkSize32[2] = {8, 4};
    const std::size_t globalWorkSizeDiv16[2] = {m_imgWid / 8, m_imgHei / 2};
    const std::size_t localWorkSize16[2] = {8, 2};

    const std::size_t globalWorkSizeStd[2] = {m_imgWid, m_imgHei};
    const std::size_t globalWorkSizeDenoise[2] = {m_imgWid / 8, m_imgHei};
    const std::size_t globalWorkSizeOne[2] = {32, 2};
    GetContentManager()->ReadAllBytes(m_dst0.data(), m_imgSize, "bayer.data");

    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(m_commandQueue.Get(), m_deviceImg[0].Get(), CL_FALSE, 0, sizeof(cl_char) * m_imgSize, m_dst0.data(), 0, nullptr, nullptr));

    cl_event hEvent;
    RAPIDOPENCL_CHECK(
      clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[0].Get(), 2, nullptr, globalWorkSizeDiv8, localWorkSize32, 0, nullptr, &hEvent));
    RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
    double time = GetExecutionTime(hEvent);
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: badpixel): {} ms", time);

    m_pixelValueR = 0, m_pixelValueG = 0, m_pixelValueB = 0;
    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(m_commandQueue.Get(), m_pixelValue[0].Get(), CL_FALSE, 0, sizeof(cl_int), &m_pixelValueR, 0, nullptr, nullptr));
    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(m_commandQueue.Get(), m_pixelValue[1].Get(), CL_FALSE, 0, sizeof(cl_int), &m_pixelValueG, 0, nullptr, nullptr));
    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(m_commandQueue.Get(), m_pixelValue[2].Get(), CL_FALSE, 0, sizeof(cl_int), &m_pixelValueB, 0, nullptr, nullptr));
    double timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[1].Get(), 2, nullptr, globalWorkSizeDiv16, localWorkSize32, 0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: sigma): {} ms", timeTotal / m_cycleNum);

    timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[2].Get(), 2, nullptr, globalWorkSizeDiv16, localWorkSize16, 0, nullptr, &hEvent));
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
    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(m_commandQueue.Get(), m_deviceDist[0].Get(), CL_FALSE, 0, sizeof(cl_int) * m_BINS, m_inDistR.data(), 0, nullptr, nullptr));
    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(m_commandQueue.Get(), m_deviceDist[1].Get(), CL_FALSE, 0, sizeof(cl_int) * m_BINS, m_inDistG.data(), 0, nullptr, nullptr));
    RAPIDOPENCL_CHECK(
      clEnqueueWriteBuffer(m_commandQueue.Get(), m_deviceDist[2].Get(), CL_FALSE, 0, sizeof(cl_int) * m_BINS, m_inDistB.data(), 0, nullptr, nullptr));

    timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[3].Get(), 2, nullptr, globalWorkSizeDiv16, localWorkSize16, 0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: equalize1): {} ms", timeTotal / m_cycleNum);

    timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[4].Get(), 2, nullptr, globalWorkSizeOne, localWorkSize16, 0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: equalize2): {} ms", timeTotal / m_cycleNum);

    timeTotal = 0;
    for (int i = 0; i < m_cycleNum; i++)
    {
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[5].Get(), 2, nullptr, globalWorkSizeDiv16, localWorkSize16, 0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      timeTotal += time;
    }
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: equalize3): {} ms", timeTotal / m_cycleNum);

    RAPIDOPENCL_CHECK(
      clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[6].Get(), 2, nullptr, globalWorkSizeDiv8, localWorkSize32, 0, nullptr, &hEvent));
    RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
    time = GetExecutionTime(hEvent);
    FSLLOG3_INFO("Kernel execution time on GPU (kernel: debayer): {} ms", time);

    if (m_denoiseEn)
    {
      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[7].Get(), 2, nullptr, globalWorkSizeStd, localWorkSize32, 0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      FSLLOG3_INFO("Kernel execution time on GPU (kernel: rgba2yuyv): {} ms", time);

      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[8].Get(), 2, nullptr, globalWorkSizeDenoise, localWorkSize32, 0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      FSLLOG3_INFO("Kernel execution time on GPU (kernel: bilateral): {} ms", time);

      RAPIDOPENCL_CHECK(
        clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[9].Get(), 2, nullptr, globalWorkSizeStd, localWorkSize32, 0, nullptr, &hEvent));
      RAPIDOPENCL_CHECK(clWaitForEvents(1, &hEvent));
      time = GetExecutionTime(hEvent);
      FSLLOG3_INFO("Kernel execution time on GPU (kernel: yuyv2rgba): {} ms", time);
    }

    if (m_save)
    {
      Bitmap bitmap;
      FSLLOG3_INFO("Saving images...");
      ConvertToRGBA(kernels[6], m_deviceImg[0], m_deviceImg[4], m_commandQueue, m_dst4.data());
      const char* fileName = "0-SourceImage.bmp";
      CopyToBMP(bitmap, fileName, m_dst4.data());

      ConvertToRGBA(kernels[6], m_deviceImg[1], m_deviceImg[4], m_commandQueue, m_dst4.data());
      fileName = "1-RemoveBadPixel.bmp";
      CopyToBMP(bitmap, fileName, m_dst4.data());

      ConvertToRGBA(kernels[6], m_deviceImg[2], m_deviceImg[4], m_commandQueue, m_dst4.data());
      fileName = "2-WhiteBalance.bmp";
      CopyToBMP(bitmap, fileName, m_dst4.data());

      ConvertToRGBA(kernels[6], m_deviceImg[3], m_deviceImg[4], m_commandQueue, m_dst4.data());
      fileName = "3-Equalization.bmp";
      CopyToBMP(bitmap, fileName, m_dst4.data());
      if (m_denoiseEn)
      {
        RAPIDOPENCL_CHECK(clEnqueueReadBuffer(m_commandQueue.Get(), m_deviceImg[8].Get(), CL_FALSE, 0, sizeof(cl_char) * m_imgSize * 4, m_dst5.data(),
                                              0, nullptr, nullptr));
        fileName = "4-RemoveNoise.bmp";
        CopyToBMP(bitmap, fileName, m_dst5.data());
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
    m_YBuf.resize(size);
    m_UVBuf.resize(size * 2);
    m_YBufOut.resize(size);
    m_inDistR.resize(m_BINS);
    m_inDistG.resize(m_BINS);
    m_inDistB.resize(m_BINS);
    m_outDistR.resize(m_BINS);
    m_outDistG.resize(m_BINS);
    m_outDistB.resize(m_BINS);
    m_pixelValue.resize(3);
    m_deviceDist.resize(6);

    m_deviceImg.resize(9);
    const auto defaultFlags = CL_MEM_USE_HOST_PTR;
    m_deviceImg[0].Reset(context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_dst0.data());
    m_deviceImg[1].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_dst1.data());
    m_deviceImg[2].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_dst2.data());
    m_deviceImg[3].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_dst3.data());
    m_deviceImg[4].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size * 4, m_dst4.data());
    if (m_denoiseEn)
    {
      m_deviceImg[5].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_YBuf.data());
      m_deviceImg[6].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size * 2, m_UVBuf.data());
      m_deviceImg[7].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size, m_YBufOut.data());
      m_deviceImg[8].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_char) * size * 4, m_dst5.data());
    }

    m_pixelValue[0].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int), &m_pixelValueR);
    m_pixelValue[1].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int), &m_pixelValueG);
    m_pixelValue[2].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int), &m_pixelValueB);

    m_deviceDist[0].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * m_BINS, m_inDistR.data());
    m_deviceDist[1].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * m_BINS, m_inDistG.data());
    m_deviceDist[2].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * m_BINS, m_inDistB.data());
    m_deviceDist[3].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * m_BINS, m_outDistR.data());
    m_deviceDist[4].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * m_BINS, m_outDistG.data());
    m_deviceDist[5].Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(cl_int) * m_BINS, m_outDistB.data());
  }


  void SoftISP::CopyToBMP(Bitmap& bitmap, const IO::Path& fileName, const void* ptr)
  {
    bitmap.Reset(ptr, m_imgSize * 4, PxExtent2D(static_cast<int32_t>(m_imgWid), static_cast<int32_t>(m_imgHei)), PixelFormat::B8G8R8A8_UNORM,
                 BitmapOrigin::UpperLeft);
    GetPersistentDataManager()->Write(fileName, bitmap);
  }
}
