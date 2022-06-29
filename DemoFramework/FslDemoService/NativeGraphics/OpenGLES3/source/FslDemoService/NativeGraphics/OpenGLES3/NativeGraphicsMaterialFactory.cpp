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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsDeviceShaders.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsMaterialFactory.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderCreateInfo.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <utility>

namespace Fsl::GLES3
{
  namespace
  {
    //! Set up the predefined shaders
    std::array<BasicNativeShaderCreateInfo, 3> g_predefindShaders = {
      // PredefinedShaderType::Vertex
      BasicNativeShaderCreateInfo(BasicShaderStageFlag::Vertex, NativeGraphicsDeviceShaders::GetVertexShader(),
                                  NativeGraphicsDeviceShaders::VertexShaderVertexDecl.AsReadOnlySpan()),
      // PredefinedShaderType::Fragment
      BasicNativeShaderCreateInfo(BasicShaderStageFlag::Fragment, NativeGraphicsDeviceShaders::GetFragmentShader(), {}),
      // PredefinedShaderType::FragmentSdf
      BasicNativeShaderCreateInfo(BasicShaderStageFlag::Fragment, NativeGraphicsDeviceShaders::GetSdfFragmentShader(), {})};


    void CheckIfMaterialDeclarationIsSupported(const BlendState blendState, const BasicMaterialVariableDeclarationSpan& materialDeclaration)
    {
      switch (blendState)
      {
      case BlendState::Additive:
      case BlendState::AlphaBlend:
      case BlendState::NonPremultiplied:
      case BlendState::Opaque:
        {
          // auto index = materialDeclaration.MaterialElementIndexOf(BasicMaterialVariableElementUsage::ModelViewProj, 0);
          // if (index < 0)
          //{
          //  throw NotSupportedException("materialDeclaration is not compatible");
          //}
          // if (materialDeclaration.At(index).Format != BasicMaterialVariableElementFormat::Matrix4x4)
          //{
          //  throw NotSupportedException("ModelViewProj format is unsupported");
          //}
          break;
        }
      case BlendState::Sdf:
        {
          // auto index = materialDeclaration.MaterialElementIndexOf(BasicMaterialVariableElementUsage::ModelViewProj, 0);
          // if (index < 0)
          //{
          //  throw NotSupportedException("materialDeclaration is not compatible");
          //}
          // if (materialDeclaration.At(index).Format != BasicMaterialVariableElementFormat::Matrix4x4)
          //{
          //  throw NotSupportedException("ModelViewProj format is unsupported");
          //}
          auto index = materialDeclaration.MaterialElementIndexOf(BasicMaterialVariableElementUsage::SdfSmooth, 0);
          if (index < 0)
          {
            throw NotSupportedException("materialDeclaration is not compatible");
          }
          if (materialDeclaration.At(index).Format != BasicMaterialVariableElementFormat::Float1)
          {
            throw NotSupportedException("SdfSmooth format is unsupported");
          }
          break;
        }
      default:
        throw NotSupportedException("Unsupported BlendState");
      }
    }
  }

