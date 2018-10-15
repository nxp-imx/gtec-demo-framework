#ifndef OPENVG_SIMPLEBENCH_POINTBENCH_HPP
#define OPENVG_SIMPLEBENCH_POINTBENCH_HPP
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

#include <FslUtil/OpenVG/VGImageBuffer.hpp>
#include <FslUtil/OpenVG/VGPaintBuffer.hpp>
#include <FslUtil/OpenVG/VGPathBuffer.hpp>
#include <FslUtil/OpenVG/VGFontBuffer.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslGraphics/Color.hpp>
#include "IBench.hpp"
#include <vector>

namespace Fsl
{
  //! @brief The point benchmark tests various strategies for drawing points.
  class PointBench : public IBench
  {
  public:
    struct Mode
    {
      enum Enum
      {
        // Use vgClear
        Clear,
        // use a single line path, moving it around
        Line,
        // create a new path
        NewPath,
        // ModifyPath,
        // Draw using one bitmap of the right color
        Bitmap,
        // Utilize a bitmap font containing bitmap glyphs containing one pixel.
        // use the x,y adjust arrays to position the glyph
        BitmapFont
      };
    };

    PointBench(const DemoAppConfig& config, const int32_t pointCount, const Mode::Enum mode);
    ~PointBench() override;

    std::string GetName() const override;
    void Restart() override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const Point2& screenResolution) override;

  private:
    int32_t m_pointCount;
    Mode::Enum m_mode;
    uint32_t m_index;
    std::vector<Color> m_colors;
    std::vector<Vector4> m_colorsV4;
    OpenVG::VGImageBuffer m_imageColorParent;
    OpenVG::VGImageBuffer m_imageColors[8];
    OpenVG::VGPathBuffer m_buffer;
    OpenVG::VGPaintBuffer m_paint;
    std::vector<VGfloat> m_pathCoords;
    std::vector<VGubyte> m_pathSegments;
    VGPath m_hPath;
    OpenVG::VGFontBuffer m_fontBuffer;
    std::vector<VGuint> m_fontGlyphs;
  };
}

#endif
