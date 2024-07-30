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

// OpenCV 4 App

#include "OpenCV102.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

namespace Fsl
{
  using namespace cv;
  namespace
  {
    namespace LocalConfig
    {
      constexpr auto WindowName = "Edge map";
    }

    Mat g_image, g_gray, g_edge, g_cedge;
    // define a trackbar callback
    void OnTrackbar(int edgeThresh, void* /*data*/)
    {
      blur(g_gray, g_edge, Size(3, 3));
      // Run the edge detector on grayscale
      Canny(g_edge, g_edge, edgeThresh, edgeThresh * 3, 3);
      g_cedge = Scalar::all(0);
      g_image.copyTo(g_cedge, g_edge);
      imshow(LocalConfig::WindowName, g_cedge);
    }
  }


  OpenCV102::OpenCV102(const DemoAppConfig& config)
    : DemoAppConsole(config)
  {
  }

  OpenCV102::~OpenCV102() = default;


  void OpenCV102::Run()
  {
    int edgeThresh = 1;
    auto contentManager = GetContentManager();
    IO::Path contentPath = contentManager->GetContentPath();

    IO::Path pathImg1 = IO::Path::Combine(contentPath, "Image.png");

    /// Read image ( same size, same type )
    g_image = imread(pathImg1.ToUTF8String());

    if (g_image.data == nullptr)
    {
      throw GraphicsException("Error loading src1");
    }

    g_cedge.create(g_image.size(), g_image.type());
    cvtColor(g_image, g_gray, COLOR_BGR2GRAY);
    // Create a window
    namedWindow(LocalConfig::WindowName, 1);

    // create a toolbar
    createTrackbar("Canny threshold:", LocalConfig::WindowName, nullptr, 100, OnTrackbar);
    setTrackbarPos("Canny threshold:", LocalConfig::WindowName, edgeThresh);

    // Show the image
    OnTrackbar(edgeThresh, nullptr);
    // Wait for a key stroke;
    waitKey(0);
  }
}
