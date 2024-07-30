#ifndef FSLSIMPLEUI_BASE_PROPERTY_DEPENDENCYPROPERTYUICOLOR_HPP
#define FSLSIMPLEUI_BASE_PROPERTY_DEPENDENCYPROPERTYUICOLOR_HPP
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

#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslDataBinding/Base/PropertyChangeReason.hpp>
#include <FslDataBinding/Base/ScopedDependencyObject.hpp>
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Base/UIColorConverter.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>


namespace Fsl::UI
{
  struct DependencyPropertyUIColor
  {
    DataBinding::TypedDependencyProperty<UIColor> ExternalColor{UIColors::White()};
    UIRenderColor InternalColor;

    DependencyPropertyUIColor(const UIColorConverter converter, const UIColor value)
      : ExternalColor(value)
      , InternalColor(converter.Convert(value))
    {
    }

    UIColor Get() const
    {
      return ExternalColor.Get();
    }

    bool Set(const UIColorConverter converter, DataBinding::ScopedDependencyObject& rDependencyObject, const UIColor value,
             const DataBinding::PropertyChangeReason changeReason = DataBinding::PropertyChangeReason::Modified)
    {
      bool result = ExternalColor.Set(rDependencyObject, value, changeReason);
      InternalColor = converter.Convert(value);
      return result;
    }
  };
}

#endif
