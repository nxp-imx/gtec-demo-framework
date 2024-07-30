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
 *         * Copyright (c) 2012 Freescale Semiconductor, Inc.
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

// rgb888
__kernel void rgb_to_gray(__global uchar* input, __global uchar* output, int width, int height)
{
  int x = get_global_id(0);
  int y = get_global_id(1);
  int id = (y * width) + x;

  float r;
  float g;
  float b;
  float value;
  int i;

  i = id * 3;

  r = (uchar)input[i];
  g = (uchar)input[i + 1];
  b = (uchar)input[i + 2];

  // value = (r + b + g) / 3.0;   // average

  value = (0.299 * r) + (0.587 * g) + (0.114 * b);    // weighted

  output[id] = (uchar)round(value);
}

// rgb 888
__kernel void gray_to_rgb(__global uchar* input, __global uchar* output, int width, int height)
{
  int x = get_global_id(0);
  int y = get_global_id(1);
  int id = (y * width) + x;

  int i;

  i = id * 3;

  /* weighted

  float r;
  float g;
  float b;
  float value;

  r = (uchar) input[id] / 0.299;
        g = (uchar) input[id] / 0.587;
        b = (uchar) input[id] / 0.114;

  output[i] = (uchar) round (r);
  output[i + 1] = (uchar) round (g);
  output[i + 2] = (uchar) round (b); */


  output[i] = input[id];
  output[i + 1] = input[id];
  output[i + 2] = input[id];
}


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
__kernel void rgb_to_hsv_conversions(__global uchar* input, __global uchar* output, int width, int height, char cvtcolor_flag

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

__kernel void rgb888_to_rgb565_conversions(__global uchar* input, __global uchar* output, int width, int height, char cvtcolor_flag)
{
  int x = get_global_id(0);
  int y = get_global_id(1);

  int id = (y * width) + x;

  int i = id * 3;
  int j = id * 2;

  uchar r, g, b;

  ushort rgb565 = 0;

  ushort r5_mask = 0xf800;
  ushort g6_mask = 0x07e0;
  ushort b5_mask = 0x001f;

  ushort byte0;
  ushort byte1;

  int m, n, o;

  // RGB888 to RGB565
  if (cvtcolor_flag == 0)
  {
    r = round((input[i] / 255.0) * 31.0);
    g = round((input[i + 1] / 255.0) * 63.0);
    b = round((input[i + 2] / 255.0) * 31.0);

    m = ((r << 11) & r5_mask);
    n = ((g << 5) & g6_mask);
    o = (b & b5_mask);

    rgb565 = m | n | o;

    output[j] = (uchar)((rgb565 >> 8) & 0x00ff);
    output[j + 1] = (uchar)(rgb565 & 0x00ff);
  }

  // RGB565 to RGB888
  if (cvtcolor_flag == 1)
  {
    byte0 = (input[j] << 8);
    byte1 = input[j + 1];

    rgb565 = byte0 | byte1;

    r = (rgb565 & r5_mask) >> 11;
    g = (rgb565 & g6_mask) >> 5;
    b = (rgb565 & b5_mask);

    output[i] = round((r / 31.0) * 255);
    output[i + 1] = round((g / 63.0) * 255);
    output[i + 2] = round((b / 31.0) * 255);
  }
}


float AbsMin(float x, float y, float z)
{
  float res = 0;

  if (z < x)
  {
    res = x;
  }
  else
  {
    if (z > y)
    {
      res = y;
    }
    else
    {
      res = z;
    }
  }

  return res;
}

__kernel void rgb888_to_uyvy_conversions(__global uchar* input, __global uchar* output, int width, int height, char cvtcolor_flag)
{
  int x = get_global_id(0);
  int y = get_global_id(1);

  int id = (y * width) + x;

  int i = id * 12;
  int j = id * 8;

  float r0, g0, b0;
  float r1, g1, b1;
  float r2, g2, b2;
  float r3, g3, b3;


  // RGB888 to UYVY
  if (cvtcolor_flag == 0)
  {
    float Y0, U0;
    float Y1, V1;
    float Y2, U2;
    float Y3, V3;

    r0 = input[i];
    g0 = input[i + 1];
    b0 = input[i + 2];

    r1 = input[i + 3];
    g1 = input[i + 4];
    b1 = input[i + 5];

    r2 = input[i + 6];
    g2 = input[i + 7];
    b2 = input[i + 8];

    r3 = input[i + 9];
    g3 = input[i + 10];
    b3 = input[i + 11];


    // RGB888 to YUV444

    Y0 = (0.299 * r0) + (0.587 * g0) + (0.114 * b0);
    U0 = (-0.169 * r0) - (0.331 * g0) + (0.499 * b0) + 128;
    // V0 = (0.499 * r0) - (0.418 * g0) - (0.0813 * b0) + 128;

    Y1 = (0.299 * r1) + (0.587 * g1) + (0.114 * b1);
    // U1 = (-0.169 * r1) - (0.331 * g1) + (0.499 * b1) + 128;
    V1 = (0.499 * r1) - (0.418 * g1) - (0.0813 * b1) + 128;

    Y2 = (0.299 * r2) + (0.587 * g2) + (0.114 * b2);
    U2 = (-0.169 * r2) - (0.331 * g2) + (0.499 * b2) + 128;
    // V2 = (0.499 * r2) - (0.418 * g2) - (0.0813 * b2) + 128;

    Y3 = (0.299 * r3) + (0.587 * g3) + (0.114 * b3);
    // U3 = (-0.169 * r3) - (0.331 * g3) + (0.499 * b3) + 128;
    V3 = (0.499 * r3) - (0.418 * g3) - (0.0813 * b3) + 128;

    if (Y0 > 255)
      Y0 = 255;
    if (Y0 < 0)
      Y0 = 0;
    if (U0 > 255)
      U0 = 255;
    if (U0 < 0)
      U0 = 0;

    if (Y1 > 255)
      Y1 = 255;
    if (Y1 < 0)
      Y1 = 0;
    if (V1 > 255)
      V1 = 255;
    if (V1 < 0)
      V1 = 0;

    if (Y2 > 255)
      Y2 = 255;
    if (Y2 < 0)
      Y2 = 0;
    if (U2 > 255)
      U2 = 255;
    if (U2 < 0)
      U2 = 0;

    if (Y3 > 255)
      Y3 = 255;
    if (Y3 < 0)
      Y3 = 0;
    if (V3 > 255)
      V3 = 255;
    if (V3 < 0)
      V3 = 0;

    // YUV444 to YUV422 (UYVY 16bit)
    output[j] = round(U0);
    output[j + 1] = round(Y0);
    output[j + 2] = round(V1);
    output[j + 3] = round(Y1);

    output[j + 4] = round(U2);
    output[j + 5] = round(Y2);
    output[j + 6] = round(V3);
    output[j + 7] = round(Y3);
  }

  // UYVY to RGB888
  if (cvtcolor_flag == 1)
  {
    float Y0, U0, V0;
    float Y1, U1, V1;
    float Y2, U2, V2;
    float Y3, U3, V3;

    uchar u0, y0, v1, y1, u2, y2, v3, y3;

    u0 = input[j];
    y0 = input[j + 1];
    v1 = input[j + 2];
    y1 = input[j + 3];

    u2 = input[j + 4];
    y2 = input[j + 5];
    v3 = input[j + 6];
    y3 = input[j + 7];

    // YUV422 to YUV444
    Y0 = y0;
    U0 = u0;
    V0 = v1;

    Y1 = y1;
    U1 = u0;
    V1 = v1;

    Y2 = y2;
    U2 = u2;
    V2 = v3;

    Y3 = y3;
    U3 = u2;
    V3 = v3;

    r0 = (0.997 * Y0) - (0.007 * U0) + (1.405 * V0) - 189.114;
    g0 = Y0 - (0.343 * U0) - (0.716 * V0) + 135.46;
    b0 = (1.006 * Y0) + (1.780 * U0) - 227.81;

    r1 = (0.997 * Y1) - (0.007 * U1) + (1.405 * V1) - 189.114;
    g1 = Y1 - (0.343 * U1) - (0.716 * V1) + 135.46;
    b1 = (1.006 * Y1) + (1.780 * U1) - 227.810;


    r2 = (0.997 * Y2) - (0.007 * U2) + (1.405 * V2) - 189.114;
    g2 = Y2 - (0.343 * U2) - (0.716 * V2) + 135.46;
    b2 = (1.006 * Y2) + (1.780 * U2) - 227.81;

    r3 = (0.997 * Y3) - (0.007 * U3) + (1.405 * V3) - 189.114;
    g3 = Y3 - (0.343 * U3) - (0.716 * V3) + 135.46;
    b3 = (1.006 * Y3) + (1.780 * U3) - 227.81;

    if (r0 > 255)
      r0 = 255;
    if (r0 < 0)
      r0 = 0;
    if (g0 > 255)
      g0 = 255;
    if (g0 < 0)
      g0 = 0;
    if (b0 > 255)
      b0 = 255;
    if (b0 < 0)
      b0 = 0;

    if (r1 > 255)
      r1 = 255;
    if (r1 < 0)
      r1 = 0;
    if (g1 > 255)
      g1 = 255;
    if (g1 < 0)
      g1 = 0;
    if (b1 > 255)
      b1 = 255;
    if (b1 < 0)
      b1 = 0;

    if (r2 > 255)
      r2 = 255;
    if (r2 < 0)
      r2 = 0;
    if (g2 > 255)
      g2 = 255;
    if (g2 < 0)
      g2 = 0;
    if (b2 > 255)
      b2 = 255;
    if (b2 < 0)
      b2 = 0;

    if (r3 > 255)
      r3 = 255;
    if (r3 < 0)
      r3 = 0;
    if (g3 > 255)
      g3 = 255;
    if (g3 < 0)
      g3 = 0;
    if (b3 > 255)
      b3 = 255;
    if (b3 < 0)
      b3 = 0;

    output[i] = round(r0);
    output[i + 1] = round(g0);
    output[i + 2] = round(b0);
    output[i + 3] = round(r1);
    output[i + 4] = round(g1);
    output[i + 5] = round(b1);
    output[i + 6] = round(r2);
    output[i + 7] = round(g2);
    output[i + 8] = round(b2);
    output[i + 9] = round(r3);
    output[i + 10] = round(g3);
    output[i + 11] = round(b3);
  }
}
