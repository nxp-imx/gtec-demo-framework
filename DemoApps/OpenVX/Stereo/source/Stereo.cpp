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

// OpenVX 1.1 project
#include "Stereo.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <RapidOpenVX/Check.hpp>


namespace Fsl
{
  using namespace cv;
  using namespace std;

  using namespace RapidOpenVX;

  namespace
  {
  }

  Stereo::Stereo(const DemoAppConfig& config)
    : DemoAppOpenVX(config)
    , m_context(ResetMode::Create)
  {
    m_mSize1 = Size(640, 480);
    m_mSize2 = Size(m_mSize1.width / 2, m_mSize1.height / 2);
    m_mSize4 = Size(m_mSize2.width / 2, m_mSize2.height / 2);
    m_minDisparity = 0;
    m_maxDisparity = 119;

    m_disparityRange4 = abs((m_maxDisparity - m_minDisparity) / 4);
    m_disparityRange2 = abs((m_maxDisparity - m_minDisparity) / 2);
    m_disparityRange1 = abs((m_maxDisparity - m_minDisparity));
  }

  Stereo::~Stereo() = default;

  void Stereo::Run()
  {
    Mat imglRGB;
    Mat imgrRGB;
    m_graphl.resize(3);
    m_graphr.resize(3);

    UserNodes nodes(GetContentManager());
    RAPIDOPENVX_CHECK(nodes.VXPublishNodeKernels(m_context.Get()));
    // size 1/4
    AllocateMemory(m_context.Get(), m_mSize4);
    const vx_df_image fmtColor = VX_DF_IMAGE_U8;
    m_mImagesLeft4.Reset(m_context.Get(), static_cast<vx_uint32>(m_mSize4.width), static_cast<vx_uint32>(m_mSize4.height), fmtColor);
    m_mImagesRight4.Reset(m_context.Get(), static_cast<vx_uint32>(m_mSize4.width), static_cast<vx_uint32>(m_mSize4.height), fmtColor);
    m_graphl[0].Reset(m_context.Get());
    m_graphl[1].Reset(m_context.Get());
    m_graphl[2].Reset(m_context.Get());

    m_graphr[0].Reset(m_context.Get());
    m_graphr[1].Reset(m_context.Get());
    m_graphr[2].Reset(m_context.Get());

    auto contentManager = GetContentManager();
    IO::Path contentPath = contentManager->GetContentPath();
    IO::Path pathImgl = IO::Path::Combine(contentPath, "im0.png");
    IO::Path pathImgr = IO::Path::Combine(contentPath, "im1.png");

    imglRGB = imread(pathImgl.ToUTF8String());
    imgrRGB = imread(pathImgr.ToUTF8String());

    LoadInputImage(imglRGB, imgrRGB);
    CopyToBuffer(m_mImagesLeft[0].Get(), m_mImgl4);
    CopyToBuffer(m_mImagesRight[0].Get(), m_mImgr4);

    auto directive = VX_DIRECTIVE_ENABLE_PERFORMANCE;
    RAPIDOPENVX_CHECK(vxDirective((vx_reference)m_context.Get(), directive));

    SetNodes(&nodes, 4);

    ProcessGraph(m_graphl[0].Get());
    ProcessGraph(m_graphr[0].Get());

    ProcessGraph(m_graphl[1].Get());
    ProcessGraph(m_graphr[1].Get());

    ProcessGraph(m_graphl[2].Get());
    ProcessGraph(m_graphr[2].Get());

    vx_perf_t perfL[3];
    vx_perf_t perfR[3];
    vx_uint64 timeSum = 0;
    auto inAttribute = VX_GRAPH_PERFORMANCE;
    for (int32_t i = 0; i < 3; i++)
    {
      RAPIDOPENVX_CHECK(vxQueryGraph(m_graphl[i].Get(), inAttribute, &perfL[i], sizeof(perfL[i])));
      RAPIDOPENVX_CHECK(vxQueryGraph(m_graphr[i].Get(), inAttribute, &perfR[i], sizeof(perfR[i])));
      timeSum += perfL[i].avg;
      timeSum += perfR[i].avg;
    }
    FSLLOG3_INFO("");
    FSLLOG3_INFO("Kernels execution time:");
    FSLLOG3_INFO("      -----------------------");
    FSLLOG3_INFO("      1/4 execution time:        {}ms", timeSum / 1000000);

    // size 1/2
    AllocateMemory(m_context.Get(), m_mSize2);
    m_mImagesLeft2.Reset(m_context.Get(), static_cast<vx_uint32>(m_mSize2.width), static_cast<vx_uint32>(m_mSize2.height), fmtColor);
    m_mImagesRight2.Reset(m_context.Get(), static_cast<vx_uint32>(m_mSize2.width), static_cast<vx_uint32>(m_mSize2.height), fmtColor);
    m_graphl[0].Reset(m_context.Get());
    m_graphl[1].Reset(m_context.Get());
    m_graphl[2].Reset(m_context.Get());

    m_graphr[0].Reset(m_context.Get());
    m_graphr[1].Reset(m_context.Get());
    m_graphr[2].Reset(m_context.Get());

    CopyToBuffer(m_mImagesLeft[0].Get(), m_mImgl2);
    CopyToBuffer(m_mImagesRight[0].Get(), m_mImgr2);

    SetNodes(&nodes, 2);

    ProcessGraph(m_graphl[0].Get());
    ProcessGraph(m_graphr[0].Get());

    ProcessGraph(m_graphl[1].Get());
    ProcessGraph(m_graphr[1].Get());

    ProcessGraph(m_graphl[2].Get());
    ProcessGraph(m_graphr[2].Get());

    vx_uint64 timeSum1 = 0;
    for (int32_t i = 0; i < 3; i++)
    {
      RAPIDOPENVX_CHECK(vxQueryGraph(m_graphl[i].Get(), inAttribute, &perfL[i], sizeof(perfL[i])));
      RAPIDOPENVX_CHECK(vxQueryGraph(m_graphr[i].Get(), inAttribute, &perfR[i], sizeof(perfR[i])));
      timeSum1 += perfL[i].avg;
      timeSum1 += perfR[i].avg;
    }
    FSLLOG3_INFO("");
    FSLLOG3_INFO("      -----------------------");
    FSLLOG3_INFO("      1/2 execution time:        {}ms", timeSum1 / 1000000);

    // size 1/1
    AllocateMemory(m_context.Get(), m_mSize1);
    m_mImagesLeft1.Reset(m_context.Get(), static_cast<vx_uint32>(m_mSize1.width), static_cast<vx_uint32>(m_mSize1.height), fmtColor);
    m_mImagesRight1.Reset(m_context.Get(), static_cast<vx_uint32>(m_mSize1.width), static_cast<vx_uint32>(m_mSize1.height), fmtColor);
    m_graphl[0].Reset(m_context.Get());
    m_graphl[1].Reset(m_context.Get());
    m_graphl[2].Reset(m_context.Get());

    m_graphr[0].Reset(m_context.Get());
    m_graphr[1].Reset(m_context.Get());
    m_graphr[2].Reset(m_context.Get());

    CopyToBuffer(m_mImagesLeft[0].Get(), m_mImgl1);
    CopyToBuffer(m_mImagesRight[0].Get(), m_mImgr1);

    SetNodes1(&nodes);

    ProcessGraph(m_graphl[0].Get());
    ProcessGraph(m_graphr[0].Get());

    ProcessGraph(m_graphl[1].Get());
    ProcessGraph(m_graphr[1].Get());

    ProcessGraph(m_graphl[2].Get());
    ProcessGraph(m_graphr[2].Get());

    vx_uint64 timeSum2 = 0;
    for (int32_t i = 0; i < 3; i++)
    {
      RAPIDOPENVX_CHECK(vxQueryGraph(m_graphl[i].Get(), inAttribute, &perfL[i], sizeof(perfL[i])));
      RAPIDOPENVX_CHECK(vxQueryGraph(m_graphr[i].Get(), inAttribute, &perfR[i], sizeof(perfR[i])));
      timeSum2 += perfL[i].avg;
      timeSum2 += perfR[i].avg;
    }
    FSLLOG3_INFO("");
    FSLLOG3_INFO("      -----------------------");
    FSLLOG3_INFO("      1/1 execution time:        {}ms", timeSum2 / 1000000);

    FSLLOG3_INFO("");
    FSLLOG3_INFO("      -----------------------");
    FSLLOG3_INFO("      Total execution time:        {}ms", (timeSum + timeSum1 + timeSum2) / 1000000);

    SaveImage(m_mImagesLeft1.Get(), "DisparityLeft.jpg", m_mSize1);
  }

