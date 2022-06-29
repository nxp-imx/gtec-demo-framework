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
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslDataBinding/Base/DataBindingInstanceHandle.hpp>
#include <FslDataBinding/Base/DataSourceFlags.hpp>
#include <FslDataBinding/Base/Internal/IPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/ServiceBindingRecord.hpp>
#include <array>
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
      DataBindingInstanceHandle Handle;
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

      DeferredBindingRecord() = default;
      DeferredBindingRecord(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource)
        : TargetHandle(hTarget)
        , SourceHandle(hSource)
      {
      }
    };

    HandleVector<Internal::ServiceBindingRecord> m_instances;
    // We ensure that this vector can always hold m_instances.Count entries (so the schedule of a destroy will never fail)
    std::vector<DataBindingInstanceHandle> m_scheduledForDestroy;
    std::queue<DataBindingInstanceHandle> m_changes;
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
      return m_changes.size();
    }

    std::size_t PendingDestroys() const noexcept
    {
      return m_scheduledForDestroy.size();
    }

    DataBindingInstanceHandle CreateDependencyObject();
    DataBindingInstanceHandle CreateDataSourceObject(const DataSourceFlags flags);
    DataBindingInstanceHandle CreateDependencyObjectProperty(DataBindingInstanceHandle hDependencyObject,
                                                             const DependencyPropertyDefinition& propertyDefinition,
                                                             std::unique_ptr<Internal::IDependencyPropertyMethods> methods);
    DataBindingInstanceHandle CreateDependencyObjectObserverProperty(const DataBindingInstanceHandle hOwner,
                                                                     const DependencyPropertyDefinition& propertyDefinition,
                                                                     std::unique_ptr<Internal::ObserverDependencyPropertyMethods> methods);

    //! Call this if you want to 'verify' that the instance was a property during debugging.
    bool DestroyProperty(const DataBindingInstanceHandle hInstance) noexcept;
    //! Can destroy instance types
    bool DestroyInstance(const DataBindingInstanceHandle hInstance) noexcept;

    bool ClearBinding(const DataBindingInstanceHandle hTarget);
    bool SetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding);
    bool Changed(const DataBindingInstanceHandle hInstance);
    bool IsPropertyReadOnly(const DataBindingInstanceHandle hInstance) const noexcept;

    //! Execute all pending changes
    void ExecuteChanges();

    //! Scan the internal state to see if its consistent
    bool SanityCheck() const;

  private:
    DataBindingInstanceHandle DoCreateDependencyObjectProperty(const DataBindingInstanceHandle hOwner,
                                                               const DependencyPropertyDefinition& propertyDefinition,
                                                               std::unique_ptr<Internal::IPropertyMethods> methods,
                                                               const DataBindingInstanceType propertyType,
                                                               const Internal::InstanceState::Flags flags);


    void ExecutePendingChangesNow();
    void ExecutePendingBindingsNow();
    void ExecuteObserverCallbacksNow();
    void ExecuteChangesTo(const DataBindingInstanceHandle hSource, const Internal::ServiceBindingRecord& source);
    void ExecuteInstanceObserverCallback(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource);
    bool ExecuteDependencyPropertyGetSet(const DataBindingInstanceHandle hTarget, const Internal::ServiceBindingRecord& target,
                                         const Internal::ServiceBindingRecord& source);

    // If a exception occurs when provided with a valid binding the 'old' source will be cleared and nothing of the requested bind will have been
    // applied
    bool DoSetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding);

    bool ScheduleChanged(const DataBindingInstanceHandle hChangedInstance, Internal::ServiceBindingRecord& rChangedInstance, const bool force);
    void DestroyScheduledNow() noexcept;
    void EnsureDestroyCapacity();
    void DoDestroyInstanceNow(DataBindingInstanceHandle hInstance) noexcept;

    void CheckForCyclicDependencies(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource) const;
    bool IsInstanceTarget(const DataBindingInstanceHandle hInstance, const DataBindingInstanceHandle hEntry) const;
  };
}

#endif
