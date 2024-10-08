#ifndef FSLDEMOHOST_BASE_SERVICE_TEST_ITESTSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_TEST_ITESTSERVICE_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDemoHost/Base/Service/Test/TestScreenshotConfig.hpp>
#include <FslDemoHost/Base/Service/Test/TestScreenshotNameScheme.hpp>

namespace Fsl
{
  class ITestService
  {
  public:
    virtual ~ITestService() = default;

    //! @brief Get the current screenshot frequency
    virtual uint32_t GetScreenshotFrequency() const = 0;

    //! @brief Set the current screenshot frequency
    //! @param frequency to take a screenshot (0 == disabled)
    virtual void SetScreenshotFrequency(const uint32_t frequency) = 0;

    //! @brief Get the current name scheme
    virtual TestScreenshotNameScheme GetScreenshotNameScheme() const = 0;

    //! @brief Set the current name scheme
    virtual void SetScreenshotNameScheme(const TestScreenshotNameScheme scheme) = 0;

    //! @brief Set configuration
    virtual void SetScreenshotConfig(const TestScreenshotConfig& config) = 0;

    //! @brief Called once a frame swap has been completed
    virtual void OnFrameSwapCompleted() = 0;
  };
}

#endif
