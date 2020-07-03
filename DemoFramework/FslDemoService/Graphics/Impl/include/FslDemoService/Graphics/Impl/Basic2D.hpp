#ifndef FSLDEMOSERVICE_GRAPHICS_IMPL_BASIC2D_HPP
#define FSLDEMOSERVICE_GRAPHICS_IMPL_BASIC2D_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Color.hpp>
#include <FslDemoService/Graphics/IBasic2D.hpp>
#include <memory>
#include <string>

namespace Fsl
{
  class INativeGraphicsBasic2D;

  //! @brief This represents the minimal set of drawing operations that are supported on all hardware.
  //! @note  This is useful for rendering basic debug output. Since this is designed for basic debugging its also very
  //         lenient with its input parameters. Most wrong parameters just produce no output and a log message.
  class Basic2D final : public IBasic2D
  {
    std::shared_ptr<INativeGraphicsBasic2D> m_native;
    PxSize2D m_fontSize;
    bool m_inBegin;

  public:
    explicit Basic2D(std::shared_ptr<INativeGraphicsBasic2D> native);

    //! @brief Begin the draw sequence
    //! @note all draw operations must occur between a begin and end call.
    void Begin() final;
    void End() final;
    void DrawPoints(const Vector2* const pSrc, const int32_t length, const Color& color) final;
    void DrawString(const char* const psz, const Vector2& dstPositionPxf) final;
    void DrawString(const std::string& str, const Vector2& dstPositionPxf) final;
    void DrawString(const StringViewLite& strView, const Vector2& dstPositionPxf) final;
    void DrawString(const char* const psz, const PxPoint2& dstPositionPx) final;
    void DrawString(const std::string& str, const PxPoint2& dstPositionPx) final;
    void DrawString(const StringViewLite& strView, const PxPoint2& dstPositionPx) final;
    PxSize2D FontSize() const final;
  };
}

#endif
