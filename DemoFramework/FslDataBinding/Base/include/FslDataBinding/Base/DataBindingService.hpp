#ifndef FSLDATABINDING_BASE_DATABINDINGSERVICE_HPP
#define FSLDATABINDING_BASE_DATABINDINGSERVICE_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslDataBinding/Base/BindingMode.hpp>
#include <FslDataBinding/Base/DataBindingInstanceHandle.hpp>
#include <FslDataBinding/Base/DataSourceFlags.hpp>
#include <FslDataBinding/Base/Internal/IPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/ServiceBindingRecord.hpp>
#include <FslDataBinding/Base/PropertyChangeReason.hpp>
#include <FslDataBinding/Base/TwoWayDataBindingGroupManager.hpp>
#include <array>
#include <deque>
#include <memory>
#include <queue>
#include <typeindex>
#include <utility>

namespace Fsl::DataBinding
{
  class Binding;
  class DependencyPropertyDefinition;

  namespace Internal
  {
    class IDependencyPropertyMethods;
    class ObserverDependencyPropertyMethods;

    namespace DbsConstants
    {
      constexpr uint32_t MaxMultiBindSize = 8;
    }
  }

  class DataBindingService final
  {
    friend class DataBindingServiceDebug;

    enum class CallContextState
    {
      Idle,
      ExecutingChanges,
      ExecutePendingBindings,
      ExecutingObserverCallbacks
    };

    struct CallContext
    {
      CallContextState State{CallContextState::Idle};

    private:
      std::array<DataBindingInstanceHandle, Internal::DbsConstants::MaxMultiBindSize> m_handles;
      std::uint32_t m_handleCount;

    public:
      void ClearHandles()
      {
        assert(m_handleCount <= m_handles.size());
        for (std::size_t i = 0; i < m_handleCount; ++i)
        {
          m_handles[i] = {};
        }
        m_handleCount = 0;
      }

      void SetHandle(const DataBindingInstanceHandle handle)
      {
        assert(m_handleCount == 0);
        m_handles[0] = handle;
        m_handleCount = 1;
      }

      void SetHandles(const ReadOnlySpan<DataBindingInstanceHandle> handles)
      {
        assert(m_handleCount == 0);
        assert(handles.size() <= m_handles.size());
        for (std::size_t i = 0; i < handles.size(); ++i)
        {
          m_handles[i] = handles[i];
        }
        m_handleCount = UncheckedNumericCast<uint32_t>(handles.size());
      }

      bool HandlesEmpty() const
      {
        return m_handleCount <= 0;
      }

      bool ContainsHandle(const DataBindingInstanceHandle handle) const
      {
        for (std::size_t i = 0; i < m_handleCount; ++i)
        {
          if (m_handles[i] == handle)
          {
            return true;
          }
        }
        return false;
      }
    };

    struct ObserverRecord
    {
      DataBindingInstanceHandle TargetHandle;
      DataBindingInstanceHandle SourceHandle;

      constexpr ObserverRecord() noexcept = default;
      constexpr explicit ObserverRecord(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource) noexcept
        : TargetHandle(hTarget)
        , SourceHandle(hSource)
      {
      }
    };

    struct DeferredBindingRecord
    {
      DataBindingInstanceHandle TargetHandle;
      DataBindingInstanceHandle SourceHandle;
      BindingMode Mode{BindingMode::OneWay};

      DeferredBindingRecord() = default;
      DeferredBindingRecord(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource, const BindingMode mode) noexcept
        : TargetHandle(hTarget)
        , SourceHandle(hSource)
        , Mode(mode)
      {
      }
    };

    TwoWayDataBindingGroupManager m_groupManager;

    HandleVector<Internal::ServiceBindingRecord> m_instances;
    // We ensure that this vector can always hold m_instances.Count entries (so the schedule of a destroy will never fail)
    std::vector<DataBindingInstanceHandle> m_scheduledForDestroy;
    std::vector<DataBindingInstanceHandle> m_pendingChanges;
    std::queue<DataBindingInstanceHandle> m_changesOneWay;
    std::queue<DataBindingInstanceHandle> m_changesTwoWay;
    std::queue<ObserverRecord> m_pendingObserverCallbacks;
    std::queue<DeferredBindingRecord> m_pendingBindings;
    CallContext m_callContext;

  public:
    DataBindingService(const DataBindingService&) = delete;
    DataBindingService& operator=(const DataBindingService&) = delete;

    DataBindingService() = default;
    ~DataBindingService();
    void MarkShutdownIntend() noexcept;

    std::size_t InstanceCount() const noexcept
    {
      return m_instances.Count();
    }

    std::size_t PendingChanges() const noexcept
    {
      return m_pendingChanges.size();
    }

    std::size_t PendingDestroys() const noexcept
    {
      return m_scheduledForDestroy.size();
    }

    DataBindingInstanceHandle CreateDataSourceObject(const DataSourceFlags flags);

