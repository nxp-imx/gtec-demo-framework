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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include "FurTexture.hpp"

namespace Fsl
{

  std::vector<uint8_t> FurTexture::GenerateWave(const int width, const int height, const float density, const int numLayers)
  {
    //read the width and height of the texture
    int totalPixels = width * height;
    const int cbArray = totalPixels * 4;

    //an array to hold our pixels
    std::vector<uint8_t> colors(cbArray);

    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        float max_layer_n = 0.2f + 0.8f * (std::sin(x / (float)height * 20.0f) / 2.0f + 0.5f);

        const int dstPixelIndex = (y * width * 4) + (x * 4);
        assert((dstPixelIndex % 4) == 0);
        colors[dstPixelIndex + 0] = uint8_t(max_layer_n * 255);
        colors[dstPixelIndex + 1] = uint8_t(max_layer_n * 255);
        colors[dstPixelIndex + 2] = uint8_t(max_layer_n * 255);
        colors[dstPixelIndex + 3] = 255;
      }
    }
    return colors;
  }


  std::vector<uint8_t> FurTexture::Generate(const int width, const int height, const float density, const int numLayers)
  {
    //read the width and height of the texture
    int totalPixels = width * height;

    const int cbArray = totalPixels * 4;

    //an array to hold our pixels
    std::vector<uint8_t> colors(cbArray);

    //initialize all pixels to transparent black
    std::fill(colors.begin(), colors.end(), 0);

    //compute the number of opaque pixels = nr of hair strands
    const int nrStrands = (int)(density * totalPixels);

    //compute the number of strands that stop at each layer
    const int strandsPerLayer = std::max(nrStrands / numLayers, 1);

    //srand (static_cast<unsigned int>(time(nullptr)));
    srand(1337); // We want to utilize the same density pattern on each run!

    //fill texture with opaque pixels
    for (int i = 0; i < nrStrands; i++)
    {
      int x, y;
      //random position on the texture
      x = rand() % width;
      y = rand() % height;

      //compute max layer
      int max_layer = i / strandsPerLayer;
      //normalize into [0..1] range
      float max_layer_n = (float)max_layer / (float)numLayers;


      //max_layer_n = 0.2f + 0.8f * (std::sin(x / (float)height * 20.0f) / 2.0f + 0.5f);
      //{
      //  Vector2 dist((float)x / height - 0.5f, (float)y / width - 0.5f);
      //  max_layer_n = 0.4f + 0.6f * (std::cos(dist.Length() * 50.0f) / 2.0f + 0.5f);
      //}

      //put color (which has an alpha value of 255, i.e. opaque)
      //max_layer_n needs to be multiplied by 255 to achieve a color in [0..255] range
      max_layer_n = std::max(std::min(max_layer_n, 1.0f), 0.0f);
      const int dstPixelIndex = (y * width * 4) + (x * 4);
      assert((dstPixelIndex % 4) == 0);
      colors[dstPixelIndex + 0] = uint8_t(max_layer_n * 255);
      colors[dstPixelIndex + 1] = uint8_t(max_layer_n * 255);
      colors[dstPixelIndex + 2] = uint8_t(max_layer_n * 255);
      colors[dstPixelIndex + 3] = 255;
    }
    return colors;
  }




  std::vector<uint8_t> FurTexture::GenerateSmooth(const int width, const int height, const float density, const int numLayers)
  {
    //read the width and height of the texture
    int totalPixels = width * height;

    const int cbArray = totalPixels * 4;

    //an array to hold our pixels
    std::vector<uint8_t> colors(cbArray);

    //initialize all pixels to transparent black
    std::fill(colors.begin(), colors.end(), 0);

    //compute the number of opaque pixels = nr of hair strands
    const int nrStrands = (int)(density * totalPixels);

    //compute the number of strands that stop at each layer
    const int strandsPerLayer = std::max(nrStrands / numLayers, 1);

    //srand (static_cast<unsigned int>(time(nullptr)));
    srand(1337); // We want to utilize the same density pattern on each run!

    const float scale = 0.8f;

    //fill texture with opaque pixels
    for (int i = 0; i < nrStrands; i++)
    {
      int x, y;
      //random position on the texture
      x = rand() % width;
      y = rand() % height;

      //compute max layer
      int max_layer = i / strandsPerLayer;
      //normalize into [0..1] range
      float max_layer_n = (float)max_layer / (float)numLayers;


      //max_layer_n = 0.2f + 0.8f * (std::sin(x / (float)height * 20.0f) / 2.0f + 0.5f);
      //{
      //  Vector2 dist((float)x / height - 0.5f, (float)y / width - 0.5f);
      //  max_layer_n = 0.4f + 0.6f * (std::cos(dist.Length() * 50.0f) / 2.0f + 0.5f);
      //}

      //put color (which has an alpha value of 255, i.e. opaque)
      //max_layer_n needs to be multiplied by 255 to achieve a color in [0..255] range
      max_layer_n = std::max(std::min(max_layer_n, 1.0f), 0.0f);
      {
        const int dstPixelIndex = (y * width * 4) + (x * 4);
        assert((dstPixelIndex % 4) == 0);
        colors[dstPixelIndex + 0] = uint8_t(max_layer_n * 255);
        colors[dstPixelIndex + 1] = uint8_t(max_layer_n * 255);
        colors[dstPixelIndex + 2] = uint8_t(max_layer_n * 255);
        colors[dstPixelIndex + 3] = 255;
      }
      //if (x > 0)
      //{
      //  const int dstPixelIndex = (y * width * 4) + ((x-1) * 4);
      //  assert((dstPixelIndex % 4) == 0);
      //  colors[dstPixelIndex + 0] = uint8_t(max_layer_n * 255 * scale);
      //  colors[dstPixelIndex + 1] = uint8_t(max_layer_n * 255 * scale);
      //  colors[dstPixelIndex + 2] = uint8_t(max_layer_n * 255 * scale);
      //  colors[dstPixelIndex + 3] = 255;
      //}
      //if (x < (width-1))
      //{
      //  const int dstPixelIndex = (y * width * 4) + ((x + 1) * 4);
      //  assert((dstPixelIndex % 4) == 0);
      //  colors[dstPixelIndex + 0] = uint8_t(max_layer_n * 255 * scale);
      //  colors[dstPixelIndex + 1] = uint8_t(max_layer_n * 255 * scale);
      //  colors[dstPixelIndex + 2] = uint8_t(max_layer_n * 255 * scale);
      //  colors[dstPixelIndex + 3] = 255;
      //}
      if (y > 0)
      {
        const int dstPixelIndex = ((y-1) * width * 4) + (x * 4);
        assert((dstPixelIndex % 4) == 0);
        colors[dstPixelIndex + 0] = uint8_t(max_layer_n * 255 * scale);
        colors[dstPixelIndex + 1] = uint8_t(max_layer_n * 255 * scale);
        colors[dstPixelIndex + 2] = uint8_t(max_layer_n * 255 * scale);
        colors[dstPixelIndex + 3] = 255;
      }
      if (y < (height - 1))
      {
        const int dstPixelIndex = ((y+1) * width * 4) + (x * 4);
        assert((dstPixelIndex % 4) == 0);
        colors[dstPixelIndex + 0] = uint8_t(max_layer_n * 255 * 0.75f);
        colors[dstPixelIndex + 1] = uint8_t(max_layer_n * 255 * 0.75f);
        colors[dstPixelIndex + 2] = uint8_t(max_layer_n * 255 * 0.75f);
        colors[dstPixelIndex + 3] = 255;
      }
    }
    return colors;
  }
}
