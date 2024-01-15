#ifndef FSLDATABINDING_BASE_INTERNAL_PROPERTYCHANGESTATEUTIL_HPP
#define FSLDATABINDING_BASE_INTERNAL_PROPERTYCHANGESTATEUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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
#include <FslDataBinding/Base/Internal/PropertyChangeState.hpp>
#include <FslDataBinding/Base/PropertyChangeReason.hpp>
#include <cassert>

namespace Fsl::DataBinding::Internal::PropertyChangeStateUtil
{
  inline constexpr PropertyChangeState UncheckedToPropertyChangeState(const PropertyChangeReason reason) noexcept
  {
    assert(reason == PropertyChangeReason::Refresh || reason == PropertyChangeReason::Modified);
    return static_cast<PropertyChangeState>(static_cast<uint32_t>(reason) + 1u);
  };

  inline constexpr PropertyChangeReason UncheckedToPropertyChangeReason(const PropertyChangeState state) noexcept
  {
    assert(state == Internal::PropertyChangeState::Refresh || state == Internal::PropertyChangeState::Modified);
    return static_cast<PropertyChangeReason>(static_cast<uint32_t>(state) - 1u);
  };

  // Validate conversion assumption
  static_assert(UncheckedToPropertyChangeState(PropertyChangeReason::Modified) == PropertyChangeState::Modified);
  static_assert(UncheckedToPropertyChangeState(PropertyChangeReason::Refresh) == PropertyChangeState::Refresh);

  static_assert(UncheckedToPropertyChangeReason(PropertyChangeState::Modified) == PropertyChangeReason::Modified);
  static_assert(UncheckedToPropertyChangeReason(PropertyChangeState::Refresh) == PropertyChangeReason::Refresh);
}

#endif
