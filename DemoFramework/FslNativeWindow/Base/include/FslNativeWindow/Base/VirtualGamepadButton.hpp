#ifndef FSLNATIVEWINDOW_BASE_VIRTUALGAMEPADBUTTON_HPP
#define FSLNATIVEWINDOW_BASE_VIRTUALGAMEPADBUTTON_HPP
/****************************************************************************************************************************************************
* Copyright 2017 NXP
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

namespace Fsl
{
  enum class VirtualGamepadButton
  {
    A              = 0x000001,
    B              = 0x000002,
    C              = 0x000004,
    X              = 0x000008,
    Y              = 0x000010,
    Z              = 0x000020,
    LeftShoulder   = 0x000040,
    RightShoulder  = 0x000080,
    LeftShoulder2  = 0x000100,
    RightShoulder2 = 0x000200,
    LeftThumb      = 0x000400,
    RightThumb     = 0x000800,
    Start          = 0x001000,
    Select         = 0x002000,
    Mode           = 0x004000,
    Back           = 0x008000,
    DpadUp         = 0x010000,
    DpadDown       = 0x020000,
    DpadLeft       = 0x040000,
    DpadRight      = 0x080000,
    DpadCenter     = 0x100000,
  };
}

#endif
