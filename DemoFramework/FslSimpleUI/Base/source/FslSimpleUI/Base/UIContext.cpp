/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/UIContext.hpp>
#include <FslSimpleUI/Render/Base/IMeshManager.hpp>
#include <utility>

namespace Fsl::UI
{
  UIContext::UIContext(std::shared_ptr<DataBinding::DataBindingService> dataBindingService, std::shared_ptr<IWindowManager> windowManager,
                       std::shared_ptr<WindowEventSender> eventSender, std::shared_ptr<IMeshManager> meshManager)
    : DataBindingService(std::move(dataBindingService))
    , WindowManager(std::move(windowManager))
    , EventSender(std::move(eventSender))
    , MeshManager(std::move(meshManager))
  {
    if (!DataBindingService)
    {
      throw std::invalid_argument("dataBindingService can not be null");
    }
    if (!WindowManager)
    {
      throw std::invalid_argument("windowManager can not be null");
    }
    if (!EventSender)
    {
      throw std::invalid_argument("eventSender can not be null");
    }
    if (!MeshManager)
    {
      throw std::invalid_argument("meshManager can not be null");
    }
  }

  UIContext ::~UIContext() = default;
}
