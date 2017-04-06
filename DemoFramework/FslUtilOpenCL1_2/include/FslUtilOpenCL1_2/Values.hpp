#ifndef FSLUTILOPENCL1_2_VALUES_HPP
#define FSLUTILOPENCL1_2_VALUES_HPP
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

// Based on RapidOpenCL https://github.com/Unarmed1000/RapidOpenCL with permission.

#include <CL/cl.h>

namespace Fsl
{
  namespace OpenCL
  {
    namespace Values
    {
      static const cl_command_queue INVALID_COMMAND_QUEUE = nullptr;
      static const cl_context INVALID_CONTEXT = nullptr;
      static const cl_device_id INVALID_DEVICE_ID = nullptr;
      static const cl_event INVALID_EVENT = nullptr;
      static const cl_kernel INVALID_KERNEL = nullptr;
      static const cl_mem INVALID_MEM = nullptr;
      static const cl_platform_id INVALID_PLATFORM_ID = nullptr;
      static const cl_program INVALID_PROGRAM = nullptr;
      static const cl_sampler INVALID_SAMPLER = nullptr;
    };
  }
}

#endif
