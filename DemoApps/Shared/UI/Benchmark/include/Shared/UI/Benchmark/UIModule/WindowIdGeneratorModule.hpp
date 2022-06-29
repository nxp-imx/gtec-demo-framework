#ifndef SHARED_UI_BENCHMARK_UIMODULE_WINDOWIDGENERATORMODULE_HPP
#define SHARED_UI_BENCHMARK_UIMODULE_WINDOWIDGENERATORMODULE_HPP
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

#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslSimpleUI/Base/Module/AExternalModule.hpp>
#include <Shared/UI/Benchmark/Persistence/Window/CustomWindowId.hpp>
#include <Shared/UI/Benchmark/UIModule/ICustomWindowInfoModule.hpp>
#include <optional>
#include <unordered_map>

namespace Fsl
{
  struct PxPoint2;

  namespace UI
  {
    class IWindowBasicInfo;
    class IWindowClickInputTargetLocater;
  }

  class WindowIdGeneratorModule final
    : public UI::AExternalModule
    , public ICustomWindowInfoModule
  {
    struct WindowRecord
    {
      uint64_t WindowId{0};

      constexpr WindowRecord() noexcept = default;
      explicit constexpr WindowRecord(const uint64_t windowId) noexcept
        : WindowId(windowId)
      {
      }
    };

    std::shared_ptr<UI::IWindowClickInputTargetLocater> m_targetLocater;
    std::shared_ptr<UI::IWindowBasicInfo> m_info;

    std::unordered_map<const UI::IWindowId*, WindowRecord> m_mapWindowToId;
    std::unordered_map<uint64_t, const UI::IWindowId*> m_mapIdToWindow;

    uint64_t m_currentWindowId{0};

  public:
    explicit WindowIdGeneratorModule(const UI::ExternalModuleCreateInfo& createInfo);

    void OnWindowAdd(const std::shared_ptr<UI::IWindowId>& window) final;
    void OnWindowDispose(const std::shared_ptr<UI::IWindowId>& window) final;

    //! @brief Try to locate a window that would receive a mouse over event
    CustomWindowId TryGetMouseOverWindow(const PxPoint2& hitPositionPx) const final;

    //! @brief Try to locate a window that would receive a mouse over event
    CustomWindowId TryGetClickInputWindow(const PxPoint2& hitPositionPx) const final;

    //! @brief Get the current screen-space rectangle of the given window
    PxRectangle GetWindowRectanglePx(const CustomWindowId windowId) const final;

    //! @brief Get the current screen-space rectangle of the given window
    std::optional<PxRectangle> TryGetWindowRectanglePx(const CustomWindowId windowId) const final;

  private:
    uint64_t CreateWindowId();
  };
}

#endif
