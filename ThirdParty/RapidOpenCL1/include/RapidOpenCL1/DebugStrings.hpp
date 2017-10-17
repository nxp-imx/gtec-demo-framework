#ifndef RAPIDOPENCL1_DEBUGSTRINGS_HPP
#define RAPIDOPENCL1_DEBUGSTRINGS_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2016, Rene Thrane
//* All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
//*    documentation and/or other materials provided with the distribution.
//* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
//*    software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//***************************************************************************************************************************************************

#include <CL/cl.h>

namespace RapidOpenCL1
{
  //! Extend the debug namespace with 'convenience' methods.
  //! This file adds conversion methods that helps transform various things to a nice string representation.
  //! WARNING: this requires you to compile the CPP file.
  namespace Debug
  {
    //! @brief Convert the error code to a string
    inline const char* ErrorCodeToString(const cl_int errorCode)
    {
      switch (errorCode)
      {
      case CL_SUCCESS:
        return "CL_SUCCESS";
      case CL_DEVICE_NOT_FOUND:
        return "CL_DEVICE_NOT_FOUND";
      case CL_DEVICE_NOT_AVAILABLE:
        return "CL_DEVICE_NOT_AVAILABLE";
      case CL_COMPILER_NOT_AVAILABLE:
        return "CL_COMPILER_NOT_AVAILABLE";
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
      case CL_OUT_OF_RESOURCES:
        return "CL_OUT_OF_RESOURCES";
      case CL_OUT_OF_HOST_MEMORY:
        return "CL_OUT_OF_HOST_MEMORY";
      case CL_PROFILING_INFO_NOT_AVAILABLE:
        return "CL_PROFILING_INFO_NOT_AVAILABLE";
      case CL_MEM_COPY_OVERLAP:
        return "CL_MEM_COPY_OVERLAP";
      case CL_IMAGE_FORMAT_MISMATCH:
        return "CL_IMAGE_FORMAT_MISMATCH";
      case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
      case CL_BUILD_PROGRAM_FAILURE:
        return "CL_BUILD_PROGRAM_FAILURE";
      case CL_MAP_FAILURE:
        return "CL_MAP_FAILURE";
      case CL_MISALIGNED_SUB_BUFFER_OFFSET:
        return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
      case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
        return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
#ifdef CL_COMPILE_PROGRAM_FAILURE
      case CL_COMPILE_PROGRAM_FAILURE:
        return "CL_COMPILE_PROGRAM_FAILURE";
#endif
#ifdef CL_LINKER_NOT_AVAILABLE
      case CL_LINKER_NOT_AVAILABLE:
        return "CL_LINKER_NOT_AVAILABLE";
#endif
#ifdef CL_LINK_PROGRAM_FAILURE
      case CL_LINK_PROGRAM_FAILURE:
        return "CL_LINK_PROGRAM_FAILURE";
#endif
#ifdef CL_DEVICE_PARTITION_FAILED
      case CL_DEVICE_PARTITION_FAILED:
        return "CL_DEVICE_PARTITION_FAILED";
#endif
#ifdef CL_KERNEL_ARG_INFO_NOT_AVAILABLE
      case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
        return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
#endif
      case CL_INVALID_VALUE:
        return "CL_INVALID_VALUE";
      case CL_INVALID_DEVICE_TYPE:
        return "CL_INVALID_DEVICE_TYPE";
      case CL_INVALID_PLATFORM:
        return "CL_INVALID_PLATFORM";
      case CL_INVALID_DEVICE:
        return "CL_INVALID_DEVICE";
      case CL_INVALID_CONTEXT:
        return "CL_INVALID_CONTEXT";
      case CL_INVALID_QUEUE_PROPERTIES:
        return "CL_INVALID_QUEUE_PROPERTIES";
      case CL_INVALID_COMMAND_QUEUE:
        return "CL_INVALID_COMMAND_QUEUE";
      case CL_INVALID_HOST_PTR:
        return "CL_INVALID_HOST_PTR";
      case CL_INVALID_MEM_OBJECT:
        return "CL_INVALID_MEM_OBJECT";
      case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
      case CL_INVALID_IMAGE_SIZE:
        return "CL_INVALID_IMAGE_SIZE";
      case CL_INVALID_SAMPLER:
        return "CL_INVALID_SAMPLER";
      case CL_INVALID_BINARY:
        return "CL_INVALID_BINARY";
      case CL_INVALID_BUILD_OPTIONS:
        return "CL_INVALID_BUILD_OPTIONS";
      case CL_INVALID_PROGRAM:
        return "CL_INVALID_PROGRAM";
      case CL_INVALID_PROGRAM_EXECUTABLE:
        return "CL_INVALID_PROGRAM_EXECUTABLE";
      case CL_INVALID_KERNEL_NAME:
        return "CL_INVALID_KERNEL_NAME";
      case CL_INVALID_KERNEL_DEFINITION:
        return "CL_INVALID_KERNEL_DEFINITION";
      case CL_INVALID_KERNEL:
        return "CL_INVALID_KERNEL";
      case CL_INVALID_ARG_INDEX:
        return "CL_INVALID_ARG_INDEX";
      case CL_INVALID_ARG_VALUE:
        return "CL_INVALID_ARG_VALUE";
      case CL_INVALID_ARG_SIZE:
        return "CL_INVALID_ARG_SIZE";
      case CL_INVALID_KERNEL_ARGS:
        return "CL_INVALID_KERNEL_ARGS";
      case CL_INVALID_WORK_DIMENSION:
        return "CL_INVALID_WORK_DIMENSION";
      case CL_INVALID_WORK_GROUP_SIZE:
        return "CL_INVALID_WORK_GROUP_SIZE";
      case CL_INVALID_WORK_ITEM_SIZE:
        return "CL_INVALID_WORK_ITEM_SIZE";
      case CL_INVALID_GLOBAL_OFFSET:
        return "CL_INVALID_GLOBAL_OFFSET";
      case CL_INVALID_EVENT_WAIT_LIST:
        return "CL_INVALID_EVENT_WAIT_LIST";
      case CL_INVALID_EVENT:
        return "CL_INVALID_EVENT";
      case CL_INVALID_OPERATION:
        return "CL_INVALID_OPERATION";
      case CL_INVALID_GL_OBJECT:
        return "CL_INVALID_GL_OBJECT";
      case CL_INVALID_BUFFER_SIZE:
        return "CL_INVALID_BUFFER_SIZE";
      case CL_INVALID_MIP_LEVEL:
        return "CL_INVALID_MIP_LEVEL";
      case CL_INVALID_GLOBAL_WORK_SIZE:
        return "CL_INVALID_GLOBAL_WORK_SIZE";
      case CL_INVALID_PROPERTY:
        return "CL_INVALID_PROPERTY";
#ifdef CL_INVALID_IMAGE_DESCRIPTOR
      case CL_INVALID_IMAGE_DESCRIPTOR:
        return "CL_INVALID_IMAGE_DESCRIPTOR";
#endif
#ifdef CL_INVALID_COMPILER_OPTIONS
      case CL_INVALID_COMPILER_OPTIONS:
        return "CL_INVALID_COMPILER_OPTIONS";
#endif
#ifdef CL_INVALID_LINKER_OPTIONS
      case CL_INVALID_LINKER_OPTIONS:
        return "CL_INVALID_LINKER_OPTIONS";
#endif
#ifdef CL_INVALID_DEVICE_PARTITION_COUNT
      case CL_INVALID_DEVICE_PARTITION_COUNT:
        return "CL_INVALID_DEVICE_PARTITION_COUNT";
#endif
      default:
        return "Unknown";
      }
    }    

    //! @brief Convert the device type to a string
    inline const char* DeviceTypeTostring(const cl_device_type deviceType)
    {
      switch (deviceType)
      {
#ifdef CL_DEVICE_TYPE_CPU
      case CL_DEVICE_TYPE_CPU:
        return "CL_DEVICE_TYPE_CPU";
#endif
#ifdef CL_DEVICE_TYPE_GPU
      case CL_DEVICE_TYPE_GPU:
        return "CL_DEVICE_TYPE_GPU";
#endif
#ifdef CL_DEVICE_TYPE_ACCELERATOR
      case CL_DEVICE_TYPE_ACCELERATOR:
        return "CL_DEVICE_TYPE_ACCELERATOR";
#endif
#ifdef CL_DEVICE_TYPE_CUSTOM
      case CL_DEVICE_TYPE_CUSTOM:
        return "CL_DEVICE_TYPE_CUSTOM";
#endif
#ifdef CL_DEVICE_TYPE_ALL
      case CL_DEVICE_TYPE_ALL:
        return "CL_DEVICE_TYPE_ALL";
#endif
      default:
        return "Unknown";
      }
    }
  }
}

#endif
