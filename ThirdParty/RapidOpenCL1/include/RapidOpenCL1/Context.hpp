#ifndef RAPIDOPENCL1_CONTEXT_HPP
#define RAPIDOPENCL1_CONTEXT_HPP
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

// Auto-generated OpenCL 1 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000/RAIIGen)

#include <RapidOpenCL1/CustomTypes.hpp>
#include <RapidOpenCL1/CheckError.hpp>
#include <RapidOpenCL1/System/Macro.hpp>
#include <CL/cl.h>
#include <cassert>

namespace RapidOpenCL1
{
  //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
  class Context
  {
    cl_context m_context;
  public:
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    //! @brief Move assignment operator
    Context& operator=(Context&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_context = other.m_context;

        // Remove the data from other
        other.m_context = nullptr;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Context(Context&& other)
      : m_context(other.m_context)
    {
      // Remove the data from other
      other.m_context = nullptr;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Context()
      : m_context(nullptr)
    {
    }

    //! @brief Assume control of the Context (this object becomes responsible for releasing it)
    explicit Context(const cl_context context)
      : Context()
    {
      Reset(context);
    }

    //! @brief Create the requested resource
    //! @note  Function: clCreateContext
    Context(const cl_context_properties * pContextProperties, const cl_uint numDevices, const cl_device_id * pDeviceId, FNOpenCLCreateCallback pfnNotify, void * pUserData)
      : Context()
    {
      Reset(pContextProperties, numDevices, pDeviceId, pfnNotify, pUserData);
    }

    //! @brief Create the requested resource
    //! @note  Function: clCreateContextFromType
    Context(const cl_context_properties * pContextProperties, const cl_device_type deviceType, FNOpenCLCreateCallback pfnNotify, void * pUserData)
      : Context()
    {
      Reset(pContextProperties, deviceType, pfnNotify, pUserData);
    }

    ~Context()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    cl_context Release() RAPIDOPENCL_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_context;
      m_context = nullptr;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_context != nullptr);

      clReleaseContext(m_context);
      m_context = nullptr;
    }

    //! @brief Destroys any owned resources and assume control of the Context (this object becomes responsible for releasing it)
    void Reset(const cl_context context)
    {
      if (IsValid())
        Reset();


      m_context = context;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: clCreateContext
    void Reset(const cl_context_properties * pContextProperties, const cl_uint numDevices, const cl_device_id * pDeviceId, FNOpenCLCreateCallback pfnNotify, void * pUserData)
    {
      // We do the check here to be user friendly, if it becomes a performance issue switch it to a assert.

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      cl_int errorCode;
      const cl_context context = clCreateContext(pContextProperties, numDevices, pDeviceId, pfnNotify, pUserData, &errorCode);
      CheckError(errorCode, "clCreateContext", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_context = context;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: clCreateContextFromType
    void Reset(const cl_context_properties * pContextProperties, const cl_device_type deviceType, FNOpenCLCreateCallback pfnNotify, void * pUserData)
    {
      // We do the check here to be user friendly, if it becomes a performance issue switch it to a assert.

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      cl_int errorCode;
      const cl_context context = clCreateContextFromType(pContextProperties, deviceType, pfnNotify, pUserData, &errorCode);
      CheckError(errorCode, "clCreateContextFromType", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_context = context;
    }

    //! @brief Get the associated resource handle
    cl_context Get() const
    {
      return m_context;
    }

    //! @brief Get a pointer to the associated resource handle
    const cl_context* GetPointer() const
    {
      return &m_context;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_context != nullptr;
    }

    //! @note  Function: clRetainContext
    cl_int RetainContext()
    {
      return clRetainContext(m_context);
    }

    //! @note  Function: clGetContextInfo
    cl_int GetContextInfo(const cl_context_info contextInfo, const size_t size, void * pVoid, size_t * pSize)
    {
      return clGetContextInfo(m_context, contextInfo, size, pVoid, pSize);
    }

    //! @note  Function: clGetSupportedImageFormats
    cl_int GetSupportedImageFormats(const cl_mem_flags memFlags, const cl_mem_object_type memObjectType, const cl_uint uint, cl_image_format * pImageFormat, cl_uint * pUint)
    {
      return clGetSupportedImageFormats(m_context, memFlags, memObjectType, uint, pImageFormat, pUint);
    }


#if CL_VERSION_1_2
    //! @note  Function: clLinkProgram
    cl_program LinkProgram(const cl_uint numDevices, const cl_device_id * pDeviceList, const char * pOptions, const cl_uint numInputPrograms, const cl_program * pInputPrograms, FNOpenCLLinkProgramCallback pfnNotify, void * pUserData, cl_int * pErrorCode)
    {
      return clLinkProgram(m_context, numDevices, pDeviceList, pOptions, numInputPrograms, pInputPrograms, pfnNotify, pUserData, pErrorCode);
    }
#endif
  };
}

#endif
