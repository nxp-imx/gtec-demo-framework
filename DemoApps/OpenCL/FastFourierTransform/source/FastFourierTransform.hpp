#ifndef OPENCL_FASTFOURIERTRANSFORM_FASTFOURIERTRANSFORM_HPP
#define OPENCL_FASTFOURIERTRANSFORM_FASTFOURIERTRANSFORM_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslDemoAppOpenCL/DemoAppOpenCL.hpp>
#include <FslUtilOpenCL1_1/Buffer.hpp>
#include <FslUtilOpenCL1_1/ContextEx.hpp>
#include <FslUtilOpenCL1_1/CommandQueue.hpp>
#include <FslUtilOpenCL1_1/Kernel.hpp>
#include <FslUtilOpenCL1_1/UserEvent.hpp>
#include <array>

namespace Fsl
{
  // Support 2^16 = 65536 point FFT
  const int32_t FFT_MAX_LOG2N = 16;
  const int32_t FFT_MAX = 1 << FFT_MAX_LOG2N;


  class FastFourierTransform : public DemoAppOpenCL
  {
    uint32_t m_length;
    bool m_save;
    std::size_t m_blockSize;

    OpenCL::ContextEx m_context;
    cl_device_id m_deviceId;
    OpenCL::CommandQueue m_commandQueue;

    // h_Freal and h_Fimag represent the input signal to be transformed.
    std::vector<float> m_Freal;
    std::vector<float> m_Fimag;
    // h_Rreal and h_Rimag represent the transformed output.
    std::vector<float> m_Rreal;
    std::vector<float> m_Rimag;

    //  real & imag interleaved
    // time-domain input samples
    std::vector<float> m_intime;
    // freq-domain output samples
    std::vector<float> m_outfft;

    // m_deviceMemFreal and m_deviceMemFimag represent the input signal to be transformed.
    // m_deviceMemRreal and m_deviceMemRimag represent the transformed output.
    OpenCL::Buffer m_deviceMemFreal;
    OpenCL::Buffer m_deviceMemFimag;
    OpenCL::Buffer m_deviceMemRreal;
    OpenCL::Buffer m_deviceMemRimag;
    // real & imag interleaved
    OpenCL::Buffer m_deviceMemIntime; // time-domain input samples
    OpenCL::Buffer m_deviceMemOutfft; // freq-domain output samples

    OpenCL::UserEvent m_gpuDone;
    std::array<OpenCL::UserEvent, FFT_MAX_LOG2N> m_gpuExecution;

  public:
    FastFourierTransform(const DemoAppConfig& config);
    ~FastFourierTransform();
  protected:
    virtual void Run() override;
  private:
    void AllocateHostMemory(const std::size_t len);
    void AllocateDeviceMemory(const cl_context context, const unsigned size, const unsigned copyOffset);
    void PrintGpuTime(const uint32_t kernelCount) const;
  };
}

#endif
