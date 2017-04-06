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
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include "PointBench.hpp"
#include <VG/openvg.h>
#include <cassert>
#include <sstream>

namespace Fsl
{
  // FIX: The modify path code is not correct as the path length could change :/
  namespace
  {
    const int32_t POINTS_PER_LINE = 200;

    void BenchPointsViaClear(const uint32_t index, const int32_t count, const int32_t width, const std::vector<Vector4>& colors)
    {
      const std::size_t colorCount = colors.size();
      std::size_t colorIndex = index % colorCount;
      vgSetfv(VG_CLEAR_COLOR, 4, colors[colorIndex].DirectAccess());

      uint32_t offset = index & 1;
      int x = 0;
      int y = 0;
      for (int i = 0; i < count; ++i)
      {
        vgClear(x, y, 1, 1);

        x += 2;
        if (x >= width)
        {
          ++offset;
          x = offset & 1;
          ++y;
          colorIndex = (colorIndex + 1) % colorCount;
          vgSetfv(VG_CLEAR_COLOR, 4, colors[colorIndex].DirectAccess());
        }
      }
    }


    void BenchPointsViaLine(const uint32_t index, const int32_t count, const int32_t width, const VGPath& hLine,
                          const VGPaint& hPaint, const std::vector<Vector4>& colors)
    {
      const std::size_t colorCount = colors.size();
      std::size_t colorIndex = index % colorCount;
      vgSetParameterfv(hPaint, VG_PAINT_COLOR, 4, colors[colorIndex].DirectAccess());

      vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
      vgSetf(VG_STROKE_LINE_WIDTH, 1);
      vgSetPaint(hPaint, VG_STROKE_PATH);

      uint32_t offset = index & 1;
      int x = 0;
      int y = 0;
      for (int i = 0; i < count; ++i)
      {
        {
          vgLoadIdentity();
          vgTranslate((float)x, (float)y);
          vgDrawPath(hLine, VG_STROKE_PATH);
        }
        x += 2;
        if (x >= width)
        {
          ++offset;
          x = offset & 1;
          ++y;
          colorIndex = (colorIndex + 1) % colorCount;
          vgSetParameterfv(hPaint, VG_PAINT_COLOR, 4, colors[colorIndex].DirectAccess());
        }
      }
    }


