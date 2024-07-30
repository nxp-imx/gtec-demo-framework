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

#include <FslDemoApp/OpenCL/DemoAppOpenCL.hpp>
#include <FslUtil/OpenCL1_2/ContextEx.hpp>
#include <RapidOpenCL1/Buffer.hpp>
#include <RapidOpenCL1/CommandQueue.hpp>
#include <RapidOpenCL1/Kernel.hpp>
#include <RapidOpenCL1/UserEvent.hpp>
#include <array>

namespace Fsl
{
  // Support 2^16 = 65536 point FFT
  constexpr int32_t FftMaxLoG2N = 16;
  constexpr int32_t FftMax = 1 << FftMaxLoG2N;


  class FastFourierTransform final : public DemoAppOpenCL
  {
    uint32_t m_length;
    bool m_save;
    std::size_t m_blockSize;

    OpenCL::ContextEx m_context;
    RapidOpenCL1::CommandQueue m_commandQueue;

    // h_Freal and h_Fimag represent the input signal to be transformed.
    std::vector<float> m_realF;
    std::vector<float> m_imagF;
    // h_Rreal and h_Rimag represent the transformed output.
    std::vector<float> m_realR;
    std::vector<float> m_imagR;

    //  real & imag interleaved
    // time-domain input samples
    std::vector<float> m_intime;
    // freq-domain output samples
    std::vector<float> m_outFft;

    // m_deviceMemRealF and m_deviceMemImagR represent the input signal to be transformed.
    // m_deviceMemRealR and m_deviceMemImagR represent the transformed output.
    RapidOpenCL1::Buffer m_deviceMemRealF;
    RapidOpenCL1::Buffer m_deviceMemImagF;
    RapidOpenCL1::Buffer m_deviceMemRealR;
    RapidOpenCL1::Buffer m_deviceMemImagR;
    // real & imag interleaved
    RapidOpenCL1::Buffer m_deviceMemInTime;    // time-domain input samples
    RapidOpenCL1::Buffer m_deviceMemOutFft;    // freq-domain output samples

    RapidOpenCL1::UserEvent m_gpuDone;
    std::array<RapidOpenCL1::UserEvent, FftMaxLoG2N> m_gpuExecution;

  public:
    explicit FastFourierTransform(const DemoAppConfig& config);
    ~FastFourierTransform() final;

  protected:
    void Run() final;

  private:
    void AllocateHostMemory(const std::size_t len);
    void AllocateDeviceMemory(const cl_context context, const unsigned size, const unsigned copyOffset);
    void PrintGpuTime(const uint32_t kernelCount) const;
  };
}

#endif
