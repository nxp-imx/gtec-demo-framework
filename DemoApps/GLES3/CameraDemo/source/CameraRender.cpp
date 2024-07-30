/****************************************************************************************************************************************************
 * Copyright 2018, 2024 NXP
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

#include "CameraRender.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/Camera/Platform/PlatformCameraSystem.hpp>
#include <array>
#include <limits>

// translate, rotate, scale, perspective
#include <glm/gtc/matrix_transform.hpp>
// value_ptr
#include <glm/gtc/type_ptr.hpp>
#include <memory>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    constexpr GLuint VertexLoc = 0;
    constexpr GLuint VertexTexLoc = 1;

    constexpr std::array<GLBindAttribLocation, 2> ShaderAttributeArray = {GLBindAttribLocation(VertexLoc, "g_vPosition"),
                                                                          GLBindAttribLocation(VertexTexLoc, "g_vTexCoord")};

    // Vertices
    // cameraPlaneVertices[0,1,2] = X,Y,Z Vertex 1
    // cameraPlaneVertices[3,4,5] = X,Y,Z Vertex 2
    // cameraPlaneVertices[6,7,8] = X,Y,Z Vertex 3
    // cameraPlaneVertices[9,10,11] = X,Y,Z Vertex 4
    const std::array<GLuint, 6> g_cameraPlaneIndices = {1, 0, 3, 1, 3, 2};

    const std::array<GLfloat, 8> g_cameraPlaneTexCoords = {0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0};
  }

  CameraRender::CameraRender(const std::shared_ptr<IContentManager>& contentManager, const PxSize2D& currentSizePx)
    : m_cameraSystem(Helios::PlatformCameraSystem::CreateCameraSystem())
    , m_camera(m_cameraSystem.Create())
  {
    const auto cameraExtent = m_camera.GetExtent();
    const auto cameraStride = m_camera.GetStride();
    // Try to find a OpenGL ES 3 texture configuration that matches the camera pixel format and stride
    m_cameraTextureConfig = GLRawBitmapUtil::Convert(m_camera.GetPixelFormat(), cameraExtent.Width.Value, cameraStride, true);

    // Allocate space for the target bitmap
    m_bitmapBuffer.resize(cameraExtent.Height.Value * cameraStride);

    // ReadOnlyRawBitmap(pContent, width, height, pixelFormat, stride, origin)

    // Compile the shader program, the shaders are located in the Content Folder
    m_program.Reset(contentManager->ReadAllText("Shader.vert"), contentManager->ReadAllText("Shader.frag"),
                    SpanUtil::AsReadOnlySpan(ShaderAttributeArray));
    const GLuint hProgram = m_program.Get();

    GLuint buffer = 0;

    // Get the uniform locations from the shader
    m_modelMatrixLoc = glGetUniformLocation(hProgram, "g_matModel");
    m_viewMatrixLoc = glGetUniformLocation(hProgram, "g_matView");
    m_projMatrixLoc = glGetUniformLocation(hProgram, "g_matProj");
    m_textureLoc = glGetUniformLocation(hProgram, "s_texture");

    // Assign the vertex info to cover the whole display under an orthogonal projection
    m_cameraPlaneVertices[4] = static_cast<float>(currentSizePx.RawHeight());
    m_cameraPlaneVertices[6] = static_cast<float>(currentSizePx.RawWidth());
    m_cameraPlaneVertices[7] = static_cast<float>(currentSizePx.RawHeight());
    m_cameraPlaneVertices[9] = static_cast<float>(currentSizePx.RawWidth());

    // Create the VAO and VBO for the quad
    glGenVertexArrays(1, &m_cameraVAO);
    glBindVertexArray(m_cameraVAO);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * g_cameraPlaneIndices.size(), g_cameraPlaneIndices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, UncheckedNumericCast<GLsizeiptr>(sizeof(GLfloat) * m_cameraPlaneVertices.size()), m_cameraPlaneVertices.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(VertexLoc);
    glVertexAttribPointer(VertexLoc, 3, GL_FLOAT, 0, 0, nullptr);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * g_cameraPlaneTexCoords.size(), g_cameraPlaneTexCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(VertexTexLoc);
    glVertexAttribPointer(VertexTexLoc, 2, GL_FLOAT, 0, 0, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create and allocate the space for the texture
    glGenTextures(1, &m_textureHandle);
    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, currentSizePx.RawWidth(), currentSizePx.RawHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Now set the uniform values
    glUseProgram(hProgram);

    m_projMatrix = glm::ortho(0.0f, static_cast<float>(currentSizePx.RawWidth()), 0.0f, static_cast<float>(currentSizePx.RawHeight()), 0.1f, 100.0f);
    glUniformMatrix4fv(m_projMatrixLoc, 1, GL_FALSE, glm::value_ptr(m_projMatrix));

    m_viewMatrix = glm::lookAt(glm::vec3(0.0, 0.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(m_viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));

    m_modelMatrix = glm::mat4(1.0);
    glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

    glUniform1i(m_textureLoc, 0);

    GL_CHECK(glUseProgram(0));
  }


  void CameraRender::Draw(const DemoTime& /*demoTime*/)
  {
    // Configure a 'RawBitmapEx' based on the camera settings and point it to m_bitmapBuffer.data()
    RawBitmapEx targetBitmap(RawBitmapEx::Create(SpanUtil::AsSpan(m_bitmapBuffer), m_camera.GetExtent(), m_camera.GetPixelFormat(),
                                                 m_camera.GetStride(), BitmapOrigin::UpperLeft));

    bool hasNewFrame = m_camera.TryRender(targetBitmap, m_frameId);

    // Do not cull polygons
    glDisable(GL_CULL_FACE);

    // Update the texture

    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    if (hasNewFrame)
    {
      const auto cameraSize = m_camera.GetSize();
      // Update the texture if it changed
      glTexImage2D(GL_TEXTURE_2D, 0, m_cameraTextureConfig.InternalFormat, cameraSize.RawWidth(), cameraSize.RawHeight(), 0,
                   m_cameraTextureConfig.Format, m_cameraTextureConfig.Type, targetBitmap.Content());
    }

    // Show the new frame
    glUseProgram(m_program.Get());
    glBindVertexArray(m_cameraVAO);
    glActiveTexture(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glUseProgram(0);
  }
}
