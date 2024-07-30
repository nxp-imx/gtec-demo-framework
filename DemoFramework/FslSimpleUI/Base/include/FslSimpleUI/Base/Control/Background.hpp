#ifndef FSLSIMPLEUI_BASE_CONTROL_BACKGROUND_HPP
#define FSLSIMPLEUI_BASE_CONTROL_BACKGROUND_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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

#include <FslSimpleUI/Base/Control/ContentControl.hpp>
#include <FslSimpleUI/Base/Mesh/ContentSpriteMesh.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>

namespace Fsl::UI
{
  class WindowContext;

  class Background final : public ContentControl
  {
    using base_type = ContentControl;

  protected:
    // NOLINTNEXTLINE(readability-identifier-naming)
    const std::shared_ptr<WindowContext> m_windowContext;

  private:
    ContentSpriteMesh m_background;
    DependencyPropertyUIColor m_propertyBackgroundColor;

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyBackgroundColor;

    explicit Background(const std::shared_ptr<WindowContext>& context);

    const std::shared_ptr<IContentSprite>& GetBackground() const
    {
      return m_background.GetSprite();
    }
    void SetBackground(const std::shared_ptr<IContentSprite>& value);

    UIColor GetBackgroundColor() const noexcept
    {
      return m_propertyBackgroundColor.Get();
    }

    bool SetBackgroundColor(const UIColor value);

    void WinDraw(const UIDrawContext& context) final;

  protected:
    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) override;
    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;
  };
}

#endif
