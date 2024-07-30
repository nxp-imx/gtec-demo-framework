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
#include <FslBase/Span/SpanUtil_Vector.hpp>
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

namespace Fsl
{
  namespace
  {
    namespace KernelName
    {
      constexpr auto RadiX2Fft = "fft_radix2";
      constexpr auto RadiX4Fft = "fft_radix4";
    }


    const std::array<int, FftMaxLoG2N> g_p = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};


    const std::array<int, FftMaxLoG2N> g_twop = {2 * 1,   2 * 2,   2 * 4,    2 * 8,    2 * 16,   2 * 32,   2 * 64,    2 * 128,
                                                 2 * 256, 2 * 512, 2 * 1024, 2 * 2048, 2 * 4096, 2 * 8192, 2 * 16384, 2 * 32768};


    const std::array<int, FftMaxLoG2N> g_threep = {3 * 1,   3 * 2,   3 * 4,    3 * 8,    3 * 16,   3 * 32,   3 * 64,    3 * 128,
                                                   3 * 256, 3 * 512, 3 * 1024, 3 * 2048, 3 * 4096, 3 * 8192, 3 * 16384, 3 * 32768};


    const std::array<int, FftMaxLoG2N> g_pminus1 = {1 - 1,   2 - 1,   4 - 1,    8 - 1,    16 - 1,   32 - 1,   64 - 1,    128 - 1,
                                                    256 - 1, 512 - 1, 1024 - 1, 2048 - 1, 4096 - 1, 8192 - 1, 16384 - 1, 32768 - 1};

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const cl_float g_minusPIoverp[FftMaxLoG2N] = {
      -MathHelper::PI,          -MathHelper::PI / 2.f,    -MathHelper::PI / 4.f,     -MathHelper::PI / 8.f,
      -MathHelper::PI / 16.f,   -MathHelper::PI / 32.f,   -MathHelper::PI / 64.f,    -MathHelper::PI / 128.f,
      -MathHelper::PI / 256.f,  -MathHelper::PI / 512.f,  -MathHelper::PI / 1024.f,  -MathHelper::PI / 2048.f,
      -MathHelper::PI / 4096.f, -MathHelper::PI / 8192.f, -MathHelper::PI / 16384.f, -MathHelper::PI / 32768.f};

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const cl_float g_minusPIover2p[FftMaxLoG2N] = {
      -MathHelper::PI / 2.f,    -MathHelper::PI / 4.f,     -MathHelper::PI / 8.f,     -MathHelper::PI / 16.f,
      -MathHelper::PI / 32.f,   -MathHelper::PI / 64.f,    -MathHelper::PI / 128.f,   -MathHelper::PI / 256.f,
      -MathHelper::PI / 512.f,  -MathHelper::PI / 1024.f,  -MathHelper::PI / 2048.f,  -MathHelper::PI / 4096.f,
      -MathHelper::PI / 8192.f, -MathHelper::PI / 16384.f, -MathHelper::PI / 32768.f, -MathHelper::PI / 65536.f};

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const cl_float g_minusPIover2p2x[FftMaxLoG2N] = {
      -2.f * MathHelper::PI / 2.f,    -2.f * MathHelper::PI / 4.f,     -2.f * MathHelper::PI / 8.f,     -2.f * MathHelper::PI / 16.f,
      -2.f * MathHelper::PI / 32.f,   -2.f * MathHelper::PI / 64.f,    -2.f * MathHelper::PI / 128.f,   -2.f * MathHelper::PI / 256.f,
      -2.f * MathHelper::PI / 512.f,  -2.f * MathHelper::PI / 1024.f,  -2.f * MathHelper::PI / 2048.f,  -2.f * MathHelper::PI / 4096.f,
      -2.f * MathHelper::PI / 8192.f, -2.f * MathHelper::PI / 16384.f, -2.f * MathHelper::PI / 32768.f, -2.f * MathHelper::PI / 65536.f};

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const cl_float g_minusPIover2p3x[FftMaxLoG2N] = {
      -3.f * MathHelper::PI / 2.f,    -3.f * MathHelper::PI / 4.f,     -3.f * MathHelper::PI / 8.f,     -3.f * MathHelper::PI / 16.f,
      -3.f * MathHelper::PI / 32.f,   -3.f * MathHelper::PI / 64.f,    -3.f * MathHelper::PI / 128.f,   -3.f * MathHelper::PI / 256.f,
      -3.f * MathHelper::PI / 512.f,  -3.f * MathHelper::PI / 1024.f,  -3.f * MathHelper::PI / 2048.f,  -3.f * MathHelper::PI / 4096.f,
      -3.f * MathHelper::PI / 8192.f, -3.f * MathHelper::PI / 16384.f, -3.f * MathHelper::PI / 32768.f, -3.f * MathHelper::PI / 65536.f};


