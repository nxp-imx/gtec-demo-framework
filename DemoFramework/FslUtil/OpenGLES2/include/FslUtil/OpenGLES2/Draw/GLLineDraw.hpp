#ifndef FSLUTIL_OPENGLES2_DRAW_GLLINEDRAW_HPP
#define FSLUTIL_OPENGLES2_DRAW_GLLINEDRAW_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
#include <FslGraphics/Vertices/VertexSpan.hpp>
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <FslUtil/OpenGLES2/GLVertexBuffer.hpp>
#include <array>
#include <cstddef>
#include <vector>

namespace Fsl::GLES2
{
  //! @brief This is a pure utility class to make it easier to render dynamic lines primarily intended for debugging
  //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
  class GLLineDraw
  {
  public:
    static constexpr const uint32_t VerticesPerLine = 2;
    static constexpr const uint32_t DefaultCapacity = 4096;

    //! Direct access to the line vertex buffer, to be able to query it, but its not intended for modifications
    GLVertexBuffer LineVertexBuffer;

    GLLineDraw()
      : GLLineDraw(DefaultCapacity)
    {
    }

    explicit GLLineDraw(const uint32_t initialLineCapacity);

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @param vertexSpan will be copied to the LineVertexBuffer and then rendered
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    void Draw(const VertexPositionColor* pVertices, const std::size_t vertexCount, const GLVertexAttribLink* const pLinks,
              const std::size_t linkCount);

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    template <std::size_t TSize>
    void Draw(const VertexPositionColor* pVertices, const std::size_t vertexCount, const std::array<GLVertexAttribLink, TSize>& links)
    {
      Draw(pVertices, vertexCount, links.data(), links.size());
    }

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    void Draw(const VertexPositionColor* pVertices, const std::size_t vertexCount, const std::vector<GLVertexAttribLink>& links)
    {
      Draw(pVertices, vertexCount, links.data(), links.size());
    }

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    template <std::size_t TSize1, std::size_t TSize2>
    void Draw(const std::array<VertexPositionColor, TSize1>& vertices, const std::array<GLVertexAttribLink, TSize2>& links)
    {
      Draw(vertices.data(), vertices.size(), links.data(), links.size());
    }

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    template <std::size_t TSize>
    void Draw(const std::array<VertexPositionColor, TSize>& vertices, const std::vector<GLVertexAttribLink>& links)
    {
      Draw(vertices.data(), vertices.size(), links.data(), links.size());
    }

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    template <std::size_t TSize>
    void Draw(const std::vector<VertexPositionColor>& vertices, const std::array<GLVertexAttribLink, TSize>& links)
    {
      Draw(vertices.data(), vertices.size(), links.data(), links.size());
    }

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    void Draw(const std::vector<VertexPositionColor>& vertices, const std::vector<GLVertexAttribLink>& links)
    {
      Draw(vertices.data(), vertices.size(), links.data(), links.size());
    }

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    template <std::size_t TSize>
    void Draw(const VertexSpan<VertexPositionColor>& vertexSpan, const std::array<GLVertexAttribLink, TSize>& links)
    {
      Draw(vertexSpan.pVertices, vertexSpan.VertexCount, links.data(), links.size());
    }

    //! @brief Draw the supplied lines using a dynamic vertex buffer.
    //! @note  It's recommended to only call Draw once per frame and double buffering it could improve performance!
    void Draw(const VertexSpan<VertexPositionColor>& vertexSpan, const std::vector<GLVertexAttribLink>& links)
    {
      Draw(vertexSpan.pVertices, vertexSpan.VertexCount, links.data(), links.size());
    }
  };
}

#endif
