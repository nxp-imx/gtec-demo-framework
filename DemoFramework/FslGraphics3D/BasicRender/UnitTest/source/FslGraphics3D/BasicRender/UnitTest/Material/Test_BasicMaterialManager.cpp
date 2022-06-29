/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/UnitTest/Helper/Render/NativeTextureTestImpl.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics3D/BasicRender/Material/BasicMaterialManager.hpp>
#include <FslGraphics3D/BasicRender/Shader/BasicShaderManager.hpp>
#include <memory>
#include "NativeMaterialTestFactory.hpp"
#include "NativeShaderTestFactory.hpp"
#include "NativeTexture2DTest.hpp"

using namespace Fsl;

namespace
{
  namespace LocalConfig
  {
    constexpr uint32_t MaxFramesInFlight = 2;
    constexpr uint32_t DeferCount = MaxFramesInFlight;

    constexpr VertexDeclarationArray<3> VertexDecl = VertexPositionColorTexture::GetVertexDeclarationArray();

    static_assert(LocalConfig::MaxFramesInFlight > 0, "MaxFramesInFlight must be > 0");
  }

  class Test_BasicMaterialManager : public TestFixtureFslGraphics
  {
  public:
    std::shared_ptr<NativeShaderTestFactory> m_testShaderFactory;
    std::shared_ptr<NativeMaterialTestFactory> m_testFactory;
    Graphics3D::BasicShaderManager m_basicShaderManager;
    Graphics3D::BasicMaterialManager m_manager;

    Test_BasicMaterialManager()
      : m_testShaderFactory(std::make_shared<NativeShaderTestFactory>())
      , m_testFactory(std::make_shared<NativeMaterialTestFactory>())
      , m_basicShaderManager(m_testShaderFactory)
      , m_manager(LocalConfig::MaxFramesInFlight, m_testFactory, m_basicShaderManager)
    {
    }
  };
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, Construct_Default)
{
  EXPECT_EQ(0u, m_manager.MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, CreateDependentResources_Empty_NoDestroy)
{
  m_manager.CreateDependentResources();
  EXPECT_EQ(0u, m_manager.MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateDependentResources_Empty)
{
  m_manager.CreateDependentResources();
  m_manager.DestroyDependentResources();
  EXPECT_EQ(0u, m_manager.MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, OnRenderEvent_Empty_SwapchainLost)
{
  m_manager.CreateDependentResources();
  m_manager.OnRenderSystemEvent(BasicRenderSystemEvent::SwapchainLost);
  m_manager.DestroyDependentResources();
}

TEST_F(Test_BasicMaterialManager, OnRenderEvent_Empty_SwapchainRecreated)
{
  m_manager.CreateDependentResources();
  m_manager.OnRenderSystemEvent(BasicRenderSystemEvent::SwapchainLost);
  m_manager.OnRenderSystemEvent(BasicRenderSystemEvent::SwapchainRecreated);
  m_manager.DestroyDependentResources();
}

TEST_F(Test_BasicMaterialManager, OnRenderEvent_Empty_SwapchainRecreated_NoLoss)
{
  m_manager.CreateDependentResources();
  m_manager.OnRenderSystemEvent(BasicRenderSystemEvent::SwapchainRecreated);
  m_manager.DestroyDependentResources();
}

TEST_F(Test_BasicMaterialManager, OnRenderEvent_Empty_NoDepedentResources_SwapchainLost)
{
  m_manager.OnRenderSystemEvent(BasicRenderSystemEvent::SwapchainLost);
}

TEST_F(Test_BasicMaterialManager, OnRenderEvent_Empty_NoDepedentResources_SwapchainRecreated)
{
  m_manager.OnRenderSystemEvent(BasicRenderSystemEvent::SwapchainRecreated);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, CreateMaterial_InvalidCreateInfo)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, VertexDeclarationSpan());
  EXPECT_THROW(m_manager.CreateMaterial(createInfo, texture, false), std::invalid_argument);
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_NullTexture)
{
  std::shared_ptr<INativeTexture2D> texture;
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  EXPECT_THROW(m_manager.CreateMaterial(createInfo, texture, false), std::invalid_argument);
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_2x)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  std::shared_ptr<INativeTexture2D> texture1 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(2));
  BasicMaterialCreateInfo createInfo0(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterialCreateInfo createInfo1(BlendState::AlphaBlend, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo0, texture0, false);
  BasicMaterial mat1 = m_manager.CreateMaterial(createInfo1, texture1, false);
  EXPECT_EQ(2u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_2x_SameCreateInfo)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  std::shared_ptr<INativeTexture2D> texture1 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(2));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);
  BasicMaterial mat1 = m_manager.CreateMaterial(createInfo, texture1, false);
  EXPECT_EQ(2u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_2x_SameCreateInfo_SameTexture)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);
  BasicMaterial mat1 = m_manager.CreateMaterial(createInfo, texture0, false);

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_2x_SameCreateInfo_SameTexture_ExpiredTracker)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  {    // let the tracker expire
    BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);
  }
  BasicMaterial mat1 = m_manager.CreateMaterial(createInfo, texture0, false);
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, CloneMaterial)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  std::shared_ptr<INativeTexture2D> texture1 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(2));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);
  BasicMaterial mat1 = m_manager.CloneMaterial(mat0, texture1, false);
  EXPECT_EQ(2u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, PreUpdate_Empty)
{
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.PreUpdate();
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}


