/****************************************************************************************************************************************************
 * IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 *
 * By downloading, copying, installing or using the software you agree to this license.
 * If you do not agree to this license, do not download, install,
 * copy or use the software.
 *
 *
 *         * Freescale's OpenCL SDK License Agreement
 *
 *         * Copyright (c) 2024 Freescale Semiconductor, Inc.
 *         * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 *    following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRIC LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

// for use of rgb_to_hsv_conversions

float MAX(float a, float b, float c)
{
  float res;

  if (a > b)
    res = a;
  else
    res = b;

  if (c > res)
    res = c;

  return res;
}

float MIN(float a, float b, float c)
{
  float res;

  if (a < b)
    res = a;
  else
    res = b;

  if (c < res)
    res = c;

  return res;
}

// RGB888
// H range 0 to 180 degrees
// S from 0 to 100
// v from 0 to 254
__kernel void rgb_to_hsv_conversions(__global uchar* input, __global uchar* output, int width, int height, uchar cvtcolor_flag

)
{
  int x = get_global_id(0);
  int y = get_global_id(1);
  int id = (y * width) + x;

  float min;
  float max;
  float h, s, v;
  float r, g, b;
  float delta;

  int i = id * 3;

  // RGB to HSV
  if (cvtcolor_flag == 0)
  {
    r = input[i];
    g = input[i + 1];
    b = input[i + 2];

    min = MIN(r, g, b);
    max = MAX(r, g, b);

    v = max;    // v
    delta = max - min;

    if (max != 0)
      s = delta / max;    // s
    else
    {
      // r = g = b = 0		// s = 0, v is undefined
      s = 0;
      h = -1;
      return;
    }

    if (r == max)
    {
      h = 60.0 * (g - b) / delta;

      if (h < 0)
        h += 360.0;
    }

    else if (g == max)
    {
      h = 120.0 + (60.0 * (b - r) / delta);
    }

    else
    {
      h = 240.0 + (60.0 * (r - g) / delta);
    }


    h = h / 2.0;
    // h = 180.0 - h;

    s = s * 100.0;
    // v = v / 254 * 100.0;

    output[i] = (uchar)round(h);
    output[i + 1] = (uchar)round(s);
    output[i + 2] = (uchar)round(v);
  }


  // HSV to RGB
  if (cvtcolor_flag == 1)
  {
    h = input[i];
    s = input[i + 1];
    v = input[i + 2];

    int aux;
    float f, p, q, t;

    h = h * 2.0;
    // h = 180 - h;
    s = s / 100.0;
    // v = v * 254.0 / 100.0;

    h = h / 60.0;    // sector 0 to 5
    aux = floor(h);
    f = h - aux;    // factorial part of h

    // calculate the colors
    p = v * (1.0 - s);
    q = v * (1.0 - s * f);
    t = v * (1.0 - (s * (1.0 - f)));

    switch (aux)
    {
    case 0:
      r = v;
      g = t;
      b = p;
      break;

    case 1:
      r = q;
      g = v;
      b = p;
      break;

    case 2:
      r = p;
      g = v;
      b = t;
      break;

    case 3:
      r = p;
      g = q;
      b = v;
      break;

    case 4:
      r = t;
      g = p;
      b = v;
      break;

    default:
      r = v;
      g = p;
      b = q;
      break;
    }

    output[i] = (uchar)round(r);
    output[i + 1] = (uchar)round(g);
    output[i + 2] = (uchar)round(b);
  }
}
