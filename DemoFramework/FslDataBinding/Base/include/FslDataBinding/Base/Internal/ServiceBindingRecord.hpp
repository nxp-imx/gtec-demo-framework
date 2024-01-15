#ifndef FSLDATABINDING_BASE_INTERNAL_SERVICEBINDINGRECORD_HPP
#define FSLDATABINDING_BASE_INTERNAL_SERVICEBINDINGRECORD_HPP
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

#include <FslBase/Collections/TightHostedVector.hpp>
#include <FslDataBinding/Base/Bind/IComplexBinding.hpp>
#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/BindingMode.hpp>
#include <FslDataBinding/Base/DataBindingInstanceHandle.hpp>
#include <FslDataBinding/Base/Internal/IPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/InstanceState.hpp>
#include <cassert>
#include <memory>

namespace Fsl::DataBinding::Internal
{
  namespace ServicePropertyVectorIndex
  {
    constexpr std::size_t Sources = 0;
    constexpr std::size_t Properties = 1;
    constexpr std::size_t Targets = 2;
  }

  struct ServiceBindingRecord
  {
  private:
    struct ServiceBindingSourceRecord
    {
      DataBindingInstanceHandle Handle;
      std::shared_ptr<IComplexBinding> UserBinding;
      BindingMode Mode{BindingMode::OneWay};

      constexpr ServiceBindingSourceRecord() noexcept = default;
      explicit ServiceBindingSourceRecord(const DataBindingInstanceHandle handle, std::shared_ptr<IComplexBinding> userBinding,
                                          const BindingMode mode) noexcept
        : Handle(handle)
        , UserBinding(std::move(userBinding))
        , Mode(mode)
      {
      }
    };

    ServiceBindingSourceRecord m_source;

  public:
    Internal::InstanceState Instance;
    // It's still public for erase atm
    TightHostedVector<DataBindingInstanceHandle, 3> m_handles;
    std::unique_ptr<Internal::IPropertyMethods> Methods;

    ServiceBindingRecord() = default;
    explicit ServiceBindingRecord(const DataBindingInstanceType type, const Internal::InstanceState::Flags flags)
      : Instance(type, Internal::PropertyMethodsImplType::NotAvailable, flags)
    {
    }

    explicit ServiceBindingRecord(const DataBindingInstanceType type, const Internal::PropertyMethodsImplType methodsImplType,
                                  const Internal::InstanceState::Flags flags, std::unique_ptr<Internal::IPropertyMethods> operation)
      : Instance(type, methodsImplType, flags)
      , Methods(std::move(operation))
    {
    }

    bool HasValidSourceHandles() const noexcept
    {
      // There will only be valid handles if the 'primary' source handle is valid
      return m_source.Handle.IsValid();
    }

    BindingMode SourceBindingMode() const noexcept
    {
      assert(HasValidSourceHandles());
      return m_source.Mode;
    }

    const std::shared_ptr<IComplexBinding>& SourceUserBinding() const noexcept
    {
      return m_source.UserBinding;
    }

    std::size_t SourceHandleCount() const noexcept
    {
      if (m_source.Handle.IsValid())
      {
        return m_handles.Empty(ServicePropertyVectorIndex::Sources) ? 1u : m_handles.Size(ServicePropertyVectorIndex::Sources);
      }
      return 0u;
    }

    ReadOnlySpan<DataBindingInstanceHandle> TargetHandles() const noexcept
    {
      return m_handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Targets);
    }

    void ClearTargetHandles() noexcept
    {
      m_handles.Clear(Internal::ServicePropertyVectorIndex::Targets);
    }

    ReadOnlySpan<DataBindingInstanceHandle> PropertyHandles() const noexcept
    {
      return m_handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Properties);
    }

    void ClearPropertyHandles() noexcept
    {
      m_handles.Clear(Internal::ServicePropertyVectorIndex::Properties);
    }

    ReadOnlySpan<DataBindingInstanceHandle> SourceHandles() const noexcept
    {
      if (m_source.Handle.IsValid())
      {
        return m_handles.Empty(ServicePropertyVectorIndex::Sources)
                 ? ReadOnlySpan<DataBindingInstanceHandle>(&m_source.Handle, 1u, OptimizationCheckFlag::NoCheck)
                 : m_handles.AsReadOnlySpan(ServicePropertyVectorIndex::Sources);
      }
      return {};
    }

    bool ContainsSource(const DataBindingInstanceHandle handle) const noexcept
    {
      for (auto srcHandle : SourceHandles())
      {
        if (handle == srcHandle)
        {
          return true;
        }
      }
      return false;
    }

    //! BEWARE: this only clears the source handles stored here. All linked information in the actual source records need to be deleted
    //! before calling this
    void ClearSourceHandles() noexcept
    {
      m_source = {};
      m_handles.Clear(Internal::ServicePropertyVectorIndex::Sources);
    }

    //! Beware this only sets the source record. All linked information will have to be set elsewhere
    //! We expect all existing source handles to have been cleared before this is called.
    void SetSource(const Binding& binding)
    {
      assert(!m_source.Handle.IsValid());
      assert(!m_source.UserBinding);
      assert(SourceHandles().empty());
      auto sourceHandles = binding.SourceHandlesAsSpan();
      assert(!sourceHandles.empty());
      try
      {
        m_source = ServiceBindingSourceRecord(sourceHandles.front(), binding.ComplexBinding(), binding.Mode());
        if (sourceHandles.size() > 1u)
        {
          m_handles.PushBack(Internal::ServicePropertyVectorIndex::Sources, sourceHandles);
        }
      }
      catch (const std::exception&)
      {
        ClearSourceHandles();
        throw;
      }
    }
  };
}

#endif