    int Radix(const int n)
    {
      int j = 0;
      for (int i = 0; i <= 31; ++i)
      {
        if ((n & (1 << i)) == 0)
        {
          ++j;
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


    void CopyToDevice(const RapidOpenCL1::CommandQueue& commandQueue, const cl_mem mem, const ReadOnlySpan<float> hostSpan)
    {
      RAPIDOPENCL_CHECK(
        clEnqueueWriteBuffer(commandQueue.Get(), mem, CL_FALSE, 0, sizeof(float) * hostSpan.size(), hostSpan.data(), 0, nullptr, nullptr));
    }


    void CopyFromDevice(const RapidOpenCL1::CommandQueue& commandQueue, const cl_mem dMem, Span<float> hostSpan, RapidOpenCL1::UserEvent* pGpuDone)
    {
      if (pGpuDone == nullptr)
      {
        RAPIDOPENCL_CHECK(
          clEnqueueReadBuffer(commandQueue.Get(), dMem, CL_FALSE, 0, sizeof(float) * hostSpan.size(), hostSpan.data(), 0, nullptr, nullptr));
      }
      else
      {
        cl_event hEvent = nullptr;
        RAPIDOPENCL_CHECK(
          clEnqueueReadBuffer(commandQueue.Get(), dMem, CL_FALSE, 0, sizeof(float) * hostSpan.size(), hostSpan.data(), 0, nullptr, &hEvent));
        // Hand off the event to a managed object
        pGpuDone->Reset(hEvent);
      }
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
    , m_gpuDone(RapidOpenCL1::Values::INVALID_EVENT)
  {
    auto optionParser = config.GetOptions<OptionParser>();
    const auto length = optionParser->GetLength();
    if (length > FftMax)
    {
      throw std::invalid_argument(fmt::format("FFT length cannot be greater than {}", FftMax));
    }

    if (length < 16)
    {
      throw std::invalid_argument("FFT length has to at least be 16.");
    }

    if ((length != 1) && ((length & (length - 1)) != 0u))
    {
      throw std::invalid_argument(fmt::format("FFT length ({}) must be a power-of-2.", length));
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

      // create the OpenCL context on available GPU devices
      m_context.Reset(CL_DEVICE_TYPE_GPU);

      FSLLOG3_INFO("Creating Command Queue...");
      m_commandQueue.Reset(m_context.Get(), m_context.GetDeviceId(), CL_QUEUE_PROFILING_ENABLE);
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
    OpenCL::ProgramEx program(m_context.Get(), m_context.GetDeviceId(), strProgram);

    FSLLOG3_INFO("creating radix-{} kernels...", rad);

    std::array<RapidOpenCL1::Kernel, FftMaxLoG2N> kernels;
    if (2 == rad)
    {
      for (unsigned kk = 0; kk < log2n; kk++)
      {
        FSLLOG3_INFO("Creating kernel {} {} (p={})...", KernelName::RadiX2Fft, kk, g_p[kk]);
        kernels[kk].Reset(program.Get(), KernelName::RadiX2Fft);
      }
    }
    else
    {    // radix-4
      for (unsigned kk = 0; kk < log2n; kk += 2)
      {
        FSLLOG3_INFO("Creating kernel {} {}...", KernelName::RadiX4Fft, (kk >> 1));
        kernels[kk >> 1].Reset(program.Get(), KernelName::RadiX4Fft);
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
        void* in = (0 == (kk & 1)) ? &m_deviceMemInTime : &m_deviceMemOutFft;
        void* out = (0 == (kk & 1)) ? &m_deviceMemOutFft : &m_deviceMemInTime;
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
        void* in = (0 == (idx & 1)) ? &m_deviceMemInTime : &m_deviceMemOutFft;
        void* out = (0 == (idx & 1)) ? &m_deviceMemOutFft : &m_deviceMemInTime;
        FSLLOG3_INFO("Setting kernel args for kernel {} (p={})...", idx, g_p[kk]);
        const cl_kernel hKernel = kernels[kk].Get();
        clSetKernelArg(hKernel, 0, sizeof(cl_mem), in);
        clSetKernelArg(hKernel, 1, sizeof(cl_mem), out);
        clSetKernelArg(hKernel, 2, sizeof(unsigned), &g_p[kk]);
        clSetKernelArg(hKernel, 3, sizeof(unsigned), &g_pminus1[kk]);
        clSetKernelArg(hKernel, 4, sizeof(unsigned), &g_twop[kk]);
        clSetKernelArg(hKernel, 5, sizeof(unsigned), &g_threep[kk]);
        clSetKernelArg(hKernel, 6, sizeof(cl_float), &g_minusPIover2p[kk]);
        clSetKernelArg(hKernel, 7, sizeof(cl_float), &g_minusPIover2p2x[kk]);
        clSetKernelArg(hKernel, 8, sizeof(cl_float), &g_minusPIover2p3x[kk]);
      }    // end (for 1,4,16,...,N/4)
    }    // end (if radix-2 or radix-4)

    const std::size_t globalWorkSize = (2 == rad) ? (1 << (log2n - 1)) : (m_length >> 2);
    const std::size_t localWorkSize = (m_blockSize <= globalWorkSize) ? m_blockSize : globalWorkSize;

    cl_mem dResult = RapidOpenCL1::Values::INVALID_MEM;
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

        dResult = ((0 == (kk & 1)) ? m_deviceMemOutFft : m_deviceMemInTime).Get();
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

        dResult = ((0 == (kk & 1)) ? m_deviceMemOutFft : m_deviceMemInTime).Get();
      }
    }
    if (dResult == RapidOpenCL1::Values::INVALID_MEM)
    {
      throw std::runtime_error("Internal error. variable not set");
    }

    CopyFromDevice(m_commandQueue, dResult, SpanUtil::AsSpan(m_outFft, workOffset, 2 * worksize), &m_gpuDone);

    // wait for copy event
    const cl_event hGPUDone = m_gpuDone.Get();
    RAPIDOPENCL_CHECK(clWaitForEvents(1, &hGPUDone));
    PrintGpuTime((2 == rad) ? log2n : (log2n >> 1));

    FSLLOG3_INFO("Successful.");
    if (m_save)
    {
      auto result = BuildResultString(m_length, m_outFft);
      GetPersistentDataManager()->WriteAlltext("fft_output.csv", result);
    }
  }


  void FastFourierTransform::AllocateHostMemory(const std::size_t len)
  {
    m_realF.resize(len);
    m_imagF.resize(len);
    m_realR.resize(len);
    m_imagR.resize(len);
    //  real/imag interleaved input time-domain samples
    m_intime.resize(len * 2);
    //  real/imag interleaved output FFT data
    m_outFft.resize(len * 2);

    const unsigned n = 16;
    for (unsigned i = 0; i < len; ++i)
    {
      m_realF[i] = static_cast<float>((i + 1) % n);
      m_imagF[i] = static_cast<float>((i + 1) % n);
      m_intime[2 * i] = m_intime[2 * i + 1] = static_cast<float>((i + 1) % n);
      // m_Rreal[i] = 0;
      // m_Rimag[i] = 0;
      m_outFft[2 * i] = m_outFft[2 * i + 1] = 0;
    }
  }


  void FastFourierTransform::AllocateDeviceMemory(const cl_context context, const unsigned size, const unsigned copyOffset)
  {
    constexpr auto DefaultFlags = CL_MEM_COPY_HOST_PTR;
    {
      Span<float> span = SpanUtil::AsSpan(m_realF, copyOffset, size);
      m_deviceMemRealF.Reset(context, DefaultFlags | CL_MEM_READ_ONLY, sizeof(float) * span.size(), span.data());
      CopyToDevice(m_commandQueue, m_deviceMemRealF.Get(), span);
    }

    {
      Span<float> span = SpanUtil::AsSpan(m_imagF, copyOffset, size);
      m_deviceMemImagF.Reset(context, DefaultFlags | CL_MEM_READ_ONLY, sizeof(float) * span.size(), span.data());
      CopyToDevice(m_commandQueue, m_deviceMemImagF.Get(), span);
    }

    {    //  copy real/imag interleaved input data to device
      Span<float> span = SpanUtil::AsSpan(m_intime, copyOffset * 2, size * 2);
      m_deviceMemInTime.Reset(context, DefaultFlags | CL_MEM_READ_WRITE, sizeof(float) * span.size(), span.data());
      Span<float> span2 = SpanUtil::AsSpan(m_outFft, 0, size * 2);
      CopyFromDevice(m_commandQueue, m_deviceMemInTime.Get(), span2, nullptr);    // debug
    }

    {
      Span<float> span = SpanUtil::AsSpan(m_realR, copyOffset, size);
      m_deviceMemRealR.Reset(context, DefaultFlags | CL_MEM_WRITE_ONLY, sizeof(float) * span.size(), span.data());
      CopyToDevice(m_commandQueue, m_deviceMemRealR.Get(), span);
    }

    {
      Span<float> span = SpanUtil::AsSpan(m_imagR, copyOffset, size);
      m_deviceMemImagR.Reset(context, DefaultFlags | CL_MEM_WRITE_ONLY, sizeof(float) * span.size(), span.data());
      CopyToDevice(m_commandQueue, m_deviceMemImagR.Get(), span);
    }

    {    //  copy real/imag interleaved out FFT to device
      Span<float> span = SpanUtil::AsSpan(m_outFft, copyOffset * 2, size * 2);
      m_deviceMemOutFft.Reset(context, DefaultFlags | CL_MEM_READ_WRITE, sizeof(float) * span.size(), span.data());
      CopyToDevice(m_commandQueue, m_deviceMemInTime.Get(), span);
    }
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
