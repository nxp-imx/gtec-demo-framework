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

// OpenVX 1.1 project
#include "SoftISP.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <RapidOpenVX/Check.hpp>
#include "OptionParser.hpp"

namespace Fsl
{
  using namespace RapidOpenVX;

  namespace
  {
    void ConvertToRGBA(UserNodes& nodes, const Context& context, const Image& inImage, Image& outImage)
    {
      Graph graph;
      graph.Reset(context.Get());
      RAPIDOPENVX_CHECK(nodes.nxpDebayerNode(graph.Get(), inImage.Get(), outImage.Get()));
      RAPIDOPENVX_CHECK(vxVerifyGraph(graph.Get()));
      RAPIDOPENVX_CHECK(vxProcessGraph(graph.Get()));
    }
  }

  SoftISP::SoftISP(const DemoAppConfig& config)
    : DemoAppOpenVX(config)
    , m_context(ResetMode::Create)
    , m_save(true)
  {
    auto optionParser = config.GetOptions<OptionParser>();
    m_denoiseStatus = optionParser->GetDenoiseStatus();
    FSLLOG3_INFO("Denoise status: {}", m_denoiseStatus);
  }


  SoftISP::~SoftISP()
  {
  }


  void SoftISP::Run()
  {
    UserNodes nodes(GetContentManager());
    nodes.m_denoiseEn = m_denoiseStatus;
    vx_rectangle_t rect = {0, 0, 0, 0};
    vx_map_id map_id;
    vx_imagepatch_addressing_t imgInfo = VX_IMAGEPATCH_ADDR_INIT;
    void* imgBaseAddr = nullptr;
    vx_uint32* distBaseAddr = nullptr;

    rect.end_x = m_imgWid;
    rect.end_y = m_imgHei;
    RAPIDOPENVX_CHECK(nodes.vxPublishNodeKernels(m_context.Get()));
    AllocateMemory(m_context.Get());

    m_graph.Reset(m_context.Get());
    RAPIDOPENVX_CHECK(vxMapImagePatch(m_imagesObj[0].Get(), &rect, 0, &map_id, &imgInfo, &imgBaseAddr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0));
    GetContentManager()->ReadAllBytes(imgBaseAddr, m_imgSize, "bayer.data");
    RAPIDOPENVX_CHECK(vxUnmapImagePatch(m_imagesObj[0].Get(), map_id));
    imgBaseAddr = nullptr;

    for (std::size_t j = 0; j < 6; j++)
    {
      vxMapDistribution(m_dist[j].Get(), &map_id, (void**)&distBaseAddr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);
      for (std::size_t i = 0; i < 256; i++)
      {
        *(distBaseAddr + i) = 0;
      }
      vxUnmapDistribution(m_dist[j].Get(), map_id);
      distBaseAddr = nullptr;
    }
    for (std::size_t j = 6; j < 8; j++)
    {
      vxMapDistribution(m_dist[j].Get(), &map_id, (void**)&distBaseAddr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);
      for (std::size_t i = 0; i < 2; i++)
      {
        *(distBaseAddr + i) = 0;
      }
      vxUnmapDistribution(m_dist[j].Get(), map_id);
      distBaseAddr = nullptr;
    }
    RAPIDOPENVX_CHECK(vxDirective((vx_reference)m_context.Get(), VX_DIRECTIVE_ENABLE_PERFORMANCE));

    RAPIDOPENVX_CHECK(nodes.nxpBadPixelDetectNode(m_graph.Get(), m_imagesObj[0].Get(), m_imagesObj[1].Get()));
    RAPIDOPENVX_CHECK(nodes.nxpHistogramNode(m_graph.Get(), m_imagesObj[1].Get(), m_dist[0].Get(), m_dist[1].Get(), m_dist[2].Get()));
    RAPIDOPENVX_CHECK(nodes.nxpEqualizeHistStep1Node(m_graph.Get(), m_dist[0].Get(), m_dist[1].Get(), m_dist[2].Get(), m_dist[3].Get(),
                                                     m_dist[4].Get(), m_dist[5].Get(), m_dist[6].Get(), m_dist[7].Get()));
    RAPIDOPENVX_CHECK(nodes.nxpWhiteBalanceNode(m_graph.Get(), m_imagesObj[1].Get(), m_dist[6].Get(), m_dist[7].Get(), m_imagesObj[2].Get()));
    RAPIDOPENVX_CHECK(
      nodes.nxpEqualizeHistNode(m_graph.Get(), m_imagesObj[1].Get(), m_dist[3].Get(), m_dist[4].Get(), m_dist[5].Get(), m_imagesObj[3].Get()));
    RAPIDOPENVX_CHECK(nodes.nxpDebayerNode(m_graph.Get(), m_imagesObj[3].Get(), m_imagesObj[4].Get()));
    if (nodes.m_denoiseEn)
    {
      RAPIDOPENVX_CHECK(nodes.nxpRGBA2YUVNode(m_graph.Get(), m_imagesObj[4].Get(), m_imagesObj[5].Get(), m_imagesObj[6].Get()));
      RAPIDOPENVX_CHECK(nodes.nxpBilateralNode(m_graph.Get(), m_imagesObj[6].Get(), m_imagesObj[7].Get()));
      RAPIDOPENVX_CHECK(nodes.nxpYUV2RGBANode(m_graph.Get(), m_imagesObj[5].Get(), m_imagesObj[7].Get(), m_imagesObj[8].Get()));
    }

    RAPIDOPENVX_CHECK(vxVerifyGraph(m_graph.Get()));
    RAPIDOPENVX_CHECK(vxProcessGraph(m_graph.Get()));

    vx_perf_t perf;
    vxQueryGraph(m_graph.Get(), VX_GRAPH_PERFORMANCE, &perf, sizeof(perf));
    FSLLOG3_INFO("Graph execution time on GPU(default pipeline): {}ms", perf.avg / 1000000.0);

    if (m_save)
    {
      Bitmap bitmap;
      FSLLOG3_INFO("Saving images...");
      ConvertToRGBA(nodes, m_context, m_imagesObj[0], m_imagesObj[4]);
      const char* fileName = "0-SourceImage.bmp";
      CopyToBMP(bitmap, fileName, m_imagesObj[4]);

      ConvertToRGBA(nodes, m_context, m_imagesObj[1], m_imagesObj[4]);
      fileName = "1-RemoveBadPixel.bmp";
      CopyToBMP(bitmap, fileName, m_imagesObj[4]);

      ConvertToRGBA(nodes, m_context, m_imagesObj[2], m_imagesObj[4]);
      fileName = "2-WhiteBalance.bmp";
      CopyToBMP(bitmap, fileName, m_imagesObj[4]);

      ConvertToRGBA(nodes, m_context, m_imagesObj[3], m_imagesObj[4]);
      fileName = "3-Equalization.bmp";
      CopyToBMP(bitmap, fileName, m_imagesObj[4]);
      if (nodes.m_denoiseEn)
      {
        fileName = "4-RemoveNoise.bmp";
        CopyToBMP(bitmap, fileName, m_imagesObj[8]);
      }
    }
  }


