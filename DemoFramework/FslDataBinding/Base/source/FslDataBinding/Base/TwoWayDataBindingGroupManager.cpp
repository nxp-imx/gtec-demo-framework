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

#include <FslDataBinding/Base/TwoWayDataBindingGroupManager.hpp>
#include <cassert>

namespace Fsl::DataBinding
{
  DataBindingGroupInstanceHandle TwoWayDataBindingGroupManager::CreateGroup(const DataBindingInstanceHandle hChangedInstanceHandle,
                                                                            const PropertyChangeReason reason)
  {
    return DataBindingGroupInstanceHandle(m_groups.Add(GroupRecord(hChangedInstanceHandle, reason)));
  }


  void TwoWayDataBindingGroupManager::UncheckedSetGroupInfo(const DataBindingGroupInstanceHandle hGroup,
                                                            const DataBindingInstanceHandle hChangedInstanceHandle, const PropertyChangeReason reason)
  {
    GroupRecord& rGroupEntry = m_groups.FastGet(hGroup.Value);
    rGroupEntry.ChangedInstanceHandle = hChangedInstanceHandle;
    rGroupEntry.Reason = reason;
  }


  void TwoWayDataBindingGroupManager::AddToGroup(const DataBindingGroupInstanceHandle hGroup, const DataBindingInstanceHandle hInstance)
  {
    assert(!TryGetGroup(hInstance).IsValid());
    m_instanceToGroupMap.emplace(hInstance.Value, hGroup);
  }


  bool TwoWayDataBindingGroupManager::TryAddToGroup(const DataBindingGroupInstanceHandle hGroup, const DataBindingInstanceHandle hInstance)
  {
    if (!TryGetGroup(hInstance).IsValid())
    {
      m_instanceToGroupMap.emplace(hInstance.Value, hGroup);
      return true;
    }
    return false;
  }


  void TwoWayDataBindingGroupManager::ClearGroups()
  {
    m_groups.Clear();
    m_instanceToGroupMap.clear();
  }
}
