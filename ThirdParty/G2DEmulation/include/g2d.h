/*
 *  Copyright (C) 2013-2015 Freescale Semiconductor, Inc.
 *  All Rights Reserved.
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
 */

/*
 *	g2d.h
 *	Gpu 2d header file declare all g2d APIs exposed to application
 *	History :
 *	Date(y.m.d)        Author            Version        Description
 *	2012-10-22         Li Xianzhong      0.1            Created
 *	2013-02-22         Li Xianzhong      0.2            g2d_copy API is added
 *	2013-03-21         Li Xianzhong      0.4            g2d clear/rotation/flip APIs are supported
 *	2013-04-09         Li Xianzhong      0.5            g2d alpha blending feature is enhanced
 *	2013-05-17         Li Xianzhong      0.6            support vg core in g2d library
 *	2013-12-23         Li Xianzhong      0.7            support blend dim feature
 *	2014-03-20         Li Xianzhong      0.8            support pre-multipied & de-mutlipy out alpha
 *  2015-04-10         Meng Mingming     0.9            support multiple source blit
 *  2015-11-03         Meng Mingming     1.0            support query 2D hardware type and feature
 *  2016-05-24         Meng Mingming     1.1            support get g2d_buf from dma fd
 */

#ifndef __G2D_H__
#define __G2D_H__

#ifdef __cplusplus
extern "C" {
#endif

#define G2D_VERSION_MAJOR 1
#define G2D_VERSION_MINOR 1
#define G2D_VERSION_PATCH 0

enum g2d_format
{
  // rgb formats
  G2D_RGB565 = 0,
  G2D_RGBA8888 = 1,
  G2D_RGBX8888 = 2,
  G2D_BGRA8888 = 3,
  G2D_BGRX8888 = 4,
  G2D_BGR565 = 5,

  G2D_ARGB8888 = 6,
  G2D_ABGR8888 = 7,
  G2D_XRGB8888 = 8,
  G2D_XBGR8888 = 9,
  G2D_RGB888 = 10,

  // yuv formats
  G2D_NV12 = 20,
  G2D_I420 = 21,
  G2D_YV12 = 22,
  G2D_NV21 = 23,
  G2D_YUYV = 24,
  G2D_YVYU = 25,
  G2D_UYVY = 26,
  G2D_VYUY = 27,
  G2D_NV16 = 28,
  G2D_NV61 = 29,
};

enum g2d_blend_func
{
  // basic blend
  G2D_ZERO = 0,
  G2D_ONE = 1,
  G2D_SRC_ALPHA = 2,
  G2D_ONE_MINUS_SRC_ALPHA = 3,
  G2D_DST_ALPHA = 4,
  G2D_ONE_MINUS_DST_ALPHA = 5,

  // extensive blend is set with basic blend together,
  // such as, G2D_ONE | G2D_PRE_MULTIPLIED_ALPHA
  G2D_PRE_MULTIPLIED_ALPHA = 0x10,
  G2D_DEMULTIPLY_OUT_ALPHA = 0x20,
};

enum g2d_cap_mode
{
  G2D_BLEND = 0,
  G2D_DITHER = 1,
  G2D_GLOBAL_ALPHA = 2,    // only support source global alpha
  G2D_BLEND_DIM = 3,       // support special blend effect
  G2D_BLUR = 4,            // blur effect
};

enum g2d_feature
{
  G2D_SCALING = 0,
  G2D_ROTATION,
  G2D_SRC_YUV,
  G2D_DST_YUV,
  G2D_MULTI_SOURCE_BLT,
};

enum g2d_rotation
{
  G2D_ROTATION_0 = 0,
  G2D_ROTATION_90 = 1,
  G2D_ROTATION_180 = 2,
  G2D_ROTATION_270 = 3,
  G2D_FLIP_H = 4,
  G2D_FLIP_V = 5,
};

enum g2d_cache_mode
{
  G2D_CACHE_CLEAN = 0,
  G2D_CACHE_FLUSH = 1,
  G2D_CACHE_INVALIDATE = 2,
};

enum g2d_hardware_type
{
  G2D_HARDWARE_2D = 0,    // default type
  G2D_HARDWARE_VG = 1,
};

struct g2d_surface
{
  enum g2d_format format;

  int planes[3];    // surface buffer addresses are set in physical planes separately
                    // RGB:  planes[0] - RGB565/RGBA8888/RGBX8888/BGRA8888/BRGX8888
                    // NV12: planes[0] - Y, planes[1] - packed UV
                    // I420: planes[0] - Y, planes[1] - U, planes[2] - V
                    // YV12: planes[0] - Y, planes[1] - V, planes[2] - U
                    // NV21: planes[0] - Y, planes[1] - packed VU
                    // YUYV: planes[0] - packed YUYV
                    // YVYU: planes[0] - packed YVYU
                    // UYVY: planes[0] - packed UYVY
                    // VYUY: planes[0] - packed VYUY
                    // NV16: planes[0] - Y, planes[1] - packed UV
                    // NV61: planes[0] - Y, planes[1] - packed VU

  // blit rectangle in surface
  int left;
  int top;
  int right;
  int bottom;

  int stride;    // surface buffer stride

  int width;     // surface width
  int height;    // surface height

  // alpha blending parameters
  enum g2d_blend_func blendfunc;

  // the global alpha value is 0 ~ 255
  int global_alpha;

  // clrcolor format is RGBA8888, used as dst for clear, as src for blend dim
  int clrcolor;

  // rotation degree
  enum g2d_rotation rot;
};

struct g2d_surface_pair
{
  struct g2d_surface s;
  struct g2d_surface d;
};

struct g2d_buf
{
  void* buf_handle;
  void* buf_vaddr;
  int buf_paddr;
  int buf_size;
};

int g2d_open(void** handle);
int g2d_close(void* handle);

int g2d_make_current(void* handle, enum g2d_hardware_type type);

int g2d_clear(void* handle, struct g2d_surface* area);
int g2d_blit(void* handle, struct g2d_surface* src, struct g2d_surface* dst);
int g2d_copy(void* handle, struct g2d_buf* d, struct g2d_buf* s, int size);
int g2d_multi_blit(void* handle, struct g2d_surface_pair* sp[], int layers);

int g2d_query_hardware(void* handle, enum g2d_hardware_type type, int* available);
int g2d_query_feature(void* handle, enum g2d_feature feature, int* available);
int g2d_query_cap(void* handle, enum g2d_cap_mode cap, int* enable);
int g2d_enable(void* handle, enum g2d_cap_mode cap);
int g2d_disable(void* handle, enum g2d_cap_mode cap);

// FIX: deprecated g2d_cache_op, its actually not part of this header anymore
int g2d_cache_op(struct g2d_buf* buf, enum g2d_cache_mode op);
struct g2d_buf* g2d_alloc(int size, int cacheable);
struct g2d_buf* g2d_buf_from_fd(int fd);
int g2d_free(struct g2d_buf* buf);

int g2d_flush(void* handle);
int g2d_finish(void* handle);

#ifdef __cplusplus
}
#endif

#endif
