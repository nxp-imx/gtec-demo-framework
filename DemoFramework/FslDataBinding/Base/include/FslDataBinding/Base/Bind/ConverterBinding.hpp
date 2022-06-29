#ifndef FSLDATABINDING_BASE_BIND_CONVERTERBINDING_HPP
#define FSLDATABINDING_BASE_BIND_CONVERTERBINDING_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDataBinding/Base/Bind/AConverterBinding.hpp>
#include <FslDataBinding/Base/Internal/TypedPropertyMethodsUtil.hpp>
#include <functional>
#include <utility>

namespace Fsl::DataBinding
{
  template <typename TTarget, typename TSource>
  class ConverterBinding final : public AConverterBinding
  {
  public:
    using target_value_type = TTarget;
    using source_value_type = TSource;
    using converter_function_type = std::function<target_value_type(const source_value_type value)>;

  private:
    converter_function_type m_fnConvert;

  public:
    explicit ConverterBinding(converter_function_type fnConvert)
      : m_fnConvert(std::move(fnConvert))
    {
      if (!m_fnConvert)
      {
        throw std::invalid_argument("fnConvert can not be empty");
      }
    }

    std::type_index GetSourceType() const final
    {
      return typeid(source_value_type);
    }

    std::type_index GetTargetType() const final
    {
      return typeid(target_value_type);
    }


    Internal::PropertySetResult Convert(const Internal::PropertyMethodsImplType setPropertyMethodsImplType, Internal::IPropertyMethods* const pSet,
                                        const Internal::PropertyMethodsImplType getPropertyMethodsImplType,
                                        Internal::IPropertyMethods* const pGet) final
    {
      switch (getPropertyMethodsImplType)
      {
      case Internal::PropertyMethodsImplType::ATypedDependencyProperty:
        {    // Try the normal get method
          const auto* const pTypedGetOperation = dynamic_cast<const Internal::ATypedDependencyPropertyMethods<source_value_type>*>(pGet);
          if (pTypedGetOperation != nullptr)
          {
            return Internal::TypedPropertyMethodsUtil::SetByValue(setPropertyMethodsImplType, pSet, m_fnConvert(pTypedGetOperation->Get()));
          }
          break;
        }
      case Internal::PropertyMethodsImplType::ATypedDependencyPropertyRef:
        {    // Try the ref get method
          const auto* const pTypedGetOperation = dynamic_cast<const Internal::ATypedDependencyPropertyRefMethods<source_value_type>*>(pGet);
          if (pTypedGetOperation != nullptr)
          {
            return Internal::TypedPropertyMethodsUtil::SetByRef(setPropertyMethodsImplType, pSet, m_fnConvert(pTypedGetOperation->Get()));
          }
          break;
        }
      default:
        break;
      }
      return Internal::PropertySetResult::UnsupportedGetType;
    }
  };
}

#endif
