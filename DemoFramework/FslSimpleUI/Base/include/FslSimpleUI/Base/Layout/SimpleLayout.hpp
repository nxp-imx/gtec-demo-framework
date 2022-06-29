#ifndef FSLSIMPLEUI_BASE_LAYOUT_SIMPLELAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_SIMPLELAYOUT_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Base/WindowCollection/GenericWindowCollection.hpp>

namespace Fsl::UI
{
  class SimpleLayout : public Layout
  {
  public:
    using collection_type = GenericWindowCollection<GenericWindowCollectionRecordBase>;

  private:
    collection_type m_children;

  public:
    explicit SimpleLayout(const std::shared_ptr<BaseWindowContext>& context);
    void WinInit() override;

    void ClearChildren() override
    {
      m_children.Clear();
    }

    void AddChild(const std::shared_ptr<BaseWindow>& window) override
    {
      m_children.Add(window);
      window->SYS_SetParentBaseColor(GetFinalBaseColor());
    }

    void RemoveChild(const std::shared_ptr<BaseWindow>& window) override
    {
      m_children.Remove(window);
    }

    std::size_t GetChildCount() const override
    {
      return m_children.size();
    }

    bool ContainsChild(const std::shared_ptr<BaseWindow>& window) const
    {
      return m_children.Contains(window);
    }

  protected:
    void OnPropertiesUpdated(const PropertyTypeFlags& flags) override;

    inline bool empty() const noexcept
    {
      return m_children.empty();
    }

    inline collection_type::queue_type::size_type size() const noexcept
    {
      return m_children.size();
    }

    inline collection_type::queue_type::const_iterator begin() const noexcept
    {
      return m_children.begin();
    }

    inline collection_type::queue_type::const_iterator end() const noexcept
    {
      return m_children.end();
    }
  };
}

#endif
