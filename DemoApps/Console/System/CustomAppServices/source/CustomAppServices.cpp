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

#include "CustomAppServices.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <fmt/std.h>    // to allow us to print a std::thread::id
#include <chrono>

namespace Fsl
{
  CustomAppServices::CustomAppServices(const DemoAppConfig& config)
    : DemoAppConsoleMinimal(config)
    , m_asyncService(config.DemoServiceProvider.Get<IAsyncExampleService>())
    , m_globalService(config.DemoServiceProvider.Get<IGlobalExampleService>())
    , m_localService(config.DemoServiceProvider.Get<ILocalExampleService>())
  {
    const auto threadId = std::this_thread::get_id();
    FSLLOG3_INFO("CustomAppServices: created on thread: (ThreadId: {})", threadId);

    m_futureAdd = m_asyncService->AddAsync(42, 2);
    const int32_t globalResult = m_globalService->Add(43, 3);
    const int32_t localResult = m_localService->Add(44, 4);

    FSLLOG3_INFO("CustomAppServices: global result: {} (ThreadId: {})", globalResult, threadId);
    FSLLOG3_INFO("CustomAppServices: local result: {} (ThreadId: {})", localResult, threadId);
  }


  CustomAppServices::~CustomAppServices()
  {
    FSLLOG3_INFO("CustomAppServices: destroyed on thread: (ThreadId: {})", std::this_thread::get_id());
  }


  void CustomAppServices::Run()
  {
    const auto threadId = std::this_thread::get_id();
    {    // Wait for the result from the async operation started in the constructor
      using namespace std::chrono_literals;

      FSLLOG3_INFO("CustomAppServices: Waiting for result. (ThreadId: {})", threadId);
      bool busyWait = false;
      if (!busyWait)
      {
        if (m_futureAdd.wait_for(5s) == std::future_status::ready)
        {
          FSLLOG3_INFO("CustomAppServices: AddAsync result: {} (ThreadId: {})", m_futureAdd.get(), threadId);
          m_futureAdd = {};
        }
        else
        {
          FSLLOG3_ERROR("CustomAppServices: AddAsync result: failed");
        }
      }
      {
        while (m_futureAdd.valid())
        {
          if (m_futureAdd.wait_for(0s) == std::future_status::ready)
          {
            FSLLOG3_INFO("CustomAppServices: AddAsync result: {} (ThreadId: {})", m_futureAdd.get(), threadId);
            m_futureAdd = {};
          }
        }
      }
    }
  }
}
