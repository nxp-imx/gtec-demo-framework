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

__kernel void rgb888_to_rgb565_conversions(__global uchar* input, __global uchar* output, int width, int height, uchar cvtcolor_flag)
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
