#ifndef OPENVX_SOFTISP_SOFTISP_HPP
#define OPENVX_SOFTISP_SOFTISP_HPP
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

#include <FslDemoApp/OpenVX/DemoAppOpenVX.hpp>
#include <RapidOpenVX/Context.hpp>
#include <RapidOpenVX/Distribution.hpp>
#include <RapidOpenVX/Graph.hpp>
#include <RapidOpenVX/Image.hpp>
#include <VX/vx.h>
#include <VX/vx_ext_program.h>
#include <VX/vx_helper.h>
#include <VX/vxu.h>
#include "UserNodes.hpp"

namespace Fsl
{
  class SoftISP : public DemoAppOpenVX
  {
    RapidOpenVX::Context m_context;
    RapidOpenVX::Graph m_graph;
    const std::size_t m_imgWid = 1920;
    const std::size_t m_imgHei = 1080;
    const std::size_t m_imgSize = 1920 * 1080;

    std::vector<RapidOpenVX::Image> m_imagesObj;
    std::vector<RapidOpenVX::Distribution> m_dist;
    bool m_save;
    bool m_denoiseStatus;

  public:
    SoftISP(const DemoAppConfig& config);
    ~SoftISP();

  protected:
    virtual void Run() override;
    void AllocateMemory(const vx_context context);
    void CopyToBMP(Bitmap& bitmap, const IO::Path& fileName, const RapidOpenVX::Image& image);
  };
}

#endif
