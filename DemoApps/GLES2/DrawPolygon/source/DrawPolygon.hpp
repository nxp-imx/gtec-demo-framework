#ifndef GLES2_DRAWPOLYGON_HPP
#define GLES2_DRAWPOLYGON_HPP
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

#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>
#include <FslUtil/OpenGLES2/GLTexture.hpp>
#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

namespace Fsl
{
  namespace
  {
    struct crankPoint
    {
      int32_t x;
      int32_t y;
    };

    struct crankPolygon
    {
      crankPoint* points;
      uint32_t numPoints;
    };

    struct offscreen_fbo_t
    {
      //FBO Data
      GLuint FBO;
      GLuint texture;
      GLuint depth;
      GLint texSamples;
      bool multiSampleExtensionsAvailable;
      GLfloat width;
      GLfloat height;
        
      //Geometry Data
      polygonGL polygon;
      std::vector<GLuint> indices;
      std::vector<pointsForGL> strokeVertices;
      std::vector<GLuint> strokeIndices;
      std::vector<pointsForGL> strokeCornerVertices;
      std::vector<GLuint> strokeCornerIndices;
      glm::vec4 polygonColor;
      glm::vec4 strokeColor;
    };
  }
    
  class DrawPolygon final : public DemoAppGLES2
  {
    GLES2::GLProgram m_polygonProgram;
    GLES2::GLProgram m_fboProgram;

    struct offscreen_fbo_t m_osFBO;

    GLint m_polygonVertexPositionLoc{-1};
    GLint m_polygonMVPMatrixLoc{-1};
    GLint m_polygonColorLoc{-1};

    std::array<pointsForGL, 4> m_quadVertices;
    
    GLint m_quadVertexPositionLoc{-1};
    GLint m_quadTexCoordsLoc{-1};
    GLint m_quadMVPMatrixLoc{-1};
    GLint m_quadSamplerLoc{-1};
    GLint m_quadWidthLoc{-1};
    GLint m_quadHeightLoc{-1};

    glm::mat4 m_polygonModelMatrix{};
    glm::mat4 m_polygonViewMatrix{};
    glm::mat4 m_polygonProjMatrix{};
    glm::mat4 m_polygonMVPMatrix{};

    glm::mat4 m_quadModelMatrix{};
    glm::mat4 m_quadViewMatrix{};
    glm::mat4 m_quadProjMatrix{};
    glm::mat4 m_quadMVPMatrix{};

    PxSize2D m_windowSize;

  public:
    DrawPolygon(const DemoAppConfig& config);
    ~DrawPolygon() final;
  protected:
    void Update(const DemoTime& demoTime) final;
    void Draw(const DemoTime& demoTime) final;
    void create_offscreen_polygon(offscreen_fbo_t& osFbo, const glm::vec4& color, const glm::vec4& strokeColor, float width,
                                  const crankPolygon& polygon);
    int32_t draw_polygon_to_offscreen_buffer(offscreen_fbo_t* osFbo);
    int32_t draw_offscreen_buffer_to_screen(offscreen_fbo_t* osFbo);
    void PreparePolygonForPipeline(const crankPolygon& inputPolygon, polygonGL& pipelinePolygon);
    void PrepareShadersAndData();
    void PrepareFBO(struct offscreen_fbo_t& osFbo);
    void PrepareQuadGeometry();
    void TriangulateMapbox(const polygonGL& inputPolygon, std::vector<uint32_t>& indices);
    void CalculateOutlineGeometry(const polygonGL& polygon, std::vector<pointsForGL>& strokeVertices, std::vector<uint32_t>& strokeIndices,
                                  std::vector<pointsForGL>& strokeCornerVertices, std::vector<uint32_t>& strokeCornerIndices,
                                  const float& width);
  };
}

#endif
