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

#include <g2d.h>
#include <cstdio>

extern "C" int g2d_open(void** handle)
{
  return -1;
}


extern "C" int g2d_close(void* handle)
{
  return 0;
}


extern "C" int g2d_make_current(void* handle, enum g2d_hardware_type type)
{
  return 0;
}


extern "C" int g2d_clear(void* handle, struct g2d_surface* area)
{
  return 0;
}


extern "C" int g2d_blit(void* handle, struct g2d_surface* src, struct g2d_surface* dst)
{
  return 0;
}


extern "C" int g2d_copy(void* handle, struct g2d_buf* d, struct g2d_buf* s, int size)
{
  return 0;
}


extern "C" int g2d_multi_blit(void* handle, struct g2d_surface_pair* sp[], int layers)
{
  return 0;
}


extern "C" int g2d_query_hardware(void* handle, enum g2d_hardware_type type, int* available)
{
  return 0;
}


extern "C" int g2d_query_feature(void* handle, enum g2d_feature feature, int* available)
{
  return 0;
}


extern "C" int g2d_query_cap(void* handle, enum g2d_cap_mode cap, int* enable)
{
  return 0;
}

extern "C" int g2d_enable(void* handle, enum g2d_cap_mode cap)
{
  return 0;
}

extern "C" int g2d_disable(void* handle, enum g2d_cap_mode cap)
{
  return 0;
}


extern "C" int g2d_cache_op(struct g2d_buf* buf, enum g2d_cache_mode op)
{
  return 0;
}

extern "C" struct g2d_buf* g2d_alloc(int size, int cacheable)
{
  printf("g2d_alloc not implemented");
  return nullptr;
}

g2d_buf* g2d_buf_from_fd(int fd)
{
  printf("g2d_buf_from_fd not implemented");
  return nullptr;
}


extern "C" int g2d_free(struct g2d_buf* buf)
{
  return 0;
}


extern "C" int g2d_flush(void* handle)
{
  return 0;
}


extern "C" int g2d_finish(void* handle)
{
  return 0;
}
