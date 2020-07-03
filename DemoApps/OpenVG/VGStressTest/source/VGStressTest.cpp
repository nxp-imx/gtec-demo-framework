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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/System/Threading/Thread.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include "Logo.hpp"
#include "VGStressTest.hpp"
#include "VGStressTestOptionParser.hpp"
#include <VG/openvg.h>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>

namespace Fsl
{
  namespace
  {
    /*
    void CreateSpiralVertices(std::vector<Vector2>& rDst, const int numVerticesPerCircle, const int numRevolutions, const float shrink,
      const Rectangle& dstArea, bool bAddEndVertex = false)
    {
      rDst.resize((numVerticesPerCircle * numRevolutions) + (bAddEndVertex ? 1 : 0));
      const int w2 = dstArea.Width() / 2;
      const int h2 = dstArea.Height() / 2;
      const float angleAdd = (2 * MathHelper::PI) / numVerticesPerCircle;
      float modOffset = 0;
      int dstIdx = 0;

      const float shrinkStep = shrink / numVerticesPerCircle;

      for (int circleIdx = 0; circleIdx < numRevolutions; ++circleIdx)
      {
        float angle = 0.0f;
        for (int i = 0; i < numVerticesPerCircle; ++i)
        {
          float mod = modOffset + (i * shrinkStep);
          rDst[dstIdx] = Vector2(dstArea.X() + w2 + (std::sin(angle) * (w2 - mod)), dstArea.Y() + h2 + (std::cos(angle) * (h2 - mod)));
          ++dstIdx;
          angle += angleAdd;
        }
        modOffset += shrink;
      }
      if (bAddEndVertex)
      {
        int i = 0;
        float angle = 0.0f;
        float mod = modOffset + (i * shrinkStep);
        rDst[dstIdx] = Vector2(dstArea.X() + w2 + (std::sin(angle) * (w2 - mod)), dstArea.Y() + h2 + (std::cos(angle) * (h2 - mod)));
      }
    }*/

    // Creates the spiral around the center and grows it
    void CreateSpiralVertices(std::vector<Vector2>& rDst, const int numVerticesPerCircle, const int numRevolutions, const float grow,
                              const Point2& dstCenter, const bool bAddEndVertex = false)
    {
      rDst.resize((numVerticesPerCircle * numRevolutions) + (bAddEndVertex ? 1 : 0));
      const float angleAdd = (2 * MathHelper::PI) / numVerticesPerCircle;
      float modOffset = grow / 2;
      int dstIdx = 0;

      const float growStep = grow / numVerticesPerCircle;

      for (int circleIdx = 0; circleIdx < numRevolutions; ++circleIdx)
      {
        float angle = 0.0f;
        for (int i = 0; i < numVerticesPerCircle; ++i)
        {
          float mod = modOffset + (i * growStep);
          rDst[dstIdx] = Vector2(dstCenter.X + (std::sin(angle) * mod), dstCenter.Y + (std::cos(angle) * mod));
          ++dstIdx;
          angle += angleAdd;
        }
        modOffset += grow;
      }
      if (bAddEndVertex)
      {
        int i = 0;
        float angle = 0.0f;
        float mod = modOffset + (i * growStep);
        rDst[dstIdx] = Vector2(dstCenter.X + (std::sin(angle) * mod), dstCenter.Y + (std::cos(angle) * mod));
      }
    }