  NativeGraphicsMaterialFactory::NativeGraphicsMaterialFactory()
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::Construct");
  }


  NativeGraphicsMaterialFactory::~NativeGraphicsMaterialFactory() noexcept
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::Destruct");
  }


  void NativeGraphicsMaterialFactory::CreateDependentResources(const PxExtent2D& screenExtentPx)
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::CreateDependentResources");
    try
    {
      m_dependentResources = DependentResources(screenExtentPx);
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("CreateDependentResources failed with: {}", ex.what());
      DestroyDependentResources();
      throw;
    }
  }


  void NativeGraphicsMaterialFactory::DestroyDependentResources()
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::DestroyDependentResources");
    FSLLOG3_WARNING_IF(!m_dependentResources.Materials.Empty(), "There are still {} materials active, force freeing them",
                       m_dependentResources.Materials.Count());
    m_dependentResources = {};
  }


  ReadOnlySpan<BasicNativeShaderCreateInfo> NativeGraphicsMaterialFactory::GetPredefinedShaders() const
  {
    return ReadOnlySpanUtil::AsSpan(g_predefindShaders);
  }


  BasicNativeShaderHandle NativeGraphicsMaterialFactory::CreateShader(const BasicNativeShaderCreateInfo& createInfo)
  {
    return m_resources.ShaderManager.Create(createInfo);
  }


  bool NativeGraphicsMaterialFactory::DestroyShader(const BasicNativeShaderHandle hShader) noexcept
  {
    return m_resources.ShaderManager.Destroy(hShader);
  }


  void NativeGraphicsMaterialFactory::CreateMaterials(Span<BasicNativeMaterialHandle> dstMaterialHandles,
                                                      ReadOnlySpan<BasicNativeMaterialCreateInfo> createInfoSpan)
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::CreatePipelines");

    if (!m_dependentResources.IsValid)
    {
      throw UsageErrorException("Cant CreateMaterials in the current state");
    }

    try
    {
      // Ensure that we have enough capacity
      m_dependentResources.Materials.Reserve(UncheckedNumericCast<uint32_t>(m_dependentResources.Materials.Count() + createInfoSpan.size()));

      for (uint32_t i = 0; i < createInfoSpan.size(); ++i)
      {
        assert(!dstMaterialHandles[i].IsValid());
        int32_t handle = m_dependentResources.Materials.Add(CreateMaterial(createInfoSpan[i]));
        dstMaterialHandles[i] = BasicNativeMaterialHandle(handle);
      }
    }
    catch (const std::exception&)
    {
      // Clear all temporarily written handles
      for (uint32_t i = 0; i < dstMaterialHandles.size(); ++i)
      {
        if (dstMaterialHandles[i].IsValid())
        {
          m_dependentResources.Materials.Remove(dstMaterialHandles[i].Value);
          dstMaterialHandles[i] = {};
        }
      }
      throw;
    }
  }

  bool NativeGraphicsMaterialFactory::DestroyMaterial(const BasicNativeMaterialHandle hMaterial)
  {
    FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::DestroyPipeline({})", hMaterial.Value);
    if (!m_dependentResources.IsValid)
    {
      FSLLOG3_ERROR("Can't destroy a material in this state");
      return false;
    }
    MaterialRecord* pMaterial = m_dependentResources.Materials.TryGet(hMaterial.Value);
    if (pMaterial == nullptr)
    {
      return false;
    }
    m_attribManager.ReleaseConfig(pMaterial->AttribLinkHandle);
    pMaterial->AttribLinkHandle = NativeMaterialAttribHandle::Invalid();
    m_resources.ProgramManager.Release(pMaterial->ProgramHandle);
    pMaterial->ProgramHandle = NativeProgramHandle::Invalid();
    return m_dependentResources.Materials.Remove(hMaterial.Value);
  }


  NativeGraphicsMaterialFactory::MaterialRecord NativeGraphicsMaterialFactory::CreateMaterial(const BasicNativeMaterialCreateInfo& createInfo)
  {
    assert(createInfo.IsValid());

    CheckIfMaterialDeclarationIsSupported(createInfo.MaterialInfo.Blend, createInfo.MaterialDeclaration);

    const NativeProgramHandle hProgram =
      m_resources.ProgramManager.Acquire(m_resources.ShaderManager, createInfo.VertexShaderHandle, createInfo.FragmentShaderHandle);
    try
    {
      const auto& programRecord = m_resources.ProgramManager.Get(hProgram);

      const NativeMaterialAttribHandle hAttribLink = m_attribManager.AcquireConfig(programRecord.LocVertexPosition, programRecord.LocVertexColor,
                                                                                   programRecord.LocVertexTextureCoord, createInfo.VertexDeclaration);
      try
      {
        return MaterialRecord(createInfo.MaterialInfo, hProgram, hAttribLink, programRecord.Program.Get(), programRecord.LocSmoothing);
      }
      catch (const std::exception&)
      {
        m_attribManager.ReleaseConfig(hAttribLink);
        throw;
      }
    }
    catch (const std::exception&)
    {
      m_resources.ProgramManager.Release(hProgram);
      throw;
    }
  }
}
