#ifndef OPENVX_SOFTISP_KERNELS_VXC_HPP
#define OPENVX_SOFTISP_KERNELS_VXC_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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

char vxcKernelSource[] =
{
"#include \"cl_viv_vx_ext.h\" \n\
\n\
 \n\
 \n\
__kernel void whitebalance \n\
  ( \n\
  __read_only image2d_t   in_image, \n\
  vx_distribution alpha,\n\
      vx_distribution  beta,\n\
  __write_only image2d_t  out_image \n\
  ) \n\
{ \n\
  int2 coord_in = (int2)(get_global_id(0), get_global_id(1)); \n\
  int2 coord_out = coord_in; \n\
  vxc_uchar16 lineA, lineB;\n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineB, in_image, coord_in, 0x20, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  int pixelEven, pixelOdd, temp;\n\
  vxc_uchar16 pixelOut;\n\
  float alpha_v = (float)(*(alpha.ptr))/(*(alpha.ptr + 1));\n\
  float beta_v = (float)(*(beta.ptr))/(*(beta.ptr + 1));\n\
  pixelEven = convert_int(lineA.s0);\n\
  temp = alpha_v*pixelEven;\n\
  if(temp > 255)\n\
    pixelOut.s0 = convert_uchar(255);\n\
  else\n\
    pixelOut.s0 = convert_uchar(temp);\n\
  pixelOut.s1 = lineA.s1;\n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, pixelOut, VXC_MODIFIER(0, 1, 0, VXC_RM_TowardZero, 0)); \n\
  coord_out.y ++; \n\
  pixelOdd = convert_int(lineB.s1);\n\
  temp = beta_v*pixelOdd;\n\
  if(temp > 255)\n\
    pixelOut.s1 = convert_uchar(255);\n\
  else\n\
    pixelOut.s1 = convert_uchar(temp);\n\
  pixelOut.s0 = lineB.s0;\n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, pixelOut, VXC_MODIFIER(0, 1, 0, VXC_RM_TowardZero, 0)); \n\
}\n\
__kernel void histogram\n\
(\n\
  read_only image2d_t   in_image,\n\
      vx_distribution     in_distR,\n\
      vx_distribution     in_distG,\n\
      vx_distribution     in_distB\n\
)\n\
{\n\
      int2 coord_in = (int2)(get_global_id(0), get_global_id(1));\n\
  vxc_uchar16 zero = 0, lineA, lineB;\n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineB, in_image, coord_in, 0x20, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  uchar pixelEven, pixelOdd;\n\
  pixelEven = convert_uchar(lineA.s0);\n\
  if(pixelEven >= in_distR.offset)\n\
  {\n\
    if(pixelEven < (in_distR.offset + in_distR.rang))\n\
    {\n\
      int i = (convert_uint(pixelEven) - in_distR.offset) / in_distR.window_r;\n\
      atomic_inc(in_distR.ptr + i);\n\
    }\n\
  }\n\
  pixelEven = convert_uchar(lineA.s1);\n\
  if(pixelEven >= in_distG.offset)\n\
  {\n\
    if(pixelEven < (in_distG.offset + in_distG.rang))\n\
    {\n\
      int i = (convert_uint(pixelEven) - in_distG.offset) / in_distG.window_r;\n\
      atomic_inc(in_distG.ptr + i);\n\
    }\n\
  }\n\
  pixelOdd = convert_uchar(lineB.s0);\n\
  if(pixelOdd >= in_distG.offset)\n\
  {\n\
    if(pixelOdd < (in_distG.offset + in_distG.rang))\n\
    {\n\
      int i = (convert_uint(pixelOdd) - in_distG.offset) / in_distG.window_r;\n\
      atomic_inc(in_distG.ptr + i);\n\
    }\n\
  }\n\
  pixelOdd = convert_uchar(lineB.s1);\n\
  if(pixelOdd >= in_distB.offset)\n\
  {\n\
    if(pixelOdd < (in_distB.offset + in_distB.rang))\n\
    {\n\
      int i = (convert_uint(pixelOdd) - in_distB.offset) / in_distB.window_r;\n\
      atomic_inc(in_distB.ptr + i);\n\
    }\n\
  }\n\
}\n\
#define imgWid 1920\n\
#define imgHei 1080\n\
__kernel void EqualizeHist0\n\
(\n\
  vx_distribution   in_distR,\n\
      vx_distribution   in_distG,\n\
      vx_distribution   in_distB,\n\
      vx_distribution   out_distR,\n\
      vx_distribution   out_distG,\n\
      vx_distribution   out_distB,\n\
      vx_distribution alpha,\n\
      vx_distribution  beta\n\
)\n\
{\n\
  int global_id = get_global_id(1) * get_global_size(0) + get_global_id(0);\n\
  int gid = get_group_id(1)* get_num_groups(0) + get_group_id(0);\n\
  int total = imgWid * imgHei;\n\
  int pixel_value_R = 0,pixel_value_G = 0, pixel_value_B = 0;\n\
if (global_id==0){\n\
  float scaler, scaleg, scaleb;\n\
  int sumr = 0;\n\
        int ir = 0;\n\
        while (!(*(in_distR.ptr + ir)))\n\
            ++ir;\n\
        if ((total>>2) == (*(in_distR.ptr + ir))){\n\
              scaler = 1;\n\
              *(out_distR.ptr + ir) = ir;\n\
              pixel_value_R = ir*(total>>2);\n\
        }\n\
        else{\n\
          pixel_value_R = ir*(*(in_distR.ptr + ir));\n\
    scaler = 255.f / (total/4 - *(in_distR.ptr + ir));\n\
    *(out_distR.ptr + ir) = 0;\n\
    ir++;\n\
            for (; ir < 256; ir++){\n\
              pixel_value_R += ir*(*(in_distR.ptr + ir));\n\
                  sumr += *(in_distR.ptr + ir);\n\
                  *(out_distR.ptr + ir) = (int)convert_uchar_sat_rte(convert_float(sumr) * scaler);\n\
                  //*(out_distR.ptr + ir) = convert_int_sat_rte(convert_float(sumr) * scaler);\n\
            }\n\
  }\n\
  int sumg = 0;\n\
        int ig = 0;\n\
        while (!(*(in_distG.ptr + ig)))\n\
            ++ig;\n\
        if (total>>1 == *(in_distG.ptr + ig)){\n\
              scaleg = 1;\n\
              *(out_distG.ptr + ig) = ig;\n\
              pixel_value_G = ig*(total>>1);\n\
        }\n\
        else{\n\
          pixel_value_G = ig*(*(in_distG.ptr + ig));\n\
    scaleg = 255.f / (total/2 - *(in_distG.ptr + ig));\n\
    *(out_distG.ptr + ig) = 0;\n\
    ig++;\n\
              for (; ig < 256; ig++){\n\
                pixel_value_G += ig*(*(in_distG.ptr + ig));\n\
                  sumg += *(in_distG.ptr + ig);\n\
                  *(out_distG.ptr + ig) = (int)convert_uchar_sat_rte(convert_float(sumg) * scaleg);\n\
                  //*(out_distG.ptr + ig) = convert_int_sat_rte(convert_float(sumg) * scaleg);\n\
              }\n\
  }\n\
  int sumb = 0;\n\
        int ib = 0;\n\
        while (!(*(in_distB.ptr + ib)))\n\
            ++ib;\n\
        if ((total>>2) == *(in_distB.ptr + ib)){\n\
              scaleb = 1;\n\
                *(out_distB.ptr + ib) = ib;\n\
                pixel_value_B = ib*(total>>2);\n\
        }\n\
        else{\n\
          pixel_value_B = ib*(*(in_distB.ptr + ib));\n\
    scaleb = 255.f / (total/4 - *(in_distB.ptr + ib));\n\
    *(out_distB.ptr + ib) = 0;\n\
    ib++;\n\
              for (; ib < 256; ib++){ \n\
                pixel_value_B += ib*(*(in_distB.ptr + ib));\n\
                  sumb += *(in_distB.ptr + ib);\n\
                  *(out_distB.ptr + ib) = (int)convert_uchar_sat_rte(convert_float(sumb) * scaleb);\n\
                  //*(out_distB.ptr + ib) = convert_int_sat_rte(convert_float(sumb) * scaleb);\n\
              }\n\
  }\n\
}\n\
barrier(CLK_LOCAL_MEM_FENCE);\n\
if (global_id==0){\n\
  int R_ave = pixel_value_R/(total>>2);\n\
  int G_ave = pixel_value_G/(total>>1);\n\
  int B_ave = pixel_value_B/(total>>2);\n\
  //*(alpha.ptr) = G_ave/R_ave;\n\
  //*(beta.ptr ) = G_ave/B_ave;\n\
  *(alpha.ptr) = G_ave;\n\
  *(alpha.ptr + 1) = R_ave;\n\
  *(beta.ptr ) = G_ave;\n\
  *(beta.ptr + 1) = B_ave;\n\
}\n\
}\n\
__kernel void EqualizeHist\n\
(\n\
  __read_only image2d_t   in_image,\n\
  vx_distribution   in_distR,\n\
      vx_distribution   in_distG,\n\
      vx_distribution   in_distB,\n\
      __write_only image2d_t  out_image \n\
)\n\
{\n\
      int2 coord_in = (int2)(get_global_id(0), get_global_id(1));\n\
      int2 coord_out = coord_in; \n\
  vxc_uchar16 zero = 0, lineA, lineB;\n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineB, in_image, coord_in, 0x20, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  int pixelEven, pixelOdd;\n\
  vxc_uchar16 pixelOut;\n\
  pixelEven = convert_int(lineA.s0);\n\
  pixelOdd = convert_int(lineA.s1);\n\
  //pixelOut.s0 = convert_uchar(pixelEven);\n\
  //pixelOut.s1 = convert_uchar(pixelOdd);\n\
  pixelOut.s0 = convert_uchar(*(in_distR.ptr + pixelEven));\n\
  pixelOut.s1 = convert_uchar(*(in_distG.ptr + pixelOdd));\n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, pixelOut, VXC_MODIFIER(0, 1, 0, VXC_RM_TowardZero, 0)); \n\
  coord_out.y ++; \n\
  pixelEven = convert_int(lineB.s0);\n\
  pixelOdd = convert_int(lineB.s1);\n\
  //pixelOut.s0 = convert_uchar(pixelEven);\n\
  //pixelOut.s1 = convert_uchar(pixelOdd);\n\
  pixelOut.s0 = convert_uchar(*(in_distG.ptr + pixelEven));\n\
  pixelOut.s1 = convert_uchar(*(in_distB.ptr + pixelOdd));\n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, pixelOut, VXC_MODIFIER(0, 1, 0, VXC_RM_TowardZero, 0)); \n\
}\n\
 _viv_uniform VXC_512Bits EE_EO_AccTwoLines; \n\
_viv_uniform VXC_512Bits EO_OO_AccTwoLines; \n\
_viv_uniform VXC_512Bits EE_EO_AccOneLine; \n\
_viv_uniform VXC_512Bits EO_OO_AccOneLine; \n\
_viv_uniform VXC_512Bits EE_EO_PackBGR; \n\
_viv_uniform VXC_512Bits EO_OO_PackBGR; \n\
_viv_uniform VXC_512Bits BGR_2_BGRA;\n\
__kernel void debayer \n\
  ( \n\
  __read_only image2d_t   in_image, \n\
  __write_only image2d_t  out_image \n\
  ) \n\
{ \n\
  int2 coord_in = (int2)(get_global_id(0), get_global_id(1)); \n\
  int2 coord_out = coord_in; \n\
  //coord_out.x = coord_out.x * 3; \n\
  coord_out.x = coord_out.x * 4; \n\
  coord_in.xy = coord_in.xy - 2; \n\
 \n\
  vxc_uchar16 bgr_register, bgra_register; \n\
  vxc_uchar16 lineA, lineB, lineC, lineD; \n\
  vxc_uchar16 one = 1;\n\
  vxc_short8 acc_0, acc_1, acc_2, dst; \n\
 \n\
  //process EE EO  \n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(img_load, lineB, in_image, coord_in, 0x20, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(img_load, lineC, in_image, coord_in, 0x40, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp4x8, acc_0, lineA, lineB, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EE_EO_AccTwoLines); \n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0x60, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(img_load, lineD, in_image, coord_in, 0x80, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp4x8, acc_1, lineC, lineC, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EE_EO_AccOneLine); \n\
  VXC_OP4(dp4x8, acc_2, lineD, lineA, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EE_EO_AccTwoLines); \n\
  VXC_OP4(iadd, dst, acc_0, acc_1, acc_2, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp2x16, bgr_register, dst, lineC, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 1), EE_EO_PackBGR); \n\
  VXC_OP4(dp2x16, bgra_register, bgr_register, one, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 1), BGR_2_BGRA); \n\
  //VXC_OP4_NoDest(img_store, out_image, coord_out, bgr_register, VXC_MODIFIER(0, 11, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, bgra_register, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  coord_out.y ++; \n\
 \n\
  //process OE OO  \n\
  VXC_OP4(dp4x8, acc_0, lineB, lineC, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EO_OO_AccTwoLines); \n\
  VXC_OP4(img_load, lineB, in_image, coord_in, 0xa0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp4x8, acc_2, lineA, lineA, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EO_OO_AccOneLine); \n\
  VXC_OP4(dp4x8, acc_1, lineB, lineD, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EO_OO_AccTwoLines); \n\
  VXC_OP4(iadd, dst, acc_0, acc_1, acc_2, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp2x16, bgr_register, dst, lineA, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 1), EO_OO_PackBGR); \n\
  //VXC_OP4_NoDest(img_store, out_image, coord_out, bgr_register, VXC_MODIFIER(0, 11, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp2x16, bgra_register, bgr_register, one, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 1), BGR_2_BGRA); \n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, bgra_register, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  coord_out.y ++; \n\
 \n\
  //process EE EO  \n\
  VXC_OP4(dp4x8, acc_0, lineC, lineA, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EE_EO_AccTwoLines); \n\
  VXC_OP4(img_load, lineC, in_image, coord_in, 0xc0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp4x8, acc_1, lineD, lineD, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EE_EO_AccOneLine); \n\
  VXC_OP4(dp4x8, acc_2, lineC, lineB, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EE_EO_AccTwoLines); \n\
  VXC_OP4(iadd, dst, acc_0, acc_1, acc_2, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp2x16, bgr_register, dst, lineD, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 1), EE_EO_PackBGR); \n\
  //VXC_OP4_NoDest(img_store, out_image, coord_out, bgr_register, VXC_MODIFIER(0, 11, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp2x16, bgra_register, bgr_register, one, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 1), BGR_2_BGRA); \n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, bgra_register, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  coord_out.y ++; \n\
 \n\
  //process OE OO  \n\
  VXC_OP4(dp4x8, acc_0, lineA, lineD, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EO_OO_AccTwoLines); \n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0xe0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp4x8, acc_2, lineB, lineB, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EO_OO_AccOneLine); \n\
  VXC_OP4(dp4x8, acc_1, lineA, lineC, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0), EO_OO_AccTwoLines); \n\
  VXC_OP4(iadd, dst, acc_0, acc_1, acc_2, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp2x16, bgr_register, dst, lineB, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 1), EO_OO_PackBGR); \n\
  //VXC_OP4_NoDest(img_store, out_image, coord_out, bgr_register, VXC_MODIFIER(0, 11, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp2x16, bgra_register, bgr_register, one, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 1), BGR_2_BGRA); \n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, bgra_register, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
}\n\
_viv_uniform VXC_512Bits RR0_RR2_OneLines; \n\
_viv_uniform VXC_512Bits RR1_OneLines; \n\
_viv_uniform VXC_512Bits Acc1_TwoLines; \n\
_viv_uniform VXC_512Bits Acc2_TwoLines; \n\
_viv_uniform VXC_512Bits Select_M; \n\
_viv_uniform VXC_512Bits RM_Max_Min;\n\
_viv_uniform VXC_512Bits Sum_Div_128;\n\
_viv_uniform VXC_512Bits Sum_Div_32;\n\
_viv_uniform VXC_512Bits Sum_Div_8;\n\
\n\
_viv_uniform VXC_512Bits BB0_BB2_OneLines; \n\
_viv_uniform VXC_512Bits BB1_OneLines; \n\
_viv_uniform VXC_512Bits RG_GG0_GG2_OneLines; \n\
_viv_uniform VXC_512Bits RG_GG1_OneLines; \n\
_viv_uniform VXC_512Bits GB_GG0_GG2_OneLines; \n\
_viv_uniform VXC_512Bits GB_GG1_OneLines; \n\
__kernel void badpixel \n\
  ( \n\
  __read_only image2d_t   in_image, \n\
  __write_only image2d_t  out_image\n\
  ) \n\
{ \n\
  int2 coord_in = (int2)(get_global_id(0), get_global_id(1)); \n\
  int2 coord_out = coord_in; \n\
  coord_in.xy = coord_in.xy - 2; \n\
 \n\
  vxc_uchar16 lineA, lineB, lineC, lineD, lineE, lineF; \n\
  vxc_uchar16 RR0, RR1, RR2, tmpMax, tmpMin, maxV, minV, diffV;\n\
  vxc_uchar16 highValue, lowValue, avgR, avgG, avgB;\n\
  vxc_uchar16 GG0, GG1, GG2;\n\
  vxc_uchar16 outVector, tmpOut;\n\
  vxc_uchar16 BB0, BB1, BB2;\n\
  vxc_ushort8 acc1, acc2, tmpsum1, tmpsum2;\n\
  vxc_uchar16 sum1, sum2, sum3;\n\
 \n\
  //Process line odd: R G \n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(img_load, lineB, in_image, coord_in, 0x20, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(img_load, lineC, in_image, coord_in, 0x40, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(img_load, lineD, in_image, coord_in, 0x60, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(img_load, lineE, in_image, coord_in, 0x80, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(img_load, lineF, in_image, coord_in, 0xa0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4(dp2x16, RR0, lineA, lineA, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), RR0_RR2_OneLines);\n\
  VXC_OP4(dp2x16, RR2, lineE, lineE, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), RR0_RR2_OneLines);\n\
  VXC_OP4(dp2x16, RR1, lineC, lineC, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), RR1_OneLines);\n\
  tmpMax = viv_intrinsic_vxmc_Filter_uc(RR0, RR1, RR2, VXC_MODIFIER_FILTER(0, 13, 0, VXC_FM_Max, 0));\n\
  tmpMin = viv_intrinsic_vxmc_Filter_uc(RR0, RR1, RR2, VXC_MODIFIER_FILTER(0, 13, 0, VXC_FM_Min, 0));\n\
  VXC_OP4(dp2x16, maxV, tmpMax, tmpMax, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Select_M); \n\
  VXC_OP4(dp2x16, minV, tmpMin, tmpMin, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Select_M); \n\
  VXC_OP4(dp4x8, acc1, RR0, RR1, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Acc1_TwoLines); \n\
  VXC_OP4(dp4x8, acc2, RR2, RR1, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Acc2_TwoLines); \n\
  VXC_OP4(dp4x8, tmpsum1, acc1, maxV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), RM_Max_Min); \n\
  VXC_OP4(dp4x8, tmpsum2, acc2, minV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), RM_Max_Min); \n\
  VXC_OP4(dp2x16, sum1, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_128); \n\
  VXC_OP4(dp2x16, sum2, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_32); \n\
  VXC_OP4(dp2x16, sum3, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_8); \n\
  VXC_OP4(iadd, avgR, sum1, sum2, sum3, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP3(abs_diff, diffV, maxV, minV, VXC_MODIFIER_FILTER(0, 4, 0, VXC_FM_Min, 0));\n\
  VXC_OP3(iadd, highValue, avgR, diffV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP3(abs_diff, lowValue, avgR, diffV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0));\n\
  tmpOut.lo = lineC.s2468ace0;\n\
  outVector.s02468ace = (tmpOut.lo > highValue.lo || tmpOut.lo < lowValue.lo) ? avgR.lo : tmpOut.lo;\n\
  \n\
  \n\
  VXC_OP4(dp2x16, GG0, lineB, lineA, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), RG_GG0_GG2_OneLines);\n\
  VXC_OP4(dp2x16, GG2, lineD, lineE, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), RG_GG0_GG2_OneLines);\n\
  VXC_OP4(dp2x16, GG1, lineC, lineC, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), RG_GG1_OneLines);\n\
  tmpMax = viv_intrinsic_vxmc_Filter_uc(GG0, GG1, GG2, VXC_MODIFIER_FILTER(0, 13, 0, VXC_FM_Max, 0));\n\
  tmpMin = viv_intrinsic_vxmc_Filter_uc(GG0, GG1, GG2, VXC_MODIFIER_FILTER(0, 13, 0, VXC_FM_Min, 0));\n\
  VXC_OP4(dp2x16, maxV, tmpMax, tmpMax, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Select_M); \n\
  VXC_OP4(dp2x16, minV, tmpMin, tmpMin, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Select_M); \n\
  VXC_OP4(dp4x8, acc1, GG0, GG1, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Acc1_TwoLines); \n\
  VXC_OP4(dp4x8, acc2, GG2, GG1, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Acc2_TwoLines); \n\
  VXC_OP4(dp4x8, tmpsum1, acc1, maxV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), RM_Max_Min); \n\
  VXC_OP4(dp4x8, tmpsum2, acc2, minV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), RM_Max_Min); \n\
  VXC_OP4(dp2x16, sum1, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_128); \n\
  VXC_OP4(dp2x16, sum2, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_32); \n\
  VXC_OP4(dp2x16, sum3, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_8); \n\
  VXC_OP4(iadd, avgG, sum1, sum2, sum3, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP3(abs_diff, diffV, maxV, minV, VXC_MODIFIER_FILTER(0, 4, 0, VXC_FM_Min, 0));\n\
  VXC_OP3(iadd, highValue, avgG, diffV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP3(abs_diff, lowValue, avgG, diffV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0));\n\
  tmpOut.lo = lineC.s3579bdf1;\n\
  outVector.s13579bdf = (tmpOut.lo > highValue.lo || tmpOut.lo < lowValue.lo) ? avgG.lo : tmpOut.lo;\n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, outVector, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0)); \n\
  coord_out.y++;\n\
  \n\
  \n\
  VXC_OP4(dp2x16, GG0, lineC, lineB, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), GB_GG0_GG2_OneLines);\n\
  VXC_OP4(dp2x16, GG2, lineE, lineF, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), GB_GG0_GG2_OneLines);\n\
  VXC_OP4(dp2x16, GG1, lineD, lineD, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), GB_GG1_OneLines);\n\
  tmpMax = viv_intrinsic_vxmc_Filter_uc(GG0, GG1, GG2, VXC_MODIFIER_FILTER(0, 13, 0, VXC_FM_Max, 0));\n\
  tmpMin = viv_intrinsic_vxmc_Filter_uc(GG0, GG1, GG2, VXC_MODIFIER_FILTER(0, 13, 0, VXC_FM_Min, 0));\n\
  VXC_OP4(dp2x16, maxV, tmpMax, tmpMax, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Select_M); \n\
  VXC_OP4(dp2x16, minV, tmpMin, tmpMin, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Select_M); \n\
  VXC_OP4(dp4x8, acc1, GG0, GG1, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Acc1_TwoLines); \n\
  VXC_OP4(dp4x8, acc2, GG2, GG1, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Acc2_TwoLines); \n\
  VXC_OP4(dp4x8, tmpsum1, acc1, maxV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), RM_Max_Min); \n\
  VXC_OP4(dp4x8, tmpsum2, acc2, minV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), RM_Max_Min); \n\
  VXC_OP4(dp2x16, sum1, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_128); \n\
  VXC_OP4(dp2x16, sum2, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_32); \n\
  VXC_OP4(dp2x16, sum3, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_8); \n\
  VXC_OP4(iadd, avgG, sum1, sum2, sum3, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP3(abs_diff, diffV, maxV, minV, VXC_MODIFIER_FILTER(0, 4, 0, VXC_FM_Min, 0));\n\
  VXC_OP3(iadd, highValue, avgG, diffV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP3(abs_diff, lowValue, avgG, diffV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0));\n\
  tmpOut.lo = lineD.s2468ace0;\n\
  outVector.s02468ace = (tmpOut.lo > highValue.lo || tmpOut.lo < lowValue.lo) ? avgG.lo : tmpOut.lo;\n\
  \n\
  VXC_OP4(dp2x16, BB0, lineB, lineA, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), BB0_BB2_OneLines);\n\
  VXC_OP4(dp2x16, BB2, lineD, lineE, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), BB0_BB2_OneLines);\n\
  VXC_OP4(dp2x16, BB1, lineC, lineC, VXC_MODIFIER(0, 14, 0, VXC_RM_TowardZero, 0), BB1_OneLines);\n\
  tmpMax = viv_intrinsic_vxmc_Filter_uc(BB0, BB1, BB2, VXC_MODIFIER_FILTER(0, 13, 0, VXC_FM_Max, 0));\n\
  tmpMin = viv_intrinsic_vxmc_Filter_uc(BB0, BB1, BB2, VXC_MODIFIER_FILTER(0, 13, 0, VXC_FM_Min, 0));\n\
  VXC_OP4(dp2x16, maxV, tmpMax, tmpMax, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Select_M); \n\
  VXC_OP4(dp2x16, minV, tmpMin, tmpMin, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Select_M); \n\
  VXC_OP4(dp4x8, acc1, BB0, BB1, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Acc1_TwoLines); \n\
  VXC_OP4(dp4x8, acc2, BB2, BB1, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Acc2_TwoLines); \n\
  VXC_OP4(dp4x8, tmpsum1, acc1, maxV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), RM_Max_Min); \n\
  VXC_OP4(dp4x8, tmpsum2, acc2, minV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), RM_Max_Min); \n\
  VXC_OP4(dp2x16, sum1, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_128); \n\
  VXC_OP4(dp2x16, sum2, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_32); \n\
  VXC_OP4(dp2x16, sum3, tmpsum1, tmpsum2, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0), Sum_Div_8); \n\
  VXC_OP4(iadd, avgB, sum1, sum2, sum3, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP3(abs_diff, diffV, maxV, minV, VXC_MODIFIER_FILTER(0, 4, 0, VXC_FM_Min, 0));\n\
  VXC_OP3(iadd, highValue, avgB, diffV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP3(abs_diff, lowValue, avgB, diffV, VXC_MODIFIER(0, 4, 0, VXC_RM_TowardZero, 0));\n\
  tmpOut.lo = lineD.s3579bdf1;\n\
  outVector.s13579bdf = (tmpOut.lo > highValue.lo || tmpOut.lo < lowValue.lo) ? avgB.lo : tmpOut.lo;\n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, outVector, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0)); \n\
}\n\
_viv_uniform VXC_512Bits RGBA2Y_UV; \n\
_viv_uniform VXC_512Bits Y_UV2RGBA; \n\
__kernel void rgba2yuv\n\
  ( \n\
  __read_only image2d_t   in_image, \n\
  __write_only image2d_t  out_image, \n\
  __write_only image2d_t  out_image_y \n\
  ) \n\
{ \n\
  int2 coord_in = (int2)(get_global_id(0), get_global_id(1)); \n\
  int2 coord_out = coord_in; \n\
  int2 coord_out_color = coord_in; \n\
  coord_out_color.x = coord_out_color.x * 3;\n\
  coord_in.x = coord_in.x * 4;\n\
  vxc_uchar16 lineA;\n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  //lineA = read_imageui(in_image, coord_in);\n\
  float4 tmp1, tmp2, tmp3, tmp4;\n\
  VXC_OP4(dp4x4, tmp1, lineA, lineA, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0), RGBA2Y_UV); \n\
  vxc_uchar4 outY;\n\
  vxc_uchar16 outColor;\n\
  outY.x = convert_uchar_rtz(tmp1.x);\n\
  outColor.s0 = convert_uchar_rtz(tmp1.y);\n\
  outColor.s1 = convert_uchar_rtz(tmp1.z);\n\
  outColor.s2 = 255;\n\
  \n\
  vxc_uchar16 lineB;\n\
  lineB = lineA.s456789abcdef0123;\n\
  VXC_OP4(dp4x4, tmp2, lineB, lineB, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0), RGBA2Y_UV); \n\
  outY.y = convert_uchar_rtz(tmp2.x);\n\
  outColor.s3 = convert_uchar_rtz(tmp2.y);\n\
  outColor.s4 = convert_uchar_rtz(tmp2.z);\n\
  outColor.s5 = 255;\n\
  \n\
  vxc_uchar16 lineC;\n\
  lineC = lineA.s89abcdef01234567;\n\
  VXC_OP4(dp4x4, tmp3, lineC, lineC, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0), RGBA2Y_UV); \n\
  outY.z = convert_uchar_rtz(tmp3.x);\n\
  outColor.s6 = convert_uchar_rtz(tmp3.y);\n\
  outColor.s7 = convert_uchar_rtz(tmp3.z);\n\
  outColor.s8 = 255;\n\
  \n\
  vxc_uchar16 lineD;\n\
  lineD = lineA.scdef0123456789ab;\n\
  VXC_OP4(dp4x4, tmp4, lineD, lineD, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0), RGBA2Y_UV); \n\
  outY.w = convert_uchar_rtz(tmp4.x);\n\
  outColor.s9 = convert_uchar_rtz(tmp4.y);\n\
  outColor.sa = convert_uchar_rtz(tmp4.z);\n\
  outColor.sb = 255;\n\
  \n\
  VXC_OP4_NoDest(img_store, out_image, coord_out_color, outColor, VXC_MODIFIER(0, 11, 0, VXC_RM_TowardZero, 0)); \n\
  VXC_OP4_NoDest(img_store, out_image_y, coord_out, outY, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0)); \n\
  //write_imageui(out_image, coord_out, lineA);\n\
}\n\
__kernel void yuv2rgba\n\
  ( \n\
  __read_only image2d_t   in_image, \n\
  __read_only image2d_t   in_image_y, \n\
  __write_only image2d_t  out_image \n\
  ) \n\
{ \n\
  int2 coord_in = (int2)(get_global_id(0), get_global_id(1)); \n\
  int2 coord_out = coord_in; \n\
  int2 coord_in_uv = coord_in; \n\
  coord_in_uv.x = coord_in_uv.x * 3;\n\
  coord_out.x = coord_out.x * 4;\n\
  vxc_uchar16 lineA0, lineB0;\n\
  VXC_OP4(img_load, lineA0, in_image, coord_in_uv, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineB0, in_image_y, coord_in, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  vxc_float4 tmp1, tmp2, tmp3, tmp4;\n\
  \n\
  VXC_OP4(dp4x4, tmp1, lineA0, lineB0, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0), Y_UV2RGBA); \n\
  vxc_uchar16 outColor;\n\
  outColor.s0 = convert_uchar_rtz(tmp1.x);\n\
  outColor.s1 = convert_uchar_rtz(tmp1.y);\n\
  outColor.s2 = convert_uchar_rtz(tmp1.z);\n\
  outColor.s3 = 255;\n\
  \n\
  vxc_uchar16 lineA1, lineB1;\n\
  lineA1 = lineA0.s3456789abcdef012;\n\
  lineB1 =  lineB0.s123456789abcdef0;\n\
  VXC_OP4(dp4x4, tmp2, lineA1, lineB1, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0), Y_UV2RGBA); \n\
  outColor.s4 = convert_uchar_rtz(tmp2.x);\n\
  outColor.s5 = convert_uchar_rtz(tmp2.y);\n\
  outColor.s6 = convert_uchar_rtz(tmp2.z);\n\
  outColor.s7 = 255;\n\
  \n\
  vxc_uchar16 lineA2, lineB2;\n\
  lineA2 = lineA0.s6789abcdef012345;\n\
  lineB2 =  lineB0.s23456789abcdef01;\n\
  VXC_OP4(dp4x4, tmp3, lineA2, lineB2, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0), Y_UV2RGBA); \n\
  outColor.s8 = convert_uchar_rtz(tmp3.x);\n\
  outColor.s9 = convert_uchar_rtz(tmp3.y);\n\
  outColor.sa = convert_uchar_rtz(tmp3.z);\n\
  outColor.sb = 255;\n\
  \n\
  vxc_uchar16 lineA3, lineB3;\n\
  lineA3 = lineA0.s9abcdef012345678;\n\
  lineB3 =  lineB0.s3456789abcdef012;\n\
  VXC_OP4(dp4x4, tmp4, lineA3, lineB3, VXC_MODIFIER(0, 3, 0, VXC_RM_TowardZero, 0), Y_UV2RGBA); \n\
  outColor.sc = convert_uchar_rtz(tmp4.x);\n\
  outColor.sd = convert_uchar_rtz(tmp4.y);\n\
  outColor.se = convert_uchar_rtz(tmp4.z);\n\
  outColor.sf = 255;\n\
  \n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, outColor, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0)); \n\
}\n\
#define SIGMA 32 //>>11\n\
__kernel void bilateral\n\
  ( \n\
  __read_only image2d_t   in_image, \n\
  __write_only image2d_t  out_image \n\
  ) \n\
{ \n\
  \n\
  int2 coord_in = (int2)(get_global_id(0), get_global_id(1)); \n\
  int2 coord_out = coord_in; \n\
  coord_in.xy = coord_in.xy - 3; \n\
  float med1[7][7] = {1.000, 0.722, 0.556, 0.500, 0.556, 0.722, 1.000,\n\
            0.722, 0.444, 0.278, 0.222, 0.278, 0.444, 0.722,\n\
            0.556, 0.278, 0.111, 0.056, 0.111, 0.278, 0.556,\n\
            0.500, 0.222, 0.056, 0.000, 0.056, 0.222, 0.500,\n\
            0.556, 0.278, 0.111, 0.056, 0.111, 0.278, 0.556,\n\
            0.722, 0.444, 0.278, 0.222, 0.278, 0.444, 0.722,\n\
            1.000, 0.722, 0.556, 0.500, 0.556, 0.722, 1.000};\n\
  vxc_uchar16 lineA, lineB, lineC, lineD, lineE, lineF, lineG;\n\
  VXC_OP4(img_load, lineA, in_image, coord_in, 0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineB, in_image, coord_in, 0x20, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineC, in_image, coord_in, 0X40, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineD, in_image, coord_in, 0x60, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineE, in_image, coord_in, 0x80, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineF, in_image, coord_in, 0xa0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  VXC_OP4(img_load, lineG, in_image, coord_in, 0xc0, VXC_MODIFIER(0, 15, 0, VXC_RM_TowardZero, 0));\n\
  vxc_uchar8 zero = 0;\n\
  vxc_uchar8 tmp[7], lineP, rangeWeight0 = 0, out, test;\n\
  float8  rangeWeight1 = 0, spaceWeight = 0, weight = 0, sumWeight = 0;\n\
  float8 tmpPixel = 0, tmpOut = 0;\n\
  uchar8 test1;\n\
  float8 test0, test2;\n\
  lineP = lineD.s3456789a;\n\
  tmp[0] = lineA.s01234567;\n\
  tmp[1] = lineA.s12345678;\n\
  tmp[2] = lineA.s23456789;\n\
  tmp[3] = lineA.s3456789a;\n\
  tmp[4] = lineA.s456789ab;\n\
  tmp[5] = lineA.s56789abc;\n\
  tmp[6] = lineA.s6789abcd;\n\
  for(int i = 0; i < 7; i++){\n\
    VXC_OP3(abs_diff, rangeWeight0, tmp[i], lineP, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0));\n\
    test0.s0 = convert_float(rangeWeight0.s0);\n\
    test0.s1 = convert_float(rangeWeight0.s1);\n\
    test0.s2 = convert_float(rangeWeight0.s2);\n\
    test0.s3 = convert_float(rangeWeight0.s3);\n\
    test0.s4 = convert_float(rangeWeight0.s4);\n\
    test0.s5 = convert_float(rangeWeight0.s5);\n\
    test0.s6 = convert_float(rangeWeight0.s6);\n\
    test0.s7 = convert_float(rangeWeight0.s7);\n\
    rangeWeight1 = test0 * test0 / (32 * 32 * 2);\n\
    spaceWeight = (float8)med1[0][i];\n\
    weight = exp(-(rangeWeight1 + spaceWeight));\n\
    sumWeight += weight;\n\
    tmpPixel.s0 = convert_float(tmp[i].s0);\n\
    tmpPixel.s1 = convert_float(tmp[i].s1);\n\
    tmpPixel.s2 = convert_float(tmp[i].s2);\n\
    tmpPixel.s3 = convert_float(tmp[i].s3);\n\
    tmpPixel.s4 = convert_float(tmp[i].s4);\n\
    tmpPixel.s5 = convert_float(tmp[i].s5);\n\
    tmpPixel.s6 = convert_float(tmp[i].s6);\n\
    tmpPixel.s7 = convert_float(tmp[i].s7);\n\
    tmpOut += tmpPixel * weight;\n\
  }\n\
  \n\
  tmp[0] = lineB.s01234567;\n\
  tmp[1] = lineB.s12345678;\n\
  tmp[2] = lineB.s23456789;\n\
  tmp[3] = lineB.s3456789a;\n\
  tmp[4] = lineB.s456789ab;\n\
  tmp[5] = lineB.s56789abc;\n\
  tmp[6] = lineB.s6789abcd;\n\
  for(int i = 0; i < 7; i++){\n\
    VXC_OP3(abs_diff, rangeWeight0, tmp[i], lineP, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0));\n\
    test0.s0 = convert_float(rangeWeight0.s0);\n\
    test0.s1 = convert_float(rangeWeight0.s1);\n\
    test0.s2 = convert_float(rangeWeight0.s2);\n\
    test0.s3 = convert_float(rangeWeight0.s3);\n\
    test0.s4 = convert_float(rangeWeight0.s4);\n\
    test0.s5 = convert_float(rangeWeight0.s5);\n\
    test0.s6 = convert_float(rangeWeight0.s6);\n\
    test0.s7 = convert_float(rangeWeight0.s7);\n\
    rangeWeight1 = test0 * test0 / (32 * 32 * 2);\n\
    spaceWeight = (float8)med1[1][i];\n\
    weight = exp(-(rangeWeight1 + spaceWeight));\n\
    sumWeight += weight;\n\
    tmpPixel.s0 = convert_float(tmp[i].s0);\n\
    tmpPixel.s1 = convert_float(tmp[i].s1);\n\
    tmpPixel.s2 = convert_float(tmp[i].s2);\n\
    tmpPixel.s3 = convert_float(tmp[i].s3);\n\
    tmpPixel.s4 = convert_float(tmp[i].s4);\n\
    tmpPixel.s5 = convert_float(tmp[i].s5);\n\
    tmpPixel.s6 = convert_float(tmp[i].s6);\n\
    tmpPixel.s7 = convert_float(tmp[i].s7);\n\
    tmpOut += tmpPixel * weight;\n\
  }\n\
  \n\
  tmp[0] = lineC.s01234567;\n\
  tmp[1] = lineC.s12345678;\n\
  tmp[2] = lineC.s23456789;\n\
  tmp[3] = lineC.s3456789a;\n\
  tmp[4] = lineC.s456789ab;\n\
  tmp[5] = lineC.s56789abc;\n\
  tmp[6] = lineC.s6789abcd;\n\
  for(int i = 0; i < 7; i++){\n\
    VXC_OP3(abs_diff, rangeWeight0, tmp[i], lineP, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0));\n\
    test0.s0 = convert_float(rangeWeight0.s0);\n\
    test0.s1 = convert_float(rangeWeight0.s1);\n\
    test0.s2 = convert_float(rangeWeight0.s2);\n\
    test0.s3 = convert_float(rangeWeight0.s3);\n\
    test0.s4 = convert_float(rangeWeight0.s4);\n\
    test0.s5 = convert_float(rangeWeight0.s5);\n\
    test0.s6 = convert_float(rangeWeight0.s6);\n\
    test0.s7 = convert_float(rangeWeight0.s7);\n\
    rangeWeight1 = test0 * test0 / (32 * 32 * 2);\n\
    spaceWeight = (float8)med1[2][i];\n\
    weight = exp(-(rangeWeight1 + spaceWeight));\n\
    sumWeight += weight;\n\
    tmpPixel.s0 = convert_float(tmp[i].s0);\n\
    tmpPixel.s1 = convert_float(tmp[i].s1);\n\
    tmpPixel.s2 = convert_float(tmp[i].s2);\n\
    tmpPixel.s3 = convert_float(tmp[i].s3);\n\
    tmpPixel.s4 = convert_float(tmp[i].s4);\n\
    tmpPixel.s5 = convert_float(tmp[i].s5);\n\
    tmpPixel.s6 = convert_float(tmp[i].s6);\n\
    tmpPixel.s7 = convert_float(tmp[i].s7);\n\
    tmpOut += tmpPixel * weight;\n\
  }\n\
  \n\
  tmp[0] = lineD.s01234567;\n\
  tmp[1] = lineD.s12345678;\n\
  tmp[2] = lineD.s23456789;\n\
  tmp[3] = lineD.s3456789a;\n\
  tmp[4] = lineD.s456789ab;\n\
  tmp[5] = lineD.s56789abc;\n\
  tmp[6] = lineD.s6789abcd;\n\
  for(int i = 0; i < 7; i++){\n\
    VXC_OP3(abs_diff, rangeWeight0, tmp[i], lineP, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0));\n\
    test0.s0 = convert_float(rangeWeight0.s0);\n\
    test0.s1 = convert_float(rangeWeight0.s1);\n\
    test0.s2 = convert_float(rangeWeight0.s2);\n\
    test0.s3 = convert_float(rangeWeight0.s3);\n\
    test0.s4 = convert_float(rangeWeight0.s4);\n\
    test0.s5 = convert_float(rangeWeight0.s5);\n\
    test0.s6 = convert_float(rangeWeight0.s6);\n\
    test0.s7 = convert_float(rangeWeight0.s7);\n\
    rangeWeight1 = test0 * test0 / (32 * 32 * 2);\n\
    spaceWeight = (float8)med1[3][i];\n\
    weight = exp(-(rangeWeight1 + spaceWeight));\n\
    sumWeight += weight;\n\
    tmpPixel.s0 = convert_float(tmp[i].s0);\n\
    tmpPixel.s1 = convert_float(tmp[i].s1);\n\
    tmpPixel.s2 = convert_float(tmp[i].s2);\n\
    tmpPixel.s3 = convert_float(tmp[i].s3);\n\
    tmpPixel.s4 = convert_float(tmp[i].s4);\n\
    tmpPixel.s5 = convert_float(tmp[i].s5);\n\
    tmpPixel.s6 = convert_float(tmp[i].s6);\n\
    tmpPixel.s7 = convert_float(tmp[i].s7);\n\
    tmpOut += tmpPixel * weight;\n\
  }\n\
  \n\
  tmp[0] = lineE.s01234567;\n\
  tmp[1] = lineE.s12345678;\n\
  tmp[2] = lineE.s23456789;\n\
  tmp[3] = lineE.s3456789a;\n\
  tmp[4] = lineE.s456789ab;\n\
  tmp[5] = lineE.s56789abc;\n\
  tmp[6] = lineE.s6789abcd;\n\
  for(int i = 0; i < 7; i++){\n\
    VXC_OP3(abs_diff, rangeWeight0, tmp[i], lineP, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0));\n\
    test0.s0 = convert_float(rangeWeight0.s0);\n\
    test0.s1 = convert_float(rangeWeight0.s1);\n\
    test0.s2 = convert_float(rangeWeight0.s2);\n\
    test0.s3 = convert_float(rangeWeight0.s3);\n\
    test0.s4 = convert_float(rangeWeight0.s4);\n\
    test0.s5 = convert_float(rangeWeight0.s5);\n\
    test0.s6 = convert_float(rangeWeight0.s6);\n\
    test0.s7 = convert_float(rangeWeight0.s7);\n\
    rangeWeight1 = test0 * test0 / (32 * 32 * 2);\n\
    spaceWeight = (float8)med1[4][i];\n\
    weight = exp(-(rangeWeight1 + spaceWeight));\n\
    sumWeight += weight;\n\
    tmpPixel.s0 = convert_float(tmp[i].s0);\n\
    tmpPixel.s1 = convert_float(tmp[i].s1);\n\
    tmpPixel.s2 = convert_float(tmp[i].s2);\n\
    tmpPixel.s3 = convert_float(tmp[i].s3);\n\
    tmpPixel.s4 = convert_float(tmp[i].s4);\n\
    tmpPixel.s5 = convert_float(tmp[i].s5);\n\
    tmpPixel.s6 = convert_float(tmp[i].s6);\n\
    tmpPixel.s7 = convert_float(tmp[i].s7);\n\
    tmpOut += tmpPixel * weight;\n\
  }\n\
  \n\
  tmp[0] = lineF.s01234567;\n\
  tmp[1] = lineF.s12345678;\n\
  tmp[2] = lineF.s23456789;\n\
  tmp[3] = lineF.s3456789a;\n\
  tmp[4] = lineF.s456789ab;\n\
  tmp[5] = lineF.s56789abc;\n\
  tmp[6] = lineF.s6789abcd;\n\
  for(int i = 0; i < 7; i++){\n\
    VXC_OP3(abs_diff, rangeWeight0, tmp[i], lineP, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0));\n\
    test0.s0 = convert_float(rangeWeight0.s0);\n\
    test0.s1 = convert_float(rangeWeight0.s1);\n\
    test0.s2 = convert_float(rangeWeight0.s2);\n\
    test0.s3 = convert_float(rangeWeight0.s3);\n\
    test0.s4 = convert_float(rangeWeight0.s4);\n\
    test0.s5 = convert_float(rangeWeight0.s5);\n\
    test0.s6 = convert_float(rangeWeight0.s6);\n\
    test0.s7 = convert_float(rangeWeight0.s7);\n\
    rangeWeight1 = test0 * test0 / (32 * 32 * 2);\n\
    spaceWeight = (float8)med1[5][i];\n\
    weight = exp(-(rangeWeight1 + spaceWeight));\n\
    sumWeight += weight;\n\
    tmpPixel.s0 = convert_float(tmp[i].s0);\n\
    tmpPixel.s1 = convert_float(tmp[i].s1);\n\
    tmpPixel.s2 = convert_float(tmp[i].s2);\n\
    tmpPixel.s3 = convert_float(tmp[i].s3);\n\
    tmpPixel.s4 = convert_float(tmp[i].s4);\n\
    tmpPixel.s5 = convert_float(tmp[i].s5);\n\
    tmpPixel.s6 = convert_float(tmp[i].s6);\n\
    tmpPixel.s7 = convert_float(tmp[i].s7);\n\
    tmpOut += tmpPixel * weight;\n\
  }\n\
  \n\
  tmp[0] = lineG.s01234567;\n\
  tmp[1] = lineG.s12345678;\n\
  tmp[2] = lineG.s23456789;\n\
  tmp[3] = lineG.s3456789a;\n\
  tmp[4] = lineG.s456789ab;\n\
  tmp[5] = lineG.s56789abc;\n\
  tmp[6] = lineG.s6789abcd;\n\
  for(int i = 0; i < 7; i++){\n\
    VXC_OP3(abs_diff, rangeWeight0, tmp[i], lineP, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0));\n\
    test0.s0 = convert_float(rangeWeight0.s0);\n\
    test0.s1 = convert_float(rangeWeight0.s1);\n\
    test0.s2 = convert_float(rangeWeight0.s2);\n\
    test0.s3 = convert_float(rangeWeight0.s3);\n\
    test0.s4 = convert_float(rangeWeight0.s4);\n\
    test0.s5 = convert_float(rangeWeight0.s5);\n\
    test0.s6 = convert_float(rangeWeight0.s6);\n\
    test0.s7 = convert_float(rangeWeight0.s7);\n\
    rangeWeight1 = test0 * test0 / (32 * 32 * 2);\n\
    spaceWeight = (float8)med1[6][i];\n\
    weight = exp(-(rangeWeight1 + spaceWeight));\n\
    sumWeight += weight;\n\
    tmpPixel.s0 = convert_float(tmp[i].s0);\n\
    tmpPixel.s1 = convert_float(tmp[i].s1);\n\
    tmpPixel.s2 = convert_float(tmp[i].s2);\n\
    tmpPixel.s3 = convert_float(tmp[i].s3);\n\
    tmpPixel.s4 = convert_float(tmp[i].s4);\n\
    tmpPixel.s5 = convert_float(tmp[i].s5);\n\
    tmpPixel.s6 = convert_float(tmp[i].s6);\n\
    tmpPixel.s7 = convert_float(tmp[i].s7);\n\
    tmpOut += tmpPixel * weight;\n\
  }\n\
  tmpOut = tmpOut / sumWeight;\n\
  out.s0 = convert_uchar(tmpOut.s0);\n\
  out.s1 = convert_uchar(tmpOut.s1);\n\
  out.s2 = convert_uchar(tmpOut.s2);\n\
  out.s3 = convert_uchar(tmpOut.s3);\n\
  out.s4 = convert_uchar(tmpOut.s4);\n\
  out.s5 = convert_uchar(tmpOut.s5);\n\
  out.s6 = convert_uchar(tmpOut.s6);\n\
  out.s7 = convert_uchar(tmpOut.s7);\n\
  VXC_OP4_NoDest(img_store, out_image, coord_out, out, VXC_MODIFIER(0, 7, 0, VXC_RM_TowardZero, 0)); \n\
}\n\
"
};
#endif
