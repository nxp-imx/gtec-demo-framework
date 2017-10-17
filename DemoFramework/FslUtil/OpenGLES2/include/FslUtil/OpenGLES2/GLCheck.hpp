#ifndef FSLUTIL_OPENGLES2_GLCHECK_HPP
#define FSLUTIL_OPENGLES2_GLCHECK_HPP
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
#include <FslUtil/OpenGLES2/Common.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <sstream>

#define GL_CHECK_FOR_ERROR() \
  { \
    GLenum rESULT = glGetError(); \
    if (rESULT != GL_NO_ERROR) \
    { \
      std::stringstream sTREAM; \
      sTREAM << "Failed with error code " << rESULT << " at " << __FILE__ << "(" << __LINE__ << ")"; \
      throw Fsl::GLES2::GLESGraphicsException(sTREAM.str(), rESULT, __FILE__, __LINE__); \
    } \
  }


// Call method X and then check glGetError to see if a error occurred and if it did then throw a Fsl::GLESGraphicsException
// TODO: implement a better version of this macro as it can give unexpected behavior
#define GL_CHECK(X) \
  X; \
  { \
    GLenum rESULT = glGetError(); \
    if (rESULT != GL_NO_ERROR) \
    { \
      std::stringstream sTREAM; \
      sTREAM << #X << " failed with error code " << rESULT << " at " << __FILE__ << "(" << __LINE__ << ")"; \
      throw Fsl::GLES2::GLESGraphicsException(sTREAM.str(), rESULT, __FILE__, __LINE__); \
    } \
  }


// Call method X and then check glGetError to see if a error occurred and if it did then call 'return'
// TODO: implement a better version of this macro as it can give unexpected behavior
#define GL_ON_ERROR_LOG_AND_RETURN(X) \
  X; \
  { \
    GLenum rESULT = glGetError(); \
    if (rESULT != GL_NO_ERROR) \
    { \
      FSLLOG_ERROR(#X << " failed with error code " << rESULT << " at " << __FILE__ << "(" << __LINE__ << ")"); \
      return; \
    } \
   }

// TODO: implement a better version of this macro as it can give unexpected behavior
#define GL_LOG_ERROR(X) \
  X; \
  { \
    GLenum rESULT = glGetError(); \
    if (rESULT != GL_NO_ERROR) \
    { \
      FSLLOG_ERROR(#X << " failed with error code " << rESULT << " at " << __FILE__ << "(" << __LINE__ << ")"); \
    } \
  }


namespace Fsl
{
  namespace GLES2
  {
  }
}

#endif
