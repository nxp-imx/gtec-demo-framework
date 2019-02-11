#ifndef SHARED_HDR_BASICSCENE_API_OPENGLES3_FRAGMENTUBODATACREATOR_HPP
#define SHARED_HDR_BASICSCENE_API_OPENGLES3_FRAGMENTUBODATACREATOR_HPP
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

#include <Shared/HDR/BasicScene/API/OpenGLES3/FragmentUBOData.hpp>


namespace Fsl
{
  namespace FragmentUBODataCreator
  {
    inline FragmentUBOData PrepareLights()
    {
      FragmentUBOData fragmentUBO;
      // lighting info
      fragmentUBO.LightPositions[0] = Vector3(0.0f, 0.0f, -31.5f);
      fragmentUBO.LightPositions[1] = Vector3(-1.4f, -1.9f, -9.0f);
      fragmentUBO.LightPositions[2] = Vector3(0.0f, -1.8f, -4.0f);
      // fragmentUBO.LightPositions[3] = Vector3(0.0f, 0.0f, 0.0f);
      fragmentUBO.LightPositions[3] = Vector3(0.8f, -1.7f, -6.0f);

      fragmentUBO.LightColors[0] = Vector3(200.0f, 200.0f, 200.0f);
      fragmentUBO.LightColors[1] = Vector3(0.1f, 0.0f, 0.0f);
      fragmentUBO.LightColors[2] = Vector3(0.0f, 0.0f, 0.2f);
      fragmentUBO.LightColors[3] = Vector3(0.0f, 0.1f, 0.0f);
      // fragmentUBO.LightColors[3] = Vector3(221.0f, 1.1f)

      fragmentUBO.Exposure = 1.0f;

      return fragmentUBO;
    }
  };
}

#endif
