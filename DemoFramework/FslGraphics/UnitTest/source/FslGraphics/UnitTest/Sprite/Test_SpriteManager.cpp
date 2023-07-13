/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/SpriteDpConfig.hpp>
#include <FslGraphics/Sprite/SpriteManager.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <FslGraphics/UnitTest/Helper/Sprite/Material/Test/SpriteMaterialImpl.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  class DummyUIMaterial final : public ISprite
  {
  public:
    SpriteMaterialInfo TestMaterialInfo;
    uint32_t TestDensityDpi{SpriteDpConfig::BaseDpi};

    void Resize(const uint32_t densityDpi) final
    {
      TestDensityDpi = densityDpi;
    }

    uint32_t GetMaterialCount() const final
    {
      return 1u;
    }

    const SpriteMaterialInfo& GetMaterialInfo(const uint32_t index) const final
    {
      FSL_PARAM_NOT_USED(index);
      return TestMaterialInfo;
    }
  };

  using TestSprite_SpriteManager = TestFixtureFslGraphics;
}


TEST(TestSprite_SpriteManager, Construct_Default)
{
  SpriteManager manager(160, false);
  EXPECT_EQ(0u, manager.Count());
}


TEST(TestSprite_SpriteManager, Add)
{
  auto material = std::make_shared<DummyUIMaterial>();

  SpriteManager manager(160, false);
  manager.Add(material);

  EXPECT_EQ(1u, manager.Count());
  EXPECT_TRUE(manager.Contains(material));
}


TEST(TestSprite_SpriteManager, AddRemove)
{
  auto material = std::make_shared<DummyUIMaterial>();

  SpriteManager manager(160, false);
  manager.Add(material);
  EXPECT_EQ(1u, manager.Count());
  EXPECT_TRUE(manager.Contains(material));
  manager.Remove(material);
  EXPECT_EQ(0u, manager.Count());
}


TEST(TestSprite_SpriteManager, Resize)
{
  auto material0 = std::make_shared<DummyUIMaterial>();
  auto material1 = std::make_shared<DummyUIMaterial>();
  auto material2 = std::make_shared<DummyUIMaterial>();

  SpriteManager manager(160, false);
  manager.Add(material0);
  manager.Add(material1);
  manager.Add(material2);
  EXPECT_EQ(3u, manager.Count());

  EXPECT_EQ(SpriteDpConfig::BaseDpi, material0->TestDensityDpi);
  EXPECT_EQ(SpriteDpConfig::BaseDpi, material0->TestDensityDpi);
  EXPECT_EQ(SpriteDpConfig::BaseDpi, material0->TestDensityDpi);

  constexpr uint32_t newDensityDpi = SpriteDpConfig::BaseDpi * 2;

  manager.Resize(newDensityDpi);

  EXPECT_EQ(newDensityDpi, material0->TestDensityDpi);
  EXPECT_EQ(newDensityDpi, material0->TestDensityDpi);
  EXPECT_EQ(newDensityDpi, material0->TestDensityDpi);
}


TEST(TestSprite_SpriteManager, AddBasicImageSprite_InvalidRenderMaterial)
{
  SpriteManager manager(160, false);

  constexpr AtlasTextureInfo textureInfo;
  EXPECT_THROW(manager.AddBasicImageSprite(SpriteMaterialInfo(), textureInfo, IO::PathView("test")), std::invalid_argument);
}


TEST(TestSprite_SpriteManager, AddBasicImageSprite_InvalidTrim)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 30, 40), PxThicknessU::Create(1, 2, 3, 4), SpriteDpConfig::BaseDpi);

  EXPECT_THROW(manager.AddBasicImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test")), UsageErrorException);
}


TEST(TestSprite_SpriteManager, AddBasicImageSprite_InvalidDpi)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 30, 40), PxThicknessU(), 0);

  EXPECT_THROW(manager.AddBasicImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test")), std::invalid_argument);
}

TEST(TestSprite_SpriteManager, AddBasicImageSprite_WidthOutOfBounds)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 512 - 10 + 1, 40), PxThicknessU(), SpriteDpConfig::BaseDpi);

  EXPECT_THROW(manager.AddBasicImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test")), std::invalid_argument);
}

TEST(TestSprite_SpriteManager, AddBasicImageSprite_HeightOutOfBounds)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 30, 1024 - 20 + 1), PxThicknessU(), SpriteDpConfig::BaseDpi);

  EXPECT_THROW(manager.AddBasicImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test")), std::invalid_argument);
}


TEST(TestSprite_SpriteManager, AddBasicImageSprite)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 30, 40), PxThicknessU(), SpriteDpConfig::BaseDpi);

  auto value = manager.AddBasicImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test"));
  EXPECT_EQ(1u, manager.Count());
  EXPECT_TRUE(manager.Contains(value));
}


TEST(TestSprite_SpriteManager, AddImageSprite_InvalidRenderMaterial)
{
  SpriteManager manager(160, false);

  constexpr AtlasTextureInfo textureInfo;
  EXPECT_THROW(manager.AddImageSprite(SpriteMaterialInfo(), textureInfo, IO::PathView("test")), std::invalid_argument);
}


TEST(TestSprite_SpriteManager, AddImageSprite_InvalidDpi)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 30, 40), PxThicknessU::Create(1, 2, 3, 4), 0);

  EXPECT_THROW(manager.AddImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test")), std::invalid_argument);
}


TEST(TestSprite_SpriteManager, AddImageSprite_WidthOutOfBounds)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 512 - 10 + 1, 40), PxThicknessU::Create(1, 2, 3, 4), SpriteDpConfig::BaseDpi);

  EXPECT_THROW(manager.AddImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test")), std::invalid_argument);
}


TEST(TestSprite_SpriteManager, AddImageSprite_HeightOutOfBounds)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 30, 1024 - 20 + 1), PxThicknessU::Create(1, 2, 3, 4),
                                         SpriteDpConfig::BaseDpi);

  EXPECT_THROW(manager.AddImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test")), std::invalid_argument);
}


TEST(TestSprite_SpriteManager, AddImageSprite)
{
  SpriteManager manager(160, false);

  constexpr SpriteMaterialId spriteMaterialId(1);
  constexpr auto textureExtent = PxExtent2D::Create(512, 1024);
  auto renderMaterial = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, true, renderMaterial);
  constexpr AtlasTextureInfo textureInfo(PxRectangleU32::Create(10, 20, 30, 40), PxThicknessU::Create(1, 2, 3, 4), SpriteDpConfig::BaseDpi);

  auto value = manager.AddImageSprite(spriteMaterialInfo, textureInfo, IO::PathView("test"));
  EXPECT_EQ(1u, manager.Count());
  EXPECT_TRUE(manager.Contains(value));
}
