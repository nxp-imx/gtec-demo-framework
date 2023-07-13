#ifndef FSLGRAPHICS_UNITTEST_HELPER_RENDER_DYNAMICNATIVETEXTURETESTIMPL_HPP
#define FSLGRAPHICS_UNITTEST_HELPER_RENDER_DYNAMICNATIVETEXTURETESTIMPL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <cassert>
#include <vector>

namespace Fsl
{
  class DynamicNativeTextureTestImpl : public IDynamicNativeTexture2D
  {
    std::vector<uint8_t> m_content;
    PxExtent3D m_extentPx;

  public:
    DynamicNativeTextureTestImpl(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
      : m_extentPx(texture.GetExtent())
    {
      SetData(texture, filterHint, textureFlags);
    }

    BasicNativeTextureHandle TryGetNativeHandle() const noexcept override
    {
      return {};
    }

    NativeTextureArea CalcNativeTextureArea(const PxRectangleU32& imageRectanglePx) const override
    {
      assert(static_cast<float>(m_extentPx.Width.Value) > 0.0f);
      assert(static_cast<float>(m_extentPx.Height.Value) > 0.0f);
      return {imageRectanglePx.RawLeft() == 0 ? 0.0f : static_cast<float>(imageRectanglePx.RawLeft()) / static_cast<float>(m_extentPx.Width.Value),
              imageRectanglePx.RawTop() == 0 ? 0.0f : static_cast<float>(imageRectanglePx.RawTop()) / static_cast<float>(m_extentPx.Height.Value),
              imageRectanglePx.Right() >= m_extentPx.Width
                ? 1.0f
                : static_cast<float>(imageRectanglePx.RawRight()) / static_cast<float>(m_extentPx.Width.Value),
              imageRectanglePx.Bottom() >= m_extentPx.Height
                ? 1.0f
                : static_cast<float>(imageRectanglePx.RawBottom()) / static_cast<float>(m_extentPx.Height.Value)};
    }

    //! @brief Set the data of the texture
    void SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags) override
    {
      FSL_PARAM_NOT_USED(filterHint);
      FSL_PARAM_NOT_USED(textureFlags);

      m_content.clear();
      const auto* const pSrc = static_cast<const uint8_t*>(texture.GetContent());
      assert(pSrc != nullptr);
      m_content.insert(m_content.begin(), pSrc, pSrc + texture.GetByteSize());
    }
  };
}

#endif
