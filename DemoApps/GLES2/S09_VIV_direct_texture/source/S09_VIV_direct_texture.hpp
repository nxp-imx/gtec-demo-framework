#ifndef GLES2_S09_VIV_DIRECT_TEXTURE_S09_VIV_DIRECT_TEXTURE_HPP
#define GLES2_S09_VIV_DIRECT_TEXTURE_S09_VIV_DIRECT_TEXTURE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Matrix.hpp>
#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>
#include <FslUtil/OpenGLES2/GLTexture.hpp>

namespace Fsl
{
  class S09_VIV_direct_texture : public DemoAppGLES2
  {
    GLES2::GLProgram m_program;

    GLint m_locTransformMat;
    Matrix m_matTransform;
    float m_angle;
    std::vector<uint8_t> m_raw_video;

  public:
    S09_VIV_direct_texture(const DemoAppConfig& config);
    ~S09_VIV_direct_texture();

  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const FrameInfo& frameInfo) override;
    int LoadFrame();
    int Load420Texture(int Width, int Height, int format);

  private:
    void* m_planes[3];
    int m_fileSize;
    int m_frameSize;
    int m_ySize;
    int m_uSize;
    int m_vSize;
    int m_vFrames;
    int m_data_index;
  };
}

#endif
