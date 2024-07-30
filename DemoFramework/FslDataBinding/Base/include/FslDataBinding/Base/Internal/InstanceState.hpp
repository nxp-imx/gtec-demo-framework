#ifndef FSLDATABINDING_BASE_INTERNAL_INSTANCESTATE_HPP
#define FSLDATABINDING_BASE_INTERNAL_INSTANCESTATE_HPP
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
#include <FslDataBinding/Base/DataBindingInstanceState.hpp>
#include <FslDataBinding/Base/DataBindingInstanceType.hpp>
#include <FslDataBinding/Base/Internal/PropertyChangeState.hpp>
#include <FslDataBinding/Base/Internal/PropertyMethodsImplType.hpp>
#include <cassert>

namespace Fsl::DataBinding::Internal
{
  namespace InstanceStateShifts
  {
    constexpr uint32_t DataBindingInstanceState = 8;
    constexpr uint32_t PropertyMethodsImplType = 12;
    constexpr uint32_t PropertyChangeState = 16;
  }

  struct InstanceState
  {
    using base_type = uint32_t;

    enum class Flags : base_type
    {
      NoFlags = 0,


      // Set if this
      Observable = 0x20000000,
      // Is set if there are pending changes
      HasPendingChanges = 0x40000000,

      // NOLINTNEXTLINE(readability-identifier-naming)
      MASK_DataBindingInstanceType = 0x000000FF,
      // NOLINTNEXTLINE(readability-identifier-naming)
      MASK_DataBindingInstanceState = 0x00000F00,
      // NOLINTNEXTLINE(readability-identifier-naming)
      MASK_PropertyMethodsImplType = 0x0000F000,
      // NOLINTNEXTLINE(readability-identifier-naming)
      MASK_PropertyChangeState = 0x00030000
    };

  private:
    Flags m_flags{Flags::NoFlags};

  public:
    constexpr InstanceState() noexcept = default;


    explicit constexpr InstanceState(const DataBindingInstanceType type, const PropertyMethodsImplType methodsImplType) noexcept
    {
      SetDataBindingInstanceType(type);
      SetDataBindingInstanceState(DataBindingInstanceState::Alive);
      SetPropertyMethodsImplType(methodsImplType);
    }

    explicit constexpr InstanceState(const DataBindingInstanceType type, const PropertyMethodsImplType methodsImplType, const Flags flags) noexcept
      : m_flags(flags)
    {
      // When calling this the type flags should really be empty
      assert(!IsAnyEnabled(Flags::MASK_DataBindingInstanceType));
      assert(!IsAnyEnabled(Flags::MASK_DataBindingInstanceState));
      assert(!IsAnyEnabled(Flags::MASK_PropertyMethodsImplType));
      SetDataBindingInstanceType(type);
      SetDataBindingInstanceState(DataBindingInstanceState::Alive);
      SetPropertyMethodsImplType(methodsImplType);
    }

    constexpr inline bool IsObservable() const noexcept
    {
      return static_cast<Flags>((static_cast<base_type>(m_flags) & static_cast<base_type>(Flags::Observable))) == Flags::Observable;
    }

    constexpr inline bool HasPendingChanges() const noexcept
    {
      return static_cast<Flags>((static_cast<base_type>(m_flags) & static_cast<base_type>(Flags::HasPendingChanges))) == Flags::HasPendingChanges;
    }

    constexpr inline void MarkPendingChanges() noexcept
    {
      m_flags = static_cast<Flags>(static_cast<base_type>(m_flags) | static_cast<base_type>(Flags::HasPendingChanges));
    }

    constexpr inline void ClearPendingChanges() noexcept
    {
      m_flags = static_cast<Flags>(static_cast<base_type>(m_flags) & (~static_cast<base_type>(Flags::HasPendingChanges)));
    }


    constexpr inline bool IsEnabled(const Flags flag) const noexcept
    {
      return static_cast<Flags>((static_cast<base_type>(m_flags) & static_cast<base_type>(flag))) == flag;
    }


    constexpr inline bool IsAnyEnabled(const Flags flag) const noexcept
    {
      return (static_cast<base_type>(m_flags) & static_cast<base_type>(flag)) != 0u;
    }

    constexpr inline void Enable(const Flags flag) noexcept
    {
      m_flags = static_cast<Flags>(static_cast<base_type>(m_flags) | static_cast<base_type>(flag));
    }


    constexpr inline void Disable(const Flags flag) noexcept
    {
      m_flags = static_cast<Flags>(static_cast<base_type>(m_flags) & (~static_cast<base_type>(flag)));
    }

    constexpr inline void Set(const Flags flag, const bool enabled) noexcept
    {
      m_flags = static_cast<Flags>(enabled ? (static_cast<base_type>(m_flags) | static_cast<base_type>(flag))
                                           : (static_cast<base_type>(m_flags) & (~static_cast<base_type>(flag))));
    }

