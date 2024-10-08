#ifndef FSLSIMPLEUI_BASE_CONTROL_BUTTONBASE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_BUTTONBASE_HPP
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

#include <FslSimpleUI/Base/BaseWindow.hpp>

namespace Fsl::UI
{
  //! @brief The basic implementation of a button
  //! @note  This is intended as a base class for simple buttons.
  class ButtonBase : public BaseWindow
  {
    using base_type = BaseWindow;

    bool m_isEnabled{true};
    bool m_isDown{false};

  public:
    enum class ButtonPressState
    {
      Down = 0,
      Up = 1,
      //! The button was released but the 'press' was canceled
      UpCancelled = 0x2 | Up,
    };

    virtual bool IsEnabled() const
    {
      return m_isEnabled;
    }
    virtual void SetEnabled(const bool enabled);

  protected:
    explicit ButtonBase(const std::shared_ptr<BaseWindowContext>& context);
    void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) override;

    //! @brief Check if the button is down at the moment
    bool IsDown() const
    {
      return m_isDown;
    }

    //! @brief Called when the button is pressed
    virtual void Pressed(const ButtonPressState state)
    {
      FSL_PARAM_NOT_USED(state);
    }

  private:
    void CancelButtonDown();
  };
}

#endif
