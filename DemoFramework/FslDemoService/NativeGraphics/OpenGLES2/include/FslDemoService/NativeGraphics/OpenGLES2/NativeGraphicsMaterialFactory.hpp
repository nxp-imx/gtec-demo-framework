#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSMATERIALFACTORY_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSMATERIALFACTORY_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialHandle.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclaration.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/ExtendedCameraInfo.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsMaterialAttribManager.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeMaterialAttribHandle.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <FslUtil/OpenGLES2/GLVertexAttribLink.hpp>
#include <array>
#include <cassert>
#include <memory>
#include <utility>

namespace Fsl
{
  namespace GLES2
  {
    class NativeGraphicsTextureFactory;

    class NativeGraphicsMaterialFactory
    {
      struct ProgramRecord
      {
        uint32_t CameraChangeId{0};
        GLProgram Program;
        // Uniform locations
        GLint LocMatModelViewProj{GLValues::INVALID_LOCATION};
        GLint LocTexture{GLValues::INVALID_LOCATION};
        GLint LocSmoothing{GLValues::INVALID_LOCATION};
        // Attrib locations
        GLint LocVertexPosition{GLValues::INVALID_LOCATION};
        GLint LocVertexColor{GLValues::INVALID_LOCATION};
        GLint LocVertexTextureCoord{GLValues::INVALID_LOCATION};

        ProgramRecord() = default;
        explicit ProgramRecord(GLProgram program, const GLint locMatModelViewProj, const GLint locTexture, const GLint locSmoothing,
                               const GLint locVertexPosition, const GLint locVertexColor, const GLint locVertexTextureCoord)
          : Program(std::move(program))
          , LocMatModelViewProj(locMatModelViewProj)
          , LocTexture(locTexture)
          , LocSmoothing(locSmoothing)
          , LocVertexPosition(locVertexPosition)
          , LocVertexColor(locVertexColor)
          , LocVertexTextureCoord(locVertexTextureCoord)
        {
        }

        bool IsValid() const
        {
          return Program.IsValid();
        }
      };

    public:
      struct MaterialRecord
      {
        BasicMaterialInfo MaterialInfo;
        uint32_t ProgramIndex{0};
        GLuint ProgramHandle{GLValues::INVALID_HANDLE};
        GLint LocMatModelViewProj{GLValues::INVALID_LOCATION};
        GLint LocTexture{GLValues::INVALID_LOCATION};
        GLint LocSmoothing{GLValues::INVALID_LOCATION};
        NativeMaterialAttribHandle AttribLinkHandle;

        MaterialRecord() = default;

        explicit MaterialRecord(const BasicMaterialInfo& materialInfo, const uint32_t programIndex, const GLuint programHandle,
                                const GLint locMatModelViewProj, const GLint locTexture, const GLint locSmoothing,
                                const NativeMaterialAttribHandle attribLinkHandle)
          : MaterialInfo(materialInfo)
          , ProgramIndex(programIndex)
          , ProgramHandle(programHandle)
          , LocMatModelViewProj(locMatModelViewProj)
          , LocTexture(locTexture)
          , LocSmoothing(locSmoothing)
          , AttribLinkHandle(attribLinkHandle)
        {
        }

        bool IsValid() const
        {
          return ProgramHandle != GLValues::INVALID_HANDLE;
        }
      };

    private:
      struct Resources
      {
        std::array<ProgramRecord, 2> Programs;

        explicit Resources(std::array<ProgramRecord, 2> programs)
          : Programs(std::move(programs))
        {
        }
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

      void CreateMaterials(Span<BasicNativeMaterialHandle> dstMaterialHandles, ReadOnlySpan<BasicNativeMaterialCreateInfo> createInfoSpan);
      bool DestroyMaterial(const BasicNativeMaterialHandle hMaterial);


      void ClearCachedCameraChangeIds();

      inline void SetMaterialCameraInfo(const BasicNativeMaterialHandle hMaterial, const uint32_t cameraChangeId,
                                        const ExtendedCameraInfo& cameraInfo)
      {
        assert(hMaterial.IsValid());
        const NativeGraphicsMaterialFactory::MaterialRecord& materialRecord = GetMaterial(hMaterial);
        SetMaterialCameraInfo(materialRecord, cameraChangeId, cameraInfo);
      }

      inline void SetMaterialCameraInfo(const NativeGraphicsMaterialFactory::MaterialRecord& materialRecord, const uint32_t cameraChangeId,
                                        const ExtendedCameraInfo& cameraInfo)
      {
        if (cameraChangeId != m_resources.Programs[materialRecord.ProgramIndex].CameraChangeId)
        {
          m_resources.Programs[materialRecord.ProgramIndex].CameraChangeId = cameraChangeId;
          glUniformMatrix4fv(materialRecord.LocMatModelViewProj, 1, 0, static_cast<const GLfloat*>(cameraInfo.ModelViewProjection.DirectAccess()));
        }
      }

      inline void SetAttribArrays(const NativeGraphicsMaterialFactory::MaterialRecord& materialRecord)
      {
        assert(materialRecord.IsValid());
        m_attribManager.ApplyAttribArrays(materialRecord.AttribLinkHandle);
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
      static uint32_t FindMatchingProgramIndex(const BasicNativeMaterialCreateInfo& createInfo);
      static std::array<ProgramRecord, 2> PreallocatePrograms();
    };
  }
}

#endif
