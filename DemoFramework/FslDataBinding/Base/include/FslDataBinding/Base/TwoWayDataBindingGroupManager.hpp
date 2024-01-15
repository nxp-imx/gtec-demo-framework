#ifndef FSLDATABINDING_BASE_TWOWAYDATABINDINGGROUPMANAGER_HPP
#define FSLDATABINDING_BASE_TWOWAYDATABINDINGGROUPMANAGER_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslDataBinding/Base/DataBindingGroupInstanceHandle.hpp>
#include <FslDataBinding/Base/DataBindingInstanceHandle.hpp>
#include <FslDataBinding/Base/PropertyChangeReason.hpp>
#include <unordered_map>

namespace Fsl::DataBinding
{
  class TwoWayDataBindingGroupManager final
  {
    struct GroupRecord
    {
      DataBindingInstanceHandle ChangedInstanceHandle;
      PropertyChangeReason Reason{PropertyChangeReason::Refresh};

      GroupRecord() = default;
      GroupRecord(const DataBindingInstanceHandle hChangedInstanceHandle, const PropertyChangeReason reason)
        : ChangedInstanceHandle(hChangedInstanceHandle)
        , Reason(reason)
      {
      }
    };

    HandleVector<GroupRecord> m_groups;

    std::unordered_map<uint32_t, DataBindingGroupInstanceHandle> m_instanceToGroupMap;

  public:
    DataBindingGroupInstanceHandle TryGetGroup(const DataBindingInstanceHandle hInstance) const noexcept
    {
      auto itrFind = m_instanceToGroupMap.find(hInstance.Value);
      return itrFind != m_instanceToGroupMap.end() ? itrFind->second : DataBindingGroupInstanceHandle();
    }

    DataBindingGroupInstanceHandle CreateGroup(const DataBindingInstanceHandle hChangedInstanceHandle, const PropertyChangeReason reason);
    void UncheckedSetGroupInfo(const DataBindingGroupInstanceHandle hGroup, const DataBindingInstanceHandle hChangedInstanceHandle,
                               const PropertyChangeReason reason);
    void AddToGroup(const DataBindingGroupInstanceHandle hGroup, const DataBindingInstanceHandle hInstance);
    bool TryAddToGroup(const DataBindingGroupInstanceHandle hGroup, const DataBindingInstanceHandle hInstance);
    void ClearGroups();

    uint32_t GroupCount() const noexcept
    {
      return m_groups.Count();
    }

    DataBindingInstanceHandle operator[](const uint32_t index) const noexcept
    {
      return m_groups[index].ChangedInstanceHandle;
    }
  };
}

#endif
