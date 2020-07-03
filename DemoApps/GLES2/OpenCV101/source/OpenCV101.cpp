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

// Included first since OpenCV currently has a conflict with the X11 header, so this is a workaround for that
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
// Normal includes that dont conflict
#include "OpenCV101.hpp"
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>
#include <iostream>

namespace Fsl
{
  using namespace GLES2;
  using namespace cv;


  OpenCV101::OpenCV101(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_batch(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
  {
    double alpha = 0.5;
    double beta = 0.0f;
    double input = 0.0f;

    Mat src1;
    Mat src2;
    Mat dst;

    /// Ask the user enter alpha
    std::cout << " Simple Linear Blender " << std::endl;
    std::cout << "-----------------------" << std::endl;
    std::cout << "* Enter alpha [0-1]: ";
    // std::cin >> input;
    input = 1;

    /// We use the alpha provided by the user if it is between 0 and 1
    if (input >= 0.0 && input <= 1.0)
    {
      alpha = input;
    }

    auto contentManager = GetContentManager();
    IO::Path contentPath = contentManager->GetContentPath();

    IO::Path pathImg1 = IO::Path::Combine(contentPath, "Image1.png");
    IO::Path pathImg2 = IO::Path::Combine(contentPath, "Image2.png");


    /// Read image ( same size, same type )
    src1 = imread(pathImg1.ToUTF8String());
    src2 = imread(pathImg2.ToUTF8String());

    if (src1.data == nullptr)
    {
      throw GraphicsException("Error loading src1");
    }
    if (src2.data == nullptr)
    {
      throw GraphicsException("Error loading src2");
    }

    // Create Windows
    namedWindow("Linear Blend", 1);

    beta = (1.0 - alpha);
    addWeighted(src1, alpha, src2, beta, 0.0, dst);

    imshow("Linear Blend", dst);
  }


  OpenCV101::~OpenCV101() = default;


  void OpenCV101::Update(const DemoTime& /*demoTime*/)
  {
    // This call is only necessary if you utilize the highgui components.
    // It allow OpenCV to process its queue. Unfortunately its the only way and can cause a 1ms delay.
    waitKey(1);
  }


  void OpenCV101::Draw(const DemoTime& /*demoTime*/)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}
