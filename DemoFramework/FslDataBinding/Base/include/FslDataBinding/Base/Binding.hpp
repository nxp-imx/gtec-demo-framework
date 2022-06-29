#ifndef FSLDATABINDING_BASE_BINDING_HPP
#define FSLDATABINDING_BASE_BINDING_HPP
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

#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslDataBinding/Base/Bind/IComplexBinding.hpp>
#include <FslDataBinding/Base/Bind/IMultiBinding.hpp>
#include <FslDataBinding/Base/Bind/ISingleBinding.hpp>
#include <FslDataBinding/Base/DataBindingInstanceHandle.hpp>
#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace Fsl::DataBinding
{
  class Binding final
  {
    DataBindingInstanceHandle m_hPrimarySource;
    std::shared_ptr<IComplexBinding> m_complexBinding;
    std::vector<DataBindingInstanceHandle> m_multiSource;

  public:
    explicit Binding(const DataBindingInstanceHandle hSource)
      : m_hPrimarySource(hSource)
    {
    }

    explicit Binding(std::shared_ptr<ISingleBinding> singleBinding, const DataBindingInstanceHandle hSource)
      : m_hPrimarySource(hSource)
      , m_complexBinding(std::move(singleBinding))
    {
    }

    template <class... THandles, std::enable_if_t<((sizeof...(THandles)) > 0u)>* = nullptr>
    explicit Binding(std::shared_ptr<IMultiBinding> multiBinding, const DataBindingInstanceHandle hSource, THandles... sourceHandles)
      : m_hPrimarySource(hSource)
      , m_complexBinding(std::move(multiBinding))
      , m_multiSource{hSource, (sourceHandles)...}
    {
      // we expect the array to be of the right size
      assert(m_multiSource.size() == (1u + sizeof...(THandles)));
    }


    bool HasValidSourceHandles() const noexcept;

    inline const std::shared_ptr<IComplexBinding>& ComplexBinding() const noexcept
    {
      return m_complexBinding;
    }

    inline ReadOnlySpan<DataBindingInstanceHandle> SourceHandlesAsSpan() const noexcept
    {
      return m_multiSource.empty() ? ReadOnlySpan<DataBindingInstanceHandle>(&m_hPrimarySource, 1u, OptimizationCheckFlag::NoCheck)
                                   : ReadOnlySpanUtil::AsSpan(m_multiSource);
    }

    bool ContainsSource(const DataBindingInstanceHandle handle) const noexcept;
  };
}

#endif
