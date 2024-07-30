#ifndef FSLSIMPLEUI_BASE_CONTROL_IMAGE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMAGE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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

#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/ItemScalePolicy.hpp>
#include <FslSimpleUI/Base/Mesh/SizedSpriteMesh.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <memory>

namespace Fsl
{
  class ISizedSprite;

  namespace UI
  {
    class WindowContext;

    //! @brief This is the recommended image control. It is DP aware so it will render the image so it fits the users display.
    class Image : public BaseWindow
    {
      using base_type = BaseWindow;

    protected:
      // NOLINTNEXTLINE(readability-identifier-naming)
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      SizedSpriteMesh m_content;
      DependencyPropertyUIColor m_propertyContentColor;
      DataBinding::TypedDependencyProperty<ItemScalePolicy> m_propertyScalePolicy{ItemScalePolicy::FitKeepAR};
      DataBinding::TypedDependencyProperty<bool> m_propertyRotateImageCW;

    public:
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyContentColor;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyScalePolicy;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyRotateImageCW;

      explicit Image(const std::shared_ptr<WindowContext>& context);

      const std::shared_ptr<ISizedSprite>& GetContent() const
      {
        return m_content.GetSprite();
      }

      void SetContent(const std::shared_ptr<ISizedSprite>& value);
      void SetContent(std::shared_ptr<ISizedSprite>&& value);

      UIColor GetContentColor() const noexcept
      {
        return m_propertyContentColor.Get();
      }

      bool SetContentColor(const UIColor value);

      ItemScalePolicy GetScalePolicy() const noexcept
      {
        return m_propertyScalePolicy.Get();
      }

      bool SetScalePolicy(const ItemScalePolicy value);

      bool GetRotateImageCW() const noexcept
      {
        return m_propertyRotateImageCW.Get();
      }

      bool SetRotateImageCW(const bool value);


      void WinDraw(const UIDrawContext& context) override;

    protected:
      UIRenderColor GetContentInternalColor() const noexcept
      {
        return m_propertyContentColor.InternalColor;
      }

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;
    };
  }
}
#endif
