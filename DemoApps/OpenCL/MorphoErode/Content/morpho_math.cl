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

__kernel void dilate_op(__global uchar* input, __global uchar* output, int width, int height)
{
  int i;
  int x = get_global_id(0);
  int y = get_global_id(1);
  int id = (y * width) + x;
  float max = 0;
  float values[5];

  if ((x == 0) || (y == 0) || (x == (width - 1)) || (y == (height - 1)))
  {
    output[id] = input[id];
    return;
  }

  values[0] = input[id - width];
  values[1] = input[id - 1];
  values[2] = input[id];
  values[3] = input[id + 1];
  values[4] = input[id + width];

  if (values[0] > max)
    max = values[0];
  if (values[1] > max)
    max = values[1];
  if (values[2] > max)
    max = values[2];
  if (values[3] > max)
    max = values[3];
  if (values[4] > max)
    max = values[4];

  output[id] = max;
}

__kernel void erode_op(__global uchar* input, __global uchar* output, int width, int height)
{
  int i;
  int x = get_global_id(0);
  int y = get_global_id(1);
  int id = (y * width) + x;
  float min = 255.0;
  float values[5];

  if ((x == 0) || (y == 0) || (x == (width - 1)) || (y == (height - 1)))
  {
    output[id] = input[id];
    return;
  }

  values[0] = input[id - width];
  values[1] = input[id - 1];
  values[2] = input[id];
  values[3] = input[id + 1];
  values[4] = input[id + width];

  if (values[0] < min)
    min = values[0];
  if (values[1] < min)
    min = values[1];
  if (values[2] < min)
    min = values[2];
  if (values[3] < min)
    min = values[3];
  if (values[4] < min)
    min = values[4];

  output[id] = min;
}
