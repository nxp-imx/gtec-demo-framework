#ifndef FSLSIMPLEUI_BASE_CONTROL_FMTVALUELABEL_HPP
#define FSLSIMPLEUI_BASE_CONTROL_FMTVALUELABEL_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2022-2023 NXP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinition.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslSimpleUI/Base/Control/LabelBase.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <fmt/format.h>
#include <cstring>
#include <string>
#include <type_traits>

namespace Fsl::UI
{
  template <typename TValue>
  class FmtValueLabel : public LabelBase
  {
    using base_type = LabelBase;

  public:
    using value_type = TValue;

  private:
    std::string m_format;
    mutable fmt::memory_buffer m_buffer;

    typename DataBinding::TypedDependencyProperty<value_type> m_propertyContent;

  public:
    explicit FmtValueLabel(const std::shared_ptr<WindowContext>& context)
      : LabelBase(context)
      , m_format(std::is_floating_point<value_type>::value ? "{:.2f}" : "{}")
    {
      RebuildCache();
    }

    const std::string& GetFormatString() const
    {
      return m_format;
    }

    //! @return true if the format string was changed (false if the format string was already equal to the value being set)
    bool SetFormatString(const char* const psz)
    {
      return SetFormatString(StringViewLite(psz));
    }

    //! @return true if the format string was changed (false if the format string was already equal to the value being set)
    bool SetFormatString(const StringViewLite& value)
    {
      if (value == m_format)
      {
        return false;
      }

      m_format.assign(value.data(), value.size());
      RebuildCache();
      PropertyUpdated(PropertyType::Content);
      return true;
    }

    //! @return true if the format string was changed (false if the format string was already equal to the value being set)
    bool SetFormatString(const std::string& strFormat)
    {
      if (strFormat == m_format)
      {
        return false;
      }

      m_format = strFormat;
      RebuildCache();
      PropertyUpdated(PropertyType::Content);
      return true;
    }


    //! @return true if the format string was changed (false if the format string was already equal to the value being set)
    bool SetFormatString(std::string&& strFormat)
    {
      if (strFormat == m_format)
      {
        return false;
      }

      m_format = std::move(strFormat);
      RebuildCache();
      PropertyUpdated(PropertyType::Content);
      return true;
    }

    value_type GetContent() const
    {
      return m_propertyContent.Get();
    }

    bool SetContent(const value_type value)
    {
      const bool changed = m_propertyContent.Set(ThisDependencyObject(), value);
      if (changed)
      {
        RebuildCache();
        PropertyUpdated(PropertyType::Content);
      }
      return changed;
    }

    PxSize2D MeasureRenderedValue(const value_type value) const
    {
      fmt::memory_buffer tmpBuffer;
      fmt::format_to(std::back_inserter(tmpBuffer), m_format, value);
      return DoMeasureRenderedString(StringViewLite(tmpBuffer.data(), tmpBuffer.size()));
    }

  protected:
    StringViewLite DoGetContent() const override
    {
      return StringViewLite(m_buffer.data(), m_buffer.size());
    }

  public:
    inline static typename DataBinding::DependencyPropertyDefinition PropertyContent =
      DataBinding::DependencyPropertyDefinitionFactory::Create<value_type, FmtValueLabel, &FmtValueLabel::GetContent, &FmtValueLabel::SetContent>(
        "Content");

  protected:
    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override
    {
      auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                                           DataBinding::PropLinkRefs(PropertyContent, m_propertyContent));
      return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
    }

    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) override
    {
      auto res = DataBinding::DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                                    DataBinding::PropLinkRefs(PropertyContent, m_propertyContent));
      return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
    }


    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) final
    {
      base_type::ExtractAllProperties(rProperties);
      rProperties.push_back(FmtValueLabel::PropertyContent);
    }

  private:
    void RebuildCache()
    {
      m_buffer.clear();
      fmt::format_to(std::back_inserter(m_buffer), m_format, GetContent());
      DoSetContent(DoGetContent());
    }
  };
}

#endif