    constexpr inline DataBindingInstanceType GetType() const noexcept
    {
      return static_cast<DataBindingInstanceType>(static_cast<base_type>(m_flags) & static_cast<base_type>(Flags::MASK_DataBindingInstanceType));
    }

    constexpr inline DataBindingInstanceState GetState() const noexcept
    {
      return static_cast<DataBindingInstanceState>((static_cast<base_type>(m_flags) & static_cast<base_type>(Flags::MASK_DataBindingInstanceState)) >>
                                                   InstanceStateShifts::DataBindingInstanceState);
    }
    constexpr inline PropertyMethodsImplType GetPropertyMethodsImplType() const noexcept
    {
      return static_cast<PropertyMethodsImplType>((static_cast<base_type>(m_flags) & static_cast<base_type>(Flags::MASK_PropertyMethodsImplType)) >>
                                                  InstanceStateShifts::PropertyMethodsImplType);
    }


    constexpr void SetDataBindingInstanceState(const DataBindingInstanceState state) noexcept
    {
      assert(static_cast<base_type>(state) <=
             (static_cast<base_type>(Flags::MASK_DataBindingInstanceState) >> InstanceStateShifts::DataBindingInstanceState));

      const uint32_t stateFlags = (static_cast<base_type>(state) << InstanceStateShifts::DataBindingInstanceState) &
                                  static_cast<base_type>(Flags::MASK_DataBindingInstanceState);
      m_flags = static_cast<Flags>((static_cast<base_type>(m_flags) & (~static_cast<base_type>(Flags::MASK_DataBindingInstanceState))) | stateFlags);
    }


    constexpr inline PropertyChangeState GetPropertyChangeState() const noexcept
    {
      return static_cast<PropertyChangeState>((static_cast<base_type>(m_flags) & static_cast<base_type>(Flags::MASK_PropertyChangeState)) >>
                                              InstanceStateShifts::PropertyChangeState);
    }

    constexpr void SetPropertyChangeState(const PropertyChangeState changeReason) noexcept
    {
      assert(static_cast<base_type>(changeReason) <=
             (static_cast<base_type>(Flags::MASK_PropertyChangeState) >> InstanceStateShifts::PropertyChangeState));

      const uint32_t stateFlags =
        (static_cast<base_type>(changeReason) << InstanceStateShifts::PropertyChangeState) & static_cast<base_type>(Flags::MASK_PropertyChangeState);
      m_flags = static_cast<Flags>((static_cast<base_type>(m_flags) & (~static_cast<base_type>(Flags::MASK_PropertyChangeState))) | stateFlags);
    }

    constexpr void ClearPropertyChangeState() noexcept
    {
      m_flags = static_cast<Flags>(static_cast<base_type>(m_flags) & (~static_cast<base_type>(Flags::MASK_PropertyChangeState)));
    }

  private:
    constexpr void SetDataBindingInstanceType(const DataBindingInstanceType type) noexcept
    {
      assert(static_cast<base_type>(type) < static_cast<base_type>(Flags::MASK_DataBindingInstanceType));

      const uint32_t typeFlags = (static_cast<base_type>(type) & static_cast<base_type>(Flags::MASK_DataBindingInstanceType));
      m_flags = static_cast<Flags>((static_cast<base_type>(m_flags) & (~static_cast<base_type>(Flags::MASK_DataBindingInstanceType))) | typeFlags);
    }
    constexpr void SetPropertyMethodsImplType(const PropertyMethodsImplType implType) noexcept
    {
      assert(static_cast<base_type>(implType) <=
             (static_cast<base_type>(Flags::MASK_PropertyMethodsImplType) >> InstanceStateShifts::PropertyMethodsImplType));

      const uint32_t stateFlags = (static_cast<base_type>(implType) << InstanceStateShifts::PropertyMethodsImplType) &
                                  static_cast<base_type>(Flags::MASK_PropertyMethodsImplType);
      m_flags = static_cast<Flags>((static_cast<base_type>(m_flags) & (~static_cast<base_type>(Flags::MASK_PropertyMethodsImplType))) | stateFlags);
    }
  };


  constexpr inline InstanceState::Flags operator|(const InstanceState::Flags lhs, const InstanceState::Flags rhs) noexcept
  {
    return static_cast<InstanceState::Flags>(static_cast<InstanceState::base_type>(lhs) | static_cast<InstanceState::base_type>(rhs));
  }

  constexpr inline InstanceState::Flags operator&(const InstanceState::Flags lhs, const InstanceState::Flags rhs) noexcept
  {
    return static_cast<InstanceState::Flags>(static_cast<InstanceState::base_type>(lhs) & static_cast<InstanceState::base_type>(rhs));
  }
}

#endif
