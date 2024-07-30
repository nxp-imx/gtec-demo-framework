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

__kernel void rgb888_to_uyvy_conversions(__global uchar* input, __global uchar* output, int width, int height, uchar cvtcolor_flag)
{
  int x = get_global_id(0);
  int y = get_global_id(1);

  float r0, g0, b0;
  float r1, g1, b1;
  float r2, g2, b2;
  float r3, g3, b3;


  // RGB888 to UYVY
  if (cvtcolor_flag == 0)
  {
    float Y0, U0;
    float Y1, V0;
    float Y2, U2;
    float Y3, V2;
    if ((x < width) && (x % 4 == 0))
    {
      int src_index = 3 * mad24(y, width, x);
      int dst_index = 2 * mad24(y, width, x);

      if (y < height)
      {
        r0 = input[src_index];
        g0 = input[src_index + 1];
        b0 = input[src_index + 2];

        r1 = input[src_index + 3];
        g1 = input[src_index + 4];
        b1 = input[src_index + 5];

        r2 = input[src_index + 6];
        g2 = input[src_index + 7];
        b2 = input[src_index + 8];

        r3 = input[src_index + 9];
        g3 = input[src_index + 10];
        b3 = input[src_index + 11];

        Y0 = (0.299 * r0) + (0.587 * g0) + (0.114 * b0);
        U0 = (-0.169 * r0) - (0.331 * g0) + (0.499 * b0) + 128;
        V0 = (0.499 * r0) - (0.418 * g0) - (0.0813 * b0) + 128;
        Y1 = (0.299 * r1) + (0.587 * g1) + (0.114 * b1);

        Y2 = (0.299 * r2) + (0.587 * g2) + (0.114 * b2);
        U2 = (-0.169 * r2) - (0.331 * g2) + (0.499 * b2) + 128;
        V2 = (0.499 * r2) - (0.418 * g2) - (0.0813 * b2) + 128;
        Y3 = (0.299 * r3) + (0.587 * g3) + (0.114 * b3);

        if (Y0 > 255)
          Y0 = 255;
        if (Y0 < 0)
          Y0 = 0;
        if (U0 > 255)
          U0 = 255;
        if (U0 < 0)
          U0 = 0;
        if (V0 > 255)
          V0 = 255;
        if (V0 < 0)
          V0 = 0;
        if (Y1 > 255)
          Y1 = 255;
        if (Y1 < 0)
          Y1 = 0;

        if (U2 > 255)
          U2 = 255;
        if (U2 < 0)
          U2 = 0;
        if (V2 > 255)
          V2 = 255;
        if (V2 < 0)
          V2 = 0;
        if (Y2 > 255)
          Y2 = 255;
        if (Y2 < 0)
          Y2 = 0;
        if (Y3 > 255)
          Y3 = 255;
        if (Y3 < 0)
          Y3 = 0;

        output[dst_index] = round(U0);
        output[dst_index + 1] = round(Y0);
        output[dst_index + 2] = round(V0);
        output[dst_index + 3] = round(Y1);

        output[dst_index + 4] = round(U2);
        output[dst_index + 5] = round(Y2);
        output[dst_index + 6] = round(V2);
        output[dst_index + 7] = round(Y3);
      }
    }
  }


  // UYVY to RGB888
  if (cvtcolor_flag == 1)
  {
    float Y0, U0, V0;
    float Y1, U1, V1;
    float Y2, U2, V2;
    float Y3, U3, V3;

    uchar u0, y0, v1, y1, u2, y2, v3, y3;
    if ((x < width) && (x % 4 == 0))
    {
      int src_index = 2 * mad24(y, width, x);
      int dst_index = 3 * mad24(y, width, x);

      if (y < height)
      {
        u0 = input[src_index];
        y0 = input[src_index + 1];
        v1 = input[src_index + 2];
        y1 = input[src_index + 3];

        u2 = input[src_index + 4];
        y2 = input[src_index + 5];
        v3 = input[src_index + 6];
        y3 = input[src_index + 7];

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

        output[dst_index] = round(r0);
        output[dst_index + 1] = round(g0);
        output[dst_index + 2] = round(b0);
        output[dst_index + 3] = round(r1);
        output[dst_index + 4] = round(g1);
        output[dst_index + 5] = round(b1);
        output[dst_index + 6] = round(r2);
        output[dst_index + 7] = round(g2);
        output[dst_index + 8] = round(b2);
        output[dst_index + 9] = round(r3);
        output[dst_index + 10] = round(g3);
        output[dst_index + 11] = round(b3);
      }
    }
  }
}