    void GenerateQuadricSpiralVertices(std::vector<Vector2>& rDst, const int numRevolutions, const float shrink, const Rectangle& dstArea)
    {
      //      G---F
      //  9---8--7|
      //  |1--0  ||
      //  ||     ||
      //  A2     6E
      //  ||     ||
      //  |3--4--5|
      //  B---C---D

      // 0,1,2,3,4,5,6,7 (8 points per circle)
      // 8,9,A,B,C,D,E,F
      const int numVerticesPerCircle = 8;

      CreateSpiralVertices(rDst, numVerticesPerCircle, numRevolutions, shrink, dstArea.GetCenter(), true);

      // Flatten the corners
      for (int i = 0; i < numRevolutions; ++i)
      {
        const int offset = i * numVerticesPerCircle;
        rDst[offset + 1] = Vector2(rDst[offset + 2].X, rDst[offset + 0].Y);
        rDst[offset + 3] = Vector2(rDst[offset + 2].X, rDst[offset + 4].Y);
        rDst[offset + 5] = Vector2(rDst[offset + 6].X, rDst[offset + 4].Y);
        rDst[offset + 7] = Vector2(rDst[offset + 6].X, rDst[offset + 8].Y);
      }
    }


    void ExtractLinePathTo(OpenVG::VGPathBuffer& rDst, const std::vector<Vector2>& srcVertices, const std::size_t vertexOffset,
                           const std::size_t vertexLength)
    {
      // Allocate the segments
      std::vector<VGubyte> segments(vertexLength + 1);
      segments[0] = VG_MOVE_TO_ABS;
      for (std::size_t i = 1; i <= vertexLength; ++i)
        segments[i] = VG_LINE_TO_ABS;

      rDst.Reset(srcVertices, static_cast<uint32_t>(vertexOffset), static_cast<uint32_t>(vertexLength + 1), segments);
    }


    void ExtractCubicPathTo(OpenVG::VGPathBuffer& rDst, const std::vector<Vector2>& srcVertices, const std::size_t vertexOffset,
                            const std::size_t vertexLength)
    {
      // Allocate the segments
      std::vector<VGubyte> segments(2);
      segments[0] = VG_MOVE_TO_ABS;
      segments[1] = VG_CUBIC_TO_ABS;

      // Extract the vertices
      std::vector<VGfloat> vertices(4 * 2);

      const Vector2 v1 = srcVertices[vertexOffset];
      const Vector2 v2 = srcVertices[vertexOffset + vertexLength / 2];
      const Vector2 v3 = srcVertices[vertexOffset + vertexLength - 1];

      const float dxA = v2.X - v1.X;
      const float dyA = v2.Y - v1.Y;
      const float dxB = v3.X - v2.X;
      const float dyB = v3.Y - v2.Y;

      vertices[0] = v1.X;
      vertices[1] = v1.Y;
      vertices[2] = v1.X + dxA * 0.5f;
      vertices[3] = v1.Y + dyA * 0.5f;
      vertices[4] = v2.X + dxB * 0.5f;
      vertices[5] = v2.Y + dyB * 0.5f;
      vertices[6] = v3.X;
      vertices[7] = v3.Y;

      rDst.Reset(vertices, segments);
    }

    void GenerateSegmentedSpiral(std::vector<VGPathBufferPtr>& rPaths, const int numRevolutions, const float shrinkPerRevolution,
                                 const int numSegmentsPerRevolution, const Rectangle& dstArea)
    {
      const int verticesPerCircle = 256;

      std::vector<Vector2> spiralVertices;
      CreateSpiralVertices(spiralVertices, verticesPerCircle, numRevolutions, shrinkPerRevolution, dstArea.GetCenter());

      // Extract line path segments for the spiral
      {
        const std::size_t numSegments = static_cast<std::size_t>(numSegmentsPerRevolution * numRevolutions);
        const auto num = spiralVertices.size() / numSegments;
        rPaths.resize(numSegments);
        for (std::size_t i = 0; i < numSegments; ++i)
        {
          VGPathBufferPtr newPath(new OpenVG::VGPathBuffer());
          ExtractLinePathTo(*newPath, spiralVertices, i * num, num - 1);
          rPaths[i] = newPath;
        }
      }
    }


