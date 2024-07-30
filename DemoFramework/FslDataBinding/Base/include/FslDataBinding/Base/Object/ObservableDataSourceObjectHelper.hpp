#ifndef FSLDATABINDING_BASE_OBJECT_OBSERVABLEDATASOURCEOBJECTHELPER_HPP
#define FSLDATABINDING_BASE_OBJECT_OBSERVABLEDATASOURCEOBJECTHELPER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/Object/PropLinkRefs.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinition.hpp>
#include <FslDataBinding/Base/Property/PropertySetBindingResult.hpp>
#include <FslDataBinding/Base/Property/TypedReadOnlyDependencyProperty.hpp>
#include <memory>


namespace Fsl::DataBinding::ObservableDataSourceObjectHelper
{
  template <typename TOwner, typename TProperty>
  inline void PropertySanityCheckGet([[maybe_unused]] TOwner* pOwnerThis, [[maybe_unused]] ScopedDataSourceObject& rThisDependencyObject,
                                     [[maybe_unused]] const DependencyPropertyDefinition& sourceDef,
                                     [[maybe_unused]] PropLinkRefs<TProperty> propertyRefs)
  {
    assert(pOwnerThis != nullptr);

    // Can't do the following since: It occurs before the compare and it could belong to a base class
    // assert(targetDef.OwnerType() == typeid(TOwner));

    //! The property definition does not belong to the expected class
    //! -> Incorrect property implementation or incorrect property definition used.
    assert(propertyRefs.PropertyDef.OwnerType() == typeid(TOwner));
    //! The property definition content type does not match the property type its being linked with.
    //! -> Incorrect property implementation or incorrect property definition used.
    assert(propertyRefs.PropertyDef.Type() == propertyRefs.Property.Type());
  }

  template <typename TOwner, typename TProperty>
  inline void PropertySanityCheckSet([[maybe_unused]] TOwner* pOwnerThis, [[maybe_unused]] ScopedDataSourceObject& rThisDependencyObject,
                                     [[maybe_unused]] const DependencyPropertyDefinition& targetDef,
                                     [[maybe_unused]] PropLinkRefs<TProperty> propertyRefs)
  {
    assert(pOwnerThis != nullptr);

    // Can't do the following since: It occurs before the compare and it could belong to a base class
    // assert(targetDef.OwnerType() == typeid(TOwner));

    //! The property definition does not belong to the expected class
    //! -> Incorrect property implementation or incorrect property definition used.
    assert(propertyRefs.PropertyDef.OwnerType() == typeid(TOwner));
    //! The property definition content type does not match the property type its being linked with.
    //! -> Incorrect property implementation or incorrect property definition used.
    assert(propertyRefs.PropertyDef.Type() == propertyRefs.Property.Type());
  }


  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // TryGetPropertyHandle
  // -----------------------------------------------------------------------------------------------------------------------------------------------

  template <typename TOwner, typename TPropertyValue>
  inline DataBindingInstanceHandle TryGetPropertyHandle(TOwner* pOwnerThis, ScopedDataSourceObject& rThisDependencyObject,
                                                        const DependencyPropertyDefinition& sourceDef,
                                                        PropLinkRefs<TypedReadOnlyDependencyProperty<TPropertyValue>> propertyRefs)
  {
    PropertySanityCheckGet<TOwner, TypedReadOnlyDependencyProperty<TPropertyValue>>(pOwnerThis, rThisDependencyObject, sourceDef, propertyRefs);
    return sourceDef == propertyRefs.PropertyDef
             ? propertyRefs.Property.GetInstanceHandleOnDemand(pOwnerThis, rThisDependencyObject, propertyRefs.PropertyDef)
             : DataBindingInstanceHandle();
  }


  template <typename TOwner, typename TPropertyValue, typename... Args>
  inline DataBindingInstanceHandle TryGetPropertyHandle(TOwner* pOwnerThis, ScopedDataSourceObject& rThisDependencyObject,
                                                        const DependencyPropertyDefinition& sourceDef,
                                                        PropLinkRefs<TypedReadOnlyDependencyProperty<TPropertyValue>> propertyRefs, Args... args)
  {
    PropertySanityCheckGet<TOwner, TypedReadOnlyDependencyProperty<TPropertyValue>>(pOwnerThis, rThisDependencyObject, sourceDef, propertyRefs);
    return sourceDef == propertyRefs.PropertyDef
             ? propertyRefs.Property.GetInstanceHandleOnDemand(pOwnerThis, rThisDependencyObject, propertyRefs.PropertyDef)
             : TryGetPropertyHandle(pOwnerThis, rThisDependencyObject, sourceDef, args...);
  }
}

#endif
