#ifndef FSLGRAPHICS_UNITTEST_HELPER_IMAGETESTTEMPLATE_HPP
#define FSLGRAPHICS_UNITTEST_HELPER_IMAGETESTTEMPLATE_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/PixelFormatLayout.hpp>
#include <FslUnitTest/FormatDebugInfo.hpp>
#include <FslUnitTest/TestCustomChecks.hpp>
#include <gtest/gtest.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace Fsl
{
  // TImage Image Container Concept
  //
  // struct ImageContainerUtilConcept
  //{
  //  using image_t = Bitmap;                                           // The image class
  //  using pixel_t = PixelTestUtil::R8G8B8;                            // PixelConcept class
  //  using color_template_t = PixelTestUtil::R8G8B8::ColorTemplate;    // A color template containg: R(), G(), B(), White(), Black()
  //
  //  static image_t MakeImage(const PxExtent2D extent, const PixelFormat pixelFormat, const BitmapOrigin origin);
  //  static const PxExtent2D GetExtent(const image_t& image);
  //  static const pixel_t GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin);
  //  static void SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin);
  //};


  template <typename TImageConfig>
  struct ImageTestTemplate
  {
    using image_t = typename TImageConfig::image_t;

    static uint32_t CalcYPosition(const image_t& image, const uint32_t y, const BitmapOrigin checkOrigin)
    {
      switch (checkOrigin)
      {
      case BitmapOrigin::UpperLeft:
      case BitmapOrigin::Undefined:
        return y;
      case BitmapOrigin::LowerLeft:
        return TImageConfig::GetExtent(image).Height - 1 - y;
      default:
        throw NotSupportedException("Unsupported BitmapOrigin");
      }
    }

    static image_t GetBasic4X1(BitmapOrigin origin = BitmapOrigin::Undefined)
    {
      const bool ignoreOrigin = false;
      image_t image = TImageConfig::MakeImage(PxExtent2D(4, 1), TImageConfig::ActivePixelFormat, origin);
      TImageConfig::SetPixel(image, 0, 0, TImageConfig::color_template_t::R(), ignoreOrigin);
      TImageConfig::SetPixel(image, 1, 0, TImageConfig::color_template_t::G(), ignoreOrigin);
      TImageConfig::SetPixel(image, 2, 0, TImageConfig::color_template_t::B(), ignoreOrigin);
      TImageConfig::SetPixel(image, 3, 0, TImageConfig::color_template_t::White(), ignoreOrigin);
      return image;
    }

    static image_t GetBasic4X2(BitmapOrigin origin = BitmapOrigin::Undefined, const bool ignoreOrigin = false)
    {
      image_t image = TImageConfig::MakeImage(PxExtent2D(4, 2), TImageConfig::ActivePixelFormat, origin);
      TImageConfig::SetPixel(image, 0, 0, TImageConfig::color_template_t::R(), ignoreOrigin);
      TImageConfig::SetPixel(image, 1, 0, TImageConfig::color_template_t::G(), ignoreOrigin);
      TImageConfig::SetPixel(image, 2, 0, TImageConfig::color_template_t::B(), ignoreOrigin);
      TImageConfig::SetPixel(image, 3, 0, TImageConfig::color_template_t::White(), ignoreOrigin);
      TImageConfig::SetPixel(image, 0, 1, TImageConfig::color_template_t::Black(), ignoreOrigin);
      TImageConfig::SetPixel(image, 1, 1, TImageConfig::color_template_t::White(), ignoreOrigin);
      TImageConfig::SetPixel(image, 2, 1, TImageConfig::color_template_t::Black(), ignoreOrigin);
      TImageConfig::SetPixel(image, 3, 1, TImageConfig::color_template_t::White(), ignoreOrigin);
      return image;
    }


    static ::testing::AssertionResult CheckIsBasic4X1(const image_t& image)
    {
      const PxExtent2D imageExtent = TImageConfig::GetExtent(image);
      if (imageExtent.Width != 4 || imageExtent.Height != 1 || image.GetPixelFormat() != TImageConfig::ActivePixelFormat)
      {
        std::string strError = fmt::format("Image.");
        FormatDebugInfoTwoValues<uint32_t>(strError, " Width", imageExtent.Width, 4);
        FormatDebugInfoTwoValues<uint32_t>(strError, " Height", imageExtent.Height, 1);
        FormatDebugInfoTwoValues(strError, " PixelFormat", image.GetPixelFormat(), TImageConfig::ActivePixelFormat);
        return ::testing::AssertionFailure() << strError;
      }

      // TImageConfig::ActivePixelFormat
      const auto pixel0 = TImageConfig::GetPixel(image, 0, 0, false);
      const auto pixel1 = TImageConfig::GetPixel(image, 1, 0, false);
      const auto pixel2 = TImageConfig::GetPixel(image, 2, 0, false);
      const auto pixel3 = TImageConfig::GetPixel(image, 3, 0, false);

      if (pixel0 == TImageConfig::color_template_t::R() && pixel1 == TImageConfig::color_template_t::G() &&
          pixel2 == TImageConfig::color_template_t::B() && pixel3 == TImageConfig::color_template_t::White())
      {
        return ::testing::AssertionSuccess();
      }

      std::string strError = fmt::format("The image did not contain the expected pixels.");
      FormatDebugInfoTwoValues(strError, " Pixel0", pixel0, TImageConfig::color_template_t::R());
      FormatDebugInfoTwoValues(strError, " Pixel1", pixel1, TImageConfig::color_template_t::G());
      FormatDebugInfoTwoValues(strError, " Pixel2", pixel2, TImageConfig::color_template_t::B());
      FormatDebugInfoTwoValues(strError, " Pixel3", pixel3, TImageConfig::color_template_t::White());
      return ::testing::AssertionFailure() << strError;
    }


    static ::testing::AssertionResult CheckIsBasic4X2(const image_t& image, const BitmapOrigin checkOrigin = BitmapOrigin::Undefined)
    {
      const PxExtent2D imageExtent = TImageConfig::GetExtent(image);
      if (imageExtent.Width != 4 || imageExtent.Height != 2 || image.GetPixelFormat() != TImageConfig::ActivePixelFormat)
      {
        std::string strError = fmt::format("image.");
        FormatDebugInfoTwoValues<uint32_t>(strError, " Width", imageExtent.Width, 4);
        FormatDebugInfoTwoValues<uint32_t>(strError, " Height", imageExtent.Height, 2);
        FormatDebugInfoTwoValues(strError, " PixelFormat", image.GetPixelFormat(), TImageConfig::ActivePixelFormat);
        return ::testing::AssertionFailure() << strError;
      }

      const bool ignoreOrigin = checkOrigin != BitmapOrigin::Undefined;
      const auto y0 = CalcYPosition(image, 0, checkOrigin);
      const auto y1 = CalcYPosition(image, 1, checkOrigin);

      // TImageConfig::ActivePixelFormat
      const auto pixel0Y0 = TImageConfig::GetPixel(image, 0, y0, ignoreOrigin);
      const auto pixel1Y0 = TImageConfig::GetPixel(image, 1, y0, ignoreOrigin);
      const auto pixel2Y0 = TImageConfig::GetPixel(image, 2, y0, ignoreOrigin);
      const auto pixel3Y0 = TImageConfig::GetPixel(image, 3, y0, ignoreOrigin);
      const auto pixel0Y1 = TImageConfig::GetPixel(image, 0, y1, ignoreOrigin);
      const auto pixel1Y1 = TImageConfig::GetPixel(image, 1, y1, ignoreOrigin);
      const auto pixel2Y1 = TImageConfig::GetPixel(image, 2, y1, ignoreOrigin);
      const auto pixel3Y1 = TImageConfig::GetPixel(image, 3, y1, ignoreOrigin);

      if (pixel0Y0 == TImageConfig::color_template_t::R() && pixel1Y0 == TImageConfig::color_template_t::G() &&
          pixel2Y0 == TImageConfig::color_template_t::B() && pixel3Y0 == TImageConfig::color_template_t::White() &&
          pixel0Y1 == TImageConfig::color_template_t::Black() && pixel1Y1 == TImageConfig::color_template_t::White() &&
          pixel2Y1 == TImageConfig::color_template_t::Black() && pixel3Y1 == TImageConfig::color_template_t::White())
      {
        return ::testing::AssertionSuccess();
      }

      std::string strError = fmt::format("The image did not contain the expected pixels.");
      FormatDebugInfoTwoValues(strError, " Pixel 0,0", pixel0Y0, TImageConfig::color_template_t::R());
      FormatDebugInfoTwoValues(strError, " Pixel 1,0", pixel1Y0, TImageConfig::color_template_t::G());
      FormatDebugInfoTwoValues(strError, " Pixel 2,0", pixel2Y0, TImageConfig::color_template_t::B());
      FormatDebugInfoTwoValues(strError, " Pixel 3,0", pixel2Y0, TImageConfig::color_template_t::White());
      FormatDebugInfoTwoValues(strError, " Pixel 0,1", pixel0Y1, TImageConfig::color_template_t::Black());
      FormatDebugInfoTwoValues(strError, " Pixel 1,1", pixel1Y1, TImageConfig::color_template_t::White());
      FormatDebugInfoTwoValues(strError, " Pixel 2,1", pixel2Y1, TImageConfig::color_template_t::Black());
      FormatDebugInfoTwoValues(strError, " Pixel 3,1", pixel3Y1, TImageConfig::color_template_t::White());
      return ::testing::AssertionFailure() << strError;
    }
  };
}
#endif
