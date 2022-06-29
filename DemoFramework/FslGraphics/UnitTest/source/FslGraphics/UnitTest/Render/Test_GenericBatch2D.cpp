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

#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslGraphics/Render/GenericBatch2D.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <memory>

using namespace Fsl;

namespace
{
  using TestRender_GenericBatch2D = TestFixtureFslGraphics;

  struct DummyTextureInfo
  {
    PxExtent3D Extent;

    bool IsValid() const
    {
      return true;
    }

    bool operator==(const DummyTextureInfo& rhs) const
    {
      return Extent == rhs.Extent;
    }

    bool operator!=(const DummyTextureInfo& rhs) const
    {
      return !(*this == rhs);
    }
  };

  class DummyQuadBatch
  {
  public:
    void Begin(const PxSize2D& /*sizePx*/, const BlendState /*blendState*/, const BatchSdfRenderConfig& /*sdfRenderConfig*/,
               const bool /*restoreState*/)
    {
    }
    void DrawQuads(const VertexPositionColorTexture* const /*pVertices*/, const uint32_t /*length*/, const DummyTextureInfo& /*textureInfo*/)
    {
    }
    void End()
    {
    }
  };
}


TEST(TestRender_GenericBatch2D, Construct)
{
  const auto quadRenderer = std::make_shared<DummyQuadBatch>();
  const PxExtent2D currentExtent(1024, 768);

  GenericBatch2D<std::shared_ptr<DummyQuadBatch>, DummyTextureInfo, GenericBatch2DFormat::Normal> dummy(quadRenderer, currentExtent);
}

TEST(TestRender_GenericBatch2D, BeginEnd)
{
  const auto quadRenderer = std::make_shared<DummyQuadBatch>();
  const PxExtent2D currentExtent(1024, 768);

  GenericBatch2D<std::shared_ptr<DummyQuadBatch>, DummyTextureInfo, GenericBatch2DFormat::Normal> dummy(quadRenderer, currentExtent);
  dummy.Begin();
  dummy.End();
}

TEST(TestRender_GenericBatch2D, BeginDrawEnd)
{
  const auto quadRenderer = std::make_shared<DummyQuadBatch>();
  const PxExtent2D currentExtent(1024, 768);

  GenericBatch2D<std::shared_ptr<DummyQuadBatch>, DummyTextureInfo, GenericBatch2DFormat::Normal> dummy(quadRenderer, currentExtent);
  dummy.Begin();
  dummy.Draw(DummyTextureInfo(), PxAreaRectangleF(0, 10, 20, 30), Color::White());
  dummy.End();
}
