/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include "MeshFurRender.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>

namespace Fsl
{
  namespace
  {
    IO::Path GetVert(const IO::Path& shaderPath, const bool useHighPrecision)
    {
      return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur_hp.vert.spv" : "Fur_lp.vert.spv"));
    }


    IO::Path GetFrag(const IO::Path& shaderPath, const bool useHighPrecision, const int lightCount)
    {
      switch (lightCount)
      {
      case 1:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur1_hp.frag.spv" : "Fur1_lp.frag.spv"));
      case 2:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur2_hp.frag.spv" : "Fur2_lp.frag.spv"));
      case 3:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur3_hp.frag.spv" : "Fur3_lp.frag.spv"));
      case 4:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "Fur4_hp.frag.spv" : "Fur4_lp.frag.spv"));
      default:
        return IO::Path::Combine(shaderPath, (useHighPrecision ? "FurN_hp.frag.spv" : "FurN_lp.frag.spv"));
      }
    }


    IO::Path GetVertAndLog(const IO::Path& shaderPath, const bool useHighPrecision)
    {
      auto path = GetVert(shaderPath, useHighPrecision);
      FSLLOG3_INFO("Vert shader: '{}' HighPrecision: {}", path, useHighPrecision);
      return path;
    }


    IO::Path GetFragAndLog(const IO::Path& shaderPath, const bool useHighPrecision, const int lightCount)
    {
      auto path = GetFrag(shaderPath, useHighPrecision, lightCount);
      FSLLOG3_INFO("Frag shader: '{}' HighPrecision: {} LightCount: {}", path, useHighPrecision, lightCount);
      return path;
    }
  }


  MeshFurRender::MeshFurRender(const IContentManager& contentManager, const Vulkan::VUDevice& device, const IO::Path& shaderPath,
                               const bool useHighPrecision, const int lightCount)
  {
    // Done this way as old versions of RapidVulkan did not include a constructor that takes a vector
    VertShader.Reset(device.Get(), 0, contentManager.ReadBytes(GetVertAndLog(shaderPath, useHighPrecision)));
    FragShader.Reset(device.Get(), 0, contentManager.ReadBytes(GetFragAndLog(shaderPath, useHighPrecision, lightCount)));
  }

  MeshFurRender::MeshFurRender(const IContentManager& contentManager, const Vulkan::VUDevice& device, const IO::Path& vertShaderPath,
                               const IO::Path& fragShaderPath, const int /*lightCount*/)
  {
    // Done this way as old versions of RapidVulkan did not include a constructor that takes a vector
    VertShader.Reset(device.Get(), 0, contentManager.ReadBytes(vertShaderPath));
    FragShader.Reset(device.Get(), 0, contentManager.ReadBytes(fragShaderPath));
  }
}