TEST_F(Test_BasicMaterialManager, PreUpdate_2x_Empty)
{
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.PreUpdate();
  m_manager.PreUpdate();
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, PreUpdate_Mat1)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.PreUpdate();
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}


TEST_F(Test_BasicMaterialManager, PreUpdate_2x_Mat1)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.PreUpdate();
  m_manager.PreUpdate();
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST_F(Test_BasicMaterialManager, PreUpdate_Mat1_Destroyed)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  {
    BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);
  }

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.PreUpdate();
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, PreUpdate_Mat1_Destroyed2)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.PreUpdate();
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  mat0 = {};
  m_manager.PreUpdate();
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, PreUpdate_Empty_WithDependentResources)
{
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(0u, m_manager.MaterialCount());
    EXPECT_EQ(0u, m_testFactory->MaterialCount());
    m_manager.PreUpdate();
    EXPECT_EQ(0u, m_manager.MaterialCount());
    EXPECT_EQ(0u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}


TEST_F(Test_BasicMaterialManager, PreUpdate_2x_Empty_WithDependentResources)
{
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(0u, m_manager.MaterialCount());
    EXPECT_EQ(0u, m_testFactory->MaterialCount());
    m_manager.PreUpdate();
    EXPECT_EQ(0u, m_manager.MaterialCount());
    EXPECT_EQ(0u, m_testFactory->MaterialCount());
    m_manager.PreUpdate();
    EXPECT_EQ(0u, m_manager.MaterialCount());
    EXPECT_EQ(0u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, PreUpdate_Mat1_WithDependentResources)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);

  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
    m_manager.PreUpdate();
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}


