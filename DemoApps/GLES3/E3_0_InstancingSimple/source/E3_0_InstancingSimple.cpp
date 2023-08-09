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
// Instancing.c
//
//    Demonstrates drawing multiple objects in a single draw call with
//    geometry instancing
//

/*
 * OpenGL ES 3.0 Tutorial 3
 *
 * Draws n number of cubes using instanced draw calls.
 */

#include "E3_0_InstancingSimple.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <vector>

// Defines needed for Instancing
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define srandom srand
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define random rand


namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    constexpr GLuint POSITION_LOC = 0;
    constexpr GLuint COLOR_LOC = 1;
    constexpr GLuint MVP_LOC = 2;


    /// \brief Generates geometry for a cube.  Allocates memory for the vertex data and stores
    ///        the results in the arrays.  Generate index list for a TRIANGLES
    /// \param scale The size of the cube, use 1.0 for a unit cube.
    /// \param vertices If not nullptr, will contain array of float3 positions
    /// \param normals If not nullptr, will contain array of float3 normals
    /// \param texCoords If not nullptr, will contain array of float2 texCoords
    /// \param indices If not nullptr, will contain the array of indices for the triangle strip
    /// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
    ///         if it is not nullptr ) as a GL_TRIANGLE_STRIP
    //
    int GenCube(float scale, std::vector<GLfloat>* pVertices, std::vector<GLfloat>* pNormals, std::vector<GLfloat>* pTexCoords,
                std::vector<GLuint>* pIndices)
    {
      int i = 0;
      constexpr int numVertices = 24;
      constexpr int numIndices = 36;

      const std::array<GLfloat, numVertices* 3> cubeVerts = {
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f,
      };

      const std::array<GLfloat, numVertices* 3> cubeNormals = {
        0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
        0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f,
        0.0f,  0.0f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
      };

      const std::array<GLfloat, numVertices* 2> cubeTex = {
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
      };

      // Allocate memory for buffers
      if (pVertices != nullptr)
      {
        pVertices->resize(3 * numVertices);
        memcpy(pVertices->data(), cubeVerts.data(), cubeVerts.size() * sizeof(GLfloat));

        for (i = 0; i < numVertices * 3; i++)
        {
          (*pVertices)[i] *= scale;
        }
      }

      if (pNormals != nullptr)
      {
        pNormals->resize(3 * numVertices);
        memcpy(pNormals->data(), cubeNormals.data(), cubeNormals.size() * sizeof(GLfloat));
      }

      if (pTexCoords != nullptr)
      {
        pTexCoords->resize(2 * numVertices);
        memcpy(pTexCoords->data(), cubeTex.data(), cubeTex.size() * sizeof(GLfloat));
      }


      // Generate the indices
      if (pIndices != nullptr)
      {
        const std::array<GLuint, numIndices> cubeIndices = {0,  2,  1,  0,  3,  2,  4,  5,  6,  4,  6,  7,  8,  9,  10, 8,  10, 11,
                                                            12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21};

        pIndices->resize(numIndices);
        memcpy(pIndices->data(), cubeIndices.data(), cubeIndices.size() * sizeof(GLuint));
      }

      return numIndices;
    }
  }


  E3_0_InstancingSimple::E3_0_InstancingSimple(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();

    GL_CHECK(glUseProgram(hProgram));

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));

    std::vector<GLfloat> positions;
    std::vector<GLuint> indices;

    // OSTEP1 Generate the vertex data
    m_userData.numIndices = GenCube(0.1f, &positions, nullptr, nullptr, &indices);

    // OSTEP2 Fill Index buffer object with Index data
    try
    {
      GL_CHECK(glGenBuffers(1, &m_userData.indicesIBO));
      GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.indicesIBO));
      GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_userData.numIndices, indices.data(), GL_STATIC_DRAW));
      GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

      // OSTEP3 Vertex Buffer Object with Vertex Data
      GL_CHECK(glGenBuffers(1, &m_userData.positionVBO));
      GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.positionVBO));
      GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat) * 3, positions.data(), GL_STATIC_DRAW));

      // Random color for each instance
      {
        GLubyte colors[NUM_INSTANCES][4];    // NOLINT(modernize-avoid-c-arrays)
        int instance = 0;

        srandom(0);

        for (instance = 0; instance < NUM_INSTANCES; instance++)
        {
          colors[instance][0] = random() % 255;
          colors[instance][1] = random() % 255;
          colors[instance][2] = random() % 255;
          colors[instance][3] = 0;
        }

        // OSTEP4 Generate a VBO for Colors
        GL_CHECK(glGenBuffers(1, &m_userData.colorVBO));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.colorVBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * 4, colors, GL_STATIC_DRAW));
      }

      // Allocate storage to store MVP per instance
      {
        int instance = 0;

        // Random angle for each instance, compute the MVP later
        for (instance = 0; instance < NUM_INSTANCES; instance++)
        {
          m_userData.angle[instance] = static_cast<float>(random() % 32768) / 32767.0f * 360.0f;
        }
        // OSTEP5 Generate a VBO for each Projection Matrix
        GL_CHECK(glGenBuffers(1, &m_userData.mvpVBO));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.mvpVBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * sizeof(Matrix), nullptr, GL_DYNAMIC_DRAW));
      }
      GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
    catch (const std::exception&)
    {
      Cleanup();
      throw;
    }
  }


  E3_0_InstancingSimple::~E3_0_InstancingSimple()
  {
    Cleanup();
  }


  void E3_0_InstancingSimple::Update(const DemoTime& demoTime)
  {
    const float deltaTime = demoTime.DeltaTime;
    const auto aspectRatio = GetWindowAspectRatio();

    Matrix* pMatBuf = nullptr;
    int instance = 0;
    int numRows = 0;
    int numColumns = 0;

    // Generate a perspective matrix with a 60 degree FOV
    const Matrix matPerspective = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(60.0f), aspectRatio, 1.0f, 20.0f);


    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.mvpVBO));
    GL_CHECK(pMatBuf = reinterpret_cast<Matrix*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Matrix) * NUM_INSTANCES, GL_MAP_WRITE_BIT)));

    assert(pMatBuf != nullptr);

    // Compute a per-instance MVP that translates and rotates each instance differently
    numRows = static_cast<int>(sqrtf(NUM_INSTANCES));
    numColumns = numRows;

    for (instance = 0; instance < NUM_INSTANCES; instance++)
    {
      float translateX = (static_cast<float>(instance % numRows) / static_cast<float>(numRows)) * 2.0f - 1.0f;
      float translateY =
        (static_cast<float>(instance / numColumns) / static_cast<float>(numColumns)) * 2.0f - 1.0f;    // NOLINT(bugprone-integer-division)

      // Compute a rotation angle based on time to rotate the cube
      m_userData.angle[instance] += (deltaTime * 40.0f);

      if (m_userData.angle[instance] >= 360.0f)
      {
        m_userData.angle[instance] -= 360.0f;
      }

      // Generate a model view matrix to rotate/translate the cube
      // Per-instance translation
      const float angle = m_userData.angle[instance] * MathHelper::TO_RADS;
      Matrix matModelView =
        Matrix::CreateRotationX(angle) * Matrix::CreateRotationZ(angle) * Matrix::CreateTranslation(translateX, translateY, -2.0f);

      // Compute the final MVP by multiplying the
      // model view and perspective matrices together
      pMatBuf[instance] = matModelView * matPerspective;
    }

    GL_CHECK(glUnmapBuffer(GL_ARRAY_BUFFER));
  }


  void E3_0_InstancingSimple::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    PxSize2D sizePx = GetWindowSizePx();

    glViewport(0, 0, sizePx.RawWidth(), sizePx.RawHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load the vertex position
    glBindBuffer(GL_ARRAY_BUFFER, m_userData.positionVBO);
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(POSITION_LOC);

    // OSTEP6 Load the instance color buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_userData.colorVBO);
    glVertexAttribPointer(COLOR_LOC, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(GLubyte), nullptr);
    glEnableVertexAttribArray(COLOR_LOC);

    // OSTEP7 Set the Attribute Divisor for Colors
    glVertexAttribDivisor(COLOR_LOC, 1);    // One color per instance


    // OSTEP8 Load the instance MVP buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_userData.mvpVBO);

    // Load each matrix row of the MVP.  Each row gets an increasing attribute location.
    glVertexAttribPointer(MVP_LOC + 0, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), nullptr);
    glVertexAttribPointer(MVP_LOC + 1, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), reinterpret_cast<const void*>(sizeof(GLfloat) * 4));
    glVertexAttribPointer(MVP_LOC + 2, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), reinterpret_cast<const void*>(sizeof(GLfloat) * 8));
    glVertexAttribPointer(MVP_LOC + 3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), reinterpret_cast<const void*>(sizeof(GLfloat) * 12));
    glEnableVertexAttribArray(MVP_LOC + 0);
    glEnableVertexAttribArray(MVP_LOC + 1);
    glEnableVertexAttribArray(MVP_LOC + 2);
    glEnableVertexAttribArray(MVP_LOC + 3);

    // OSTEP9 One MVP per instance
    glVertexAttribDivisor(MVP_LOC + 0, 1);
    glVertexAttribDivisor(MVP_LOC + 1, 1);
    glVertexAttribDivisor(MVP_LOC + 2, 1);
    glVertexAttribDivisor(MVP_LOC + 3, 1);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.indicesIBO);

    // OSTEP10 Draw the cubes
    glDrawElementsInstanced(GL_TRIANGLES, m_userData.numIndices, GL_UNSIGNED_INT, nullptr, NUM_INSTANCES);
  }


  void E3_0_InstancingSimple::Cleanup()
  {
    if (m_userData.mvpVBO != GLValues::INVALID_HANDLE)
    {
      glDeleteVertexArrays(1, &m_userData.mvpVBO);
      m_userData.mvpVBO = GLValues::INVALID_HANDLE;
    }

    if (m_userData.colorVBO != GLValues::INVALID_HANDLE)
    {
      glDeleteVertexArrays(1, &m_userData.colorVBO);
      m_userData.colorVBO = GLValues::INVALID_HANDLE;
    }

    if (m_userData.positionVBO != GLValues::INVALID_HANDLE)
    {
      glDeleteVertexArrays(1, &m_userData.positionVBO);
      m_userData.positionVBO = GLValues::INVALID_HANDLE;
    }

    if (m_userData.indicesIBO != GLValues::INVALID_HANDLE)
    {
      glDeleteVertexArrays(1, &m_userData.indicesIBO);
      m_userData.indicesIBO = GLValues::INVALID_HANDLE;
    }
  }
}
