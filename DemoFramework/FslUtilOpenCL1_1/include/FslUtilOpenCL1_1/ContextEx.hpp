#ifndef FSLUTILOPENCL1_1_CONTEXTEX_HPP
#define FSLUTILOPENCL1_1_CONTEXTEX_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtilOpenCL1_1/Common.hpp>
#include <FslUtilOpenCL1_1/Context.hpp>
#include <FslBase/Attributes.hpp>
#include <CL/cl.h>

namespace Fsl
{
  namespace OpenCL
  {
    //! This class is highly experimental and will probably be removed.
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class ContextEx
    {
      cl_platform_id m_platformId;
      Context m_context;
    public:
      ContextEx(const ContextEx&) = delete;
      ContextEx& operator=(const ContextEx&) = delete;

      //! @brief Move assignment operator
      ContextEx& operator=(ContextEx&& other);
      //! @brief Move constructor
      ContextEx(ContextEx&& other);

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      ContextEx();

      ~ContextEx();

      //! @brief Assume control of the Context (this object becomes responsible for releasing it)
      explicit ContextEx(const cl_platform_id platformId, const cl_context context);

      //! @brief Create the requested resource
      ContextEx(const cl_device_type deviceType);

      //! @brief returns the managed handle and releases the ownership.
      cl_context Release() FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
      {
        m_platformId = nullptr;
        return m_context.Release();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset()
      {
        m_platformId = nullptr;
        m_context.Reset();
      }

      //! @brief Destroys any owned resources and assume control of the ContextEx (this object becomes responsible for releasing it)
      void Reset(const cl_platform_id platformId, const cl_context context)
      {
        m_context.Reset(context);
        m_platformId = platformId;
      }

      // ! @brief Destroys any owned resources and then creates the requested one
      void Reset(const cl_device_type deviceType);

      //! @brief Get the associated resource handle
      cl_platform_id GetPlatformId() const
      {
        return m_platformId;
      }

      //! @brief Get the associated resource handle
      cl_context Get() const
      {
        return m_context.Get();
      }

      //! @brief Check if this object contains a valid resource
      bool IsValid() const
      {
        return m_context.IsValid();
      }
    };
  }
}

#endif
