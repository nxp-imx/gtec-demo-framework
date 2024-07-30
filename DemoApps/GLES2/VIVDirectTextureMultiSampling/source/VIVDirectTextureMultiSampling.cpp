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
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif

#include "VIVDirectTextureMultiSampling.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>

namespace Fsl
{
  namespace
  {
    const GLfloat g_vertices[][2] = {{-0.5f, -0.5f}, {0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, 0.5f}};

    const GLfloat g_texcoords[][2] = {{0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}};

    GLfloat g_transformMatrix[16] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    constexpr std::array<GLES2::GLBindAttribLocation, 2> g_shaderAttributeArray = {GLES2::GLBindAttribLocation(0, "vPosition"),
                                                                                   GLES2::GLBindAttribLocation(1, "my_Texcoor")};

    constexpr int WIDTH = 160;
    constexpr int HEIGHT = 120;
  }


  VIVDirectTextureMultiSampling::VIVDirectTextureMultiSampling(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_program()
    , m_y_texture()
    , m_uv_texture()
    , m_yTex(0)
    , m_uvTex(0)
    , m_locVertices(0)
    , m_locTexCoord(0)
    , m_locTransformMat(0)
    , m_raw_video()
    , m_fileSize(0)
    , m_frameSize(0)
    , m_ySize(0)
    , m_uSize(0)
    , m_vSize(0)
    , m_vFrames(0)
    , m_data_index(0)
    , m_matTransform()
    , m_angle(0.0)
  {
    GLuint gTexObj = 0;
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"), SpanUtil::AsReadOnlySpan(g_shaderAttributeArray));
    const GLuint hProgram = m_program.Get();
    // Grab location of shader attributes.
    GL_CHECK(m_locVertices = glGetAttribLocation(hProgram, "my_Vertex"));
    GL_CHECK(m_locTexCoord = glGetAttribLocation(hProgram, "my_Texcoor"));
    // Transform Matrix is uniform for all vertices here.
    GL_CHECK(m_locTransformMat = glGetUniformLocation(hProgram, "my_TransMatrix"));
    GL_CHECK(glUseProgram(hProgram));

    m_locSampler[0] = glGetUniformLocation(hProgram, "my_Texture0");
    m_locSampler[1] = glGetUniformLocation(hProgram, "my_Texture1");

    GetContentManager()->ReadAllBytes(m_raw_video, "f430_160x120xNV21.yuv");
    m_fileSize = m_raw_video.size();
    gTexObj = Load420Texture(WIDTH, HEIGHT, GL_VIV_NV21);

    if (gTexObj == 0)
    {
      throw GraphicsException("420 video texture not loaded");
    }

    GL_CHECK(glEnableVertexAttribArray(m_locVertices));
    GL_CHECK(glEnableVertexAttribArray(m_locTexCoord));

    // set data in the arrays
    GL_CHECK(glVertexAttribPointer(m_locVertices, 2, GL_FLOAT, GL_FALSE, 0, &g_vertices[0][0]));
    GL_CHECK(glVertexAttribPointer(m_locTexCoord, 2, GL_FLOAT, GL_FALSE, 0, &g_texcoords[0][0]));
    GL_CHECK(glUniformMatrix4fv(m_locTransformMat, 1, GL_FALSE, g_transformMatrix));
  }

  VIVDirectTextureMultiSampling::~VIVDirectTextureMultiSampling()
  {
    glDisableVertexAttribArray(m_locVertices);
    glDisableVertexAttribArray(m_locTexCoord);
  }


  void VIVDirectTextureMultiSampling::Update(const DemoTime& demoTime)
  {
    m_matTransform = Matrix::CreateRotationY(m_angle);
    m_angle += 0.1f;
  }


  void VIVDirectTextureMultiSampling::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());