    void BenchPointsViaNewPath(const uint32_t index, const int32_t count, const int32_t width, const VGPaint& hPaint, const std::vector<Vector4>& colors,
                             std::vector<VGfloat>& rPathCoords, std::vector<VGubyte>& rPathSegments)
    {
      const float bias = 0.0f;
      const float scale = 1.0f;

      const std::size_t colorCount = colors.size();
      std::size_t colorIndex = index % colorCount;
      vgSetParameterfv(hPaint, VG_PAINT_COLOR, 4, colors[colorIndex].DirectAccess());

      vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
      vgSetf(VG_STROKE_LINE_WIDTH, 1);
      vgSetPaint(hPaint, VG_STROKE_PATH);
      vgLoadIdentity();

      int dstIndex = 0;
      VGfloat* pDstCoords = rPathCoords.data();

      uint32_t offset = index & 1;
      int x = 0;
      VGfloat y = 0.0f;
      for (int i = 0; i < count; ++i)
      {
        {
          pDstCoords[dstIndex] = (VGfloat)x;
          pDstCoords[dstIndex + 1] = y;
          pDstCoords[dstIndex + 2] = (VGfloat)x + 1;
          pDstCoords[dstIndex + 3] = y;
          dstIndex += 4;
        }
        x += 2;
        if (x >= width)
        {
          ++offset;
          x = offset & 1;
          ++y;

          {
            VGPath hPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, scale, bias, dstIndex / 2, dstIndex, VG_PATH_CAPABILITY_APPEND_TO);
            vgAppendPathData(hPath, dstIndex / 2, rPathSegments.data(), pDstCoords);
            vgDrawPath(hPath, VG_STROKE_PATH);
            vgDestroyPath(hPath);
            dstIndex = 0;
          }
          colorIndex = (colorIndex + 1) % colorCount;
          vgSetParameterfv(hPaint, VG_PAINT_COLOR, 4, colors[colorIndex].DirectAccess());
        }
      }
      if (dstIndex > 0)
      {
        VGPath hPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, scale, bias, dstIndex / 2, dstIndex, VG_PATH_CAPABILITY_APPEND_TO);
        vgAppendPathData(hPath, dstIndex / 2, rPathSegments.data(), pDstCoords);
        vgDrawPath(hPath, VG_STROKE_PATH);
        vgDestroyPath(hPath);
      }

    }


    //void BenchPointsViaModifyPath(const uint32_t index, const int32_t count, const int32_t width, const VGPath hPath, const VGPaint& hPaint, const std::vector<Vector4>& colors,
    //  std::vector<VGfloat>& rPathCoords, std::vector<VGubyte>& rPathSegments)
    //{
    //  const float bias = 0.0f;
    //  const float scale = 1.0f;


    //  const std::size_t colorCount = colors.size();
    //  std::size_t colorIndex = index % colorCount;
    //  vgSetParameterfv(hPaint, VG_PAINT_COLOR, 4, colors[colorIndex].DirectAccess());

    //  vgSetf(VG_STROKE_LINE_WIDTH, 1);
    //  vgSetPaint(hPaint, VG_STROKE_PATH);
    //  vgLoadIdentity();

    //  int dstIndex = 0;
    //  VGfloat* pDstCoords = rPathCoords.data();

    //  uint32_t offset = index & 1;
    //  int x = 0;
    //  int y = 0;
    //  for (int i = 0; i < count; ++i)
    //  {
    //    {
    //      pDstCoords[dstIndex] = (VGfloat)x;
    //      pDstCoords[dstIndex + 1] = (VGfloat)y;
    //      pDstCoords[dstIndex + 2] = (VGfloat)x + 1;
    //      pDstCoords[dstIndex + 3] = (VGfloat)y;
    //      dstIndex += 4;
    //    }
    //    x += 2;
    //    if (x >= width)
    //    {
    //      ++offset;
    //      x = offset & 1;
    //      ++y;

    //      {
    //        vgModifyPathCoords(hPath, 0, dstIndex / 2, pDstCoords);
    //        vgDrawPath(hPath, VG_STROKE_PATH);
    //        dstIndex = 0;
    //      }
    //      colorIndex = (colorIndex + 1) % colorCount;
    //      vgSetParameterfv(hPaint, VG_PAINT_COLOR, 4, colors[colorIndex].DirectAccess());
    //    }
    //  }
    //  if ( dstIndex > 0 )
    //  {
    //    vgModifyPathCoords(hPath, 0, dstIndex / 2, pDstCoords);
    //    vgDrawPath(hPath, VG_STROKE_PATH);
    //  }

    //}


    void BenchPointsViaBitmap(const uint32_t index, const int32_t count, const int32_t width, const OpenVG::VGImageBuffer*const pColors, const std::size_t colorCount)
    {
      std::size_t colorIndex = index % colorCount;

      VGImage hCurrentImage = pColors[colorIndex].GetHandle();

      vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

      uint32_t offset = index & 1;
      int x = 0;
      int y = 0;
      for (int i = 0; i < count; ++i)
      {
        {
          vgLoadIdentity();
          vgTranslate((VGfloat)x, (VGfloat)y);
          vgDrawImage(hCurrentImage);
        }
        x += 2;
        if (x >= width)
        {
          ++offset;
          x = offset & 1;
          ++y;
          colorIndex = (colorIndex + 1) % colorCount;
          hCurrentImage = pColors[colorIndex].GetHandle();
        }
      }
    }


    void BenchPointsViaBitmapFont(const uint32_t index, const int32_t count, const int32_t width, const VGFont hFont, const std::vector<Color>& colors, std::vector<VGuint>& glyphs, std::vector<VGfloat>& rPathCoords)
    {
      const std::size_t colorCount = colors.size();
      std::size_t colorIndex = index % colorCount;

      float origin[2] = { 0, 0 };
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);

      int dstIndex = 0;

      VGuint* pGlyphs = glyphs.data();
      VGfloat* pDstCoordsX = rPathCoords.data();
      VGfloat* pDstCoordsY = pDstCoordsX + (rPathCoords.size() / 2);

      uint32_t offset = index & 1;
      int x = 0;
      VGfloat y = 0;
      for (int i = 0; i < count; ++i)
      {
        {
          pDstCoordsX[dstIndex] = 2.0f;
          pDstCoordsY[dstIndex] = 0.0f;
          pGlyphs[dstIndex] = (VGuint)colorIndex;
          ++dstIndex;
        }
        x += 2;
        if (x >= width)
        {
          x = offset & 1;
          {
            vgLoadIdentity();
            vgTranslate((VGfloat)x, y);
            vgSetfv(VG_GLYPH_ORIGIN, 2, origin);
            vgDrawGlyphs(hFont, dstIndex, pGlyphs, pDstCoordsX, pDstCoordsY, VG_FILL_PATH, VG_TRUE);
            dstIndex = 0;
          }
          ++offset;
          ++y;
          colorIndex = (colorIndex + 1) % colorCount;
        }
      }
      if (dstIndex > 0)
      {
        x = offset & 1;
        vgLoadIdentity();
        vgTranslate((VGfloat)x, y);
        vgSetfv(VG_GLYPH_ORIGIN, 2, origin);
        vgDrawGlyphs(hFont, dstIndex, pGlyphs, pDstCoordsX, pDstCoordsY, VG_FILL_PATH, VG_TRUE);
      }
    }
  }


  PointBench::PointBench(const DemoAppConfig& config, const int32_t pointCount, const Mode::Enum mode)
    : m_pointCount(pointCount)
    , m_mode(mode)
    , m_index(0)
    , m_colors(8)
    , m_colorsV4(8)
    , m_buffer()
    , m_paint()
    , m_pathCoords(POINTS_PER_LINE * 4)
    , m_pathSegments(POINTS_PER_LINE * 2)
    , m_hPath(VG_INVALID_HANDLE)
    , m_fontBuffer()
    , m_fontGlyphs(POINTS_PER_LINE)
  {
    Bitmap bitmap(8, 1, PixelFormat::R8G8B8A8_UNORM);

    m_colors[0] = Color::White();
    m_colors[1] = Color::Red();
    m_colors[2] = Color::Green();
    m_colors[3] = Color::Blue();
    m_colors[4] = Color(0xFF808080);
    m_colors[5] = Color(0xFFFF00FF);
    m_colors[6] = Color(0xFF00FFFF);
    m_colors[7] = Color(0xFFFFFF00);


    const auto colorCount = static_cast<uint32_t>(m_colors.size());
    for (uint32_t i = 0; i < colorCount; ++i)
    {
      m_colorsV4[i] = m_colors[i].ToVector4();
      bitmap.SetNativePixel(i, 0u, m_colors[i].PackedValue());
    }
    m_imageColorParent.Reset(bitmap, VG_IMAGE_QUALITY_FASTER);
    for (uint32_t i = 0; i < colorCount; ++i)
    {
      m_imageColors[i].Reset(vgChildImage(m_imageColorParent.GetHandle(), i, 0, 1, 1), Point2(1,1));
    }

    m_paint.Reset(vgCreatePaint());

    // Create the parent image
    for (std::size_t i = 0; i < m_pathSegments.size(); i += 2)
    {
      m_pathSegments[i] = VG_MOVE_TO_ABS;
      m_pathSegments[i + 1] = VG_LINE_TO_ABS;
    }

    if (m_mode == Mode::Line )
    {
      std::vector<Vector2> vertices(2);
      std::vector<VGubyte> segments(2);
      vertices[0] = Vector2(0, 0);
      vertices[1] = Vector2(1, 0);
      segments[0] = VG_MOVE_TO_ABS;
      segments[1] = VG_LINE_TO_ABS;
      m_buffer.Reset(vertices, segments);
    }

    //if (m_mode == Mode::ModifyPath)
    //{
    //  const float bias = 0.0f;
    //  const float scale = 1.0f;
    //  m_hPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, scale, bias, m_pathSegments.size(), m_pathCoords.size(), VG_PATH_CAPABILITY_APPEND_TO | VG_PATH_CAPABILITY_MODIFY);
    //  vgAppendPathData(m_hPath, m_pathSegments.size(), m_pathSegments.data(), m_pathCoords.data());
    //}
    if (m_mode == Mode::BitmapFont)
    {
      VGfloat zero[2]{};
      m_fontBuffer.Reset(8);
      const VGFont hFont = m_fontBuffer.GetHandle();
      for (uint32_t i = 0; i < colorCount; ++i)
      {
        vgSetGlyphToImage(hFont, i, m_imageColors[i].GetHandle(), zero, zero);
      }
    }
  }


  PointBench::~PointBench()
  {
    if (m_hPath != VG_INVALID_HANDLE)
    {
      vgDestroyPath(m_hPath);
      m_hPath = VG_INVALID_HANDLE;
    }
  }


  std::string PointBench::GetName() const
  {
    std::stringstream buffer;

    switch (m_mode)
    {
    case Mode::Clear:
      buffer << "PointBench_Clear";
      break;
    case Mode::Line:
      buffer << "PointBench_Line";
      break;
    case Mode::NewPath:
      buffer << "PointBench_NewPath";
      break;
    //case Mode::ModifyPath:
    //  buffer << "PointBench_ModifyPath";
    //  break;
    case Mode::Bitmap:
      buffer << "PointBench_Bitmap";
      break;
    case Mode::BitmapFont:
      buffer << "PointBench_BitmapFont";
      break;
    default:
      buffer << "PointBench_UnknownMode";
      break;
    }
    buffer << " (" << m_pointCount << ")";
    return buffer.str();
  }


  void PointBench::Restart()
  {
    m_index = 0;
  }


  void PointBench::Update(const DemoTime& demoTime)
  {
  }


  void PointBench::Draw(const Point2& screenResolution)
  {
    const int32_t width = POINTS_PER_LINE * 2;

    switch (m_mode)
    {
    case Mode::Clear:
      BenchPointsViaClear(m_index, m_pointCount, width, m_colorsV4);
      break;
    case Mode::Line:
      BenchPointsViaLine(m_index, m_pointCount, width, m_buffer.GetHandle(), m_paint.GetHandle(), m_colorsV4);
      break;
    case Mode::NewPath:
      BenchPointsViaNewPath(m_index, m_pointCount, width, m_paint.GetHandle(), m_colorsV4, m_pathCoords, m_pathSegments);
      break;
    //case Mode::ModifyPath:
    //  BenchPointsViaModifyPath(m_index, m_pointCount, width, m_hPath, m_paint.GetHandle(), m_colorsV4, m_pathCoords, m_pathSegments);
    //  break;
    case Mode::Bitmap:
      BenchPointsViaBitmap(m_index, m_pointCount, width, m_imageColors, m_colors.size());
      break;
    case Mode::BitmapFont:
      BenchPointsViaBitmapFont(m_index, m_pointCount, width, m_fontBuffer.GetHandle(), m_colors, m_fontGlyphs, m_pathCoords);
      break;
    default:
      break;
    }
    ++m_index;
  }
}