TEST_F(Test_BasicMaterialManager, PreUpdate_2x_Mat1_WithDependentResources)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);

  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
    m_manager.PreUpdate();
    m_manager.PreUpdate();
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST_F(Test_BasicMaterialManager, PreUpdate_Mat1_Destroyed_WithDependentResources)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  {
    BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);
  }

  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  EXPECT_EQ(1u, m_manager.MaterialCount());

  // Since the material has already been marked as destroyed, the CreateDependent will do the GC
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(0u, m_manager.MaterialCount());
    EXPECT_EQ(0u, m_testFactory->MaterialCount());
    m_manager.PreUpdate();
    EXPECT_EQ(0u, m_manager.MaterialCount());
    EXPECT_EQ(0u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, PreUpdate_Mat1_Destroyed2_WithDependentResources)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);

  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  EXPECT_EQ(1u, m_manager.MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
    m_manager.PreUpdate();
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());

    // The native material will be destroyed once DeferCount PreUpdate's has occurred
    mat0 = {};
    for (uint32_t i = 0; i < LocalConfig::DeferCount; ++i)
    {
      m_manager.PreUpdate();
      EXPECT_EQ(0u, m_manager.MaterialCount());
      EXPECT_EQ(1u, m_testFactory->MaterialCount());
    }

    // Now the destroyed material will actually be destroyed
    m_manager.PreUpdate();
    EXPECT_EQ(0u, m_manager.MaterialCount());
    EXPECT_EQ(0u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, CreateMaterial_ThenCreateDependent)
{
  std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_2x_ThenCreateDependent)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  std::shared_ptr<INativeTexture2D> texture1 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(2));
  BasicMaterialCreateInfo createInfo0(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterialCreateInfo createInfo1(BlendState::AlphaBlend, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo0, texture0, false);
  BasicMaterial mat1 = m_manager.CreateMaterial(createInfo1, texture1, false);

  EXPECT_EQ(2u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(2u, m_manager.MaterialCount());
    EXPECT_EQ(2u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(2u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_2x_SameCreateInfo_ThenCreateDependent)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  std::shared_ptr<INativeTexture2D> texture1 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(2));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);
  BasicMaterial mat1 = m_manager.CreateMaterial(createInfo, texture1, false);

  EXPECT_EQ(2u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(2u, m_manager.MaterialCount());
    EXPECT_EQ(2u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(2u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_2x_SameCreateInfo_SameTexture_ThenCreateDependent)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);
  BasicMaterial mat1 = m_manager.CreateMaterial(createInfo, texture0, false);

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

TEST_F(Test_BasicMaterialManager, CreateMaterial_2x_SameCreateInfo_SameTexture_ExpiredTracker_ThenCreateDependent)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

  {    // let the tracker expire
    BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);
  }
  BasicMaterial mat1 = m_manager.CreateMaterial(createInfo, texture0, false);

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, CreateMaterial_DependentResources)
{
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  BasicMaterial mat0;
  m_manager.CreateDependentResources();
  {
    std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
    BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

    mat0 = m_manager.CreateMaterial(createInfo, texture, false);
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}


TEST_F(Test_BasicMaterialManager, CreateMaterial_DependentResources_MatDestroyedOnDependentDestroy)
{
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    std::shared_ptr<INativeTexture2D> texture = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
    BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());

    BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture, false);
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(0u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, CloneMaterial_DependentResources)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  std::shared_ptr<INativeTexture2D> texture1 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(2));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);
  BasicMaterial mat1;

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());

    mat1 = m_manager.CloneMaterial(mat0, texture1, false);
    EXPECT_EQ(2u, m_manager.MaterialCount());
    EXPECT_EQ(2u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(2u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(Test_BasicMaterialManager, CloneMaterial_DependentResources_MatDestroyedOnDependentDestroy)
{
  std::shared_ptr<INativeTexture2D> texture0 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(1));
  BasicMaterialCreateInfo createInfo(BlendState::Opaque, LocalConfig::VertexDecl.AsReadOnlySpan());
  BasicMaterial mat0 = m_manager.CreateMaterial(createInfo, texture0, false);

  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
  m_manager.CreateDependentResources();
  {
    EXPECT_EQ(1u, m_manager.MaterialCount());
    EXPECT_EQ(1u, m_testFactory->MaterialCount());

    std::shared_ptr<INativeTexture2D> texture1 = std::make_shared<NativeTexture2DTest>(BasicNativeTextureHandle(2));

    BasicMaterial mat1 = m_manager.CloneMaterial(mat0, texture1, false);
    EXPECT_EQ(2u, m_manager.MaterialCount());
    EXPECT_EQ(2u, m_testFactory->MaterialCount());
  }
  m_manager.DestroyDependentResources();
  EXPECT_EQ(1u, m_manager.MaterialCount());
  EXPECT_EQ(0u, m_testFactory->MaterialCount());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
