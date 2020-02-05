#ifndef FSLSIMPLEUI_APP_UIDEMOAPPEXTENSION_HPP
#define FSLSIMPLEUI_APP_UIDEMOAPPEXTENSION_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/String/UTF8String.hpp>
#include <FslDemoApp/Base/DemoAppExtension.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasMap.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>
#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>

namespace Fsl
{
  class AtlasFont;
  class DemoAppConfig;
  class TransitionCache;

  namespace UI
  {
    class IEventListener;
  }

  class UIDemoAppExtension : public DemoAppExtension
  {
    UI::UIManager m_uiManager;
    BasicTextureAtlas m_defaultTextureAtlas;
    std::shared_ptr<AtlasFont> m_defaultFont;
    TextureAtlasMap m_textureAtlasMap;
    std::shared_ptr<UI::WindowContext> m_context;
    std::shared_ptr<TransitionCache> m_transitionCache;

  public:
    UIDemoAppExtension(const UIDemoAppExtension&) = delete;
    UIDemoAppExtension& operator=(const UIDemoAppExtension&) = delete;

    UIDemoAppExtension(const DemoAppConfig& demoAppConfig, const std::shared_ptr<UI::IEventListener>& eventListener, const UTF8String& fontName);
    ~UIDemoAppExtension();

    void RegisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener);
    void UnregisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener);

    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void Resized(const Point2& size) override;
    void PreUpdate(const DemoTime& demoTime) override;
    void Update(const DemoTime& demoTime) override;
    void PostUpdate(const DemoTime& demoTime) override;

    void Draw();

    //! @brief Get access to the window context
    std::shared_ptr<UI::WindowContext> GetContext() const
    {
      return m_context;
    }

    //! @brief Get the window manager
    std::shared_ptr<UI::IWindowManager> GetWindowManager() const
    {
      return m_uiManager.GetWindowManager();
    }

    //! @brief Retrieve a atlas texture from the default texture atlas
    AtlasTexture2D GetAtlasTexture2D(const UTF8String& filename) const;

    //! @brief Retrieve the atlas texture used for the default texture atlas
    Texture2D GetAtlasTexture() const;

    //! @brief Get the transition cache
    std::shared_ptr<TransitionCache> GetTransitionCache() const;

    const ITextureAtlas& GetDefaultTextureAtlas() const
    {
      return m_defaultTextureAtlas;
    }
  };
}

#endif
