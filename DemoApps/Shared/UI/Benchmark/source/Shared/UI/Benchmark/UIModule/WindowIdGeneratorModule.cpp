/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
#include <FslSimpleUI/Base/Module/ExternalModuleCreateInfo.hpp>
#include <FslSimpleUI/Base/Module/IExternalModuleHost.hpp>
#include <FslSimpleUI/Base/Module/IWindowBasicInfo.hpp>
#include <FslSimpleUI/Base/Module/IWindowClickInputTargetLocater.hpp>
#include <Shared/UI/Benchmark/UIModule/WindowIdGeneratorModule.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  WindowIdGeneratorModule::WindowIdGeneratorModule(const UI::ExternalModuleCreateInfo& createInfo)
    : UI::AExternalModule(createInfo)
    , m_targetLocater(createInfo.ModuleHost.GetTargetLocater())
    , m_info(createInfo.ModuleHost.GetWindowInfo())
  {
  }


  void WindowIdGeneratorModule::OnWindowAdd(const std::shared_ptr<UI::IWindowId>& window)
  {
    assert(window);
    const uint64_t windowId = CreateWindowId();
    m_mapWindowToId[window.get()] = WindowRecord(windowId);
    m_mapIdToWindow[windowId] = window.get();
    // FSLLOG3_INFO("Window #{}, total windows: {}", windowId, m_map.size());
  }

  void WindowIdGeneratorModule::OnWindowDispose(const std::shared_ptr<UI::IWindowId>& window)
  {
    // Erase the window if it exists
    auto itrFind = m_mapWindowToId.find(window.get());
    if (itrFind == m_mapWindowToId.end())
    {
      FSLLOG3_DEBUG_WARNING("OnDispose: Unknown window encountered!");
      return;
    }

    m_mapIdToWindow.erase(itrFind->second.WindowId);
    m_mapWindowToId.erase(window.get());
    // FSLLOG3_INFO("total windows: {}", m_map.size());
  }


  CustomWindowId WindowIdGeneratorModule::TryGetMouseOverWindow(const PxPoint2& hitPositionPx) const
  {
    auto foundWindow = m_targetLocater->TryGetMouseOverWindow(hitPositionPx);
    const UI::IWindowId* pWindowId = foundWindow.get();
    if (pWindowId == nullptr)
    {
      return {};
    }
    auto itrFind = m_mapWindowToId.find(pWindowId);
    if (itrFind == m_mapWindowToId.end())
    {
      FSLLOG3_DEBUG_WARNING("TryGetMouseOverWindow: Unknown window encountered!");
      return {};
    }

    return CustomWindowId(itrFind->second.WindowId);
  }

  CustomWindowId WindowIdGeneratorModule::TryGetClickInputWindow(const PxPoint2& hitPositionPx) const
  {
    auto foundWindow = m_targetLocater->TryGetClickInputWindow(hitPositionPx);
    const UI::IWindowId* pWindowId = foundWindow.get();
    if (pWindowId == nullptr)
    {
      return {};
    }
    auto itrFind = m_mapWindowToId.find(pWindowId);
    if (itrFind == m_mapWindowToId.end())
    {
      FSLLOG3_DEBUG_WARNING("TryGetClickInputWindow: Unknown window encountered!");
      return {};
    }

    return CustomWindowId(itrFind->second.WindowId);
  }


  PxRectangle WindowIdGeneratorModule::GetWindowRectanglePx(const CustomWindowId windowId) const
  {
    auto itrFind = m_mapIdToWindow.find(windowId.Value);
    if (itrFind == m_mapIdToWindow.end())
    {
      throw NotFoundException("Unknown window");
    }
    return m_info->GetWindowRectanglePx(itrFind->second);
  }


  std::optional<PxRectangle> WindowIdGeneratorModule::TryGetWindowRectanglePx(const CustomWindowId windowId) const
  {
    auto itrFind = m_mapIdToWindow.find(windowId.Value);
    if (itrFind != m_mapIdToWindow.end())
    {
      return m_info->TryGetWindowRectanglePx(itrFind->second);
    }
    return {};
  }


  uint64_t WindowIdGeneratorModule::CreateWindowId()
  {
    if (m_currentWindowId == std::numeric_limits<uint64_t>::max())
    {
      // For the use cases here, we should never record sequences that are long enough for this to ever occur.
      throw NotSupportedException("We reached the limit of this input tracker's id generation");
    }
    ++m_currentWindowId;
    return m_currentWindowId;
  }
}
