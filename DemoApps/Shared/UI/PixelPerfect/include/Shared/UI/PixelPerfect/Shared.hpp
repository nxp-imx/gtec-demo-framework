#ifndef SHARED_UI_PIXELPERFECT_SHARED_HPP
#define SHARED_UI_PIXELPERFECT_SHARED_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasMap.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <memory>

namespace Fsl
{
  class Shared final : public UI::EventListener
  {
    struct TestGraphicsAtlas
    {
      TextureAtlasMap AtlasMap;
      Texture2D Texture;

      AtlasTexture2D GetAtlasTexture(const IO::Path& path) const
      {
        return {Texture, AtlasMap.GetAtlasTextureInfo(path)};
      }
    };

    struct TestTextures
    {
      AtlasTexture2D RedBlue160dp;
      AtlasTexture2D RedBlue240dp;
      AtlasTexture2D RedBlue320dp;
      AtlasTexture2D RedBlue480dp;
      AtlasTexture2D RedBlue640dp;
      AtlasTexture2D Text160dp;
      AtlasTexture2D Text240dp;
      AtlasTexture2D Text320dp;
      AtlasTexture2D Text480dp;
      AtlasTexture2D Text640dp;
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<INativeBatch2D> m_nativeBatch;
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> m_slider;
    std::shared_ptr<UI::BackgroundLabelButton> m_btnDefault;

    TestGraphicsAtlas m_testAtlas;
    TestTextures m_testTextures;

    fmt::memory_buffer m_buffer;

  public:
    explicit Shared(const DemoAppConfig& config);
    ~Shared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    void OnKeyEvent(const KeyEvent& event);

    // From EventListener
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;
    void Draw();

  private:
    PxPoint2 DrawScaleMatrix(const PxPoint2& offsetPx, const PxPoint2& spacePx, const AtlasTexture2D& srcTexture, const Color& color,
                             const float errorOffsetPxf);
    void SetDefaultValues();
    void DecreaseSlider();
    void IncreaseSlider();
  };
}

#endif