    // Clear background.
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUniformMatrix4fv(m_locTransformMat, 1, GL_FALSE, m_matTransform.DirectAccess());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    if (m_vFrames > 1)
      LoadFrame();
  }

  void VIVDirectTextureMultiSampling::LoadFrame()
  {
    uint8_t* videoData = m_raw_video.data() + m_data_index;

    if ((m_fileSize - m_data_index) < m_frameSize)
      m_data_index = 0;

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(m_locSampler[0], 0);
    glBindTexture(GL_TEXTURE_2D, m_yTex);
    std::memcpy(m_planes[0], videoData, m_ySize);
    // std::copy(videoData, videoData+m_ySize, m_planes[0]);
    GL_CHECK(glTexDirectVIV(GL_TEXTURE_2D, WIDTH, HEIGHT, GL_ALPHA, (GLvoid**)&m_planes[0]));
    glTexDirectInvalidateVIV(GL_TEXTURE_2D);
    videoData += m_ySize;

    if (m_vSize > 0)
    {
      glActiveTexture(GL_TEXTURE1);
      glUniform1i(m_locSampler[1], 1);
      glBindTexture(GL_TEXTURE_2D, m_uvTex);
      std::memcpy(m_planes[1], videoData, m_vSize);
      // std::copy(videoData, videoData+m_vSize, m_planes[1]);
      GL_CHECK(glTexDirectVIV(GL_TEXTURE_2D, WIDTH, HEIGHT, GL_LUMINANCE8_ALPHA8_EXT, (GLvoid**)&m_planes[1]));
      videoData += m_vSize;
      glTexDirectInvalidateVIV(GL_TEXTURE_2D);
      ;
    }

    m_data_index += m_frameSize;
    if ((m_fileSize - m_data_index) < m_frameSize)
      m_data_index = 0;
    GL_CHECK_FOR_ERROR();
  }


  int VIVDirectTextureMultiSampling::Load420Texture(int texWidth, int texHeight, int format)
  {
    uint8_t* videoData = m_raw_video.data() + m_data_index;
    GLuint result = 0;

    switch (format)
    {
    case GL_VIV_YV12:
      m_ySize = texWidth * texHeight;
      m_vSize = m_ySize / 4;
      m_uSize = m_vSize;
      break;

    case GL_VIV_NV12:
    case GL_VIV_NV21:
      m_ySize = texWidth * texHeight;
      m_vSize = m_ySize / 2;
      m_uSize = 0;
      break;

    case GL_VIV_YUY2:
    case GL_VIV_UYVY:
      m_ySize = 2 * texWidth * texHeight;
      m_vSize = 0;
      m_uSize = 0;
      break;

    case GL_VIV_I420:
      m_ySize = texWidth * texHeight;
      m_uSize = m_ySize / 4;
      m_vSize = m_vSize;
      break;
      return 0;
    }

    m_frameSize = m_ySize + m_uSize + m_vSize;
    m_vFrames = m_fileSize / m_frameSize;
    // Determine the number of frames in the file.
    if ((m_fileSize <= 0) || (m_frameSize <= 0))
    {
      return 0;
    }
    m_data_index = 0;
    // Create the texture.
    // init y texture
    PxSize2D ySize = PxSize2D::Create(WIDTH, HEIGHT);
    GL_CHECK(glGenTextures(1, &m_yTex));
    m_y_texture.Reset(m_yTex, ySize);
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_yTex));
    GL_CHECK(glUniform1i(m_locSampler[0], 0));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexDirectVIV(GL_TEXTURE_2D, texWidth, texHeight, GL_ALPHA, (GLvoid**)&m_planes[0]));
    std::memcpy(m_planes[0], videoData, m_ySize);
    // std::memcy(videoData, videoData+m_ySize, m_planes[0]);
    videoData += m_ySize;
    glTexDirectInvalidateVIV(GL_TEXTURE_2D);

    // init uv texture
    PxSize2D uvSize = PxSize2D::Create(WIDTH / 2, HEIGHT / 2);
    GL_CHECK(glGenTextures(1, &m_uvTex));
    m_uv_texture.Reset(m_uvTex, uvSize);
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + 1));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_uvTex));
    GL_CHECK(glUniform1i(m_locSampler[1], 1));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexDirectVIV(GL_TEXTURE_2D, texWidth / 2, texHeight / 2, GL_LUMINANCE8_ALPHA8_EXT, (GLvoid**)&m_planes[1]));
    if (m_vSize > 0)
    {
      std::memcpy(m_planes[1], videoData, m_vSize);
      // std::copy(videoData, videoData+m_vSize, m_planes[1]);
      videoData += m_vSize;
    }
    GL_CHECK_FOR_ERROR();

    m_data_index += m_frameSize;
    if ((m_fileSize - m_data_index) < m_frameSize)
      m_data_index = 0;

    // Mark as dirty.
    glTexDirectInvalidateVIV(GL_TEXTURE_2D);

    GL_CHECK_FOR_ERROR();
    // Success.
    result = m_yTex;
    return result;
  }
}
