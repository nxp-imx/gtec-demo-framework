#ifndef OPENVG_VGSTRESSTEST_CONFIGSPIRAL_HPP
#define OPENVG_VGSTRESSTEST_CONFIGSPIRAL_HPP
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

namespace Fsl
{
  class ConfigSpiral
  {
    int m_type;
    int m_quadricSpiralLayers;
    int m_quadricSpiralRevolutionCount;
    float m_quadricSpiralRevolutionChange;
    float m_quadricSpiralStrokeLineWidth;
    int m_segmentedSpiralRevolutionCount;
    float m_segmentedSpiralRevolutionChange;
    float m_segmentedSpiralStrokeLineWidth;
    int m_segmentedSpiralSegmentsPerRevolution;
    bool m_adaptToScreen;
    bool m_toggleMinMax;

  public:
    ConfigSpiral();

    int GetType() const;
    void SetType(const int value);

    int GetQuadricSpiralLayers() const;
    void SetQuadricSpiralLayers(const int value);

    int GetQuadricSpiralRevolutionCount() const;
    void SetQuadricSpiralRevolutionCount(const int value);

    float GetQuadricSpiralRevolutionChange() const;
    float GetQuadricSpiralRevolutionChange(const int scrWidth, const int scrHeight) const;
    void SetQuadricSpiralRevolutionChange(const float value);

    float GetQuadricSpiralStrokeLineWidth() const;
    void SetQuadricSpiralStrokeLineWidth(const float value);

    int GetSegmentedSpiralRevolutionCount() const;
    void SetSegmentedSpiralRevolutionCount(const int value);

    float GetSegmentedSpiralRevolutionChange() const;
    float GetSegmentedSpiralRevolutionChange(const int scrWidth, const int scrHeight) const;
    void SetSegmentedSpiralRevolutionChange(const float value);

    float GetSegmentedSpiralStrokeLineWidth() const;
    void SetSegmentedSpiralStrokeLineWidth(const float value);

    int GetSegmentedSpiralSegmentsPerRevolution() const;
    void SetSegmentedSpiralSegmentsPerRevolution(const int value);

    bool GetAdaptToScreen() const;
    void SetAdaptToScreen(const bool value);

    bool GetToggleMinMax() const;
    void SetToggleMinMax(const bool value);

  };

}

#endif