  void Stereo::ProcessGraph(const vx_graph inGraph)
  {
    RAPIDOPENVX_CHECK(vxVerifyGraph(inGraph));
    RAPIDOPENVX_CHECK(vxProcessGraph(inGraph));
  }

  void Stereo::SetNodes(UserNodes* inNode, const int32_t pyr)
  {
    RAPIDOPENVX_CHECK(inNode->NXPCensusNode(m_graphl[0].Get(), m_mImagesLeft[0].Get(), m_mImagesLeft[1].Get()));
    RAPIDOPENVX_CHECK(inNode->NXPCensusNode(m_graphr[0].Get(), m_mImagesRight[0].Get(), m_mImagesRight[1].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPOutliersDetectionNode(m_graphl[1].Get(), m_mImagesLeft[2].Get(), m_mImagesLeft[3].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPOutliersDetectionNode(m_graphr[1].Get(), m_mImagesRight[2].Get(), m_mImagesRight[3].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPLeftRightConsistencyNode(m_graphl[2].Get(), m_mImagesLeft[3].Get(), m_mImagesRight[3].Get(), m_mImagesLeft[4].Get(),
                                                          m_mImagesRight[4].Get()));
    RAPIDOPENVX_CHECK(inNode->NXPDisparityRefinementNode(m_graphl[2].Get(), m_mImagesLeft[4].Get(), m_mImagesLeft[5].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPDisparityRefinementNode(m_graphr[2].Get(), m_mImagesRight[4].Get(), m_mImagesRight[5].Get()));
    if (pyr == 4)
    {
      RAPIDOPENVX_CHECK(inNode->NXPMatchLRNode(m_graphl[1].Get(), m_mImagesLeft[1].Get(), m_mImagesRight[1].Get(), m_mImagesLeft[2].Get(),
                                               m_minDisparity, m_disparityRange4));
      RAPIDOPENVX_CHECK(inNode->NXPMatchRRNode(m_graphr[1].Get(), m_mImagesLeft[1].Get(), m_mImagesRight[1].Get(), m_mImagesRight[2].Get(),
                                               m_minDisparity, m_disparityRange4));

      RAPIDOPENVX_CHECK(inNode->NXPMedianFilterNode(m_graphl[2].Get(), m_mImagesLeft[5].Get(), m_mImagesLeft4.Get()));
      RAPIDOPENVX_CHECK(inNode->NXPMedianFilterNode(m_graphr[2].Get(), m_mImagesRight[5].Get(), m_mImagesRight4.Get()));
    }
    else if (pyr == 2)
    {
      vx_int32 disRange = 4;
      RAPIDOPENVX_CHECK(inNode->NXPMatchFinerLRNode(m_graphl[1].Get(), m_mImagesLeft[1].Get(), m_mImagesRight[1].Get(), m_mImagesLeft[2].Get(),
                                                    m_minDisparity, m_disparityRange2, m_mImagesLeft4.Get(), m_mImagesRight4.Get(), disRange));
      RAPIDOPENVX_CHECK(inNode->NXPMatchFinerRRNode(m_graphr[1].Get(), m_mImagesLeft[1].Get(), m_mImagesRight[1].Get(), m_mImagesRight[2].Get(),
                                                    m_minDisparity, m_disparityRange2, m_mImagesLeft4.Get(), m_mImagesRight4.Get(), disRange));

      RAPIDOPENVX_CHECK(inNode->NXPMedianFilterNode(m_graphl[2].Get(), m_mImagesLeft[5].Get(), m_mImagesLeft2.Get()));
      RAPIDOPENVX_CHECK(inNode->NXPMedianFilterNode(m_graphr[2].Get(), m_mImagesRight[5].Get(), m_mImagesRight2.Get()));
    }
  }

  void Stereo::SetNodes1(UserNodes* inNode)
  {
    vx_int32 disRange = 6;
    RAPIDOPENVX_CHECK(inNode->NXPCensusNode(m_graphl[0].Get(), m_mImagesLeft[0].Get(), m_mImagesLeft[1].Get()));
    RAPIDOPENVX_CHECK(inNode->NXPCensusNode(m_graphr[0].Get(), m_mImagesRight[0].Get(), m_mImagesRight[1].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPMatchFinerLRNode(m_graphl[1].Get(), m_mImagesLeft[1].Get(), m_mImagesRight[1].Get(), m_mImagesLeft[2].Get(),
                                                  m_minDisparity, m_disparityRange1, m_mImagesLeft2.Get(), m_mImagesRight2.Get(), disRange));
    RAPIDOPENVX_CHECK(inNode->NXPMatchFinerRRNode(m_graphr[1].Get(), m_mImagesLeft[1].Get(), m_mImagesRight[1].Get(), m_mImagesRight[2].Get(),
                                                  m_minDisparity, m_disparityRange1, m_mImagesLeft2.Get(), m_mImagesRight2.Get(), disRange));

    RAPIDOPENVX_CHECK(inNode->NXPOutliersDetectionNode(m_graphl[1].Get(), m_mImagesLeft[2].Get(), m_mImagesLeft[3].Get()));
    RAPIDOPENVX_CHECK(inNode->NXPOutliersDetectionNode(m_graphr[1].Get(), m_mImagesRight[2].Get(), m_mImagesRight[3].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPDisparityRefinementNode(m_graphl[1].Get(), m_mImagesLeft[3].Get(), m_mImagesLeft[4].Get()));
    RAPIDOPENVX_CHECK(inNode->NXPDisparityRefinementNode(m_graphr[1].Get(), m_mImagesRight[3].Get(), m_mImagesRight[4].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPLeftRightConsistencyNode(m_graphl[2].Get(), m_mImagesLeft[4].Get(), m_mImagesRight[4].Get(), m_mImagesLeft[5].Get(),
                                                          m_mImagesRight[5].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPOutliersDetectionNode(m_graphl[2].Get(), m_mImagesLeft[5].Get(), m_mImagesLeft[6].Get()));
    RAPIDOPENVX_CHECK(inNode->NXPOutliersDetectionNode(m_graphr[2].Get(), m_mImagesRight[5].Get(), m_mImagesRight[6].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPDisparityRefinementNode(m_graphl[2].Get(), m_mImagesLeft[6].Get(), m_mImagesLeft[7].Get()));
    RAPIDOPENVX_CHECK(inNode->NXPDisparityRefinementNode(m_graphr[2].Get(), m_mImagesRight[6].Get(), m_mImagesRight[7].Get()));

    RAPIDOPENVX_CHECK(inNode->NXPMedianFilterNode(m_graphl[2].Get(), m_mImagesLeft[7].Get(), m_mImagesLeft1.Get()));
    RAPIDOPENVX_CHECK(inNode->NXPMedianFilterNode(m_graphr[2].Get(), m_mImagesRight[7].Get(), m_mImagesRight1.Get()));
  }

  void Stereo::CopyToBuffer(vx_image inImage, const Mat input)
  {
    auto width = static_cast<vx_uint32>(input.cols);
    auto height = static_cast<vx_uint32>(input.rows);
    auto strideX = static_cast<vx_int32>(input.elemSize());
    const auto strideY = static_cast<vx_int32>(width);
    const vx_rectangle_t imgRect = {0, 0, width, height};
    vx_imagepatch_addressing_t imgInfo = {width, height, strideX, strideY};
    vx_uint8* ocvData = input.data;

    const auto inFmt = VX_MEMORY_TYPE_HOST;
    const auto dataUsage = VX_WRITE_ONLY;
    RAPIDOPENVX_CHECK(vxCopyImagePatch(inImage, &imgRect, 0u, &imgInfo, ocvData, dataUsage, inFmt));
  }

  void Stereo::LoadInputImage(Mat imglRGB, Mat imgrRGB)
  {
    if (imglRGB.empty() || imgrRGB.empty())
    {
      FSLLOG3_ERROR("image opening failed.");
    }

    if (imglRGB.size() != imgrRGB.size())
    {
      FSLLOG3_ERROR("Left and right images must be the same size.");
    }

    if (imglRGB.size() != m_mSize1 || imgrRGB.size() != m_mSize1)
    {
      resize(imglRGB, imglRGB, m_mSize1);
      resize(imgrRGB, imgrRGB, m_mSize1);
    }

    cvtColor(imgrRGB, m_mImgr1, CV_BGR2GRAY);
    cvtColor(imglRGB, m_mImgl1, CV_BGR2GRAY);

    /*********************************************************************************************
    * Prepare different resolutions using gaussian pyramid (using OpenCV),
      also prepare variables used during the program
    *********************************************************************************************/

    // create different resolution mats
    // greyscale images
    pyrDown(m_mImgl1, m_mImgl2, Size(m_mImgl1.cols / 2, m_mImgl1.rows / 2));
    pyrDown(m_mImgl2, m_mImgl4, Size(m_mImgl2.cols / 2, m_mImgl2.rows / 2));

    pyrDown(m_mImgr1, m_mImgr2, Size(m_mImgr1.cols / 2, m_mImgr1.rows / 2));
    pyrDown(m_mImgr2, m_mImgr4, Size(m_mImgr2.cols / 2, m_mImgr2.rows / 2));
  }

  void Stereo::SaveImage(const vx_image inImage, const string& fileName, const Size imgSize)
  {
    vx_rectangle_t imgRect = {0, 0, 0, 0};
    const auto width = static_cast<vx_uint32>(imgSize.width);
    const auto height = static_cast<vx_uint32>(imgSize.height);
    const auto strideY = static_cast<vx_int32>(width);
    Mat imgCPU(imgSize, CV_8UC1);

    imgRect.end_x = width;
    imgRect.end_y = height;

    vx_imagepatch_addressing_t imgInfo = {width, height, 1, strideY};
    vx_uint8* tmpPtr = imgCPU.data;
    const auto inFmt = VX_MEMORY_TYPE_HOST;
    const auto dataUsage = VX_READ_ONLY;
    RAPIDOPENVX_CHECK(vxCopyImagePatch(inImage, &imgRect, 0u, &imgInfo, tmpPtr, dataUsage, inFmt));

    imwrite(fileName, imgCPU);
  }

  void Stereo::AllocateMemory(const vx_context context, const Size imgSize)
  {
    auto imgWidth = static_cast<vx_uint32>(imgSize.width);
    auto imgHeight = static_cast<vx_uint32>(imgSize.height);
    if (imgSize == m_mSize1)
    {
      m_mImagesLeft.resize(8);
      m_mImagesRight.resize(8);
    }
    else
    {
      m_mImagesLeft.resize(6);
      m_mImagesRight.resize(6);
    }
    const vx_df_image fmtColor = VX_DF_IMAGE_U8;
    m_mImagesLeft[0].Reset(context, imgWidth, imgHeight, fmtColor);
    m_mImagesLeft[1].Reset(context, imgWidth, imgHeight * 10u, fmtColor);
    m_mImagesLeft[2].Reset(context, imgWidth, imgHeight, fmtColor);
    m_mImagesLeft[3].Reset(context, imgWidth, imgHeight, fmtColor);
    m_mImagesLeft[4].Reset(context, imgWidth, imgHeight, fmtColor);
    m_mImagesLeft[5].Reset(context, imgWidth, imgHeight, fmtColor);
    if (imgSize == m_mSize1)
    {
      m_mImagesLeft[6].Reset(context, imgWidth, imgHeight, fmtColor);
      m_mImagesLeft[7].Reset(context, imgWidth, imgHeight, fmtColor);
    }

    m_mImagesRight[0].Reset(context, imgWidth, imgHeight, fmtColor);
    m_mImagesRight[1].Reset(context, imgWidth, imgHeight * 10u, fmtColor);
    m_mImagesRight[2].Reset(context, imgWidth, imgHeight, fmtColor);
    m_mImagesRight[3].Reset(context, imgWidth, imgHeight, fmtColor);
    m_mImagesRight[4].Reset(context, imgWidth, imgHeight, fmtColor);
    m_mImagesRight[5].Reset(context, imgWidth, imgHeight, fmtColor);
    if (imgSize == m_mSize1)
    {
      m_mImagesRight[6].Reset(context, imgWidth, imgHeight, fmtColor);
      m_mImagesRight[7].Reset(context, imgWidth, imgHeight, fmtColor);
    }
  }
}
