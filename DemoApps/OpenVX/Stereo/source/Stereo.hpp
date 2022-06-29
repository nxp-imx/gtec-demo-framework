#ifndef OPENVX_STEREO_STEREO_HPP
#define OPENVX_STEREO_STEREO_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <VX/vx.h>
#include <VX/vx_ext_program.h>
#include <VX/vx_helper.h>
#include <VX/vxu.h>
#include <iostream>
#include "UserNodes.hpp"

namespace Fsl
{
  class Stereo : public DemoAppOpenVX
  {
    RapidOpenVX::Context m_context;
    std::vector<RapidOpenVX::Graph> m_graphl;
    std::vector<RapidOpenVX::Graph> m_graphr;
    std::vector<RapidOpenVX::Image> m_mImagesLeft;
    std::vector<RapidOpenVX::Image> m_mImagesRight;
    RapidOpenVX::Image m_mImagesLeft4;
    RapidOpenVX::Image m_mImagesRight4;
    RapidOpenVX::Image m_mImagesLeft2;
    RapidOpenVX::Image m_mImagesRight2;
    RapidOpenVX::Image m_mImagesLeft1;
    RapidOpenVX::Image m_mImagesRight1;
    cv::Size m_mSize1;
    cv::Size m_mSize2;
    cv::Size m_mSize4;

    cv::Mat m_mImgl1;
    cv::Mat m_mImgr1;
    cv::Mat m_mImgl2;
    cv::Mat m_mImgr2;
    cv::Mat m_mImgl4;
    cv::Mat m_mImgr4;

    vx_int32 m_minDisparity;
    vx_int32 m_maxDisparity;
    vx_int32 m_disparityRange4;
    vx_int32 m_disparityRange2;
    vx_int32 m_disparityRange1;

  public:
    Stereo(const DemoAppConfig& config);
    ~Stereo() override;

  protected:
    void Run() override;
    void ProcessGraph(const vx_graph inGraph);
    void SetNodes(UserNodes* inNode, const int32_t pyr);
    void SetNodes1(UserNodes* inNode);
    void AllocateMemory(const vx_context context, cv::Size imgSize);
    void LoadInputImage(cv::Mat imglRGB, cv::Mat imgrRGB);
    void CopyToBuffer(vx_image inImage, const cv::Mat input);
    void SaveImage(const vx_image inImage, const std::string& fileName, const cv::Size imgSize);
  };
}

#endif
