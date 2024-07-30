#ifndef SHARED_SYSTEM_GESTURES_GESTURESSHARED_HPP
#define SHARED_SYSTEM_GESTURES_GESTURESSHARED_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslSimpleUI/Base/Gesture/GestureManager.hpp>
#include <FslSimpleUI/Base/Gesture/Velocity/VelocityTracker.hpp>
#include <Shared/System/Gestures/MoveableRectangleManager.hpp>
#include <fmt/format.h>
#include <memory>

namespace Fsl
{
  class DemoAppConfig;
  struct DemoTime;
  class IGraphicsService;
  class KeyEvent;
  class MouseButtonEvent;
  class MouseMoveEvent;
  class SpriteNativeAreaCalc;

  class GesturesShared
  {
    std::shared_ptr<IProfilerService> m_profiler;

    DemoWindowMetrics m_cachedWindowMetrics;
    SpriteUnitConverter m_spriteUnitConverter;

    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<INativeBatch2D> m_batch;

    Texture2D m_texFill;
    Texture2D m_texSdf;
    TextureAtlasSpriteFont m_fontSdf;

    Fsl::UI::GestureManager m_gestureManager;
    bool m_mouseLeftMouseDown{false};

    MoveableRectangleManager m_moveableRectangleManager;

    fmt::memory_buffer m_resolutionBuffer1;
    fmt::memory_buffer m_resolutionBuffer2;
    fmt::memory_buffer m_resolutionBuffer3;
    fmt::memory_buffer m_fpsBuffer1;
    fmt::memory_buffer m_fpsBuffer2;

    bool m_showFramePacingX{true};
    bool m_showFramePacingY{true};
    PxPoint2 m_position0Px;
    PxPoint2 m_position1Px;
    PxValue m_add0Px;
    PxValue m_add1Px;

  public:
    explicit GesturesShared(const DemoAppConfig& config, const SpriteNativeAreaCalc& spriteNativeAreaCalc);
    ~GesturesShared();

    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);

    void OnMouseButtonEvent(const MouseButtonEvent& event);
    void OnMouseMoveEvent(const MouseMoveEvent& event);
    void OnKeyEvent(const KeyEvent& event);

    void Update(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);

  private:
    void UpdateDuringDrawFramePacingAnimation();
    void RebuildStatsCache();
    void SetToDefaults();
  };
}


#endif
