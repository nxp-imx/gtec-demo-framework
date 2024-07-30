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

__kernel void median_filter(__global uchar* input, __global uchar* output, int width, int height)
{
  int median_kernel[9];

  int i;
  int j;
  int aux;

  int x = get_global_id(0);
  int y = get_global_id(1);
  int id = (y * width) + x;

  if ((x == 0) || (y == 0) || (x == (width - 1)) || (y == (height - 1)))
  {
    output[id] = input[id];
    return;
  }

  median_kernel[0] = input[id - width - 1];
  median_kernel[1] = input[id - width];
  median_kernel[2] = input[id - width + 1];

  median_kernel[3] = input[id - 1];
  median_kernel[4] = input[id];
  median_kernel[5] = input[id + 1];

  median_kernel[6] = input[id + width - 1];
  median_kernel[7] = input[id + width];
  median_kernel[8] = input[id + width + 1];

  for (i = 0; i < 5; i++)
  {
    for (j = i + 1; j < 9; j++)
    {
      if (median_kernel[i] > median_kernel[j])
      {
        aux = median_kernel[i];
        median_kernel[i] = median_kernel[j];
        median_kernel[j] = aux;
      }
    }
  }

  output[id] = median_kernel[4];
}
