/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <Shared/System/CustomAppServices/Global/GlobalExampleService.hpp>
#include <fmt/std.h>    // to allow us to print a std::thread::id

namespace Fsl
{
  GlobalExampleService::GlobalExampleService(const ServiceProvider& serviceProvider)
    : AThreadSafeSynchronousService(serviceProvider)
  {
    FSLLOG3_INFO("GlobalExampleService: created (ThreadId: {})", std::this_thread::get_id());
  }


  GlobalExampleService::~GlobalExampleService()
  {
    FSLLOG3_INFO("GlobalExampleService: destroyed (ThreadId: {})", std::this_thread::get_id());
  }


  int32_t GlobalExampleService::Add(const int32_t a, const int32_t b) const
  {
    //! global services need to be thread safe, so we take a mutex lock here
    std::lock_guard<std::mutex> lock(m_mutex);

    const int32_t result = a + b;

    FSLLOG3_INFO("GlobalExampleService: Add({},{}) result: {} (ThreadId: {})", a, b, result, std::this_thread::get_id());

    return result;
  }
}
