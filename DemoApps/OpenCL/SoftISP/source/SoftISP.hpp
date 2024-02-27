#ifndef OPENCL_SOFTISP_SOFTISP_HPP
#define OPENCL_SOFTISP_SOFTISP_HPP
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

#include <FslBase/Span/Span.hpp>
#include <FslDemoApp/OpenCL/DemoAppOpenCL.hpp>
#include <FslUtil/OpenCL1_2/ContextEx.hpp>
#include <RapidOpenCL1/Buffer.hpp>
#include <RapidOpenCL1/CommandQueue.hpp>
#include <RapidOpenCL1/Kernel.hpp>
#include <RapidOpenCL1/UserEvent.hpp>
#include <vector>

namespace Fsl
{
  class SoftISP : public DemoAppOpenCL
  {
    bool m_denoiseEn;
    int32_t m_cycleNum = 1000;

    std::vector<uint8_t> m_dst0;
    std::vector<uint8_t> m_dst1;
    std::vector<uint8_t> m_dst2;
    std::vector<uint8_t> m_dst3;
    std::vector<uint8_t> m_dst4;
    std::vector<uint8_t> m_dst5;
    std::vector<uint8_t> m_yBuf;
    std::vector<uint8_t> m_yBufOut;
    std::vector<uint8_t> m_uvBuf;
    int m_pixelValueR;
    int m_pixelValueG;
    int m_pixelValueB;
    std::vector<int> m_inDistR;
    std::vector<int> m_inDistG;
    std::vector<int> m_inDistB;
    std::vector<int> m_outDistR;
    std::vector<int> m_outDistG;
    std::vector<int> m_outDistB;
    bool m_save;

    std::vector<RapidOpenCL1::Buffer> m_deviceImg;
    std::vector<RapidOpenCL1::Buffer> m_pixelValue;
    std::vector<RapidOpenCL1::Buffer> m_deviceDist;

  public:
    explicit SoftISP(const DemoAppConfig& config);
    ~SoftISP() override;

  protected:
    void Run() override;
    void AllocateMemory(const cl_context context, const std::size_t size);
    void CopyToBMP(Bitmap& rBitmap, const IO::Path& fileName, const Span<uint8_t> span);
  };
}

#endif
