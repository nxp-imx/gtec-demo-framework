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
#include "ReferenceOnePassBlurredDraw.hpp"
#include <cassert>

namespace Fsl
{
  using namespace GLES2;


  namespace
  {
    const GLTextureImageParameters g_framebufferImageParams(GL_RGB565, GL_RGB, GL_UNSIGNED_BYTE);

    int UpdateKernelLength(const int32_t kernelLength)
    {
      int32_t moddedKernelLength = std::max(kernelLength, 3);
      int newKernelLength = ((moddedKernelLength / 2) * 2) + 1;
      moddedKernelLength = (newKernelLength < moddedKernelLength ? newKernelLength + 2 : newKernelLength);
      return moddedKernelLength;
    }
  }

  // FIX: the way the setup is done currently we cant change the framebuffer size (since the 'scene' thinks is using the fullscreen)

  ReferenceOnePassBlurredDraw::ReferenceOnePassBlurredDraw(const DemoAppConfig& config, const Config& blurConfig)
    : ABlurredDraw("Reference one pass")
    , m_batch2D(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
    , m_screenResolution(config.ScreenResolution)
    , m_framebufferOrg(config.ScreenResolution, GLTextureParameters(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), g_framebufferImageParams, GL_DEPTH_COMPONENT16)
  {
    if (!m_batch2D)
      throw std::runtime_error("NativeBatch2D unavailable");

    const int moddedKernelLength = UpdateKernelLength(blurConfig.KernelLength);
    FSLLOG_WARNING_IF(moddedKernelLength != blurConfig.KernelLength, "The one pass shader is not compatible with the supplied kernel length of " << blurConfig.KernelLength << " using " << moddedKernelLength);

    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();

    float sigma = blurConfig.Sigma;
    if (blurConfig.UseOptimalSigma)
    {
      sigma = GausianHelper::FindOptimalSigma(moddedKernelLength);
      FSLLOG("Using sigma of " << sigma);
    }
    std::vector<double> kernel;
    GausianHelper::CalculateGausianKernel(kernel, moddedKernelLength, sigma);


    const std::string fragTemplate = contentManager->ReadAllText("GaussianTemplate.frag");
    const std::string gaussianFrag = GausianHelper::GenerateGausianFragmentShader(fragTemplate, kernel, moddedKernelLength, m_framebufferOrg.GetSize());
    const std::string basicShader = contentManager->ReadAllText("BasicShader.vert");
    m_programBlur.Reset(basicShader, gaussianFrag);
    m_programNormal.Reset(basicShader, contentManager->ReadAllText("BasicShader.frag"));
    //m_programBlur.Reset(basicShader, contentManager->ReadAllText("BasicShader.frag"));


    VBHelper::BuildVB(m_vertexBufferLeft, BoxF(-1, -1, 0, 1), BoxF(0.0f, 0.0f, 0.5f, 1.0f));
    VBHelper::BuildVB(m_vertexBufferRight, BoxF(0, -1, 1, 1), BoxF(0.5f, 0.0f, 1.0f, 1.0f));
  }


  void ReferenceOnePassBlurredDraw::Draw(AScene*const pScene)
  {
    assert(pScene != nullptr);

    // Render the 'source' image that we want to partially blur
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferOrg.Get());
    {
      pScene->Draw();
    }
    // Since we are only doing opaque non-overlapping 2d-composition type operations we can disable blend and depth testing
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    // Composite the final image
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    {
      glViewport(0, 0, m_screenResolution.X, m_screenResolution.Y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_framebufferOrg.GetTextureInfo().Handle);

      // Draw the left side using the normal shader
      glUseProgram(m_programNormal.Get());
      glBindBuffer(m_vertexBufferLeft.GetTarget(), m_vertexBufferLeft.Get());
      m_vertexBufferLeft.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      // Draw the right side using the blur shader
      glUseProgram(m_programBlur.Get());
      glBindBuffer(m_vertexBufferRight.GetTarget(), m_vertexBufferRight.Get());
      m_vertexBufferRight.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

}
