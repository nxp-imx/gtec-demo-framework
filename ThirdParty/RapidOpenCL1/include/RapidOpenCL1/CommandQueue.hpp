#ifndef RAPIDOPENCL1_COMMANDQUEUE_HPP
#define RAPIDOPENCL1_COMMANDQUEUE_HPP
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
  class CommandQueue
  {
    cl_command_queue m_commandQueue;
  public:
    CommandQueue(const CommandQueue&) = delete;
    CommandQueue& operator=(const CommandQueue&) = delete;

    //! @brief Move assignment operator
    CommandQueue& operator=(CommandQueue&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_commandQueue = other.m_commandQueue;

        // Remove the data from other
        other.m_commandQueue = nullptr;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    CommandQueue(CommandQueue&& other)
      : m_commandQueue(other.m_commandQueue)
    {
      // Remove the data from other
      other.m_commandQueue = nullptr;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    CommandQueue()
      : m_commandQueue(nullptr)
    {
    }

    //! @brief Assume control of the CommandQueue (this object becomes responsible for releasing it)
    explicit CommandQueue(const cl_command_queue commandQueue)
      : CommandQueue()
    {
      Reset(commandQueue);
    }

    //! @brief Create the requested resource
    //! @note  Function: clCreateCommandQueue
    CommandQueue(const cl_context context, const cl_device_id deviceId, const cl_command_queue_properties commandQueueProperties)
      : CommandQueue()
    {
      Reset(context, deviceId, commandQueueProperties);
    }

    ~CommandQueue()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    cl_command_queue Release() RAPIDOPENCL_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_commandQueue;
      m_commandQueue = nullptr;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_commandQueue != nullptr);

      clReleaseCommandQueue(m_commandQueue);
      m_commandQueue = nullptr;
    }

    //! @brief Destroys any owned resources and assume control of the CommandQueue (this object becomes responsible for releasing it)
    void Reset(const cl_command_queue commandQueue)
    {
      if (IsValid())
        Reset();


      m_commandQueue = commandQueue;
    }

#ifdef _WIN32
#pragma warning( push )
    // Disable the warning about deprecation as its not deprecated in 1.1
#pragma warning( disable : 4996)
#endif

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: clCreateCommandQueue
    void Reset(const cl_context context, const cl_device_id deviceId, const cl_command_queue_properties commandQueueProperties)
    {
      // We do the check here to be user friendly, if it becomes a performance issue switch it to a assert.

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      cl_int errorCode;
      const cl_command_queue commandQueue = clCreateCommandQueue(context, deviceId, commandQueueProperties, &errorCode);
      CheckError(errorCode, "clCreateCommandQueue", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_commandQueue = commandQueue;
    }

#ifdef _WIN32
#pragma warning( pop )
#endif

    //! @brief Get the associated resource handle
    cl_command_queue Get() const
    {
      return m_commandQueue;
    }

    //! @brief Get a pointer to the associated resource handle
    const cl_command_queue* GetPointer() const
    {
      return &m_commandQueue;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_commandQueue != nullptr;
    }

    //! @note  Function: clRetainCommandQueue
    cl_int RetainCommandQueue()
    {
      return clRetainCommandQueue(m_commandQueue);
    }

    //! @note  Function: clGetCommandQueueInfo
    cl_int GetCommandQueueInfo(const cl_command_queue_info commandQueueInfo, const size_t size, void * pVoid, size_t * pSize)
    {
      return clGetCommandQueueInfo(m_commandQueue, commandQueueInfo, size, pVoid, pSize);
    }

    //! @note  Function: clFlush
    cl_int Flush()
    {
      return clFlush(m_commandQueue);
    }

    //! @note  Function: clFinish
    cl_int Finish()
    {
      return clFinish(m_commandQueue);
    }


#if CL_VERSION_1_2
    //! @note  Function: clEnqueueMigrateMemObjects
    cl_int EnqueueMigrateMemObjects(const cl_uint numMemObjects, const cl_mem * pMemObjects, const cl_mem_migration_flags memMigrationFlags, const cl_uint numEventsInWaitList, const cl_event * pEventWaitList, cl_event * pEvent)
    {
      return clEnqueueMigrateMemObjects(m_commandQueue, numMemObjects, pMemObjects, memMigrationFlags, numEventsInWaitList, pEventWaitList, pEvent);
    }
#endif


#if CL_VERSION_1_2
    //! @note  Function: clEnqueueMarkerWithWaitList
    cl_int EnqueueMarkerWithWaitList(const cl_uint numEventsInWaitList, const cl_event * pEventWaitList, cl_event * pEvent)
    {
      return clEnqueueMarkerWithWaitList(m_commandQueue, numEventsInWaitList, pEventWaitList, pEvent);
    }
#endif


#if CL_VERSION_1_2
    //! @note  Function: clEnqueueBarrierWithWaitList
    cl_int EnqueueBarrierWithWaitList(const cl_uint numEventsInWaitList, const cl_event * pEventWaitList, cl_event * pEvent)
    {
      return clEnqueueBarrierWithWaitList(m_commandQueue, numEventsInWaitList, pEventWaitList, pEvent);
    }
#endif


    //! @note  Function: clEnqueueWaitForEvents
    // cl_int EnqueueWaitForEvents(const cl_uint uint, const cl_event * pEvent)
    // {
    //   return clEnqueueWaitForEvents(m_commandQueue, uint, pEvent);
    // }

    //! @note  Function: clEnqueueBarrier
    // cl_int EnqueueBarrier()
    // {
    //   return clEnqueueBarrier(m_commandQueue);
    // }

  };
}

#endif