    void GenerateCubicSegmentedSpiral(std::vector<VGPathBufferPtr>& rPaths, const int numRevolutions, const float shrink,
                                      const int numSegmentsPerRevolution, const Rectangle& dstArea)
    {
      const int verticesPerCircle = 256;

      std::vector<Vector2> spiralVertices;
      CreateSpiralVertices(spiralVertices, verticesPerCircle, numRevolutions, shrink, dstArea.GetCenter());

      // Extract cubic paths from the line segments
      {
        const std::size_t numSegments = numSegmentsPerRevolution * numRevolutions;
        const std::size_t num = spiralVertices.size() / numSegments;
        rPaths.resize(numSegments);
        for (std::size_t i = 0; i < numSegments; ++i)
        {
          VGPathBufferPtr newPath(new OpenVG::VGPathBuffer());
          ExtractCubicPathTo(*newPath, spiralVertices, i * num, num - 1);
          rPaths[i] = newPath;
        }
      }
    }

    void GenerateQuadricSpiral(OpenVG::VGPathBuffer& rPath, const int numRevolutions, const float shrink, const Rectangle& dstArea)
    {
      std::vector<Vector2> srcVertices;
      GenerateQuadricSpiralVertices(srcVertices, numRevolutions, shrink, dstArea);

      // Allocate the segments
      // 8  |  9 -> 5    //  9/2 + 1 = 5
      // 16 | 17 -> 9    // 17/2 + 1 = 8
      // 24 | 25 ->      // 25/2 + 1 = 13
      const std::size_t pathLength = (srcVertices.size() / 2) + 1;
      std::vector<VGubyte> segments(pathLength);
      segments[0] = VG_MOVE_TO_ABS;
      for (std::size_t i = 1; i < pathLength; ++i)
        segments[i] = VG_QUAD_TO_ABS;

      rPath.Reset(srcVertices, segments);
    }


    void DrawPath(const OpenVG::VGPathBuffer& rPath, const VGPaint& stroke, const float* pColorStroke)
    {
      vgSetParameterfv(stroke, VG_PAINT_COLOR, 4, pColorStroke);
      vgSetPaint(stroke, VG_STROKE_PATH);

      vgDrawPath(rPath.GetHandle(), VG_STROKE_PATH);
    }


    void DrawSegmentedSpiral(const std::vector<VGPathBufferPtr>& paths, const float strokeLineWidth, const VGPaint paintStroke,
                             const VGPaint paintFill, const float* const pColor)
    {
      float colorFill[] = {1.0f, 1.0f, 0.0f, 1.0f};

      vgSetParameterfv(paintFill, VG_PAINT_COLOR, 4, &colorFill[0]);
      vgSetf(VG_STROKE_LINE_WIDTH, strokeLineWidth);

      vgSeti(VG_STROKE_CAP_STYLE, VG_CAP_ROUND);
      vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_ROUND);

      const std::size_t count = paths.size();
      for (std::size_t i = 0; i < count; ++i)
      {
        DrawPath(*(paths[i]), paintStroke, pColor);
      }
    }

    void DrawStrokedQuadricSpiral(const OpenVG::VGPathBuffer& VGPathBuffer, const float strokeLineWidth, const VGPaint paint, float* pColor,
                                  const int numLayers, const uint16_t angle)
    {
      VGfloat d[] = {500, 15};
      vgSetfv(VG_STROKE_DASH_PATTERN, 2, d);
      vgSeti(VG_STROKE_CAP_STYLE, VG_CAP_ROUND);
      vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_ROUND);

      vgSetf(VG_STROKE_LINE_WIDTH, strokeLineWidth);

      vgLoadIdentity();

      for (int i = 0; i < numLayers; ++i)
      {
        DrawPath(VGPathBuffer, paint, pColor);
        vgTranslate((std::sin((float)angle * MathHelper::TO_RADS) * i * 0.005f) + (i * 0.005f),
                    (std::cos((float)angle * MathHelper::TO_RADS) * i * 0.005f) + (i * 0.005f));
      }
      vgLoadIdentity();

