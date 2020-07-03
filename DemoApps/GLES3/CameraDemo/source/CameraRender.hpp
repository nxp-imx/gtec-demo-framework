#ifndef GLES3_CAMERADEMO_CAMERARENDER_HPP
#define GLES3_CAMERADEMO_CAMERARENDER_HPP
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

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLRawBitmapUtil.hpp>
#include <Shared/Camera/System/Camera.hpp>
#include <Shared/Camera/System/CameraSystem.hpp>
#include <GLES3/gl3.h>
#include <array>
// Includes for glm and matrix manipulation
#include <glm/glm.hpp>

namespace Fsl
{
  class CameraRender
  {
    Helios::CameraSystem m_cameraSystem;
    Helios::Camera m_camera;
    GLES3::GLRawBitmapUtil::Result m_cameraTextureConfig;
    GLES3::GLProgram m_program;
    std::vector<uint8_t> m_bitmapBuffer;
    RawBitmapEx m_targetBitmap;
    GLuint m_textureHandle{};
    uint32_t m_frameId{};

    std::array<GLfloat, 12> m_cameraPlaneVertices{};

    GLuint m_cameraVAO{};

    GLint m_modelMatrixLoc{};
    GLint m_viewMatrixLoc{};
    GLint m_projMatrixLoc{};

    glm::mat4 m_modelMatrix{};
    glm::mat4 m_viewMatrix{};
    glm::mat4 m_projMatrix{};

    GLint m_textureLoc{};

  public:
    CameraRender(const std::shared_ptr<IContentManager>& contentManager, const PxSize2D& currentSizePx);

    void Draw(const DemoTime& demoTime);
  };
}

#endif
