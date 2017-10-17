#ifndef RAPIDOPENCL1_SAMPLER_HPP
#define RAPIDOPENCL1_SAMPLER_HPP
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
  class Sampler
  {
    cl_sampler m_sampler;
  public:
    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    //! @brief Move assignment operator
    Sampler& operator=(Sampler&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_sampler = other.m_sampler;

        // Remove the data from other
        other.m_sampler = nullptr;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Sampler(Sampler&& other)
      : m_sampler(other.m_sampler)
    {
      // Remove the data from other
      other.m_sampler = nullptr;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Sampler()
      : m_sampler(nullptr)
    {
    }

    //! @brief Assume control of the Sampler (this object becomes responsible for releasing it)
    explicit Sampler(const cl_sampler sampler)
      : Sampler()
    {
      Reset(sampler);
    }

    //! @brief Create the requested resource
    //! @note  Function: clCreateSampler
    Sampler(const cl_context context, const cl_bool normalizedCoords, const cl_addressing_mode addressingMode, const cl_filter_mode filterMode)
      : Sampler()
    {
      Reset(context, normalizedCoords, addressingMode, filterMode);
    }

    ~Sampler()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    cl_sampler Release() RAPIDOPENCL_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_sampler;
      m_sampler = nullptr;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_sampler != nullptr);

      clReleaseSampler(m_sampler);
      m_sampler = nullptr;
    }

    //! @brief Destroys any owned resources and assume control of the Sampler (this object becomes responsible for releasing it)
    void Reset(const cl_sampler sampler)
    {
      if (IsValid())
        Reset();


      m_sampler = sampler;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: clCreateSampler
    void Reset(const cl_context context, const cl_bool normalizedCoords, const cl_addressing_mode addressingMode, const cl_filter_mode filterMode)
    {
      // We do the check here to be user friendly, if it becomes a performance issue switch it to a assert.

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      cl_int errorCode;
      const cl_sampler sampler = clCreateSampler(context, normalizedCoords, addressingMode, filterMode, &errorCode);
      CheckError(errorCode, "clCreateSampler", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_sampler = sampler;
    }

    //! @brief Get the associated resource handle
    cl_sampler Get() const
    {
      return m_sampler;
    }

    //! @brief Get a pointer to the associated resource handle
    const cl_sampler* GetPointer() const
    {
      return &m_sampler;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_sampler != nullptr;
    }

    //! @note  Function: clRetainSampler
    cl_int RetainSampler()
    {
      return clRetainSampler(m_sampler);
    }

    //! @note  Function: clGetSamplerInfo
    cl_int GetSamplerInfo(const cl_sampler_info samplerInfo, const size_t size, void * pVoid, size_t * pSize)
    {
      return clGetSamplerInfo(m_sampler, samplerInfo, size, pVoid, pSize);
    }
  };
}

#endif
