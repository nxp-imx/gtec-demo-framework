#ifndef FSLDATABINDING_UNITTEST_FSLDATABINDING_BASE_UNITTEST_UTTYPEDDEPENDENCYPROPERTYREFMETHODS_HPP
#define FSLDATABINDING_UNITTEST_FSLDATABINDING_BASE_UNITTEST_UTTYPEDDEPENDENCYPROPERTYREFMETHODS_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslDataBinding/Base/Internal/ATypedDependencyPropertyRefMethods.hpp>

namespace Fsl
{
  template <typename TPropertyValue>
  class UTTypedDependencyPropertyRefMethods : public Fsl::DataBinding::Internal::ATypedDependencyPropertyRefMethods<TPropertyValue>
  {
    TPropertyValue m_value{};

  public:
    UTTypedDependencyPropertyRefMethods() = default;
    explicit UTTypedDependencyPropertyRefMethods(const TPropertyValue value)
      : m_value(value)
    {
    }

    const TPropertyValue& Get() const final
    {
      return m_value;
    }

    bool Set(const TPropertyValue& value) final
    {
      const bool changed = value != m_value;
      m_value = value;
      return changed;
    }

    DataBinding::Internal::PropertySetResult TrySet(const Fsl::DataBinding::Internal::IPropertyMethods* const pGet) final
    {
      {    // Try a known get type
        const auto* const pTypedGetOperation =
          dynamic_cast<const Fsl::DataBinding::Internal::ATypedDependencyPropertyRefMethods<TPropertyValue>*>(pGet);
        if (pTypedGetOperation != nullptr)
        {
          return Set(pTypedGetOperation->Get()) ? DataBinding::Internal::PropertySetResult::ValueChanged
                                                : DataBinding::Internal::PropertySetResult::ValueUnchanged;
        }
      }
      return DataBinding::Internal::PropertySetResult::NotSupported;
    }
  };

}

#endif
