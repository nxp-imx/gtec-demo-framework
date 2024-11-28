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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Dp/FmtDpPoint2F.hpp>
#include <FslBase/Log/Math/FmtVector2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxSize2D.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxVector2.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragBeginBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragEndBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureFlickBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureTapBasicEvent.hpp>
#include <Shared/System/Gestures/GesturesShared.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr PxValue Speed(2);
      constexpr PxSize1D FramePacingSize = PxSize1D::Create(50);

      constexpr IO::PathView PathSdfAtlas("Font/SdfAtlas/FontAtlas.bta");
      constexpr IO::PathView PathSdfAtlasTexture("Font/SdfAtlas/FontAtlas.png");
      constexpr IO::PathView PathSdfAtlasFont("Font/SdfAtlas/FontAtlas_Font.fbk");
      constexpr IO::PathView PathFillTexture("Fill/Fill.png");
    }
  }


  GesturesShared::GesturesShared(const DemoAppConfig& config, const SpriteNativeAreaCalc& spriteNativeAreaCalc)
    : m_profiler(config.DemoServiceProvider.Get<IProfilerService>())
    , m_cachedWindowMetrics(config.WindowMetrics)
    , m_spriteUnitConverter(config.WindowMetrics.DensityDpi)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_batch(m_graphics->GetNativeBatch2D())
    , m_gestureManager(UI::GestureFlags::Everything, UI::GestureDetector(UI::GestureFlags::Everything), config.WindowMetrics.DensityDpi)
    , m_moveableRectangleManager(config.WindowMetrics.GetSizePx(), config.WindowMetrics.DensityDpi)
    , m_add0Px(LocalConfig::Speed)
    , m_add1Px(LocalConfig::Speed)
  {
    m_gestureManager.SetEnabled(true);

    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();
    {
      Bitmap bitmap;
      contentManager->Read(bitmap, LocalConfig::PathFillTexture, PixelFormat::R8G8B8A8_UNORM);
      m_texFill = Texture2D(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);

      BasicTextureAtlas sdfAtlas;
      contentManager->Read(sdfAtlas, LocalConfig::PathSdfAtlas);
      contentManager->Read(bitmap, LocalConfig::PathSdfAtlasTexture, PixelFormat::R8G8B8A8_UNORM);

      m_texSdf = Texture2D(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);

      BasicFontKerning sdfFontBasicKerning;
      contentManager->Read(sdfFontBasicKerning, LocalConfig::PathSdfAtlasFont);

      m_fontSdf.Reset(spriteNativeAreaCalc, m_texSdf.GetExtent(), sdfAtlas, sdfFontBasicKerning, config.WindowMetrics.DensityDpi);
    }
  }


  GesturesShared::~GesturesShared() = default;


  void GesturesShared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    if (windowMetrics != m_cachedWindowMetrics)
    {
      m_cachedWindowMetrics = windowMetrics;
      m_spriteUnitConverter.SetDensityDpi(m_cachedWindowMetrics.DensityDpi);
      m_gestureManager.SetDpi(windowMetrics.DensityDpi);
      m_moveableRectangleManager.OnConfigurationChanged(windowMetrics.GetSizePx(), windowMetrics.DensityDpi);
    }
  }


  void GesturesShared::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.GetButton() == VirtualMouseButton::Left)
    {
      const bool previousState = m_mouseLeftMouseDown;
      m_mouseLeftMouseDown = event.IsPressed();
      const auto state = m_mouseLeftMouseDown ? UI::EventTransactionState::Begin : UI::EventTransactionState::End;
      m_gestureManager.AddMovement(event.GetTimestamp(), event.GetPosition(), state, m_mouseLeftMouseDown == previousState,
                                   UI::MovementOwnership::Unhandled);
    }
  }


  void GesturesShared::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (m_mouseLeftMouseDown)
    {
      m_gestureManager.AddMovement(event.GetTimestamp(), event.GetPosition(), UI::EventTransactionState::Begin, true,
                                   UI::MovementOwnership::Unhandled);
    }
  }

  void GesturesShared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::Space:
      SetToDefaults();
      break;
    case VirtualKey::X:
      m_showFramePacingX = !m_showFramePacingX;
      break;
    case VirtualKey::Y:
      m_showFramePacingY = !m_showFramePacingY;
      break;
    default:
      break;
    }
  }


  void GesturesShared::Update(const DemoTime& demoTime)
  {
    RebuildStatsCache();

    m_gestureManager.Process();
    UI::GestureBasicEvent currentEvent;
    if (m_gestureManager.TryGet(currentEvent))
    {
      switch (currentEvent.Type)
      {
      case UI::GestureEventType::Nop:
        FSLLOG3_INFO("Nop");
        break;
      case UI::GestureEventType::GestureDragBegin:
        {
          const auto typedEvent = UI::GestureDragBeginBasicEvent::Decode(currentEvent);
          const bool found = m_moveableRectangleManager.TryBeginDrag(typedEvent.PositionPx);
          FSLLOG3_INFO("GestureDragBegin PositionPx {} found  {}", typedEvent.PositionPx, found);
        }
        break;
      case UI::GestureEventType::GestureDrag:
        {
          const auto typedEvent = UI::GestureDragBasicEvent::Decode(currentEvent);
          FSLLOG3_VERBOSE("GestureDrag PositionPx {}", typedEvent.PositionPx);
          m_moveableRectangleManager.TryDrag(typedEvent.PositionPx);
          break;
        }
      case UI::GestureEventType::GestureDragEnd:
        {
          const auto typedEvent = UI::GestureDragEndBasicEvent::Decode(currentEvent);
          FSLLOG3_INFO("GestureDragEnd PositionPx {} FlickVelocityDp {}", typedEvent.PositionPx, typedEvent.FlickVelocityDpf);

          m_moveableRectangleManager.TryDragEnd(typedEvent.PositionPx, typedEvent.FlickVelocityDpf);
          break;
        }
      case UI::GestureEventType::GestureFlick:
        {
          const auto typedEvent = UI::GestureFlickBasicEvent::Decode(currentEvent);
          FSLLOG3_INFO("GestureDragFlick PositionPx {} VelocityDp {}", typedEvent.PositionPx, typedEvent.VelocityDpf);
          break;
        }
      case UI::GestureEventType::GestureTap:
        {
          const auto typedEvent = UI::GestureTapBasicEvent::Decode(currentEvent);
          FSLLOG3_INFO("GestureTap PositionPx {}", typedEvent.PositionPx);
          break;
        }
      default:
        FSLLOG3_WARNING("Unknown event type: {}", fmt::underlying(currentEvent.Type));
        break;
      }
    }
    m_moveableRectangleManager.Process(demoTime.ElapsedTime);
    // m_moveableRectangleManager.Process(TimeSpan(TimeSpan::TicksPerSecond / 60));
    // m_moveableRectangleManager.Process(TimeSpan(TimeSpan::TicksPerSecond / 240));
  }


  void GesturesShared::Draw(const DemoTime& demoTime)
  {
    UpdateDuringDrawFramePacingAnimation();

    m_batch->Begin(BlendState::Sdf);

    Vector2 dstPositionPx;
    m_batch->DrawString(m_texSdf, m_fontSdf, StringViewLite(m_resolutionBuffer1.data(), m_resolutionBuffer1.size()), dstPositionPx, Colors::White());
    dstPositionPx.Y += static_cast<float>(m_fontSdf.LineSpacingPx().RawValue());
    m_batch->DrawString(m_texSdf, m_fontSdf, StringViewLite(m_resolutionBuffer2.data(), m_resolutionBuffer2.size()), dstPositionPx, Colors::White());
    dstPositionPx.Y += static_cast<float>(m_fontSdf.LineSpacingPx().RawValue());
    m_batch->DrawString(m_texSdf, m_fontSdf, StringViewLite(m_resolutionBuffer3.data(), m_resolutionBuffer3.size()), dstPositionPx, Colors::White());
    dstPositionPx.Y += static_cast<float>(m_fontSdf.LineSpacingPx().RawValue());
    m_batch->DrawString(m_texSdf, m_fontSdf, StringViewLite(m_fpsBuffer1.data(), m_fpsBuffer1.size()), dstPositionPx, Colors::White());
    dstPositionPx.Y += static_cast<float>(m_fontSdf.LineSpacingPx().RawValue());
    m_batch->DrawString(m_texSdf, m_fontSdf, StringViewLite(m_fpsBuffer2.data(), m_fpsBuffer2.size()), dstPositionPx, Colors::White());

    m_batch->ChangeTo(BlendState::Opaque);

    if (m_showFramePacingX)
    {
      m_batch->Draw(m_texFill, PxRectangle(m_position0Px, PxSize2D(LocalConfig::FramePacingSize, LocalConfig::FramePacingSize)), Colors::Cyan());
    }
    if (m_showFramePacingY)
    {
      m_batch->Draw(m_texFill, PxRectangle(m_position1Px, PxSize2D(LocalConfig::FramePacingSize, LocalConfig::FramePacingSize)), Colors::Cyan());
    }

    for (const MoveableRectangleInfo& entry : m_moveableRectangleManager.GetDrawInfoSpan())
    {
      m_batch->Draw(m_texFill, entry.RectanglePx, entry.RenderColor);
    }

    m_batch->End();
  }

  void GesturesShared::UpdateDuringDrawFramePacingAnimation()
  {
    const auto farX = m_cachedWindowMetrics.GetSizePx().Width() - LocalConfig::FramePacingSize;
    const auto farY = m_cachedWindowMetrics.GetSizePx().Height() - LocalConfig::FramePacingSize;

    m_position0Px.X += m_add0Px;
    m_position0Px.Y = farY;
    if (m_position0Px.X > farX)
    {
      m_position0Px.X = farX;
      m_add0Px = PxValue(-LocalConfig::Speed);
    }
    if (m_position0Px.X < PxValue::Zero())
    {
      m_position0Px.X = PxValue::Zero();
      m_add0Px = PxValue(LocalConfig::Speed);
    }

    m_position1Px.X = farX;
    m_position1Px.Y += m_add1Px;
    if (m_position1Px.Y > farY)
    {
      m_position1Px.Y = farY;
      m_add1Px = PxValue(-LocalConfig::Speed);
    }
    if (m_position1Px.Y < PxValue::Zero())
    {
      m_position1Px.Y = PxValue::Zero();
      m_add1Px = PxValue(LocalConfig::Speed);
    }
  }


  void GesturesShared::RebuildStatsCache()
  {
    m_resolutionBuffer1.clear();
    fmt::format_to(std::back_inserter(m_resolutionBuffer1), "{:.1f}x{:.1f}dp", m_cachedWindowMetrics.SizeDp.X, m_cachedWindowMetrics.SizeDp.Y);
    m_resolutionBuffer2.clear();
    fmt::format_to(std::back_inserter(m_resolutionBuffer2), "{}x{}px", m_cachedWindowMetrics.GetSizePx().RawWidth(),
                   m_cachedWindowMetrics.GetSizePx().RawHeight());
    m_resolutionBuffer3.clear();
    fmt::format_to(std::back_inserter(m_resolutionBuffer3), "DensityDpi: {}", m_cachedWindowMetrics.DensityDpi);

    auto averageFrameTime = m_profiler->GetAverageFrameTime();

    m_fpsBuffer1.clear();
    fmt::format_to(std::back_inserter(m_fpsBuffer1), "FPS: {:.0f}", averageFrameTime.GetFramePerSecond());
    m_fpsBuffer2.clear();
    fmt::format_to(std::back_inserter(m_fpsBuffer2), "Time: {:4.1f}", static_cast<double>(averageFrameTime.TotalTime) / 1000.0);
  }


  void GesturesShared::SetToDefaults()
  {
    m_moveableRectangleManager.SetToDefaults();
  }
}
