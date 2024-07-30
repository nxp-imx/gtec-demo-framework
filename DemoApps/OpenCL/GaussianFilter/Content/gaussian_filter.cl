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

__kernel void gaussian_filter(__global uchar* input, __global uchar* output, int width, int height)
{
  int x = get_global_id(0);
  int y = get_global_id(1);
  int id = (y * width) + x;

  float sum = 0;

  float kernel_weights[3][3] = {{1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}, {2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0}, {1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}};

  if ((x == 0) || (y == 0) || (x == (width - 1)) || (y == (height - 1)))
  {
    output[id] = input[id];
    return;
  }

  sum = input[id - width - 1] * kernel_weights[0][0];
  sum += input[id - width] * kernel_weights[0][1];
  sum += input[id - width + 1] * kernel_weights[0][2];

  sum += input[id - 1] * kernel_weights[1][0];
  sum += input[id] * kernel_weights[1][1];
  sum += input[id + 1] * kernel_weights[1][2];

  sum += input[id + width - 1] * kernel_weights[2][0];
  sum += input[id + width] * kernel_weights[2][1];
  sum += input[id + width + 1] * kernel_weights[2][2];

  output[id] = sum;
}
