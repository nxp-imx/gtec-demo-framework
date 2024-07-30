#ifndef FSLNATIVEWINDOW_BASE_VIRTUALKEY_HPP
#define FSLNATIVEWINDOW_BASE_VIRTUALKEY_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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


namespace Fsl
{
  struct VirtualKey
  {
    enum Enum
    {
      Undefined = 0,

      Escape = 0x001B,
      Tab = 0x0009,
      Return = 0x000A,
      // Enter = 0x001D,
      Space = 0x0020,
      Backspace = 0x007F,

      Code0 = '0',
      Code1 = '1',
      Code2 = '2',
      Code3 = '3',
      Code4 = '4',
      Code5 = '5',
      Code6 = '6',
      Code7 = '7',
      Code8 = '8',
      Code9 = '9',

      A = 'A',
      B = 'B',
      C = 'C',
      D = 'D',
      E = 'E',
      F = 'F',
      G = 'G',
      H = 'H',
      I = 'I',
      J = 'J',
      K = 'K',
      L = 'L',
      M = 'M',
      N = 'N',
      O = 'O',
      P = 'P',
      Q = 'Q',
      R = 'R',
      S = 'S',
      T = 'T',
      U = 'U',
      V = 'V',
      W = 'W',
      X = 'X',
      Y = 'Y',
      Z = 'Z',

      LeftShift = 0x8000,
      RightShift = 0x8001,

      // Game pad mappings
      GamePadButtonA = 0xF000,
      GamePadButtonB = 0xF001,
      GamePadButtonC = 0xF002,
      GamePadButtonX = 0xF003,
      GamePadButtonY = 0xF004,
      GamePadButtonZ = 0xF005,
      GamePadButtonLeftShoulder = 0xF006,
      GamePadButtonRightShoulder = 0xF007,
      GamePadButtonLeftShoulder2 = 0xF008,
      GamePadButtonRightShoulder2 = 0xF009,
      GamePadButtonLeftThumb = 0xF00A,
      GamePadButtonRightThumb = 0xF00B,
      GamePadButtonStart = 0xF00C,
      GamePadButtonSelect = 0xF00D,
      GamePadButtonMode = 0xF00E,
      GamePadButtonBack = 0xF00F,
      GamePadDpadUp = 0xF010,
      GamePadDpadDown = 0xF011,
      GamePadDpadLeft = 0xF012,
      GamePadDpadRight = 0xF013,
      GamePadDpadCenter = 0xF014,

      // NOLINTNEXTLINE(readability-identifier-naming)
      SYS_GAMEPAD_RANGE_FIRST = GamePadButtonA,
      // NOLINTNEXTLINE(readability-identifier-naming)
      SYS_GAMEPAD_RANGE_LAST = GamePadDpadCenter,

      UpArrow = 0xF700,
      DownArrow = 0xF701,
      LeftArrow = 0xF702,
      RightArrow = 0xF703,

      F1 = 0xF704,
      F2 = 0xF705,
      F3 = 0xF706,
      F4 = 0xF707,
      F5 = 0xF708,
      F6 = 0xF709,
      F7 = 0xF70A,
      F8 = 0xF70B,
      F9 = 0xF70C,
      F10 = 0xF70D,
      F11 = 0xF70E,
      F12 = 0xF70F,

      Insert = 0xF727,
      Delete = 0xF728,
      Home = 0xF729,
      End = 0xF72B,
      PageUp = 0xF72C,
      PageDown = 0xF72D,

      PrintScreen = 0xF72E,
      ScrollLock = 0xF72F,
      Pause = 0xF730,
      Break = 0xF732,

      Add = 0xF740,
      Subtract = 0xF741,

      Unknown = 0xABCD,
    };
  };
}

#endif
