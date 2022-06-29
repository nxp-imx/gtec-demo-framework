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

// Included first since OpenCV currently has a conflict with the X11 header, so this is a workaround for that
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
// Normal includes that dont conflict
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <cmath>
#include <iostream>
#include "OpenCV101.hpp"

namespace Fsl
{
  using namespace Vulkan;
  using namespace cv;


  OpenCV101::OpenCV101(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_batch(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
  {
    double alpha = 0.5;
    double beta = 0.0;
    double input = 0.0;

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


  void OpenCV101::Update(const DemoTime& /*demoTime*/)
  {
    // This call is only necessary if you utilize the highgui components.
    // It allow OpenCV to process its queue. Unfortunately its the only way and can cause a 1ms delay.
    waitKey(1);
  }


  void OpenCV101::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      VkClearColorValue clearColorValue{};
      clearColorValue.float32[0] = 0.5f;
      clearColorValue.float32[1] = 0.5f;
      clearColorValue.float32[2] = 0.5f;
      clearColorValue.float32[3] = 1.0f;

      VkClearValue clearValues = {clearColorValue};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = 1;
      renderPassBeginInfo.pClearValues = &clearValues;

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass OpenCV101::OnBuildResources(const VulkanBasic::BuildResourcesContext& /*context*/)
  {
    // FIX: Replace this with a renderPass that matches your requirements
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void OpenCV101::OnFreeResources()
  {
    m_dependentResources = {};
  }
}
