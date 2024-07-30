#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSPROGRAMMANAGER_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSPROGRAMMANAGER_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/ExtendedCameraInfo.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeProgramHandle.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeShaderComboKey.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderHandle.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <map>
#include <utility>
#include <vector>

namespace Fsl::GLES3
{
  class NativeGraphicsShaderManager;

  class NativeGraphicsProgramManager
  {
    struct Record
    {
      GLProgram Program;
      // Uniform locations
      GLint LocMatModelViewProj{GLValues::InvalidLocation};
      GLint LocTexture{GLValues::InvalidLocation};
      GLint LocSmoothing{GLValues::InvalidLocation};
      // Attrib locations
      GLint LocVertexPosition{GLValues::InvalidLocation};
      GLint LocVertexColor{GLValues::InvalidLocation};
      GLint LocVertexTextureCoord{GLValues::InvalidLocation};

      uint32_t ReferenceCount{0};
      uint32_t CameraChangeId{0};

      NativeShaderComboKey ComboKey;

      Record() = default;

      explicit Record(GLProgram program, const GLint locMatModelViewProj, const GLint locTexture, const GLint locSmoothing,
                      const GLint locVertexPosition, const GLint locVertexColor, const GLint locVertexTextureCoord,
                      const NativeShaderComboKey comboKey)
        : Program(std::move(program))
        , LocMatModelViewProj(locMatModelViewProj)
        , LocTexture(locTexture)
        , LocSmoothing(locSmoothing)
        , LocVertexPosition(locVertexPosition)
        , LocVertexColor(locVertexColor)
        , LocVertexTextureCoord(locVertexTextureCoord)
        , ReferenceCount(1)
        , ComboKey(comboKey)
      {
      }
    };

  private:
    HandleVector<Record> m_records;
    std::map<NativeShaderComboKey, NativeProgramHandle> m_programDict;
    std::vector<GLBindAttribLocation> m_bindAttribScratchpad;

  public:
    NativeGraphicsProgramManager();

    NativeProgramHandle Acquire(NativeGraphicsShaderManager& rShaderManager, const BasicNativeShaderHandle hVertexShader,
                                const BasicNativeShaderHandle hFragmentShader);
    bool Release(const NativeProgramHandle handle) noexcept;


    void ClearCachedCameraChangeIds() noexcept
    {
      for (HandleVector<Record>::index_type i = 0; i < m_records.Count(); ++i)
      {
        m_records[i].CameraChangeId = 0;
      }
    }

    inline void SetMaterialCameraInfo(const NativeProgramHandle hProgram, const uint32_t cameraChangeId, const ExtendedCameraInfo& cameraInfo)
    {
      Record& rRecord = m_records.Get(hProgram.Value);
      if (cameraChangeId != rRecord.CameraChangeId)
      {
        rRecord.CameraChangeId = cameraChangeId;
        glUniformMatrix4fv(rRecord.LocMatModelViewProj, 1, 0, static_cast<const GLfloat*>(cameraInfo.ModelViewProjection.DirectAccess()));
      }
    }

    const Record& Get(const NativeProgramHandle hProgram) const
    {
      return m_records.Get(hProgram.Value);
    }
  };
}

#endif