    //! @brief Create a dependency object
    //! @note  This is used for any dependency object currently being tracked and bind-able.
    DataBindingInstanceHandle CreateDependencyObject();


    //! @brief Create a dependency object property
    //! @param hDependencyObject the DependencyObject to create the property on.
    //! @note  This creates a actual 'bindable' property
    DataBindingInstanceHandle CreateDependencyObjectProperty(DataBindingInstanceHandle hDependencyObject,
                                                             const DependencyPropertyDefinition& propertyDefinition,
                                                             std::unique_ptr<Internal::IDependencyPropertyMethods> methods);

    //! @brief Create a dependency object property
    //! @param hDependencyObject the DependencyObject to create the property on.
    //! @note  This creates a actual 'bindable' property
    DataBindingInstanceHandle CreateReadOnlyDependencyObjectProperty(DataBindingInstanceHandle hDependencyObject,
                                                                     const DependencyPropertyDefinition& propertyDefinition,
                                                                     std::unique_ptr<Internal::IDependencyPropertyMethods> methods);

    //! @brief Create a dependency object observer property
    DataBindingInstanceHandle CreateDependencyObjectObserverProperty(const DataBindingInstanceHandle hOwner,
                                                                     const DependencyPropertyDefinition& propertyDefinition,
                                                                     std::unique_ptr<Internal::ObserverDependencyPropertyMethods> methods);

    //! Call this if you want to 'verify' that the instance was a property during debugging.
    bool DestroyProperty(const DataBindingInstanceHandle hInstance) noexcept;
    //! Can destroy instance types
    bool DestroyInstance(const DataBindingInstanceHandle hInstance) noexcept;

    bool ClearBinding(const DataBindingInstanceHandle hTarget);
    bool SetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding);
    bool Changed(const DataBindingInstanceHandle hInstance, const PropertyChangeReason changeReason);
    bool IsPropertyReadOnly(const DataBindingInstanceHandle hInstance) const noexcept;

    //! Execute all pending changes
    void ExecuteChanges();

    //! Scan the internal state to see if its consistent
    bool SanityCheck() const;

  private:
    bool SanityCheckAllParentSourceMarkedAsChanged(const Internal::ServiceBindingRecord& record) const;
    bool SanityCheckNoPropertyChangeState() const;

    DataBindingInstanceHandle DoCreateDependencyObjectProperty(const DataBindingInstanceHandle hOwner,
                                                               const DependencyPropertyDefinition& propertyDefinition,
                                                               std::unique_ptr<Internal::IPropertyMethods> methods,
                                                               const DataBindingInstanceType propertyType,
                                                               const Internal::InstanceState::Flags flags);

    void DeterminePendingChanges();
    void DeterminePendingChange(const DataBindingInstanceHandle hChangedInstance, Internal::ServiceBindingRecord& rChangedInstance);
    void ScheduleTwoWayChanges();
    void ExecutePendingChangesNow();
    void ExecutePendingTwoWayChangesNow();
    void ExecutePendingOneWayChangesNow();
    void ExecutePendingBindingsNow();
    void ExecuteObserverCallbacksNow();
    void ExecuteTwoWayChangesTo(const DataBindingInstanceHandle hSource, const Internal::ServiceBindingRecord& source,
                                const DataBindingInstanceHandle hSkip);
    void ExecuteOneWayChangesTo(const DataBindingInstanceHandle hSource, const Internal::ServiceBindingRecord& source);
    void ExecuteInstanceObserverCallback(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource);
    bool ExecuteDependencyPropertyGetSet(const DataBindingInstanceHandle hTarget, const Internal::ServiceBindingRecord& target,
                                         const Internal::ServiceBindingRecord& source);
    bool ExecuteDependencyPropertyReverseGetSet(const DataBindingInstanceHandle hTo, const Internal::ServiceBindingRecord& to,
                                                const Internal::ServiceBindingRecord& from);

    // If a exception occurs when provided with a valid binding the 'old' source will be cleared and nothing of the requested bind will have been
    // applied
    bool DoSetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding);

    bool ScheduleChanged(const DataBindingInstanceHandle hChangedInstance, Internal::ServiceBindingRecord& rChangedInstance,
                         const PropertyChangeReason changeReason);
    void RecursiveMarkAsChanged(const DataBindingInstanceHandle hInstance, Internal::ServiceBindingRecord& rInstance);
    void DestroyScheduledNow() noexcept;
    void EnsureDestroyCapacity();
    void DoDestroyInstanceNow(DataBindingInstanceHandle hInstance) noexcept;

    void CheckForCyclicDependencies(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource) const;
    void CheckTwoWayBindingSourceRules(const DataBindingInstanceHandle hSource) const;
    void CheckTwoWayBindingTargetRules(const Internal::ServiceBindingRecord& targetInstance) const;
    bool IsInstanceTarget(const DataBindingInstanceHandle hInstance, const DataBindingInstanceHandle hEntry) const;
  };
}

#endif
