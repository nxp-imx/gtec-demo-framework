#ifndef FSLGRAPHICS_PIXELCHANNELORDER_HPP
#define FSLGRAPHICS_PIXELCHANNELORDER_HPP
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
  //! Describes the preferred ordering of color channels
  enum class PixelChannelOrder
  {
    //! Not defined
    Undefined = 0,
    //! Prefer channels defined in the order R, G, B, A.
    //! If a channel isn't available its just removed from the above order list. So BGR would become RGB.
    RGBA = 1,
    //! Prefer channels defined in the order B, G, R, A
    //! If a channel isn't available its just removed from the above order list. So RGB would become BGR.
    BGRA = 2,
    //! Prefer channels defined in the order A, R, G, B
    //! If a channel isn't available its just removed from the above order list. So BRA would become ARB.
    ARGB = 3,
    //! Prefer channels defined in the order A, B, G, R
    //! If a channel isn't available its just removed from the above order list. So ARB would become BRA.
    ABGR = 4
  };
}

#endif
