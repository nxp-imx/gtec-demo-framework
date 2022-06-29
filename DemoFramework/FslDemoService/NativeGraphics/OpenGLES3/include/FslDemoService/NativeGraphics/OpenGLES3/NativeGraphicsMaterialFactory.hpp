#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSMATERIALFACTORY_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSMATERIALFACTORY_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/ExtendedCameraInfo.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsMaterialAttribManager.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsProgramManager.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsShaderManager.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeMaterialAttribHandle.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeProgramHandle.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialHandle.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderHandle.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclaration.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLVertexAttribLink.hpp>
#include <cassert>
#include <memory>
#include <utility>

namespace Fsl
{
  struct BasicNativeShaderCreateInfo;
}

namespace Fsl::GLES3
{
  class NativeGraphicsTextureFactory;

  class NativeGraphicsMaterialFactory
  {
  public:
    struct MaterialRecord
    {
      BasicMaterialInfo MaterialInfo;
      NativeProgramHandle ProgramHandle;
      NativeMaterialAttribHandle AttribLinkHandle;
      GLuint CachedProgramHandle{GLValues::INVALID_HANDLE};
      GLint CachedLocSmoothing{GLValues::INVALID_LOCATION};

      MaterialRecord() = default;

      explicit MaterialRecord(const BasicMaterialInfo& materialInfo, const NativeProgramHandle programHandle,
                              const NativeMaterialAttribHandle attribLinkHandle, const GLuint cachedProgramHandle, const GLint cachedLocSmoothing)
        : MaterialInfo(materialInfo)
        , ProgramHandle(programHandle)
        , AttribLinkHandle(attribLinkHandle)
        , CachedProgramHandle(cachedProgramHandle)
        , CachedLocSmoothing(cachedLocSmoothing)
      {
      }

      bool IsValid() const
      {
        return ProgramHandle.IsValid();
      }
    };

  private:
    struct Resources
    {
      NativeGraphicsShaderManager ShaderManager;
      NativeGraphicsProgramManager ProgramManager;
    };

    struct DependentResources
    {
      bool IsValid{false};
      PxExtent2D ScreenExtentPx;

      HandleVector<MaterialRecord> Materials;

      DependentResources() = default;

      explicit DependentResources(const PxExtent2D& screenExtentPx)
        : IsValid(true)
        , ScreenExtentPx(screenExtentPx)
      {
      }
    };

    struct PushConstantRecord
    {
      Matrix ModelViewProj;
      // 0.25f / (spread * scale)
      float Smoothing{0.0f};

      constexpr PushConstantRecord() noexcept = default;
      constexpr PushConstantRecord(const Matrix& modelViewProj, const float smoothing) noexcept
        : ModelViewProj(modelViewProj)
        , Smoothing(smoothing)
      {
      }
    };

    NativeGraphicsMaterialAttribManager m_attribManager;
    Resources m_resources;
    DependentResources m_dependentResources;

  public:
    NativeGraphicsMaterialFactory();
    ~NativeGraphicsMaterialFactory() noexcept;

    void CreateDependentResources(const PxExtent2D& screenExtentPx);
    void DestroyDependentResources();


    ReadOnlySpan<BasicNativeShaderCreateInfo> GetPredefinedShaders() const;
    BasicNativeShaderHandle CreateShader(const BasicNativeShaderCreateInfo& createInfo);
    bool DestroyShader(const BasicNativeShaderHandle hShader) noexcept;

    void CreateMaterials(Span<BasicNativeMaterialHandle> dstMaterialHandles, ReadOnlySpan<BasicNativeMaterialCreateInfo> createInfoSpan);
    bool DestroyMaterial(const BasicNativeMaterialHandle hMaterial);


    void ClearCachedCameraChangeIds() noexcept
    {
      m_resources.ProgramManager.ClearCachedCameraChangeIds();
    }

    inline void SetMaterialCameraInfo(const BasicNativeMaterialHandle hMaterial, const uint32_t cameraChangeId, const ExtendedCameraInfo& cameraInfo)
    {
      assert(hMaterial.IsValid());
      const NativeGraphicsMaterialFactory::MaterialRecord& materialRecord = GetMaterial(hMaterial);
      SetMaterialCameraInfo(materialRecord, cameraChangeId, cameraInfo);
    }

    inline void SetMaterialCameraInfo(const NativeGraphicsMaterialFactory::MaterialRecord& materialRecord, const uint32_t cameraChangeId,
                                      const ExtendedCameraInfo& cameraInfo)
    {
      m_resources.ProgramManager.SetMaterialCameraInfo(materialRecord.ProgramHandle, cameraChangeId, cameraInfo);
    }

    inline const VertexElementAttribLinks& GetVertexElementAttribLinks(const NativeGraphicsMaterialFactory::MaterialRecord& materialRecord) const
    {
      assert(materialRecord.IsValid());
      return m_attribManager.GetVertexElementAttribLinks(materialRecord.AttribLinkHandle);
    }


    const MaterialRecord& GetMaterial(const BasicNativeMaterialHandle hMaterial) const
    {
      return m_dependentResources.Materials.Get(hMaterial.Value);
    }

    const MaterialRecord* TryGetMaterial(const BasicNativeMaterialHandle hMaterial) const
    {
      return m_dependentResources.Materials.TryGet(hMaterial.Value);
    }

  private:
    MaterialRecord CreateMaterial(const BasicNativeMaterialCreateInfo& createInfo);
  };
}

#endif
