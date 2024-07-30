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

#include "Example2.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/OpenVG/VGCheck.hpp>
#include <array>
#include <cmath>
#include <iostream>


namespace Fsl
{
  using namespace OpenVG;
  namespace
  {
    const std::vector<VGubyte> g_vgTriangleSegments = {
      VG_MOVE_TO_ABS,
      VG_LINE_TO_ABS,
      VG_LINE_TO_ABS,
      VG_CLOSE_PATH,
    };

    const std::vector<VGubyte> g_vgQuadCurveSegments = {
      VG_MOVE_TO_ABS,
      VG_LINE_TO_ABS,
      VG_QUAD_TO_ABS,
      VG_CLOSE_PATH,
    };

    const std::vector<VGubyte> g_vgCubicCurveSegments = {
      VG_MOVE_TO_ABS, VG_LINE_TO_ABS, VG_CUBIC_TO_ABS, VG_LINE_TO_ABS, VG_CLOSE_PATH,
    };

    const std::vector<VGubyte> g_vgArcSegments = {
      VG_MOVE_TO_ABS,
      VG_LINE_TO_ABS,
      VG_SCCWARC_TO_ABS,
      VG_CLOSE_PATH,
    };
  }


  Example2::Example2(const DemoAppConfig& config)
    : DemoAppVG(config)
  {
    std::array<VGfloat, 4> afClearColour = {0.6f, 0.8f, 1.0f, 1.0f};
    vgSetfv(VG_CLEAR_COLOR, UncheckedNumericCast<VGint>(afClearColour.size()), afClearColour.data());
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();

    const PxSize2D currentSizePx = GetWindowSizePx();
    float scaleX = static_cast<float>(currentSizePx.RawWidth()) / 1280.0f;
    float scaleY = static_cast<float>(currentSizePx.RawHeight()) / 1080.0f;
    std::vector<Vector2> vgTrianglePoints;
    std::vector<Vector2> vgQuadCurvePoints;
    std::vector<Vector2> vgCubicCurvePoints;
    std::vector<VGfloat> vgArcPoints;

    vgTrianglePoints.emplace_back(100.0f * scaleX, 100.0f * scaleY);
    vgTrianglePoints.emplace_back(300.0f * scaleX, 300.0f * scaleY);
    vgTrianglePoints.emplace_back(500.0f * scaleX, 100.0f * scaleY);

    vgQuadCurvePoints.emplace_back(100.0f * scaleX, 400.0f * scaleY);
    vgQuadCurvePoints.emplace_back(100.0f * scaleX, 600.0f * scaleY);
    vgQuadCurvePoints.emplace_back(300.0f * scaleX, 700.0f * scaleY);
    vgQuadCurvePoints.emplace_back(500.0f * scaleX, 600.0f * scaleY);

    vgCubicCurvePoints.emplace_back(620.0f * scaleX, 400.0f * scaleY);
    vgCubicCurvePoints.emplace_back(620.0f * scaleX, 500.0f * scaleY);
    vgCubicCurvePoints.emplace_back(680.0f * scaleX, 700.0f * scaleY);
    vgCubicCurvePoints.emplace_back(700.0f * scaleX, 200.0f * scaleY);
    vgCubicCurvePoints.emplace_back(800.0f * scaleX, 500.0f * scaleY);
    vgCubicCurvePoints.emplace_back(800.0f * scaleX, 400.0f * scaleY);


    vgArcPoints.push_back(620.0f * scaleX);
    vgArcPoints.push_back(200.0f * scaleY);

    vgArcPoints.push_back(620.0f * scaleX);
    vgArcPoints.push_back(100.0f * scaleY);


    vgArcPoints.push_back(10.0f);
    vgArcPoints.push_back(10.0f);
    vgArcPoints.push_back(0.0f);


    vgArcPoints.push_back(700.0f * scaleX);
    vgArcPoints.push_back(150.0f * scaleY);


    // create path
    m_vgTrianglePath.Reset(vgTrianglePoints, g_vgTriangleSegments);
    m_vgQuadPath.Reset(vgQuadCurvePoints, g_vgQuadCurveSegments);
    m_vgCubicPath.Reset(vgCubicCurvePoints, g_vgCubicCurveSegments);
    m_vgArcPath.Reset(vgArcPoints, g_vgArcSegments);

    // create paint
    m_vgTrianglePaint = vgCreatePaint();
    vgSetParameteri(m_vgTrianglePaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetColor(m_vgTrianglePaint, 0xFF0000FF);
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();

    m_vgQuadPaint = vgCreatePaint();
    vgSetParameteri(m_vgQuadPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetColor(m_vgQuadPaint, 0x00FF00FF);
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();

    m_vgCubicPaint = vgCreatePaint();
    vgSetParameteri(m_vgCubicPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetColor(m_vgCubicPaint, 0x0000FFFF);
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();

    m_vgArcPaint = vgCreatePaint();
    vgSetParameteri(m_vgArcPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetColor(m_vgArcPaint, 0x00FFFFFF);
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();

    m_vgStrokePaint = vgCreatePaint();
    vgSetParameteri(m_vgStrokePaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);

    vgSetColor(m_vgStrokePaint, 0xFFFFFFFF);
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
  }


  Example2::~Example2() = default;


  void Example2::Update(const DemoTime& demoTime)
  {
  }


  void Example2::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    vgClear(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());

    // Set transformation matrix mode
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    // Loads identity matrix
    vgLoadIdentity();

    vgSetPaint(m_vgTrianglePaint, VG_FILL_PATH);
    vgSetf(VG_STROKE_LINE_WIDTH, 10.0);
    vgSetPaint(m_vgStrokePaint, VG_STROKE_PATH);
    vgDrawPath(m_vgTrianglePath.GetHandle(), VG_STROKE_PATH | VG_FILL_PATH);

    vgSetPaint(m_vgQuadPaint, VG_FILL_PATH);
    vgSetf(VG_STROKE_LINE_WIDTH, 5.0);
    vgDrawPath(m_vgQuadPath.GetHandle(), VG_STROKE_PATH | VG_FILL_PATH);

    vgSetPaint(m_vgCubicPaint, VG_FILL_PATH);
    vgSetf(VG_STROKE_LINE_WIDTH, 15.0);
    vgDrawPath(m_vgCubicPath.GetHandle(), VG_STROKE_PATH | VG_FILL_PATH);

    vgSetPaint(m_vgArcPaint, VG_FILL_PATH);
    vgSetf(VG_STROKE_LINE_WIDTH, 20.0);
    vgDrawPath(m_vgArcPath.GetHandle(), VG_STROKE_PATH | VG_FILL_PATH);

    vgFinish();
  }
}
