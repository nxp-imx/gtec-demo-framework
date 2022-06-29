// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// ParticleSystem.c
//
//    This is an example that demonstrates rendering a particle system
//    using a vertex shader and point sprites.
//

/*
 * OpenGL ES 3.0 Tutorial 7
 *
 * Draws a Particle system
 */

#include "E7_0_ParticleSystem.hpp"
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <cstdlib>
#include <iostream>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    constexpr GLuint ATTRIBUTE_LIFETIME_LOCATION = 0;
    constexpr GLuint ATTRIBUTE_STARTPOSITION_LOCATION = 1;
    constexpr GLuint ATTRIBUTE_ENDPOSITION_LOCATION = 2;
  }


  E7_0_ParticleSystem::E7_0_ParticleSystem(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();

    // use our shader
    GL_CHECK(glUseProgram(hProgram));

    // Get the attribute locations
    GL_CHECK(m_userData.lifetimeLoc = glGetAttribLocation(hProgram, "a_lifetime"));
    GL_CHECK(m_userData.startPositionLoc = glGetAttribLocation(hProgram, "a_startPosition"));
    GL_CHECK(m_userData.endPositionLoc = glGetAttribLocation(hProgram, "a_endPosition"));

    // Get the uniform locations
    GL_CHECK(m_userData.timeLoc = glGetUniformLocation(hProgram, "u_time"));
    GL_CHECK(m_userData.centerPositionLoc = glGetUniformLocation(hProgram, "u_centerPosition"));
    GL_CHECK(m_userData.colorLoc = glGetUniformLocation(hProgram, "u_color"));
    GL_CHECK(m_userData.samplerLoc = glGetUniformLocation(hProgram, "s_texture"));

    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

    // Fill in particle data array
    srand(0);

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
      float* particleData = &m_userData.particleData[i * PARTICLE_SIZE];

      // Lifetime of particle
      (*particleData++) = (static_cast<float>(rand() % 10000) / 10000.0f);

      // End position of particle
      (*particleData++) = (static_cast<float>(rand() % 10000) / 5000.0f) - 1.0f;
      (*particleData++) = (static_cast<float>(rand() % 10000) / 5000.0f) - 1.0f;
      (*particleData++) = (static_cast<float>(rand() % 10000) / 5000.0f) - 1.0f;

      // Start position of particle
      (*particleData++) = (static_cast<float>(rand() % 10000) / 40000.0f) - 0.125f;
      (*particleData++) = (static_cast<float>(rand() % 10000) / 40000.0f) - 0.125f;
      (*particleData++) = (static_cast<float>(rand() % 10000) / 40000.0f) - 0.125f;
    }

    {    // We use a scope here so we only keep the bitmap in memory as long as we need it
      Bitmap smoke;
      content->Read(smoke, "smoke.png", PixelFormat::R8G8B8_UNORM);
      m_userData.Texture.SetData(smoke, GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE));
    }
  }


  E7_0_ParticleSystem::~E7_0_ParticleSystem() = default;


  void E7_0_ParticleSystem::Update(const DemoTime& demoTime)
  {
    m_userData.time += 1 / 4.0f * demoTime.DeltaTime;

    if (m_userData.time >= 1.0f)
    {
      std::array<float, 3> centerPos{};
      std::array<float, 4> color{};

      m_userData.time = 0.0f;

      // Pick a new start location and color
      centerPos[0] = (static_cast<float>(rand() % 10000) / 10000.0f) - 0.5f;
      centerPos[1] = (static_cast<float>(rand() % 10000) / 10000.0f) - 0.5f;
      centerPos[2] = (static_cast<float>(rand() % 10000) / 10000.0f) - 0.5f;

      GL_CHECK(glUniform3fv(m_userData.centerPositionLoc, 1, centerPos.data()));

      // Random color
      color[0] = (static_cast<float>(rand() % 10000) / 20000.0f) + 0.5f;
      color[1] = (static_cast<float>(rand() % 10000) / 20000.0f) + 0.5f;
      color[2] = (static_cast<float>(rand() % 10000) / 20000.0f) + 0.5f;
      color[3] = 0.5;

      GL_CHECK(glUniform4fv(m_userData.colorLoc, 1, color.data()));
    }

    // Load uniform time variable
    GL_CHECK(glUniform1f(m_userData.timeLoc, m_userData.time));
  }


  void E7_0_ParticleSystem::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    PxSize2D sizePx = GetWindowSizePx();

    glViewport(0, 0, sizePx.Width(), sizePx.Height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load the vertex attributes
    glVertexAttribPointer(ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT, GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat), m_userData.particleData);
    glVertexAttribPointer(ATTRIBUTE_ENDPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat), &m_userData.particleData[1]);
    glVertexAttribPointer(ATTRIBUTE_STARTPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat), &m_userData.particleData[4]);

    glEnableVertexAttribArray(ATTRIBUTE_LIFETIME_LOCATION);
    glEnableVertexAttribArray(ATTRIBUTE_ENDPOSITION_LOCATION);
    glEnableVertexAttribArray(ATTRIBUTE_STARTPOSITION_LOCATION);

    // Blend particles

    // FIX: These enables look wrong
    // glEnable(0x8861);   // GL_POINT_SPRITE
    // glEnable(0x8642);   // GL_VERTEX_PROGRAM_POINT_SIZE

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    // glBlendFunc(GL_ONE, GL_ONE);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_userData.Texture.Get());

    // Set the sampler texture unit to 0
    glUniform1i(m_userData.samplerLoc, 0);

    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

    const auto glErr = glGetError();
    if (GL_NO_ERROR != glErr)
    {
      FSLLOG3_ERROR("Errors While Drawing 0x{:x}", glErr);
    }
  }
}
