/*
* Copyright (c) 2012 Freescale Semiconductor, Inc.
*/

/****************************************************************************
*
*    Copyright 2012 Vivante Corporation, Sunnyvale, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#include <Shared/EnvironmentMapping/SphereMeshCreator.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <cassert>


namespace Fsl
{
  //! @brief Code extracted from the S07, S08 samples.
  void SphereMeshCreator::Create(std::vector<VertexPosition>& rSphereVertices, std::vector<uint16_t>& rIndices, const int width, const int height)
  {
    int x, y, icnt = 0;
    int offset;

    rSphereVertices.resize(width*height);

    int dstIndex = 0;
    for (y = 0; y < height; ++y)
    {
      float normY = (float)y / (float)(height - 1);
      float polar = MathHelper::PI * normY;
      float polarSin = (float)std::sin(polar);
      float polarCos = (float)std::cos(polar);

      for (x = 0; x < width; ++x)
      {
        float normX = (float)x / (float)(width - 1);
        float azim = 2.0f * MathHelper::PI * normX;
        float azimSin = (float)std::sin(azim);
        float azimCos = (float)std::cos(azim);
        rSphereVertices[dstIndex].Position = Vector3(polarSin * azimCos, polarSin * azimSin, polarCos);
        ++dstIndex;
      }
    }

    offset = 0;
    rIndices.resize((width - 1)*(height - 1) * 2 * 3);
    uint16_t* indexArray = rIndices.data();

    for (y = 0; y < height - 1; ++y)
    {
      for (x = 1; x < width; ++x)
      {
        indexArray[icnt++] = uint16_t(offset + x - 1 + width);
        indexArray[icnt++] = uint16_t(offset + x - 1);
        indexArray[icnt++] = uint16_t(offset + x);
        indexArray[icnt++] = uint16_t(offset + x + width);
        indexArray[icnt++] = uint16_t(offset + x - 1 + width);
        indexArray[icnt++] = uint16_t(offset + x);
      }
      offset += width;
    }
  }
}
