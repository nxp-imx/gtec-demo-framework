/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include "GausianHelper.hpp"
#include "VBHelper.hpp"
#include "TwoPassLinearScaledBlurredDraw.hpp"
#include <FslGraphics/Exceptions.hpp>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace Fsl
{
  using namespace GLES2;

  namespace
  {
    const GLTextureImageParameters g_framebufferImageParams(GL_RGB565, GL_RGB, GL_UNSIGNED_BYTE);

    int UpdateScaledKernelLength(const int32_t kernelLength)
    {
      int32_t moddedKernelLength = std::max(kernelLength, 5);
      int32_t newKernelLength = ((moddedKernelLength / 4) * 4) + 1;
      return (newKernelLength < moddedKernelLength ? newKernelLength + 4 : newKernelLength);
    }

    int UpdateKernelLength(const int32_t kernelLength)
    {
      int32_t moddedKernelLength = UpdateScaledKernelLength(kernelLength / 2);
      moddedKernelLength = (moddedKernelLength * 2) + 1;
      return moddedKernelLength;
    }
  }

  //! Uses the two pass linear technique and further reduces the bandwidth requirement by
  //! downscaling the 'source image' to 1/4 its size (1/2w x 1/2h) before applying the blur and
  //! and then upscaling the blurred image to provide the final image.
  //! This works well for large kernel sizes and relatively high sigma's but the downscaling
  //! produces visible artifacts with low sigma's
  TwoPassLinearScaledBlurredDraw::TwoPassLinearScaledBlurredDraw(const DemoAppConfig& config, const Config& blurConfig)
    : ABlurredDraw("Two pass linear scaled")
    , m_batch2D(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
    , m_screenResolution(config.ScreenResolution)
    , m_framebufferOrg(Point2(m_screenResolution.X, m_screenResolution.Y), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_framebufferImageParams, GL_DEPTH_COMPONENT16)
    , m_framebufferBlur1()
    , m_framebufferBlur2()
    , m_shaders()
  {
    if (!m_batch2D)
      throw std::runtime_error("NativeBatch2D unavailable");

    // Ensure that the kernel size is correct
    int moddedKernelLength = UpdateKernelLength(blurConfig.KernelLength);
    FSLLOG_WARNING_IF(moddedKernelLength != blurConfig.KernelLength, "The two pass linear scaled shader is not compatible with the supplied kernel length of " << blurConfig.KernelLength << " using " << moddedKernelLength);

    // Simplistic scaling of the kernel so it handles the down-sampling of the image correctly
    moddedKernelLength = UpdateScaledKernelLength(moddedKernelLength / 2);
    float moddedSigma = blurConfig.Sigma / 2.0f;
    FSLLOG("Scaled actual kernel length: " << moddedKernelLength << " which becomes a " << ((moddedKernelLength / 2)+1) << " linear kernel" );

    // The downscaled framebuffer needs to contain 'half of the kernel width' extra pixels on the left to ensure that we can use
    // them for the blur calc of the first pixel we are interested in
    const int quadWidth = m_screenResolution.X / 4;
    int blurFBWidth = quadWidth + (moddedKernelLength / 2);
    blurFBWidth += ((blurFBWidth % 16) != 0 ? (16 - (blurFBWidth % 16)) : 0);
    int addedPixels = blurFBWidth - quadWidth;

    m_framebufferBlur1.Reset(Point2(blurFBWidth, m_screenResolution.Y / 2), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_framebufferImageParams);
    m_framebufferBlur2.Reset(Point2(blurFBWidth, m_screenResolution.Y / 2), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_framebufferImageParams);


    // Prepare the shaders
    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();
    moddedSigma = blurConfig.UseOptimalSigma ? -1.0f : moddedSigma;
    m_shaders.Reset(contentManager, moddedKernelLength, moddedSigma, m_framebufferBlur1.GetSize(), m_framebufferBlur2.GetSize(),
                    TwoPassShaders::Linear, blurConfig.TheShaderType);

    const float xOrg = ((m_framebufferOrg.GetSize().X - ((m_framebufferOrg.GetSize().X / 2) + addedPixels * 2)) / float(m_framebufferOrg.GetSize().X));
    const float xFinal = addedPixels / float(m_framebufferBlur2.GetSize().X);

    VBHelper::BuildVB(m_vertexBufferLeft, BoxF(-1, -1, 0, 1), BoxF(0.0f, 0.0f, 0.5f, 1.0f));
    VBHelper::BuildVB(m_vertexBufferRightX, BoxF(-1, -1, 1, 1), BoxF(xOrg, 0.0f, 1.0f, 1.0f));
    VBHelper::BuildVB(m_vertexBufferRightX2, BoxF(-1, -1, 1, 1), BoxF(0.0f, 0.0f, 1.0f, 1.0f));

    VBHelper::BuildVB(m_vertexBufferRightY, BoxF(0, -1, 1, 1), BoxF(xFinal, 0.0f, 1.0f, 1.0f));
  }



  void TwoPassLinearScaledBlurredDraw::Draw(AScene*const pScene)
  {
    assert(pScene != nullptr);

    // Render the 'source' image that we want to partially blur
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferOrg.Get());
    {
      glViewport(0, 0, m_framebufferOrg.GetSize().X, m_framebufferOrg.GetSize().Y);
      pScene->Draw();
    }
    // Since we are only doing opaque 2d-composition type operations we can disable blend and depth testing
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    // Downscale the right side of the image to 1/4 of its size (1/2w x 1/2h)
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferBlur1.Get());
    {
      glViewport(0, 0, m_framebufferBlur1.GetSize().X, m_framebufferBlur1.GetSize().Y);
      glClear(GL_COLOR_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_framebufferOrg.GetTextureInfo().Handle);

      glUseProgram(m_shaders.ProgramNormal.Get());
      glBindBuffer(m_vertexBufferRightX.GetTarget(), m_vertexBufferRightX.Get());
      m_vertexBufferRightX.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Blur the scaled image in X
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferBlur2.Get());
    {
      glViewport(0, 0, m_framebufferBlur2.GetSize().X, m_framebufferBlur2.GetSize().Y);
      glClear(GL_COLOR_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_framebufferBlur1.GetTextureInfo().Handle);

      glUseProgram(m_shaders.ProgramBlurX.Get());
      glBindBuffer(m_vertexBufferRightX2.GetTarget(), m_vertexBufferRightX2.Get());
      m_vertexBufferRightX2.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Blur the scaled image in Y
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferBlur1.Get());
    {
      glViewport(0, 0, m_framebufferBlur1.GetSize().X, m_framebufferBlur1.GetSize().Y);
      glClear(GL_COLOR_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_framebufferBlur2.GetTextureInfo().Handle);

      glUseProgram(m_shaders.ProgramBlurY.Get());
      glBindBuffer(m_vertexBufferRightX2.GetTarget(), m_vertexBufferRightX2.Get());
      m_vertexBufferRightX2.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Since we are only doing opaque non-overlapping 2d-composition type operations we can disable blend and depth testing
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    {
      glViewport(0, 0, m_screenResolution.X, m_screenResolution.Y);
      glClear(GL_COLOR_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_framebufferOrg.GetTextureInfo().Handle);

      // Draw the left side using the normal shader
      glUseProgram(m_shaders.ProgramNormal.Get());
      glBindBuffer(m_vertexBufferLeft.GetTarget(), m_vertexBufferLeft.Get());
      m_vertexBufferLeft.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      // Draw the right side using a normal shader, scaling the blurred image back to normal size
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_framebufferBlur1.GetTextureInfo().Handle);
      glUseProgram(m_shaders.ProgramNormal.Get());
      glBindBuffer(m_vertexBufferRightY.GetTarget(), m_vertexBufferRightY.Get());
      m_vertexBufferRightY.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

}
