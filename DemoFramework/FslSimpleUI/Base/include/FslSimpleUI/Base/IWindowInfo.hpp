#ifndef FSLSIMPLEUI_BASE_IWINDOWINFO_HPP
#define FSLSIMPLEUI_BASE_IWINDOWINFO_HPP
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

#include <FslBase/Math/Vector2.hpp>

namespace Fsl
{
  namespace UI
  {
    class IWindowId;

    class IWindowInfo
    {
    public:
      virtual ~IWindowInfo() {};

      //! @brief Convert the point from coordinates relative to the window to screen coordinates.
      //!        If the window is unknown in the window manager Vector2.Zero is returned (and a warning is logged in debug builds)
      //! @param pWindow the window coordinate system to convert from (if window == null, we expect the point to be relative to the root window)
      virtual Vector2 PointToScreen(const IWindowId*const pWindow, const Vector2& point) const = 0;

      //! @brief Convert the point from coordinates relative to the screen to be relative to the supplied window.
      //!        if the window is unknown in the window manager Vector2.Zero is returned (and a warning is logged in debug builds)
      //! @param pWindow the window coordinate system to convert to (if window == null we convert it to be relative to the root window)
      virtual Vector2 PointFromScreen(const IWindowId*const pWindow, const Vector2& point) const = 0;
    };
  }
}

#endif
