#ifndef GLES3_E7_0_PARTICLESYSTEM_E7_0_PARTICLESYSTEM_HPP
#define GLES3_E7_0_PARTICLESYSTEM_E7_0_PARTICLESYSTEM_HPP
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


#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NUM_PARTICLES 2000

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PARTICLE_SIZE 7

namespace Fsl
{
  class E7_0_ParticleSystem : public DemoAppGLES3
  {
    struct UserData
    {
      //
      GLint lifetimeLoc{0};
      GLint startPositionLoc{0};
      GLint endPositionLoc{0};
      GLint timeLoc{0};
      GLint colorLoc{0};
      GLint centerPositionLoc{0};
      GLint samplerLoc{0};

      // Texture handle
      GLES3::GLTexture Texture;

      // Particle vertex data
      float particleData[NUM_PARTICLES * PARTICLE_SIZE]{};    // NOLINT[modernize-avoid-c-arrays]

      // Current time
      float time{1.0f};

      UserData()
      {
        for (int i = 0; i < NUM_PARTICLES * PARTICLE_SIZE; ++i)
        {
          particleData[i] = 0.0f;
        }
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    explicit E7_0_ParticleSystem(const DemoAppConfig& config);
    ~E7_0_ParticleSystem() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;
  };
}

#endif
