#ifndef FSLUTIL_OPENVG_VGCHECK_HPP
#define FSLUTIL_OPENVG_VGCHECK_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/OpenVG/Common.hpp>
#include <FslUtil/OpenVG/Util.hpp>
#include <VG/openvg.h>
#include <sstream>


#define FSLGRAPHICSOPENVG_CHECK(X) Fsl::OpenVG::Util::Check((X), #X, __FILE__, __LINE__)
#define FSLGRAPHICSOPENVG_CHECK2(X, mESSAGE) Fsl::OpenVG::Util::Check((X), (mESSAGE), __FILE__, __LINE__)


// Check vgGetError to see if a error occurred and if it did then throw a Fsl::GraphicsException
// WARNING: Using this macro in a if statement without {} causes the error check part of this macro to be executed anyway.
// It also causes a compile error if trying to use a single line if else statements since it unwraps to two lines not one!
#define FSLGRAPHICSOPENVG_CHECK_FOR_ERROR()                                                            \
  {                                                                                                    \
    VGErrorCode VGeRROR = vgGetError();                                                                \
    if (VGeRROR != VG_NO_ERROR)                                                                        \
    {                                                                                                  \
      throw Fsl::GraphicsException(Fsl::OpenVG::Util::ToNiceMessage("", VGeRROR, __FILE__, __LINE__)); \
    }                                                                                                  \
  }


// Beware that using this macro in a if statement without {} causes the error check part of this macro to be executed anyway
// WARNING: Using this macro in a if statement without {} causes the error check part of this macro to be executed anyway.
// It also causes a compile error if trying to use a single line if else statements since it unwraps to two lines not one!
#define FSLGRAPHICSOPENVG_ON_ERROR_LOG_AND_RETURN(X)                                   \
  {                                                                                    \
    (X);                                                                               \
    VGErrorCode VGeRROR = vgGetError();                                                \
    if (VGeRROR != VG_NO_ERROR)                                                        \
    {                                                                                  \
      FSLLOG_ERROR(Fsl::OpenVG::Util::ToNiceMessage(#X, VGeRROR, __FILE__, __LINE__)); \
      return;                                                                          \
    }                                                                                  \
  }


#endif
