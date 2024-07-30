/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/Span/SpanUtil_ValueCompare.hpp>
#include <FslDataBinding/Base/Bind/AConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/AMultiConverterBinding.hpp>
#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/Internal/IDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/ObserverDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/PropertyChangeState.hpp>
#include <FslDataBinding/Base/Internal/PropertyChangeStateUtil.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <utility>

#ifdef NDEBUG
#define LOCAL_SANITY_CHECK
#endif

#ifndef LOCAL_SANITY_CHECK
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_DO_SANITY_CHECK() \
  {                             \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_DO_SANITY_NO_PROPERTY_CHANGE_STATE() \
  {                                                \
  }
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_DO_SANITY_CHECK() assert(SanityCheck());
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_DO_SANITY_NO_PROPERTY_CHANGE_STATE() assert(SanityCheckNoPropertyChangeState());
#endif

namespace Fsl::DataBinding
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t MaxExecuteLoopCount = 1024;
    }

    struct HandleArrayVector
    {
      std::array<DataBindingInstanceHandle, Internal::DbsConstants::MaxMultiBindSize> Handles;
      uint32_t Count{};

      void AddHandle(const DataBindingInstanceHandle handle) noexcept
      {
        assert(Count < Handles.size());
        Handles[Count] = handle;
        ++Count;
      }

      ReadOnlySpan<DataBindingInstanceHandle> SourceHandles() const noexcept
      {
        return SpanUtil::UncheckedFirstReadOnlySpan(Handles, Count);
      }
    };


    Internal::InstanceState::Flags ToFlags(const DataSourceFlags srcFlags) noexcept
    {
      Internal::InstanceState::Flags dstFlags = Internal::InstanceState::Flags::NoFlags;

      if (DataSourceFlagsUtil::IsEnabled(srcFlags, DataSourceFlags::Observable))
      {
        dstFlags = dstFlags | Internal::InstanceState::Flags::Observable;
      }
      return dstFlags;
    }

    //! Erase the first instance of the given handle
    template <std::size_t TIndex, std::size_t TArraySize>
    bool EraseFirst(TightHostedVector<DataBindingInstanceHandle, TArraySize>& rHandles, DataBindingInstanceHandle handle) noexcept
    {
      static_assert(TIndex < TArraySize, "Index out of bounds");

      auto itr = rHandles.Begin(TIndex);
      const auto itrEnd = rHandles.End(TIndex);
      while (itr != itrEnd)
      {
        if (*itr == handle)
        {
          rHandles.Erase(TIndex, itr);
          return true;
        }
        ++itr;
      }
      return false;
    }

    //! Check if 'record' has a two-way bound target
    bool HasTwoWayBoundTarget(const HandleVector<Internal::ServiceBindingRecord>& instances, const Internal::ServiceBindingRecord& record) noexcept
    {
      auto targets = record.TargetHandles();
      for (const auto target : targets)
      {
        if (instances.FastGet(target.Value).SourceBindingMode() == BindingMode::TwoWay)
        {
          return true;
        }
      }
      return false;
    }

    //! Check if 'record' has a two way bound source or target
    bool IsTwoWayBound(const HandleVector<Internal::ServiceBindingRecord>& instances, const Internal::ServiceBindingRecord& record) noexcept
    {
      return (record.HasValidSourceHandles() && record.SourceBindingMode() == BindingMode::TwoWay) || HasTwoWayBoundTarget(instances, record);
    }

    struct CreateTwoWayGroupContext
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      TwoWayDataBindingGroupManager& rGroupManager;
      const HandleVector<Internal::ServiceBindingRecord>& Instances;
      const DataBindingGroupInstanceHandle Group;

      CreateTwoWayGroupContext(TwoWayDataBindingGroupManager& rTheGroupManager, const HandleVector<Internal::ServiceBindingRecord>& instances,
                               const DataBindingGroupInstanceHandle group)
        : rGroupManager(rTheGroupManager)
        , Instances(instances)
        , Group(group)
      {
      }
    };


    void AddToGroup(const CreateTwoWayGroupContext& context, const DataBindingInstanceHandle hInstance,
                    const Internal::ServiceBindingRecord& instanceRecord)
    {
      // Add the initial entry
      if (context.rGroupManager.TryAddToGroup(context.Group, hInstance))
      {
        {    // Add the targets
          for (const auto hTarget : instanceRecord.TargetHandles())
          {
            const Internal::ServiceBindingRecord* pTargetInstance = context.Instances.TryGet(hTarget.Value);
            if (pTargetInstance != nullptr)
            {
              AddToGroup(context, hTarget, *pTargetInstance);
            }
          }
        }
        if (instanceRecord.HasValidSourceHandles())
        {    // Add sources
          for (const auto hSource : instanceRecord.SourceHandles())
          {
            const Internal::ServiceBindingRecord* pTargetInstance = context.Instances.TryGet(hSource.Value);
            if (pTargetInstance != nullptr)
            {
              AddToGroup(context, hSource, *pTargetInstance);
            }
          }
        }
      }
    }


    // This will not add or remove elements from rInstances so any references or pointers into it will remain unchanged
    bool ClearSourceBindings(HandleVector<Internal::ServiceBindingRecord>& rInstances, const DataBindingInstanceHandle hTarget) noexcept
    {
      Internal::ServiceBindingRecord& rTargetRecord = rInstances.FastGet(hTarget.Value);

      const ReadOnlySpan<DataBindingInstanceHandle> sourceHandles = rTargetRecord.SourceHandles();
      FSLLOG3_VERBOSE4("Clearing source bindings from {}", hTarget.Value);
      for (DataBindingInstanceHandle srcHandle : sourceHandles)
      {
        FSLLOG3_VERBOSE4("- source {}", srcHandle.Value);

        assert(srcHandle != hTarget);
        // Lookup the source
        auto& rOldSource = rInstances.FastGet(srcHandle.Value);
        // then find and remove the key from the source targets vector
        EraseFirst<Internal::ServicePropertyVectorIndex::Targets>(rOldSource.SysHandles, hTarget);
      }
      const bool changed = !sourceHandles.empty();

      rTargetRecord.ClearSourceHandles();
      return changed;
    }


    bool IsSourceBindingsBeingChanged(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget,
                                      const Binding& binding) noexcept
    {
      auto newSourceHandles = binding.SourceHandlesAsSpan();
      auto oldSourceHandles = instances.FastGet(hTarget.Value).SourceHandles();
      return !SpanUtil::ValueEquals(newSourceHandles, oldSourceHandles);
    }


    void VerifyInstancesAreAlive(const HandleVector<Internal::ServiceBindingRecord>& instances, const ReadOnlySpan<DataBindingInstanceHandle> handles)
    {
      for (auto handle : handles)
      {
        const auto& sourceInstance = instances.Get(handle.Value);
        if (sourceInstance.Instance.GetState() != DataBindingInstanceState::Alive)
        {
          throw DeadInstanceException(fmt::format("source '{}' must be alive", handle.Value));
        }
      }
    }

    void ValidateBindToDependencyObject(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget,
                                        const Binding& binding)
    {
      FSL_PARAM_NOT_USED(instances);
      FSL_PARAM_NOT_USED(hTarget);
      FSL_PARAM_NOT_USED(binding);
      throw BindingIncompatiblePropertiesException("Bind target can not be a DependencyObject");
    }


    void ValidateBindToDataSourceObject(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget,
                                        const Binding& binding)
    {
      FSL_PARAM_NOT_USED(instances);
      FSL_PARAM_NOT_USED(hTarget);
      FSL_PARAM_NOT_USED(binding);
      throw BindingIncompatiblePropertiesException("Bind target can not be a DataSourceObject");
    }


    void ValidateBindToDependencyObserverProperty(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                  const DataBindingInstanceHandle hTarget, const Binding& binding)

    {
      // Binds to DependencyObserverProperty must have exactly one source which is a DataSourceObject
      const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
      if (sourceHandlesSpan.size() != 1u)
      {
        throw BindingErrorException("ObserverProperty binding should always have a source count of 1");
      }
      if (instances.FastGet(sourceHandlesSpan.front().Value).Instance.GetType() != DataBindingInstanceType::DataSourceObject)
      {
        throw BindingIncompatiblePropertiesException("ObserverProperty binding sources should always be of type 'DataSourceObject'");
      }
      if (binding.ComplexBinding())
      {
        throw BindingErrorException("ObserverProperty binding should not have a ComplexBinding object");
      }
      if (!instances.FastGet(hTarget.Value).Methods)
      {
        throw InternalErrorException("No methods associated");
      }
      // We have a valid bind observer property bind
    }


    void ValidateTypeAndBindingCompatibilitySet(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                const Internal::IPropertyMethods& targetInstance, const Binding& binding)
    {
      const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
      if (sourceHandlesSpan.size() != 1u)
      {
        throw BindingUnsupportedException(fmt::format(
          "a basic bind between a target and source can only contain exactly one source. But {} sources was supplied", sourceHandlesSpan.size()));
      }

      const Internal::ServiceBindingRecord& sourceRecord = instances.FastGet(sourceHandlesSpan.front().Value);
      if (binding.Mode() == BindingMode::TwoWay && sourceRecord.Methods->IsReadOnly())
      {
        throw TwoWayBindingReadOnlySourceException("A ReadOnly property can not participate in two way binding");
      }

      // Verify that the types match
      if (sourceRecord.Methods->GetType() != targetInstance.GetType())
      {
        // The types did not match
        throw BindingIncompatibleTypesException(
          "target and source type must match or a ConversionBinding or MultiConversionBinding must be supplied");
      }
    }

    void ValidateTypeAndBindingCompatibilityConvert(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                    const Internal::IPropertyMethods& targetInstance, const Binding& binding)
    {
      const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
      if (sourceHandlesSpan.size() != 1u)
      {
        throw BindingUnsupportedException("AConverterBinding takes exactly one source as input!");
      }

      const Internal::ServiceBindingRecord& sourceRecord = instances.FastGet(sourceHandlesSpan.front().Value);
      if (binding.Mode() == BindingMode::TwoWay && sourceRecord.Methods->IsReadOnly())
      {
        throw TwoWayBindingReadOnlySourceException("A ReadOnly property can not participate in two way binding");
      }

      const std::type_index propertySourceType = sourceRecord.Methods->GetType();
      const std::type_index propertyTargetType = targetInstance.GetType();

      // Warning removed as there are good reasons to do this (for example to do a calculation on the value)
      // FSLLOG3_WARNING_IF(propertySourceType == propertyTargetType, "A conversion binding is not necessary as the types are compatible");

      const auto* const pConverter = dynamic_cast<const AConverterBinding*>(binding.ComplexBinding().get());
      if (pConverter == nullptr)
      {
        throw BindingIncompatibleTypesException("binding was not of the promised type!");
      }

      const std::type_index converterSourceType = pConverter->GetSourceType();
      if (propertySourceType != converterSourceType)
      {
        throw BindingIncompatibleTypesException(fmt::format("Conversion source binding '{}' is not compatible with the expected source type of '{}'",
                                                            converterSourceType.name(), propertySourceType.name()));
      }
      const std::type_index converterTargetType = pConverter->GetTargetType();
      if (propertyTargetType != converterTargetType)
      {
        throw BindingIncompatibleTypesException(fmt::format("Conversion target binding '{}' is not compatible with the expected target type of '{}'",
                                                            converterTargetType.name(), propertyTargetType.name()));
      }
    }

    void ValidateTypeAndBindingCompatibilityMultiConvert(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                         const Internal::IPropertyMethods& targetInstance, const Binding& binding)
    {
      const auto* const pConverter = dynamic_cast<const AMultiConverterBinding*>(binding.ComplexBinding().get());
      if (pConverter == nullptr)
      {
        throw BindingIncompatibleTypesException("binding was not of the promised type!");
      }

      {    // Verify the the converter matches the supplied sources
        const ReadOnlySpan<PropertyTypeInfo> converterSourceTypesSpan = pConverter->GetSourceTypes();
        const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
        if (converterSourceTypesSpan.size() != sourceHandlesSpan.size())
        {
          throw BindingUnsupportedException(fmt::format("AMultiConverterBinding was expecting {} bindings but only {} were provided",
                                                        converterSourceTypesSpan.size(), sourceHandlesSpan.size()));
        }
        // This has been verified earlier
        assert(!sourceHandlesSpan.empty());

        // Run through the source bindings and verify the types match the expected converter types
        for (std::size_t sourceIndex = 0; sourceIndex < sourceHandlesSpan.size(); ++sourceIndex)
        {
          const auto hSource = sourceHandlesSpan[sourceIndex];

          const Internal::ServiceBindingRecord& sourceRecord = instances.FastGet(hSource.Value);
          if (binding.Mode() == BindingMode::TwoWay && sourceRecord.Methods->IsReadOnly())
          {
            throw TwoWayBindingReadOnlySourceException("A ReadOnly property can not participate in two way binding");
          }

          const std::type_index propertySourceType = sourceRecord.Methods->GetType();
          if (propertySourceType != converterSourceTypesSpan[sourceIndex].TypeIndex)
          {
            throw BindingIncompatibleTypesException(
              fmt::format("Conversion source binding '{}' is not compatible with the expected source type of '{}'",
                          converterSourceTypesSpan[sourceIndex].TypeIndex.name(), propertySourceType.name()));
          }
        }
      }

      const std::type_index propertyTargetType = targetInstance.GetType();
      const std::type_index converterTargetType = pConverter->GetTargetType();
      if (propertyTargetType != converterTargetType)
      {
        throw BindingIncompatibleTypesException(fmt::format("Conversion target binding '{}' is not compatible with the expected target type of '{}'",
                                                            converterTargetType.name(), propertyTargetType.name()));
      }
    }

    void ValidateTypeAndBindingCompatibility(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                             const Internal::IPropertyMethods& targetInstance, const Binding& binding)
    {
      if (!binding.ComplexBinding())
      {
        ValidateTypeAndBindingCompatibilitySet(instances, targetInstance, binding);
      }
      else
      {
        const BindingType complexBindingType = binding.ComplexBinding()->GetBindingType();
        switch (complexBindingType)
        {
        case BindingType::AConverterBinding:
          ValidateTypeAndBindingCompatibilityConvert(instances, targetInstance, binding);
          break;
        case BindingType::AMultiConverterBinding:
          ValidateTypeAndBindingCompatibilityMultiConvert(instances, targetInstance, binding);
          break;
        default:
          throw BindingUnsupportedException(fmt::format("Unsupported binding type: {}", static_cast<int32_t>(complexBindingType)));
        }
      }
    }

    void ValidateBindToDependencyProperty(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget,
                                          const Binding& binding)
    {
      const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
      // We already verified that this is not empty earlier
      assert(!sourceHandlesSpan.empty());


      // DependencyProperty require that all sources are DependencyProperty's and they must have methods attached
      for (const auto hSource : sourceHandlesSpan)
      {
        const Internal::ServiceBindingRecord& sourcRecord = instances.FastGet(hSource.Value);
        if (sourcRecord.Instance.GetType() != DataBindingInstanceType::DependencyProperty &&
            sourcRecord.Instance.GetType() != DataBindingInstanceType::ReadOnlyDependencyProperty)
        {
          throw BindingIncompatiblePropertiesException("hSource must be a DependencyProperty or ReadOnlyDependencyProperty");
        }
        if (!sourcRecord.Methods)
        {
          // Source must have methods associated (that should have been checked at creation time)
          throw InternalErrorException("No methods associated");
        }
      }

      const Internal::ServiceBindingRecord& targetRecord = instances.FastGet(hTarget.Value);
      if (!targetRecord.Methods)
      {
        // Target must have methods associated (that should have been checked at creation time)
        throw InternalErrorException("No methods associated");
      }

      ValidateTypeAndBindingCompatibility(instances, *targetRecord.Methods, binding);
    }


    void ValidateBind(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget, const Binding& binding)
    {
      // Check that the target is alive
      if (instances.Get(hTarget.Value).Instance.GetState() != DataBindingInstanceState::Alive)
      {
        throw DeadInstanceException("hTarget must be alive");
      }

      {    // Verify that there are at least one source binding and that all source bindings are alive
        const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
        const auto sourceHandleCount = sourceHandlesSpan.size();
        if (sourceHandleCount <= 0u)
        {
          throw BindingUnsupportedException("There should always be at least one source in a binding");
        }
        if (sourceHandleCount > Internal::DbsConstants::MaxMultiBindSize)
        {
          throw BindingUnsupportedException(
            fmt::format("There can not be more than {} source bind entries. Got: {}", Internal::DbsConstants::MaxMultiBindSize, sourceHandleCount));
        }

        // Verify that all the source instances are alive
        VerifyInstancesAreAlive(instances, sourceHandlesSpan);
      }
      // At this point all handles we got from outside has been 'verified' so we can use 'FastGet' operations to retrieve records from now on

      const DataBindingInstanceType targetInstanceType = instances.FastGet(hTarget.Value).Instance.GetType();
      switch (targetInstanceType)
      {
      case DataBindingInstanceType::DependencyObject:
        ValidateBindToDependencyObject(instances, hTarget, binding);
        return;
      case DataBindingInstanceType::DependencyProperty:
        ValidateBindToDependencyProperty(instances, hTarget, binding);
        return;
      case DataBindingInstanceType::DependencyObserverProperty:
        ValidateBindToDependencyObserverProperty(instances, hTarget, binding);
        return;
      case DataBindingInstanceType::DataSourceObject:
        ValidateBindToDataSourceObject(instances, hTarget, binding);
        return;
      case DataBindingInstanceType::ReadOnlyDependencyProperty:
        ValidateBindToDependencyProperty(instances, hTarget, binding);
        return;
      }
      throw BindingUnsupportedException(fmt::format("Unsupported DataBindingInstanceType: {}", static_cast<int32_t>(targetInstanceType)));
    }


    Internal::PropertySetResult ExecuteDependencyPropertyGetSetMultiConverterBinding(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                                                     const Internal::ServiceBindingRecord& target)
    {
      auto* const pMultiConverter = dynamic_cast<AMultiConverterBinding*>(target.SourceUserBinding().get());
      if (pMultiConverter != nullptr)
      {
        std::array<Internal::PropertyGetInfo, Internal::DbsConstants::MaxMultiBindSize> getters;
        auto sourceHandles = target.SourceHandles();
        assert(sourceHandles.size() <= getters.size());
        for (std::size_t i = 0; i < sourceHandles.size(); ++i)
        {
          const Internal::ServiceBindingRecord* const pSourceRecord = instances.TryGet(sourceHandles[i].Value);
          if (pSourceRecord == nullptr)
          {
            FSLLOG3_DEBUG_WARNING("Failed to acquire source binding record");
            return Internal::PropertySetResult::UnsupportedGetType;
          }
          getters[i] = Internal::PropertyGetInfo(pSourceRecord->Instance.GetPropertyMethodsImplType(), pSourceRecord->Methods.get());
        }
        return pMultiConverter->Convert(target.Instance.GetPropertyMethodsImplType(), target.Methods.get(),
                                        SpanUtil::UncheckedFirstReadOnlySpan(getters, sourceHandles.size()));
      }
      return Internal::PropertySetResult::UnsupportedBindingType;
    }


    Internal::PropertySetResult
      ExecuteDependencyPropertyReverseGetSetMultiConverterBinding(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                                  const Internal::ServiceBindingRecord& from)
    {
      // Since this is a reverse operation the 'from' actually contains the converter
      auto* const pMultiConverter = dynamic_cast<AMultiConverterBinding*>(from.SourceUserBinding().get());
      if (pMultiConverter != nullptr)
      {
        std::array<Internal::PropertySetInfo, Internal::DbsConstants::MaxMultiBindSize> setters;
        auto toHandles = from.SourceHandles();
        assert(toHandles.size() <= setters.size());
        for (std::size_t i = 0; i < toHandles.size(); ++i)
        {
          const Internal::ServiceBindingRecord* const pTargetRecord = instances.TryGet(toHandles[i].Value);
          if (pTargetRecord == nullptr)
          {
            FSLLOG3_DEBUG_WARNING("Failed to acqurie binding record");
            return Internal::PropertySetResult::UnsupportedGetType;
          }
          setters[i] = Internal::PropertySetInfo(pTargetRecord->Instance.GetPropertyMethodsImplType(), pTargetRecord->Methods.get());
        }
        std::array<Internal::PropertySetResult, Internal::DbsConstants::MaxMultiBindSize> setResults{};
        if (!pMultiConverter->TryConvertBack(SpanUtil::AsSpan(setResults), SpanUtil::UncheckedFirstReadOnlySpan(setters, toHandles.size()),
                                             Internal::PropertyGetInfo(from.Instance.GetPropertyMethodsImplType(), from.Methods.get())))
        {
          return Internal::PropertySetResult::NotSupported;
        }
        // Scan the result and determine if we encountered a error or if a value was changed/unchanged
        bool valueChanged = false;
        for (std::size_t i = 0; i < toHandles.size(); ++i)
        {
          switch (setResults[i])
          {
          case Internal::PropertySetResult::ValueChanged:
            valueChanged = true;
            break;
          case Internal::PropertySetResult::ValueUnchanged:
            break;
          default:
            return setResults[i];
          }
        }
        return valueChanged ? Internal::PropertySetResult::ValueChanged : Internal::PropertySetResult::ValueUnchanged;
      }
      return Internal::PropertySetResult::UnsupportedBindingType;
    }


    bool HandleSetResult(const Internal::PropertySetResult setResult)
    {
      switch (setResult)
      {
      case Internal::PropertySetResult::ValueUnchanged:
        return false;
      case Internal::PropertySetResult::ValueChanged:
        return true;
      case Internal::PropertySetResult::UnsupportedGetType:
        throw NotSupportedException("Unsupported get type (this should not occur)");
      case Internal::PropertySetResult::UnsupportedSetType:
        throw NotSupportedException("Unsupported set type (this should not occur)");
      case Internal::PropertySetResult::UnsupportedBindingType:
        throw NotSupportedException("Unsupported binding type (this should not occur)");
      case Internal::PropertySetResult::NotSupported:
        throw NotSupportedException("Set is unsupported (this should not occur)");
      }
      throw InternalErrorException(fmt::format("Unsupported setResult: {}", static_cast<int32_t>(setResult)));
    }
  }


  DataBindingService::~DataBindingService()
  {
    MarkShutdownIntend();
  }


  void DataBindingService::MarkShutdownIntend() noexcept
  {
    DestroyScheduledNow();
    if (!m_instances.Empty())
    {
      FSLLOG3_ERROR("there are {} data binding instances that where not properly destroyed", m_instances.Count());
    }
  }


  DataBindingInstanceHandle DataBindingService::CreateDataSourceObject(const DataSourceFlags flags)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDataSourceObject: Can not be called from this context");
    }

    EnsureDestroyCapacity();
    auto handle = m_instances.Add(Internal::ServiceBindingRecord(DataBindingInstanceType::DataSourceObject, ToFlags(flags)));
    return DataBindingInstanceHandle(handle);
  }


  DataBindingInstanceHandle DataBindingService::CreateDependencyObject()
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDependencyObject: Can not be called from this context");
    }

    EnsureDestroyCapacity();
    auto handle = m_instances.Add(Internal::ServiceBindingRecord(DataBindingInstanceType::DependencyObject, Internal::InstanceState::Flags::NoFlags));
    return DataBindingInstanceHandle(handle);
  }


  DataBindingInstanceHandle DataBindingService::CreateDependencyObjectProperty(DataBindingInstanceHandle hDependencyObject,
                                                                               const DependencyPropertyDefinition& propertyDefinition,
                                                                               std::unique_ptr<Internal::IDependencyPropertyMethods> methods)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDependencyObjectProperty: Can not be called from this context");
    }

    // Properties by default are observable (so they can send change notifications)
    const auto flags = Internal::InstanceState::Flags::Observable;
    return DoCreateDependencyObjectProperty(hDependencyObject, propertyDefinition, std::move(methods), DataBindingInstanceType::DependencyProperty,
                                            flags);
  }


  DataBindingInstanceHandle DataBindingService::CreateReadOnlyDependencyObjectProperty(DataBindingInstanceHandle hDependencyObject,
                                                                                       const DependencyPropertyDefinition& propertyDefinition,
                                                                                       std::unique_ptr<Internal::IDependencyPropertyMethods> methods)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDependencyObjectProperty: Can not be called from this context");
    }

    // Properties by default are observable (so they can send change notifications)
    const auto flags = Internal::InstanceState::Flags::Observable;
    return DoCreateDependencyObjectProperty(hDependencyObject, propertyDefinition, std::move(methods),
                                            DataBindingInstanceType::ReadOnlyDependencyProperty, flags);
  }


  DataBindingInstanceHandle
    DataBindingService::CreateDependencyObjectObserverProperty(const DataBindingInstanceHandle hOwner,
                                                               const DependencyPropertyDefinition& propertyDefinition,
                                                               std::unique_ptr<Internal::ObserverDependencyPropertyMethods> methods)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDependencyObjectObserverProperty: Can not be called from this context");
    }

    // Observer properties are by default not observable (since they are used to receive)
    const auto flags = Internal::InstanceState::Flags::NoFlags;
    return DoCreateDependencyObjectProperty(hOwner, propertyDefinition, std::move(methods), DataBindingInstanceType::DependencyObserverProperty,
                                            flags);
  }


  bool DataBindingService::DestroyProperty(const DataBindingInstanceHandle hInstance) noexcept
  {
    assert(!m_instances.IsValidHandle(hInstance.Value) ||
           (m_instances.FastGet(hInstance.Value).Instance.GetType() == DataBindingInstanceType::DependencyProperty ||
            m_instances.FastGet(hInstance.Value).Instance.GetType() == DataBindingInstanceType::DependencyObserverProperty ||
            m_instances.FastGet(hInstance.Value).Instance.GetType() == DataBindingInstanceType::ReadOnlyDependencyProperty));
    return DestroyInstance(hInstance);
  }


  bool DataBindingService::DestroyInstance(const DataBindingInstanceHandle hInstance) noexcept
  {
    LOCAL_DO_SANITY_CHECK();

    assert(m_callContext.State == CallContextState::Idle || m_callContext.State == CallContextState::ExecutingObserverCallbacks);

    Internal::ServiceBindingRecord* pRecord = m_instances.TryGet(hInstance.Value);
    const bool wasKnownAndAlive = pRecord != nullptr && pRecord->Instance.GetState() == DataBindingInstanceState::Alive;
    if (wasKnownAndAlive)
    {
      // Mark the record as destroyed and schedule it for destruction
      pRecord->Instance.SetDataBindingInstanceState(DataBindingInstanceState::DestroyScheduled);
      pRecord->Methods.reset();    // Clear the methods to ensure we don't call a invalid pointer
      for (const auto hProperty : pRecord->PropertyHandles())
      {
        Internal::ServiceBindingRecord& rPropertyRecord = m_instances.FastGet(hProperty.Value);
        rPropertyRecord.Instance.SetDataBindingInstanceState(DataBindingInstanceState::DestroyScheduled);
        rPropertyRecord.Methods.reset();    // Clear the methods to ensure we don't call a invalid pointer
      }
      assert(m_scheduledForDestroy.capacity() > m_scheduledForDestroy.size());
      try
      {
        // Finally schedule the instance for destruction
        m_scheduledForDestroy.push_back(hInstance);
      }
      catch (const std::exception&)
      {
        FSLLOG3_ERROR("push failed, this should never happen as we always ensure that capacity is can contain all possible instances");
        std::abort();
      }
    }
    LOCAL_DO_SANITY_CHECK();
    return wasKnownAndAlive;
  }

  bool DataBindingService::ClearBinding(const DataBindingInstanceHandle hTarget)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks &&
        m_callContext.State != CallContextState::ExecutingChanges)
    {
      throw UsageErrorException("ClearBinding: Can not be called from this context");
    }
    if (m_instances.Get(hTarget.Value).Instance.GetState() != DataBindingInstanceState::Alive)
    {
      throw DeadInstanceException("hSource must be alive");
    }
    return ClearSourceBindings(m_instances, hTarget);
  }


  bool DataBindingService::SetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      const Internal::ServiceBindingRecord& targetInstance = m_instances.Get(hTarget.Value);
      if (targetInstance.Instance.GetType() == DataBindingInstanceType::DependencyObserverProperty)
      {
        // Validate the bind right away before we defer it
        ValidateBind(m_instances, hTarget, binding);
        // DependencyObserverProperty has no complex bindings and exactly one source
        assert(!binding.ComplexBinding());
        const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
        assert(sourceHandlesSpan.size() == 1u);
        m_pendingBindings.emplace(hTarget, sourceHandlesSpan.front(), binding.Mode());
        return true;
      }
      throw UsageErrorException("SetBinding: Can not be called from this context");
    }
    return DoSetBinding(hTarget, binding);
  }


  bool DataBindingService::Changed(const DataBindingInstanceHandle hInstance, const PropertyChangeReason changeReason)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      if (m_callContext.State == CallContextState::ExecutingChanges && m_callContext.ContainsHandle(hInstance))
      {
        // We ignore 'changed' calls from the specific instance that we are executing a callback to
        return true;
      }
      throw UsageErrorException("Changed: Can not be called from this context");
    }
    Internal::ServiceBindingRecord& rChangedInstance = m_instances.Get(hInstance.Value);
    return ScheduleChanged(hInstance, rChangedInstance, changeReason);
  }


  bool DataBindingService::IsPropertyReadOnly(const DataBindingInstanceHandle hInstance) const noexcept
  {
    bool isReadOnly = false;
    if (m_callContext.State != CallContextState::ExecutingChanges)
    {
      const Internal::ServiceBindingRecord* const pRecord = m_instances.TryGet(hInstance.Value);
      isReadOnly = pRecord == nullptr || (pRecord->HasValidSourceHandles() && !IsTwoWayBound(m_instances, *pRecord));
    }
    return isReadOnly;
  }


  void DataBindingService::ExecuteChanges()
  {
    if (m_callContext.State != CallContextState::Idle)
    {
      throw UsageErrorException("ExecuteChanges: Can not be called from this context");
    }
    uint32_t changeLoopCounter = 0;
    do
    {
      assert(m_changesOneWay.empty());

      // Since we are single threaded and destroy the instance id's here
      // It means that the 'deferred' changes to destroyed instances will fail to acquire their instance and thereby be ignored.
      DestroyScheduledNow();

      // Determine pending changes
      DeterminePendingChanges();
      // Execute all pending changes
      ExecutePendingChangesNow();
      // Execute all pending observer bindings now
      ExecutePendingBindingsNow();

      ExecuteObserverCallbacksNow();
      ++changeLoopCounter;
    } while (!m_pendingChanges.empty() && changeLoopCounter < LocalConfig::MaxExecuteLoopCount);
    FSLLOG3_WARNING_IF(changeLoopCounter >= LocalConfig::MaxExecuteLoopCount, "ExecuteChanges max allowed loops reached");
  }


  bool DataBindingService::DoSetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding)
  {
    assert(m_callContext.State == CallContextState::Idle || m_callContext.State == CallContextState::ExecutingObserverCallbacks ||
           m_callContext.State == CallContextState::ExecutePendingBindings);
    LOCAL_DO_SANITY_CHECK();
    if (!binding.HasValidSourceHandles())
    {
      return ClearBinding(hTarget);
    }
    try
    {
      if (binding.ContainsSource(hTarget))
      {
        throw CyclicBindingException("Circular dependency found (cant bind to itself)");
      }
      ValidateBind(m_instances, hTarget, binding);
    }
    catch (const std::exception&)
    {
      ClearSourceBindings(m_instances, hTarget);
      LOCAL_DO_SANITY_CHECK();
      throw;
    }

    const bool changed = IsSourceBindingsBeingChanged(m_instances, hTarget, binding);
    if (changed)
    {
      // the binding was changed, so delete the old binding (if it exist)
      ClearSourceBindings(m_instances, hTarget);
    }

    LOCAL_DO_SANITY_CHECK();

    // check if the binding is being modified
    if (changed)
    {
      HandleArrayVector inProgressSourceBindings;
      try
      {
        for (const auto hSource : binding.SourceHandlesAsSpan())
        {
          inProgressSourceBindings.AddHandle(hSource);

          // Check if the source would create a cyclic dependency
          CheckForCyclicDependencies(hTarget, hSource);

          if (binding.Mode() == BindingMode::TwoWay)
          {
            CheckTwoWayBindingSourceRules(hSource);
          }

          // Link the source to the target
          // - We do the push first so that if it throws 'the new bind' will be fully reversed.
          //   But we do lose any previous binding that might have been removed already
          m_instances.FastGet(hSource.Value).SysHandles.PushBack(Internal::ServicePropertyVectorIndex::Targets, hTarget);
        }

        // All cyclic dependency checks passed and we updated all the sources with hTarget as a target
        // So its time to update the target record with the new source
        Internal::ServiceBindingRecord& rTargetInstance = m_instances.FastGet(hTarget.Value);
        if (binding.Mode() == BindingMode::OneWay)
        {
          CheckTwoWayBindingTargetRules(rTargetInstance);
        }

        rTargetInstance.SetSource(binding);
        // If the target is marked as changed, mark all parent sources as changed as well
        if (rTargetInstance.Instance.HasPendingChanges())
        {
          RecursiveMarkAsChanged(hTarget, rTargetInstance);
        }
        LOCAL_DO_SANITY_CHECK();

        // We only allow skipping during
        assert(m_callContext.State != CallContextState::ExecutePendingBindings ||
               rTargetInstance.Instance.GetType() == DataBindingInstanceType::DependencyObserverProperty);

        if (m_callContext.State != CallContextState::ExecutePendingBindings)
        {
          // Mark source as changed as a simple but slightly expensive way to populate the target
          for (const auto hSource : binding.SourceHandlesAsSpan())
          {
            ScheduleChanged(hSource, m_instances.FastGet(hSource.Value), PropertyChangeReason::Refresh);
          }
        }

        LOCAL_DO_SANITY_CHECK();
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_VERBOSE("Exception occurred: '{}'", ex.what());
        {    // Clear the source handles of the target
          Internal::ServiceBindingRecord& rTargetInstance = m_instances.FastGet(hTarget.Value);
          rTargetInstance.ClearSourceHandles();
        }

        // Rolling back all in-progress changes
        for (const auto hSource : inProgressSourceBindings.SourceHandles())
        {
          EraseFirst<Internal::ServicePropertyVectorIndex::Targets>(m_instances.FastGet(hSource.Value).SysHandles, hTarget);
        }
        LOCAL_DO_SANITY_CHECK();
        throw;
      }
    }
    return changed;
  }


  bool DataBindingService::ScheduleChanged(const DataBindingInstanceHandle hChangedInstance, Internal::ServiceBindingRecord& rChangedInstance,
                                           const PropertyChangeReason changeReason)
  {
    assert(rChangedInstance.Instance.IsObservable());

    bool allowChanges = true;
    if (rChangedInstance.Instance.GetState() == DataBindingInstanceState::Alive &&
        (rChangedInstance.HasValidSourceHandles() || !rChangedInstance.SysHandles.Empty(Internal::ServicePropertyVectorIndex::Targets)))
    {
      const auto instanceChangeState = rChangedInstance.Instance.GetPropertyChangeState();
      assert(changeReason == PropertyChangeReason::Refresh || changeReason == PropertyChangeReason::Modified);
      assert(instanceChangeState == Internal::PropertyChangeState::Unchanged || instanceChangeState == Internal::PropertyChangeState::Refresh ||
             instanceChangeState == Internal::PropertyChangeState::Modified);

      if (instanceChangeState == Internal::PropertyChangeState::Unchanged)
      {    // Untouched instance, so just mark it with the reason and push a instance changed entry
        rChangedInstance.Instance.SetPropertyChangeState(Internal::PropertyChangeStateUtil::UncheckedToPropertyChangeState(changeReason));
        // The instance can not exist already as any 'pushed' entries are marked with a different PropertyChangeState
        assert(std::find(m_pendingChanges.begin(), m_pendingChanges.end(), hChangedInstance) == m_pendingChanges.end());
        m_pendingChanges.push_back(hChangedInstance);
      }
      else if (instanceChangeState == Internal::PropertyChangeState::Refresh && changeReason == PropertyChangeReason::Refresh)
      {    // the instance is marked as a refresh change and the new change is also a refresh request, so just remove the initial queued entry and
           // insert a new one
        assert(instanceChangeState == Internal::PropertyChangeState::Refresh);
        // Replace the 'refresh' request with a more serious modify request
        auto itrFind = std::find(m_pendingChanges.begin(), m_pendingChanges.end(), hChangedInstance);
        assert(itrFind != m_pendingChanges.end());
        m_pendingChanges.erase(itrFind);
        m_pendingChanges.push_back(hChangedInstance);
      }
      else if (changeReason == PropertyChangeReason::Modified)
      {    // as its a modify we just replace any existing entry with the new state
        assert(instanceChangeState == Internal::PropertyChangeState::Refresh || instanceChangeState == Internal::PropertyChangeState::Modified);
        // Replace the 'refresh' request with a more serious modify request
        auto itrFind = std::find(m_pendingChanges.begin(), m_pendingChanges.end(), hChangedInstance);
        assert(itrFind != m_pendingChanges.end());
        m_pendingChanges.erase(itrFind);
        rChangedInstance.Instance.SetPropertyChangeState(Internal::PropertyChangeState::Modified);
        m_pendingChanges.push_back(hChangedInstance);
      }
    }
    // the new design always allow the local changes and rely on the 'ExecuteChanges' to correct it
    return allowChanges;
  }


  void DataBindingService::RecursiveMarkAsChanged(const DataBindingInstanceHandle hInstance, Internal::ServiceBindingRecord& rInstance)
  {
    assert(rInstance.Instance.GetState() == DataBindingInstanceState::Alive);

    ReadOnlySpan<DataBindingInstanceHandle> sourceHandles = rInstance.SourceHandles();
    bool wasMarked = rInstance.Instance.HasPendingChanges();
    rInstance.Instance.MarkPendingChanges();

    bool hasSource = !sourceHandles.empty();
    if (hasSource)
    {
      hasSource = false;
      for (const auto& hSource : sourceHandles)
      {
        Internal::ServiceBindingRecord& rSourceInstance = m_instances.FastGet(hSource.Value);
        if (rSourceInstance.Instance.GetState() == DataBindingInstanceState::Alive)
        {
          hasSource = true;
          RecursiveMarkAsChanged(hSource, rSourceInstance);
        }
      }
    }
    if (!hasSource)
    {
      if (!wasMarked)
      {
        m_changesOneWay.push(hInstance);
      }
    }
  }


  bool DataBindingService::SanityCheck() const
  {
    for (uint32_t i = 0; i < m_instances.Count(); ++i)
    {
      const Internal::ServiceBindingRecord& record = m_instances[i];
      const auto hCurrent = DataBindingInstanceHandle(m_instances.FastIndexToHandle(i));

      // Validate that all handle properties are valid
      for (const auto handle : record.PropertyHandles())
      {
        if (m_instances.TryGet(handle.Value) == nullptr)
        {
          return false;
        }
      }
      for (const auto handle : record.TargetHandles())
      {
        // Verify that the instance exist
        const auto* pRecord = m_instances.TryGet(handle.Value);
        if (pRecord == nullptr)
        {    // The record no longer exist so the To entries are corrupt
          return false;
        }
        // Verify that the handle exist in the found Source
        if (!pRecord->ContainsSource(hCurrent))
        {    // The handle was not found (so the two way linking is corrupt)
          return false;
        }
      }

      {
        // Verify that the instance exist
        for (auto sourceHandle : record.SourceHandles())
        {
          const Internal::ServiceBindingRecord* pSourceRecord = m_instances.TryGet(sourceHandle.Value);
          if (pSourceRecord == nullptr)
          {    // The record no longer exist so the Source entries are corrupt
            return false;
          }
          // Verify that the handle exist in the found To
          const auto sourceRecordTargetSpan = pSourceRecord->TargetHandles();
          if (std::find(sourceRecordTargetSpan.begin(), sourceRecordTargetSpan.end(), hCurrent) == sourceRecordTargetSpan.end())
          {    // The handle was not found (so the two way linking is corrupt)
            return false;
          }
          // Verify that two way bindings are valid
          if (record.SourceBindingMode() == BindingMode::TwoWay && pSourceRecord->HasValidSourceHandles() &&
              pSourceRecord->SourceBindingMode() == BindingMode::OneWay)
          {
            // A source that is bound as 'two way' can not itself have a source that is one way
            return false;
          }
        }

        // We do only need to run this check if the record is marked as changed and if it has sources attached
        if (record.Instance.HasPendingChanges() && record.HasValidSourceHandles())
        {    // Check that instances that are marked with pending changes either has no source or all parent sources are marked as well
          if (!SanityCheckAllParentSourceMarkedAsChanged(record))
          {
            return false;
          }
        }
      }
    }
    return true;
  }

  bool DataBindingService::SanityCheckAllParentSourceMarkedAsChanged(const Internal::ServiceBindingRecord& record) const
  {
    auto sources = record.SourceHandles();
    for (auto hSource : sources)
    {
      const Internal::ServiceBindingRecord& sourceRecord = m_instances.FastGet(hSource.Value);

      if (sourceRecord.Instance.GetState() == DataBindingInstanceState::Alive &&
          (!sourceRecord.Instance.HasPendingChanges() ||
           (sourceRecord.HasValidSourceHandles() && !SanityCheckAllParentSourceMarkedAsChanged(sourceRecord))))
      {
        return false;
      }
    }
    return true;
  }


  bool DataBindingService::SanityCheckNoPropertyChangeState() const
  {
    if (!SanityCheck())
    {
      return false;
    }
    for (uint32_t i = 0; i < m_instances.Count(); ++i)
    {
      if (m_instances[i].Instance.GetPropertyChangeState() != Internal::PropertyChangeState::Unchanged)
      {
        return false;
      }
    }
    return true;
  }


  DataBindingInstanceHandle DataBindingService::DoCreateDependencyObjectProperty(
    const DataBindingInstanceHandle hOwner, [[maybe_unused]] const DependencyPropertyDefinition& propertyDefinition,
    std::unique_ptr<Internal::IPropertyMethods> methods, const DataBindingInstanceType propertyType, const Internal::InstanceState::Flags flags)
  {
    assert(m_callContext.State == CallContextState::Idle || m_callContext.State == CallContextState::ExecutingObserverCallbacks);
    assert(propertyType == DataBindingInstanceType::DependencyProperty || propertyType == DataBindingInstanceType::DependencyObserverProperty ||
           propertyType == DataBindingInstanceType::ReadOnlyDependencyProperty);

    // We can not cache the returned record as we do a add to m_instances before we need to write to it
    {
      auto ownerType = m_instances.Get(hOwner.Value).Instance.GetType();
      if (ownerType != DataBindingInstanceType::DependencyObject && ownerType != DataBindingInstanceType::DataSourceObject &&
          ownerType != DataBindingInstanceType::ReadOnlyDependencyProperty && ownerType != DataBindingInstanceType::DependencyProperty)
      {
        throw InvalidParentInstanceException(
          "hDependencyObject must be a DataBindingInstanceType::DependencyObject, DataBindingInstanceType::DataSourceObject, "
          "DataBindingInstanceType::ReadOnlyDependencyProperty or DataBindingInstanceType::DependencyProperty");
      }
    }
    EnsureDestroyCapacity();

    const Internal::PropertyMethodsImplType methodsImplType = methods->GetImplType();
    DataBindingInstanceHandle hNew(m_instances.Add(Internal::ServiceBindingRecord(propertyType, methodsImplType, flags, std::move(methods))));
    try
    {
      Internal::ServiceBindingRecord& rParentInstance = m_instances.FastGet(hOwner.Value);
      rParentInstance.SysHandles.PushBack(Internal::ServicePropertyVectorIndex::Properties, hNew);
    }
    catch (const std::exception&)
    {
      DoDestroyInstanceNow(hNew);
      assert(SanityCheck());
      throw;
    }
    LOCAL_DO_SANITY_CHECK();
    return hNew;
  }

  void DataBindingService::ExecutePendingBindingsNow()
  {
    assert(m_callContext.State == CallContextState::Idle);
    LOCAL_DO_SANITY_CHECK();

    try
    {
      m_callContext.State = CallContextState::ExecutePendingBindings;
      while (!m_pendingBindings.empty())
      {
        const auto pendingBinding = m_pendingBindings.front();
        m_pendingBindings.pop();
        DoSetBinding(pendingBinding.TargetHandle, Binding(pendingBinding.SourceHandle, pendingBinding.Mode));
        m_pendingObserverCallbacks.emplace(pendingBinding.TargetHandle, pendingBinding.SourceHandle);
      }
      m_callContext = {};
    }
    catch (const std::exception&)
    {
      m_callContext = {};
      FSLLOG3_ERROR("Exeception during DataBinding ExecuteObserverCallbacksNow");
      throw;
    }
    LOCAL_DO_SANITY_CHECK();
  }


  void DataBindingService::ExecuteObserverCallbacksNow()
  {
    assert(m_callContext.State == CallContextState::Idle);
    assert(m_callContext.HandlesEmpty());

    if (!m_pendingObserverCallbacks.empty())
    {
      LOCAL_DO_SANITY_CHECK();

      try
      {
        m_callContext.State = CallContextState::ExecutingObserverCallbacks;
        while (!m_pendingObserverCallbacks.empty())
        {
          const auto observerRecord = m_pendingObserverCallbacks.front();
          m_pendingObserverCallbacks.pop();
          ExecuteInstanceObserverCallback(observerRecord.TargetHandle, observerRecord.SourceHandle);
        }
        m_callContext = {};
      }
      catch (const std::exception&)
      {
        m_callContext = {};
        FSLLOG3_ERROR("Exeception during DataBinding ExecuteObserverCallbacksNow");
        throw;
      }
      LOCAL_DO_SANITY_CHECK();
    }
  }


  void DataBindingService::DeterminePendingChanges()
  {
    assert(m_changesOneWay.empty());

    m_groupManager.ClearGroups();
    for (const DataBindingInstanceHandle hChangedInstance : m_pendingChanges)
    {
      Internal::ServiceBindingRecord* pChangedInstance = m_instances.TryGet(hChangedInstance.Value);
      if (pChangedInstance != nullptr)
      {
        DeterminePendingChange(hChangedInstance, *pChangedInstance);
        pChangedInstance->Instance.ClearPropertyChangeState();
      }
    }
    m_pendingChanges.clear();

    // m_groupManager now contains information about the last element that was changed inside a two-way binding group
    ScheduleTwoWayChanges();

    // Sanity check the current state. This verifies that no nodes are marked as changed after 'DeterminePendingChanges' was done
    LOCAL_DO_SANITY_NO_PROPERTY_CHANGE_STATE();
  }

  void DataBindingService::DeterminePendingChange(const DataBindingInstanceHandle hChangedInstance, Internal::ServiceBindingRecord& rChangedInstance)
  {
    if (rChangedInstance.Instance.GetState() == DataBindingInstanceState::Alive)
    {
      if (IsTwoWayBound(m_instances, rChangedInstance))
      {    // A two way binding, so we need to resolve
        DataBindingGroupInstanceHandle hTwoWayGroup = m_groupManager.TryGetGroup(hChangedInstance);
        const auto changeReason =
          Internal::PropertyChangeStateUtil::UncheckedToPropertyChangeReason(rChangedInstance.Instance.GetPropertyChangeState());
        if (!hTwoWayGroup.IsValid())
        {
          hTwoWayGroup = m_groupManager.CreateGroup(hChangedInstance, changeReason);
          CreateTwoWayGroupContext context(m_groupManager, m_instances, hTwoWayGroup);
          AddToGroup(context, hChangedInstance, rChangedInstance);
          assert(hTwoWayGroup.IsValid());
        }
        else if (changeReason != PropertyChangeReason::Refresh)
        {    // Only update the info if its something else than refresh
          m_groupManager.UncheckedSetGroupInfo(hTwoWayGroup, hChangedInstance, changeReason);
        }
      }
      else
      {
        if (!rChangedInstance.HasValidSourceHandles())
        {    // there is no source attached to the instance
          if (!rChangedInstance.SysHandles.Empty(Internal::ServicePropertyVectorIndex::Targets) && !rChangedInstance.Instance.HasPendingChanges())
          {
            rChangedInstance.Instance.MarkPendingChanges();
            m_changesOneWay.push(hChangedInstance);
          }
        }
        else
        {    // there is a source attached, but the constraints on the instance was changed, so allow the change and mark all 'parent' sources as
          RecursiveMarkAsChanged(hChangedInstance, rChangedInstance);
        }
      }
    }
  }


  void DataBindingService::ScheduleTwoWayChanges()
  {
    assert(m_changesTwoWay.empty());
    const uint32_t groupCount = m_groupManager.GroupCount();
    for (uint32_t i = 0; i < groupCount; ++i)
    {
      const DataBindingInstanceHandle hChangedInstanceHandle = m_groupManager[i];
      m_changesTwoWay.push(hChangedInstanceHandle);
    }
    // Clear the group manager content as its no longer needed
    m_groupManager.ClearGroups();
  }

  // Beware this modifies the m_pendingObserverCallbacks with observer instances that need to be executed
  void DataBindingService::ExecutePendingChangesNow()
  {
    assert(m_pendingObserverCallbacks.empty());
    ExecutePendingTwoWayChangesNow();
    ExecutePendingOneWayChangesNow();
  }

  void DataBindingService::ExecutePendingTwoWayChangesNow()
  {
    assert(m_callContext.State == CallContextState::Idle);
    assert(m_callContext.HandlesEmpty());

    if (!m_changesTwoWay.empty())
    {
      LOCAL_DO_SANITY_CHECK();

      try
      {
        m_callContext.State = CallContextState::ExecutingChanges;
        while (!m_changesTwoWay.empty())
        {
          const auto hChangedInstance = m_changesTwoWay.front();
          m_changesTwoWay.pop();
          auto* pChangedInstance = m_instances.TryGet(hChangedInstance.Value);
          if (pChangedInstance != nullptr)
          {
            pChangedInstance->Instance.ClearPendingChanges();

            // this method does not modify m_instances so the reference "pChangedInstance" into it is safe
            ExecuteTwoWayChangesTo(hChangedInstance, *pChangedInstance, {});
          }
        }
        m_callContext = {};
      }
      catch (const std::exception&)
      {
        m_callContext = {};
        FSLLOG3_ERROR("Exeception during DataBinding ExecutPendingChangesNow");
        throw;
      }
      LOCAL_DO_SANITY_CHECK();
    }
  }

  void DataBindingService::ExecutePendingOneWayChangesNow()
  {
    assert(m_callContext.State == CallContextState::Idle);
    assert(m_callContext.HandlesEmpty());

    if (!m_changesOneWay.empty())
    {
      LOCAL_DO_SANITY_CHECK();

      try
      {
        m_callContext.State = CallContextState::ExecutingChanges;
        while (!m_changesOneWay.empty())
        {
          const auto hChangedInstance = m_changesOneWay.front();
          m_changesOneWay.pop();
          auto* pChangedInstance = m_instances.TryGet(hChangedInstance.Value);
          if (pChangedInstance != nullptr)
          {
            assert(pChangedInstance->Instance.HasPendingChanges());
            pChangedInstance->Instance.ClearPendingChanges();

            // this method does not modify m_instances so the reference "pChangedInstance" into it is safe
            ExecuteOneWayChangesTo(hChangedInstance, *pChangedInstance);
          }
        }
        m_callContext = {};
      }
      catch (const std::exception&)
      {
        m_callContext = {};
        FSLLOG3_ERROR("Exception during DataBinding ExecutePendingOneWayChangesNow");
        throw;
      }
      LOCAL_DO_SANITY_CHECK();
    }
  }

  void DataBindingService::ExecuteTwoWayChangesTo(const DataBindingInstanceHandle hSource, const Internal::ServiceBindingRecord& source,
                                                  const DataBindingInstanceHandle hSkip)
  {
    assert(m_callContext.State == CallContextState::ExecutingChanges);
    if (source.Instance.GetState() == DataBindingInstanceState::Alive)
    {
      // Execute the change to 'sources'
      for (const auto hFrom : source.SourceHandles())
      {
        if (hFrom != hSkip)
        {
          auto& rFrom = m_instances.FastGet(hFrom.Value);
          if (rFrom.Instance.GetState() == DataBindingInstanceState::Alive)
          {
            switch (rFrom.Instance.GetType())
            {
            // case DataBindingInstanceType::DependencyObserverProperty:
            //   m_pendingObserverCallbacks.emplace(hFrom, hSource);
            //   break;
            case DataBindingInstanceType::DependencyProperty:
              ExecuteDependencyPropertyReverseGetSet(hFrom, rFrom, source);
              // Two-way changes are always propagated to all 'group members'
              rFrom.Instance.ClearPendingChanges();
              // Execute any pending changes to dependent resources
              ExecuteTwoWayChangesTo(hFrom, rFrom, hSource);
              break;
            default:
              throw InternalErrorException("Change to a object of a unsupported type");
            }
          }
          else
          {
            FSLLOG3_ERROR("Encountered dead 'to' instance, that ought to have been filtered before");
          }
        }
      }

      // Execute the change to 'targets'
      for (const auto hTarget : source.TargetHandles())
      {
        if (hTarget != hSkip)
        {
          auto& rTarget = m_instances.FastGet(hTarget.Value);
          if (rTarget.Instance.GetState() == DataBindingInstanceState::Alive)
          {
            switch (rTarget.Instance.GetType())
            {
            // case DataBindingInstanceType::DependencyObserverProperty:
            //   m_pendingObserverCallbacks.emplace(hTarget, hSource);
            //   break;
            case DataBindingInstanceType::DependencyProperty:
              ExecuteDependencyPropertyGetSet(hTarget, rTarget, source);
              // Two-way changes are always propagated to all 'group members'
              rTarget.Instance.ClearPendingChanges();
              // Execute any pending changes to dependent resources
              ExecuteTwoWayChangesTo(hTarget, rTarget, hSource);
              break;
            default:
              throw InternalErrorException("Change to a object of a unsupported type");
            }
          }
          else
          {
            FSLLOG3_ERROR("Encountered dead 'to' instance, that ought to have been filtered before");
          }
        }
      }
    }
    else
    {
      FSLLOG3_ERROR("Encountered dead 'changed' instance, that ought to have been filtered before");
    }
  }

  // Beware this modifies the m_pendingObserverCallbacks with observer instances that need to be executed
  void DataBindingService::ExecuteOneWayChangesTo(const DataBindingInstanceHandle hSource, const Internal::ServiceBindingRecord& source)
  {
    assert(m_callContext.State == CallContextState::ExecutingChanges);
    if (source.Instance.GetState() == DataBindingInstanceState::Alive)
    {
      // Execute the change
      for (const auto hTarget : source.TargetHandles())
      {
        auto& rTarget = m_instances.FastGet(hTarget.Value);
        if (rTarget.Instance.GetState() == DataBindingInstanceState::Alive)
        {
          bool changed = false;
          switch (rTarget.Instance.GetType())
          {
          case DataBindingInstanceType::DependencyObserverProperty:
            m_pendingObserverCallbacks.emplace(hTarget, hSource);
            break;
          // case DataBindingInstanceType::DependencyObject:
          // case DataBindingInstanceType::Target:
          //  assert(target.pDataTarget != nullptr);
          //  target.pDataTarget->OnChanged(hChangedInstance);
          //  break;
          case DataBindingInstanceType::DependencyProperty:
            changed = ExecuteDependencyPropertyGetSet(hTarget, rTarget, source);
            break;
          default:
            throw InternalErrorException("Change to a object of a unsupported type");
          }
          if (changed || rTarget.Instance.HasPendingChanges())
          {
            rTarget.Instance.ClearPendingChanges();

            // Execute any pending changes to dependent resources
            ExecuteOneWayChangesTo(hTarget, rTarget);
          }
        }
        else
        {
          FSLLOG3_ERROR("Encountered dead 'to' instance, that ought to have been filtered before");
        }
      }
    }
    else
    {
      FSLLOG3_ERROR("Encountered dead 'changed' instance, that ought to have been filtered before");
    }
  }


  void DataBindingService::ExecuteInstanceObserverCallback(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource)
  {
    assert(m_callContext.State == CallContextState::ExecutingObserverCallbacks);
    assert(hTarget.IsValid());
    assert(hSource.IsValid());
    try
    {
      Internal::IPropertyMethods* pMethods = nullptr;
      {
        const Internal::ServiceBindingRecord& record = m_instances.FastGet(hTarget.Value);
        assert(record.Instance.GetType() == DataBindingInstanceType::DependencyObserverProperty);
        assert(record.Methods);
        if (record.Instance.GetState() == DataBindingInstanceState::Alive)
        {
          pMethods = record.Methods.get();
        }
        else
        {
          FSLLOG3_VERBOSE4("observed callback object is dead, so skipping callback");
        }
      }
      if (pMethods != nullptr)
      {
        if (!pMethods->TryInvoke(hSource))
        {
          FSLLOG3_WARNING("Invoke failed");
        }
        // The source should not have been deleted while executing this
        assert(m_instances.IsValidHandle(hSource.Value));
      }
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Exeception occurred during ExecuteInstanceObserverCallback: {}", ex.what());
      throw;
    }
  }


  bool DataBindingService::ExecuteDependencyPropertyGetSet(const DataBindingInstanceHandle hTarget, const Internal::ServiceBindingRecord& target,
                                                           const Internal::ServiceBindingRecord& source)
  {
    assert(m_callContext.State == CallContextState::ExecutingChanges);
    assert(hTarget.IsValid());
    assert(source.Methods);
    assert(target.Methods);
    Internal::PropertySetResult setResult{Internal::PropertySetResult::NotSupported};
    try
    {
      {
        m_callContext.SetHandle(hTarget);
        if (!target.SourceUserBinding())
        {
          // We assume we have a both a get and set
          setResult = target.Methods->TrySet(source.Methods.get());
        }
        else
        {
          auto* const pConverter = dynamic_cast<AConverterBinding*>(target.SourceUserBinding().get());
          if (pConverter != nullptr)
          {
            setResult = pConverter->Convert(target.Instance.GetPropertyMethodsImplType(), target.Methods.get(),
                                            source.Instance.GetPropertyMethodsImplType(), source.Methods.get());
          }
          else
          {
            setResult = ExecuteDependencyPropertyGetSetMultiConverterBinding(m_instances, target);
          }
        }
        m_callContext.ClearHandles();
      }
      assert(m_callContext.State == CallContextState::ExecutingChanges);
    }
    catch (const std::exception& ex)
    {
      assert(m_callContext.State == CallContextState::ExecutingChanges);
      m_callContext.ClearHandles();
      FSLLOG3_ERROR("Exeception occurred during ExecuteDependencyPropertyGetSet callback: {}", ex.what());
      throw;
    }
    return HandleSetResult(setResult);
  }


  bool DataBindingService::ExecuteDependencyPropertyReverseGetSet(const DataBindingInstanceHandle hTo, const Internal::ServiceBindingRecord& to,
                                                                  const Internal::ServiceBindingRecord& from)
  {
    assert(m_callContext.State == CallContextState::ExecutingChanges);
    assert(hTo.IsValid());
    assert(from.Methods);
    assert(to.Methods);
    Internal::PropertySetResult setResult{Internal::PropertySetResult::NotSupported};
    try
    {
      {
        // Since its a reverse get/set we need to check the source user binding to see if there is a converter inplace
        if (!from.SourceUserBinding())
        {
          m_callContext.SetHandle(hTo);
          // We assume we have a both a get and set
          setResult = to.Methods->TrySet(from.Methods.get());
        }
        else
        {
          auto* const pConverter = dynamic_cast<AConverterBinding*>(from.SourceUserBinding().get());
          if (pConverter != nullptr)
          {
            m_callContext.SetHandle(hTo);

            setResult = pConverter->ConvertBack(to.Instance.GetPropertyMethodsImplType(), to.Methods.get(),
                                                from.Instance.GetPropertyMethodsImplType(), from.Methods.get());
          }
          else
          {
            m_callContext.SetHandles(from.SourceHandles());
            setResult = ExecuteDependencyPropertyReverseGetSetMultiConverterBinding(m_instances, from);
          }
        }
        m_callContext.ClearHandles();
      }
      assert(m_callContext.State == CallContextState::ExecutingChanges);
    }
    catch (const std::exception& ex)
    {
      assert(m_callContext.State == CallContextState::ExecutingChanges);
      m_callContext.ClearHandles();
      FSLLOG3_ERROR("Exeception occurred during ExecuteDependencyPropertyGetSet callback: {}", ex.what());
      throw;
    }
    return HandleSetResult(setResult);
  }

  void DataBindingService::DestroyScheduledNow() noexcept
  {
    for (const auto hDestroy : m_scheduledForDestroy)
    {
      DoDestroyInstanceNow(hDestroy);
    }
    m_scheduledForDestroy.clear();
  }


  void DataBindingService::EnsureDestroyCapacity()
  {
    std::size_t desiredCapacity(m_instances.Count());
    ++desiredCapacity;

    if (desiredCapacity > m_scheduledForDestroy.capacity())
    {
      m_scheduledForDestroy.reserve(desiredCapacity + 128);
    }
  }


  void DataBindingService::DoDestroyInstanceNow(DataBindingInstanceHandle hInstance) noexcept
  {
    auto* pRecord = m_instances.TryGet(hInstance.Value);
    if (pRecord != nullptr)
    {
      pRecord->Instance.SetDataBindingInstanceState(DataBindingInstanceState::Destroyed);
      FSLLOG3_VERBOSE4("Destroying instance {}", hInstance.Value);

      // Remove all bindings that use this instance as a target
      if (pRecord->HasValidSourceHandles())
      {
        // This will not add or remove elements from m_instances so our pRecord pointer is still valid after the call
        ClearSourceBindings(m_instances, hInstance);
      }

      // Remove all bindings that use this instance as a source
      for (const auto hTarget : pRecord->SysHandles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Targets))
      {
        Internal::ServiceBindingRecord& rToRecord = m_instances.FastGet(hTarget.Value);
        assert(rToRecord.ContainsSource(hInstance));
        // This warning could safely occur during 'shutdown' so we need a way to eliminate false warnings.
        if (rToRecord.SourceHandleCount() > 1u)
        {
          // FSLLOG3_DEBUG_WARNING("- Clearing multibind source because one of the linked sources was destroyed");
          for (const DataBindingInstanceHandle srcHandle : rToRecord.SourceHandles())
          {
            FSLLOG3_VERBOSE4("- Remove source {} from {}", srcHandle.Value, hTarget.Value);
            Internal::ServiceBindingRecord& rMultiBindSource = m_instances.FastGet(srcHandle.Value);
            EraseFirst<Internal::ServicePropertyVectorIndex::Targets>(rMultiBindSource.SysHandles, hTarget);
          }
        }
        else
        {
          FSLLOG3_VERBOSE4("- Remove source {} from {}", hInstance.Value, hTarget.Value);
        }
        rToRecord.ClearSourceHandles();
      }
      pRecord->ClearTargetHandles();

      // Destroy all properties of this instance
      for (const auto hProperty : pRecord->PropertyHandles())
      {
        DoDestroyInstanceNow(hProperty);
      }
      pRecord->ClearPropertyHandles();

      m_instances.FastRemoveAt(m_instances.FastHandleToIndex(hInstance.Value));
    }
  }


  void DataBindingService::CheckForCyclicDependencies(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource) const
  {
    if (hTarget == hSource)
    {
      throw CyclicBindingException("Circular dependency found (cant bind to itself)");
    }

    const auto& targetRecord = m_instances.FastGet(hTarget.Value);
    // The target is not expected to have any existing source
    assert(!targetRecord.HasValidSourceHandles());

    for (const auto entry : targetRecord.TargetHandles())
    {
      if (IsInstanceTarget(entry, hSource))
      {
        throw CyclicBindingException("Circular dependency found");
      }
    }
  }


  void DataBindingService::CheckTwoWayBindingSourceRules(const DataBindingInstanceHandle hSource) const
  {
    const Internal::ServiceBindingRecord& sourceRecord = m_instances.FastGet(hSource.Value);
    if (sourceRecord.HasValidSourceHandles() && sourceRecord.SourceBindingMode() == BindingMode::OneWay)
    {
      throw TwoWayBindingSourceException("A two way binding source can not be the target of a one way binding");
    }
  }


  void DataBindingService::CheckTwoWayBindingTargetRules(const Internal::ServiceBindingRecord& targetInstance) const
  {
    for (const auto entry : targetInstance.TargetHandles())
    {
      const Internal::ServiceBindingRecord& targetRecord = m_instances.FastGet(entry.Value);
      if (targetRecord.SourceBindingMode() == BindingMode::TwoWay)
      {
        throw TwoWayBindingSourceException("A one way binding target can not be a two way binding source");
      }
    }
  }


  bool DataBindingService::IsInstanceTarget(const DataBindingInstanceHandle hInstance, const DataBindingInstanceHandle hEntry) const
  {
    bool isTarget = hInstance == hEntry;
    if (!isTarget)
    {
      const auto& record = m_instances.FastGet(hInstance.Value);
      if (record.Instance.GetState() == DataBindingInstanceState::Alive)
      {
        for (const auto entry : record.TargetHandles())
        {
          if (IsInstanceTarget(entry, hEntry))
          {
            isTarget = true;
            break;
          }
        }
      }
    }
    return isTarget;
  }
}
