#ifndef FSLSIMPLEUI_BASE_LAYOUT_CANVASLAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_CANVASLAYOUT_HPP
/****************************************************************************************************************************************************
* Copyright 2018 NXP
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

#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Base/WindowCollection/GenericWindowCollection.hpp>

namespace Fsl
{
  namespace UI
  {
    class CanvasLayout : public Layout
    {
      struct WindowRecord : GenericWindowCollectionRecordBase
      {
        Vector2 Position;

        WindowRecord(const std::shared_ptr<BaseWindow>& window)
          : GenericWindowCollectionRecordBase(window)
          , Position()
        {
        }
      };

      using collection_type = GenericWindowCollection<WindowRecord>;
      collection_type m_children;
    public:
      CanvasLayout(const std::shared_ptr<WindowContext>& context);
      virtual void WinInit() override;

      virtual void ClearChildren() override
      {
        m_children.Clear();
      }

      virtual void AddChild(const std::shared_ptr<BaseWindow>& window) override
      {
        m_children.Add(window);
      }

      virtual void RemoveChild(const std::shared_ptr<BaseWindow>& window) override
      {
        m_children.Remove(window);
      }

      void SetChildPosition(const std::shared_ptr<BaseWindow>& window, const Vector2& position);

    protected:
      virtual Vector2 ArrangeOverride(const Vector2& finalSize) override;
      virtual Vector2 MeasureOverride(const Vector2& availableSize) override;
    };
  }
}

#endif
