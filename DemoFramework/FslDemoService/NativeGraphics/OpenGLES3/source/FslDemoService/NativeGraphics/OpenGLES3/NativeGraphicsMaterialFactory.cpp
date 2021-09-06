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

#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsMaterialFactory.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <utility>

namespace Fsl
{
  namespace GLES3
  {
    namespace
    {
      constexpr const char* const g_vertexShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform mat4 MatModelViewProj;\n"
        "\n"
        "layout(location = 0) in vec4 VertexPosition;\n"
        "layout(location = 1) in vec4 VertexColor;\n"
        "layout(location = 2) in vec2 VertexTextureCoord;\n"
        "\n"
        "out vec4 FragColor;\n"
        "out vec2 FragTextureCoord;\n"
        "\n"
        "void main()\n"
        "{"
        "  gl_Position = MatModelViewProj * VertexPosition;\n"
        "  FragColor = VertexColor;\n"
        "  FragTextureCoord = VertexTextureCoord;\n"
        "}";


      constexpr const char* const g_fragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "\n"
        "uniform sampler2D Texture;\n"
        "\n"
        "in vec4 FragColor;\n"
        "in vec2 FragTextureCoord;\n"
        "\n"
        "out vec4 o_fragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "  o_fragColor = texture(Texture,FragTextureCoord) * FragColor;\n"
        "}\n";


      constexpr const char* const g_fragmentSdfShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "\n"
        "uniform sampler2D Texture;\n"
        "uniform float Smoothing;\n"
        "\n"
        "in vec4 FragColor;\n"
        "in vec2 FragTextureCoord;\n"
        "\n"
        "out vec4 o_fragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "  float distance = texture(Texture, FragTextureCoord).a;\n"
        "  float alpha = smoothstep(0.5 - Smoothing, 0.5 + Smoothing, distance);\n"
        "  o_fragColor = vec4(FragColor.rgb, FragColor.a * alpha);\n"
        "}\n";


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
      : m_resources(PreallocatePrograms())
    {
      FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::Construct");
    }


    NativeGraphicsMaterialFactory::~NativeGraphicsMaterialFactory() noexcept
    {
      FSLLOG3_VERBOSE3("NativeGraphicsPipelineFactory::Destruct");
    }


    void NativeGraphicsMaterialFactory::ClearCachedCameraChangeIds()
    {
      for (auto& rEntry : m_resources.Programs)
      {
        rEntry.CameraChangeId = 0;
      }
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
      return m_dependentResources.Materials.Remove(hMaterial.Value);
    }


    NativeGraphicsMaterialFactory::MaterialRecord NativeGraphicsMaterialFactory::CreateMaterial(const BasicNativeMaterialCreateInfo& createInfo)
    {
      assert(createInfo.IsValid());

      CheckIfMaterialDeclarationIsSupported(createInfo.MaterialInfo.Blend, createInfo.MaterialDeclaration);

      const uint32_t programIndex = FindMatchingProgramIndex(createInfo);
      const ProgramRecord& programRecord = m_resources.Programs[programIndex];
      assert(programRecord.IsValid());

      const NativeMaterialAttribHandle hAttribLink = m_attribManager.AcquireConfig(programRecord.LocVertexPosition, programRecord.LocVertexColor,
                                                                                   programRecord.LocVertexTextureCoord, createInfo.VertexDeclaration);


      return MaterialRecord(createInfo.MaterialInfo, programIndex, programRecord.Program.Get(), programRecord.LocMatModelViewProj,
                            programRecord.LocTexture, programRecord.LocSmoothing, hAttribLink);
    }


    uint32_t NativeGraphicsMaterialFactory::FindMatchingProgramIndex(const BasicNativeMaterialCreateInfo& createInfo)
    {
      switch (createInfo.MaterialInfo.Blend)
      {
      case BlendState::Additive:
      case BlendState::AlphaBlend:
      case BlendState::NonPremultiplied:
      case BlendState::Opaque:
        return 0;
      case BlendState::Sdf:
        return 1;
      default:
        throw NotSupportedException("Unsupported blend state");
      }
    }


    std::array<NativeGraphicsMaterialFactory::ProgramRecord, 2> NativeGraphicsMaterialFactory::PreallocatePrograms()
    {
      std::array<NativeGraphicsMaterialFactory::ProgramRecord, 2> programs{};

      // Normal program
      programs[0].Program.Reset(g_vertexShader, g_fragmentShader);
      programs[0].LocMatModelViewProj = programs[0].Program.GetUniformLocation("MatModelViewProj");
      programs[0].LocTexture = programs[0].Program.GetUniformLocation("Texture");
      // programs[0].LocSmoothing;
      programs[0].LocVertexPosition = programs[0].Program.GetAttribLocation("VertexPosition");
      programs[0].LocVertexColor = programs[0].Program.GetAttribLocation("VertexColor");
      programs[0].LocVertexTextureCoord = programs[0].Program.GetAttribLocation("VertexTextureCoord");

      // SDF program
      programs[1].Program.Reset(g_vertexShader, g_fragmentSdfShader);
      programs[1].LocMatModelViewProj = programs[1].Program.GetUniformLocation("MatModelViewProj");
      programs[1].LocTexture = programs[1].Program.GetUniformLocation("Texture");
      programs[1].LocSmoothing = programs[1].Program.GetUniformLocation("Smoothing");
      programs[1].LocVertexPosition = programs[1].Program.GetAttribLocation("VertexPosition");
      programs[1].LocVertexColor = programs[1].Program.GetAttribLocation("VertexColor");
      programs[1].LocVertexTextureCoord = programs[1].Program.GetAttribLocation("VertexTextureCoord");

      // Update the texture
      for (const auto& record : programs)
      {
        if (record.LocTexture != GLValues::INVALID_LOCATION)
        {
          glUseProgram(record.Program.Get());
          glUniform1i(record.LocTexture, 0);
        }
      }
      glUseProgram(0);
      return programs;
    }
  }
}
