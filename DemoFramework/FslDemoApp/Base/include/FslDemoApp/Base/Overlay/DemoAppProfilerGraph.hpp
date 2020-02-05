#ifndef FSLDEMOAPP_BASE_OVERLAY_DEMOAPPPROFILERGRAPH_HPP
#define FSLDEMOAPP_BASE_OVERLAY_DEMOAPPPROFILERGRAPH_HPP
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

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Collections/CircularFixedSizeBuffer.hpp>
#include <memory>
#include <vector>

namespace Fsl
{
  class IBasic2D;
  struct Color;
  struct Vector2;

  class DemoAppProfilerGraph
  {
    int32_t m_minValue;
    int32_t m_maxValue;
    Point2 m_size;
    CircularFixedSizeBuffer<int32_t> m_entries;
    std::vector<Vector2> m_coords;

  public:
    DemoAppProfilerGraph(const int32_t minValue, const int32_t maxValue, const Point2& size);

    Point2 GetSize() const;
    void Add(const int32_t latestValue);
    void Draw(const std::shared_ptr<IBasic2D>& basic2D, const Vector2& dstPosition, const Color& color);
  };
}

#endif
