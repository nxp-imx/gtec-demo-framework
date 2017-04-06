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

#include <FslUtilOpenCL1_1/Check.hpp>
#include <FslUtilOpenCL1_1/ProgramEx.hpp>
#include <FslUtilOpenCL1_1/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>
#include <utility>

namespace Fsl
{
  namespace OpenCL
  {
    using namespace OpenCL;

    namespace
    {
      cl_program Build(const cl_context context, const cl_device_id deviceId, const std::string& strProgram)
      {
        const char* programCodeArray[1] = { strProgram.c_str() };
        const std::size_t programLengthArray[1] = { strProgram.size() };

        // Create the program for all GPUs in the context
        cl_int errorCode = CL_SUCCESS;
        const auto hProgram = clCreateProgramWithSource(context, 1, programCodeArray, programLengthArray, &errorCode);
        FSLUTILOPENCL_CHECK2(errorCode, "clCreateProgramWithSource");
        if (hProgram == nullptr)
          throw OpenCLErrorException("clCreateProgramWithSource returned a invalid handle", errorCode, __FILE__, __LINE__);

        errorCode = clBuildProgram(hProgram, 0, nullptr, "", nullptr, nullptr);
        if (errorCode != CL_SUCCESS)
        {
          FSLLOG("*** Source start ***\n" << strProgram << "\n*** Source end ***\n\n");

          // Determine the size of the log
          std::size_t logSize;
          FSLUTILOPENCL_CHECK(clGetProgramBuildInfo(hProgram, deviceId, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize));

          std::vector<char> errorLog(std::max(logSize, static_cast<std::size_t>(1)));

          // Retrieve the log
          FSLUTILOPENCL_CHECK(clGetProgramBuildInfo(hProgram, deviceId, CL_PROGRAM_BUILD_LOG, errorLog.size(), errorLog.data(), nullptr));
          FSLLOG("*** Error log start ***\n" << &errorLog[0] << "\n*** Error Log End ***\n\n");

          throw OpenCLErrorException("clBuildProgram failed", errorCode, __FILE__, __LINE__);
        }
        return hProgram;
      }
    }


    // move assignment operator
    ProgramEx& ProgramEx::operator=(ProgramEx&& other)
    {
      if (this != &other)
      {
        // This works because this is the only member variable
        m_program = std::move(other.m_program);
      }
      return *this;
    }


    // Transfer ownership from other to this
    ProgramEx::ProgramEx(ProgramEx&& other)
      : m_program(std::move(other.m_program))
    {
    }


    ProgramEx::ProgramEx()
      : m_program()
    {
    }


    ProgramEx::ProgramEx(const cl_program program)
      : m_program(program)
    {
    }


    ProgramEx::ProgramEx(const cl_context context, const cl_device_id deviceId, const std::string& program)
      : m_program(Build(context, deviceId, program))
    {
    }


    void ProgramEx::Reset(const cl_context context, const cl_device_id deviceId, const std::string& program)
    {
      // Free any currently allocated resource
      if (m_program.IsValid())
        m_program.Reset();

      m_program.Reset(Build(context, deviceId, program));
    }

  }
}
