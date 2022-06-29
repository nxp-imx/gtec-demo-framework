/****************************************************************************
 *
 *    Copyright 2012 - 2016 Vivante Corporation, Santa Clara, California.
 *    All Rights Reserved.
 *
 *    Permission is hereby granted, free of charge, to any person obtaining
 *    a copy of this software and associated documentation files (the
 *    'Software'), to deal in the Software without restriction, including
 *    without limitation the rights to use, copy, modify, merge, publish,
 *    distribute, sub license, and/or sell copies of the Software, and to
 *    permit persons to whom the Software is furnished to do so, subject
 *    to the following conditions:
 *
 *    The above copyright notice and this permission notice (including the
 *    next paragraph) shall be included in all copies or substantial
 *    portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 *    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
 *    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/

// Based on a sample by Vivante
// Converted to a DemoFramework OpenCl sample.

#include "FastFourierTransform.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslUtil/OpenCL1_2/OpenCLHelper.hpp>
#include <FslUtil/OpenCL1_2/ProgramEx.hpp>
#include <RapidOpenCL1/Check.hpp>
#include <RapidOpenCL1/Values.hpp>
#include <CL/cl.h>
#include <array>
#include <cassert>
#include <iomanip>
#include <sstream>
#include "OptionParser.hpp"

using namespace RapidOpenCL1;

namespace Fsl
{
  using namespace OpenCL;

  namespace
  {
    const auto RADIX2_FFT_KERNEL = "fft_radix2";
    const auto RADIX4_FFT_KERNEL = "fft_radix4";


    const std::array<int, FFT_MAX_LOG2N> g_p = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};


    const std::array<int, FFT_MAX_LOG2N> g_twop = {2 * 1,   2 * 2,   2 * 4,    2 * 8,    2 * 16,   2 * 32,   2 * 64,    2 * 128,
                                                   2 * 256, 2 * 512, 2 * 1024, 2 * 2048, 2 * 4096, 2 * 8192, 2 * 16384, 2 * 32768};


    const std::array<int, FFT_MAX_LOG2N> g_threep = {3 * 1,   3 * 2,   3 * 4,    3 * 8,    3 * 16,   3 * 32,   3 * 64,    3 * 128,
                                                     3 * 256, 3 * 512, 3 * 1024, 3 * 2048, 3 * 4096, 3 * 8192, 3 * 16384, 3 * 32768};


    const std::array<int, FFT_MAX_LOG2N> g_pminus1 = {1 - 1,   2 - 1,   4 - 1,    8 - 1,    16 - 1,   32 - 1,   64 - 1,    128 - 1,
                                                      256 - 1, 512 - 1, 1024 - 1, 2048 - 1, 4096 - 1, 8192 - 1, 16384 - 1, 32768 - 1};

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const cl_float g_minusPIoverp[FFT_MAX_LOG2N] = {
      -MathHelper::PI,          -MathHelper::PI / 2.f,    -MathHelper::PI / 4.f,     -MathHelper::PI / 8.f,
      -MathHelper::PI / 16.f,   -MathHelper::PI / 32.f,   -MathHelper::PI / 64.f,    -MathHelper::PI / 128.f,
      -MathHelper::PI / 256.f,  -MathHelper::PI / 512.f,  -MathHelper::PI / 1024.f,  -MathHelper::PI / 2048.f,
      -MathHelper::PI / 4096.f, -MathHelper::PI / 8192.f, -MathHelper::PI / 16384.f, -MathHelper::PI / 32768.f};

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const cl_float g_minusPIover2p[FFT_MAX_LOG2N] = {
      -MathHelper::PI / 2.f,    -MathHelper::PI / 4.f,     -MathHelper::PI / 8.f,     -MathHelper::PI / 16.f,
      -MathHelper::PI / 32.f,   -MathHelper::PI / 64.f,    -MathHelper::PI / 128.f,   -MathHelper::PI / 256.f,
      -MathHelper::PI / 512.f,  -MathHelper::PI / 1024.f,  -MathHelper::PI / 2048.f,  -MathHelper::PI / 4096.f,
      -MathHelper::PI / 8192.f, -MathHelper::PI / 16384.f, -MathHelper::PI / 32768.f, -MathHelper::PI / 65536.f};

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const cl_float g_minusPIover2p_2x[FFT_MAX_LOG2N] = {
      -2.f * MathHelper::PI / 2.f,    -2.f * MathHelper::PI / 4.f,     -2.f * MathHelper::PI / 8.f,     -2.f * MathHelper::PI / 16.f,
      -2.f * MathHelper::PI / 32.f,   -2.f * MathHelper::PI / 64.f,    -2.f * MathHelper::PI / 128.f,   -2.f * MathHelper::PI / 256.f,
      -2.f * MathHelper::PI / 512.f,  -2.f * MathHelper::PI / 1024.f,  -2.f * MathHelper::PI / 2048.f,  -2.f * MathHelper::PI / 4096.f,
      -2.f * MathHelper::PI / 8192.f, -2.f * MathHelper::PI / 16384.f, -2.f * MathHelper::PI / 32768.f, -2.f * MathHelper::PI / 65536.f};

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const cl_float g_minusPIover2p_3x[FFT_MAX_LOG2N] = {
      -3.f * MathHelper::PI / 2.f,    -3.f * MathHelper::PI / 4.f,     -3.f * MathHelper::PI / 8.f,     -3.f * MathHelper::PI / 16.f,
      -3.f * MathHelper::PI / 32.f,   -3.f * MathHelper::PI / 64.f,    -3.f * MathHelper::PI / 128.f,   -3.f * MathHelper::PI / 256.f,
      -3.f * MathHelper::PI / 512.f,  -3.f * MathHelper::PI / 1024.f,  -3.f * MathHelper::PI / 2048.f,  -3.f * MathHelper::PI / 4096.f,
      -3.f * MathHelper::PI / 8192.f, -3.f * MathHelper::PI / 16384.f, -3.f * MathHelper::PI / 32768.f, -3.f * MathHelper::PI / 65536.f};


    int Radix(const int N)
    {
      int i = 0;
      int j = 0;
      for (; i <= 31; i++)
      {
        if ((N & (1 << i)) == 0)
        {
          j++;
        }
        else
        {
          break;
        }
      }
      return (0 == (j % 2)) ? 4 : 2;
    }


    uint32_t Log2NFFT(const uint32_t size)
    {
      uint32_t v = size;
      uint32_t log2n = 0;
      while ((v >>= 1) != 0u)
      {
        ++log2n;
      }
      return log2n;
    }


    std::string BuildResultString(const unsigned size, const std::vector<float>& buffer)
    {
      std::stringstream stream;
      stream << std::fixed << std::setprecision(6);    // replicate the %f default setting
      for (uint32_t i = 0; i < size; ++i)
      {
        stream << buffer[2 * i] << "," << buffer[2 * i + 1] << "\r\n";
      }
      return stream.str();
    }


    void CopyToDevice(const CommandQueue& commandQueue, const cl_mem mem, float* const hostPtr, const unsigned size)
    {
      RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), mem, CL_FALSE, 0, sizeof(float) * size, hostPtr, 0, nullptr, nullptr));
    }


    void CopyFromDevice(const CommandQueue& commandQueue, const cl_mem dMem, float* const hostPtr, const unsigned size, UserEvent* pGpuDone)
    {
      if (pGpuDone == nullptr)
      {
        RAPIDOPENCL_CHECK(clEnqueueReadBuffer(commandQueue.Get(), dMem, CL_FALSE, 0, sizeof(float) * size, hostPtr, 0, nullptr, nullptr));
      }
      else
      {
        cl_event hEvent = nullptr;
        RAPIDOPENCL_CHECK(clEnqueueReadBuffer(commandQueue.Get(), dMem, CL_FALSE, 0, sizeof(float) * size, hostPtr, 0, nullptr, &hEvent));
        // Hand off the event to a managed object
        pGpuDone->Reset(hEvent);
      }
    }


    cl_uint GetDeviceCount(const cl_context context)
    {
      std::size_t nDeviceBytes = 0;
      RAPIDOPENCL_CHECK(clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, nullptr, &nDeviceBytes));
      return (static_cast<cl_uint>(nDeviceBytes) / sizeof(cl_device_id));
    }


    cl_uint GetNumComputeUnits(const cl_platform_id platform, const cl_device_type deviceType)
    {
      // Get all the devices
      FSLLOG3_INFO("Get the Device info and select Device...");
      const auto devices = OpenCLHelper::GetDeviceIDs(platform, deviceType);

      // Set target device and Query number of compute units on targetDevice
      FSLLOG3_INFO("# of Devices Available = {}", devices.size());

      cl_uint numComputeUnits = 0;
      RAPIDOPENCL_CHECK(clGetDeviceInfo(devices[0], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(numComputeUnits), &numComputeUnits, nullptr));

      FSLLOG3_INFO("# of Compute Units = {}", numComputeUnits);
      return numComputeUnits;
    }


    double GetExecutionTime(const cl_event event)
    {
      cl_ulong start = 0;
      cl_ulong end = 0;
      cl_int err = 0;
      err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, nullptr);
      err |= clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, nullptr);
      if (err != 0)
      {
        return 0;
      }
      return 1.0e-9 * static_cast<double>(end - start);    // convert nanoseconds to seconds
    }
  }


  FastFourierTransform::FastFourierTransform(const DemoAppConfig& config)
    : DemoAppOpenCL(config)
    , m_length(16)
    , m_save(true)
    , m_blockSize(16)
    , m_deviceId(nullptr)
    , m_gpuDone(Values::INVALID_EVENT)
  {
    auto optionParser = config.GetOptions<OptionParser>();
    const auto length = optionParser->GetLength();
    if (length > FFT_MAX)
    {
      std::stringstream errorMessage;
      errorMessage << "FFT length cannot be greater than " << FFT_MAX;
      throw std::invalid_argument(errorMessage.str());
    }

    if (length < 16)
    {
      throw std::invalid_argument("FFT length has to at least be 16.");
    }

    if ((length != 1) && ((length & (length - 1)) != 0u))
    {
      std::stringstream errorMessage;
      errorMessage << "FFT length (" << length << ") must be a power-of-2.";
      throw std::invalid_argument(errorMessage.str());
    }
    m_length = length;

    FSLLOG3_INFO("Block size: {}", m_blockSize);
    FSLLOG3_INFO("Length: {}", m_length);
    FSLLOG3_INFO("Print result: {}", (m_save ? "yes" : "no"));


    {    // initExecution

      // Allocate host memory (and initialize input signal)
      AllocateHostMemory(m_length);
      if (m_save)
      {
        auto result = BuildResultString(m_length, m_intime);
        GetPersistentDataManager()->WriteAlltext("fft_input.csv", result);
      }

      FSLLOG3_INFO("Initializing device(s)...");
      const cl_device_type deviceType = CL_DEVICE_TYPE_GPU;

      // create the OpenCL context on available GPU devices
      m_context.Reset(deviceType);

      if (GetDeviceCount(m_context.Get()) <= 0)
      {
        throw InitFailedException("No OpenCL specific devices!");
      }

      const cl_uint ciComputeUnitsCount = GetNumComputeUnits(m_context.GetPlatformId(), deviceType);
      FSLLOG3_INFO("# compute units = {}", ciComputeUnitsCount);

      FSLLOG3_INFO("Getting device id...");
      RAPIDOPENCL_CHECK(clGetContextInfo(m_context.Get(), CL_CONTEXT_DEVICES, sizeof(cl_device_id), &m_deviceId, nullptr));

      FSLLOG3_INFO("Creating Command Queue...");
      m_commandQueue.Reset(m_context.Get(), m_deviceId, CL_QUEUE_PROFILING_ENABLE);
    }
  }


  FastFourierTransform::~FastFourierTransform() = default;


  void FastFourierTransform::Run()
  {
    if (m_length == 0)
    {
      return;
    }

    // figure out if we can use a radix-4 FFT : otherwise radix-2
    auto rad = Radix(UncheckedNumericCast<int>(m_length));
    if (4 == rad && ((16 == m_length) || (256 == m_length) || (4096 == m_length) || (65536 == m_length)))
    {
      rad = 2;
    }

    // log2(n) is the # of kernels that will be invoked (for a radix-2 FFT)
    const unsigned int log2n = Log2NFFT(m_length);
    if (log2n <= 0 || log2n >= 32)
    {
      throw NotSupportedException("Invalid length");
    }

    FSLLOG3_INFO("log2(fft size) = log2({})={}", m_length, log2n);
    FSLLOG3_INFO("Compiling radix-{} FFT Program for GPU...", rad);

    // compileProgram("fft.cl");

    const std::string strProgram = GetContentManager()->ReadAllText("fft.cl");
    ProgramEx program(m_context.Get(), m_deviceId, strProgram);

    FSLLOG3_INFO("creating radix-{} kernels...", rad);

    std::array<Kernel, FFT_MAX_LOG2N> kernels;
    if (2 == rad)
    {
      for (unsigned kk = 0; kk < log2n; kk++)
      {
        FSLLOG3_INFO("Creating kernel {} {} (p={})...", RADIX2_FFT_KERNEL, kk, g_p[kk]);
        kernels[kk].Reset(program.Get(), RADIX2_FFT_KERNEL);
      }
    }
    else
    {    // radix-4
      for (unsigned kk = 0; kk < log2n; kk += 2)
      {
        FSLLOG3_INFO("Creating kernel {} {}...", RADIX4_FFT_KERNEL, (kk >> 1));
        kernels[kk >> 1].Reset(program.Get(), RADIX4_FFT_KERNEL);
      }
    }

    const auto worksize = m_length;
    const std::size_t workOffset = 0;
    AllocateDeviceMemory(m_context.Get(), worksize, workOffset);

    if (2 == rad)
    {
      // FFT kernel invoked for p=1, p=2, ..., p=n/2
      // input and output swapped each time
      for (unsigned kk = 0; kk < log2n; kk++)
      {
        void* in = (0 == (kk & 1)) ? &m_deviceMemIntime : &m_deviceMemOutfft;
        void* out = (0 == (kk & 1)) ? &m_deviceMemOutfft : &m_deviceMemIntime;
        FSLLOG3_INFO("Setting kernel args for kernel {} (p={})...", kk, g_p[kk]);
        const cl_kernel hKernel = kernels[kk].Get();
        clSetKernelArg(hKernel, 0, sizeof(cl_mem), in);
        clSetKernelArg(hKernel, 1, sizeof(cl_mem), out);
        clSetKernelArg(hKernel, 2, sizeof(unsigned), &g_p[kk]);
        clSetKernelArg(hKernel, 3, sizeof(unsigned), &g_pminus1[kk]);
        clSetKernelArg(hKernel, 4, sizeof(cl_float), &g_minusPIoverp[kk]);
      }    // end (for 1,2,4,8,...N/2)
    }
    else
    {
      // radix-4, FFT kernel invoked for p=1, p=4, ..., p=n/4
      for (uint32_t kk = 0; kk < log2n; kk += 2)
      {
        uint32_t idx = kk >> 1;
        void* in = (0 == (idx & 1)) ? &m_deviceMemIntime : &m_deviceMemOutfft;
        void* out = (0 == (idx & 1)) ? &m_deviceMemOutfft : &m_deviceMemIntime;
        FSLLOG3_INFO("Setting kernel args for kernel {} (p={})...", idx, g_p[kk]);
        const cl_kernel hKernel = kernels[kk].Get();
        clSetKernelArg(hKernel, 0, sizeof(cl_mem), in);
        clSetKernelArg(hKernel, 1, sizeof(cl_mem), out);
        clSetKernelArg(hKernel, 2, sizeof(unsigned), &g_p[kk]);
        clSetKernelArg(hKernel, 3, sizeof(unsigned), &g_pminus1[kk]);
        clSetKernelArg(hKernel, 4, sizeof(unsigned), &g_twop[kk]);
        clSetKernelArg(hKernel, 5, sizeof(unsigned), &g_threep[kk]);
        clSetKernelArg(hKernel, 6, sizeof(cl_float), &g_minusPIover2p[kk]);
        clSetKernelArg(hKernel, 7, sizeof(cl_float), &g_minusPIover2p_2x[kk]);
        clSetKernelArg(hKernel, 8, sizeof(cl_float), &g_minusPIover2p_3x[kk]);
      }    // end (for 1,4,16,...,N/4)
    }      // end (if radix-2 or radix-4)

    const std::size_t globalWorkSize = (2 == rad) ? (1 << (log2n - 1)) : (m_length >> 2);
    const std::size_t localWorkSize = (m_blockSize <= globalWorkSize) ? m_blockSize : globalWorkSize;

    cl_mem d_result = Values::INVALID_MEM;
    if (2 == rad)
    {
      for (unsigned kk = 0; kk < log2n; kk++)
      {
        // note to self: up to 8 it works, beyond that it does not
        FSLLOG3_INFO("running kernel {} (p={})...", kk, g_p[kk]);

        cl_event hEvent = nullptr;
        RAPIDOPENCL_CHECK(
          clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[kk].Get(), 1, nullptr, &globalWorkSize, &localWorkSize, 0, nullptr, &hEvent));
        // Hand the event over to a managed object
        m_gpuExecution[kk].Reset(hEvent);

        d_result = ((0 == (kk & 1)) ? m_deviceMemOutfft : m_deviceMemIntime).Get();
      }
    }
    else
    {
      // radix-4
      for (uint32_t kk = 0; kk < log2n; kk += 2)
      {
        uint32_t idx = kk >> 1;
        FSLLOG3_INFO("running kernel {},  (p={})...", idx, g_p[kk]);

        cl_event hEvent = nullptr;
        RAPIDOPENCL_CHECK(
          clEnqueueNDRangeKernel(m_commandQueue.Get(), kernels[idx].Get(), 1, nullptr, &globalWorkSize, &localWorkSize, 0, nullptr, &hEvent));
        // Hand the event over to a managed object
        m_gpuExecution[idx].Reset(hEvent);

        d_result = ((0 == (kk & 1)) ? m_deviceMemOutfft : m_deviceMemIntime).Get();
      }
    }
    if (d_result == Values::INVALID_MEM)
    {
      throw std::runtime_error("Internal error. variable not set");
    }

    CopyFromDevice(m_commandQueue, d_result, m_outfft.data() + workOffset, 2 * worksize, &m_gpuDone);

    // wait for copy event
    const cl_event hGPUDone = m_gpuDone.Get();
    RAPIDOPENCL_CHECK(clWaitForEvents(1, &hGPUDone));
    PrintGpuTime((2 == rad) ? log2n : (log2n >> 1));

    FSLLOG3_INFO("Successful.");
    if (m_save)
    {
      auto result = BuildResultString(m_length, m_outfft);
      GetPersistentDataManager()->WriteAlltext("fft_output.csv", result);
    }
  }


  void FastFourierTransform::AllocateHostMemory(const std::size_t len)
  {
    m_Freal.resize(len);
    m_Fimag.resize(len);
    m_Rreal.resize(len);
    m_Rimag.resize(len);
    //  real/imag interleaved input time-domain samples
    m_intime.resize(len * 2);
    //  real/imag interleaved output FFT data
    m_outfft.resize(len * 2);

    const unsigned n = 16;
    for (unsigned i = 0; i < len; ++i)
    {
      m_Freal[i] = static_cast<float>((i + 1) % n);
      m_Fimag[i] = static_cast<float>((i + 1) % n);
      m_intime[2 * i] = m_intime[2 * i + 1] = static_cast<float>((i + 1) % n);
      // m_Rreal[i] = 0;
      // m_Rimag[i] = 0;
      m_outfft[2 * i] = m_outfft[2 * i + 1] = 0;
    }
  }


  void FastFourierTransform::AllocateDeviceMemory(const cl_context context, const unsigned size, const unsigned copyOffset)
  {
    const auto defaultFlags = CL_MEM_COPY_HOST_PTR;

    m_deviceMemFreal.Reset(context, defaultFlags | CL_MEM_READ_ONLY, sizeof(float) * size, m_Freal.data() + copyOffset);
    CopyToDevice(m_commandQueue, m_deviceMemFreal.Get(), m_Freal.data() + copyOffset, size);

    m_deviceMemFimag.Reset(context, defaultFlags | CL_MEM_READ_ONLY, sizeof(float) * size, m_Fimag.data() + copyOffset);
    CopyToDevice(m_commandQueue, m_deviceMemFimag.Get(), m_Fimag.data() + copyOffset, size);

    //  copy real/imag interleaved input data to device
    m_deviceMemIntime.Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(float) * size * 2, m_intime.data() + copyOffset * 2);
    CopyFromDevice(m_commandQueue, m_deviceMemIntime.Get(), m_outfft.data(), size * 2, nullptr);    // debug

    m_deviceMemRreal.Reset(context, defaultFlags | CL_MEM_WRITE_ONLY, sizeof(float) * size, m_Rreal.data() + copyOffset);
    CopyToDevice(m_commandQueue, m_deviceMemRreal.Get(), m_Rreal.data() + copyOffset, size);

    m_deviceMemRimag.Reset(context, defaultFlags | CL_MEM_WRITE_ONLY, sizeof(float) * size, m_Rimag.data() + copyOffset);
    CopyToDevice(m_commandQueue, m_deviceMemRimag.Get(), m_Rimag.data() + copyOffset, size);

    //  copy real/imag interleaved out FFT to device
    m_deviceMemOutfft.Reset(context, defaultFlags | CL_MEM_READ_WRITE, sizeof(float) * size * 2, m_outfft.data() + copyOffset * 2);
    CopyToDevice(m_commandQueue, m_deviceMemIntime.Get(), m_outfft.data() + copyOffset * 2, size * 2);
  }


  void FastFourierTransform::PrintGpuTime(const uint32_t kernelCount) const
  {
    assert(kernelCount < m_gpuExecution.size());

    double total = 0;
    for (uint32_t i = 0; i < kernelCount; ++i)
    {
      double t = GetExecutionTime(m_gpuExecution[i].Get());
      FSLLOG3_INFO("Kernel execution time on GPU (kernel {}): {:10.6} seconds", i, t);
      total += t;
    }
    FSLLOG3_INFO("Total Kernel execution time on GPU: {:10.6} seconds", total);
  }
}
