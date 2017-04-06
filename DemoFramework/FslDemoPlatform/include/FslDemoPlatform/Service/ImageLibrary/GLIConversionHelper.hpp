#ifndef FSLDEMOPLATFORM_SERVICE_IMAGELIBRARY_GLICONVERSIONHELPER_HPP
#define FSLDEMOPLATFORM_SERVICE_IMAGELIBRARY_GLICONVERSIONHELPER_HPP
#ifdef FSL_FEATURE_GLI
/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Texture/TextureType.hpp>
#include <gli/format.hpp>
#include <gli/target.hpp>

namespace Fsl
{
  class GLIConversionHelper
  {
  public:
    //! @brief Try to convert the pixel format
    //! @returns the pixel format or FORMAT_UNDEFINED if unable to convert
    static gli::format TryConvert(const PixelFormat format);

    //! @brief Try to convert the pixel format
    //! @returns the pixel format or PixelFormat::Undefined if unable to convert
    static PixelFormat TryConvert(const gli::format format);

    //! @brief Try to convert the texture type
    //! @param textureType to convert
    //! @param the resulting target
    //! @returns true if the target was converted successfully
    static bool TryConvert(const TextureType textureType, gli::target& rResult);

    //! @brief Try to convert the texture type
    //! @returns the TextureType or TextureType::Undefined in unable to convert
    static TextureType TryConvert(const gli::target target);
  };
}
#endif
#endif
