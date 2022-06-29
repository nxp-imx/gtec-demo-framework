#ifndef FSLUTIL_OPENGLES2_GLCOMPRESSEDFORMAT_HPP
#define FSLUTIL_OPENGLES2_GLCOMPRESSEDFORMAT_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

namespace Fsl::GLES2
{
  // Custom formats supported by some GPU's under OpenGL ES2.
  enum class GLCompressedFormat
  {
    R11_EAC = 0x9270,
    SIGNED_R11_EAC = 0x9271,
    RG11_EAC = 0x9272,
    SIGNED_RG11_EAC = 0x9273,

    RGB8_ETC2 = 0x9274,
    SRGB8_ETC2 = 0x9275,
    RGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9276,
    SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9277,
    RGBA8_ETC2_EAC = 0x9278,
    SRGB8_ALPHA8_ETC2_EAC = 0x9279
  };
}

#endif
