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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <RapidOpenVX/Check.hpp>
#include <RapidOpenVX/Exceptions.hpp>
#include <RapidOpenVX/Image.hpp>
#include "VxTutorial1.hpp"
#include <VX/vx.h>
#include <VX/vxu.h>
#include <cassert>
#include <cmath>

namespace Fsl
{
  using namespace RapidOpenVX;

  namespace
  {
    void CopyImageFromCPUToGPU(Image& rImage, const Bitmap& srcBitmap)
    {
      assert(srcBitmap.GetPixelFormat() == PixelFormat::EX_ALPHA8_UNORM);

      const uint32_t imageWidth = static_cast<uint32_t>(srcBitmap.Width());
      const uint32_t imageHeight = static_cast<uint32_t>(srcBitmap.Height());

      vx_rectangle_t imageRect = { 0, 0, imageWidth, imageHeight };
      vx_imagepatch_addressing_t imageInfo = VX_IMAGEPATCH_ADDR_INIT;
      void* pImageAddress = nullptr;
      RAPIDOPENVX_CHECK(vxAccessImagePatch(rImage.Get(), &imageRect, 0, &imageInfo, &pImageAddress, VX_WRITE_ONLY));

      if ((1 != imageInfo.step_y) || (1 != imageInfo.step_x) || (imageHeight != imageInfo.dim_y) || (imageWidth != imageInfo.dim_x))
        throw std::runtime_error("vx procedure error");

      {
        RawBitmap rawSrcBitmap;
        Bitmap::ScopedDirectAccess(srcBitmap, rawSrcBitmap);
        const uint8_t* pSrcBitmap = static_cast<const uint8_t*>(rawSrcBitmap.Content());
        const auto srcStride = rawSrcBitmap.Stride();

        for (uint32_t y = 0; y < imageHeight; ++y)
        {
          for (uint32_t x = 0; x < imageWidth; ++x)
          {
            uint8_t* pImagePixels = static_cast<uint8_t*>(vxFormatImagePatchAddress2d(pImageAddress, x, y, &imageInfo));
            assert(pImagePixels != nullptr);
            *pImagePixels = pSrcBitmap[(y * srcStride) + x];
          }
        }
      }

      RAPIDOPENVX_CHECK(vxCommitImagePatch(rImage.Get(), &imageRect, 0, &imageInfo, pImageAddress));
    }


    void CopyImagesFromGPUToCPU(Bitmap& dstBitmap, const Image& srcImage1, const Image& srcImage2)
    {
      assert(dstBitmap.GetPixelFormat() == PixelFormat::EX_ALPHA8_UNORM);

      const uint32_t imageWidth = static_cast<uint32_t>(dstBitmap.Width());
      const uint32_t imageHeight = static_cast<uint32_t>(dstBitmap.Height());
      vx_rectangle_t imageRect = { 0, 0, imageWidth, imageHeight };

      // transfer image from gpu to cpu
      vx_imagepatch_addressing_t imageInfo1 = VX_IMAGEPATCH_ADDR_INIT;
      vx_imagepatch_addressing_t imageInfo2 = VX_IMAGEPATCH_ADDR_INIT;
      void* pImageAddress1 = nullptr;
      void* pImageAddress2 = nullptr;
      RAPIDOPENVX_CHECK(vxAccessImagePatch(srcImage1.Get(), &imageRect, 0, &imageInfo1, &pImageAddress1, VX_READ_ONLY));
      RAPIDOPENVX_CHECK(vxAccessImagePatch(srcImage2.Get(), &imageRect, 0, &imageInfo2, &pImageAddress2, VX_READ_ONLY));

      if ((imageInfo2.dim_y != imageInfo1.dim_y) || (imageInfo2.dim_x != imageInfo1.dim_x) ||
          (imageInfo2.step_y != imageInfo1.step_y) || (imageInfo2.step_x != imageInfo1.step_x) ||
          (imageInfo2.dim_y != imageHeight) || (imageInfo2.dim_x != imageWidth) ||
          (imageInfo2.step_y != 1) || (imageInfo2.step_x != 1))
      {
        throw std::runtime_error("vx procedure error");
      }

      RawBitmapEx rawDstBitmap;
      Bitmap::ScopedDirectAccess(dstBitmap, rawDstBitmap);
      uint8_t* pDstBitmap = static_cast<uint8_t*>(rawDstBitmap.Content());
      const auto dstStride = rawDstBitmap.Stride();

      for (uint32_t y = 0; y < imageHeight; ++y)
      {
        for (uint32_t x = 0; x < imageWidth; ++x)
        {
          const int16_t* pImagePixel1 = static_cast<const int16_t*>(vxFormatImagePatchAddress2d(pImageAddress1, x, y, &imageInfo1));
          const int16_t* pImagePixel2 = static_cast<const int16_t*>(vxFormatImagePatchAddress2d(pImageAddress2, x, y, &imageInfo2));

          const float value = static_cast<float>(*pImagePixel1) * static_cast<float>(*pImagePixel1) + static_cast<float>(*pImagePixel2) * static_cast<float>(*pImagePixel2);
          uint16_t dstPixel = static_cast<uint16_t>(std::sqrt(value));
          if (dstPixel > 0xff)
            dstPixel = 0xff;

          pDstBitmap[(y*dstStride) + x] = static_cast<uint8_t>(dstPixel);
        }
      }

      RAPIDOPENVX_CHECK(vxCommitImagePatch(srcImage1.Get(), nullptr, 0, &imageInfo1, pImageAddress1));
      RAPIDOPENVX_CHECK(vxCommitImagePatch(srcImage2.Get(), nullptr, 0, &imageInfo2, pImageAddress2));
    }
  }


  VxTutorial1::VxTutorial1(const DemoAppConfig& config)
    : DemoAppOpenVX(config)
    , m_context(ResetMode::Create)
  {
  }


  VxTutorial1::~VxTutorial1()
  {
  }


  void VxTutorial1::Run()
  {
    // Read the image data
    Bitmap bitmap;
    GetContentManager()->Read(bitmap, "Test_gray.png", PixelFormat::EX_ALPHA8_UNORM);

    const uint32_t imageWidth = static_cast<uint32_t>(bitmap.Width());
    const uint32_t imageHeight = static_cast<uint32_t>(bitmap.Height());

    Image image0(m_context.Get(), imageWidth, imageHeight, VX_DF_IMAGE_U8);
    Image image1(m_context.Get(), imageWidth, imageHeight, VX_DF_IMAGE_S16);
    Image image2(m_context.Get(), imageWidth, imageHeight, VX_DF_IMAGE_S16);

    CopyImageFromCPUToGPU(image0, bitmap);

    // process image
    RAPIDOPENVX_CHECK(vxuSobel3x3(m_context.Get(), image0.Get(), image1.Get(), image2.Get()));

    CopyImagesFromGPUToCPU(bitmap, image1, image2);

    // Save result to image
    GetPersistentDataManager()->Write("Test_sobel.bmp", bitmap);

    FSLLOG("vx process success.");
  }
}
