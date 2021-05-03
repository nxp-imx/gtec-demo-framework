/****************************************************************************************************************************************************
* Copyright 2021 NXP
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

#include "DrawPolygon.hpp"
#include <EGL/egl.h>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <algorithm>
#include <vector>
#include <array>

#include "mapbox/earcut.hpp"
#include "SkienaTriangulation.h"

PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXTProc;
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleEXTProc;

namespace Fsl
{

  namespace
  {
    // Add a bunch of possible polygons here for test purposes.
    crankPoint pentagonPoints[5] = {{100, 0}, {200, 50}, {150, 200}, {50, 200}, {0, 50}};
    crankPolygon pentagon = {pentagonPoints, 5};

    crankPoint testPoints1[10] = {{26, 167}, {77, 61}, {124, 152}, {210, 62}, {333, 22}, {371, 87}, {393, 216}, {252, 334}, {251, 221}, {94, 306}};
    crankPolygon testPolygon1 = {testPoints1, 10};


    crankPoint testPoints2[13] = {{335, 125}, {346, 266}, {253, 324}, {304, 213}, {193, 216}, {197, 353}, {64, 342},
                                  {169, 195}, {49, 224},  {55, 126},  {155, 180}, {127, 70},  {193, 148}};
    crankPolygon testPolygon2 = {testPoints2, 13};

    crankPoint testPoints3[10] = {{18, 145},  {123, 145}, {185, 29},  {250, 140}, {358, 145},
                                  {262, 228}, {326, 336}, {207, 280}, {43, 338},  {109, 228}};
    crankPolygon testPolygon3 = {testPoints3, 10};


    // The stroking rendering is achieved in 2 steps:
    // 1.- Draws a quad of a configurable width at every edge of the polygon, this approach requires additional processing for the corners.
    // 2.- At each corner, I'll draw a circle of width/2 radius to help make the corners better, a circle with 32 divisions looks good enough at multiple resolutions
    // This cornering scheme should only be used on simple and big polygons or enabled/disabled using LODs; polygons that are too subdivided (like a map) wouldn't require this scheme
    constexpr std::array<pointsForGL, 33> g_sourceStrokeCorners = {
      {{0.0f, 0.0f},         {1.0000f, 0.0000f},   {0.9808f, 0.1951f},   {0.9239f, 0.3827f},   {0.8315f, 0.5556f},   {0.7071f, 0.7071f},
       {0.5556f, 0.8315f},   {0.3827f, 0.9239f},   {0.1951f, 0.9808f},   {0.0000f, 1.0000f},   {-0.1951f, 0.9808f},  {-0.3827f, 0.9239f},
       {-0.5556f, 0.8315f},  {-0.7071f, 0.7071f},  {-0.8315f, 0.5556f},  {-0.9239f, 0.3827f},  {-0.9808f, 0.1951f},  {-1.0000f, 0.0000f},
       {-0.9808f, -0.1951f}, {-0.9239f, -0.3827f}, {-0.8315f, -0.5556f}, {-0.7071f, -0.7071f}, {-0.5556f, -0.8315f}, {-0.3827f, -0.9239f},
       {-0.1951f, -0.9808f}, {0.0000f, -1.0000f},  {0.1951f, -0.9808f},  {0.3827f, -0.9239f},  {0.5556f, -0.8315f},  {0.7071f, -0.7071f},
       {0.8315f, -0.5556f},  {0.9239f, -0.3827f},  {0.9808f, -0.1951f}}};

    // These indices will be used as the source to create the final corner index buffer. 
    constexpr std::array<uint32_t, 96> g_sourceStrokeCornerIndices{0, 1,  2,  0, 2,  3,  0, 3,  4,  0, 4,  5,  0, 5,  6,  0, 6,  7,  0, 7,  8,  0, 8,  9,
                                              0, 9,  10, 0, 10, 11, 0, 11, 12, 0, 12, 13, 0, 13, 14, 0, 14, 15, 0, 15, 16, 0, 16, 17,
                                              0, 17, 18, 0, 18, 19, 0, 19, 20, 0, 20, 21, 0, 21, 22, 0, 22, 23, 0, 23, 24, 0, 24, 25,
                                              0, 25, 26, 0, 26, 27, 0, 27, 28, 0, 28, 29, 0, 29, 30, 0, 30, 31, 0, 31, 32, 0, 32, 1};

    constexpr std::array<pointsForGL, 4> g_quadTexCoords = {{{0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}}};
  }

  DrawPolygon::DrawPolygon(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_polygonProgram()
    , m_fboProgram() 
    , m_windowSize(config.WindowMetrics.GetSizePx())
  {
    create_offscreen_polygon(m_osFBO, glm::vec4(1.0, 0.0, 0.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0), 10.0f, testPolygon3);
    PrepareShadersAndData();
    PrepareQuadGeometry();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    
  }


  DrawPolygon::~DrawPolygon()
  {
    GL_CHECK(glDeleteFramebuffers(1, &m_osFBO.FBO));
    GL_CHECK(glDeleteTextures(1, &m_osFBO.texture));
    // Once m_osFBO goes out of scope, the vectors and it's respective data will be removed.
    // As I didn't create Vertex Buffers, no need to remove them
    // AS I used the Demo Framework Shader Program Wrapper, it will take care of destroying the shaders.
  }


  void DrawPolygon::Update(const DemoTime& demoTime)
  {

  }


  void DrawPolygon::Draw(const DemoTime& demoTime)
  {
    draw_polygon_to_offscreen_buffer(&m_osFBO);
    draw_offscreen_buffer_to_screen(&m_osFBO);
  }

  void DrawPolygon::create_offscreen_polygon(offscreen_fbo_t& osFbo, const glm::vec4& color, const glm::vec4& strokeColor, float width,
                                         const crankPolygon& polygon)
  {
    osFbo.width = 512.0f;
    osFbo.height = 512.0f;
    osFbo.polygonColor = glm::vec4(color);
    osFbo.strokeColor = glm::vec4(strokeColor);

    PreparePolygonForPipeline(polygon, osFbo.polygon);

    // Triangulate the polygon, use my approach first, if it fails, reverse the polygon order, if it fails again, fall back to the mapbox algorithm
    if (!(HugoTriangulate::TriangulateCpp(osFbo.polygon.points, osFbo.indices)))
    {
      FSLLOG3_INFO("Couldn't Triangulate using Hugo's method, inverting the array and trying again");
      osFbo.indices.clear();
      std::reverse(osFbo.polygon.points.begin(), osFbo.polygon.points.end());
      if (!(HugoTriangulate::TriangulateCpp(osFbo.polygon.points, osFbo.indices)))
      {
        FSLLOG3_INFO("Tried 2 times to Triangulate with Hugo's method without success, Fallback to Mapbox triangulate Function");
        osFbo.indices.clear();
        TriangulateMapbox(osFbo.polygon, osFbo.indices);
      }
    }

    CalculateOutlineGeometry(osFbo.polygon, osFbo.strokeVertices, osFbo.strokeIndices, osFbo.strokeCornerVertices, osFbo.strokeCornerIndices, width);
    PrepareFBO(osFbo);
  }

  int32_t DrawPolygon::draw_polygon_to_offscreen_buffer(offscreen_fbo_t* osFbo)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, osFbo->FBO);
    glViewport(0, 0, osFbo->width, osFbo->height);
    glClearColor(0.5, 0.5, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_polygonProgram.Get());
    glEnableVertexAttribArray(m_polygonVertexPositionLoc);
    glVertexAttribPointer(m_polygonVertexPositionLoc, 2, GL_FLOAT, 0, 0, osFbo->polygon.points.data());
    glUniform4fv(m_polygonColorLoc, 1, glm::value_ptr(osFbo->polygonColor));
    glDrawElements(GL_TRIANGLES, osFbo->indices.size(), GL_UNSIGNED_INT, osFbo->indices.data());

    glUniform4fv(m_polygonColorLoc, 1, glm::value_ptr(osFbo->strokeColor));
    glVertexAttribPointer(m_polygonVertexPositionLoc, 2, GL_FLOAT, 0, 0, osFbo->strokeVertices.data());
    glDrawElements(GL_TRIANGLES, osFbo->strokeIndices.size(), GL_UNSIGNED_INT, osFbo->strokeIndices.data());

    glVertexAttribPointer(m_polygonVertexPositionLoc, 2, GL_FLOAT, 0, 0, osFbo->strokeCornerVertices.data());
    glDrawElements(GL_TRIANGLES, osFbo->strokeCornerIndices.size(), GL_UNSIGNED_INT, osFbo->strokeCornerIndices.data());
    if (glGetError() == GL_NO_ERROR)
      return 0;
    else
      return -1;
  }

  int32_t DrawPolygon::draw_offscreen_buffer_to_screen(offscreen_fbo_t* osFbo)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_windowSize.Width(), m_windowSize.Height());
    glClearColor(0.75, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_fboProgram.Get());
    glEnableVertexAttribArray(m_quadVertexPositionLoc);
    glEnableVertexAttribArray(m_quadTexCoordsLoc);

    glVertexAttribPointer(m_quadVertexPositionLoc, 2, GL_FLOAT, 0, 0, m_quadVertices.data());
    glVertexAttribPointer(m_quadTexCoordsLoc, 2, GL_FLOAT, 0, 0, g_quadTexCoords.data());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, osFbo->texture);
    glUniform1i(m_quadSamplerLoc, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (glGetError() == GL_NO_ERROR)
      return 0;
    else
      return -1;
  }

  void DrawPolygon::PreparePolygonForPipeline(const crankPolygon& inputPolygon, polygonGL& pipelinePolygon)
  {
    for (uint32_t i = 0; i < inputPolygon.numPoints; i++)
    {
      pipelinePolygon.points.push_back({(GLfloat)inputPolygon.points[i].x, (GLfloat)inputPolygon.points[i].y});
    }
  }

  void DrawPolygon::TriangulateMapbox(const polygonGL& inputPolygon, std::vector<uint32_t>& indices)
  {
    if (!indices.empty())
      throw GraphicsException("Cannot populate a non empty index buffer, please make sure that no other triangulate function has been ran");

    using mapBoxCoord = GLfloat;
    using mapBoxPoint = std::array<mapBoxCoord, 2>;

    // Prepare a structure suitable for mapbox's earcut algorithm to process
    std::vector<std::vector<mapBoxPoint>> polygon;
    polygon.resize(1);
    for (uint32_t i = 0; i < inputPolygon.points.size(); i++)
    {
      polygon[0].push_back({inputPolygon.points[i].x, inputPolygon.points[i].y});
    }

    indices = mapbox::earcut<uint32_t>(polygon);
    for (const auto& index : indices)
    {
      FSLLOG3_INFO("Index {}", index);
    }
  }

  void DrawPolygon::PrepareShadersAndData()
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_polygonProgram.Reset(content->ReadAllText("Polygon.vert"), content->ReadAllText("Polygon.frag"));

    glUseProgram(m_polygonProgram.Get());
    m_polygonVertexPositionLoc = glGetAttribLocation(m_polygonProgram.Get(), "a_Position");
    m_polygonMVPMatrixLoc = glGetUniformLocation(m_polygonProgram.Get(), "u_matMVP");
    m_polygonColorLoc = glGetUniformLocation(m_polygonProgram.Get(), "u_color");

    m_polygonModelMatrix = glm::mat4(1.0f);
    m_polygonViewMatrix = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    m_polygonProjMatrix = glm::ortho(0.0f, m_osFBO.width, m_osFBO.height, 0.0f, 0.1f, 100.0f);
    m_polygonMVPMatrix = m_polygonProjMatrix * m_polygonViewMatrix * m_polygonModelMatrix;
    glUniformMatrix4fv(m_polygonMVPMatrixLoc, 1, 0, glm::value_ptr(m_polygonMVPMatrix));

    if (m_osFBO.multiSampleExtensionsAvailable)
    {
      m_fboProgram.Reset(content->ReadAllText("SimpleTexture.vert"), content->ReadAllText("SimpleTexture.frag"));
    }
    else
    {
      m_fboProgram.Reset(content->ReadAllText("FXAA.vert"), content->ReadAllText("FXAA.frag"));
    }
      glUseProgram(m_fboProgram.Get());

      m_quadVertexPositionLoc = glGetAttribLocation(m_fboProgram.Get(), "a_position");
      m_quadTexCoordsLoc = glGetAttribLocation(m_fboProgram.Get(), "a_texCoords");
      m_quadMVPMatrixLoc = glGetUniformLocation(m_fboProgram.Get(), "u_matMVP");
      m_quadSamplerLoc = glGetUniformLocation(m_fboProgram.Get(), "u_texSampler");

      if (!m_osFBO.multiSampleExtensionsAvailable)
      {
        m_quadWidthLoc = glGetUniformLocation(m_fboProgram.Get(), "u_grd_c_tw");
        m_quadHeightLoc = glGetUniformLocation(m_fboProgram.Get(), "u_grd_c_th");
      }

      m_quadModelMatrix = glm::mat4(1.0f);
      m_quadViewMatrix = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
      m_quadProjMatrix = glm::ortho(0.0f, (float)m_windowSize.Width(), (float)m_windowSize.Height(), 0.0f, 0.1f, 100.0f);
      m_quadMVPMatrix = m_quadProjMatrix * m_quadViewMatrix * m_quadModelMatrix;
      glUniformMatrix4fv(m_quadMVPMatrixLoc, 1, 0, glm::value_ptr(m_quadMVPMatrix));
      glUniform1f(m_quadWidthLoc, m_osFBO.width);
      glUniform1f(m_quadHeightLoc, m_osFBO.height);
  }

  void DrawPolygon::PrepareFBO(struct offscreen_fbo_t& osFbo)
  {
    
    const char* extensionString = (const char*)glGetString(GL_EXTENSIONS);

    if (strstr(extensionString, "GL_EXT_multisampled_render_to_texture") != NULL)
    {
      glRenderbufferStorageMultisampleEXTProc = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)eglGetProcAddress("glRenderbufferStorageMultisampleEXT");
      glFramebufferTexture2DMultisampleEXTProc =
        (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)eglGetProcAddress("glFramebufferTexture2DMultisampleEXT");
    }
    if ((glRenderbufferStorageMultisampleEXTProc == NULL) || (glFramebufferTexture2DMultisampleEXTProc == NULL))
    {
      osFbo.multiSampleExtensionsAvailable = false;
    }
    else
    {
      osFbo.multiSampleExtensionsAvailable = true;
    }

    if (osFbo.multiSampleExtensionsAvailable)
    {
      try
      {
        glGetIntegerv(GL_MAX_SAMPLES_EXT, &osFbo.texSamples);

        GL_CHECK(glGenFramebuffers(1, &osFbo.FBO));
        GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, osFbo.FBO));
        GL_CHECK(glGenTextures(1, &osFbo.texture));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, osFbo.texture));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, osFbo.width, osFbo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
        glBindTexture(GL_TEXTURE_2D, 0);
        GL_CHECK(glFramebufferTexture2DMultisampleEXTProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, osFbo.texture, 0, osFbo.texSamples));

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
          throw GraphicsException("There was an issue with glFramebufferTexture2DMultisampleEXT");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
      catch (GraphicsException& e)
      {
        FSLLOG3_INFO("Exception when trying to create a Multisampled buffer, falling back to FXAA shader")
        osFbo.multiSampleExtensionsAvailable = false;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Clean all previously generated objects
        GL_CHECK(glDeleteFramebuffers(1, &osFbo.FBO));
        GL_CHECK(glDeleteTextures(1, &osFbo.texture));

        GL_CHECK(glGenFramebuffers(1, &osFbo.FBO));
        GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, osFbo.FBO));
        GL_CHECK(glGenTextures(1, &osFbo.texture));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, osFbo.texture));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, osFbo.width, osFbo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
        GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, osFbo.texture, 0));
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
          throw GraphicsException("Could Not create FBO");
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
    }
  }

  void DrawPolygon::PrepareQuadGeometry()
  {
    m_quadVertices[0].x = 0.0f;
    m_quadVertices[0].y = 0.0f;
    m_quadVertices[1].x = m_osFBO.width;
    m_quadVertices[1].y = 0.0f;
    m_quadVertices[2].x = 0.0f;
    m_quadVertices[2].y = m_osFBO.height;
    m_quadVertices[3].x = m_osFBO.width;
    m_quadVertices[3].y = m_osFBO.height;
  }
  
  void DrawPolygon::CalculateOutlineGeometry(const polygonGL& polygon, std::vector<pointsForGL>& strokeVertices, std::vector<uint32_t>& strokeIndices,
                                             std::vector<pointsForGL>& strokeCornerVertices, std::vector<uint32_t>& strokeCornerIndices, const float& width)
  {
    for (uint32_t i = 0; i < polygon.points.size(); i++)
    {
      pointsForGL currentPoint = polygon.points[i];
      pointsForGL nextPoint = polygon.points[(i + 1 + polygon.points.size()) % polygon.points.size()];

      glm::vec2 currentEdge(glm::vec2(nextPoint.x - currentPoint.x, nextPoint.y - currentPoint.y));
      currentEdge = glm::normalize(currentEdge);
      glm::vec2 perpendicularEdge(-currentEdge.y, currentEdge.x);

      pointsForGL corner1{currentPoint.x + ((width / 2.0f) * perpendicularEdge.x),
                          currentPoint.y + ((width / 2.0f) * perpendicularEdge.y)};
      pointsForGL corner2{currentPoint.x - ((width / 2.0f) * perpendicularEdge.x),
                          currentPoint.y - ((width / 2.0f) * perpendicularEdge.y)};
      pointsForGL corner3{nextPoint.x + ((width / 2.0f) * perpendicularEdge.x),
                          nextPoint.y + ((width / 2.0f) * perpendicularEdge.y)};
      pointsForGL corner4{nextPoint.x - ((width / 2.0f) * perpendicularEdge.x),
                          nextPoint.y - ((width / 2.0f) * perpendicularEdge.y)};

      strokeVertices.push_back(corner1);
      strokeVertices.push_back(corner2);
      strokeVertices.push_back(corner3);
      strokeVertices.push_back(corner4);

      strokeIndices.push_back(i * 4);
      strokeIndices.push_back(i * 4 + 1);
      strokeIndices.push_back(i * 4 + 3);
      strokeIndices.push_back(i * 4);
      strokeIndices.push_back(i * 4 + 2);
      strokeIndices.push_back(i * 4  + 3);

      //Use a pre-defined circle to fill the gaps between the outline quads
      for (uint32_t j = 0; j < g_sourceStrokeCorners.size(); j++)
      {
        strokeCornerVertices.push_back(pointsForGL{(g_sourceStrokeCorners[j].x * (width / 2.0f)) + currentPoint.x,
                                                   (g_sourceStrokeCorners[j].y * (width / 2.0f)) + currentPoint.y});
      }
      for (uint32_t j = 0; j < g_sourceStrokeCornerIndices.size(); j++)
      {
        strokeCornerIndices.push_back(i * g_sourceStrokeCorners.size() + g_sourceStrokeCornerIndices[j]);
      }
    }
  }
}
