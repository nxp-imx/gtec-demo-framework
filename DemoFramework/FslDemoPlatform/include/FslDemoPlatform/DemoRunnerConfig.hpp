#ifndef FSLDEMOPLATFORM_DEMORUNNERCONFIG_HPP
#define FSLDEMOPLATFORM_DEMORUNNERCONFIG_HPP
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

#include <FslBase/ITag.hpp>
#include <FslDemoPlatform/MainLoopCallbackFunc.hpp>
#include <FslDemoPlatform/Setup/DemoSetupManagerConfig.hpp>
#include <memory>
#include <string>
#include <utility>

namespace Fsl
{
  struct DemoRunnerConfig
  {
    bool UseDefaultSignalHandlers;
    DemoSetupManagerConfig SetupManagerConfig;
    std::shared_ptr<ITag> NativeWindowTag;
    //! If this is not null the function will be called once during the mainloop
    FNMainLoopCallback MainLoopCallbackFunction{nullptr};

    DemoRunnerConfig(const bool useDefaultSignalHandlers, const IO::Path& contentPath, const IO::Path& persistentDataPath,
                     std::shared_ptr<ITag> nativeWindowTag, FNMainLoopCallback mainLoopCallbackFunction = nullptr)
      : UseDefaultSignalHandlers(useDefaultSignalHandlers)
      , SetupManagerConfig(contentPath, persistentDataPath)
      , NativeWindowTag(std::move(nativeWindowTag))
      , MainLoopCallbackFunction(mainLoopCallbackFunction)
    {
    }
  };
}

#endif
