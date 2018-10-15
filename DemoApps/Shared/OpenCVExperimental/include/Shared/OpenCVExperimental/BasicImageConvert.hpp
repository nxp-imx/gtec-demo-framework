#ifndef SHARED_OPENCVEXPERIMENTAL_BASICIMAGECONVERT_HPP
#define SHARED_OPENCVEXPERIMENTAL_BASICIMAGECONVERT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>

namespace cv
{
  // Forward declare this here as the OpenCV header might include headers that gives problems under X11.
  class Mat;
}

namespace Fsl
{
  // Highly experimental class to demonstrate basic conversion principles
  class BasicImageConvert
  {
  public:
    //! @brief Convert a Mat to a bitmap
    //! @note  This might be slow and the currently supported formats are limited
    //!        WARNING: this assumes that the cv::Mat is in the format PixelFormat::B8G8R8_UINT and it is marked as isContinuous
    //         WARNING: Real production code needs to at least verify that the src cv::Mat format is correct.
    Bitmap Convert(const cv::Mat& src, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredBitmapOrigin)
    {
      Bitmap bitmap;
      Convert(bitmap, src, desiredPixelFormat, desiredBitmapOrigin);
      return bitmap;
    }

    //! @brief Convert a Mat to a bitmap
    //! @note  This might be slow and the currently supported formats are limited
    //!        WARNING: this assumes that the cv::Mat is in the format PixelFormat::B8G8R8_UINT and it is marked as isContinuous
    //         WARNING: Real production code needs to at least verify that the src cv::Mat format is correct.
    void Convert(Bitmap& rDst, const cv::Mat& src, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredBitmapOrigin);

    //! @brief Convert a Bitmap to a mat
    //! @note  This might be slow and the currently supported formats are limited
    void Convert(cv::Mat& rDst, const Bitmap& src, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredBitmapOrigin);
  };
}

#endif