  void SoftISP::AllocateMemory(const vx_context context)
  {
    m_imagesObj.resize(9);
    m_imagesObj[0].Reset(context, m_imgWid, m_imgHei, VX_DF_IMAGE_U8);
    m_imagesObj[1].Reset(context, m_imgWid, m_imgHei, VX_DF_IMAGE_U8);
    m_imagesObj[2].Reset(context, m_imgWid, m_imgHei, VX_DF_IMAGE_U8);
    m_imagesObj[3].Reset(context, m_imgWid, m_imgHei, VX_DF_IMAGE_U8);
    m_imagesObj[4].Reset(context, m_imgWid * 4, m_imgHei, VX_DF_IMAGE_U8);
    m_imagesObj[5].Reset(context, m_imgWid * 3, m_imgHei, VX_DF_IMAGE_U8);
    m_imagesObj[6].Reset(context, m_imgWid, m_imgHei, VX_DF_IMAGE_U8);
    m_imagesObj[7].Reset(context, m_imgWid, m_imgHei, VX_DF_IMAGE_U8);
    m_imagesObj[8].Reset(context, m_imgWid * 4, m_imgHei, VX_DF_IMAGE_U8);

    vx_uint32 range = 256;
    vx_uint32 numBins = 256;
    m_dist.resize(8);
    for (std::size_t i = 0; i < 6; i++)
    {
      m_dist[i].Reset(context, numBins, 0, range);
    }
    m_dist[6].Reset(context, 2, 0, range);
    m_dist[7].Reset(context, 2, 0, range);
  }


  void SoftISP::CopyToBMP(Bitmap& bitmap, const IO::Path& fileName, const Image& image)
  {
    vx_map_id map_id;
    vx_imagepatch_addressing_t imgInfo = VX_IMAGEPATCH_ADDR_INIT;
    void* imgBaseAddr = nullptr;
    const std::size_t imgWid = 1920;
    const std::size_t imgHei = 1080;
    vx_rectangle_t rect = {0, 0, 0, 0};
    rect.end_x = imgWid * 4;
    rect.end_y = imgHei;

    RAPIDOPENVX_CHECK(vxMapImagePatch(image.Get(), &rect, 0, &map_id, &imgInfo, &imgBaseAddr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0));
    bitmap.Reset(imgBaseAddr, imgHei * imgWid * 4, PxExtent2D::Create(static_cast<int32_t>(imgWid), static_cast<int32_t>(imgHei)),
                 PixelFormat::B8G8R8A8_UNORM, BitmapOrigin::UpperLeft);
    GetPersistentDataManager()->Write(fileName, bitmap);
    RAPIDOPENVX_CHECK(vxUnmapImagePatch(image.Get(), map_id));
    imgBaseAddr = nullptr;
  }
}
