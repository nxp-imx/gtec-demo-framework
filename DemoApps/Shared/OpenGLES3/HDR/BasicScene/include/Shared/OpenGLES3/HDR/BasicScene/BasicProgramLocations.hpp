#ifndef SHARED_OPENGLES3_HDR_BASICSCENE_BASICPROGRAMLOCATIONS_HPP
#define SHARED_OPENGLES3_HDR_BASICSCENE_BASICPROGRAMLOCATIONS_HPP
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

#include <FslUtil/OpenGLES3/GLValues.hpp>

namespace Fsl
{
  struct BasicProgramLocations
  {
    GLint ModelMatrix;
    GLint ViewMatrix;
    GLint ProjMatrix;
    GLint LightPositions;
    GLint LightColors;
    BasicProgramLocations()
      : ModelMatrix(GLES3::GLValues::INVALID_LOCATION)
      , ViewMatrix(GLES3::GLValues::INVALID_LOCATION)
      , ProjMatrix(GLES3::GLValues::INVALID_LOCATION)
      , LightPositions(GLES3::GLValues::INVALID_LOCATION)
      , LightColors(GLES3::GLValues::INVALID_LOCATION)
    {
    }
  };
}

#endif
