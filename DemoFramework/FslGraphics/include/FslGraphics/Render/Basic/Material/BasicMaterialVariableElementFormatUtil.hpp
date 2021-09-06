#ifndef FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLEELEMENTFORMATUTIL_HPP
#define FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLEELEMENTFORMATUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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


#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElementFormat.hpp>

namespace Fsl
{
  class BasicMaterialVariableElementFormatUtil
  {
  public:
    constexpr static uint32_t TryGetBytesPerElement(const BasicMaterialVariableElementFormat format) noexcept
    {
      switch (format)
      {
      case BasicMaterialVariableElementFormat::Undefined:
        return 0;
      case BasicMaterialVariableElementFormat::Float1:
        return sizeof(float) * 1;
      case BasicMaterialVariableElementFormat::Matrix4x4:
        return sizeof(float) * 4 * 4;
      default:
        return -1;
      }
    }

    constexpr static int32_t TryGetElementCount(const BasicMaterialVariableElementFormat format) noexcept
    {
      switch (format)
      {
      case BasicMaterialVariableElementFormat::Undefined:
        return 0;
      case BasicMaterialVariableElementFormat::Float1:
        return 1;
      case BasicMaterialVariableElementFormat::Matrix4x4:
        return 4 * 4;
      default:
        return -1;
      }
    }

    constexpr static uint32_t GetBytesPerElement(const BasicMaterialVariableElementFormat format)
    {
      return ProcessResult(TryGetBytesPerElement(format));
    }

    constexpr static uint32_t GetElementCount(const BasicMaterialVariableElementFormat format)
    {
      return ProcessResult(TryGetElementCount(format));
    }

  private:
    constexpr inline static uint32_t ProcessResult(const int32_t res)
    {
      return res >= 0 ? static_cast<uint32_t>(res) : throw NotSupportedException("Unknown BasicMaterialElementFormat");
    }
  };
}

#endif