      vgSetfv(VG_STROKE_DASH_PATTERN, 0, nullptr);
    }
  }


  VGStressTest::VGStressTest(const DemoAppConfig& config)
    : DemoAppVG(config)
    , m_config()
    , m_stroke(VG_INVALID_HANDLE)
    , m_stroke2(VG_INVALID_HANDLE)
    , m_fill(VG_INVALID_HANDLE)
    , m_imgPaint(VG_INVALID_HANDLE)
    , m_imgPattern(VG_INVALID_HANDLE)
    , m_paths()
    , m_angle(0)
    //, m_VGS_test_pass(false)
    //, m_VGS_framerate_avg(0)
    //, m_VGS_error_count(0)
    , m_toggleMinMax(false)
  {
    std::shared_ptr<VGStressTestOptionParser> options = config.GetOptions<VGStressTestOptionParser>();
    m_config = options->GetConfig();
    m_toggleMinMax = m_config.GetToggleMinMax();


    const PxSize2D windowSizePx = GetWindowSizePx();

    m_stroke = vgCreatePaint();
    vgSetPaint(m_stroke, VG_STROKE_PATH);
    m_fill = vgCreatePaint();
    vgSetPaint(m_fill, VG_FILL_PATH);

    m_stroke2 = vgCreatePaint();
    vgSetPaint(m_stroke2, VG_STROKE_PATH);

    //#ifdef GC355_PROFILER
    //    initGC355Profiler();
    //#endif
    {
      const RawBitmap logo = Logo::GetBitmap();

      const int w = logo.Width();
      const int h = logo.Height();

      const uint8_t* pContent = static_cast<const uint8_t*>(logo.Content());
      const std::size_t imgSize = (std::size_t)logo.GetByteSize();
      std::vector<uint8_t> img(imgSize);
      std::copy(pContent, pContent + imgSize, img.begin());

      RawBitmapEx logoEx(img.data(), logo.GetExtent(), logo.GetPixelFormat(), logo.Stride(), logo.GetOrigin());
      RawBitmapUtil::Swizzle(logoEx, 3, 2, 1, 0);

      const VGImageFormat format = VG_lRGBX_8888;

      // VG_lRGBX_8888
      // Appear to be stored as AA, BB, GG, RR

      m_imgPattern = vgCreateImage(format, w, h, VG_IMAGE_QUALITY_FASTER);
      vgImageSubData(m_imgPattern, &img[0], 4 * w, /* stride */
                     format, 0, 0, w, h);
      m_imgPaint = vgCreatePaint();
      vgSetParameteri(m_imgPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_PATTERN);
      vgSetParameteri(m_imgPaint, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_REPEAT);
      vgPaintPattern(m_imgPaint, m_imgPattern);
    }

    {
      Rectangle dstArea;

      int dim = 0;
      if (windowSizePx.Width() >= windowSizePx.Height())
      {
        dim = windowSizePx.Width();
        dstArea = Rectangle(0, (windowSizePx.Height() - windowSizePx.Width()) / 2, dim, dim);
      }
      else
      {
        dim = windowSizePx.Height() / 2;
        dstArea = Rectangle((windowSizePx.Width() - windowSizePx.Height()) / 2, 0, dim, dim);
      }

      {
        std::vector<VGubyte> segments(2);
        segments[0] = VG_MOVE_TO_ABS;
        segments[1] = VG_QUAD_TO_ABS;

        std::vector<VGfloat> vertices(8);
        vertices[0] = 100;    // x1
        vertices[1] = 100;    // y1
        vertices[2] = 150;    // x1 + dx * 0.5
        vertices[3] = 100;    // y1
        vertices[4] = 200;    // x2
        vertices[5] = 150;    // y1 + dy * 0.5
        vertices[6] = 200;    // x2
        vertices[7] = 200;    // y2
      }


      switch (m_config.GetType())
      {
      case 0:
      case 1:
        GenerateQuadricSpiral(m_test, m_config.GetQuadricSpiralRevolutionCount(),
                              m_config.GetQuadricSpiralRevolutionChange(windowSizePx.Width(), windowSizePx.Height()), dstArea);
        break;
      case 2:
        GenerateCubicSegmentedSpiral(m_paths, m_config.GetSegmentedSpiralRevolutionCount(),
                                     m_config.GetSegmentedSpiralRevolutionChange(dstArea.Width(), dstArea.Height()),
                                     m_config.GetSegmentedSpiralSegmentsPerRevolution(), dstArea);
        break;
      case 3:
        GenerateSegmentedSpiral(m_paths, m_config.GetSegmentedSpiralRevolutionCount(),
                                m_config.GetSegmentedSpiralRevolutionChange(dstArea.Width(), dstArea.Height()),
                                m_config.GetSegmentedSpiralSegmentsPerRevolution(), dstArea);
        break;
      default:
        break;
      }
    }
  }


  VGStressTest::~VGStressTest()
  {
    vgDestroyPaint(m_imgPaint);
    vgDestroyPaint(m_fill);
    vgDestroyPaint(m_stroke2);
    vgDestroyPaint(m_stroke);
    vgDestroyImage(m_imgPattern);
    //#ifdef GC355_PROFILER
    //    stopGC355Profiler();
    //#endif

    // if (vgGetError() == VG_NO_ERROR) // only error to catch in this test, rest is caught by EGL setup and framework
    //  mVGS_test_pass = true;
    // mVGS_framerate_avg = GetFramerateAvg();

    // mVGS_error_count += GetDcicErrorCount();
    // if (mVGS_error_count != 0)
    //  mVGS_test_pass = false;
    //
    // LogResult(mVGS_test_pass, mVGS_framerate_avg, mVGS_error_count);
    // if (mVGS_test_pass == false)
    //{
    //  ChangeExitCode(StatusCode::Enum::ERR_UKNOWN);
    //}
  }


  void VGStressTest::Update(const DemoTime& demoTime)
  {
    // if (IsDcicCheckEnabled())
    //{
    //  m_angle = 0;
    //}
    // else
    {
      ++m_angle;
      if (m_angle == 360)
        m_angle = 0;
    }
  }


  void VGStressTest::Draw(const DemoTime& demoTime)
  {
    bool bypassRender = false;
    if (m_toggleMinMax == true)
    {
      std::time_t result = std::time(nullptr);
      if ((result % 10) < 5)
      {
        bypassRender = true;
        Thread::SleepMilliseconds(16);
      }
    }
    if (bypassRender == false)
    {
      const PxSize2D windowSizePx = GetWindowSizePx();

      float col[] = {0.0f, 0.0f, 0.0f, 0.5f};

      float colorStroke1[] = {1.0f, 1.0f, 1.0f, 0.5f};
      float colorStroke2[] = {1.0f, 1.0f, 1.0f, 0.1f};

      vgSetfv(VG_CLEAR_COLOR, 4, col);
      vgClear(0, 0, windowSizePx.Width(), windowSizePx.Height());

      switch (m_config.GetType())
      {
      case 0:
        DrawStrokedQuadricSpiral(m_test, m_config.GetQuadricSpiralStrokeLineWidth(), m_stroke2, colorStroke2, m_config.GetQuadricSpiralLayers(),
                                 m_angle);
        break;
      case 1:
        DrawStrokedQuadricSpiral(m_test, m_config.GetQuadricSpiralStrokeLineWidth(), m_imgPaint, colorStroke2, m_config.GetQuadricSpiralLayers(),
                                 m_angle);
        break;
      case 2:
      case 3:
        DrawSegmentedSpiral(m_paths, m_config.GetSegmentedSpiralStrokeLineWidth(), m_stroke, m_fill, colorStroke1);
        break;
      }
      vgFinish();    // execute all in pipe before presentation - workaround for eglSwapBuffer not flushing pipe
    }
  }
}
