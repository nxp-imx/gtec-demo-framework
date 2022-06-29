#ifndef GLES3_OPENCVMATTONATIVEBATCH_OPENCVMATTONATIVEBATCH_HPP
#define GLES3_OPENCVMATTONATIVEBATCH_OPENCVMATTONATIVEBATCH_HPP
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

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <Shared/OpenCVExperimental/BasicImageConvert.hpp>

namespace Fsl
{
  class OpenCVMatToNativeBatch : public DemoAppGLES3
  {
    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<INativeBatch2D> m_nativeBatch;
    BasicImageConvert m_convert;
    TextureAtlasSpriteFont m_font;
    Texture2D m_texFont;

    Texture2D m_texTest;
    Texture2D m_texTestR;
    Texture2D m_texTestG;
    Texture2D m_texTestB;
    Texture2D m_texTestMat;
    Texture2D m_texTestMatR;
    Texture2D m_texTestMatG;
    Texture2D m_texTestMatB;

  public:
    explicit OpenCVMatToNativeBatch(const DemoAppConfig& config);
    ~OpenCVMatToNativeBatch() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    //! @return the size of the render images in pixels
    void DrawImages(const Vector2& dstPosition, const Texture2D& tex1, const Texture2D& texR, const Texture2D& texG, const Texture2D& texB,
                    const char* const pszCaption);
    void CreateFont(const std::shared_ptr<IContentManager>& contentManager);
    void CreateReferenceTextures(const std::shared_ptr<IContentManager>& contentManager);
    void CreateOpenCVTextures(const std::shared_ptr<IContentManager>& contentManager);
  };
}

#endif
