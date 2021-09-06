#ifndef FSLSIMPLEUI_BASE_LAYOUT_COMPLEXLAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_COMPLEXLAYOUT_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslSimpleUI/Base/WindowCollection/GenericWindowCollection.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>

namespace Fsl
{
  namespace UI
  {
    template <typename TEntry>
    class ComplexLayout : public Layout
    {
      using RecordType = TEntry;

    public:
      using collection_type = GenericWindowCollection<RecordType>;

    private:
      collection_type m_children;

    public:
      explicit ComplexLayout(const std::shared_ptr<BaseWindowContext>& context)
        : Layout(context)
      {
      }

      void WinInit() override
      {
        Layout::WinInit();

        auto uiContext = GetContext()->TheUIContext.Get();
        m_children.SYS_WinInit(this, uiContext->WindowManager);
      }


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
      void OnPropertiesUpdated(const PropertyTypeFlags& flags) override
      {
        Layout::OnPropertiesUpdated(flags);
        if (flags.IsFlagged(PropertyType::BaseColor) && !m_children.empty())
        {
          for (auto itr = m_children.begin(); itr != m_children.end(); ++itr)
          {
            assert(itr->Window);
            itr->Window->SYS_SetParentBaseColor(GetFinalBaseColor());
          }
        }
      }

      const std::shared_ptr<BaseWindow>& ChildAt(const uint32_t index) const
      {
        return m_children.ChildAt(index);
      }


      inline bool empty() const
      {
        return m_children.empty();
      }

      inline typename collection_type::queue_type::size_type size() const noexcept
      {
        return m_children.size();
      }

      inline typename collection_type::queue_type::const_iterator begin() const noexcept
      {
        return m_children.begin();
      }

      inline typename collection_type::queue_type::iterator begin() noexcept
      {
        return m_children.begin();
      }

      inline typename collection_type::queue_type::const_iterator end() const noexcept
      {
        return m_children.end();
      }

      inline typename collection_type::queue_type::iterator end() noexcept
      {
        return m_children.end();
      }
    };
  }
}

#endif
