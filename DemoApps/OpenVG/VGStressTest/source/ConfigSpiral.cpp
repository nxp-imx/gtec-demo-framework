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

#include "ConfigSpiral.hpp"
#include <algorithm>
#include <cassert>

#define TYPE_COUNT 4

namespace Fsl
{
  ConfigSpiral::ConfigSpiral()
    : m_type(3)
    , m_quadricSpiralLayers(50) // 50
    , m_quadricSpiralRevolutionCount(18) // 18
    , m_quadricSpiralRevolutionChange(40.0f)
    , m_quadricSpiralStrokeLineWidth(30.0f)
    , m_segmentedSpiralRevolutionCount(36) // 36
    , m_segmentedSpiralRevolutionChange(20.0f)
    , m_segmentedSpiralStrokeLineWidth(50.0f) // 50
    , m_segmentedSpiralSegmentsPerRevolution(6) // 6
    , m_adaptToScreen(false)
    , m_toggleMinMax(false)
  {
  }

  int ConfigSpiral::GetType() const
  {
    return m_type;
  }

  void ConfigSpiral::SetType(const int value)
  {
    m_type = std::max(std::min(value, TYPE_COUNT - 1), 0);
  }

  int ConfigSpiral::GetQuadricSpiralLayers() const
  {
    return m_quadricSpiralLayers;
  }

  void ConfigSpiral::SetQuadricSpiralLayers(const int value)
  {
    m_quadricSpiralLayers = std::max(value, 1);
  }

  int ConfigSpiral::GetQuadricSpiralRevolutionCount() const
  {
    return m_quadricSpiralRevolutionCount;
  }

  void ConfigSpiral::SetQuadricSpiralRevolutionCount(const int value)
  {
    m_quadricSpiralRevolutionCount = std::max(value, 1);
  }

  float ConfigSpiral::GetQuadricSpiralRevolutionChange() const
  {
    return m_quadricSpiralRevolutionChange;
  }


  float ConfigSpiral::GetQuadricSpiralRevolutionChange(const int scrWidth, const int scrHeight) const
  {
    const float minDiv2 = (std::max(scrWidth, scrHeight) / 2) - m_quadricSpiralStrokeLineWidth;
    return (m_adaptToScreen ? minDiv2 / m_quadricSpiralRevolutionCount : m_quadricSpiralRevolutionChange);
  }

  void ConfigSpiral::SetQuadricSpiralRevolutionChange(const float value)
  {
    m_quadricSpiralRevolutionChange = std::max(value, 0.0f);
  }

  float ConfigSpiral::GetQuadricSpiralStrokeLineWidth() const
  {
    return m_quadricSpiralStrokeLineWidth;
  }

  void ConfigSpiral::SetQuadricSpiralStrokeLineWidth(const float value)
  {
    m_quadricSpiralStrokeLineWidth = std::max(value, 0.0f);
  }


  int ConfigSpiral::GetSegmentedSpiralRevolutionCount() const
  {
    return m_segmentedSpiralRevolutionCount;
  }

  void ConfigSpiral::SetSegmentedSpiralRevolutionCount(const int value)
  {
    m_segmentedSpiralRevolutionCount = std::max(value, 1);
  }

  float ConfigSpiral::GetSegmentedSpiralRevolutionChange() const
  {
    return m_segmentedSpiralRevolutionChange;
  }

  float ConfigSpiral::GetSegmentedSpiralRevolutionChange(const int scrWidth, const int scrHeight) const
  {
    const float minDiv2 = (std::min(scrWidth, scrHeight) / 2) - m_segmentedSpiralStrokeLineWidth;
    return (m_adaptToScreen ? minDiv2 / m_segmentedSpiralRevolutionCount : m_segmentedSpiralRevolutionChange);
  }

  void ConfigSpiral::SetSegmentedSpiralRevolutionChange(const float value)
  {
    m_segmentedSpiralRevolutionChange = std::max(value, 0.0f);
  }


  float ConfigSpiral::GetSegmentedSpiralStrokeLineWidth() const
  {
    return m_segmentedSpiralStrokeLineWidth;
  }

  void ConfigSpiral::SetSegmentedSpiralStrokeLineWidth(const float value)
  {
    m_segmentedSpiralStrokeLineWidth = std::max(value, 0.0f);
  }

  int ConfigSpiral::GetSegmentedSpiralSegmentsPerRevolution() const
  {
    return m_segmentedSpiralSegmentsPerRevolution;
  }

  void ConfigSpiral::SetSegmentedSpiralSegmentsPerRevolution(const int value)
  {
    m_segmentedSpiralSegmentsPerRevolution = std::max(value, 2);
  }

  bool ConfigSpiral::GetAdaptToScreen() const
  {
    return m_adaptToScreen;
  }

  void ConfigSpiral::SetAdaptToScreen(const bool value)
  {
    m_adaptToScreen = value;
  }

  bool ConfigSpiral::GetToggleMinMax() const
  {
    return m_toggleMinMax;
  }

  void ConfigSpiral::SetToggleMinMax(const bool value)
  {
    m_toggleMinMax = value;
  }
}
